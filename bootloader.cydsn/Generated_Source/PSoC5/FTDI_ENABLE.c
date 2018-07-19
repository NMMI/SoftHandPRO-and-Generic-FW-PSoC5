/*******************************************************************************
* File Name: FTDI_ENABLE.c  
* Version 1.90
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "FTDI_ENABLE.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 FTDI_ENABLE__PORT == 15 && ((FTDI_ENABLE__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: FTDI_ENABLE_Write
********************************************************************************
*
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  None
*  
*******************************************************************************/
void FTDI_ENABLE_Write(uint8 value) 
{
    uint8 staticBits = (FTDI_ENABLE_DR & (uint8)(~FTDI_ENABLE_MASK));
    FTDI_ENABLE_DR = staticBits | ((uint8)(value << FTDI_ENABLE_SHIFT) & FTDI_ENABLE_MASK);
}


/*******************************************************************************
* Function Name: FTDI_ENABLE_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to this drive mode.
*
* Return: 
*  None
*
*******************************************************************************/
void FTDI_ENABLE_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(FTDI_ENABLE_0, mode);
}


/*******************************************************************************
* Function Name: FTDI_ENABLE_Read
********************************************************************************
*
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  None
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro FTDI_ENABLE_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 FTDI_ENABLE_Read(void) 
{
    return (FTDI_ENABLE_PS & FTDI_ENABLE_MASK) >> FTDI_ENABLE_SHIFT;
}


/*******************************************************************************
* Function Name: FTDI_ENABLE_ReadDataReg
********************************************************************************
*
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 FTDI_ENABLE_ReadDataReg(void) 
{
    return (FTDI_ENABLE_DR & FTDI_ENABLE_MASK) >> FTDI_ENABLE_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(FTDI_ENABLE_INTSTAT) 

    /*******************************************************************************
    * Function Name: FTDI_ENABLE_ClearInterrupt
    ********************************************************************************
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  None 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 FTDI_ENABLE_ClearInterrupt(void) 
    {
        return (FTDI_ENABLE_INTSTAT & FTDI_ENABLE_MASK) >> FTDI_ENABLE_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
