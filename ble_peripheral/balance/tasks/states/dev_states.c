#include "dev_states.h"
#include "app_error.h"
#include "app_fifo.h"
#include "xtimers.h"

#define NRF_LOG_MODULE_NAME "ST"
#include "nrf_log.h"
#include "st_portable.h"

#define MSG_BUF_SIZE 32
static app_fifo_t st_msg_fifo;
static uint8_t msg_buf[MSG_BUF_SIZE];

typedef dev_state_t dev_state_func_t(st_data_t *data);
static dev_state_t _cur_state = ST_ACTIVE;
static st_data_t _inst_data ;

static st_tx_t  _st_tx = NULL;


void dev_state_handler_set(dev_state_handlers_t * cbs)
{
    if (cbs){
        if (cbs->st_tx) _st_tx = cbs->st_tx;
    }
} 


uint8_t get_cur_dev_state(void)
{
    return _cur_state;
}

dev_state_t do_state_active(st_data_t *data)
{
	static uint8_t is_set = 0; 
    if (data->evt == EVT_CONN){

        NRF_LOG_INFO("active and ready to connection \r\n");
        is_set = 0;
        _inst_data.evt = EVT_NULL;
        return ST_CONN; 
    } else if (data->evt == EVT_IDLE){

        is_set = 0;
        _inst_data.evt = EVT_NULL;
        return ST_IDLE;
    }else {

        if (is_set == 0){
            st_mpu_active();
            is_set = 1;
            NRF_LOG_INFO("active state \r\n");
        }
        
     int32_t adc_val[HX_ADC_NUM]={0,0,0,0};

        st_read_hx_adc(adc_val);
        int32_t rt = adc_val[0]+ adc_val[2];
        int32_t lt = adc_val[1]+ adc_val[3];
        //0,2 right led
       if ((rt-lt)< -30) {
         st_set_led_rate(1, 5);
         st_set_led_rate(0, 1);
       }else if((lt-rt)<-30){
         st_set_led_rate(0, 5);
         st_set_led_rate(1, 1);
       }else{
         st_set_led_rate(0, 1);
         st_set_led_rate(1, 1);
       }
       /*
        NRF_LOG_INFO("%u, %d, %d, %d, %d\r\n",
                get_utc(), 
                adc_val[0],
                adc_val[1],
                adc_val[2],
                adc_val[3]
                );
        */
        //st_store_active_data();

       return ST_ACTIVE;
    }
}

dev_state_t do_state_connected(st_data_t *data)
{
    static uint8_t is_set = 0; 
    if (data->evt == EVT_DISCON){
        is_set = 0;
        _inst_data.evt = EVT_NULL;
        NRF_LOG_INFO("connected and ready to fast adv \r\n");
        return ST_ACTIVE;

    }else if(EVT_SYNC == data->evt){
        is_set = 0;
        _inst_data.evt = EVT_NULL;
        return ST_SYNC;

    }else{
        if (0 == is_set){
            is_set = 1;
            NRF_LOG_INFO("connected state \r\n");
        }

        int32_t adc_val[HX_ADC_NUM]={0,0,0,0};

        st_read_hx_adc(adc_val);
        int32_t rt = adc_val[0]+ adc_val[2];
        int32_t lt = adc_val[1]+ adc_val[3];
        //0,2 right led
       if ((rt-lt)< -30) {
         st_set_led_rate(1, 5);
         st_set_led_rate(0, 1);
       }else if((lt-rt)<-30){
         st_set_led_rate(0, 5);
         st_set_led_rate(1, 1);
       }else{
         st_set_led_rate(0, 1);
         st_set_led_rate(1, 1);
       }
        NRF_LOG_INFO("%u, %d, %d, %d, %d\r\n",
                get_utc(), 
                adc_val[0],
                adc_val[1],
                adc_val[2],
                adc_val[3]
                );
        int32_t pack_data[HX_ADC_NUM+1] = {0x01, 
                                        adc_val[0],
                                        adc_val[1],
                                        adc_val[2],
                                        adc_val[3]
                                      };
        if(_st_tx) _st_tx(_cur_state, (uint8_t*)pack_data);

        return ST_CONN;
    }
}

dev_state_t do_state_idle(st_data_t *data)
{
    static uint8_t is_set = 0; 
    if(data->evt == EVT_ACTIVE){
        is_set = 0;
        _inst_data.evt = EVT_NULL;
        return ST_ACTIVE; 

    }else{
        if (0 == is_set){
            st_mpu_standy();
            is_set = 1;
            NRF_LOG_INFO("idle state \r\n");
        }
        return ST_IDLE;	
    }
}

dev_state_t do_state_sync(st_data_t *data)
{
    static uint8_t is_set = 0; 
	uint8_t log_data[20] = {0xC0, 0x20};
    if (!is_set){
        is_set = 1;
        NRF_LOG_INFO("log sync state \r\n");
		if(st_sync_start()) return ST_CONN;
    }

    if (data->evt == EVT_DISCON){
		st_sync_end();
        is_set = 0;
        return ST_ACTIVE;
    }else if (!st_sync_get_data(log_data)){
        
		if(_st_tx) _st_tx(_cur_state, log_data);
        return ST_SYNC;

    }else{
		NRF_LOG_HEXDUMP_INFO(log_data, 20);
		if(_st_tx) _st_tx(_cur_state, log_data);
		st_sync_end();
        is_set = 0;
        return ST_CONN;
    }
}

dev_state_func_t * const dev_state_table[] = {
    do_state_active,
    do_state_connected,
    do_state_idle,
    do_state_sync,
};

void dev_state_data_set(dev_st_evt_t evt )
{
    app_fifo_put(&st_msg_fifo, evt);

    NRF_LOG_INFO("Set State Event %u\r\n", evt);
}

void dev_state_init(dev_state_handlers_t * cb)
{
    dev_state_handler_set(cb);
    uint32_t err_code;
    err_code = app_fifo_init(&st_msg_fifo, msg_buf, MSG_BUF_SIZE);
    APP_ERROR_CHECK(err_code);
    st_set_hx_offset();
    NRF_LOG_INFO("stat init\r\n");
}

dev_state_t dev_state_run(void) 
{
    uint8_t p_byte;
    if (NRF_SUCCESS == app_fifo_get(&st_msg_fifo, &p_byte)){
        _inst_data.evt = p_byte;
        NRF_LOG_INFO("Get FIFO %u\r\n", _inst_data.evt);

    }else{
        _inst_data.evt = EVT_NULL;

    }

    _cur_state = dev_state_table[ _cur_state ]( &_inst_data );

    return _cur_state;
}
