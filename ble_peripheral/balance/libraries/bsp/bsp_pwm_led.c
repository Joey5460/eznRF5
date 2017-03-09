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

static low_power_pwm_t low_power_pwm_0;
static low_power_pwm_t low_power_pwm_1;
static low_power_pwm_t low_power_pwm_2;
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
static float  _rate = 1;
static float  _rate_r =1; 

/**
 * @brief Function to be called in timer interrupt.
 *
 * @param[in] p_context     General purpose pointer (unused).
 */
static void pwm_handler(void * p_context)
{
    //uint8_t new_duty_cycle;
    //static uint16_t led_0; 
    //static uint16_t led_1;
    //uint32_t err_code;
    UNUSED_PARAMETER(p_context);

    low_power_pwm_t * pwm_instance = (low_power_pwm_t*)p_context;

    if (pwm_instance->bit_mask == BSP_LED_0_MASK)
    {
        static uint16_t led0 = 0;
        uint32_t err_code = 0;
        led0 += _rate;
        led0 = led0%(sizeof(sin_table));
        err_code = low_power_pwm_duty_set(&low_power_pwm_0, sin_table[led0]);
        APP_ERROR_CHECK(err_code);

        /*
        led_0++;

        if (led_0 > TICKS_BEFORE_CHANGE_0)
        {
            new_duty_cycle = pwm_instance->period - pwm_instance->duty_cycle;
            err_code = low_power_pwm_duty_set(pwm_instance, new_duty_cycle);
            led_0 = 0;
            APP_ERROR_CHECK(err_code);
        }
        */
        //_run();
    }
    else if (pwm_instance->bit_mask == BSP_LED_1_MASK)
    {/*
        led_1++;

        if (led_1 > TICKS_BEFORE_CHANGE_1)
        {
            new_duty_cycle = pwm_instance->period - pwm_instance->duty_cycle;
            err_code = low_power_pwm_duty_set(pwm_instance, new_duty_cycle);
            led_1 = 0;
            APP_ERROR_CHECK(err_code);
        }
        */
        static uint16_t led1 = 0;
        uint32_t err_code = 0;
        led1 += _rate_r;
        led1 = led1%(sizeof(sin_table));
        err_code = low_power_pwm_duty_set(&low_power_pwm_1, sin_table[led1]);
        APP_ERROR_CHECK(err_code);
    }
    else
    {
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

    err_code = low_power_pwm_init((&low_power_pwm_0), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm_0, 20);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_1);
    low_power_pwm_config.active_high    = false;
    low_power_pwm_config.period         = 255;
    low_power_pwm_config.bit_mask       = BSP_LED_1_MASK;
    low_power_pwm_config.p_timer_id     = &lpp_timer_1;
    low_power_pwm_config.p_port         = NRF_GPIO;

    err_code = low_power_pwm_init((&low_power_pwm_1), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm_1, 150);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_2);
    low_power_pwm_config.active_high    = false;
    low_power_pwm_config.period         = 100;
    low_power_pwm_config.bit_mask       = BSP_LED_2_MASK;
    low_power_pwm_config.p_timer_id     = &lpp_timer_2;
    low_power_pwm_config.p_port         = NRF_GPIO;

    err_code = low_power_pwm_init((&low_power_pwm_2), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm_2, 20);
    APP_ERROR_CHECK(err_code);

    err_code = low_power_pwm_start((&low_power_pwm_0), low_power_pwm_0.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start((&low_power_pwm_1), low_power_pwm_1.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start((&low_power_pwm_2), low_power_pwm_2.bit_mask);

	APP_ERROR_CHECK(err_code);
}


void set_led_rate(uint8_t pos, uint8_t rate)
{
    if (0==pos){
        if (rate < sizeof(sin_table)) _rate = rate;
    }else if (1 == pos){
        if (rate < sizeof(sin_table)) _rate_r = rate;
    }
}

