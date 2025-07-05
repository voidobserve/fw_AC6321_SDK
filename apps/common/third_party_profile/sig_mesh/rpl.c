/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2020 Lingao Meng
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "adaptation.h"
#include "mesh.h"
#include "net.h"
#include "rpl.h"
#include "settings.h"

#define LOG_TAG             "[MESH-rpl]"
#define LOG_INFO_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_WARN_ENABLE
#define LOG_ERROR_ENABLE
#define LOG_DUMP_ENABLE
#include "mesh_log.h"

#if MESH_RAM_AND_CODE_MAP_DETAIL
#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".ble_mesh_rpl_bss")
#pragma data_seg(".ble_mesh_rpl_data")
#pragma const_seg(".ble_mesh_rpl_const")
#pragma code_seg(".ble_mesh_rpl_code")
#endif
#else /* MESH_RAM_AND_CODE_MAP_DETAIL */
#pragma bss_seg(".ble_mesh_bss")
#pragma data_seg(".ble_mesh_data")
#pragma const_seg(".ble_mesh_const")
#pragma code_seg(".ble_mesh_code")
#endif /* MESH_RAM_AND_CODE_MAP_DETAIL */

static struct bt_mesh_rpl replay_list[CONFIG_BT_MESH_CRPL];
static ATOMIC_DEFINE(store, CONFIG_BT_MESH_CRPL);

enum {
    PENDING_CLEAR,
    PENDING_RESET,
    RPL_FLAGS_COUNT,
};
static ATOMIC_DEFINE(rpl_flags, RPL_FLAGS_COUNT);

static inline int rpl_idx(const struct bt_mesh_rpl *rpl)
{
    return rpl - &replay_list[0];
}

static void clear_rpl(struct bt_mesh_rpl *rpl, u8 index)
{
    extern struct __rpl_val __store_rpl[CONFIG_BT_MESH_CRPL];

    if (!rpl->src) {
        return;
    }

    atomic_clear_bit(store, rpl_idx(rpl));

    __store_rpl[index].rpl.seq = 0;
    __store_rpl[index].rpl.old_iv = 0;
    __store_rpl[index].src = 0;

    node_info_store(RPL_INDEX, __store_rpl, (sizeof(struct __rpl_val) * CONFIG_BT_MESH_CRPL));

    LOG_DBG("Cleared RPL");
}

static void schedule_rpl_store(struct bt_mesh_rpl *entry, bool force)
{
    atomic_set_bit(store, rpl_idx(entry));

    if (force
#ifdef CONFIG_BT_MESH_RPL_STORE_TIMEOUT
        || CONFIG_BT_MESH_RPL_STORE_TIMEOUT >= 0
#endif
       ) {
        bt_mesh_settings_store_schedule(BT_MESH_SETTINGS_RPL_PENDING);
    }
}

void bt_mesh_rpl_update(struct bt_mesh_rpl *rpl,
                        struct bt_mesh_net_rx *rx)
{
    /* If this is the first message on the new IV index, we should reset it
     * to zero to avoid invalid combinations of IV index and seg.
     */
    if (rpl->old_iv && !rx->old_iv) {
        rpl->seg = 0;
    }

    rpl->src = rx->ctx.addr;
    rpl->seq = rx->seq;
    rpl->old_iv = rx->old_iv;

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        schedule_rpl_store(rpl, false);
    }
}

/* Check the Replay Protection List for a replay attempt. If non-NULL match
 * parameter is given the RPL slot is returned, but it is not immediately
 * updated. This is used to prevent storing data in RPL that has been rejected
 * by upper logic (access, transport commands) and for receiving the segmented messages.
 * If a NULL match is given the RPL is immediately updated (used for proxy configuration).
 */
bool bt_mesh_rpl_check(struct bt_mesh_net_rx *rx, struct bt_mesh_rpl **match)
{
    struct bt_mesh_rpl *rpl;
    int i;

    /* Don't bother checking messages from ourselves */
    if (rx->net_if == BT_MESH_NET_IF_LOCAL) {
        return false;
    }

    /* The RPL is used only for the local node */
    if (!rx->local_match) {
        return false;
    }

    for (i = 0; i < ARRAY_SIZE(replay_list); i++) {
        rpl = &replay_list[i];

        /* Empty slot */
        if (!rpl->src) {
            goto match;
        }

        /* Existing slot for given address */
        if (rpl->src == rx->ctx.addr) {
            if (!rpl->old_iv &&
                atomic_test_bit(rpl_flags, PENDING_RESET) &&
                !atomic_test_bit(store, i)) {
                /* Until rpl reset is finished, entry with old_iv == false and
                 * without "store" bit set will be removed, therefore it can be
                 * reused. If such entry is reused, "store" bit will be set and
                 * the entry won't be removed.
                 */
                goto match;
            }

            if (rx->old_iv && !rpl->old_iv) {
                return true;
            }

            if ((!rx->old_iv && rpl->old_iv) ||
                rpl->seq < rx->seq) {
                goto match;
            } else {
                return true;
            }
        }
    }

    LOG_ERR("RPL is full!");
    return true;

match:
    if (match) {
        *match = rpl;
    } else {
        bt_mesh_rpl_update(rpl, rx);
    }

    return false;
}

void bt_mesh_rpl_clear(void)
{
    LOG_DBG("");

    if (!IS_ENABLED(CONFIG_BT_SETTINGS)) {
        (void)memset(replay_list, 0, sizeof(replay_list));
        return;
    }

    atomic_set_bit(rpl_flags, PENDING_CLEAR);

    bt_mesh_settings_store_schedule(BT_MESH_SETTINGS_RPL_PENDING);
}

struct bt_mesh_rpl *bt_mesh_rpl_find(u16_t src)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(replay_list); i++) {
        if (replay_list[i].src == src) {
            return &replay_list[i];
        }
    }

    return NULL;
}

struct bt_mesh_rpl *bt_mesh_rpl_alloc(u16_t src)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(replay_list); i++) {
        if (!replay_list[i].src) {
            replay_list[i].src = src;
            return &replay_list[i];
        }
    }

    return NULL;
}

void bt_mesh_rpl_reset(void)
{
    /* Discard "old old" IV Index entries from RPL and flag
     * any other ones (which are valid) as old.
     */
    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        int i;

        for (i = 0; i < ARRAY_SIZE(replay_list); i++) {
            struct bt_mesh_rpl *rpl = &replay_list[i];

            if (!rpl->src) {
                continue;
            }

            /* Entries with "store" bit set will be stored, other entries will be
             * removed.
             */
            atomic_set_bit_to(store, i, !rpl->old_iv);
            rpl->old_iv = !rpl->old_iv;
        }

        if (i != 0) {
            atomic_set_bit(rpl_flags, PENDING_RESET);
            bt_mesh_settings_store_schedule(BT_MESH_SETTINGS_RPL_PENDING);
        }
    } else {
        int shift = 0;
        int last = 0;

        for (int i = 0; i < ARRAY_SIZE(replay_list); i++) {
            struct bt_mesh_rpl *rpl = &replay_list[i];

            if (rpl->src) {
                if (rpl->old_iv) {
                    (void)memset(rpl, 0, sizeof(*rpl));

                    shift++;
                } else {
                    rpl->old_iv = true;

                    if (shift > 0) {
                        replay_list[i - shift] = *rpl;
                    }
                }

                last = i;
            }
        }

        (void)memset(&replay_list[last - shift + 1], 0, sizeof(struct bt_mesh_rpl) * shift);
    }
}

void bt_mesh_rpl_pending_store(u16_t addr)
{
    int shift = 0;
    int last = 0;
    bool clr;
    bool rst;

    if (!IS_ENABLED(CONFIG_BT_SETTINGS) ||
        (!BT_MESH_ADDR_IS_UNICAST(addr) &&
         addr != BT_MESH_ADDR_ALL_NODES)) {
        return;
    }

    if (addr == BT_MESH_ADDR_ALL_NODES) {
        bt_mesh_settings_store_cancel(BT_MESH_SETTINGS_RPL_PENDING);
    }

    clr = atomic_test_and_clear_bit(rpl_flags, PENDING_CLEAR);
    rst = atomic_test_bit(rpl_flags, PENDING_RESET);

    for (int i = 0; i < ARRAY_SIZE(replay_list); i++) {
        struct bt_mesh_rpl *rpl = &replay_list[i];

        if (addr != BT_MESH_ADDR_ALL_NODES && addr != rpl->src) {
            continue;
        }

        if (clr) {
            clear_rpl(rpl, i);
            shift++;
        } else if (atomic_test_and_clear_bit(store, i)) {
            if (shift > 0) {
                replay_list[i - shift] = *rpl;
            }

            store_rpl(&replay_list[i - shift], (i - shift));
        } else if (rst) {
            clear_rpl(rpl, i);

            /* Check if this entry was re-used during removal. If so, shift it as well.
             * Otherwise, increment shift counter.
             */
            if (atomic_test_and_clear_bit(store, i)) {
                replay_list[i - shift] = *rpl;
                atomic_set_bit(store, i - shift);
            } else {
                shift++;
            }
        }

        last = i;

        if (addr != BT_MESH_ADDR_ALL_NODES) {
            break;
        }
    }

    atomic_clear_bit(rpl_flags, PENDING_RESET);

    if (addr == BT_MESH_ADDR_ALL_NODES) {
        (void)memset(&replay_list[last - shift + 1], 0, sizeof(struct bt_mesh_rpl) * shift);
    }
}