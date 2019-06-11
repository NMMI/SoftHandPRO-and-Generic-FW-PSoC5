/*******************************************************************************
* File Name: ENC_DMA_STATUS.c  
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

#include "ENC_DMA_STATUS.h"

#if !defined(ENC_DMA_STATUS_sts_sts_reg__REMOVED) /* Check for removal by optimization */


/*******************************************************************************
* Function Name: ENC_DMA_STATUS_Read
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
uint8 ENC_DMA_STATUS_Read(void) 
{ 
    return ENC_DMA_STATUS_Status;
}


/*******************************************************************************
* Function Name: ENC_DMA_STATUS_InterruptEnable
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
void ENC_DMA_STATUS_InterruptEnable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    ENC_DMA_STATUS_Status_Aux_Ctrl |= ENC_DMA_STATUS_STATUS_INTR_ENBL;
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: ENC_DMA_STATUS_InterruptDisable
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
void ENC_DMA_STATUS_InterruptDisable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    ENC_DMA_STATUS_Status_Aux_Ctrl &= (uint8)(~ENC_DMA_STATUS_STATUS_INTR_ENBL);
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: ENC_DMA_STATUS_WriteMask
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
void ENC_DMA_STATUS_WriteMask(uint8 mask) 
{
    #if(ENC_DMA_STATUS_INPUTS < 8u)
    	mask &= ((uint8)(1u << ENC_DMA_STATUS_INPUTS) - 1u);
	#endif /* End ENC_DMA_STATUS_INPUTS < 8u */
    ENC_DMA_STATUS_Status_Mask = mask;
}


/*******************************************************************************
* Function Name: ENC_DMA_STATUS_ReadMask
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
uint8 ENC_DMA_STATUS_ReadMask(void) 
{
    return ENC_DMA_STATUS_Status_Mask;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
