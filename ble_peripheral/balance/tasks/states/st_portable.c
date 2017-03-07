#include "st_portable.h"
#include "../../libraries/eMPL/inv_mpu_dmp_motion_driver.h"
#include "../../libraries/log/log.h"
#include "../../libraries/bsp/bsp_hx_adc.h"
#include "../../libraries/bsp/bsp_pwm_led.h"
#include "xtimers.h"
#include "../../libraries/util/util.h"
#include "nrf_delay.h"
#define NRF_LOG_MODULE_NAME "ST_PORT"
#include "nrf_log.h"
void st_mpu_active(void)
{
}

void st_mpu_standy(void)
{
}

static uint32_t _pkg_cnt = 0;
static uint32_t _last_pkg_len = 0;
const uint8_t _pkg_len = 16;
uint8_t st_sync_start(void)
{
	if (log_sync_start()){
        return 1; 
    }

	uint32_t log_len = log_sync_get_len();
	_pkg_cnt = log_len/_pkg_len;
	_last_pkg_len = log_len%_pkg_len;
	NRF_LOG_INFO("package num: %x \r\n", _pkg_cnt);
	NRF_LOG_INFO("log len: %x \r\n", log_len);
    return 0;
}

void st_sync_end(void)
{
	log_sync_end();
}

uint8_t st_sync_get_data(uint8_t * data)
{
	if (_pkg_cnt){
		data[0] = 0xC0;
		data[1] = 0x10;
		data[2] = _pkg_len;
		if (-1 == log_sync_read(_pkg_len, &data[4])){

			return 1;
		}
		NRF_LOG_HEXDUMP_INFO(data, 20);
        NRF_LOG_INFO("\r\n");
		_pkg_cnt--;
		return 0;
	}else if (_last_pkg_len){
		data[0] = 0xC0;
		data[1] = 0x10;
		data[2] = _last_pkg_len;
		log_sync_read(_last_pkg_len, &data[4]);
        _last_pkg_len=0;
		NRF_LOG_HEXDUMP_INFO(data, 20);
		return 0;
	}else{
		NRF_LOG_INFO("[log] reach eof and sync done \r\n");
		return 1;
    }
	return 1;
}

const  uint16_t  _delt = 100;
void st_store_active_data(void)
{
    static uint32_t log_timeout = 0;
	if (!log_timeout) log_timeout = get_utc();
	short gyro[3], accel[3], sensors = 0;
	uint32_t sensor_timestamp = 0;
	uint32_t steps = 0;
	unsigned char more;
	long quat[5];
	log_imu_ex_t log_imu;
	if(time_after(get_utc(), log_timeout + _delt)){ 
		if (!dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more)){
			log_imu.ax = accel[0];
			log_imu.ay = accel[1];
			log_imu.az = accel[2];
			log_imu.q0 = quat[0];
			log_imu.q1 = quat[1];
			log_imu.q2 = quat[2];
			log_imu.q3 = quat[3];
			log_imu.time = get_utc();

			dmp_get_pedometer_step_count(&steps);
			log_imu.steps = steps;

			// UNUSED_VARIABLE(log_imu);
			log_save_ex(&log_imu);
			dmp_set_pedometer_step_count(0);
			log_timeout = get_utc();
		}
	}
}

void st_read_hx_adc(uint8_t ch, int32_t * adc_val)
{
    if (0 == ch){
        //set_channel(A128); 
        read_hx_adc(adc_val);
        //get_value(5, adc_val);

    }else if(1 == ch) {
        //set_channel(B32); 
        //read_hx_adc(adc_val);
    }
}

void st_get_offset(int32_t * adc_val)
{
    get_offset(adc_val);
}
