#include "flash_spi_master.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "app_error.h"
#include "app_util_platform.h"
//#include "app_trace.h"
#include "sdk_config.h"
#include "nrf_log.h"

static volatile bool m_transfer_running = false; /**< A flag to inform about completed transfer. */
static const nrf_drv_spi_t m_spi_instance = NRF_DRV_SPI_INSTANCE(0);
//static const nrf_drv_spi_config_t m_spi_config = NRF_DRV_SPI_DEFAULT_CONFIG(0);
static nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;

/**@brief Function for SPI master event callback.
*
* Upon receiving an SPI transaction complete event, checks if received data are valid.
*
* @param[in] spi_master_evt    SPI master driver event.
*/
static void spi_master_event_handler(nrf_drv_spi_evt_t const * event)
{
  switch (event->type)
  {
      case NRF_DRV_SPI_EVENT_DONE:
          // Inform application that transfer is completed.
          m_transfer_running = false;
          break;
    
      default:
          // No implementation needed.
          break;
  }
}

/**@brief Functions prepares buffers and starts data transfer.
*
* @param[in] p_tx_data     A pointer to a buffer TX.
* @param[in] p_rx_data     A pointer to a buffer RX.
* @param[in] len           A length of the data buffers.
*/
void flash_spi_send_recv(uint8_t * const p_tx_data,
                       uint8_t * const p_rx_data,
                       const uint16_t  len)
{
  m_transfer_running = true;
  // Start transfer.
  uint32_t err_code = nrf_drv_spi_transfer(&m_spi_instance,
                                           p_tx_data,
                                           len,
                                           p_rx_data,
                                           len);
  APP_ERROR_CHECK(err_code);
  // Wait until transfer complete.
  while(m_transfer_running){
  }
}

void flash_spi_master_init(void)
{
  ret_code_t err_code;
  //spi_config.ss_pin   = SPI_SS_PIN;
  spi_config.miso_pin = SPI_MISO_PIN;
  spi_config.mosi_pin = SPI_MOSI_PIN;
  spi_config.sck_pin  = SPI_SCK_PIN;
 
  err_code = nrf_drv_spi_init(&m_spi_instance,
                              &spi_config,
                              spi_master_event_handler);
  APP_ERROR_CHECK(err_code);
}

void flash_spi_master_uninit(void)
{
  nrf_drv_spi_uninit(&m_spi_instance);
  nrf_gpio_cfg_input(spi_config.miso_pin, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(spi_config.mosi_pin, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(spi_config.sck_pin, NRF_GPIO_PIN_PULLUP);
}
