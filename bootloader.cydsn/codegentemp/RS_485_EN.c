/*******************************************************************************
* File Name: RS_485_EN.c  
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
#include "RS_485_EN.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 RS_485_EN__PORT == 15 && ((RS_485_EN__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: RS_485_EN_Write
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
void RS_485_EN_Write(uint8 value) 
{
    uint8 staticBits = (RS_485_EN_DR & (uint8)(~RS_485_EN_MASK));
    RS_485_EN_DR = staticBits | ((uint8)(value << RS_485_EN_SHIFT) & RS_485_EN_MASK);
}


/*******************************************************************************
* Function Name: RS_485_EN_SetDriveMode
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
void RS_485_EN_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(RS_485_EN_0, mode);
}


/*******************************************************************************
* Function Name: RS_485_EN_Read
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
*  Macro RS_485_EN_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 RS_485_EN_Read(void) 
{
    return (RS_485_EN_PS & RS_485_EN_MASK) >> RS_485_EN_SHIFT;
}


/*******************************************************************************
* Function Name: RS_485_EN_ReadDataReg
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
uint8 RS_485_EN_ReadDataReg(void) 
{
    return (RS_485_EN_DR & RS_485_EN_MASK) >> RS_485_EN_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(RS_485_EN_INTSTAT) 

    /*******************************************************************************
    * Function Name: RS_485_EN_ClearInterrupt
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
    uint8 RS_485_EN_ClearInterrupt(void) 
    {
        return (RS_485_EN_INTSTAT & RS_485_EN_MASK) >> RS_485_EN_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
