" A function to test the ':Pyclewn' command in a script.
function PyclewnScripting()
	" This function can be called while the previous netbeans session is
	" still open.
	if has("netbeans_enabled")
		echo "Error: netbeans is already connected."
		call input("Press the <Enter> key to continue.")
		return
	endif

	let g:pyclewn_args="--gdb=async, --pgm=/usr/local/gcc-arm-none-eabi-4_9-2015q1/bin/arm-none-eabi-gdb"
	Pyclewn gdb
    Cmapkeys
    Cfile _build/nrf52832_xxaa_s132.out 
    Ctarget remote localhost:2331
    Cbreak main
    Cmonitor reset
endfunc
