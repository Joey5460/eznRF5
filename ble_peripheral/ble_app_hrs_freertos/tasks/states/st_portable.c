#include "st_portable.h"
#include "../../libraries/eMPL/inv_mpu_dmp_motion_driver.h"
#include "../../libraries/log/log.h"
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

uint8_t st_imu_get_data(uint8_t * data)
{		
	unsigned long sensor_timestamp;
	short gyro[3], accel[3], sensors = 0;
	//uint8_t mpu_sensors = 0;
	unsigned char more;
	long quat[5];

	if (!dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more)){

		if (sensors & INV_WXYZ_QUAT ){

			//uint32_t qdata[5]={quat[0], quat[1], quat[2], quat[3]};
            uint8_t*tmp = (uint8_t*)quat;
            uint8_t i = 0;
            for (i=0;i<16;i++){
                data[i] = tmp[i];
            }
            //NRF_LOG_INFO("imu data \r\n", quat[0]);
			//if (mpu_tst_on) send_packet(PACKET_TYPE_QUAT, quat);
		}
        return 0;
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

