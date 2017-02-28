#include "util.h"
static void m2s(uint8_t *val)
{
  if (*val <= 9){
    *val += 0x30;
  } else if (*val >= 10 && *val <= 35 ){
    *val += (0x41 - 10);
  } else if (*val >= 36 && *val <= 61){
    *val += (0x61 - 36);
  } else if (62 == *val){
    *val = 0x40;
  } else if (63 == *val){
    *val = 0x2F;
  } else {
    *val = 0x3F;
  }
}

uint8_t mac2symbol(uint8_t *mac, uint8_t *out, uint8_t len)
{
  if(len != 6){
    return 0;
  }
  uint8_t i = 0;
  for(i = 0; i < len; i++){
    out[i] = mac[i] & 0x3F;
    m2s(&out[i]);
    out[i/3+6] |= (mac[i]&0xC0) >> (i%3+1)*2;
  }
  m2s(&out[6]);
  m2s(&out[7]);

  return 1;
}
