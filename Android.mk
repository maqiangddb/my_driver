
LOCAL_PATH := $(call my-dir)

######## plc_drives_center ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := plc_drives_center
LOCAL_SRC_FILES := plc_drives_center/addrmanage.cpp \
                   plc_drives_center/androidjniinterface.cpp \
                   plc_drives_center/managecenter.cpp \
                   plc_drives_center/openlibinterface.cpp \
                   plc_drives_center/localaddrs.cpp \
                   plc_drives_center/dealjnimethod.cpp \
                   plc_drives_center/tools/dealwithstring.cpp \
                   plc_drives_center/tools/plcmathmethod.cpp 
                   
LOCAL_LDLIBS    := -llog

#LOCAL_LDLIBS    += -fPIC \
#				   -ldl
#LOCAL_SHARED_LIBRARIES := libdl

include $(BUILD_SHARED_LIBRARY)

######## 1. modbus_ascii_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := modbus_ascii_master
LOCAL_SRC_FILES := modbus/modbus_ascii_master/driveinterface.cpp \
                   modbus/modbus_ascii_master/drive/datacenter.cpp \
                   modbus/modbus_ascii_master/drive/plcprotocol.cpp \
                   modbus/modbus_ascii_master/tools/dealwithstring.cpp \
                   modbus/modbus_ascii_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 2. modbus_ascii_master_special ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := modbus_ascii_master_special
LOCAL_SRC_FILES := modbus/modbus_ascii_master_special/driveinterface.cpp \
                   modbus/modbus_ascii_master_special/drive/datacenter.cpp \
                   modbus/modbus_ascii_master_special/drive/plcprotocol.cpp \
                   modbus/modbus_ascii_master_special/tools/dealwithstring.cpp \
                   modbus/modbus_ascii_master_special/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 3. modbus_rtu_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := modbus_rtu_master
LOCAL_SRC_FILES := modbus/modbus_rtu_master/driveinterface.cpp \
                   modbus/modbus_rtu_master/drive/datacenter.cpp \
                   modbus/modbus_rtu_master/drive/plcprotocol.cpp \
                   modbus/modbus_rtu_master/tools/dealwithstring.cpp \
                   modbus/modbus_rtu_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 4. modbus_rtu_master_special ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := modbus_rtu_master_special
LOCAL_SRC_FILES := modbus/modbus_rtu_master_special/driveinterface.cpp \
                   modbus/modbus_rtu_master_special/drive/datacenter.cpp \
                   modbus/modbus_rtu_master_special/drive/plcprotocol.cpp \
                   modbus/modbus_rtu_master_special/tools/dealwithstring.cpp \
                   modbus/modbus_rtu_master_special/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 5. modbus_rtu_slave ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := modbus_rtu_slave
LOCAL_SRC_FILES := modbus/modbus_rtu_slave/driveinterface.cpp \
                   modbus/modbus_rtu_slave/drive/datacenter.cpp \
                   modbus/modbus_rtu_slave/drive/plcprotocol.cpp \
                   modbus/modbus_rtu_slave/tools/dealwithstring.cpp \
                   modbus/modbus_rtu_slave/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 6. modbus_tcpip_rtu_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := modbus_tcpip_rtu_master
LOCAL_SRC_FILES := modbus/modbus_tcpip_rtu_master/driveinterface.cpp \
                   modbus/modbus_tcpip_rtu_master/drive/datacenter.cpp \
                   modbus/modbus_tcpip_rtu_master/drive/plcprotocol.cpp \
                   modbus/modbus_tcpip_rtu_master/tools/dealwithstring.cpp \
                   modbus/modbus_tcpip_rtu_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 7. modbus_tcpip_rtu_slave ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := modbus_tcpip_rtu_slave
LOCAL_SRC_FILES := modbus/modbus_tcpip_rtu_slave/driveinterface.cpp \
                   modbus/modbus_tcpip_rtu_slave/drive/datacenter.cpp \
                   modbus/modbus_tcpip_rtu_slave/drive/plcprotocol.cpp \
                   modbus/modbus_tcpip_rtu_slave/tools/dealwithstring.cpp \
                   modbus/modbus_tcpip_rtu_slave/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 8. aibus_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := aibus_master
LOCAL_SRC_FILES := aibus/aibus_master/driveinterface.cpp \
                   aibus/aibus_master/drive/datacenter.cpp \
                   aibus/aibus_master/drive/plcprotocol.cpp \
                   aibus/aibus_master/tools/dealwithstring.cpp \
                   aibus/aibus_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 9. fatek_fb_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := fatek_fb_master
LOCAL_SRC_FILES := fatek/fatek_fb_master/driveinterface.cpp \
                   fatek/fatek_fb_master/drive/datacenter.cpp \
                   fatek/fatek_fb_master/drive/plcprotocol.cpp \
                   fatek/fatek_fb_master/tools/dealwithstring.cpp \
                   fatek/fatek_fb_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 10. fuji_nb_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := fuji_nb_master
LOCAL_SRC_FILES := fuji/fuji_nb_master/driveinterface.cpp \
                   fuji/fuji_nb_master/drive/datacenter.cpp \
                   fuji/fuji_nb_master/drive/plcprotocol.cpp \
                   fuji/fuji_nb_master/tools/dealwithstring.cpp \
                   fuji/fuji_nb_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 11. hollysys_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := hollysys_master
LOCAL_SRC_FILES := hollysys/hollysys_master/driveinterface.cpp \
                   hollysys/hollysys_master/drive/datacenter.cpp \
                   hollysys/hollysys_master/drive/plcprotocol.cpp \
                   hollysys/hollysys_master/tools/dealwithstring.cpp \
                   hollysys/hollysys_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 12. inovance_hxu_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := inovance_hxu_master
LOCAL_SRC_FILES := inovance/inovance_hxu_master/driveinterface.cpp \
                   inovance/inovance_hxu_master/drive/datacenter.cpp \
                   inovance/inovance_hxu_master/drive/plcprotocol.cpp \
                   inovance/inovance_hxu_master/tools/dealwithstring.cpp \
                   inovance/inovance_hxu_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 13. lg_k10s1_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := lg_k10s1_master
LOCAL_SRC_FILES := lg/lg_k10s1_master/driveinterface.cpp \
                   lg/lg_k10s1_master/drive/datacenter.cpp \
                   lg/lg_k10s1_master/drive/plcprotocol.cpp \
                   lg/lg_k10s1_master/tools/dealwithstring.cpp \
                   lg/lg_k10s1_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 14. lg_kcnet_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := lg_kcnet_master
LOCAL_SRC_FILES := lg/lg_kcnet_master/driveinterface.cpp \
                   lg/lg_kcnet_master/drive/datacenter.cpp \
                   lg/lg_kcnet_master/drive/plcprotocol.cpp \
                   lg/lg_kcnet_master/tools/dealwithstring.cpp \
                   lg/lg_kcnet_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 15. lg_kloader_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := lg_kloader_master
LOCAL_SRC_FILES := lg/lg_kloader_master/driveinterface.cpp \
                   lg/lg_kloader_master/drive/datacenter.cpp \
                   lg/lg_kloader_master/drive/plcprotocol.cpp \
                   lg/lg_kloader_master/tools/dealwithstring.cpp \
                   lg/lg_kloader_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 16. lg_xgb_cnet_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := lg_xgb_cnet_master
LOCAL_SRC_FILES := lg/lg_xgb_cnet_master/driveinterface.cpp \
                   lg/lg_xgb_cnet_master/drive/datacenter.cpp \
                   lg/lg_xgb_cnet_master/drive/plcprotocol.cpp \
                   lg/lg_xgb_cnet_master/tools/dealwithstring.cpp \
                   lg/lg_xgb_cnet_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 17. matsushita_fp_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := matsushita_fp_master
LOCAL_SRC_FILES := matsushita/matsushita_fp_master/driveinterface.cpp \
                   matsushita/matsushita_fp_master/drive/datacenter.cpp \
                   matsushita/matsushita_fp_master/drive/plcprotocol.cpp \
                   matsushita/matsushita_fp_master/tools/dealwithstring.cpp \
                   matsushita/matsushita_fp_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 18. mitsubishi_fx2n_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := mitsubishi_fx2n_master
LOCAL_SRC_FILES := mitsubishi/mitsubishi_fx2n_master/driveinterface.cpp \
                   mitsubishi/mitsubishi_fx2n_master/drive/datacenter.cpp \
                   mitsubishi/mitsubishi_fx2n_master/drive/plcprotocol.cpp \
                   mitsubishi/mitsubishi_fx2n_master/tools/dealwithstring.cpp \
                   mitsubishi/mitsubishi_fx2n_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 19. mitsubishi_fx3u_fx3g_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := mitsubishi_fx3u_fx3g_master
LOCAL_SRC_FILES := mitsubishi/mitsubishi_fx3u_fx3g_master/driveinterface.cpp \
                   mitsubishi/mitsubishi_fx3u_fx3g_master/drive/datacenter.cpp \
                   mitsubishi/mitsubishi_fx3u_fx3g_master/drive/plcprotocol.cpp \
                   mitsubishi/mitsubishi_fx3u_fx3g_master/tools/dealwithstring.cpp \
                   mitsubishi/mitsubishi_fx3u_fx3g_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 20. mitsubishi_fx_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := mitsubishi_fx_master
LOCAL_SRC_FILES := mitsubishi/mitsubishi_fx_master/driveinterface.cpp \
                   mitsubishi/mitsubishi_fx_master/drive/datacenter.cpp \
                   mitsubishi/mitsubishi_fx_master/drive/plcprotocol.cpp \
                   mitsubishi/mitsubishi_fx_master/tools/dealwithstring.cpp \
                   mitsubishi/mitsubishi_fx_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 21. mitsubishi_q00j_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := mitsubishi_q00j_master
LOCAL_SRC_FILES := mitsubishi/mitsubishi_q00j_master/driveinterface.cpp \
                   mitsubishi/mitsubishi_q00j_master/drive/datacenter.cpp \
                   mitsubishi/mitsubishi_q00j_master/drive/plcprotocol.cpp \
                   mitsubishi/mitsubishi_q00j_master/tools/dealwithstring.cpp \
                   mitsubishi/mitsubishi_q00j_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 22. mitsubishi_q_c24n_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := mitsubishi_q_c24n_master
LOCAL_SRC_FILES := mitsubishi/mitsubishi_q_c24n_master/driveinterface.cpp \
                   mitsubishi/mitsubishi_q_c24n_master/drive/datacenter.cpp \
                   mitsubishi/mitsubishi_q_c24n_master/drive/plcprotocol.cpp \
                   mitsubishi/mitsubishi_q_c24n_master/tools/dealwithstring.cpp \
                   mitsubishi/mitsubishi_q_c24n_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 23. omron_cpm_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := omron_cpm_master
LOCAL_SRC_FILES := omron/omron_cpm_master/driveinterface.cpp \
                   omron/omron_cpm_master/drive/datacenter.cpp \
                   omron/omron_cpm_master/drive/plcprotocol.cpp \
                   omron/omron_cpm_master/tools/dealwithstring.cpp \
                   omron/omron_cpm_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 24. omron_cs_cj_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := omron_cs_cj_master
LOCAL_SRC_FILES := omron/omron_cs_cj_master/driveinterface.cpp \
                   omron/omron_cs_cj_master/drive/datacenter.cpp \
                   omron/omron_cs_cj_master/drive/plcprotocol.cpp \
                   omron/omron_cs_cj_master/tools/dealwithstring.cpp \
                   omron/omron_cs_cj_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 25. siemens_s7_200_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := siemens_s7_200_master
LOCAL_SRC_FILES := siemens/siemens_s7_200_master/driveinterface.cpp \
                   siemens/siemens_s7_200_master/drive/datacenter.cpp \
                   siemens/siemens_s7_200_master/drive/plcprotocol.cpp \
                   siemens/siemens_s7_200_master/tools/dealwithstring.cpp \
                   siemens/siemens_s7_200_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 26. siemens_s7_300_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := siemens_s7_300_master
LOCAL_SRC_FILES := siemens/siemens_s7_300_master/driveinterface.cpp \
                   siemens/siemens_s7_300_master/drive/datacenter.cpp \
                   siemens/siemens_s7_300_master/drive/plcprotocol.cpp \
                   siemens/siemens_s7_300_master/tools/dealwithstring.cpp \
                   siemens/siemens_s7_300_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 27. taiAn_tp03_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := taiAn_tp03_master
LOCAL_SRC_FILES := taian/taiAn_tp03_master/driveinterface.cpp \
                   taian/taiAn_tp03_master/drive/datacenter.cpp \
                   taian/taiAn_tp03_master/drive/plcprotocol.cpp \
                   taian/taiAn_tp03_master/tools/dealwithstring.cpp \
                   taian/taiAn_tp03_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 28. taiLing_dx_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := taiLing_dx_master
LOCAL_SRC_FILES := taiLing/taiLing_dx_master/driveinterface.cpp \
                   taiLing/taiLing_dx_master/drive/datacenter.cpp \
                   taiLing/taiLing_dx_master/drive/plcprotocol.cpp \
                   taiLing/taiLing_dx_master/tools/dealwithstring.cpp \
                   taiLing/taiLing_dx_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 29. vigor_electric_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := vigor_electric_master
LOCAL_SRC_FILES := vigor/vigor_electric_master/driveinterface.cpp \
                   vigor/vigor_electric_master/drive/datacenter.cpp \
                   vigor/vigor_electric_master/drive/plcprotocol.cpp \
                   vigor/vigor_electric_master/tools/dealwithstring.cpp \
                   vigor/vigor_electric_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 30. thinget_xc_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := thinget_xc_master
LOCAL_SRC_FILES := xinje/thinget_xc_master/driveinterface.cpp \
                   xinje/thinget_xc_master/drive/datacenter.cpp \
                   xinje/thinget_xc_master/drive/plcprotocol.cpp \
                   xinje/thinget_xc_master/tools/dealwithstring.cpp \
                   xinje/thinget_xc_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 31. delta_dvp_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := delta_dvp_master
LOCAL_SRC_FILES := delta/delta_dvp_master/driveinterface.cpp \
                   delta/delta_dvp_master/drive/datacenter.cpp \
                   delta/delta_dvp_master/drive/plcprotocol.cpp \
                   delta/delta_dvp_master/tools/dealwithstring.cpp \
                   delta/delta_dvp_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 32. emerson_ec_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := emerson_ec_master
LOCAL_SRC_FILES := emerson/emerson_ec_master/driveinterface.cpp \
                   emerson/emerson_ec_master/drive/datacenter.cpp \
                   emerson/emerson_ec_master/drive/plcprotocol.cpp \
                   emerson/emerson_ec_master/tools/dealwithstring.cpp \
                   emerson/emerson_ec_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 33. haiwell_es_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := haiwell_es_master
LOCAL_SRC_FILES := haiwell/haiwell_es_master/driveinterface.cpp \
                   haiwell/haiwell_es_master/drive/datacenter.cpp \
                   haiwell/haiwell_es_master/drive/plcprotocol.cpp \
                   haiwell/haiwell_es_master/tools/dealwithstring.cpp \
                   haiwell/haiwell_es_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 34. nanda_na200_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := nanda_na200_master
LOCAL_SRC_FILES := nanda/nanda_na200_master/driveinterface.cpp \
                   nanda/nanda_na200_master/drive/datacenter.cpp \
                   nanda/nanda_na200_master/drive/plcprotocol.cpp \
                   nanda/nanda_na200_master/tools/dealwithstring.cpp \
                   nanda/nanda_na200_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 35. nanda_na400_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := nanda_na400_master
LOCAL_SRC_FILES := nanda/nanda_na400_master/driveinterface.cpp \
                   nanda/nanda_na400_master/drive/datacenter.cpp \
                   nanda/nanda_na400_master/drive/plcprotocol.cpp \
                   nanda/nanda_na400_master/tools/dealwithstring.cpp \
                   nanda/nanda_na400_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 36. modicon_m218_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := modicon_m218_master
LOCAL_SRC_FILES := schneider/modicon_m218_master/driveinterface.cpp \
                   schneider/modicon_m218_master/drive/datacenter.cpp \
                   schneider/modicon_m218_master/drive/plcprotocol.cpp \
                   schneider/modicon_m218_master/tools/dealwithstring.cpp \
                   schneider/modicon_m218_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 37. modicon_twido_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := modicon_twido_master
LOCAL_SRC_FILES := schneider/modicon_twido_master/driveinterface.cpp \
                   schneider/modicon_twido_master/drive/datacenter.cpp \
                   schneider/modicon_twido_master/drive/plcprotocol.cpp \
                   schneider/modicon_twido_master/tools/dealwithstring.cpp \
                   schneider/modicon_twido_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 38. techwayson_v80_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := techwayson_v80_master
LOCAL_SRC_FILES := techwayson/techwayson_v80_master/driveinterface.cpp \
                   techwayson/techwayson_v80_master/drive/datacenter.cpp \
                   techwayson/techwayson_v80_master/drive/plcprotocol.cpp \
                   techwayson/techwayson_v80_master/tools/dealwithstring.cpp \
                   techwayson/techwayson_v80_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 39. zhengHang_a5_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := zhengHang_a5_master
LOCAL_SRC_FILES := zhenghang/zhengHang_a5_master/driveinterface.cpp \
                   zhenghang/zhengHang_a5_master/drive/datacenter.cpp \
                   zhenghang/zhengHang_a5_master/drive/plcprotocol.cpp \
                   zhenghang/zhengHang_a5_master/tools/dealwithstring.cpp \
                   zhenghang/zhengHang_a5_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 40. modbus_ascii_slave ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := modbus_ascii_slave
LOCAL_SRC_FILES := modbus/modbus_ascii_slave/driveinterface.cpp \
                   modbus/modbus_ascii_slave/drive/datacenter.cpp \
                   modbus/modbus_ascii_slave/drive/plcprotocol.cpp \
                   modbus/modbus_ascii_slave/tools/dealwithstring.cpp \
                   modbus/modbus_ascii_slave/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 41. samkoon_fgs_32mr ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := samkoon_fgs_32mr
LOCAL_SRC_FILES := samkoon/samkoon_fgs_32mr/driveinterface.cpp \
                   samkoon/samkoon_fgs_32mr/drive/datacenter.cpp \
                   samkoon/samkoon_fgs_32mr/drive/plcprotocol.cpp \
                   samkoon/samkoon_fgs_32mr/tools/dealwithstring.cpp \
                   samkoon/samkoon_fgs_32mr/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 42. keyence_visual_kv_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := keyence_visual_kv_master
LOCAL_SRC_FILES := keyence/keyence_visual_kv_master/driveinterface.cpp \
                   keyence/keyence_visual_kv_master/drive/datacenter.cpp \
                   keyence/keyence_visual_kv_master/drive/plcprotocol.cpp \
                   keyence/keyence_visual_kv_master/tools/dealwithstring.cpp \
                   keyence/keyence_visual_kv_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)

######## 43. allen_bradley_master ##########
include $(CLEAR_VARS)

TARGET_PLATFORM := android-3
LOCAL_MODULE    := allen_bradley_master
LOCAL_SRC_FILES := allen/allen_bradley_master/driveinterface.cpp \
                   allen/allen_bradley_master/drive/datacenter.cpp \
                   allen/allen_bradley_master/drive/plcprotocol.cpp \
                   allen/allen_bradley_master/tools/dealwithstring.cpp \
                   allen/allen_bradley_master/tools/plcmathmethod.cpp

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)
