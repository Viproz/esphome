#include "zigbee_binary_sensor_zephyr.h"
#if defined(USE_ZIGBEE) && defined(USE_NRF52) && defined(USE_BINARY_SENSOR)
#include "esphome/core/log.h"
extern "C" {
#include <zboss_api.h>
#include <zboss_api_addons.h>
#include <zb_nrf_platform.h>
#include <zigbee/zigbee_app_utils.h>
#include <zb_error_to_string.h>
}
namespace esphome::zigbee {

static const char *const TAG = "zigbee.binary_sensor";

ZigbeeBinarySensor::ZigbeeBinarySensor(binary_sensor::BinarySensor *binary_sensor) : binary_sensor_(binary_sensor) {}

void ZigbeeBinarySensor::setup() {
  // Register cluster handlers for this manufacturer-specific cluster ID.
  // ⚠ Called after ZB_AF_REGISTER_DEVICE_CTX — believed safe before network join,
  // but not verified against ZBOSS internals.
  zb_zcl_add_cluster_handlers(this->cluster_id_, ZB_ZCL_CLUSTER_SERVER_ROLE,
                              (zb_zcl_cluster_check_value_t) NULL,
                              (zb_zcl_cluster_write_attr_hook_t) NULL,
                              (zb_zcl_cluster_handler_t) NULL);

  this->binary_sensor_->add_on_state_callback([this](bool state) {
    this->cluster_attributes_->present_value = state ? ZB_TRUE : ZB_FALSE;
    ESP_LOGD(TAG, "Set attribute endpoint: %d cluster: 0x%04X, present_value %d",
             this->endpoint_, this->cluster_id_, this->cluster_attributes_->present_value);
    ZB_ZCL_SET_ATTRIBUTE(this->endpoint_, this->cluster_id_, ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID, &this->cluster_attributes_->present_value,
                         ZB_FALSE);
    this->parent_->force_report();
  });
}

void ZigbeeBinarySensor::dump_config() {
  ESP_LOGCONFIG(TAG,
                "Zigbee Binary Sensor\n"
                "  Endpoint: %d, present_value %u",
                this->endpoint_, this->cluster_attributes_->present_value);
}

}  // namespace esphome::zigbee
#endif
