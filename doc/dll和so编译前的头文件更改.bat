

set SOURCE_PATH=.

set REPACE_FILE=stdafx.h
set SUB_DIR=tools\

echo -                                                                  -
echo -1                                                                 -
echo -                                                                  -
echo --------------copy modbus_ascii_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\modbus\modbus_ascii_master\%SUB_DIR%

echo -                                                                  -
echo -2                                                                 -
echo -                                                                  -
echo --------------copy modbus_ascii_master_special dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\modbus\modbus_ascii_master_special\%SUB_DIR%

echo -                                                                  -
echo -3                                                                 -
echo -                                                                  -
echo --------------copy modbus_rtu_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\modbus\modbus_rtu_master\%SUB_DIR%

echo -                                                                  -
echo -4                                                                 -
echo -                                                                  -
echo --------------copy modbus_rtu_master_special dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\modbus\modbus_rtu_master_special\%SUB_DIR%


echo -                                                                  -
echo -5                                                                 -
echo -                                                                  -
echo --------------copy modbus_rtu_slave dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\modbus\modbus_rtu_slave\%SUB_DIR%

echo -                                                                  -
echo -6                                                                 -
echo -                                                                  -
echo --------------copy modbus_tcpip_rtu_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\modbus\modbus_tcpip_rtu_master\%SUB_DIR%

echo -                                                                  -
echo -7                                                                 -
echo -                                                                  -
echo --------------copy modbus_tcpip_rtu_slave dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\modbus\modbus_tcpip_rtu_slave\%SUB_DIR%

echo -                                                                  -
echo -8                                                                 -
echo -                                                                  -
echo --------------copy aibus_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\aibus\aibus_master\%SUB_DIR%

echo -                                                                  -
echo -9                                                                 -
echo -                                                                  -
echo --------------copy fatek_fb_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\fatek\fatek_fb_master\%SUB_DIR%

echo -                                                                  -
echo -10                                                                -
echo -                                                                  -
echo --------------copy fuji_nb_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\fuji\fuji_nb_master\%SUB_DIR%

echo -                                                                  -
echo -11                                                                -
echo -                                                                  -
echo --------------copy hollysys_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\hollysys\hollysys_master\%SUB_DIR%

echo -                                                                  -
echo -12                                                                -
echo -                                                                  -
echo --------------copy inovance_hxu_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\inovance\inovance_hxu_master\%SUB_DIR%

echo -                                                                  -
echo -13                                                                -
echo -                                                                  -
echo --------------copy lg_k10s1_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\lg\lg_k10s1_master\%SUB_DIR%

echo -                                                                  -
echo -14                                                                -
echo -                                                                  -
echo --------------copy lg_kcnet_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\lg\lg_kcnet_master\%SUB_DIR%

echo -                                                                  -
echo -15                                                                -
echo -                                                                  -
echo --------------copy lg_kloader_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\lg\lg_kloader_master\%SUB_DIR%

echo -                                                                  -
echo -16                                                                -
echo -                                                                  -
echo --------------copy lg_xgb_cnet_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\lg\lg_xgb_cnet_master\%SUB_DIR%

echo -                                                                  -
echo -17                                                                -
echo -                                                                  -
echo --------------copy matsushita_fp_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\matsushita\matsushita_fp_master\%SUB_DIR%

echo -                                                                  -
echo -18                                                                -
echo -                                                                  -
echo --------------copy mitsubishi_fx2n_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\mitsubishi\mitsubishi_fx2n_master\%SUB_DIR%

echo -                                                                  -
echo -19                                                                -
echo -                                                                  -
echo --------------copy mitsubishi_fx3u_fx3g_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\mitsubishi\mitsubishi_fx3u_fx3g_master\%SUB_DIR%

echo -                                                                  -
echo -20                                                                -
echo -                                                                  -
echo --------------copy mitsubishi_fx_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\mitsubishi\mitsubishi_fx_master\%SUB_DIR%

echo -                                                                  -
echo -21                                                                -
echo -                                                                  -
echo --------------copy mitsubishi_q00j_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\mitsubishi\mitsubishi_q00j_master\%SUB_DIR%

echo -                                                                  -
echo -22                                                                -
echo -                                                                  -
echo --------------copy mitsubishi_q_c24n_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\mitsubishi\mitsubishi_q_c24n_master\%SUB_DIR%

echo -                                                                  -
echo -23                                                                -
echo -                                                                  -
echo --------------copy omron_cpm_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\omron\omron_cpm_master\%SUB_DIR%

echo -                                                                  -
echo -24                                                                -
echo -                                                                  -
echo --------------copy omron_cs_cj_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\omron\omron_cs_cj_master\%SUB_DIR%

echo -                                                                  -
echo -25                                                                -
echo -                                                                  -
echo --------------copy siemens_s7_200_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\siemens\siemens_s7_200_master\%SUB_DIR%

echo -                                                                  -
echo -26                                                                -
echo -                                                                  -
echo --------------copy siemens_s7_300_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\siemens\siemens_s7_300_master\%SUB_DIR%

echo -                                                                  -
echo -27                                                                -
echo -                                                                  -
echo --------------copy taiAn_tp03_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\taian\taiAn_tp03_master\%SUB_DIR%

echo -                                                                  -
echo -28                                                                -
echo -                                                                  -
echo --------------copy taiLing_dx_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\taiLing\taiLing_dx_master\%SUB_DIR%

echo -                                                                  -
echo -29                                                                -
echo -                                                                  -
echo --------------copy vigor_electric_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\vigor\vigor_electric_master\%SUB_DIR%

echo -                                                                  -
echo -30                                                                -
echo -                                                                  -
echo --------------copy thinget_xc_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\xinje\thinget_xc_master\%SUB_DIR%

echo -                                                                  -
echo -31                                                                -
echo -                                                                  -
echo --------------copy delta_dvp_master dll---------------------

copy %REPACE_FILE% %SOURCE_PATH%\delta\delta_dvp_master\%SUB_DIR%

echo -                                                                  -
echo -32                                                                -
echo -                                                                  -
echo --------------copy emerson_ec_master dll---------------------

copy %REPACE_FILE% %SOURCE_PATH%\emerson\emerson_ec_master\%SUB_DIR%

echo -                                                                  -
echo -33                                                                -
echo -                                                                  -
echo --------------copy haiwell_es_master dll---------------------

copy %REPACE_FILE% %SOURCE_PATH%\haiwell\haiwell_es_master\%SUB_DIR%

echo -                                                                  -
echo -34                                                                -
echo -                                                                  -
echo --------------copy nanda_na200_master dll---------------------

copy %REPACE_FILE% %SOURCE_PATH%\nanda\nanda_na200_master\%SUB_DIR%

echo -                                                                  -
echo -35                                                                -
echo -                                                                  -
echo --------------copy nanda_na400_master dll---------------------

copy %REPACE_FILE% %SOURCE_PATH%\nanda\nanda_na400_master\%SUB_DIR%

echo -                                                                  -
echo -36                                                                -
echo -                                                                  -
echo --------------copy modicon_m218_master dll---------------------

copy %REPACE_FILE% %SOURCE_PATH%\schneider\modicon_m218_master\%SUB_DIR%

echo -                                                                  -
echo -37                                                                -
echo -                                                                  -
echo --------------copy modicon_twido_master dll---------------------

copy %REPACE_FILE% %SOURCE_PATH%\schneider\modicon_twido_master\%SUB_DIR%

echo -                                                                  -
echo -38                                                                -
echo -                                                                  -
echo --------------copy techwayson_v80_master dll---------------------

copy %REPACE_FILE% %SOURCE_PATH%\techwayson\techwayson_v80_master\%SUB_DIR%

echo -                                                                  -
echo -39                                                                -
echo -                                                                  -
echo --------------copy zhengHang_a5_master dll---------------------

copy %REPACE_FILE% %SOURCE_PATH%\zhenghang\zhengHang_a5_master\%SUB_DIR%

echo -                                                                  -
echo -40                                                                 -
echo -                                                                  -
echo --------------copy modbus_ascii_slave dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\modbus\modbus_ascii_slave\%SUB_DIR%

echo -                                                                  -
echo -41                                                                 -
echo -                                                                  -
echo --------------copy samkoon_fgs_32mr dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\samkoon\samkoon_fgs_32mr\%SUB_DIR%

echo -                                                                  -
echo -42                                                                 -
echo -                                                                  -
echo --------------copy keyence_visual_kv_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\keyence\keyence_visual_kv_master\%SUB_DIR%

echo -                                                                  -
echo -43                                                                 -
echo -                                                                  -
echo --------------copy allen_bradley_master dll---------------------
copy %REPACE_FILE% %SOURCE_PATH%\allen\allen_bradley_master\%SUB_DIR%


pause

