/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Includes ------------------------------------------------------------------*/
#include "usbd_desc.h"

#include <string.h>

#include "board.h"
#include "usbd_conf.h"
#include "usbd_core.h"
#include "usbd_req.h"
#include "usb_regs.h"


#include "usb_driver.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_DESC
  * @brief USBD descriptors module
  * @{
  */

/** @defgroup USBD_DESC_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_DESC_Private_Defines
  * @{
  */

#define USBD_VID                            0x0483

#define USBD_LANGID_STRING                  0x409
#define USBD_MANUFACTURER_STRING            "OpenTX"
#define USBD_SERIALNUMBER_FS_STRING         "00000000001B"


#if defined(BOOT)
  #define USBD_MSC_PRODUCT_FS_STRING          USB_NAME " Bootloader"
#else
  #define USBD_MSC_PRODUCT_FS_STRING          USB_NAME " Mass Storage"
#endif

#define USBD_MSC_PID                        0x5720
#define USBD_MSC_CONFIGURATION_FS_STRING    "MSC Config"
#define USBD_MSC_INTERFACE_FS_STRING        "MSC Interface"

#define USBD_HID_VID                        0x1209  // https://pid.codes
#define USBD_HID_PID                        0x4F54  // OpenTX assigned PID
#define USBD_HID_PRODUCT_FS_STRING          USB_NAME " Joystick"
#define USBD_HID_CONFIGURATION_FS_STRING    "HID Config"
#define USBD_HID_INTERFACE_FS_STRING        "HID Interface"

#define USBD_CDC_PID                        0x5740      // do not change, this ID is used by the ST USB driver for Windows
#define USBD_CDC_PRODUCT_FS_STRING          USB_NAME " Serial Port"
#define USBD_CDC_CONFIGURATION_FS_STRING    "VSP Config"
#define USBD_CDC_INTERFACE_FS_STRING        "VSP Interface"

const USBD_DEVICE USR_desc =
{
  USBD_USR_DeviceDescriptor,
  USBD_USR_LangIDStrDescriptor,
  USBD_USR_ManufacturerStrDescriptor,
  USBD_USR_ProductStrDescriptor,
  USBD_USR_SerialStrDescriptor,
  USBD_USR_ConfigStrDescriptor,
  USBD_USR_InterfaceStrDescriptor,
};


/* USB Standard Device Descriptor */
__ALIGN_BEGIN const uint8_t USBD_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN const  uint8_t USBD_LangIDDesc[USB_SIZ_STRING_LANGID] __ALIGN_END =
{
     USB_SIZ_STRING_LANGID,
     USB_DESC_TYPE_STRING,
     LOBYTE(USBD_LANGID_STRING),
     HIBYTE(USBD_LANGID_STRING),
};

__ALIGN_BEGIN uint8_t USBD_StrDesc[USB_MAX_STR_DESC_SIZ] __ALIGN_END ;	// modified by OpenTX

/*
* @brief  USBD_USR_DeviceDescriptor
*         return the device descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_USR_DeviceDescriptor( uint8_t speed , uint16_t *length)
{
  int pid = 0;
  int vid = USBD_VID;

  switch (getSelectedUsbMode()) {
    case USB_JOYSTICK_MODE:
      pid = USBD_HID_PID;
      vid = USBD_HID_VID;
      break;
#if !defined(PCBI6X)
    case USB_SERIAL_MODE:
      pid = USBD_CDC_PID;
      break;
#endif
    case USB_MASS_STORAGE_MODE:
      pid = USBD_MSC_PID;
      break;
  }

  /* USB Standard Device Descriptor */
  __ALIGN_BEGIN const uint8_t USBD_DeviceDesc[USB_SIZ_DEVICE_DESC] __ALIGN_END =
    {
      USB_SIZ_DEVICE_DESC,        /*bLength */
      USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
      0x00,                       /*bcdUSB */
      0x02,
      0x00,                       /*bDeviceClass*/
      0x00,                       /*bDeviceSubClass*/
      0x00,                       /*bDeviceProtocol*/
#if defined(STM32F0)
      USB_MAX_EP0_SIZE,       /*bMaxPacketSize*/
#else
      USB_OTG_MAX_EP0_SIZE,       /*bMaxPacketSize*/
#endif
      LOBYTE(vid),               /*idVendor*/
      HIBYTE(vid),               /*idVendor*/
      LOBYTE(pid),               /*idVendor*/
      HIBYTE(pid),               /*idVendor*/
      0x00,                       /*bcdDevice rel. 2.00*/
      0x02,
      USBD_IDX_MFC_STR,           /*Index of manufacturer  string*/
      USBD_IDX_PRODUCT_STR,       /*Index of product string*/
      USBD_IDX_SERIAL_STR,        /*Index of serial number string*/
      USBD_CFG_MAX_NUM            /*bNumConfigurations*/
    }; /* USB_DeviceDescriptor */

  *length = sizeof(USBD_DeviceDesc);
  memcpy(USBD_StrDesc, USBD_DeviceDesc, *length);
  return USBD_StrDesc;
}

/**
* @brief  USBD_USR_LangIDStrDescriptor
*         return the LangID string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_USR_LangIDStrDescriptor( uint8_t speed , uint16_t *length)
{
  *length =  sizeof(USBD_LangIDDesc);
  memcpy(USBD_StrDesc, USBD_LangIDDesc, *length);
  return USBD_StrDesc;
}


/**
* @brief  USBD_USR_ProductStrDescriptor
*         return the product string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_USR_ProductStrDescriptor( uint8_t speed , uint16_t *length)
{
  switch (getSelectedUsbMode()) {
    case USB_JOYSTICK_MODE:
      USBD_GetString ((uint8_t*)USBD_HID_PRODUCT_FS_STRING, USBD_StrDesc, length);
      break;
#if !defined(PCBI6X)
    case USB_SERIAL_MODE:
      USBD_GetString ((uint8_t*)USBD_CDC_PRODUCT_FS_STRING, USBD_StrDesc, length);
      break;
#endif
    case USB_MASS_STORAGE_MODE:
      USBD_GetString ((uint8_t*)USBD_MSC_PRODUCT_FS_STRING, USBD_StrDesc, length);
      break;
  }

  return USBD_StrDesc;
}

/**
* @brief  USBD_USR_ManufacturerStrDescriptor
*         return the manufacturer string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_USR_ManufacturerStrDescriptor( uint8_t speed , uint16_t *length)
{
  USBD_GetString ((uint8_t*)USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
}

/**
* @brief  USBD_USR_SerialStrDescriptor
*         return the serial number string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_USR_SerialStrDescriptor( uint8_t speed , uint16_t *length)
{
  USBD_GetString ((uint8_t*)USBD_SERIALNUMBER_FS_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
}

/**
* @brief  USBD_USR_ConfigStrDescriptor
*         return the configuration string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_USR_ConfigStrDescriptor( uint8_t speed , uint16_t *length)
{
  switch (getSelectedUsbMode()) {
    case USB_JOYSTICK_MODE:
      USBD_GetString ((uint8_t*)USBD_HID_CONFIGURATION_FS_STRING, USBD_StrDesc, length);
      break;
#if !defined(PCBI6X)
    case USB_SERIAL_MODE:
      USBD_GetString ((uint8_t*)USBD_CDC_CONFIGURATION_FS_STRING, USBD_StrDesc, length);
      break;
#endif
    case USB_MASS_STORAGE_MODE:
      USBD_GetString ((uint8_t*)USBD_MSC_CONFIGURATION_FS_STRING, USBD_StrDesc, length);
      break;
  }
  return USBD_StrDesc;
}


/**
* @brief  USBD_USR_InterfaceStrDescriptor
*         return the interface string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_USR_InterfaceStrDescriptor( uint8_t speed , uint16_t *length)
{
  switch (getSelectedUsbMode()) {
    case USB_JOYSTICK_MODE:
      USBD_GetString ((uint8_t*)USBD_HID_INTERFACE_FS_STRING, USBD_StrDesc, length);
      break;
#if !defined(PCBI6X)
    case USB_SERIAL_MODE:
      USBD_GetString ((uint8_t*)USBD_CDC_INTERFACE_FS_STRING, USBD_StrDesc, length);
      break;
#endif
    case USB_MASS_STORAGE_MODE:
      USBD_GetString ((uint8_t*)USBD_MSC_INTERFACE_FS_STRING, USBD_StrDesc, length);
      break;
  }
  return USBD_StrDesc;
}


#if defined(STM32F0)

static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);

uint8_t USBD_StringSerial[USB_SIZ_STRING_SERIAL] =
{
  USB_SIZ_STRING_SERIAL,       /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType */
};

/**
  * @brief  Create the serial number string descriptor 
  * @param  None 
  * @retval None
  */
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
  
  Device_Serial0 = *(uint32_t*)Device1_Identifier;
  Device_Serial1 = *(uint32_t*)Device2_Identifier;
  Device_Serial2 = *(uint32_t*)Device3_Identifier;
  
  Device_Serial0 += Device_Serial2;
  
  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &USBD_StringSerial[2] ,8);
    IntToUnicode (Device_Serial1, &USBD_StringSerial[18] ,4);
  }
}

/**
  * @brief  Convert Hex 32Bits value into char 
  * @param  value: value to convert
  * @param  pbuf: pointer to the buffer 
  * @param  len: buffer length
  * @retval None
  */
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
