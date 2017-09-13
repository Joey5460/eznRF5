#!/usr/bin/bash
# Use -gt 1 to consume two arguments per pass in the loop (e.g. each
# argument has a corresponding value to go with it).
# Use -gt 0 to consume one or more arguments per pass in the loop (e.g.
# some arguments don't have a corresponding value to go with it such
# as in the --default example).
# note: if this is set to -gt 0 the /etc/hosts part is not recognized ( may be a bug )
while [[ $# -gt 0 ]]
do
key="$1"
SD_PATH="../../../../components/softdevice/s132/hex"
APP_PATH="../ezsmart/s132/armgcc/_build"
BL_PATH="../../../dfu/bootloader_secure/pca10040/armgcc/_build"
case $key in
    -d|dfu)
        echo "packing dfu"
        #shift # past argument
        #nrfutil pkg generate --application nrf52832_xxaa.hex --key-file private.key app_dfu_package.zip
        nrfutil pkg generate --hw-version 52 --application-version 4 --application $APP_PATH/nrf52832_xxaa.hex --sd-req 0x8C --key-file private.key app_dfu_package.zip
    ;;
    -h|hex)
        echo "mergering hex"
		cp -r $BL_PATH/nrf52832_xxaa_s132.hex bootloader.hex
        mergehex -m $SD_PATH/s132_nrf52_3.0.0_softdevice.hex bootloader.hex -o sd_bl.hex
        mergehex -m sd_bl.hex $APP_PATH/nrf52832_xxaa.hex -o sd_bl_app.hex 
        mergehex -m sd_bl_app.hex bootloader_settings.hex -o sd_bl_app_cfg.hex 
    #shift # past argument
    ;;
    -f|flash)
        nrfjprog --program sd_bl_app_cfg.hex -f nrf52 --chiperase
        nrfjprog --reset -f nrf52
    ;;
    setting)
        echo "setting"
        nrfutil settings generate --family NRF52 --application $APP_PATH/nrf52832_xxaa.hex --application-version 3 --bootloader-version 1 --bl-settings-version 1 bootloader_settings.hex
    ;;
    gkey)
        nrfutil keys generate private.key
		nrfutil keys display --key pk --format code private.key --out_file public_key.c
    ;;
    update)
    nrfutil dfu ble -ic NRF51 -pkg app_dfu_package.zip -p /dev/ttyACM0 -n "Nordic_Template" -f
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
