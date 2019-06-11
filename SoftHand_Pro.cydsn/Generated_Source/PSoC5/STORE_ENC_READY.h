/*******************************************************************************
* File Name: STORE_ENC_READY.h  
* Version 1.90
*
* Description:
*  This file containts Status Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_STATUS_REG_STORE_ENC_READY_H) /* CY_STATUS_REG_STORE_ENC_READY_H */
#define CY_STATUS_REG_STORE_ENC_READY_H

#include "cyfitter.h"

#if ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC4) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5))
    #include "cytypes.h"
    #include "CyLib.h"
#else
    #include "syslib/cy_syslib.h"
#endif

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 statusState;

} STORE_ENC_READY_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

uint8 STORE_ENC_READY_Read(void) ;
void STORE_ENC_READY_InterruptEnable(void) ;
void STORE_ENC_READY_InterruptDisable(void) ;
void STORE_ENC_READY_WriteMask(uint8 mask) ;
uint8 STORE_ENC_READY_ReadMask(void) ;


/***************************************
*           API Constants
***************************************/

#define STORE_ENC_READY_STATUS_INTR_ENBL    0x10u


/***************************************
*         Parameter Constants
***************************************/

/* Status Register Inputs */
#define STORE_ENC_READY_INPUTS              1


/***************************************
*             Registers
***************************************/

/* Status Register */
#define STORE_ENC_READY_Status             (* (reg8 *) STORE_ENC_READY_sts_sts_reg__STATUS_REG )
#define STORE_ENC_READY_Status_PTR         (  (reg8 *) STORE_ENC_READY_sts_sts_reg__STATUS_REG )
#define STORE_ENC_READY_Status_Mask        (* (reg8 *) STORE_ENC_READY_sts_sts_reg__MASK_REG )
#define STORE_ENC_READY_Status_Aux_Ctrl    (* (reg8 *) STORE_ENC_READY_sts_sts_reg__STATUS_AUX_CTL_REG )

#endif /* End CY_STATUS_REG_STORE_ENC_READY_H */


/* [] END OF FILE */
