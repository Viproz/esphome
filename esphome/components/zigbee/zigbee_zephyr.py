import datetime
import random

import esphome.codegen as cg
from esphome.components.zephyr import zephyr_add_prj_conf
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_MODEL,
    CONF_NAME,
    CONF_UNIT_OF_MEASUREMENT,
    __version__,
)
from esphome.core import CORE, CoroPriority, coroutine_with_priority
from esphome.cpp_generator import (
    AssignmentExpression,
    MockObj,
    VariableDeclarationExpression,
)
from esphome.types import ConfigType

from .const import (
    BACNET_UNIT_NO_UNITS,
    BACNET_UNITS,
    CONF_POWER_SOURCE,
    CONF_ROUTER,
    CONF_WIPE_ON_BOOT,
    KEY_ZIGBEE,
    POWER_SOURCE,
    AnalogAttrs,
    AnalogAttrsOutput,
    BinaryAttrs,
    ZigbeeComponent,
    zigbee_ns,
)
from .const_zephyr import (
    CLUSTER_BASE_BINARY_SENSOR,
    CLUSTER_BASE_NUMBER,
    CLUSTER_BASE_SENSOR,
    CLUSTER_BASE_SWITCH,
    CLUSTER_RANGE_SIZE,
    CONF_IEEE802154_VENDOR_OUI,
    CONF_SLEEPY,
    CONF_ZIGBEE_BINARY_SENSOR,
    CONF_ZIGBEE_ID,
    CONF_ZIGBEE_NUMBER,
    CONF_ZIGBEE_SENSOR,
    CONF_ZIGBEE_SWITCH,
    KEY_PENDING_CLUSTERS,
    KEY_TYPE_COUNTERS,
    ZB_ZCL_BASIC_ATTRS_EXT_T,
    ZB_ZCL_CLUSTER_ID_BASIC,
    ZB_ZCL_CLUSTER_ID_IDENTIFY,
    ZB_ZCL_IDENTIFY_ATTRS_T,
)

ZigbeeBinarySensor = zigbee_ns.class_("ZigbeeBinarySensor", cg.Component)
ZigbeeSensor = zigbee_ns.class_("ZigbeeSensor", cg.Component)
ZigbeeSwitch = zigbee_ns.class_("ZigbeeSwitch", cg.Component)
ZigbeeNumber = zigbee_ns.class_("ZigbeeNumber", cg.Component)

zephyr_binary_sensor = cv.Schema(
    {
        cv.OnlyWith(CONF_ZIGBEE_ID, ["nrf52", "zigbee"]): cv.use_id(ZigbeeComponent),
        cv.OnlyWith(CONF_ZIGBEE_BINARY_SENSOR, ["nrf52", "zigbee"]): cv.declare_id(
            ZigbeeBinarySensor
        ),
    }
)

zephyr_sensor = cv.Schema(
    {
        cv.OnlyWith(CONF_ZIGBEE_ID, ["nrf52", "zigbee"]): cv.use_id(ZigbeeComponent),
        cv.OnlyWith(CONF_ZIGBEE_SENSOR, ["nrf52", "zigbee"]): cv.declare_id(
            ZigbeeSensor
        ),
    }
)

zephyr_switch = cv.Schema(
    {
        cv.OnlyWith(CONF_ZIGBEE_ID, ["nrf52", "zigbee"]): cv.use_id(ZigbeeComponent),
        cv.OnlyWith(CONF_ZIGBEE_SWITCH, ["nrf52", "zigbee"]): cv.declare_id(
            ZigbeeSwitch
        ),
    }
)

zephyr_number = cv.Schema(
    {
        cv.OnlyWith(CONF_ZIGBEE_ID, ["nrf52", "zigbee"]): cv.use_id(ZigbeeComponent),
        cv.OnlyWith(CONF_ZIGBEE_NUMBER, ["nrf52", "zigbee"]): cv.declare_id(
            ZigbeeNumber
        ),
    }
)


async def zephyr_to_code(config: ConfigType) -> "MockObj":
    zephyr_add_prj_conf("ZIGBEE", True)
    zephyr_add_prj_conf("ZIGBEE_APP_UTILS", True)
    if config[CONF_ROUTER]:
        zephyr_add_prj_conf("ZIGBEE_ROLE_ROUTER", True)
    else:
        zephyr_add_prj_conf("ZIGBEE_ROLE_END_DEVICE", True)

    zephyr_add_prj_conf("ZIGBEE_CHANNEL_SELECTION_MODE_MULTI", True)

    zephyr_add_prj_conf("CRYPTO", True)

    zephyr_add_prj_conf("NET_IPV6", False)
    zephyr_add_prj_conf("NET_IP_ADDR_CHECK", False)
    zephyr_add_prj_conf("NET_UDP", False)

    # disable all extra to reduce power and save flash
    zephyr_add_prj_conf("ZIGBEE_HAVE_SERIAL", False)
    zephyr_add_prj_conf("ZBOSS_ERROR_PRINT_TO_LOG", False)
    zephyr_add_prj_conf("DK_LIBRARY", False)

    cg.add_build_flag("-Wl,--wrap=zb_zcl_put_reporting_info_from_req")

    if CONF_IEEE802154_VENDOR_OUI in config:
        zephyr_add_prj_conf("IEEE802154_VENDOR_OUI_ENABLE", True)
        random_number = config[CONF_IEEE802154_VENDOR_OUI]
        if random_number == "random":
            random_number = random.randint(0x000000, 0xFFFFFF)
        zephyr_add_prj_conf("IEEE802154_VENDOR_OUI", random_number)

    if config[CONF_WIPE_ON_BOOT]:
        if config[CONF_WIPE_ON_BOOT] == "once":
            cg.add_define(
                "USE_ZIGBEE_WIPE_ON_BOOT_MAGIC", random.randint(0x000001, 0xFFFFFF)
            )
        cg.add_define("USE_ZIGBEE_WIPE_ON_BOOT")

    # Generate attribute lists before any await that could yield (e.g., build_automation
    # waiting for variables from other components). If the hub's priority decays while
    # yielding, deferred entity jobs may add cluster list globals that reference these
    # attribute lists before they're declared.
    await _attr_to_code(config)

    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)

    CORE.add_job(_ctx_to_code, config)

    cg.add(var.set_sleepy(config[CONF_SLEEPY]))

    return var


async def _attr_to_code(config: ConfigType) -> None:
    # Create the basic attributes structure and attribute list
    basic_attrs = zigbee_new_variable("zigbee_basic_attrs", ZB_ZCL_BASIC_ATTRS_EXT_T)
    zigbee_new_attr_list(
        "zigbee_basic_attrib_list",
        "ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT",
        zigbee_assign(basic_attrs.zcl_version, cg.RawExpression("ZB_ZCL_VERSION")),
        zigbee_assign(basic_attrs.app_version, 0),
        zigbee_assign(basic_attrs.stack_version, 0),
        zigbee_assign(basic_attrs.hw_version, 0),
        zigbee_set_string(basic_attrs.mf_name, "esphome"),
        zigbee_set_string(basic_attrs.model_id, config[CONF_MODEL]),
        zigbee_set_string(
            basic_attrs.date_code, datetime.datetime.now().strftime("%Y%m%d %H%M%S")
        ),
        zigbee_assign(
            basic_attrs.power_source,
            cg.RawExpression(POWER_SOURCE[config[CONF_POWER_SOURCE]]),
        ),
        zigbee_set_string(basic_attrs.location_id, ""),
        zigbee_assign(
            basic_attrs.ph_env, cg.RawExpression("ZB_ZCL_BASIC_ENV_UNSPECIFIED")
        ),
        zigbee_set_string(basic_attrs.sw_ver, __version__),
    )

    # Create the identify attributes structure and attribute list
    identify_attrs = zigbee_new_variable(
        "zigbee_identify_attrs", ZB_ZCL_IDENTIFY_ATTRS_T
    )
    zigbee_new_attr_list(
        "zigbee_identify_attrib_list",
        "ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST",
        zigbee_assign(
            identify_attrs.identify_time,
            cg.RawExpression("ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE"),
        ),
    )


def zigbee_new_variable(name: str, type_: str) -> cg.MockObj:
    """Create a global variable with the given name and type."""
    decl = VariableDeclarationExpression(type_, "", name)
    CORE.add_global(decl)
    return MockObj(name, ".")


def zigbee_assign(target: cg.MockObj, expression: cg.RawExpression | int) -> str:
    """Assign an expression to a target and return a reference to it."""
    cg.add(AssignmentExpression("", "", target, expression))
    return f"&{target}"


def zigbee_set_string(target: cg.MockObj, value: str) -> str:
    """Set a ZCL string value and return the target name (arrays decay to pointers)."""
    # Zigbee supports only ASCII
    value = value.encode("ascii", "ignore").decode()
    cg.add(
        cg.RawExpression(
            f"ZB_ZCL_SET_STRING_VAL({target}, {cg.safe_exp(value)}, ZB_ZCL_STRING_CONST_SIZE({cg.safe_exp(value)}))"
        )
    )
    return str(target)


def zigbee_new_attr_list(name: str, macro: str, *args: str) -> str:
    """Create an attribute list using a ZBOSS macro and return the name."""
    obj = cg.RawExpression(f"{macro}({name}, {', '.join(args)})")
    CORE.add_global(obj)
    return name


class ZigbeeClusterDesc:
    """Represents a Zigbee cluster descriptor for code generation."""

    def __init__(self, cluster_id: str | int, attr_list_name: str | None = None) -> None:
        # cluster_id may be a hex string like "0xFC00" or an int like 0xFC00.
        # We store both: the int for the macro name, the string for ZB_ZCL_CLUSTER_DESC.
        if isinstance(cluster_id, int):
            self._cluster_id_int = cluster_id
            self._cluster_id_str = f"ESPHOME_ZB_CLUSTER_{cluster_id:04X}"
        else:
            # Standard named cluster like "ZB_ZCL_CLUSTER_ID_BASIC" — pass through as-is.
            self._cluster_id_int = None
            self._cluster_id_str = cluster_id
        self._attr_list_name = attr_list_name

    @property
    def cluster_id(self) -> str:
        return self._cluster_id_str

    @property
    def has_attrs(self) -> bool:
        return self._attr_list_name is not None

    def __str__(self) -> str:
        role = (
            "ZB_ZCL_CLUSTER_SERVER_ROLE"
            if self._attr_list_name
            else "ZB_ZCL_CLUSTER_CLIENT_ROLE"
        )
        if self._attr_list_name:
            attr_count = f"ZB_ZCL_ARRAY_SIZE({self._attr_list_name}, zb_zcl_attr_t)"
            return f"ZB_ZCL_CLUSTER_DESC({self._cluster_id_str}, {attr_count}, {self._attr_list_name}, {role}, ZB_ZCL_MANUF_CODE_INVALID)"
        return f"ZB_ZCL_CLUSTER_DESC({self._cluster_id_str}, 0, NULL, {role}, ZB_ZCL_MANUF_CODE_INVALID)"


def zigbee_new_cluster_list(
    name: str, clusters: list[ZigbeeClusterDesc]
) -> tuple[str, list[ZigbeeClusterDesc]]:
    """Create a cluster list array and return its name and the clusters."""
    # Always include basic and identify clusters first
    all_clusters = [
        ZigbeeClusterDesc(ZB_ZCL_CLUSTER_ID_BASIC, "zigbee_basic_attrib_list"),
        ZigbeeClusterDesc(ZB_ZCL_CLUSTER_ID_IDENTIFY, "zigbee_identify_attrib_list"),
    ]
    all_clusters.extend(clusters)

    cluster_strs = [str(c) for c in all_clusters]
    CORE.add_global(
        cg.RawExpression(
            f"zb_zcl_cluster_desc_t {name}[] = {{{', '.join(cluster_strs)}}}"
        )
    )
    return (name, all_clusters)


def alloc_cluster_id(cluster_base: int) -> int:
    """Allocate the next available cluster ID for a given component type.

    Each component type has a 16-slot range starting at cluster_base.
    Returns the numeric cluster ID (e.g. 0xFC01 for second binary sensor).
    Raises cv.Invalid if the range is exhausted.
    """
    data: dict = CORE.data.setdefault(KEY_ZIGBEE, {})
    counters: dict = data.setdefault(KEY_TYPE_COUNTERS, {})
    index = counters.get(cluster_base, 0)
    if index >= CLUSTER_RANGE_SIZE:
        raise cv.Invalid(
            f"Too many entities of the same type: maximum {CLUSTER_RANGE_SIZE} per type"
        )
    counters[cluster_base] = index + 1
    return cluster_base + index


def get_entity_index() -> int:
    """Return the total count of registered entities so far (for unique symbol naming)."""
    data: dict = CORE.data.setdefault(KEY_ZIGBEE, {})
    return len(data.get(KEY_PENDING_CLUSTERS, []))


def zigbee_add_pending_cluster(
    cluster_desc: "ZigbeeClusterDesc",
    report_attr_count: int,
    app_device_id: str,
) -> None:
    """Accumulate a cluster for the single shared endpoint.

    The cluster list and endpoint declaration are emitted in _ctx_to_code once
    all entities have registered their clusters.
    """
    data: dict = CORE.data.setdefault(KEY_ZIGBEE, {})
    pending: list = data.setdefault(KEY_PENDING_CLUSTERS, [])
    pending.append((cluster_desc, report_attr_count, app_device_id))


@coroutine_with_priority(CoroPriority.LATE)
async def _ctx_to_code(config: ConfigType) -> None:
    pending = CORE.data[KEY_ZIGBEE][KEY_PENDING_CLUSTERS]

    # Collect all entity clusters and sum their reportable attribute counts.
    # The app_device_id is taken from the first registered entity.
    entity_clusters = [entry[0] for entry in pending]
    total_report_attr_count = sum(entry[1] for entry in pending)
    app_device_id = pending[0][2]

    cluster_list_name, clusters = zigbee_new_cluster_list(
        "zigbee_ep1_cluster_list", entity_clusters
    )

    # Emit the single endpoint declaration
    ep_name = "zigbee_ep1"
    in_cluster_num = sum(1 for c in clusters if c.has_attrs)
    out_cluster_num = len(clusters) - in_cluster_num
    cluster_ids = [c.cluster_id for c in clusters]
    CORE.add_global(
        cg.RawExpression(
            f"ESPHOME_ZB_HA_DECLARE_EP({ep_name}, 1, {cluster_list_name}, "
            f"{in_cluster_num}, {out_cluster_num}, {total_report_attr_count}, "
            f"{app_device_id}, {', '.join(cluster_ids)})"
        )
    )

    # Device context: always a single endpoint
    cg.add_define("ZIGBEE_ENDPOINTS_COUNT", 1)
    cg.add_global(
        cg.RawExpression(
            f"ZBOSS_DECLARE_DEVICE_CTX_EP_VA(zb_device_ctx, &{ep_name})"
        )
    )
    cg.add(cg.RawExpression("ZB_AF_REGISTER_DEVICE_CTX(&zb_device_ctx)"))


async def zephyr_setup_binary_sensor(entity: cg.MockObj, config: ConfigType) -> None:
    CORE.add_job(_add_binary_sensor, entity, config)


async def zephyr_setup_sensor(entity: cg.MockObj, config: ConfigType) -> None:
    CORE.add_job(_add_sensor, entity, config)


async def zephyr_setup_switch(entity: cg.MockObj, config: ConfigType) -> None:
    CORE.add_job(_add_switch, entity, config)


async def zephyr_setup_number(
    entity: cg.MockObj,
    config: ConfigType,
    min_value: float,
    max_value: float,
    step: float,
) -> None:
    CORE.add_job(_add_number, entity, config, min_value, max_value, step)


async def _add_zigbee_ep(
    entity: cg.MockObj,
    config: ConfigType,
    component_key,
    attrs_type,
    zcl_macro: str,
    cluster_base: int,
    app_device_id: str,
    extra_field_values: dict[str, int] | None = None,
) -> None:
    # Allocate a unique FC-range cluster ID for this entity type+instance.
    cluster_id_int = alloc_cluster_id(cluster_base)
    cluster_id_hex = f"0x{cluster_id_int:04X}"

    # Use total entity count as a stable index for unique C symbol names.
    entity_index = get_entity_index()

    prefix = f"zigbee_ep1_e{entity_index}"
    attrs_name = f"{prefix}_attrs"
    attr_list_name = f"{prefix}_attrib_list"

    # Create attribute struct and list.  These must be emitted now (before any
    # coroutine yield) so they appear in the global scope before the cluster list
    # that references them, which is emitted later in _ctx_to_code.
    attrs = zigbee_new_variable(attrs_name, attrs_type)

    attr_args = [
        zigbee_assign(attrs.out_of_service, 0),
        zigbee_assign(attrs.present_value, 0),
        zigbee_assign(attrs.status_flags, 0),
    ]
    if extra_field_values:
        for field_name, value in extra_field_values.items():
            attr_args.append(zigbee_assign(getattr(attrs, field_name), value))
    attr_args.append(zigbee_set_string(attrs.description, config[CONF_NAME]))

    attr_list = zigbee_new_attr_list(attr_list_name, zcl_macro, *attr_args)

    # Accumulate cluster descriptor for the single shared endpoint.
    # The endpoint declaration itself is emitted in _ctx_to_code.
    zigbee_add_pending_cluster(
        ZigbeeClusterDesc(cluster_id_int, attr_list),
        2,  # report_attr_count: present_value + status_flags
        app_device_id,
    )

    # All entities share the single endpoint 1.
    var = cg.new_Pvariable(config[component_key], entity)
    await cg.register_component(var, {})

    cg.add(var.set_endpoint(1))
    cg.add(var.set_cluster_id(cluster_id_int))
    cg.add(var.set_cluster_attributes(attrs))

    hub = await cg.get_variable(config[CONF_ZIGBEE_ID])
    cg.add(var.set_parent(hub))


async def _add_binary_sensor(entity: cg.MockObj, config: ConfigType) -> None:
    await _add_zigbee_ep(
        entity,
        config,
        CONF_ZIGBEE_BINARY_SENSOR,
        BinaryAttrs,
        "ESPHOME_ZB_ZCL_DECLARE_BINARY_INPUT_ATTRIB_LIST",
        CLUSTER_BASE_BINARY_SENSOR,
        "ZB_HA_SIMPLE_SENSOR_DEVICE_ID",
    )


async def _add_sensor(entity: cg.MockObj, config: ConfigType) -> None:
    # Get BACnet engineering unit from unit_of_measurement
    unit = config.get(CONF_UNIT_OF_MEASUREMENT, "")
    bacnet_unit = BACNET_UNITS.get(unit, BACNET_UNIT_NO_UNITS)

    await _add_zigbee_ep(
        entity,
        config,
        CONF_ZIGBEE_SENSOR,
        AnalogAttrs,
        "ESPHOME_ZB_ZCL_DECLARE_ANALOG_INPUT_ATTRIB_LIST",
        CLUSTER_BASE_SENSOR,
        "ZB_HA_CUSTOM_ATTR_DEVICE_ID",
        extra_field_values={"engineering_units": bacnet_unit},
    )


async def _add_switch(entity: cg.MockObj, config: ConfigType) -> None:
    await _add_zigbee_ep(
        entity,
        config,
        CONF_ZIGBEE_SWITCH,
        BinaryAttrs,
        "ESPHOME_ZB_ZCL_DECLARE_BINARY_OUTPUT_ATTRIB_LIST",
        CLUSTER_BASE_SWITCH,
        "ZB_HA_CUSTOM_ATTR_DEVICE_ID",
    )


async def _add_number(
    entity: cg.MockObj,
    config: ConfigType,
    min_value: float,
    max_value: float,
    step: float,
) -> None:
    # Get BACnet engineering unit from unit_of_measurement
    unit = config.get(CONF_UNIT_OF_MEASUREMENT, "")
    bacnet_unit = BACNET_UNITS.get(unit, BACNET_UNIT_NO_UNITS)

    await _add_zigbee_ep(
        entity,
        config,
        CONF_ZIGBEE_NUMBER,
        AnalogAttrsOutput,
        "ESPHOME_ZB_ZCL_DECLARE_ANALOG_OUTPUT_ATTRIB_LIST",
        CLUSTER_BASE_NUMBER,
        "ZB_HA_CUSTOM_ATTR_DEVICE_ID",
        extra_field_values={
            "max_present_value": max_value,
            "min_present_value": min_value,
            "resolution": step,
            "engineering_units": bacnet_unit,
        },
    )
