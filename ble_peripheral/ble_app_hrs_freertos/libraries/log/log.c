#include "log.h"
#include "../spiffs/spiffs.h"
#include "../../drivers/flash/w25q_flash.h"
#define NRF_LOG_MODULE_NAME "SPIFFS"
#include "nrf_log.h"
#include "nrf_delay.h"
#ifdef  EZSMART
#define PHYS_FLASH_SIZE       (4*1024*1024)
#define SPIFFS_FLASH_SIZE     (4*1024*1024)
#elif   defined ZULP
#define PHYS_FLASH_SIZE       (1024*1024)
#define SPIFFS_FLASH_SIZE     (1024*1024)
#endif //EZSMART
#define SPIFFS_PHYS_ADDR      (0)
#define SECTOR_SIZE         (4*1024) 
#define LOG_BLOCK_SIZE      (64*1024)
#define LOG_PAGE_SIZE       (256)

static spiffs _fs;
static u8_t _work [LOG_PAGE_SIZE*2] ;
static u8_t _fds[32*4];
static u8_t _cache[(LOG_PAGE_SIZE+32)*4];

static s32_t _read( u32_t addr, u32_t size, u8_t *dst)
{
    nrf_delay_ms(1);
    flash_read(addr, dst, size);
    return 0;
}

static s32_t _write(u32_t addr, u32_t size, u8_t *src)
{
    flash_program(addr, src, size);

    return 0;
}

static s32_t _erase(u32_t addr, u32_t size) 
{
    if (LOG_BLOCK_SIZE == size ) {
        flash_erase(FLASH_ERASE_BLOCK_64, addr);
    }
    return 0;
}

//static void spiffs_check_cb_f(spiffs_check_type type, spiffs_check_report report,
//        u32_t arg1, u32_t arg2) 
//{
//}


void test_spiffs() 
{
    u8_t buf[12]={9,8,7,6,5,4,3,2,1,0,11,12};

    // create a file, delete previous if it already exists, and open it for reading and writing
    spiffs_file fd = SPIFFS_open(&_fs, "ns_file", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
    if (fd < 0) {
        NRF_LOG_INFO("open errno %i\n", SPIFFS_errno(&_fs));
        return;
    }
    // write to it
    if (SPIFFS_write(&_fs, fd, (u8_t *)"Hello world", 12) < 0) {
    //if (SPIFFS_write(&_fs, fd, (u8_t *)buf, 12) < 0) {
        NRF_LOG_INFO("errno %i\n", SPIFFS_errno(&_fs));
        return;
    }
    // close it
    if (SPIFFS_close(&_fs, fd) < 0) {
        NRF_LOG_INFO("errno %i\n", SPIFFS_errno(&_fs));
        return;
    }

    int i = 0;
    for (i = 0; i< 3;i++){
        // open it
        fd = SPIFFS_open(&_fs, "ns_file", SPIFFS_RDWR, 0);
        if (fd < 0) {
            NRF_LOG_INFO("errno %i\n", SPIFFS_errno(&_fs));
            return;
        }
        /*
        // read it
        if (SPIFFS_read(&_fs, fd, (u8_t *)buf, 12) < 0) {
        NRF_LOG_INFO("errno %i\n", SPIFFS_errno(&_fs));
        return;
        }
        */
        int i = 0;
        for(i=0;i<12;i++) buf[i]=0;
        SPIFFS_read(&_fs, fd, (u8_t *)buf, 12); 
        //s32_t l = SPIFFS_read(&_fs, fd, (u8_t *)buf, 12); 
        //NRF_LOG_INFO("read len %d\r\n", l);
        // close it
        if (SPIFFS_close(&_fs, fd) < 0) {
            NRF_LOG_INFO("errno %i\n", SPIFFS_errno(&_fs));
            return;
        }
        // check it
        //NRF_LOG_INFO("--> %s <--\n", &buf[2]);
        //for(i=0;i<12;i++)
        //NRF_LOG_INFO("--> %s <--\r\n", (uint32_t *)(buf));
        NRF_LOG_HEXDUMP_INFO(buf, sizeof(buf))
    }
}

uint32_t log_init(void)
{
  //flash_chip_erase();
  flash_init();

  spiffs_config c;
  c.hal_erase_f = _erase;
  c.hal_read_f = _read;
  c.hal_write_f = _write;
  c.log_page_size = LOG_PAGE_SIZE;
  c.log_block_size = LOG_BLOCK_SIZE ;
  c.phys_erase_block = LOG_BLOCK_SIZE;
  c.phys_addr = SPIFFS_PHYS_ADDR;
  c.phys_size = SPIFFS_FLASH_SIZE;
#if SPIFFS_FILEHDL_OFFSET
  c.fh_ix_offset = TEST_SPIFFS_FILEHDL_OFFSET;
#endif
  int res = 1;
  res = SPIFFS_mount(&_fs, &c, _work, _fds, sizeof(_fds), _cache, sizeof(_cache), NULL);

  NRF_LOG_INFO("FLASH mount %x\r\n", res);

  //test_spiffs(); 

  return 0;
}

void log_uninit(void)
{
  SPIFFS_unmount(&_fs); 
  flash_uninit();
}


static vu8_t *sync_tx = NULL;

void log_set_callbacks(log_handlers_t cb)
{
    sync_tx = cb.sync_tx;
}

const char* imu_log_name= "imu.log";
const char* imu_log_ex_name= "imu.log";
static spiffs_file _fd; 

void log_save(log_imu_t *data)
{
    spiffs_file fd = SPIFFS_open(&_fs, imu_log_name, SPIFFS_CREAT | SPIFFS_APPEND |SPIFFS_RDWR, 0);
    if (fd < 0) {
        NRF_LOG_INFO("[SPIFFS] %i\n", SPIFFS_errno(&_fs));
        return;
    }
    NRF_LOG_INFO("save open \r\n" );

    if (SPIFFS_write(&_fs, fd, (u8_t *)data, sizeof(log_imu_t)) < 0) {
        NRF_LOG_INFO("[SPIFFS] %i\n", SPIFFS_errno(&_fs));
        return;
    }
    NRF_LOG_INFO("save write \r\n" );

    // close it
    if (SPIFFS_close(&_fs, fd) < 0) {
        NRF_LOG_INFO("[SPIFFS] %i\n", SPIFFS_errno(&_fs));
        return;
    }
    NRF_LOG_INFO("save close \r\n" );
}

void log_save_ex(log_imu_ex_t *data)
{
    spiffs_file fd = SPIFFS_open(&_fs, imu_log_ex_name, SPIFFS_CREAT | SPIFFS_APPEND |SPIFFS_RDWR, 0);
    if (fd < 0) {
        NRF_LOG_INFO("[SPIFFS] OPEN ERROR %i\n", SPIFFS_errno(&_fs));
        return;
    }
    NRF_LOG_INFO("save open \r\n" );

    if (SPIFFS_write(&_fs, fd, (u8_t *)data, sizeof(log_imu_ex_t)) < 0) {
        NRF_LOG_INFO("[SPIFFS] %i\n", SPIFFS_errno(&_fs));
        return;
    }
    NRF_LOG_INFO("save write \r\n" );

    // close it
    if (SPIFFS_close(&_fs, fd) < 0) {
        NRF_LOG_INFO("[SPIFFS] %i\n", SPIFFS_errno(&_fs));
        return;
    }
    NRF_LOG_INFO("save close \r\n" );
}

int8_t log_sync_start(void)
{
    NRF_LOG_INFO("log sync start \r\n");
    _fd = SPIFFS_open(&_fs, imu_log_name, SPIFFS_O_RDONLY, 0);
    if (_fd < 0) {
        NRF_LOG_INFO("[SPIFFS] start sync error%i\n", SPIFFS_errno(&_fs));
        return -1;
    }
    return 0;
}

int8_t log_sync_end(void)
{
    if (SPIFFS_close(&_fs, _fd) < 0) {
        NRF_LOG_INFO("close errno %i\n", SPIFFS_errno(&_fs));
        return -1;
    }

    NRF_LOG_INFO("[log] sync end \r\n");
    return 0;
}

uint32_t  log_sync_get_len(void)
{
    spiffs_stat s;
    SPIFFS_stat(&_fs, imu_log_name, &s);
    if (SPIFFS_errno(&_fs) == 0) return s.size;
    else {
            NRF_LOG_INFO("get log error  errno %i\r\n", SPIFFS_errno(&_fs));
        return 0;
    }
}

int8_t log_sync_read(uint8_t pkg_len, uint8_t * data)
{   
    if (!SPIFFS_eof(&_fs, _fd)){

        SPIFFS_read(&_fs, _fd, (u8_t *)&data[0], pkg_len);
        if (SPIFFS_errno(&_fs) == 0) {
            return 0;

        } else {

            NRF_LOG_INFO("read errno %i\r\n", SPIFFS_errno(&_fs));
            return -1;

        }
    } else {
        return 1;
    }

}

int8_t log_sync(void)
{
    uint8_t data[20] = {0xC0, 0x10};
    if (!SPIFFS_eof(&_fs, _fd)){

        SPIFFS_read(&_fs, _fd, (u8_t *)&data[2], sizeof(log_imu_t));
        if (SPIFFS_errno(&_fs) == 0) {
            if (sync_tx != NULL) sync_tx(data);
            NRF_LOG_INFO("sync..."); 
            int i= 0;
            for (i=0; i< sizeof(log_imu_t)+2; i++){
                nrf_delay_ms(2);
                NRF_LOG_INFO("0x%X ", data[i]);
            }
            NRF_LOG_INFO("\r\n");
            return 0;
        }else{
            NRF_LOG_INFO("read errno %i\r\n", SPIFFS_errno(&_fs));
            return -1;
        }
    }else{
        data[0] = 0xC0;
        data[1] = 0x20;
        if (sync_tx != NULL) sync_tx(data);
        NRF_LOG_INFO("[log] reach eof and sync done \r\n");
        return 1;
    
    }
}

void log_clear(void)
{
    NRF_LOG_INFO("[log] clear\r\n");
    SPIFFS_remove(&_fs, imu_log_name); 
    SPIFFS_creat(&_fs, imu_log_name, SPIFFS_RDWR);
}

void log_erase(void)
{
    flash_chip_erase();
    log_init();
}

void log_standby(void)
{
    flash_uninit();
}

void log_wakeup(void)
{
    flash_init();
}
