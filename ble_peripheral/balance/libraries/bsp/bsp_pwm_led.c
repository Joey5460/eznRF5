/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup low_power_pwm_example_main main.c
 * @{
 * @ingroup low_power_pwm_example
 * @brief Low Power PWM Example Application main file.
 *
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "app_error.h"
#include "sdk_errors.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "low_power_pwm.h"
#include "nordic_common.h"
#include <math.h>
#define NRF_LOG_MODULE_NAME "BSP_PWM"
#include "nrf_log.h"


APP_TIMER_DEF(m_leds_timer_id);
/*Ticks before change duty cycle of each LED*/
#define TICKS_BEFORE_CHANGE_0   500
#define TICKS_BEFORE_CHANGE_1   400

static low_power_pwm_t low_power_pwm[LEDS_NUMBER];
//static low_power_pwm_t low_power_pwm_1;
//static low_power_pwm_t low_power_pwm_2;
//static low_power_pwm_t low_power_pwm_3;
//static low_power_pwm_t low_power_pwm_4;
//static low_power_pwm_t low_power_pwm_5;
const uint8_t sin_table[] = {
0, 0,1,2,4,6,9,12,16,20,24,29,35,40,	
46,	53,	59,	66,	74,	81,	88,	96,	104,
112,120,128,136,144,152,160,168,175,
182,190,197,203,210,216,221,227,232,
236,240,244,247,250,252,254,255,255,
255,255,255,254,252,250,247,244,240,
236,232,227,221,216,210,203,197,190,
182,175,168,160,152,144,136,128,120,
112,104,96,88,81,74,66,59, 53, 46, 
40, 35, 29,24, 20, 16, 12, 9, 6, 4, 2,1,0
};
static uint8_t  _rate = 1;
static uint8_t  _rate_r =1; 

static uint8_t  _color_l = 1;
static uint8_t  _color_r = 1; 
/**
 * @brief Function to be called in timer interrupt.
 *
 * @param[in] p_context     General purpose pointer (unused).
 */
static void pwm_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);

    low_power_pwm_t * pwm_instance = (low_power_pwm_t*)p_context;

    static uint16_t led0 = 0;
    static uint16_t led1 = 0;
    uint32_t err_code = 0;
    if (pwm_instance->bit_mask == BSP_LED_0_MASK)
    {
        led0 += _rate;
        led0 = led0%(sizeof(sin_table));
        err_code = low_power_pwm_duty_set(&low_power_pwm[0], sin_table[led0]);
        //err_code = low_power_pwm_duty_set(&low_power_pwm_2, 0);
        //err_code = low_power_pwm_duty_set(&low_power_pwm_4, 0);
        APP_ERROR_CHECK(err_code);

    } else if (pwm_instance->bit_mask == BSP_LED_1_MASK) {
        led1 += _rate_r;
        led1 = led1%(sizeof(sin_table));
        err_code = low_power_pwm_duty_set(&low_power_pwm[1], sin_table[led1]);
        //err_code = low_power_pwm_duty_set(&low_power_pwm_3, 0);
        //err_code = low_power_pwm_duty_set(&low_power_pwm_5, 0);
        APP_ERROR_CHECK(err_code);
    }else if (pwm_instance->bit_mask == BSP_LED_2_MASK){
        led0 += _rate;
        led0 = led0%(sizeof(sin_table));
        err_code = low_power_pwm_duty_set(&low_power_pwm[2], sin_table[led0]);
        //err_code = low_power_pwm_duty_set(&low_power_pwm_0, 0);
        //err_code = low_power_pwm_duty_set(&low_power_pwm_4, 0);
        APP_ERROR_CHECK(err_code);
    }else if (pwm_instance->bit_mask == BSP_LED_3_MASK){
        led1 += _rate_r;
        led1 = led1%(sizeof(sin_table));
        err_code = low_power_pwm_duty_set(&low_power_pwm[3], sin_table[led1]);
        //err_code = low_power_pwm_duty_set(&low_power_pwm_1, 0);
        //err_code = low_power_pwm_duty_set(&low_power_pwm_5, 0);
        APP_ERROR_CHECK(err_code);
    }else if (pwm_instance->bit_mask == BSP_LED_4_MASK){
        led0 += _rate;
        led0 = led0%(sizeof(sin_table));
        err_code = low_power_pwm_duty_set(&low_power_pwm[4], sin_table[led0]);
        APP_ERROR_CHECK(err_code);

    }else if (pwm_instance->bit_mask == BSP_LED_5_MASK){
        led1 += _rate_r;
        led1 = led1%(sizeof(sin_table));
        err_code = low_power_pwm_duty_set(&low_power_pwm[5], sin_table[led1]);
        APP_ERROR_CHECK(err_code);
    }else {
 
        /*empty else*/
    }
}


/**
 * @brief Function to initalize low_power_pwm instances.
 *
 */

void bsp_pwm_led_init(void)
{
    uint32_t err_code;
    low_power_pwm_config_t low_power_pwm_config;

    APP_TIMER_DEF(lpp_timer_0);
    low_power_pwm_config.active_high    = false;
    low_power_pwm_config.period         = 255;
    low_power_pwm_config.bit_mask       = BSP_LED_0_MASK;
    low_power_pwm_config.p_timer_id     = &lpp_timer_0;
    low_power_pwm_config.p_port         = NRF_GPIO;

    err_code = low_power_pwm_init((&low_power_pwm[0]), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm[0], 20);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_1);
    low_power_pwm_config.active_high    = false;
    low_power_pwm_config.period         = 255;
    low_power_pwm_config.bit_mask       = BSP_LED_1_MASK;
    low_power_pwm_config.p_timer_id     = &lpp_timer_1;
    low_power_pwm_config.p_port         = NRF_GPIO;

    err_code = low_power_pwm_init((&low_power_pwm[1]), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm[1], 20);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_2);
    low_power_pwm_config.active_high    = false;
    low_power_pwm_config.period         = 255;
    low_power_pwm_config.bit_mask       = BSP_LED_2_MASK;
    low_power_pwm_config.p_timer_id     = &lpp_timer_2;
    low_power_pwm_config.p_port         = NRF_GPIO;

    err_code = low_power_pwm_init((&low_power_pwm[2]), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm[2], 20);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_3);
    low_power_pwm_config.active_high    = false;
    low_power_pwm_config.period         = 255;
    low_power_pwm_config.bit_mask       = BSP_LED_3_MASK;
    low_power_pwm_config.p_timer_id     = &lpp_timer_3;
    low_power_pwm_config.p_port         = NRF_GPIO;

    err_code = low_power_pwm_init((&low_power_pwm[3]), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm[3], 20);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_4);
    low_power_pwm_config.active_high    = false;
    low_power_pwm_config.period         = 255;
    low_power_pwm_config.bit_mask       = BSP_LED_4_MASK;
    low_power_pwm_config.p_timer_id     = &lpp_timer_4;
    low_power_pwm_config.p_port         = NRF_GPIO;

    err_code = low_power_pwm_init((&low_power_pwm[4]), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm[4], 20);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_5);
    low_power_pwm_config.active_high    = false;
    low_power_pwm_config.period         = 255;
    low_power_pwm_config.bit_mask       = BSP_LED_5_MASK;
    low_power_pwm_config.p_timer_id     = &lpp_timer_5;
    low_power_pwm_config.p_port         = NRF_GPIO;

    err_code = low_power_pwm_init((&low_power_pwm[5]), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm[5], 20);
    APP_ERROR_CHECK(err_code);

    //err_code = low_power_pwm_start((&low_power_pwm[0]), low_power_pwm[0].bit_mask);
    //APP_ERROR_CHECK(err_code);
    //err_code = low_power_pwm_start((&low_power_pwm[1]), low_power_pwm[1].bit_mask);
    //APP_ERROR_CHECK(err_code);
    /*
    err_code = low_power_pwm_start((&low_power_pwm_2), low_power_pwm_2.bit_mask);
	APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start((&low_power_pwm_3), low_power_pwm_3.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start((&low_power_pwm_4), low_power_pwm_4.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start((&low_power_pwm_5), low_power_pwm_5.bit_mask);
	APP_ERROR_CHECK(err_code);
    */
}


void set_led_rate(uint8_t pos, uint8_t rate)
{
    if (0==pos){
        if (rate < sizeof(sin_table)) _rate = rate;
    }else if (1 == pos){
        if (rate < sizeof(sin_table)) _rate_r = rate;
    }
}
#define CLR_CNT 3
void set_led_color(uint8_t pos, uint8_t clr)
{
    uint32_t err_code = 0;
    if (0 == pos){
        //if ((clr < CLR_CNT) && (clr != _color_l)) {
        if ((clr < CLR_CNT)) {
            _color_l = clr;
            err_code = low_power_pwm_stop(&low_power_pwm[(((clr+1)%CLR_CNT)*2)]);
            APP_ERROR_CHECK(err_code);
            err_code = low_power_pwm_stop(&low_power_pwm[(((clr+2)%CLR_CNT)*2)]);
            APP_ERROR_CHECK(err_code);
            err_code = low_power_pwm_start(&low_power_pwm[clr*2], low_power_pwm[clr*2].bit_mask);
            APP_ERROR_CHECK(err_code);
            //err_code = low_power_pwm_duty_set(&low_power_pwm[(((clr+2)%CLR_CNT)*2)], 0);
            //err_code = low_power_pwm_duty_set(&low_power_pwm[(((clr+1)%CLR_CNT)*2)], 0);
        }
        
    }else if (1 == pos){
        //if (clr < CLR_CNT && clr != _color_r) {
        if (clr < CLR_CNT) {
            _color_r = clr;
            err_code = low_power_pwm_stop(&low_power_pwm[ ((clr+1)%CLR_CNT)*2 + 1]);
            APP_ERROR_CHECK(err_code);
            err_code = low_power_pwm_stop(&low_power_pwm[ ((clr+2)%CLR_CNT)*2 + 1]);
            APP_ERROR_CHECK(err_code);
            err_code = low_power_pwm_start((&low_power_pwm[(clr*2 + 1)]), low_power_pwm[clr*2 + 1].bit_mask);
            APP_ERROR_CHECK(err_code);
        }
    }
}

