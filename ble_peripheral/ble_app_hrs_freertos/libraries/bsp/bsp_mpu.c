#include "bsp_mpu.h"
#include "mpu_port.h"
#include "../../drivers/mpu_twi_master/mpu_twim.h"

#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "../eMPL/inv_mpu.h"
#include "../eMPL/inv_mpu_dmp_motion_driver.h"

#include "nrf_delay.h"
#include "sdk_config.h"
#define NRF_LOG_MODULE_NAME "BSP_MPU"
#include "nrf_log.h"

#define MPU9250_DEV_ADDR            0x68
#define MPU6050_RA_WHO_AM_I         0x75

#define PRODUCT_WHOAMI	(0x68)
#define DEFAULT_MPU_HZ  (100)

/*
   +pitch                              
   (-90---0---+45---+90---+45----0)    
   \-------------- 
   \o
   \
   \
   \--------------
   +roll(0----(-180)(+180)---+90---0)

*/
//static signed char gyro_orientation[9] = {-1, 0, 0, 0,-1, 0, 0, 0, 1};
//for0.21
//static signed char gyro_orientation[9] = {0, 1, 0, -1, 0, 0, 0, 0, 1};

//for 0.20
static signed char gyro_orientation[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
static  unsigned short inv_row_2_scale(const signed char *row)
{
	unsigned short b;
	//
	if (row[0] > 0)    b = 0;
	else if (row[0] < 0)  b = 4;
	else if (row[1] > 0)   b = 1;
	else if (row[1] < 0)   b = 5;
	else if (row[2] > 0)       b = 2;
	else if (row[2] < 0)   b = 6;
	else         b = 7;      // error
	return b;
}

static  unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx)
{
	unsigned short scalar;


	//XYZ  010_001_000 Identity Matrix
	//XZY  001_010_000
	//YXZ  010_000_001
	//YZX  000_010_001
	//ZXY  001_000_010
	//ZYX  000_001_010


	scalar = inv_row_2_scale(mtx);
	scalar |= inv_row_2_scale(mtx + 3) << 3;
	scalar |= inv_row_2_scale(mtx + 6) << 6;
	return scalar;
}
static mpu_evt_handler_t m_mpu_evt_handler;
static volatile uint8_t is_mpu_int = 0;
uint8_t get_mpu_int(void)
{
    return is_mpu_int;
}

void clear_mpu_int(void)
{
    is_mpu_int = 0;
}

static void gpio_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	if(pin != MPU_INT_PIN)
		return;
    m_mpu_evt_handler(BSP_MPU_EVT_MOT);
    is_mpu_int = 1;
}

void bsp_mpu_evt_handler_set(mpu_evt_handler_t  mpu_evt_handler)
{
    m_mpu_evt_handler = mpu_evt_handler;
}

void bsp_mpu_int_init()
{
	//Initialize interrupt pin
    uint32_t err_code = 0;
	if (!nrf_drv_gpiote_is_init())
	{
		err_code = nrf_drv_gpiote_init();
		if (err_code != NRF_SUCCESS)
		{
            NRF_LOG_INFO("\r\n gpiote init \r\n");
            
			APP_ERROR_CHECK(err_code);
		}
	}

    //nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);
	//nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
	nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
	err_code = nrf_drv_gpiote_in_init(MPU_INT_PIN, &config, gpio_handler);
	if (err_code != NRF_SUCCESS)
	{
		NRF_LOG_INFO("\r\n gpiote in init  \r\n");
		APP_ERROR_CHECK(err_code);
	}
   // nrf_gpio_cfg_input(MPU_INT_PIN, NRF_GPIO_PIN_PULLUP);
	nrf_drv_gpiote_in_event_enable(MPU_INT_PIN, true);
}
const uint8_t mpu_init_timeout = 20;
void bsp_mpu_init(void)
{
    i2c_init();

	if(!mpu_init(0)){
		NRF_LOG_INFO("\r\n mpu_init ok \r\n");
	} else {
		NRF_LOG_INFO("\r\n mpu_init failed \r\n");
	}

	if(!mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL)) {
		NRF_LOG_INFO("\r\n mpu sensors ok \r\n");
	} else {
		NRF_LOG_INFO("\r\n mpu sensors failed \r\n");
	}

	if(!mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL)) {
		NRF_LOG_INFO("\r\n mpu fifo ok \r\n");
	} else {
		NRF_LOG_INFO("\r\n mpu fifo failed \r\n");
	}

	if(!mpu_set_sample_rate(DEFAULT_MPU_HZ)) {
		NRF_LOG_INFO("\r\n set_sample_rate ok \r\n");
	} else {
		NRF_LOG_INFO("\r\n set_sample_rate failed \r\n");
	}

    uint8_t timeout = mpu_init_timeout ; 
    while(dmp_load_motion_driver_firmware() && --timeout);

	if(timeout)	{
		NRF_LOG_INFO("\r\n load_motion_driver ok %d \r\n", timeout);
	} else {
		NRF_LOG_INFO("\r\n load_motion_driver failed \r\n");
	}

	if(!dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation))) {
		NRF_LOG_INFO("\r\n dmp_set_orientation ok \r\n");
	} else {
		NRF_LOG_INFO("\r\n dmp_set_orientation failed \r\n");
	}

	if(!dmp_enable_feature(
                DMP_FEATURE_6X_LP_QUAT |
				//DMP_FEATURE_TAP |
				DMP_FEATURE_ANDROID_ORIENT |
				DMP_FEATURE_SEND_RAW_ACCEL |
				DMP_FEATURE_SEND_CAL_GYRO  |
				DMP_FEATURE_GYRO_CAL|
                DMP_FEATURE_PEDOMETER
                ))
	{
		NRF_LOG_INFO("\r\n dmp_enable_feature ok \r\n");
	} else {
		NRF_LOG_INFO("\r\n dmp_enable_feature failed \r\n");

	}

	if(!dmp_set_fifo_rate(DEFAULT_MPU_HZ)) {
		NRF_LOG_INFO("\r\n dmp_set_fifo_rate ok \r\n");
	}else {
		NRF_LOG_INFO("\r\n dmp_set_fifo_rate failed \r\n");

	}

    timeout = mpu_init_timeout ; 
    while(mpu_set_dmp_state(1) && --timeout){}
	if(timeout) {
		NRF_LOG_INFO("\r\n mpu_set_dmp_state ok \r\n");
	} else {
		NRF_LOG_INFO("\r\n mpu_set_dmp_state failed \r\n");

	}
    bsp_mpu_int_init();
}

void bsp_mpu_uninit(void)
{
	mpu_set_dmp_state(0);
	mpu_configure_fifo(0);
    mpu_set_sensors(INV_XYZ_ACCEL); 
    mpu_lp_accel_mode(20);
    mpu_lp_motion_interrupt(300, 1, 5);
    i2c_deinit();
}

static uint8_t _tmout = 0;
void bsp_mpu_standby(void)
{
    _tmout = 3;
    while(mpu_set_sensors(INV_XYZ_ACCEL) && _tmout--); 
    //while(mpu_lp_accel_mode(20));
    _tmout = 3;
    while(mpu_lp_motion_interrupt(300, 1, 5) && _tmout--);
    i2c_deinit();
}

void bsp_mpu_wakeup(void)
{
    i2c_init();
    _tmout = 3;
    while(mpu_lp_motion_interrupt(0, 0, 0) && _tmout--);
}


