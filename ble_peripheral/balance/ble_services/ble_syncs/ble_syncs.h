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
 * @defgroup ble_sdk_srv_lbs LED Button Service
 * @{
 * @ingroup ble_sdk_srv
 *
 * @brief LED Button Service module.
 *
 * @details This module implements a custom LED Button Service with an LED and Button Characteristics.
 *          During initialization, the module adds the LED Button Service and Characteristics
 *          to the BLE stack database.
 *
 *          The application must supply an event handler for receiving LED Button Service
 *          events. Using this handler, the service notifies the application when the
 *          LED value changes.
 *
 *          The service also provides a function for letting the application notify
 *          the state of the Button Characteristic to connected peers.
 *
 * @note The application must propagate BLE stack events to the LED Button Service
 *       module by calling ble_lbs_on_ble_evt() from the @ref softdevice_handler callback.
*/

#ifndef BLE_SYNC_H__
#define BLE_SYNC_H__

#include <stdint.h>
#include <stdbool.h>
//#include "handler.h"
#include "ble.h"
#include "ble_srv_common.h"

//#define SYNC_UUID_BASE        {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, //
//                              0xDE, 0xEF, 0x12, 0x13, 0x00, 0x00, 0x00, 0x00}
#define SYNC_UUID_BASE        {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, \
                              0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
//#define SYNC_UUID_SERVICE     0x1523
#define SYNC_UUID_SERVICE     0xFA40
#define SYNC_UUID_MOV_CHAR    0xFA45

#define SETTING_UUID_CMD_CHAR 0xFA42
#define SYNC_UUID_CMD_CHAR    0xFA43
#define TEST_UUID_CMD_CHAR    0xFA4F

#define SETTING_CMD_CHAR_SIZE     5
#define SYNC_MOV_CHAR_SIZE    20    
#define SYNC_CMD_CHAR_SIZE    sizeof(uint8_t)
#define TEST_CMD_CHAR_SIZE    sizeof(uint16_t)

// Forward declaration of the ble_sync_t type.
typedef struct ble_sync_s ble_sync_t;

typedef void (*ble_sync_write_handler_t) (ble_sync_t * p_sync, uint8_t value);
typedef void (*ble_test_write_handler_t) (ble_sync_t * p_sync, uint8_t * value);
typedef void (*ble_setting_write_handler_t) (ble_sync_t * p_sync, uint8_t* value);

/** @brief LED Button Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_sync_write_handler_t sync_write_handler; /**< Event handler to be called when the SYNC Characteristic is written. */
    ble_test_write_handler_t test_write_handler; /**< Event handler to be called when the SYNC Characteristic is written. */
    ble_setting_write_handler_t setting_write_handler; /**< Event handler to be called when the SYNC Characteristic is written. */
} ble_sync_init_t;

/**@brief LED Button Service structure. This structure contains various status information for the service. */
struct ble_sync_s
{
    uint16_t                    service_handle;      /**< Handle of Inertial Measurement Unit Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    dat_char_handles;  /**< Handles related to the sync data Characteristic. */
    ble_gatts_char_handles_t    cmd_char_handles;    /**< Handles related to the command. */

    ble_gatts_char_handles_t    test_char_handles;    /**< Handles related to the command. */
    ble_gatts_char_handles_t    setting_char_handles;    /**< Handles related to the command. */
    uint8_t                     uuid_type;           /**< UUID type for the Inertial Measurement Unit Service. */
    uint16_t                    conn_handle;         /**< Handle of the current connection (as provided by the BLE stack). BLE_CONN_HANDLE_INVALID if not in a connection. */
    ble_sync_write_handler_t     sync_write_handler;   /**< Event handler to be called when the sync Characteristic is written. */
    ble_test_write_handler_t     test_write_handler;   /**< Event handler to be called when the sync Characteristic is written. */
    ble_setting_write_handler_t     setting_write_handler;   /**< Event handler to be called when the sync Characteristic is written. */
};

/**@brief Function for initializing the LED Button Service.
 *
 * @param[out] p_lbs      LED Button Service structure. This structure must be supplied by
 *                        the application. It is initialized by this function and will later
 *                        be used to identify this particular service instance.
 * @param[in] p_lbs_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was initialized successfully. Otherwise, an error code is returned.
 */
uint32_t ble_sync_init(ble_sync_t * p_sync, const ble_sync_init_t * p_sync_init);

/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the LED Button Service.
 *
 * @param[in] p_lbs      LED Button Service structure.
 * @param[in] p_ble_evt  Event received from the BLE stack.
 */
void ble_sync_on_ble_evt(ble_sync_t * p_sync, ble_evt_t * p_ble_evt);

/**@brief Function for sending a button state notification.
 *
 * @param[in] p_lbs      LED Button Service structure.
 * @param[in] button_state  New button state.
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */

uint32_t ble_sync_log_update(ble_sync_t * p_sync, uint8_t *data);

#endif // BLE_SYNC_H__

/** @} */
