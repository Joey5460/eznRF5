#ifndef _BSP_PWM_LED_H_
#define _BSP_PWM_LED_H_
void bsp_pwm_led_init(void);
void set_led_rate( uint8_t pos, uint8_t rate);
void set_led_color(uint8_t pos, uint8_t clr);
#endif //_BSP_PWM_LED_H_
