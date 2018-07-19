/*******************************************************************************
* File Name: Signal_1_C.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Signal_1_C_H) /* Pins Signal_1_C_H */
#define CY_PINS_Signal_1_C_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Signal_1_C_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Signal_1_C__PORT == 15 && ((Signal_1_C__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Signal_1_C_Write(uint8 value);
void    Signal_1_C_SetDriveMode(uint8 mode);
uint8   Signal_1_C_ReadDataReg(void);
uint8   Signal_1_C_Read(void);
void    Signal_1_C_SetInterruptMode(uint16 position, uint16 mode);
uint8   Signal_1_C_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Signal_1_C_SetDriveMode() function.
     *  @{
     */
        #define Signal_1_C_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Signal_1_C_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Signal_1_C_DM_RES_UP          PIN_DM_RES_UP
        #define Signal_1_C_DM_RES_DWN         PIN_DM_RES_DWN
        #define Signal_1_C_DM_OD_LO           PIN_DM_OD_LO
        #define Signal_1_C_DM_OD_HI           PIN_DM_OD_HI
        #define Signal_1_C_DM_STRONG          PIN_DM_STRONG
        #define Signal_1_C_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Signal_1_C_MASK               Signal_1_C__MASK
#define Signal_1_C_SHIFT              Signal_1_C__SHIFT
#define Signal_1_C_WIDTH              1u

/* Interrupt constants */
#if defined(Signal_1_C__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Signal_1_C_SetInterruptMode() function.
     *  @{
     */
        #define Signal_1_C_INTR_NONE      (uint16)(0x0000u)
        #define Signal_1_C_INTR_RISING    (uint16)(0x0001u)
        #define Signal_1_C_INTR_FALLING   (uint16)(0x0002u)
        #define Signal_1_C_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Signal_1_C_INTR_MASK      (0x01u) 
#endif /* (Signal_1_C__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Signal_1_C_PS                     (* (reg8 *) Signal_1_C__PS)
/* Data Register */
#define Signal_1_C_DR                     (* (reg8 *) Signal_1_C__DR)
/* Port Number */
#define Signal_1_C_PRT_NUM                (* (reg8 *) Signal_1_C__PRT) 
/* Connect to Analog Globals */                                                  
#define Signal_1_C_AG                     (* (reg8 *) Signal_1_C__AG)                       
/* Analog MUX bux enable */
#define Signal_1_C_AMUX                   (* (reg8 *) Signal_1_C__AMUX) 
/* Bidirectional Enable */                                                        
#define Signal_1_C_BIE                    (* (reg8 *) Signal_1_C__BIE)
/* Bit-mask for Aliased Register Access */
#define Signal_1_C_BIT_MASK               (* (reg8 *) Signal_1_C__BIT_MASK)
/* Bypass Enable */
#define Signal_1_C_BYP                    (* (reg8 *) Signal_1_C__BYP)
/* Port wide control signals */                                                   
#define Signal_1_C_CTL                    (* (reg8 *) Signal_1_C__CTL)
/* Drive Modes */
#define Signal_1_C_DM0                    (* (reg8 *) Signal_1_C__DM0) 
#define Signal_1_C_DM1                    (* (reg8 *) Signal_1_C__DM1)
#define Signal_1_C_DM2                    (* (reg8 *) Signal_1_C__DM2) 
/* Input Buffer Disable Override */
#define Signal_1_C_INP_DIS                (* (reg8 *) Signal_1_C__INP_DIS)
/* LCD Common or Segment Drive */
#define Signal_1_C_LCD_COM_SEG            (* (reg8 *) Signal_1_C__LCD_COM_SEG)
/* Enable Segment LCD */
#define Signal_1_C_LCD_EN                 (* (reg8 *) Signal_1_C__LCD_EN)
/* Slew Rate Control */
#define Signal_1_C_SLW                    (* (reg8 *) Signal_1_C__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Signal_1_C_PRTDSI__CAPS_SEL       (* (reg8 *) Signal_1_C__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Signal_1_C_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Signal_1_C__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Signal_1_C_PRTDSI__OE_SEL0        (* (reg8 *) Signal_1_C__PRTDSI__OE_SEL0) 
#define Signal_1_C_PRTDSI__OE_SEL1        (* (reg8 *) Signal_1_C__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Signal_1_C_PRTDSI__OUT_SEL0       (* (reg8 *) Signal_1_C__PRTDSI__OUT_SEL0) 
#define Signal_1_C_PRTDSI__OUT_SEL1       (* (reg8 *) Signal_1_C__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Signal_1_C_PRTDSI__SYNC_OUT       (* (reg8 *) Signal_1_C__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Signal_1_C__SIO_CFG)
    #define Signal_1_C_SIO_HYST_EN        (* (reg8 *) Signal_1_C__SIO_HYST_EN)
    #define Signal_1_C_SIO_REG_HIFREQ     (* (reg8 *) Signal_1_C__SIO_REG_HIFREQ)
    #define Signal_1_C_SIO_CFG            (* (reg8 *) Signal_1_C__SIO_CFG)
    #define Signal_1_C_SIO_DIFF           (* (reg8 *) Signal_1_C__SIO_DIFF)
#endif /* (Signal_1_C__SIO_CFG) */

/* Interrupt Registers */
#if defined(Signal_1_C__INTSTAT)
    #define Signal_1_C_INTSTAT            (* (reg8 *) Signal_1_C__INTSTAT)
    #define Signal_1_C_SNAP               (* (reg8 *) Signal_1_C__SNAP)
    
	#define Signal_1_C_0_INTTYPE_REG 		(* (reg8 *) Signal_1_C__0__INTTYPE)
#endif /* (Signal_1_C__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Signal_1_C_H */


/* [] END OF FILE */
