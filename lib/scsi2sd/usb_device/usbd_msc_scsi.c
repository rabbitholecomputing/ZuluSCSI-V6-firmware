/**
  ******************************************************************************
  * @file    usbd_msc_scsi.c
  * @author  MCD Application Team
  * @version V2.4.1
  * @date    19-June-2015
  * @brief   This file provides all the USBD SCSI layer functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
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
#include "usbd_composite.h"
#include "usbd_msc_bot.h"
#include "usbd_msc_scsi.h"
#include "usbd_msc.h"
#include "usbd_msc_data.h"



/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup MSC_SCSI 
  * @brief Mass storage SCSI layer module
  * @{
  */ 

/** @defgroup MSC_SCSI_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Variables
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_FunctionPrototypes
  * @{
  */ 
static int8_t SCSI_TestUnitReady(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_Inquiry(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ReadFormatCapacity(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ReadCapacity10(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_RequestSense (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_StartStopUnit(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ModeSense6 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ModeSense10 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_Write10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params);
static int8_t SCSI_Read10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params);
static int8_t SCSI_Verify10(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_CheckAddressRange (USBD_HandleTypeDef  *pdev, 
                                      uint8_t lun , 
                                      uint32_t blk_offset , 
                                      uint16_t blk_nbr);
static int8_t SCSI_ProcessRead (USBD_HandleTypeDef  *pdev,
                                uint8_t lun);

static int8_t SCSI_ProcessWrite (USBD_HandleTypeDef  *pdev,
                                 uint8_t lun);
/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Functions
  * @{
  */ 


/**
* @brief  SCSI_ProcessCmd
*         Process SCSI commands
* @param  pdev: device instance
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
int8_t SCSI_ProcessCmd(USBD_HandleTypeDef  *pdev,
                           uint8_t lun, 
                           uint8_t *params)
{
  
  switch (params[0])
  {
  case SCSI_TEST_UNIT_READY:
    return SCSI_TestUnitReady(pdev, lun, params);
    
  case SCSI_REQUEST_SENSE:
    return SCSI_RequestSense (pdev, lun, params);
  case SCSI_INQUIRY:
    return SCSI_Inquiry(pdev, lun, params);
    
  case SCSI_START_STOP_UNIT:
    return SCSI_StartStopUnit(pdev, lun, params);
    
  case SCSI_ALLOW_MEDIUM_REMOVAL:
    return SCSI_StartStopUnit(pdev, lun, params);
    
  case SCSI_MODE_SENSE6:
    return SCSI_ModeSense6 (pdev, lun, params);
    
  case SCSI_MODE_SENSE10:
    return SCSI_ModeSense10 (pdev, lun, params);
    
  case SCSI_READ_FORMAT_CAPACITIES:
    return SCSI_ReadFormatCapacity(pdev, lun, params);
    
  case SCSI_READ_CAPACITY10:
    return SCSI_ReadCapacity10(pdev, lun, params);
    
  case SCSI_READ10:
    return SCSI_Read10(pdev, lun, params); 
    
  case SCSI_WRITE10:
    return SCSI_Write10(pdev, lun, params);
    
  case SCSI_VERIFY10:
    return SCSI_Verify10(pdev, lun, params);
    
  default:
    SCSI_SenseCode(pdev, 
                   lun,
                   ILLEGAL_REQUEST, 
                   INVALID_CDB);    

    {
        USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
        USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);
        hmsc->bot_state = USBD_BOT_NO_DATA;
    }

    return -1;
  }
}


/**
* @brief  SCSI_TestUnitReady
*         Process SCSI Test Unit Ready Command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_TestUnitReady(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);
    
  /* case 9 : Hi > D0 */
  if (hmsc->cbw.dDataLength != 0)
  {
    SCSI_SenseCode(pdev,
                   hmsc->cbw.bLUN, 
                   ILLEGAL_REQUEST, 
                   INVALID_CDB);
    return -1;
  }  
  
  if(((USBD_StorageTypeDef *)pdev->pUserData)->IsReady(lun) !=0 )
  {
    SCSI_SenseCode(pdev,
                   lun,
                   NOT_READY, 
                   MEDIUM_NOT_PRESENT);
    
    hmsc->bot_state = USBD_BOT_NO_DATA;
    return -1;
  } 
  hmsc->bot_data_length = 0;
  return 0;
}

/**
* @brief  SCSI_Inquiry
*         Process Inquiry command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t  SCSI_Inquiry(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  uint8_t* pPage;
  uint16_t len;

	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);
  
  if (params[1] & 0x01)/*Evpd is set*/
  {
    pPage = (uint8_t *)MSC_Page00_Inquiry_Data;
    len = LENGTH_INQUIRY_PAGE00;
    if (params[4] <= len)
    {
      len = params[4];
    }
	memcpy(hmsc->bot_data, pPage, len);
  }
  else
  {
  	len = ((USBD_StorageTypeDef *)pdev->pUserData)->Inquiry(lun, hmsc->bot_data, params[4]);
    
    if (params[4] <= len)
    {
      len = params[4];
    }
  }
  hmsc->bot_data_length = len;

  return 0;
}

/**
* @brief  SCSI_ReadCapacity10
*         Process Read Capacity 10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ReadCapacity10(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);
  
  if(((USBD_StorageTypeDef *)pdev->pUserData)->GetCapacity(lun, &hmsc->scsi_blk_nbr, &hmsc->scsi_blk_size) != 0)
  {
    memset(hmsc->bot_data, 0, 8);
    if (hmsc->bot_state == USBD_BOT_DATA_IN)
    {
        if (hmsc->bot_data_length > 0)
        {
            USBD_LL_Transmit (pdev, 
                MSC_EPIN_ADDR,
                hmsc->bot_data,
                hmsc->bot_data_length);
            hmsc->csw.dDataResidue -= hmsc->bot_data_length;
            hmsc->bot_data_length = 0;
            return 0;
        }
        else
        {
            return -1; // Time to send the error.
        }
    }
    else
    {
        SCSI_SenseCode(pdev,
                   lun,
                   NOT_READY, 
                   MEDIUM_NOT_PRESENT);

        // Don't send the error just yet. Microsoft Windows fails to detect the CSW
        // prior to the 8 byte response is sent. Windows also insists on calling
        // ReadCapacity even when TestUnitReady fails with MEDIUM_NOT_PRESENT
        hmsc->bot_state = USBD_BOT_DATA_IN;
        hmsc->bot_data_length = MIN(8, hmsc->csw.dDataResidue);
        return 0;
    }
  } 
  else
  {
    
    hmsc->bot_data[0] = (uint8_t)((hmsc->scsi_blk_nbr - 1) >> 24);
    hmsc->bot_data[1] = (uint8_t)((hmsc->scsi_blk_nbr - 1) >> 16);
    hmsc->bot_data[2] = (uint8_t)((hmsc->scsi_blk_nbr - 1) >>  8);
    hmsc->bot_data[3] = (uint8_t)(hmsc->scsi_blk_nbr - 1);
    
    hmsc->bot_data[4] = (uint8_t)(hmsc->scsi_blk_size >>  24);
    hmsc->bot_data[5] = (uint8_t)(hmsc->scsi_blk_size >>  16);
    hmsc->bot_data[6] = (uint8_t)(hmsc->scsi_blk_size >>  8);
    hmsc->bot_data[7] = (uint8_t)(hmsc->scsi_blk_size);
    
    hmsc->bot_data_length = 8;
    return 0;
  }
}
/**
* @brief  SCSI_ReadFormatCapacity
*         Process Read Format Capacity command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ReadFormatCapacity(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);
  
  uint16_t blk_size;
  uint32_t blk_nbr;
  uint16_t i;
  
  for(i=0 ; i < 12 ; i++) 
  {
    hmsc->bot_data[i] = 0;
  }
  
  if(((USBD_StorageTypeDef *)pdev->pUserData)->GetCapacity(lun, &blk_nbr, &blk_size) != 0)
  {
    // Capacity List Header
    // [0] Reserved
    // [1] Reserved
    // [2] Reserved
    
    hmsc->bot_data[3] = 0x08; // Capacity List Length (8 bytes, 1 descriptor)

    // Number of blocks. MAXIMUM
    // 0x400000 is 2TB worth of 512 blocks.
    hmsc->bot_data[4] = 0x00;
    hmsc->bot_data[5] = 0x3F;
    hmsc->bot_data[6] = 0xFF;
    hmsc->bot_data[7] = 0xFF;
    
    hmsc->bot_data[8] = 0x03; // Descriptor code - No media.
    hmsc->bot_data[9] = 0x00;
    hmsc->bot_data[10] = 0x02; // 0x200 512 bytes
    hmsc->bot_data[11] = 0x00;
  } 
  else
  {
    hmsc->bot_data[3] = 0x08;
    hmsc->bot_data[4] = (uint8_t)((blk_nbr - 1) >> 24);
    hmsc->bot_data[5] = (uint8_t)((blk_nbr - 1) >> 16);
    hmsc->bot_data[6] = (uint8_t)((blk_nbr - 1) >>  8);
    hmsc->bot_data[7] = (uint8_t)(blk_nbr - 1);
    
    hmsc->bot_data[8] = 0x02; // Descriptor code - Formatted media
    hmsc->bot_data[9] = (uint8_t)(blk_size >>  16);
    hmsc->bot_data[10] = (uint8_t)(blk_size >>  8);
    hmsc->bot_data[11] = (uint8_t)(blk_size);
  }
    
    hmsc->bot_data_length = 12;
    return 0;
}

/**
* @brief  SCSI_ModeSense6
*         Process Mode Sense6 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ModeSense6 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);

  uint16_t len = 8 ;
  hmsc->bot_data_length = len;
  
  while (len) 
  {
    len--;
    hmsc->bot_data[len] = MSC_Mode_Sense6_data[len];
  }
  return 0;
}

/**
* @brief  SCSI_ModeSense10
*         Process Mode Sense10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ModeSense10 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  uint16_t len = 8;
	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);
  
  hmsc->bot_data_length = len;

  while (len) 
  {
    len--;
    hmsc->bot_data[len] = MSC_Mode_Sense10_data[len];
  }
  return 0;
}

/**
* @brief  SCSI_RequestSense
*         Process Request Sense command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/

static int8_t SCSI_RequestSense (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  uint8_t i;
	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);
  
  for(i=0 ; i < REQUEST_SENSE_DATA_LEN ; i++) 
  {
    hmsc->bot_data[i] = 0;
  }
  
  hmsc->bot_data[0]	= 0x70;		
  hmsc->bot_data[7]	= REQUEST_SENSE_DATA_LEN - 6;	
  
  if((hmsc->scsi_sense_head != hmsc->scsi_sense_tail)) {
    
    hmsc->bot_data[2]     = hmsc->scsi_sense[hmsc->scsi_sense_head].Skey;		
    hmsc->bot_data[12]    = hmsc->scsi_sense[hmsc->scsi_sense_head].w.b.ASCQ;	
    hmsc->bot_data[13]    = hmsc->scsi_sense[hmsc->scsi_sense_head].w.b.ASC;	
    hmsc->scsi_sense_head++;
    
    if (hmsc->scsi_sense_head == SENSE_LIST_DEEPTH)
    {
      hmsc->scsi_sense_head = 0;
    }
  }
  hmsc->bot_data_length = REQUEST_SENSE_DATA_LEN;  
  
  if (params[4] <= REQUEST_SENSE_DATA_LEN)
  {
    hmsc->bot_data_length = params[4];
  }
  return 0;
}

/**
* @brief  SCSI_SenseCode
*         Load the last error code in the error list
* @param  lun: Logical unit number
* @param  sKey: Sense Key
* @param  ASC: Additional Sense Key
* @retval none

*/
void SCSI_SenseCode(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t sKey, uint8_t ASC)
{
	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);
  
  hmsc->scsi_sense[hmsc->scsi_sense_tail].Skey  = sKey;
  hmsc->scsi_sense[hmsc->scsi_sense_tail].w.ASC = ASC << 8;
  hmsc->scsi_sense_tail++;
  if (hmsc->scsi_sense_tail == SENSE_LIST_DEEPTH)
  {
    hmsc->scsi_sense_tail = 0;
  }
}
/**
* @brief  SCSI_StartStopUnit
*         Process Start Stop Unit command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_StartStopUnit(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);

  hmsc->bot_data_length = 0;
  return 0;
}

/**
* @brief  SCSI_Read10
*         Process Read10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_Read10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params)
{
	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);
  
  if(hmsc->bot_state == USBD_BOT_IDLE)  /* Idle */
  {
    
    /* case 10 : Ho <> Di */
    
    if ((hmsc->cbw.bmFlags & 0x80) != 0x80)
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }    
    
    if(((USBD_StorageTypeDef *)pdev->pUserData)->IsReady(lun) !=0 )
    {
      SCSI_SenseCode(pdev,
                     lun,
                     NOT_READY, 
                     MEDIUM_NOT_PRESENT);
      return -1;
    } 
    
    hmsc->scsi_blk_addr = ((uint32_t)params[2] << 24) | \
      ((uint32_t)params[3] << 16) | \
        ((uint32_t)params[4] <<  8) | \
          (uint32_t)params[5];
    
    hmsc->scsi_blk_len =  ((uint32_t)params[7] <<  8) | \
      (uint32_t)params[8];  
    
    
    
    if( SCSI_CheckAddressRange(pdev, lun, hmsc->scsi_blk_addr, hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */
    }
    
    hmsc->bot_state = USBD_BOT_DATA_IN;
    
    /* cases 4,5 : Hi <> Dn */
    if (hmsc->cbw.dDataLength != (hmsc->scsi_blk_len * hmsc->scsi_blk_size))
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }
  }
  hmsc->bot_data_length = S2S_MSC_MEDIA_PACKET;  
  
  return SCSI_ProcessRead(pdev, lun);
}

/**
* @brief  SCSI_Write10
*         Process Write10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/

static int8_t SCSI_Write10 (USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params)
{
  USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
  USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);
  uint32_t len;
  
  if (hmsc->bot_state == USBD_BOT_IDLE) /* Idle */
  {
    /* case 8 : Hi <> Do */
    
    if ((hmsc->cbw.bmFlags & 0x80) == 0x80)
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }
    
    /* Check whether Media is ready */
    if(((USBD_StorageTypeDef *)pdev->pUserData)->IsReady(lun) !=0 )
    {
      SCSI_SenseCode(pdev,
                     lun,
                     NOT_READY, 
                     MEDIUM_NOT_PRESENT);
      return -1;
    } 
    
    /* Check If media is write-protected */
    if(((USBD_StorageTypeDef *)pdev->pUserData)->IsWriteProtected(lun) !=0 )
    {
      SCSI_SenseCode(pdev,
                     lun,
                     DATA_PROTECT,
                     WRITE_PROTECTED);
      return -1;
    } 
    
    
    hmsc->scsi_blk_addr = ((uint32_t)params[2] << 24) | \
      ((uint32_t)params[3] << 16) | \
        ((uint32_t)params[4] <<  8) | \
          (uint32_t)params[5];
    hmsc->scsi_blk_len = ((uint32_t)params[7] <<  8) | \
      (uint32_t)params[8];  
    
    /* check if LBA address is in the right range */
    if(SCSI_CheckAddressRange(pdev,
                              lun,
                              hmsc->scsi_blk_addr,
                              hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */      
    }

    len = hmsc->scsi_blk_len * hmsc->scsi_blk_size;
    
    /* cases 3,11,13 : Hn,Ho <> D0 */
    if (hmsc->cbw.dDataLength != len)
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }

    len = MIN(len, S2S_MSC_MEDIA_PACKET);
    
    /* Prepare EP to receive first data packet */
    hmsc->bot_state = USBD_BOT_DATA_OUT;  
    USBD_LL_PrepareReceive (pdev,
                      MSC_EPOUT_ADDR,
                      hmsc->bot_data, 
                      len);  
  }
  else /* Write Process ongoing */
  {
    return SCSI_ProcessWrite(pdev, lun);
  }
  return 0;
}


/**
* @brief  SCSI_Verify10
*         Process Verify10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/

static int8_t SCSI_Verify10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params)
{
	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);
  
  if ((params[1]& 0x02) == 0x02) 
  {
    SCSI_SenseCode (pdev,
                    lun, 
                    ILLEGAL_REQUEST, 
                    INVALID_FIELED_IN_COMMAND);
    return -1; /* Error, Verify Mode Not supported*/
  }
  
  if(SCSI_CheckAddressRange(pdev,
                            lun, 
                            hmsc->scsi_blk_addr, 
                            hmsc->scsi_blk_len) < 0)
  {
    return -1; /* error */      
  }
  hmsc->bot_data_length = 0;
  return 0;
}

/**
* @brief  SCSI_CheckAddressRange
*         Check address range
* @param  lun: Logical unit number
* @param  blk_offset: first block address
* @param  blk_nbr: number of block to be processed
* @retval status
*/
static int8_t SCSI_CheckAddressRange (USBD_HandleTypeDef  *pdev, uint8_t lun , uint32_t blk_offset , uint16_t blk_nbr)
{
	USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
	USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);

	// michael@codesrc.com: Re-check block limits in cause we have different values
	// for different LUN's.
    uint32_t blkNbr;
	uint16_t blkSize;
	if(((USBD_StorageTypeDef *)pdev->pUserData)->GetCapacity(lun, &blkNbr, &blkSize) != 0)
	{
    SCSI_SenseCode(pdev,
                   lun,
                   NOT_READY, 
                   MEDIUM_NOT_PRESENT);
    return -1;
  } 
	// global variables. wooo
	hmsc->scsi_blk_size = blkSize;
	hmsc->scsi_blk_nbr = blkNbr;

  if ((blk_offset + blk_nbr) > blkNbr )
  {
    SCSI_SenseCode(pdev,
                   lun, 
                   ILLEGAL_REQUEST, 
                   ADDRESS_OUT_OF_RANGE);
    return -1;
  }
  return 0;
}

/**
* @brief  SCSI_ProcessRead
*         Handle Read Process
* @param  lun: Logical unit number
* @retval status
*/
static int8_t SCSI_ProcessRead (USBD_HandleTypeDef  *pdev, uint8_t lun)
{
  USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
  USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);

  uint32_t len = hmsc->scsi_blk_len * hmsc->scsi_blk_size;

  len = MIN(len, S2S_MSC_MEDIA_PACKET);

  if( ((USBD_StorageTypeDef *)pdev->pUserData)->Read(lun ,
                              hmsc->bot_data, 
                              hmsc->scsi_blk_addr, 
                              len / hmsc->scsi_blk_size) < 0)
  {
    
    SCSI_SenseCode(pdev,
                   lun, 
                   HARDWARE_ERROR, 
                   UNRECOVERED_READ_ERROR);
    return -1; 
  }
  
  
  USBD_LL_Transmit (pdev, 
             MSC_EPIN_ADDR,
             hmsc->bot_data,
             len);
  
  hmsc->scsi_blk_addr += (len / hmsc->scsi_blk_size);
  hmsc->scsi_blk_len -= (len / hmsc->scsi_blk_size);
  
  /* case 6 : Hi = Di */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  }
  return 0;
}

/**
* @brief  SCSI_ProcessWrite
*         Handle Write Process
* @param  lun: Logical unit number
* @retval status
*/

static int8_t SCSI_ProcessWrite (USBD_HandleTypeDef  *pdev, uint8_t lun)
{
  USBD_CompositeClassData *classData = (USBD_CompositeClassData*) pdev->pClassData;
  USBD_MSC_BOT_HandleTypeDef *hmsc = &(classData->msc);

  uint32_t len = hmsc->scsi_blk_len * hmsc->scsi_blk_size;

  len = MIN(len, S2S_MSC_MEDIA_PACKET);
  
  if(((USBD_StorageTypeDef *)pdev->pUserData)->Write(lun ,
                              hmsc->bot_data, 
                              hmsc->scsi_blk_addr, 
                              len / hmsc->scsi_blk_size) < 0)
  {
    SCSI_SenseCode(pdev,
                   lun, 
                   HARDWARE_ERROR, 
                   WRITE_FAULT);     
    return -1; 
  }
  
  
  hmsc->scsi_blk_addr += (len / hmsc->scsi_blk_size);
  hmsc->scsi_blk_len -= (len / hmsc->scsi_blk_size);
  
  /* case 12 : Ho = Do */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    MSC_BOT_SendCSW (pdev, USBD_CSW_CMD_PASSED);
  }
  else
  {
    len = MIN((hmsc->scsi_blk_len * hmsc->scsi_blk_size), S2S_MSC_MEDIA_PACKET);
    /* Prepare EP to Receive next packet */
    USBD_LL_PrepareReceive (pdev, MSC_EPOUT_ADDR, hmsc->bot_data, len);
  }
  
  return 0;
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
