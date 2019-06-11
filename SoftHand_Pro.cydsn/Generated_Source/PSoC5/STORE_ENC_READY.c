/*******************************************************************************
* File Name: STORE_ENC_READY.c  
* Version 1.90
*
* Description:
*  This file contains API to enable firmware to read the value of a Status 
*  Register.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "STORE_ENC_READY.h"

#if !defined(STORE_ENC_READY_sts_sts_reg__REMOVED) /* Check for removal by optimization */


/*******************************************************************************
* Function Name: STORE_ENC_READY_Read
********************************************************************************
*
* Summary:
*  Reads the current value assigned to the Status Register.
*
* Parameters:
*  None.
*
* Return:
*  The current value in the Status Register.
*
*******************************************************************************/
uint8 STORE_ENC_READY_Read(void) 
{ 
    return STORE_ENC_READY_Status;
}


/*******************************************************************************
* Function Name: STORE_ENC_READY_InterruptEnable
********************************************************************************
*
* Summary:
*  Enables the Status Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void STORE_ENC_READY_InterruptEnable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    STORE_ENC_READY_Status_Aux_Ctrl |= STORE_ENC_READY_STATUS_INTR_ENBL;
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: STORE_ENC_READY_InterruptDisable
********************************************************************************
*
* Summary:
*  Disables the Status Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void STORE_ENC_READY_InterruptDisable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    STORE_ENC_READY_Status_Aux_Ctrl &= (uint8)(~STORE_ENC_READY_STATUS_INTR_ENBL);
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: STORE_ENC_READY_WriteMask
********************************************************************************
*
* Summary:
*  Writes the current mask value assigned to the Status Register.
*
* Parameters:
*  mask:  Value to write into the mask register.
*
* Return:
*  None.
*
*******************************************************************************/
void STORE_ENC_READY_WriteMask(uint8 mask) 
{
    #if(STORE_ENC_READY_INPUTS < 8u)
    	mask &= ((uint8)(1u << STORE_ENC_READY_INPUTS) - 1u);
	#endif /* End STORE_ENC_READY_INPUTS < 8u */
    STORE_ENC_READY_Status_Mask = mask;
}


/*******************************************************************************
* Function Name: STORE_ENC_READY_ReadMask
********************************************************************************
*
* Summary:
*  Reads the current interrupt mask assigned to the Status Register.
*
* Parameters:
*  None.
*
* Return:
*  The value of the interrupt mask of the Status Register.
*
*******************************************************************************/
uint8 STORE_ENC_READY_ReadMask(void) 
{
    return STORE_ENC_READY_Status_Mask;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
