/**
  ******************************************************************************
  * @file    usbd_hid_core.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                HID Class  Description
  *          =================================================================== 
  *           This module manages the HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - The Mouse protocol
  *             - Usage Page : Generic Desktop
  *             - Usage : Joystick)
  *             - Collection : Application 
  *      
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_hid_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
	extern u8 send_flag_ep1;
  extern u8 send_flag_ep2;
	extern u8 HIDRxBuf[64*4];
	extern u8 MassBuf[4096];
/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_HID 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_HID_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_HID_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_HID_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 




/** @defgroup USBD_HID_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_HID_Init (void  *pdev, 
                               uint8_t cfgidx);

static uint8_t  USBD_HID_DeInit (void  *pdev, 
                                 uint8_t cfgidx);

static uint8_t  USBD_HID_Setup (void  *pdev, 
                                USB_SETUP_REQ *req);

static uint8_t  *USBD_HID_GetCfgDesc (uint8_t speed, uint16_t *length);

static uint8_t  USBD_HID_DataIn (void  *pdev, uint8_t epnum);
static uint8_t  USBD_HID_DataOut (void *pdev, uint8_t epnum);
/**
  * @}
  */ 

/** @defgroup USBD_HID_Private_Variables
  * @{
  */ 

USBD_Class_cb_TypeDef  USBD_HID_cb = 
{
  USBD_HID_Init,
  USBD_HID_DeInit,
  USBD_HID_Setup,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  USBD_HID_DataIn, /*DataIn*/
  USBD_HID_DataOut, /*DataOut*/
  NULL, /*SOF */
  NULL,
  NULL,      
  USBD_HID_GetCfgDesc,
#ifdef USB_OTG_HS_CORE  
  USBD_HID_GetCfgDesc, /* use same config as per FS */
#endif  
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */        
__ALIGN_BEGIN static uint32_t  USBD_HID_AltSet  __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */      
__ALIGN_BEGIN static uint32_t  USBD_HID_Protocol  __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */  
__ALIGN_BEGIN static uint32_t  USBD_HID_IdleState __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */ 
/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_CfgDesc[USB_HID_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
  
  /************** Descriptor of  interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x04,         /*bNumEndpoints*/ //�˵�1 �жϣ��˵�2 bulk
  0x00,         /*bInterfaceClass*/ //hid 0x03
  0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
	
  /******************** Descriptor of  endpoint ********************/
  /* 18 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
  IN_EP1,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x02,          /*bmAttributes: Interrupt endpoint*/
  EP2_IN_PACKET, /*wMaxPacketSize: 0X40 Byte max */
  0x00,
  0x00,          /* bInterval: Polling Interval  */ //�˵��ѯ��ʱ�䣬��������Ϊ10��֡ʱ�䣬��10ms��
  /* 25*/
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/  
  OUT_EP1,     /*bEndpointAddress: Endpoint Address (OUT)*/
  0x02,          /*bmAttributes: Interrupt endpoint*/
  EP1_OUT_PACKET, /*wMaxPacketSize: 0x40 Byte max */
  0x00,
  0x00,          /* bInterval: Polling Interval  */
  /* 32 */
	
  /*************** ��������˵�2������ ******************/
 //bLength�ֶΡ��˵�����������Ϊ7�ֽڡ�
 0x07,
 
 //bDescriptorType�ֶΡ��˵����������Ϊ0x05��
 USB_ENDPOINT_DESCRIPTOR_TYPE,
 
 //bEndpointAddress�ֶΡ��˵�ĵ�ַ������ʹ��D12������˵�2��
 //D7λ��ʾ���ݷ�������˵�D7Ϊ1����������˵�2�ĵ�ַΪ0x82��
 IN_EP2,
 
 //bmAttributes�ֶΡ�D1~D0Ϊ�˵㴫������ѡ��
 //�ö˵�Ϊ�����˵㣬�����˵�ı��Ϊ0x02������λ����Ϊ0��
 0x02,
 
 //wMaxPacketSize�ֶΡ��ö˵�����������˵�2��������Ϊ64�ֽڡ�
 //ע����ֽ����ȡ�
 0x40,
 0x00,
 
 //bInterval�ֶΡ��˵��ѯ��ʱ�䣬����������˵���Ч��
 0x00,
 
 /*************** ��������˵�2������ ******************/
 //bLength�ֶΡ��˵�����������Ϊ7�ֽڡ�
 0x07,
 
 //bDescriptorType�ֶΡ��˵����������Ϊ0x05��
 USB_ENDPOINT_DESCRIPTOR_TYPE,
 
 //bEndpointAddress�ֶΡ��˵�ĵ�ַ������ʹ��D12������˵�2��
 //D7λ��ʾ���ݷ�������˵�D7Ϊ0����������˵�2�ĵ�ַΪ0x02��
 OUT_EP2,
 
 //bmAttributes�ֶΡ�D1~D0Ϊ�˵㴫������ѡ��
 //�ö˵�Ϊ�����˵㣬�����˵�ı��Ϊ0x02������λ����Ϊ0��
 0x02,
 
 //wMaxPacketSize�ֶΡ��ö˵�����������˵�2��������Ϊ64�ֽڡ�
 //ע����ֽ����ȡ�
 0x40,
 0x00,
 
 //bInterval�ֶΡ��˵��ѯ��ʱ�䣬����������˵���Ч��
 0x00
 // 46
} ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ] __ALIGN_END=
{
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  DATASENDER_SIZ_REPORT_DESC,/*wItemLength: Total length of Report descriptor*/
  0x00,
};
#endif 


#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */  
__ALIGN_BEGIN static uint8_t HID_MOUSE_ReportDesc[DATASENDER_SIZ_REPORT_DESC] __ALIGN_END =
{
 0x05, 0x8c, /* ��;ҳ��USAGE_PAGE (ST Page) */ 
0x09, 0x00, /* USAGE (Demo Kit) */ 
0xa1, 0x01, /* COLLECTION (Application) */ 
/* 6 */ 

// The Input report 
0x09,0x03, // USAGE ID - Vendor defined 
0x15,0x00, // �߼���СֵΪ0��LOGICAL_MINIMUM (0) 
0x26,0x00, 0xFF, // �߼����ֵ (255) 
0x75,0x08, // 8λ���ݣ���1���ֽڣ�REPORT_SIZE (8) 
0x95,0x40, // 40�����ݣ�REPORT_COUNT (20) 
0x81,0x02, // INPUT (Data,Var,Abs) 
//19
// The Output report 
0x09,0x04, // USAGE ID - Vendor defined 
0x15,0x00, // LOGICAL_MINIMUM (0) 
0x26,0x00,0xFF, // LOGICAL_MAXIMUM (255) 
0x75,0x08, // REPORT_SIZE (8) 
0x95,0x40, // REPORT_COUNT (20) 
0x91,0x02, // OUTPUT (Data,Var,Abs) 
//32
// The Feature report
/*
0x09, 0x05, // USAGE ID - Vendor defined 
0x15,0x00, // LOGICAL_MINIMUM (0) 
0x26,0x00,0xFF, // LOGICAL_MAXIMUM (255) 
0x75,0x08, // REPORT_SIZE (8) 
0x95,0x02, // REPORT_COUNT (2) 
0xB1,0x02, 
*/
/* 45 */ 
0xc0 /* END_COLLECTION */ 
//#endif 
}; 

/**
  * @}
  */ 

/** @defgroup USBD_HID_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_HID_Init
  *         Initialize the HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_Init (void  *pdev, 
                               uint8_t cfgidx)
{
   //INT
  /* Open EP1 IN */
  DCD_EP_Open(pdev,
              IN_EP1,
              EP1_IN_PACKET,
              USB_OTG_EP_INT);
  
  /* Open EP1 OUT */
  DCD_EP_Open(pdev,
              OUT_EP1,
              EP1_OUT_PACKET,
              USB_OTG_EP_INT);
	 //BULK
	  /* Open EP2 IN */
  DCD_EP_Open(pdev,
              IN_EP2,
              EP2_IN_PACKET,
              USB_OTG_EP_BULK);
  
  /* Open EP2 OUT */
  DCD_EP_Open(pdev,
              OUT_EP2,
              EP2_OUT_PACKET,
              USB_OTG_EP_BULK);
							
	DCD_EP_PrepareRx(pdev,
         OUT_EP1,
   (uint8_t*)(HIDRxBuf),
   EP1_IN_PACKET);
	 
	 
	 DCD_EP_PrepareRx(pdev,
         OUT_EP2,
   (uint8_t*)(MassBuf),
   EP2_IN_PACKET);
  
  return USBD_OK;
}

/**
  * @brief  USBD_HID_Init
  *         DeInitialize the HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_DeInit (void  *pdev, 
                                 uint8_t cfgidx)
{
  /* Close HID EPs */
  DCD_EP_Close (pdev , IN_EP1);
  DCD_EP_Close (pdev , OUT_EP1);
  DCD_EP_Close (pdev , IN_EP2);
  DCD_EP_Close (pdev , OUT_EP2);
  
  
  return USBD_OK;
}

/**
  * @brief  USBD_HID_Setup
  *         Handle the HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_HID_Setup (void  *pdev, 
                                USB_SETUP_REQ *req)
{
  uint16_t len = 0;
  uint8_t  *pbuf = NULL;
  
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
    switch (req->bRequest)
    {
      
      
    case HID_REQ_SET_PROTOCOL:
      USBD_HID_Protocol = (uint8_t)(req->wValue);
      break;
      
    case HID_REQ_GET_PROTOCOL:
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&USBD_HID_Protocol,
                        1);    
      break;
      
    case HID_REQ_SET_IDLE:
      USBD_HID_IdleState = (uint8_t)(req->wValue >> 8);
      break;
      
    case HID_REQ_GET_IDLE:
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&USBD_HID_IdleState,
                        1);        
      break;      
      
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL; 
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR: 
      if( req->wValue >> 8 == HID_REPORT_DESC)
      {
        len = MIN(DATASENDER_SIZ_REPORT_DESC , req->wLength);
        pbuf = HID_MOUSE_ReportDesc;
      }
      else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
      {
        
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
        pbuf = USBD_HID_Desc;   
#else
        pbuf = USBD_HID_CfgDesc + 0x12;
#endif 
        len = MIN(USB_HID_DESC_SIZ , req->wLength);
      }
      
      USBD_CtlSendData (pdev, 
                        pbuf,
                        len);
      
      break;
      
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        (uint8_t *)&USBD_HID_AltSet,
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      USBD_HID_AltSet = (uint8_t)(req->wValue);
      break;
    }
  }
  return USBD_OK;
}

/**
  * @brief  USBD_HID_SendReport 
  *         Send HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_Bulk_Send_ep1 (USB_OTG_CORE_HANDLE  *pdev, 
                                 uint8_t *report,
                                 uint16_t len)
{
  if (pdev->dev.device_status == USB_OTG_CONFIGURED )
  {
    DCD_EP_Tx (pdev, IN_EP1, report, len);
  }
  return USBD_OK;
}

 uint8_t USBD_Bulk_Send_ep2(USB_OTG_CORE_HANDLE  *pdev, 
                                 uint8_t *buf,
                                 uint16_t len)
{
	if (pdev->dev.device_status == USB_OTG_CONFIGURED )
  {
    DCD_EP_Tx (pdev, IN_EP2, buf, len);
  }
  return USBD_OK;
}	
	
/**
  * @brief  USBD_HID_GetCfgDesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_HID_GetCfgDesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (USBD_HID_CfgDesc);
  return USBD_HID_CfgDesc;
}

/**
  * @brief  USBD_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_HID_DataIn (void  *pdev, 
                              uint8_t epnum)
{
  
  /* Ensure that the FIFO is empty before a new transfer, this condition could 
  be caused by  a new transfer before the end of the previous transfer */
	DCD_EP_Flush(pdev, IN_EP1);
	DCD_EP_Flush(pdev, IN_EP2);
	return USBD_OK;
}

static uint8_t  USBD_HID_DataOut (void *vpdev,uint8_t epnum)
{
	USB_OTG_CORE_HANDLE *pdev = (USB_OTG_CORE_HANDLE *) vpdev;
	USB_OTG_EP  *ep;
	ep = &(pdev->dev.out_ep[OUT_EP1]);
	if(ep->xfer_count>=64)
	{ 
		DCD_EP_PrepareRx(pdev, OUT_EP1, (uint8_t*) HIDRxBuf, EP1_IN_PACKET);
		send_flag_ep1=1;		 
	}
	 
	ep = &(pdev->dev.out_ep[OUT_EP2]);
	if(ep->xfer_count>=64)
	{
		DCD_EP_PrepareRx(pdev, OUT_EP2,(uint8_t*) MassBuf, EP2_IN_PACKET);	
		send_flag_ep2=1;			 
	}

	return USBD_OK;
}

/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
