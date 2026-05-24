CONF_ZIGBEE_ID = "zigbee_id"
CONF_ZIGBEE_BINARY_SENSOR = "zigbee_binary_sensor"
CONF_ZIGBEE_SENSOR = "zigbee_sensor"
CONF_ZIGBEE_SWITCH = "zigbee_switch"
CONF_ZIGBEE_NUMBER = "zigbee_number"
CONF_SLEEPY = "sleepy"
CONF_IEEE802154_VENDOR_OUI = "ieee802154_vendor_oui"

# Keys for CORE.data storage
# Each entry: (ZigbeeClusterDesc, report_attr_count: int, app_device_id: str)
KEY_PENDING_CLUSTERS = "pending_clusters"
# Dict mapping cluster_base (int) → instance count, used to assign unique FC-range IDs
KEY_TYPE_COUNTERS = "type_counters"

# External ZBOSS SDK types (just strings for codegen)
ZB_ZCL_BASIC_ATTRS_EXT_T = "zb_zcl_basic_attrs_ext_t"
ZB_ZCL_IDENTIFY_ATTRS_T = "zb_zcl_identify_attrs_t"

# Cluster IDs — always needed for Basic and Identify
ZB_ZCL_CLUSTER_ID_BASIC = "ZB_ZCL_CLUSTER_ID_BASIC"
ZB_ZCL_CLUSTER_ID_IDENTIFY = "ZB_ZCL_CLUSTER_ID_IDENTIFY"

# Manufacturer-specific cluster ID ranges (0xFC00–0xFFFF per ZCL spec).
# Each component type occupies 16 slots, one per instance.
#
#   0xFC00–0xFC0F  binary_sensor  (16 instances)
#   0xFC10–0xFC1F  sensor         (16 instances)
#   0xFC20–0xFC2F  switch         (16 instances)
#   0xFC30–0xFC3F  number         (16 instances)
#
# Instance index within each range is assigned at codegen time by
# KEY_TYPE_COUNTERS.
CLUSTER_BASE_BINARY_SENSOR = 0xFC00
CLUSTER_BASE_SENSOR        = 0xFC10
CLUSTER_BASE_SWITCH        = 0xFC20
CLUSTER_BASE_NUMBER        = 0xFC30
CLUSTER_RANGE_SIZE         = 0x10
