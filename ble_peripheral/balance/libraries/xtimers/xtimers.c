#include "xtimers.h"
#include "app_timer.h"

#define APP_TIMER_PRESCALER              0                                          /**< Value of the RTC1 PRESCALER register. */
#define UTC_INTERVAL      APP_TIMER_TICKS(10, APP_TIMER_PRESCALER)    
APP_TIMER_DEF(m_utc_timer_id);
static uint32_t _utc_s = 0; 

static void utc_timer_handler(void * p_context)
{
  UNUSED_PARAMETER(p_context);
  _utc_s++;
}


void utc_timer_init(void)
{
    uint32_t err_code;
    err_code = app_timer_create(&m_utc_timer_id,
            APP_TIMER_MODE_REPEATED,
            utc_timer_handler);
    APP_ERROR_CHECK(err_code);
}

uint32_t get_utc(void)
{
    return _utc_s;
}

void set_utc(uint32_t sec)
{
    _utc_s = sec;
}

void utc_timer_start(void)
{
    uint32_t err_code;
    err_code = app_timer_start(m_utc_timer_id,
            UTC_INTERVAL,
            NULL);
    APP_ERROR_CHECK(err_code);
}

void utc_timer_stop(void)
{
    uint32_t err_code;
    err_code = app_timer_stop(m_utc_timer_id);
    APP_ERROR_CHECK(err_code);
}

