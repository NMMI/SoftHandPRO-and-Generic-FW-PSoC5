/*******************************************************************************
* File Name: ISR_RS485_RX.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_ISR_RS485_RX_H)
#define CY_ISR_ISR_RS485_RX_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void ISR_RS485_RX_Start(void);
void ISR_RS485_RX_StartEx(cyisraddress address);
void ISR_RS485_RX_Stop(void);

CY_ISR_PROTO(ISR_RS485_RX_Interrupt);

void ISR_RS485_RX_SetVector(cyisraddress address);
cyisraddress ISR_RS485_RX_GetVector(void);

void ISR_RS485_RX_SetPriority(uint8 priority);
uint8 ISR_RS485_RX_GetPriority(void);

void ISR_RS485_RX_Enable(void);
uint8 ISR_RS485_RX_GetState(void);
void ISR_RS485_RX_Disable(void);

void ISR_RS485_RX_SetPending(void);
void ISR_RS485_RX_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the ISR_RS485_RX ISR. */
#define ISR_RS485_RX_INTC_VECTOR            ((reg32 *) ISR_RS485_RX__INTC_VECT)

/* Address of the ISR_RS485_RX ISR priority. */
#define ISR_RS485_RX_INTC_PRIOR             ((reg8 *) ISR_RS485_RX__INTC_PRIOR_REG)

/* Priority of the ISR_RS485_RX interrupt. */
#define ISR_RS485_RX_INTC_PRIOR_NUMBER      ISR_RS485_RX__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable ISR_RS485_RX interrupt. */
#define ISR_RS485_RX_INTC_SET_EN            ((reg32 *) ISR_RS485_RX__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the ISR_RS485_RX interrupt. */
#define ISR_RS485_RX_INTC_CLR_EN            ((reg32 *) ISR_RS485_RX__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the ISR_RS485_RX interrupt state to pending. */
#define ISR_RS485_RX_INTC_SET_PD            ((reg32 *) ISR_RS485_RX__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the ISR_RS485_RX interrupt. */
#define ISR_RS485_RX_INTC_CLR_PD            ((reg32 *) ISR_RS485_RX__INTC_CLR_PD_REG)


#endif /* CY_ISR_ISR_RS485_RX_H */


/* [] END OF FILE */
