
# make 编译并下载
# make VERBOSE=1 显示编译详细过程
# make clean 清除编译临时文件
#
# 注意： Linux 下编译方式：
#     1. 从 http://pkgman.jieliapp.com/doc/all 处找到下载链接
#     2. 下载后，解压到 /opt/jieli 目录下，保证
#       /opt/jieli/common/bin/clang 存在（注意目录层次）
#     3. 确认 ulimit -n 的结果足够大（建议大于8096），否则链接可能会因为打开文件太多而失败
#       可以通过 ulimit -n 8096 来设置一个较大的值
#

# 工具路径设置
ifeq ($(OS), Windows_NT)
# Windows 下工具链位置
TOOL_DIR := C:/JL/pi32/bin
CC    := clang.exe
CXX   := clang.exe
LD    := q32s-lto-wrapper.exe
AR    := llvm-ar.exe
MKDIR := mkdir_win -p
RM    := rm -rf

SYS_LIB_DIR := C:/JL/pi32/q32s-lib
SYS_INC_DIR := C:/JL/pi32/q32s-include
EXT_CFLAGS  := # Windows 下不需要 -D__SHELL__
export PATH:=$(TOOL_DIR);$(PATH)

## 后处理脚本
FIXBAT          := ..\..\..\..\tools\utils\fixbat.exe # 用于处理 utf8->gbk 编码问题
POST_SCRIPT     := ../../../../cpu/bd19/tools/download.bat
RUN_POST_SCRIPT := ..\..\..\..\cpu\bd19\tools\download.bat
else
# Linux 下工具链位置
TOOL_DIR := /opt/jieli/q32s/bin
CC    := clang
CXX   := clang
LD    := lto-wrapper
AR    := lto-ar
MKDIR := mkdir -p
RM    := rm -rf
export OBJDUMP := $(TOOL_DIR)/objdump
export OBJCOPY := $(TOOL_DIR)/objcopy
export OBJSIZEDUMP := $(TOOL_DIR)/objsizedump

SYS_LIB_DIR := $(TOOL_DIR)/../lib
SYS_INC_DIR := $(TOOL_DIR)/../include
EXT_CFLAGS  := -D__SHELL__ # Linux 下需要这个保证正确处理 download.c
export PATH:=$(TOOL_DIR):$(PATH)

## 后处理脚本
FIXBAT          := touch # Linux下不需要处理 bat 编码问题
POST_SCRIPT     := ../../../../cpu/bd19/tools/download.sh
RUN_POST_SCRIPT := bash $(POST_SCRIPT)
endif

CC  := $(TOOL_DIR)/$(CC)
CXX := $(TOOL_DIR)/$(CXX)
LD  := $(TOOL_DIR)/$(LD)
AR  := $(TOOL_DIR)/$(AR)
# 输出文件设置
OUT_ELF   := ../../../../cpu/bd19/tools/sdk.elf
OBJ_FILE  := $(OUT_ELF).objs.txt
# 编译路径设置
BUILD_DIR := objs
# 工程路径前缀
ROOT_PREFIX := ../../../..

# 编译参数设置
CFLAGS := \
	-flto \
	-target q32s \
	-integrated-as \
	-fno-builtin \
	-mllvm -inline-threshold=5 \
	-Oz \
	-integrated-as \
	-g \
	-O0 \
	-flto \
	-Os \
	-Wcast-align \
	-w \
	-fallow-pointer-null \
	-Wincompatible-pointer-types \
	-Wundef \
	-fprefer-gnu-section \
	-Wframe-larger-than=256 \
	-Wreturn-type \
	-Wimplicit-function-declaration \
	-Werror=jl-arg-struct-check \
	-fms-extensions \


# C++额外的编译参数
CXXFLAGS :=


# 宏定义
DEFINES := \
	-DSUPPORT_MS_EXTENSIONS \
	-DCONFIG_RELEASE_ENABLE \
	-DCONFIG_CPU_BD19 \
	-DCONFIG_NEW_BREDR_ENABLE \
	-DCONFIG_FREE_RTOS_ENABLE \
	-DCONFIG_EQ_SUPPORT_ASYNC \
	-DEQ_DESIGN_IN_MASK \
	-DSINE_MAKE_IN_MASK \
	-DCONFIG_MMU_ENABLE \
	-DCONFIG_AEC_M=256 \
	-DCONFIG_AUDIO_ONCHIP \
	-DCONFIG_MEDIA_NEW_ENABLE \
	-D__GCC_Q32S__ \
	-DCONFIG_NEW_ECC_ENABLE \
	-DCONFIG_CRYPTO_TOOLBOX_OSIZE_IN_MASKROM \
	-DEVENT_HANDLER_NUM_CONFIG=2 \
	-DEVENT_TOUCH_ENABLE_CONFIG=0 \
	-DEVENT_POOL_SIZE_CONFIG=256 \
	-DCONFIG_EVENT_KEY_MAP_ENABLE=0 \
	-DTIMER_POOL_NUM_CONFIG=10 \
	-DAPP_ASYNC_POOL_NUM_CONFIG=0 \
	-DSDK_VERSION_CFG_DEFINE=0x240001 \
	-DSDK_VERSION_DATE_DEFINE=20241231 \
	-DVFS_ENABLE=1 \
	-DUSE_SDFILE_NEW=1 \
	-DVFS_FILE_POOL_NUM_CONFIG=0 \
	-DSDFILE_VERSION=0x020000 \
	-DVM_MAX_SIZE_CONFIG=16*1024 \
	-DVM_ITEM_MAX_NUM=256 \
	-DCONFIG_TWS_ENABLE \
	-DCONFIG_SPP_AND_LE_CASE_ENABLE \
	-DCONFIG_TRANSFER_ENABLE \
	-DCONFIG_LITE_AEC_ENABLE=0 \
	-DCONFIG_LITE_AUDIO \
	-DCONFIG_LMP_RESET_ENABLE \
	-DCONFIG_LMP_REFRESH_ENCRYPTION_KEY_ENABLE \
	-DCONFIG_LINK_DISTURB_SCAN_ENABLE=0 \
	-DCONFIG_BTCTRLER_TASK_DEL_ENABLE \
	-DCONFIG_UPDATA_ENABLE \
	-DCONFIG_OTA_UPDATA_ENABLE \
	-DCONFIG_ITEM_FORMAT_VM \
	-DTCFG_APP_BT_EN=1 \
	-DUSB_PC_NO_APP_MODE \


DEFINES += $(EXT_CFLAGS) # 额外的一些定义

# 头文件搜索路径
INCLUDES := \
	-I../../../../include_lib \
	-I../../../../include_lib/driver \
	-I../../../../include_lib/driver/device \
	-I../../../../include_lib/driver/cpu/bd19 \
	-I../../../../include_lib/system \
	-I../../../../include_lib/system/generic \
	-I../../../../include_lib/system/device \
	-I../../../../include_lib/system/fs \
	-I../../../../include_lib/system/ui \
	-I../../../../include_lib/system/math/cpu/bd19 \
	-I../../../../include_lib/btctrler \
	-I../../../../include_lib/btctrler/port/bd19 \
	-I../../../../include_lib/update \
	-I../../../../include_lib/agreement \
	-I../../../../include_lib/btstack/third_party/common \
	-I../../../../include_lib/btstack/third_party/rcsp \
	-I../../../../include_lib/media/media_new \
	-I../../../../include_lib/media/media_new/media \
	-I../../../../include_lib/media/media_new/media/cpu/bd19 \
	-I../../../../include_lib/media/media_new/media/cpu/bd19/asm \
	-I../../../../apps/spp_and_le/include \
	-I../../../../apps/spp_and_le/board/bd19 \
	-I../../../../apps/common \
	-I../../../../apps/common/device \
	-I../../../../apps/common/include \
	-I../../../../apps/common/power_manage \
	-I../../../../cpu/bd19 \
	-I../../../../apps/common/third_party_profile/jieli \
	-I../../../../apps/common/third_party_profile/jieli/JL_rcsp \
	-I../../../../apps/common/third_party_profile/jieli/online_db \
	-I../../../../apps/common/third_party_profile/common \
	-I../../../../include_lib/ai_stack/JL_rcsp \
	-I../../../../include_lib/btstack \
	-I../../../../apps/common/third_party_profile/jieli/JL_rcsp/bt_trans_data \
	-I../../../../apps/common/third_party_profile/jieli/JL_rcsp/rcsp_updata \
	-I../../../../apps/common/third_party_profile/jieli/gatt_common \
	-I../../../../apps/common/third_party_profile/Tecent_LL/include \
	-I../../../../apps/common/third_party_profile/Tecent_LL/tecent_ll_demo \
	-I../../../../apps/spp_and_le/examples/ll_sync \
	-I../../../../apps/common/cJSON \
	-I../../../../apps/common/third_party_profile/tuya_protocol \
	-I../../../../apps/common/third_party_profile/tuya_protocol/app/demo \
	-I../../../../apps/common/third_party_profile/tuya_protocol/app/product_test \
	-I../../../../apps/common/third_party_profile/tuya_protocol/app/uart_common \
	-I../../../../apps/common/third_party_profile/tuya_protocol/extern_components/mbedtls \
	-I../../../../apps/common/third_party_profile/tuya_protocol/port \
	-I../../../../apps/common/third_party_profile/tuya_protocol/sdk/include \
	-I../../../../apps/common/third_party_profile/tuya_protocol/sdk/lib \
	-I../../../../apps/spp_and_le/examples/tuya \
	-I../../../../apps/common/third_party_profile/hilink_protocol \
	-I../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol \
	-I../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/mbedtls \
	-I../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/mbedtls/configs \
	-I../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/mbedtls/psa \
	-I../../../../apps/spp_and_le/examples/hilink \
	-I../../../../cpu/bd19/audio_encode \
	-I../../../../cpu/bd19/audio_decode \
	-I../../../../apps/common/audio \
	-I../../../../apps/common/device/usb \
	-I../../../../apps/common/device/usb/device \
	-I../../../../apps/common/device/usb/host \
	-I$(SYS_INC_DIR) \


# 需要编译的 .c 文件
c_SRC_FILES := \
	../../../../apps/common/audio/audio_digital_vol.c \
	../../../../apps/common/bt_common/bt_test_api.c \
	../../../../apps/common/cJSON/cJSON.c \
	../../../../apps/common/debug/debug.c \
	../../../../apps/common/device/gSensor/fmy/SC7A20_E.c \
	../../../../apps/common/device/gSensor/fmy/SC7A20_TR.c \
	../../../../apps/common/device/gSensor/fmy/gSensor_manage.c \
	../../../../apps/common/device/gSensor/fmy/gsensor_api.c \
	../../../../apps/common/device/gSensor/fmy/msa310.c \
	../../../../apps/common/device/gSensor/fmy/msa310_function.c \
	../../../../apps/common/device/key/adkey.c \
	../../../../apps/common/device/key/iokey.c \
	../../../../apps/common/device/key/irkey.c \
	../../../../apps/common/device/key/key_driver.c \
	../../../../apps/common/device/key/touch_key.c \
	../../../../apps/common/device/norflash/norflash.c \
	../../../../apps/common/device/usb/device/cdc.c \
	../../../../apps/common/device/usb/device/custom_hid.c \
	../../../../apps/common/device/usb/device/descriptor.c \
	../../../../apps/common/device/usb/device/hid.c \
	../../../../apps/common/device/usb/device/msd.c \
	../../../../apps/common/device/usb/device/msd_upgrade.c \
	../../../../apps/common/device/usb/device/task_pc.c \
	../../../../apps/common/device/usb/device/uac1.c \
	../../../../apps/common/device/usb/device/uac_stream.c \
	../../../../apps/common/device/usb/device/usb_device.c \
	../../../../apps/common/device/usb/device/user_setup.c \
	../../../../apps/common/device/usb/host/adb.c \
	../../../../apps/common/device/usb/host/aoa.c \
	../../../../apps/common/device/usb/host/audio.c \
	../../../../apps/common/device/usb/host/audio_demo.c \
	../../../../apps/common/device/usb/host/hid.c \
	../../../../apps/common/device/usb/host/usb_bulk_transfer.c \
	../../../../apps/common/device/usb/host/usb_ctrl_transfer.c \
	../../../../apps/common/device/usb/host/usb_host.c \
	../../../../apps/common/device/usb/host/usb_storage.c \
	../../../../apps/common/device/usb/usb_config.c \
	../../../../apps/common/device/usb/usb_host_config.c \
	../../../../apps/common/temp_trim/dtemp_pll_trim.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_ll_demo/ll_task.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_import.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_llsync_data.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_llsync_device.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_llsync_event.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_llsync_ota.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_service.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_template.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_utils_base64.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_utils_crc.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_utils_hmac.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_utils_log.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_utils_md5.c \
	../../../../apps/common/third_party_profile/Tecent_LL/tecent_protocol/ble_qiot_utils_sha1.c \
	../../../../apps/common/third_party_profile/common/custom_cfg.c \
	../../../../apps/common/third_party_profile/hilink_protocol/hilink_ota.c \
	../../../../apps/common/third_party_profile/hilink_protocol/hilink_protocol.c \
	../../../../apps/common/third_party_profile/hilink_protocol/hilink_task.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/aes.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/arc4.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/bignum.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/blowfish.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/ccm.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/chacha20.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/chachapoly.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/cipher.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/cipher_wrap.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/des.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/gcm.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/mbedtls_main.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/md.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/md5.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/pkcs5.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/poly1305.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/ripemd160.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/sha1.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/sha256.c \
	../../../../apps/common/third_party_profile/hilink_protocol/mbedtls_protocol/sha512.c \
	../../../../apps/common/third_party_profile/jieli/JL_rcsp/bt_trans_data/rcsp_hid_inter.c \
	../../../../apps/common/third_party_profile/jieli/JL_rcsp/rcsp_bluetooth.c \
	../../../../apps/common/third_party_profile/jieli/JL_rcsp/rcsp_updata/rcsp_ch_loader_download.c \
	../../../../apps/common/third_party_profile/jieli/JL_rcsp/rcsp_updata/rcsp_user_update.c \
	../../../../apps/common/third_party_profile/jieli/gatt_common/le_gatt_client.c \
	../../../../apps/common/third_party_profile/jieli/gatt_common/le_gatt_common.c \
	../../../../apps/common/third_party_profile/jieli/gatt_common/le_gatt_server.c \
	../../../../apps/common/third_party_profile/jieli/online_db/online_db_deal.c \
	../../../../apps/common/third_party_profile/jieli/online_db/spp_online_db.c \
	../../../../apps/common/third_party_profile/tuya_protocol/app/demo/tuya_ble_app_demo.c \
	../../../../apps/common/third_party_profile/tuya_protocol/app/demo/tuya_ota.c \
	../../../../apps/common/third_party_profile/tuya_protocol/app/product_test/tuya_ble_app_production_test.c \
	../../../../apps/common/third_party_profile/tuya_protocol/app/uart_common/tuya_ble_app_uart_common_handler.c \
	../../../../apps/common/third_party_profile/tuya_protocol/extern_components/mbedtls/aes.c \
	../../../../apps/common/third_party_profile/tuya_protocol/extern_components/mbedtls/ccm.c \
	../../../../apps/common/third_party_profile/tuya_protocol/extern_components/mbedtls/hmac.c \
	../../../../apps/common/third_party_profile/tuya_protocol/extern_components/mbedtls/md5.c \
	../../../../apps/common/third_party_profile/tuya_protocol/extern_components/mbedtls/sha1.c \
	../../../../apps/common/third_party_profile/tuya_protocol/extern_components/mbedtls/sha256.c \
	../../../../apps/common/third_party_profile/tuya_protocol/port/tuya_ble_port.c \
	../../../../apps/common/third_party_profile/tuya_protocol/port/tuya_ble_port_JL.c \
	../../../../apps/common/third_party_profile/tuya_protocol/port/tuya_ble_port_peripheral.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_api.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_bulk_data.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_data_handler.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_event.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_event_handler.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_event_handler_weak.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_feature_weather.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_gatt_send_queue.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_heap.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_main.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_mem.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_mutli_tsf_protocol.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_queue.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_storage.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_unix_time.c \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/src/tuya_ble_utils.c \
	../../../../apps/common/update/testbox_update.c \
	../../../../apps/common/update/uart_update.c \
	../../../../apps/common/update/uart_update_master.c \
	../../../../apps/common/update/update.c \
	../../../../apps/spp_and_le/app_main.c \
	../../../../apps/spp_and_le/board/bd19/board_ac6321a_demo.c \
	../../../../apps/spp_and_le/board/bd19/board_ac6323a_demo.c \
	../../../../apps/spp_and_le/board/bd19/board_ac6323a_fmy.c \
	../../../../apps/spp_and_le/board/bd19/board_ac6328a_demo.c \
	../../../../apps/spp_and_le/board/bd19/board_ac6328b_dongle.c \
	../../../../apps/spp_and_le/board/bd19/board_ac6329b_demo.c \
	../../../../apps/spp_and_le/board/bd19/board_ac6329c_demo.c \
	../../../../apps/spp_and_le/board/bd19/board_ac6329e_demo.c \
	../../../../apps/spp_and_le/board/bd19/board_ac6329f_demo.c \
	../../../../apps/spp_and_le/board/bd19/board_ac632n_demo.c \
	../../../../apps/spp_and_le/config/lib_btctrler_config.c \
	../../../../apps/spp_and_le/config/lib_btstack_config.c \
	../../../../apps/spp_and_le/config/lib_driver_config.c \
	../../../../apps/spp_and_le/config/lib_media_config.c \
	../../../../apps/spp_and_le/config/lib_profile_config.c \
	../../../../apps/spp_and_le/config/lib_system_config.c \
	../../../../apps/spp_and_le/config/lib_update_config.c \
	../../../../apps/spp_and_le/config/log_config.c \
	../../../../apps/spp_and_le/examples/at_char_com/app_at_char_com.c \
	../../../../apps/spp_and_le/examples/at_char_com/at_char_cmds.c \
	../../../../apps/spp_and_le/examples/at_char_com/at_char_uart.c \
	../../../../apps/spp_and_le/examples/at_char_com/ble_at_char_client.c \
	../../../../apps/spp_and_le/examples/at_char_com/ble_at_char_com.c \
	../../../../apps/spp_and_le/examples/at_com/app_at_com.c \
	../../../../apps/spp_and_le/examples/at_com/at_cmds.c \
	../../../../apps/spp_and_le/examples/at_com/at_uart.c \
	../../../../apps/spp_and_le/examples/at_com/ble_at_client.c \
	../../../../apps/spp_and_le/examples/at_com/ble_at_com.c \
	../../../../apps/spp_and_le/examples/at_com/spp_at_trans.c \
	../../../../apps/spp_and_le/examples/ble_beacon/app_beacon.c \
	../../../../apps/spp_and_le/examples/ble_beacon/ble_beacon.c \
	../../../../apps/spp_and_le/examples/ble_central/app_central.c \
	../../../../apps/spp_and_le/examples/ble_central/ble_central.c \
	../../../../apps/spp_and_le/examples/ble_central/ble_central_server.c \
	../../../../apps/spp_and_le/examples/conn_24g/app_conn_24g.c \
	../../../../apps/spp_and_le/examples/conn_24g/ble_24g_client.c \
	../../../../apps/spp_and_le/examples/conn_24g/ble_24g_server.c \
	../../../../apps/spp_and_le/examples/dongle/app_dongle.c \
	../../../../apps/spp_and_le/examples/dongle/ble_dg_central.c \
	../../../../apps/spp_and_le/examples/dongle/edr_hid_user.c \
	../../../../apps/spp_and_le/examples/dongle/ota_dg_central.c \
	../../../../apps/spp_and_le/examples/dongle/usb_hid_devices.c \
	../../../../apps/spp_and_le/examples/dongle/usb_second_hid_devices.c \
	../../../../apps/spp_and_le/examples/findmy/app_fmy.c \
	../../../../apps/spp_and_le/examples/findmy/ble_fmy.c \
	../../../../apps/spp_and_le/examples/findmy/ble_fmy_fmna.c \
	../../../../apps/spp_and_le/examples/findmy/ble_fmy_ota.c \
	../../../../apps/spp_and_le/examples/findmy/ble_fmy_sensor_uart.c \
	../../../../apps/spp_and_le/examples/ftms/app_ftms.c \
	../../../../apps/spp_and_le/examples/ftms/ble_ftms.c \
	../../../../apps/spp_and_le/examples/hilink/app_hilink.c \
	../../../../apps/spp_and_le/examples/hilink/hilink_demo.c \
	../../../../apps/spp_and_le/examples/idle/app_idle.c \
	../../../../apps/spp_and_le/examples/ll_sync/app_ll_sync.c \
	../../../../apps/spp_and_le/examples/ll_sync/ll_sync_demo.c \
	../../../../apps/spp_and_le/examples/multi_conn/app_multi_conn.c \
	../../../../apps/spp_and_le/examples/multi_conn/ble_multi.c \
	../../../../apps/spp_and_le/examples/multi_conn/ble_multi_central.c \
	../../../../apps/spp_and_le/examples/multi_conn/ble_multi_peripheral.c \
	../../../../apps/spp_and_le/examples/nonconn_24g/app_nonconn_24g.c \
	../../../../apps/spp_and_le/examples/nonconn_24g/ble_24g_deal.c \
	../../../../apps/spp_and_le/examples/trans_data/app_spp_and_le.c \
	../../../../apps/spp_and_le/examples/trans_data/ble_trans.c \
	../../../../apps/spp_and_le/examples/trans_data/ble_trans_ios_services.c \
	../../../../apps/spp_and_le/examples/trans_data/ble_trans_search.c \
	../../../../apps/spp_and_le/examples/tuya/app_tuya.c \
	../../../../apps/spp_and_le/examples/tuya/tuya_demo.c \
	../../../../apps/spp_and_le/modules/bt/app_comm_ble.c \
	../../../../apps/spp_and_le/modules/bt/app_comm_edr.c \
	../../../../apps/spp_and_le/modules/bt/edr_emitter.c \
	../../../../apps/spp_and_le/modules/bt/spp_trans.c \
	../../../../apps/spp_and_le/modules/misc.c \
	../../../../apps/spp_and_le/modules/power/app_charge.c \
	../../../../apps/spp_and_le/modules/power/app_chargestore.c \
	../../../../apps/spp_and_le/modules/power/app_handshake.c \
	../../../../apps/spp_and_le/modules/power/app_power_manage.c \
	../../../../apps/spp_and_le/modules/rtc_alarm.c \
	../../../../apps/spp_and_le/modules/user_cfg.c \
	../../../../apps/spp_and_le/version.c \
	../../../../cpu/bd19/adc_api.c \
	../../../../cpu/bd19/audio_codec_clock.c \
	../../../../cpu/bd19/audio_decode/audio_decode.c \
	../../../../cpu/bd19/audio_encode/audio_encode.c \
	../../../../cpu/bd19/audio_pwm.c \
	../../../../cpu/bd19/audio_resample_demo.c \
	../../../../cpu/bd19/charge.c \
	../../../../cpu/bd19/chargestore.c \
	../../../../cpu/bd19/handshake_timer.c \
	../../../../cpu/bd19/iic_hw.c \
	../../../../cpu/bd19/iic_soft.c \
	../../../../cpu/bd19/irflt.c \
	../../../../cpu/bd19/ledc.c \
	../../../../cpu/bd19/ledc_test.c \
	../../../../cpu/bd19/mcpwm.c \
	../../../../cpu/bd19/plcnt.c \
	../../../../cpu/bd19/pwm_led.c \
	../../../../cpu/bd19/setup.c \
	../../../../cpu/bd19/spi.c \
	../../../../cpu/bd19/spi_test.c \
	../../../../cpu/bd19/tone_player.c \
	../../../../cpu/bd19/uart_dev.c \
	../../../../apps/spp_and_le/user_app/user_main.c


# 需要编译的 .S 文件
S_SRC_FILES :=


# 需要编译的 .s 文件
s_SRC_FILES :=


# 需要编译的 .cpp 文件
cpp_SRC_FILES :=


# 需要编译的 .cc 文件
cc_SRC_FILES :=


# 需要编译的 .cxx 文件
cxx_SRC_FILES :=


# 链接参数
LFLAGS := \
	--plugin-opt=-inline-threshold=5 \
	--plugin-opt=save-temps \
	--plugin-opt=-inline-normal-into-special-section=true \
	--plugin-opt=-dont-used-symbol-list=malloc,free,sprintf,printf,puts,putchar \
	--plugin-opt=-warn-stack-size=256 \
	--sort-common \
	--dont-complain-call-overflow \
	--gc-sections \
	--start-group \
	../../../../cpu/bd19/liba/cpu.a \
	../../../../cpu/bd19/liba/system.a \
	../../../../cpu/bd19/liba/btstack.a \
	../../../../cpu/bd19/liba/btctrler.a \
	../../../../cpu/bd19/liba/agreement.a \
	../../../../cpu/bd19/liba/rcsp_stack.a \
	../../../../cpu/bd19/liba/fmna_stack.a \
	../../../../cpu/bd19/liba/fmna_auth.a \
	../../../../cpu/bd19/liba/MotionDetection.a \
	../../../../apps/common/third_party_profile/tuya_protocol/sdk/lib/lib_tuya_v200_Q32S.a \
	../../../../cpu/bd19/liba/media.a \
	../../../../cpu/bd19/liba/lc3_codec_lib.a \
	../../../../cpu/bd19/liba/usbc_codec_lib.a \
	../../../../cpu/bd19/liba/lib_adpcm_ima_code.a \
	../../../../cpu/bd19/liba/wav_dec_lib.a \
	../../../../cpu/bd19/liba/wtgv2_dec_lib.a \
	../../../../cpu/bd19/liba/opus_enc_lib.a \
	../../../../cpu/bd19/liba/msbc_codec_lib.a \
	../../../../cpu/bd19/liba/update.a \
	--end-group \
	-T../../../../cpu/bd19/sdk.ld \
	-M=../../../../cpu/bd19/tools/sdk.map \
	-flto \
	--plugin-opt=-inline-threshold=5 \


LIBPATHS := \
	-L$(SYS_LIB_DIR) \


LIBS := \
	$(SYS_LIB_DIR)/libm.a \
	$(SYS_LIB_DIR)/libc.a \
	$(SYS_LIB_DIR)/libm.a \
	$(SYS_LIB_DIR)/libcompiler-rt.a \



c_OBJS    := $(c_SRC_FILES:%.c=%.c.o)
S_OBJS    := $(S_SRC_FILES:%.S=%.S.o)
s_OBJS    := $(s_SRC_FILES:%.s=%.s.o)
cpp_OBJS  := $(cpp_SRC_FILES:%.cpp=%.cpp.o)
cxx_OBJS  := $(cxx_SRC_FILES:%.cxx=%.cxx.o)
cc_OBJS   := $(cc_SRC_FILES:%.cc=%.cc.o)

OBJS      := $(c_OBJS) $(S_OBJS) $(s_OBJS) $(cpp_OBJS) $(cxx_OBJS) $(cc_OBJS)
DEP_FILES := $(OBJS:%.o=%.d)


OBJS      := $(addprefix $(BUILD_DIR)/, $(OBJS:$(ROOT_PREFIX)/%=%))
DEP_FILES := $(addprefix $(BUILD_DIR)/, $(DEP_FILES:$(ROOT_PREFIX)/%=%))


VERBOSE ?= 0
ifeq ($(VERBOSE), 1)
QUITE :=
else
QUITE := @
endif

# 一些旧的 make 不支持 file 函数，需要 make 的时候指定 LINK_AT=0 make
LINK_AT ?= 1

# 表示下面的不是一个文件的名字，无论是否存在 all, clean, pre_build 这样的文件
# 还是要执行命令
# see: https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all clean pre_build

# 不要使用 make 预设置的规则
# see: https://www.gnu.org/software/make/manual/html_node/Suffix-Rules.html
.SUFFIXES:

all: pre_build $(OUT_ELF)
	$(info +POST-BUILD)
	$(QUITE) $(RUN_POST_SCRIPT) sdk

pre_build:
	$(info +PRE-BUILD)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P ../../../../cpu/bd19/sdk_used_list.c -o ../../../../cpu/bd19/sdk_used_list.used
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P ../../../../cpu/bd19/sdk_ld.c -o ../../../../cpu/bd19/sdk.ld
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P ../../../../cpu/bd19/tools/download.c -o $(POST_SCRIPT)
	$(QUITE) $(FIXBAT) $(POST_SCRIPT)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P ../../../../cpu/bd19/tools/isd_config_rule.c -o ../../../../cpu/bd19/tools/isd_config.ini

clean:
	$(QUITE) $(RM) $(OUT_ELF)
	$(QUITE) $(RM) $(BUILD_DIR)



ifeq ($(LINK_AT), 1)
$(OUT_ELF): $(OBJS)
	$(info +LINK $@)
	$(shell $(MKDIR) $(@D))
	$(file >$(OBJ_FILE), $(OBJS))
	$(QUITE) $(LD) -o $(OUT_ELF) @$(OBJ_FILE) $(LFLAGS) $(LIBPATHS) $(LIBS)
else
$(OUT_ELF): $(OBJS)
	$(info +LINK $@)
	$(shell $(MKDIR) $(@D))
	$(QUITE) $(LD) -o $(OUT_ELF) $(OBJS) $(LFLAGS) $(LIBPATHS) $(LIBS)
endif


$(BUILD_DIR)/%.c.o : $(ROOT_PREFIX)/%.c
	$(info +CC $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.S.o : $(ROOT_PREFIX)/%.S
	$(info +AS $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.s.o : $(ROOT_PREFIX)/%.s
	$(info +AS $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.cpp.o : $(ROOT_PREFIX)/%.cpp
	$(info +CXX $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.cxx.o : $(ROOT_PREFIX)/%.cxx
	$(info +CXX $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.cc.o : $(ROOT_PREFIX)/%.cc
	$(info +CXX $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

-include $(DEP_FILES)
