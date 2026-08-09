/**
 * Multi-client SLE Server for the NearLink RSSI positioning project.
 *
 * The announcement name intentionally matches the official sle_rssi_ranging
 * Client. This slice establishes and observes multiple connections; RSSI reading
 * and ranging remain Client responsibilities.
 */
#include "securec.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"

#define NEARLINK_SERVER_LOG                 "[nearlink rssi server]"
#define NEARLINK_SERVER_NAME                "sle_rssi_server"
#define NEARLINK_SERVER_ADV_HANDLE          1
#define NEARLINK_SERVER_ADV_DATA_MAX_LEN    251
#define NEARLINK_SERVER_ADV_INTERVAL        0xC8
#define NEARLINK_SERVER_CONN_INTERVAL       50
#define NEARLINK_SERVER_CONN_TIMEOUT        500
#define NEARLINK_SERVER_ADV_TX_POWER_DBM    18
#define NEARLINK_SERVER_ADV_CHANNEL_MAP     0x07
#define NEARLINK_SERVER_TASK_PRIORITY       28
#define NEARLINK_SERVER_TASK_STACK_SIZE     0x1000
#define NEARLINK_SERVER_MAX_CONNECTIONS     4U

#define NEARLINK_ADV_TYPE_DISCOVERY_LEVEL   0x01
#define NEARLINK_ADV_TYPE_ACCESS_MODE       0x02
#define NEARLINK_ADV_TYPE_COMPLETE_NAME     0x0B
#define NEARLINK_ADV_FIELD_HEADER_LEN       2U

typedef struct {
    uint8_t length;
    uint8_t type;
    uint8_t value;
} nearlink_adv_common_value_t;

typedef enum {
    NEARLINK_ANNOUNCE_STOPPED = 0,
    NEARLINK_ANNOUNCE_STARTING,
    NEARLINK_ANNOUNCE_RUNNING,
} nearlink_announce_state_t;

typedef struct {
    bool in_use;
    uint16_t conn_id;
    sle_addr_t peer_addr;
    sle_pair_state_t pair_state;
} nearlink_server_conn_ctx_t;

static nearlink_server_conn_ctx_t g_conn_table[NEARLINK_SERVER_MAX_CONNECTIONS] = {0};
static nearlink_announce_state_t g_announce_state = NEARLINK_ANNOUNCE_STOPPED;

static uint16_t nearlink_server_append_name(uint8_t *data, uint16_t max_len)
{
    const uint8_t server_name[] = NEARLINK_SERVER_NAME;
    const uint8_t server_name_len = sizeof(server_name) - 1U;

    if (max_len < (uint16_t)(server_name_len + NEARLINK_ADV_FIELD_HEADER_LEN)) {
        return 0;
    }
    data[0] = server_name_len + 1U;
    data[1] = NEARLINK_ADV_TYPE_COMPLETE_NAME;
    if (memcpy_s(&data[NEARLINK_ADV_FIELD_HEADER_LEN], max_len - NEARLINK_ADV_FIELD_HEADER_LEN,
        server_name, server_name_len) != EOK) {
        return 0;
    }
    return (uint16_t)(server_name_len + NEARLINK_ADV_FIELD_HEADER_LEN);
}

static uint16_t nearlink_server_build_announce_data(uint8_t *data)
{
    const nearlink_adv_common_value_t discovery = {
        .length = sizeof(discovery) - 1U,
        .type = NEARLINK_ADV_TYPE_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    const nearlink_adv_common_value_t access_mode = {
        .length = sizeof(access_mode) - 1U,
        .type = NEARLINK_ADV_TYPE_ACCESS_MODE,
        .value = 0,
    };
    uint16_t index = 0;

    if (memcpy_s(&data[index], NEARLINK_SERVER_ADV_DATA_MAX_LEN - index,
        &discovery, sizeof(discovery)) != EOK) {
        return 0;
    }
    index += sizeof(discovery);
    if (memcpy_s(&data[index], NEARLINK_SERVER_ADV_DATA_MAX_LEN - index,
        &access_mode, sizeof(access_mode)) != EOK) {
        return 0;
    }
    return (uint16_t)(index + sizeof(access_mode));
}

static errcode_t nearlink_server_set_announce_param(void)
{
    sle_announce_param_t param = {0};
    const uint8_t anchor_addr[SLE_ADDR_LEN] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = NEARLINK_SERVER_ADV_HANDLE;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = NEARLINK_SERVER_ADV_CHANNEL_MAP;
    param.announce_interval_min = NEARLINK_SERVER_ADV_INTERVAL;
    param.announce_interval_max = NEARLINK_SERVER_ADV_INTERVAL;
    param.conn_interval_min = NEARLINK_SERVER_CONN_INTERVAL;
    param.conn_interval_max = NEARLINK_SERVER_CONN_INTERVAL;
    param.conn_max_latency = 0;
    param.conn_supervision_timeout = NEARLINK_SERVER_CONN_TIMEOUT;
    param.announce_tx_power = NEARLINK_SERVER_ADV_TX_POWER_DBM;
    param.own_addr.type = 0;
    if (memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, anchor_addr, SLE_ADDR_LEN) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    return sle_set_announce_param(param.announce_handle, &param);
}

static errcode_t nearlink_server_set_announce_data(void)
{
    sle_announce_data_t payload = {0};
    uint8_t announce_data[NEARLINK_SERVER_ADV_DATA_MAX_LEN] = {0};
    uint8_t seek_response[NEARLINK_SERVER_ADV_DATA_MAX_LEN] = {0};

    payload.announce_data_len = nearlink_server_build_announce_data(announce_data);
    payload.seek_rsp_data_len = nearlink_server_append_name(seek_response, sizeof(seek_response));
    if ((payload.announce_data_len == 0) || (payload.seek_rsp_data_len == 0)) {
        return ERRCODE_SLE_FAIL;
    }
    payload.announce_data = announce_data;
    payload.seek_rsp_data = seek_response;
    return sle_set_announce_data(NEARLINK_SERVER_ADV_HANDLE, &payload);
}

static nearlink_server_conn_ctx_t *nearlink_server_find_connection(uint16_t conn_id)
{
    uint8_t index;

    for (index = 0; index < NEARLINK_SERVER_MAX_CONNECTIONS; index++) {
        if (g_conn_table[index].in_use && (g_conn_table[index].conn_id == conn_id)) {
            return &g_conn_table[index];
        }
    }
    return NULL;
}

static nearlink_server_conn_ctx_t *nearlink_server_find_free_connection(void)
{
    uint8_t index;

    for (index = 0; index < NEARLINK_SERVER_MAX_CONNECTIONS; index++) {
        if (!g_conn_table[index].in_use) {
            return &g_conn_table[index];
        }
    }
    return NULL;
}

static nearlink_server_conn_ctx_t *nearlink_server_find_connection_by_addr(const sle_addr_t *addr,
    const nearlink_server_conn_ctx_t *excluded_ctx)
{
    uint8_t index;

    if (addr == NULL) {
        return NULL;
    }
    for (index = 0; index < NEARLINK_SERVER_MAX_CONNECTIONS; index++) {
        if (g_conn_table[index].in_use && (&g_conn_table[index] != excluded_ctx) &&
            (g_conn_table[index].peer_addr.type == addr->type) &&
            (memcmp(g_conn_table[index].peer_addr.addr, addr->addr, SLE_ADDR_LEN) == 0)) {
            return &g_conn_table[index];
        }
    }
    return NULL;
}

static uint8_t nearlink_server_active_connection_count(void)
{
    uint8_t index;
    uint8_t count = 0;

    for (index = 0; index < NEARLINK_SERVER_MAX_CONNECTIONS; index++) {
        if (g_conn_table[index].in_use) {
            count++;
        }
    }
    return count;
}

static errcode_t nearlink_server_ensure_announce(const char *reason)
{
    errcode_t ret;

    if (g_announce_state == NEARLINK_ANNOUNCE_RUNNING) {
        osal_printk("%s announcement already running: reason=%s\r\n", NEARLINK_SERVER_LOG, reason);
        return ERRCODE_SLE_SUCCESS;
    }
    if (g_announce_state == NEARLINK_ANNOUNCE_STARTING) {
        osal_printk("%s announcement start already pending: reason=%s\r\n", NEARLINK_SERVER_LOG, reason);
        return ERRCODE_SLE_SUCCESS;
    }

    g_announce_state = NEARLINK_ANNOUNCE_STARTING;
    ret = sle_start_announce(NEARLINK_SERVER_ADV_HANDLE);
    if (ret != ERRCODE_SLE_SUCCESS) {
        g_announce_state = NEARLINK_ANNOUNCE_STOPPED;
        osal_printk("%s announcement start failed: reason=%s, status=0x%x\r\n",
            NEARLINK_SERVER_LOG, reason, ret);
        return ret;
    }
    osal_printk("%s announcement start requested: reason=%s\r\n", NEARLINK_SERVER_LOG, reason);
    return ERRCODE_SLE_SUCCESS;
}

static void nearlink_server_announce_enable_cb(uint32_t announce_id, errcode_t status)
{
    if (status != ERRCODE_SLE_SUCCESS) {
        g_announce_state = NEARLINK_ANNOUNCE_STOPPED;
        osal_printk("%s announce enable failed: id=%u, status=0x%x\r\n",
            NEARLINK_SERVER_LOG, announce_id, status);
        return;
    }
    g_announce_state = NEARLINK_ANNOUNCE_RUNNING;
    osal_printk("%s server ready: announcement running, id=%u\r\n",
        NEARLINK_SERVER_LOG, announce_id);
}

static void nearlink_server_announce_disable_cb(uint32_t announce_id, errcode_t status)
{
    if (status == ERRCODE_SLE_SUCCESS) {
        g_announce_state = NEARLINK_ANNOUNCE_STOPPED;
    }
    osal_printk("%s announce disable complete: id=%u, status=0x%x\r\n",
        NEARLINK_SERVER_LOG, announce_id, status);
}

static errcode_t nearlink_server_register_announce_callbacks(void)
{
    sle_announce_seek_callbacks_t callbacks = {0};

    callbacks.announce_enable_cb = nearlink_server_announce_enable_cb;
    callbacks.announce_disable_cb = nearlink_server_announce_disable_cb;
    return sle_announce_seek_register_callbacks(&callbacks);
}

static void nearlink_server_connection_state_cb(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        nearlink_server_conn_ctx_t *ctx = nearlink_server_find_connection(conn_id);
        bool new_connection = (ctx == NULL);
        uint8_t active_count;
        uint8_t slot;
        errcode_t ret;

        if (new_connection) {
            /* A successful incoming connection consumes the current connectable announcement. */
            g_announce_state = NEARLINK_ANNOUNCE_STOPPED;
            ctx = nearlink_server_find_free_connection();
        }
        if (ctx == NULL) {
            osal_printk("%s connection table full: conn_id=0x%02x, active=%u/%u\r\n",
                NEARLINK_SERVER_LOG, conn_id, nearlink_server_active_connection_count(),
                NEARLINK_SERVER_MAX_CONNECTIONS);
            if (addr != NULL) {
                ret = sle_disconnect_remote_device(addr);
                if (ret != ERRCODE_SLE_SUCCESS) {
                    osal_printk("%s reject extra connection failed: conn_id=0x%02x, status=0x%x\r\n",
                        NEARLINK_SERVER_LOG, conn_id, ret);
                }
            }
            return;
        }

        if (new_connection) {
            (void)memset_s(ctx, sizeof(*ctx), 0, sizeof(*ctx));
            ctx->in_use = true;
            ctx->conn_id = conn_id;
        }
        ctx->pair_state = pair_state;
        if ((addr != NULL) &&
            (memcpy_s(&ctx->peer_addr, sizeof(ctx->peer_addr), addr, sizeof(*addr)) != EOK)) {
            osal_printk("%s save peer address failed: conn_id=0x%02x\r\n", NEARLINK_SERVER_LOG, conn_id);
            (void)memset_s(ctx, sizeof(*ctx), 0, sizeof(*ctx));
            ret = sle_disconnect_remote_device(addr);
            if (ret != ERRCODE_SLE_SUCCESS) {
                osal_printk("%s disconnect after address copy failure failed: conn_id=0x%02x, status=0x%x\r\n",
                    NEARLINK_SERVER_LOG, conn_id, ret);
            }
            return;
        }

        if (addr != NULL) {
            nearlink_server_conn_ctx_t *same_addr_ctx = nearlink_server_find_connection_by_addr(addr, ctx);

            if (same_addr_ctx != NULL) {
                osal_printk("%s duplicate peer address: existing_conn_id=0x%02x, new_conn_id=0x%02x\r\n",
                    NEARLINK_SERVER_LOG, same_addr_ctx->conn_id, conn_id);
            }
        }

        slot = (uint8_t)(ctx - g_conn_table);
        active_count = nearlink_server_active_connection_count();
        if (addr != NULL) {
            osal_printk("%s connected: slot=%u, conn_id=0x%02x, pair_state=0x%x, "
                "peer=%02x:%02x:%02x:%02x:%02x:%02x, active=%u/%u\r\n",
                NEARLINK_SERVER_LOG, slot, conn_id, pair_state, addr->addr[0], addr->addr[1], addr->addr[2],
                addr->addr[3], addr->addr[4], addr->addr[5], active_count, NEARLINK_SERVER_MAX_CONNECTIONS);
        } else {
            osal_printk("%s connected: slot=%u, conn_id=0x%02x, pair_state=0x%x, peer=unknown, active=%u/%u\r\n",
                NEARLINK_SERVER_LOG, slot, conn_id, pair_state, active_count, NEARLINK_SERVER_MAX_CONNECTIONS);
        }

        if (active_count < NEARLINK_SERVER_MAX_CONNECTIONS) {
            (void)nearlink_server_ensure_announce("connection established");
        } else {
            osal_printk("%s connection capacity reached: active=%u/%u\r\n",
                NEARLINK_SERVER_LOG, active_count, NEARLINK_SERVER_MAX_CONNECTIONS);
        }
        return;
    }

    if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        nearlink_server_conn_ctx_t *ctx = nearlink_server_find_connection(conn_id);
        uint8_t active_count;

        if (ctx != NULL) {
            uint8_t slot = (uint8_t)(ctx - g_conn_table);

            (void)memset_s(ctx, sizeof(*ctx), 0, sizeof(*ctx));
            active_count = nearlink_server_active_connection_count();
            osal_printk("%s disconnected: slot=%u, conn_id=0x%02x, reason=0x%x, active=%u/%u\r\n",
                NEARLINK_SERVER_LOG, slot, conn_id, disc_reason, active_count, NEARLINK_SERVER_MAX_CONNECTIONS);
        } else {
            active_count = nearlink_server_active_connection_count();
            osal_printk("%s disconnected unknown connection: conn_id=0x%02x, reason=0x%x, active=%u/%u\r\n",
                NEARLINK_SERVER_LOG, conn_id, disc_reason, active_count, NEARLINK_SERVER_MAX_CONNECTIONS);
        }
        if (active_count < NEARLINK_SERVER_MAX_CONNECTIONS) {
            (void)nearlink_server_ensure_announce("connection released");
        }
        return;
    }

    osal_printk("%s connection state changed: conn_id=0x%02x, state=0x%x\r\n",
        NEARLINK_SERVER_LOG, conn_id, conn_state);
}

static errcode_t nearlink_server_register_connection_callbacks(void)
{
    sle_connection_callbacks_t callbacks = {0};

    callbacks.connect_state_changed_cb = nearlink_server_connection_state_cb;
    return sle_connection_register_callbacks(&callbacks);
}

static errcode_t nearlink_server_start_announce(void)
{
    errcode_t ret = nearlink_server_set_announce_param();

    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s set announce parameters failed: 0x%x\r\n", NEARLINK_SERVER_LOG, ret);
        return ret;
    }
    ret = nearlink_server_set_announce_data();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s set announce data failed: 0x%x\r\n", NEARLINK_SERVER_LOG, ret);
        return ret;
    }
    ret = nearlink_server_ensure_announce("server initialization");
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s start announce failed: 0x%x\r\n", NEARLINK_SERVER_LOG, ret);
        return ret;
    }
    osal_printk("%s announce start request accepted: name=%s, handle=%u, anchor_addr=41:42:43:44:45:46\r\n",
        NEARLINK_SERVER_LOG, NEARLINK_SERVER_NAME, NEARLINK_SERVER_ADV_HANDLE);
    return ERRCODE_SUCC;
}

static errcode_t nearlink_server_init(void)
{
    errcode_t ret;

    osal_printk("%s initialization begin\r\n", NEARLINK_SERVER_LOG);
    /* Follow the local SDK RSSI Server sequence: enable SLE before registering Server callbacks. */
    ret = enable_sle();
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s enable SLE failed: 0x%x\r\n", NEARLINK_SERVER_LOG, ret);
        return ret;
    }
    osal_printk("%s SLE enabled successfully\r\n", NEARLINK_SERVER_LOG);

    ret = nearlink_server_register_announce_callbacks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register announce callbacks failed: 0x%x\r\n", NEARLINK_SERVER_LOG, ret);
        return ret;
    }
    ret = nearlink_server_register_connection_callbacks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register connection callbacks failed: 0x%x\r\n", NEARLINK_SERVER_LOG, ret);
        return ret;
    }
    osal_printk("%s required callbacks registered\r\n", NEARLINK_SERVER_LOG);
    return nearlink_server_start_announce();
}

static void *nearlink_server_task(const char *arg)
{
    errcode_t ret;

    unused(arg);
    osal_printk("%s task started\r\n", NEARLINK_SERVER_LOG);
    ret = nearlink_server_init();
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s initialization request failed: status=0x%x\r\n", NEARLINK_SERVER_LOG, ret);
    } 
    return NULL;
}

static void nearlink_server_entry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)nearlink_server_task, 0,
        "NearlinkRssiSrv", NEARLINK_SERVER_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        if (osal_kthread_set_priority(task_handle, NEARLINK_SERVER_TASK_PRIORITY) != OSAL_SUCCESS) {
            osal_printk("%s set task priority failed\r\n", NEARLINK_SERVER_LOG);
        }
    }
    osal_kthread_unlock();

    if (task_handle == NULL) {
        osal_printk("%s create task failed\r\n", NEARLINK_SERVER_LOG);
    }
}

app_run(nearlink_server_entry);
