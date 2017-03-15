#!/usr/bin/bash
#!/usr/bin/env bash
#push
if [[ $# -eq 0 ]] ; then
    exit 0
fi
case $1 in 
    sync)
        python test_sync.py "F9:DB:10:98:41:64" "-S"
        ;;
	imu)
        python	motion-driver-client/motion-driver-client_lx.py "F9:DB:10:98:41:64" 
		;;
    scan)
        gatttool -i hci1 -b "F9:DB:10:98:41:64" -t random -I
        ;;
	*)
        ;;
esac

