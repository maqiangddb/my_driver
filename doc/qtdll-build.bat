@echo off

echo Setting up a MinGW/Qt only environment...

echo -- QTDIR set to C:\Qt\2010.04\qt

echo -- PATH set to C:\Qt\2010.04\qt\bin

echo -- Adding C:\Qt\2010.04\bin to PATH

echo -- Adding %SystemRoot%\System32 to PATH

echo -- QMAKESPEC set to win32-g++

set PATH=%PATH%;C:\Qt\2010.04\bin;C:\Qt\2010.04\mingw\bin

set PATH=%PATH%;%SystemRoot%\System32

set QMAKESPEC=win32-g++

set SOURCE_PATH=E:\AndroidProject\sd_doc\plc_drives_doc

set TARGET_PATH=E:\AndroidProject\sd_doc\上位打包\Drivers\qtdll

set BUIL_CMD=mingw32-make release

echo -                                                                  -
echo -1                                                                 -
echo -                                                                  -
echo --------------build modbus_ascii_master dll---------------------
cd %SOURCE_PATH%\modbus\modbus_ascii_master
qmake
%BUIL_CMD%
copy %SOURCE_PATH%\modbus\modbus_ascii_master\release\modbus_ascii_master.dll %TARGET_PATH%\modbus\

echo -                                                                  -
echo -2                                                                 -
echo -                                                                  -
echo --------------build modbus_ascii_master_special dll---------------------
cd %SOURCE_PATH%\modbus\modbus_ascii_master_special
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\modbus\modbus_ascii_master_special\release\modbus_ascii_master_special.dll %TARGET_PATH%\modbus\

echo -                                                                  -
echo -3                                                                 -
echo -                                                                  -
echo --------------build modbus_rtu_master dll---------------------
cd %SOURCE_PATH%\modbus\modbus_rtu_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\modbus\modbus_rtu_master\release\modbus_rtu_master.dll %TARGET_PATH%\modbus\

echo -                                                                  -
echo -4                                                                 -
echo -                                                                  -
echo --------------build modbus_rtu_master_special dll---------------------
cd %SOURCE_PATH%\modbus\modbus_rtu_master_special
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\modbus\modbus_rtu_master_special\release\modbus_rtu_master_special.dll %TARGET_PATH%\modbus\

echo -                                                                  -
echo -5                                                                 -
echo -                                                                  -
echo --------------build modbus_rtu_slave dll---------------------
cd %SOURCE_PATH%\modbus\modbus_rtu_slave
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\modbus\modbus_rtu_slave\release\modbus_rtu_slave.dll %TARGET_PATH%\modbus\

echo -                                                                  -
echo -6                                                                 -
echo -                                                                  -
echo --------------build modbus_tcpip_rtu_master dll---------------------
cd %SOURCE_PATH%\modbus\modbus_tcpip_rtu_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\modbus\modbus_tcpip_rtu_master\release\modbus_tcpip_rtu_master.dll %TARGET_PATH%\modbus\

echo -                                                                  -
echo -7                                                                 -
echo -                                                                  -
echo --------------build modbus_tcpip_rtu_slave dll---------------------
cd %SOURCE_PATH%\modbus\modbus_tcpip_rtu_slave
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\modbus\modbus_tcpip_rtu_slave\release\modbus_tcpip_rtu_slave.dll %TARGET_PATH%\modbus\

echo -                                                                  -
echo -8                                                                 -
echo -                                                                  -
echo --------------build aibus_master dll---------------------
cd %SOURCE_PATH%\aibus\aibus_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\aibus\aibus_master\release\aibus_master.dll %TARGET_PATH%\aibus\

echo -                                                                  -
echo -9                                                                 -
echo -                                                                  -
echo --------------build fatek_fb_master dll---------------------
cd %SOURCE_PATH%\fatek\fatek_fb_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\fatek\fatek_fb_master\release\fatek_fb_master.dll %TARGET_PATH%\fatek\

echo -                                                                  -
echo -10                                                                -
echo -                                                                  -
echo --------------build fuji_nb_master dll---------------------
cd %SOURCE_PATH%\fuji\fuji_nb_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\fuji\fuji_nb_master\release\fuji_nb_master.dll %TARGET_PATH%\fuji\

echo -                                                                  -
echo -11                                                                -
echo -                                                                  -
echo --------------build hollysys_master dll---------------------
cd %SOURCE_PATH%\hollysys\hollysys_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\hollysys\hollysys_master\release\hollysys_master.dll %TARGET_PATH%\hollysys\

echo -                                                                  -
echo -12                                                                -
echo -                                                                  -
echo --------------build inovance_hxu_master dll---------------------
cd %SOURCE_PATH%\inovance\inovance_hxu_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\inovance\inovance_hxu_master\release\inovance_hxu_master.dll %TARGET_PATH%\inovance\

echo -                                                                  -
echo -13                                                                -
echo -                                                                  -
echo --------------build lg_k10s1_master dll---------------------
cd %SOURCE_PATH%\lg\lg_k10s1_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\lg\lg_k10s1_master\release\lg_k10s1_master.dll %TARGET_PATH%\lg\

echo -                                                                  -
echo -14                                                                -
echo -                                                                  -
echo --------------build lg_kcnet_master dll---------------------
cd %SOURCE_PATH%\lg\lg_kcnet_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\lg\lg_kcnet_master\release\lg_kcnet_master.dll %TARGET_PATH%\lg\

echo -                                                                  -
echo -15                                                                -
echo -                                                                  -
echo --------------build lg_kloader_master dll---------------------
cd %SOURCE_PATH%\lg\lg_kloader_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\lg\lg_kloader_master\release\lg_kloader_master.dll %TARGET_PATH%\lg\

echo -                                                                  -
echo -16                                                                -
echo -                                                                  -
echo --------------build lg_xgb_cnet_master dll---------------------
cd %SOURCE_PATH%\lg\lg_xgb_cnet_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\lg\lg_xgb_cnet_master\release\lg_xgb_cnet_master.dll %TARGET_PATH%\lg\

echo -                                                                  -
echo -17                                                                -
echo -                                                                  -
echo --------------build matsushita_fp_master dll---------------------
cd %SOURCE_PATH%\matsushita\matsushita_fp_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\matsushita\matsushita_fp_master\release\matsushita_fp_master.dll %TARGET_PATH%\matsushita\

echo -                                                                  -
echo -18                                                                -
echo -                                                                  -
echo --------------build mitsubishi_fx2n_master dll---------------------
cd %SOURCE_PATH%\mitsubishi\mitsubishi_fx2n_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\mitsubishi\mitsubishi_fx2n_master\release\mitsubishi_fx2n_master.dll %TARGET_PATH%\mitsubishi\

echo -                                                                  -
echo -19                                                                -
echo -                                                                  -
echo --------------build mitsubishi_fx3u_fx3g_master dll---------------------
cd %SOURCE_PATH%\mitsubishi\mitsubishi_fx3u_fx3g_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\mitsubishi\mitsubishi_fx3u_fx3g_master\release\mitsubishi_fx3u_fx3g_master.dll %TARGET_PATH%\mitsubishi\

echo -                                                                  -
echo -20                                                                -
echo -                                                                  -
echo --------------build mitsubishi_fx_master dll---------------------
cd %SOURCE_PATH%\mitsubishi\mitsubishi_fx_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\mitsubishi\mitsubishi_fx_master\release\mitsubishi_fx_master.dll %TARGET_PATH%\mitsubishi\

echo -                                                                  -
echo -21                                                                -
echo -                                                                  -
echo --------------build mitsubishi_q00j_master dll---------------------
cd %SOURCE_PATH%\mitsubishi\mitsubishi_q00j_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\mitsubishi\mitsubishi_q00j_master\release\mitsubishi_q00j_master.dll %TARGET_PATH%\mitsubishi\

echo -                                                                  -
echo -22                                                                -
echo -                                                                  -
echo --------------build mitsubishi_q_c24n_master dll---------------------
cd %SOURCE_PATH%\mitsubishi\mitsubishi_q_c24n_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\mitsubishi\mitsubishi_q_c24n_master\release\mitsubishi_q_c24n_master.dll %TARGET_PATH%\mitsubishi\

echo -                                                                  -
echo -23                                                                -
echo -                                                                  -
echo --------------build omron_cpm_master dll---------------------
cd %SOURCE_PATH%\omron\omron_cpm_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\omron\omron_cpm_master\release\omron_cpm_master.dll %TARGET_PATH%\omron\

echo -                                                                  -
echo -24                                                                -
echo -                                                                  -
echo --------------build omron_cs_cj_master dll---------------------
cd %SOURCE_PATH%\omron\omron_cs_cj_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\omron\omron_cs_cj_master\release\omron_cs_cj_master.dll %TARGET_PATH%\omron\

echo -                                                                  -
echo -25                                                                -
echo -                                                                  -
echo --------------build siemens_s7_200_master dll---------------------
cd %SOURCE_PATH%\siemens\siemens_s7_200_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\siemens\siemens_s7_200_master\release\siemens_s7_200_master.dll %TARGET_PATH%\siemens\

echo -                                                                  -
echo -26                                                                -
echo -                                                                  -
echo --------------build siemens_s7_300_master dll---------------------
cd %SOURCE_PATH%\siemens\siemens_s7_300_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\siemens\siemens_s7_300_master\release\siemens_s7_300_master.dll %TARGET_PATH%\siemens\

echo -                                                                  -
echo -27                                                                -
echo -                                                                  -
echo --------------build taiAn_tp03_master dll---------------------
cd %SOURCE_PATH%\taian\taiAn_tp03_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\taian\taiAn_tp03_master\release\taiAn_tp03_master.dll %TARGET_PATH%\taian\

echo -                                                                  -
echo -28                                                                -
echo -                                                                  -
echo --------------build taiLing_dx_master dll---------------------
cd %SOURCE_PATH%\taiLing\taiLing_dx_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\taiLing\taiLing_dx_master\release\taiLing_dx_master.dll %TARGET_PATH%\taiLing\

echo -                                                                  -
echo -29                                                                -
echo -                                                                  -
echo --------------build vigor_electric_master dll---------------------
cd %SOURCE_PATH%\vigor\vigor_electric_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\vigor\vigor_electric_master\release\vigor_electric_master.dll %TARGET_PATH%\vigor\

echo -                                                                  -
echo -30                                                                -
echo -                                                                  -
echo --------------build thinget_xc_master dll---------------------
cd %SOURCE_PATH%\xinje\thinget_xc_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\xinje\thinget_xc_master\release\thinget_xc_master.dll %TARGET_PATH%\xinje\

echo -                                                                  -
echo -31                                                                -
echo -                                                                  -
echo --------------build delta_dvp_master dll---------------------
cd %SOURCE_PATH%\delta\delta_dvp_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\delta\delta_dvp_master\release\delta_dvp_master.dll %TARGET_PATH%\delta\

echo -                                                                  -
echo -32                                                                -
echo -                                                                  -
echo --------------build emerson_ec_master dll---------------------
cd %SOURCE_PATH%\emerson\emerson_ec_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\emerson\emerson_ec_master\release\emerson_ec_master.dll %TARGET_PATH%\emerson\

echo -                                                                  -
echo -33                                                                -
echo -                                                                  -
echo --------------build haiwell_es_master dll---------------------
cd %SOURCE_PATH%\haiwell\haiwell_es_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\haiwell\haiwell_es_master\release\haiwell_es_master.dll %TARGET_PATH%\haiwell\

echo -                                                                  -
echo -34                                                                -
echo -                                                                  -
echo --------------build nanda_na200_master dll---------------------
cd %SOURCE_PATH%\nanda\nanda_na200_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\nanda\nanda_na200_master\release\nanda_na200_master.dll %TARGET_PATH%\nanda\

echo -                                                                  -
echo -35                                                                -
echo -                                                                  -
echo --------------build nanda_na400_master dll---------------------
cd %SOURCE_PATH%\nanda\nanda_na400_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\nanda\nanda_na400_master\release\nanda_na400_master.dll %TARGET_PATH%\nanda\

echo -                                                                  -
echo -36                                                                -
echo -                                                                  -
echo --------------build modicon_m218_master dll---------------------
cd %SOURCE_PATH%\schneider\modicon_m218_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\schneider\modicon_m218_master\release\modicon_m218_master.dll %TARGET_PATH%\schneider\

echo -                                                                  -
echo -37                                                                -
echo -                                                                  -
echo --------------build modicon_twido_master dll---------------------
cd %SOURCE_PATH%\schneider\modicon_twido_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\schneider\modicon_twido_master\release\modicon_twido_master.dll %TARGET_PATH%\schneider\

echo -                                                                  -
echo -38                                                                -
echo -                                                                  -
echo --------------build techwayson_v80_master dll---------------------
cd %SOURCE_PATH%\techwayson\techwayson_v80_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\techwayson\techwayson_v80_master\release\techwayson_v80_master.dll %TARGET_PATH%\techwayson\

echo -                                                                  -
echo -39                                                                -
echo -                                                                  -
echo --------------build zhengHang_a5_master dll---------------------
cd %SOURCE_PATH%\zhenghang\zhengHang_a5_master
qmake
%BUIL_CMD%

copy %SOURCE_PATH%\zhenghang\zhengHang_a5_master\release\zhengHang_a5_master.dll %TARGET_PATH%\zhenghang\

echo -                                                                  -
echo -40                                                                 -
echo -                                                                  -
echo --------------build modbus_ascii_slave dll---------------------
cd %SOURCE_PATH%\modbus\modbus_ascii_slave
qmake
%BUIL_CMD%
copy %SOURCE_PATH%\modbus\modbus_ascii_slave\release\modbus_ascii_slave.dll %TARGET_PATH%\modbus\

echo -                                                                  -
echo -41                                                                 -
echo -                                                                  -
echo --------------build samkoon_fgs_32mr dll---------------------
cd %SOURCE_PATH%\samkoon\samkoon_fgs_32mr
qmake
%BUIL_CMD%
copy %SOURCE_PATH%\samkoon\samkoon_fgs_32mr\release\samkoon_fgs_32mr.dll %TARGET_PATH%\samkoon\


echo -                                                                  -
echo -42                                                                 -
echo -                                                                  -
echo --------------build keyence_visual_kv_master dll---------------------
cd %SOURCE_PATH%\keyence\keyence_visual_kv_master
qmake
%BUIL_CMD%
copy %SOURCE_PATH%\keyence\keyence_visual_kv_master\release\keyence_visual_kv_master.dll %TARGET_PATH%\keyence\

echo -                                                                  -
echo -43                                                                 -
echo -                                                                  -
echo --------------build allen_bradley_master dll---------------------
cd %SOURCE_PATH%\allen\allen_bradley_master
qmake
%BUIL_CMD%
copy %SOURCE_PATH%\allen\allen_bradley_master\release\allen_bradley_master.dll %TARGET_PATH%\allen\


pause

