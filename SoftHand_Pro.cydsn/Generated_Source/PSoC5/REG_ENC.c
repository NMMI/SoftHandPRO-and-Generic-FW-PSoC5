/*******************************************************************************
* File Name: REG_ENC.c  
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

#include "REG_ENC.h"

#if !defined(REG_ENC_sts_sts_reg__REMOVED) /* Check for removal by optimization */


/*******************************************************************************
* Function Name: REG_ENC_Read
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
uint8 REG_ENC_Read(void) 
{ 
    return REG_ENC_Status;
}


/*******************************************************************************
* Function Name: REG_ENC_InterruptEnable
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
void REG_ENC_InterruptEnable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    REG_ENC_Status_Aux_Ctrl |= REG_ENC_STATUS_INTR_ENBL;
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: REG_ENC_InterruptDisable
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
void REG_ENC_InterruptDisable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    REG_ENC_Status_Aux_Ctrl &= (uint8)(~REG_ENC_STATUS_INTR_ENBL);
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: REG_ENC_WriteMask
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
void REG_ENC_WriteMask(uint8 mask) 
{
    #if(REG_ENC_INPUTS < 8u)
    	mask &= ((uint8)(1u << REG_ENC_INPUTS) - 1u);
	#endif /* End REG_ENC_INPUTS < 8u */
    REG_ENC_Status_Mask = mask;
}


/*******************************************************************************
* Function Name: REG_ENC_ReadMask
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
uint8 REG_ENC_ReadMask(void) 
{
    return REG_ENC_Status_Mask;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
