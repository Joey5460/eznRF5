if [[ $# -eq 0 ]] ; then
    vim -S ble_app_hrs_freertos/ezle.vim
    exit 0
fi

while [[ $# -gt 0 ]]
do
key="$1"
case $key in
    le)
		#vim -S ~/.vim/sessions/ezle.vim
		vim -S ble_app_hrs_freertos/ezle.vim
        ;;
    b)
        vim -S ~/.vim/sessions/balance.vim
        ;;
    *)
        ;;
esac
shift # past argument or value
done


