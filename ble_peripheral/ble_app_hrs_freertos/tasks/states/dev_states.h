#ifndef _ZSTATE_H_
#define _ZSTATE_H_
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef void (*vv_t)(void);
typedef void (*st_tx_t)(uint8_t, uint8_t *);
typedef struct {
    st_tx_t  st_tx;
}dev_state_handlers_t;

typedef struct 
{
    uint32_t evt;
} st_data_t;

typedef enum {
    EVT_NULL = 0x00,
    EVT_CONN = 0x01,
	EVT_IDLE, 
	EVT_DISCON,
    EVT_SYNC,
    EVT_ACTIVE
} dev_st_evt_t;

typedef enum {
    ST_ACTIVE=0,
    ST_CONN=1,
	ST_IDLE,
    ST_SYNC,
} dev_state_t;


void dev_state_handler_set(dev_state_handlers_t *cb); 
void dev_state_data_set(dev_st_evt_t evt );
uint8_t get_cur_dev_state(void);

void dev_state_init(dev_state_handlers_t * cb);
dev_state_t dev_state_run(void); 
#endif
