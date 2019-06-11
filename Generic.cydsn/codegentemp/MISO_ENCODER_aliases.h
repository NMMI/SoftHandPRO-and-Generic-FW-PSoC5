/*******************************************************************************
* File Name: MISO_ENCODER.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_MISO_ENCODER_ALIASES_H) /* Pins MISO_ENCODER_ALIASES_H */
#define CY_PINS_MISO_ENCODER_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define MISO_ENCODER_0			(MISO_ENCODER__0__PC)
#define MISO_ENCODER_0_INTR	((uint16)((uint16)0x0001u << MISO_ENCODER__0__SHIFT))

#define MISO_ENCODER_INTR_ALL	 ((uint16)(MISO_ENCODER_0_INTR))

#endif /* End Pins MISO_ENCODER_ALIASES_H */


/* [] END OF FILE */
