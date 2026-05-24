#pragma once
#include "esphome/core/defines.h"
#if defined(USE_ZIGBEE) && defined(USE_NRF52)
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include <vector>
extern "C" {
#include <zboss_api.h>
#include <zboss_api_addons.h>
}

// copy of ZB_DECLARE_SIMPLE_DESC. Due to https://github.com/nrfconnect/sdk-nrfxlib/pull/666
#define ESPHOME_ZB_DECLARE_SIMPLE_DESC(ep_name, in_clusters_count, out_clusters_count) \
  typedef ZB_PACKED_PRE struct zb_af_simple_desc_##ep_name##_##in_clusters_count##_##out_clusters_count##_s { \
    zb_uint8_t endpoint;                  /* Endpoint */ \
    zb_uint16_t app_profile_id;           /* Application profile identifier */ \
    zb_uint16_t app_device_id;            /* Application device identifier */ \
    zb_bitfield_t app_device_version : 4; /* Application device version */ \
    zb_bitfield_t reserved : 4;           /* Reserved */ \
    zb_uint8_t app_input_cluster_count;   /* Application input cluster count */ \
    zb_uint8_t app_output_cluster_count;  /* Application output cluster count */ \
    /* Application input and output cluster list */ \
    zb_uint16_t app_cluster_list[(in_clusters_count) + (out_clusters_count)]; \
  } ZB_PACKED_STRUCT zb_af_simple_desc_##ep_name##_##in_clusters_count##_##out_clusters_count##_t

#define ESPHOME_CAT7(a, b, c, d, e, f, g) a##b##c##d##e##f##g
// needed to use ESPHOME_ZB_DECLARE_SIMPLE_DESC
#define ESPHOME_ZB_AF_SIMPLE_DESC_TYPE(ep_name, in_num, out_num) \
  ESPHOME_CAT7(zb_af_simple_desc_, ep_name, _, in_num, _, out_num, _t)

// needed to use ESPHOME_ZB_DECLARE_SIMPLE_DESC
#define ESPHOME_ZB_ZCL_DECLARE_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num, app_device_id, ...) \
  ESPHOME_ZB_DECLARE_SIMPLE_DESC(ep_name, in_clust_num, out_clust_num); \
  ESPHOME_ZB_AF_SIMPLE_DESC_TYPE(ep_name, in_clust_num, out_clust_num) \
  simple_desc_##ep_name = {ep_id, ZB_AF_HA_PROFILE_ID, app_device_id, 0, 0, in_clust_num, out_clust_num, {__VA_ARGS__}}

// needed to use ESPHOME_ZB_ZCL_DECLARE_SIMPLE_DESC
#define ESPHOME_ZB_HA_DECLARE_EP(ep_name, ep_id, cluster_list, in_cluster_num, out_cluster_num, report_attr_count, \
                                 app_device_id, ...) \
  ESPHOME_ZB_ZCL_DECLARE_SIMPLE_DESC(ep_name, ep_id, in_cluster_num, out_cluster_num, app_device_id, __VA_ARGS__); \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info##ep_name, report_attr_count); \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID, 0, NULL, \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list, \
                              (zb_af_simple_desc_1_1_t *) &simple_desc_##ep_name, report_attr_count, \
                              reporting_info##ep_name, 0, NULL)

// Named macros for all 64 manufacturer-specific cluster IDs.
// ZB_ZCL_CLUSTER_DESC token-pastes cluster_id##_SERVER_ROLE_INIT / ##_CLIENT_ROLE_INIT,
// so numeric literals cannot be used directly as cluster_id — named identifiers are required.
// We set both INIT hooks to NULL because handlers are registered at runtime via
// zb_zcl_add_cluster_handlers() in each entity's setup().
#define ESPHOME_ZB_CLUSTER_FC00 ((zb_uint16_t)0xFC00)
#define ESPHOME_ZB_CLUSTER_FC00_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC00_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC01 ((zb_uint16_t)0xFC01)
#define ESPHOME_ZB_CLUSTER_FC01_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC01_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC02 ((zb_uint16_t)0xFC02)
#define ESPHOME_ZB_CLUSTER_FC02_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC02_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC03 ((zb_uint16_t)0xFC03)
#define ESPHOME_ZB_CLUSTER_FC03_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC03_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC04 ((zb_uint16_t)0xFC04)
#define ESPHOME_ZB_CLUSTER_FC04_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC04_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC05 ((zb_uint16_t)0xFC05)
#define ESPHOME_ZB_CLUSTER_FC05_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC05_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC06 ((zb_uint16_t)0xFC06)
#define ESPHOME_ZB_CLUSTER_FC06_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC06_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC07 ((zb_uint16_t)0xFC07)
#define ESPHOME_ZB_CLUSTER_FC07_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC07_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC08 ((zb_uint16_t)0xFC08)
#define ESPHOME_ZB_CLUSTER_FC08_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC08_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC09 ((zb_uint16_t)0xFC09)
#define ESPHOME_ZB_CLUSTER_FC09_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC09_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0A ((zb_uint16_t)0xFC0A)
#define ESPHOME_ZB_CLUSTER_FC0A_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0A_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0B ((zb_uint16_t)0xFC0B)
#define ESPHOME_ZB_CLUSTER_FC0B_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0B_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0C ((zb_uint16_t)0xFC0C)
#define ESPHOME_ZB_CLUSTER_FC0C_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0C_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0D ((zb_uint16_t)0xFC0D)
#define ESPHOME_ZB_CLUSTER_FC0D_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0D_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0E ((zb_uint16_t)0xFC0E)
#define ESPHOME_ZB_CLUSTER_FC0E_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0E_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0F ((zb_uint16_t)0xFC0F)
#define ESPHOME_ZB_CLUSTER_FC0F_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC0F_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC10 ((zb_uint16_t)0xFC10)
#define ESPHOME_ZB_CLUSTER_FC10_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC10_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC11 ((zb_uint16_t)0xFC11)
#define ESPHOME_ZB_CLUSTER_FC11_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC11_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC12 ((zb_uint16_t)0xFC12)
#define ESPHOME_ZB_CLUSTER_FC12_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC12_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC13 ((zb_uint16_t)0xFC13)
#define ESPHOME_ZB_CLUSTER_FC13_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC13_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC14 ((zb_uint16_t)0xFC14)
#define ESPHOME_ZB_CLUSTER_FC14_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC14_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC15 ((zb_uint16_t)0xFC15)
#define ESPHOME_ZB_CLUSTER_FC15_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC15_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC16 ((zb_uint16_t)0xFC16)
#define ESPHOME_ZB_CLUSTER_FC16_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC16_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC17 ((zb_uint16_t)0xFC17)
#define ESPHOME_ZB_CLUSTER_FC17_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC17_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC18 ((zb_uint16_t)0xFC18)
#define ESPHOME_ZB_CLUSTER_FC18_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC18_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC19 ((zb_uint16_t)0xFC19)
#define ESPHOME_ZB_CLUSTER_FC19_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC19_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1A ((zb_uint16_t)0xFC1A)
#define ESPHOME_ZB_CLUSTER_FC1A_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1A_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1B ((zb_uint16_t)0xFC1B)
#define ESPHOME_ZB_CLUSTER_FC1B_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1B_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1C ((zb_uint16_t)0xFC1C)
#define ESPHOME_ZB_CLUSTER_FC1C_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1C_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1D ((zb_uint16_t)0xFC1D)
#define ESPHOME_ZB_CLUSTER_FC1D_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1D_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1E ((zb_uint16_t)0xFC1E)
#define ESPHOME_ZB_CLUSTER_FC1E_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1E_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1F ((zb_uint16_t)0xFC1F)
#define ESPHOME_ZB_CLUSTER_FC1F_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC1F_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC20 ((zb_uint16_t)0xFC20)
#define ESPHOME_ZB_CLUSTER_FC20_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC20_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC21 ((zb_uint16_t)0xFC21)
#define ESPHOME_ZB_CLUSTER_FC21_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC21_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC22 ((zb_uint16_t)0xFC22)
#define ESPHOME_ZB_CLUSTER_FC22_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC22_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC23 ((zb_uint16_t)0xFC23)
#define ESPHOME_ZB_CLUSTER_FC23_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC23_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC24 ((zb_uint16_t)0xFC24)
#define ESPHOME_ZB_CLUSTER_FC24_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC24_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC25 ((zb_uint16_t)0xFC25)
#define ESPHOME_ZB_CLUSTER_FC25_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC25_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC26 ((zb_uint16_t)0xFC26)
#define ESPHOME_ZB_CLUSTER_FC26_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC26_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC27 ((zb_uint16_t)0xFC27)
#define ESPHOME_ZB_CLUSTER_FC27_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC27_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC28 ((zb_uint16_t)0xFC28)
#define ESPHOME_ZB_CLUSTER_FC28_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC28_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC29 ((zb_uint16_t)0xFC29)
#define ESPHOME_ZB_CLUSTER_FC29_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC29_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2A ((zb_uint16_t)0xFC2A)
#define ESPHOME_ZB_CLUSTER_FC2A_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2A_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2B ((zb_uint16_t)0xFC2B)
#define ESPHOME_ZB_CLUSTER_FC2B_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2B_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2C ((zb_uint16_t)0xFC2C)
#define ESPHOME_ZB_CLUSTER_FC2C_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2C_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2D ((zb_uint16_t)0xFC2D)
#define ESPHOME_ZB_CLUSTER_FC2D_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2D_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2E ((zb_uint16_t)0xFC2E)
#define ESPHOME_ZB_CLUSTER_FC2E_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2E_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2F ((zb_uint16_t)0xFC2F)
#define ESPHOME_ZB_CLUSTER_FC2F_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC2F_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC30 ((zb_uint16_t)0xFC30)
#define ESPHOME_ZB_CLUSTER_FC30_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC30_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC31 ((zb_uint16_t)0xFC31)
#define ESPHOME_ZB_CLUSTER_FC31_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC31_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC32 ((zb_uint16_t)0xFC32)
#define ESPHOME_ZB_CLUSTER_FC32_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC32_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC33 ((zb_uint16_t)0xFC33)
#define ESPHOME_ZB_CLUSTER_FC33_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC33_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC34 ((zb_uint16_t)0xFC34)
#define ESPHOME_ZB_CLUSTER_FC34_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC34_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC35 ((zb_uint16_t)0xFC35)
#define ESPHOME_ZB_CLUSTER_FC35_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC35_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC36 ((zb_uint16_t)0xFC36)
#define ESPHOME_ZB_CLUSTER_FC36_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC36_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC37 ((zb_uint16_t)0xFC37)
#define ESPHOME_ZB_CLUSTER_FC37_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC37_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC38 ((zb_uint16_t)0xFC38)
#define ESPHOME_ZB_CLUSTER_FC38_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC38_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC39 ((zb_uint16_t)0xFC39)
#define ESPHOME_ZB_CLUSTER_FC39_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC39_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3A ((zb_uint16_t)0xFC3A)
#define ESPHOME_ZB_CLUSTER_FC3A_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3A_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3B ((zb_uint16_t)0xFC3B)
#define ESPHOME_ZB_CLUSTER_FC3B_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3B_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3C ((zb_uint16_t)0xFC3C)
#define ESPHOME_ZB_CLUSTER_FC3C_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3C_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3D ((zb_uint16_t)0xFC3D)
#define ESPHOME_ZB_CLUSTER_FC3D_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3D_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3E ((zb_uint16_t)0xFC3E)
#define ESPHOME_ZB_CLUSTER_FC3E_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3E_CLIENT_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3F ((zb_uint16_t)0xFC3F)
#define ESPHOME_ZB_CLUSTER_FC3F_SERVER_ROLE_INIT NULL
#define ESPHOME_ZB_CLUSTER_FC3F_CLIENT_ROLE_INIT NULL

// Cluster revision used in ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(name, ESPHOME_CUSTOM)
#define ESPHOME_CUSTOM_CLUSTER_REVISION_DEFAULT ((zb_uint16_t) 0x0001u)

namespace esphome::zigbee {

struct BinaryAttrs {
  zb_bool_t out_of_service;
  zb_bool_t present_value;
  zb_uint8_t status_flags;
  zb_uchar_t description[ZB_ZCL_MAX_STRING_SIZE];
};

struct AnalogAttrs {
  zb_bool_t out_of_service;
  float present_value;
  zb_uint8_t status_flags;
  zb_uint16_t engineering_units;
  zb_uchar_t description[ZB_ZCL_MAX_STRING_SIZE];
};

struct AnalogAttrsOutput : AnalogAttrs {
  float max_present_value;
  float min_present_value;
  float resolution;
};

class ZigbeeComponent : public Component {
 public:
  void setup() override;
  void dump_config() override;
  // The endpoint parameter is accepted for API compatibility but all entities
  // share the single endpoint 1; callbacks are dispatched by cluster_id inside
  // each entity's zcl_device_cb_ implementation.
  void add_callback(zb_uint8_t /*endpoint*/, std::function<void(zb_bufid_t bufid)> &&cb) {
    this->callbacks_.push_back(std::move(cb));
  }
  template<typename F> void add_on_join_callback(F &&cb) { this->join_cb_.add(std::forward<F>(cb)); }
  void zboss_signal_handler_esphome(zb_bufid_t bufid);
  void after_reporting_info(zb_zcl_configure_reporting_req_t *config_rep_req, zb_zcl_attr_addr_info_t *attr_addr_info);
  void factory_reset();
  void force_report();
  void loop() override;
  void set_sleepy(bool sleepy) { this->sleepy_ = sleepy; }

 protected:
  static void zcl_device_cb(zb_bufid_t bufid);
  void on_join_(bool factory_new);
#ifdef USE_ZIGBEE_WIPE_ON_BOOT
  void erase_flash_(int area);
#endif
  void dump_reporting_();
  std::vector<std::function<void(zb_bufid_t bufid)>> callbacks_{};
  CallbackManager<void(bool)> join_cb_;
  bool force_report_{false};
  uint32_t sleep_time_{};
  uint32_t sleep_remainder_{};
  bool sleepy_{};
};

class ZigbeeEntity {
 public:
  void set_parent(ZigbeeComponent *parent) { this->parent_ = parent; }
  void set_endpoint(zb_uint8_t endpoint) { this->endpoint_ = endpoint; }
  void set_cluster_id(zb_uint16_t cluster_id) { this->cluster_id_ = cluster_id; }

 protected:
  zb_uint8_t endpoint_{0};
  zb_uint16_t cluster_id_{0};
  ZigbeeComponent *parent_{nullptr};
};

extern ZigbeeComponent *global_zigbee;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

}  // namespace esphome::zigbee
#endif
