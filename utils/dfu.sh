#!/usr/bin/bash
# Use -gt 1 to consume two arguments per pass in the loop (e.g. each
# argument has a corresponding value to go with it).
# Use -gt 0 to consume one or more arguments per pass in the loop (e.g.
# some arguments don't have a corresponding value to go with it such
# as in the --default example).
# note: if this is set to -gt 0 the /etc/hosts part is not recognized ( may be a bug )
BOARD='pca10040'
PRJ='ble_app_hrs_freertos'
RT_PATH='../..'
while [[ $# -gt 0 ]]
do
key="$1"
DFU_PATH="$RT_PATH/projs/dfu"
SD_PATH="$RT_PATH/components/softdevice/s132/hex"
APP_PATH="$RT_PATH/projs/ble_peripheral/$PRJ/$BOARD/s132/armgcc"
BL_PATH="$DFU_PATH/bootloader_secure/$BOARD/armgcc/_build"
case $key in
-d|dfu)
    echo "******packing dfu*******"
    #nrfutil pkg generate --application nrf52832_xxaa.hex --key-file private.key app_dfu_package.zip
    ##sd 3.0
    #nrfutil pkg generate --hw-version 52 --application-version 4 --application $APP_PATH/_build/nrf52832_xxaa.hex --sd-req 0x8C --key-file private.key app_dfu_package.zip
    ##sd5.0
    nrfutil pkg generate --hw-version 52 --application-version 4 --application $APP_PATH/_build/nrf52832_xxaa.hex --sd-req 0x8C --key-file private.key app_dfu_package.zip
    ;;

-h|hex)
    echo "****mergering hex****"
    cp -r $BL_PATH/nrf52832_xxaa_s132.hex bootloader.hex
    mergehex -m $SD_PATH/s132_nrf52_3.0.0_softdevice.hex bootloader.hex -o sd_bl.hex
    mergehex -m sd_bl.hex $APP_PATH/_build/nrf52832_xxaa.hex -o sd_bl_app.hex 
    mergehex -m sd_bl_app.hex bootloader_settings.hex -o sd_bl_app_cfg.hex 
    #shift # past argument
    ;;
-f|flash)
    nrfjprog --program sd_bl_app_cfg.hex -f nrf52 --chiperase
    nrfjprog --reset -f nrf52
    ;;

-b|bootloader)
    echo "****building bootloader****"
    cp public_key.c $DFU_PATH/bootloader_secure/dfu_public_key.c
    pushd $DFU_PATH/bootloader_secure/$BOARD/armgcc && make && popd
    ;;
-a|app)
    echo "****building app****"
    pushd $APP_PATH && make && popd
    ;;
setting)
    echo "****setting****"
    nrfutil settings generate --family NRF52 --application $APP_PATH/_build/nrf52832_xxaa.hex --application-version 3 --bootloader-version 1 --bl-settings-version 1 bootloader_settings.hex
    ;;

gkey)
    echo "****generating key****"
    nrfutil keys generate private.key
    nrfutil keys display --key pk --format code private.key --out_file public_key.c
    ;;

mecc)
    #echo "****bulding mecc****"
    #pushd $RT_PATH/external/micro-ecc && dos2unix build_all.sh && bash build_all.sh && popd
    ;;
clean)
    echo "****cleaning****"
    rm *.hex *.zip
    ;;
-u|update)
    nrfutil dfu ble -ic NRF52 -pkg app_dfu_package.zip -p /dev/ttyACM0 -n "Nordic_Template" -f
    ;;

--default)
    DEFAULT=YES
    ;;
*)
    # unknown option
    ;;
esac
shift # past argument or value
done
