#include "ble_terminal.h"
#include "terminal_protocol.h"


#include <string.h>
#include <assert.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/util/util.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "store/config/ble_store_config.h"

void ble_store_config_init(void);  //不加会报error（Werror），但实际在别的源文件里定义了。

static const char *TAG = "ble_terminal";

#define DEVICE_NAME "SleepTerminal_V1"

static uint8_t own_addr_type;

/*
 * UUID:
 * Service UUID: 9f2a0001-7d6a-4c2b-9f4b-2e7a1f000001
 * Write Char  : 9f2a0002-7d6a-4c2b-9f4b-2e7a1f000001
 *
 * 注意：
 * BLE 128-bit UUID 在 NimBLE 里按小端数组填写。
 */
static const ble_uuid128_t terminal_service_uuid =
    BLE_UUID128_INIT(
        0x01, 0x00, 0x00, 0x1f,
        0x7a, 0x2e,
        0x4b, 0x9f,
        0x2b, 0x4c,
        0x6a, 0x7d,
        0x01, 0x00, 0x2a, 0x9f
    );

static const ble_uuid128_t terminal_write_char_uuid =
    BLE_UUID128_INIT(
        0x01, 0x00, 0x00, 0x1f,
        0x7a, 0x2e,
        0x4b, 0x9f,
        0x2b, 0x4c,
        0x6a, 0x7d,
        0x02, 0x00, 0x2a, 0x9f
    );

static int terminal_gap_event(struct ble_gap_event *event, void *arg);
static void terminal_advertise(void);

static int terminal_write_access_cb(
    uint16_t conn_handle,
    uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt,
    void *arg
)
{
    uint8_t buf[32];
    uint16_t len = 0;
    int rc;

    if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    rc = ble_hs_mbuf_to_flat(ctxt->om, buf, sizeof(buf), &len);
    if (rc != 0) {
        ESP_LOGE(TAG, "mbuf to flat failed, rc=%d", rc);
        return BLE_ATT_ERR_UNLIKELY;
    }

    ESP_LOGI(TAG, "write received, len=%u", len);
    ESP_LOG_BUFFER_HEX(TAG, buf, len);

    if (len != 16) {
        ESP_LOGE(TAG, "invalid packet length: %u, expected 16", len);
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    terminal_sensor_data_t sensor_data;

    if (!terminal_parse_sensor_packet(buf, len, &sensor_data)) {
        ESP_LOGE(TAG, "parse sensor packet failed");
        return BLE_ATT_ERR_UNLIKELY;
    }

    ESP_LOGI(TAG,
             "APP RX: seq=%u, lux=%" PRIu32 " lx, uv=%.2f, battery=%u%%, status=%u",
             sensor_data.seq,
             sensor_data.lux,
             sensor_data.uv_x100 / 100.0f,
             sensor_data.battery,
             sensor_data.status);

    return 0;
}

static const struct ble_gatt_chr_def terminal_characteristics[] = {
    {
        .uuid = &terminal_write_char_uuid.u,
        .access_cb = terminal_write_access_cb,
        .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
    },
    {
        0,
    }
};

static const struct ble_gatt_svc_def terminal_services[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &terminal_service_uuid.u,
        .characteristics = terminal_characteristics,
    },
    {
        0,
    }
};

static void terminal_on_reset(int reason)
{
    ESP_LOGE(TAG, "NimBLE reset, reason=%d", reason);
}

static void terminal_on_sync(void)
{
    int rc;

    rc = ble_hs_util_ensure_addr(0);
    if (rc != 0) {
        ESP_LOGE(TAG, "ensure addr failed, rc=%d", rc);
        return;
    }

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "infer addr type failed, rc=%d", rc);
        return;
    }

    ESP_LOGI(TAG, "NimBLE synced, start advertising");
    terminal_advertise();
}

static void terminal_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE host task started");

    nimble_port_run();

    nimble_port_freertos_deinit();
}

static void terminal_advertise(void)
{
    struct ble_hs_adv_fields fields;
    struct ble_gap_adv_params adv_params;
    const char *name;
    int rc;

    memset(&fields, 0, sizeof(fields));

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "set adv fields failed, rc=%d", rc);
        return;
    }

    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(
        own_addr_type,
        NULL,
        BLE_HS_FOREVER,
        &adv_params,
        terminal_gap_event,
        NULL
    );

    if (rc != 0) {
        ESP_LOGE(TAG, "start advertising failed, rc=%d", rc);
        return;
    }

    ESP_LOGI(TAG, "advertising started, device name=%s", DEVICE_NAME);
}

static int terminal_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            ESP_LOGI(TAG, "connected, conn_handle=%d", event->connect.conn_handle);
        } else {
            ESP_LOGE(TAG, "connect failed, status=%d", event->connect.status);
            terminal_advertise();
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "disconnected, reason=%d", event->disconnect.reason);
        terminal_advertise();
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        ESP_LOGI(TAG, "connection updated, status=%d", event->conn_update.status);
        return 0;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "mtu updated, mtu=%d", event->mtu.value);
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "advertising complete, restart advertising");
        terminal_advertise();
        return 0;

    default:
        return 0;
    }
}

esp_err_t ble_terminal_init(void)
{
    int rc;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs init failed");
        return ret;
    }

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nimble_port_init failed, ret=%d", ret);
        return ret;
    }

    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_svc_gap_device_name_set(DEVICE_NAME);
    if (rc != 0) {
        ESP_LOGE(TAG, "set device name failed, rc=%d", rc);
        return ESP_FAIL;
    }

    rc = ble_gatts_count_cfg(terminal_services);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gatts_count_cfg failed, rc=%d", rc);
        return ESP_FAIL;
    }

    rc = ble_gatts_add_svcs(terminal_services);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gatts_add_svcs failed, rc=%d", rc);
        return ESP_FAIL;
    }

    ble_hs_cfg.reset_cb = terminal_on_reset;
    ble_hs_cfg.sync_cb = terminal_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_store_config_init();

    nimble_port_freertos_init(terminal_host_task);

    ESP_LOGI(TAG, "BLE terminal init done");

    return ESP_OK;
}