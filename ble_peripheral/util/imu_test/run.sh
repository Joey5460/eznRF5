#!/usr/bin/bash
#!/usr/bin/env bash
#push
if [[ $# -eq 0 ]] ; then
    exit 0
fi

#rm -rf filt.dat
#rm -rf up.dat
#./test_run.py "-fup1.dat"
#./plot_imu.py
#./test_run.py "-b"  
#./ezulp.py "68:C9:0B:06:BA:8B" "-S"  
#./ezulp.py "D2:34:33:14:6F:D1" "-S"  
#./ezulp.py "E4:0E:51:95:04:35" "-S"
#./ezulp.py "EE:95:8F:FE:82:D9" "-S"  
case $1 in 
    sync)
		./dev_test.py "F9:DB:10:98:41:64" "-S"  
		#./dev_test.py "EE:95:8F:FE:82:D9" "-S"  
        ;;
	imu)
        python	motion-driver-client_lx.py "F9:DB:10:98:41:64"   
        #python	motion-driver-client_lx.py "EE:95:8F:FE:82:D9"   
		;;
	*)
        ;;
esac

