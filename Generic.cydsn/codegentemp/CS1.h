/*******************************************************************************
* File Name: CS1.h  
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

#if !defined(CY_PINS_CS1_H) /* Pins CS1_H */
#define CY_PINS_CS1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "CS1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 CS1__PORT == 15 && ((CS1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    CS1_Write(uint8 value);
void    CS1_SetDriveMode(uint8 mode);
uint8   CS1_ReadDataReg(void);
uint8   CS1_Read(void);
void    CS1_SetInterruptMode(uint16 position, uint16 mode);
uint8   CS1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the CS1_SetDriveMode() function.
     *  @{
     */
        #define CS1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define CS1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define CS1_DM_RES_UP          PIN_DM_RES_UP
        #define CS1_DM_RES_DWN         PIN_DM_RES_DWN
        #define CS1_DM_OD_LO           PIN_DM_OD_LO
        #define CS1_DM_OD_HI           PIN_DM_OD_HI
        #define CS1_DM_STRONG          PIN_DM_STRONG
        #define CS1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define CS1_MASK               CS1__MASK
#define CS1_SHIFT              CS1__SHIFT
#define CS1_WIDTH              1u

/* Interrupt constants */
#if defined(CS1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in CS1_SetInterruptMode() function.
     *  @{
     */
        #define CS1_INTR_NONE      (uint16)(0x0000u)
        #define CS1_INTR_RISING    (uint16)(0x0001u)
        #define CS1_INTR_FALLING   (uint16)(0x0002u)
        #define CS1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define CS1_INTR_MASK      (0x01u) 
#endif /* (CS1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define CS1_PS                     (* (reg8 *) CS1__PS)
/* Data Register */
#define CS1_DR                     (* (reg8 *) CS1__DR)
/* Port Number */
#define CS1_PRT_NUM                (* (reg8 *) CS1__PRT) 
/* Connect to Analog Globals */                                                  
#define CS1_AG                     (* (reg8 *) CS1__AG)                       
/* Analog MUX bux enable */
#define CS1_AMUX                   (* (reg8 *) CS1__AMUX) 
/* Bidirectional Enable */                                                        
#define CS1_BIE                    (* (reg8 *) CS1__BIE)
/* Bit-mask for Aliased Register Access */
#define CS1_BIT_MASK               (* (reg8 *) CS1__BIT_MASK)
/* Bypass Enable */
#define CS1_BYP                    (* (reg8 *) CS1__BYP)
/* Port wide control signals */                                                   
#define CS1_CTL                    (* (reg8 *) CS1__CTL)
/* Drive Modes */
#define CS1_DM0                    (* (reg8 *) CS1__DM0) 
#define CS1_DM1                    (* (reg8 *) CS1__DM1)
#define CS1_DM2                    (* (reg8 *) CS1__DM2) 
/* Input Buffer Disable Override */
#define CS1_INP_DIS                (* (reg8 *) CS1__INP_DIS)
/* LCD Common or Segment Drive */
#define CS1_LCD_COM_SEG            (* (reg8 *) CS1__LCD_COM_SEG)
/* Enable Segment LCD */
#define CS1_LCD_EN                 (* (reg8 *) CS1__LCD_EN)
/* Slew Rate Control */
#define CS1_SLW                    (* (reg8 *) CS1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define CS1_PRTDSI__CAPS_SEL       (* (reg8 *) CS1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define CS1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) CS1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define CS1_PRTDSI__OE_SEL0        (* (reg8 *) CS1__PRTDSI__OE_SEL0) 
#define CS1_PRTDSI__OE_SEL1        (* (reg8 *) CS1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define CS1_PRTDSI__OUT_SEL0       (* (reg8 *) CS1__PRTDSI__OUT_SEL0) 
#define CS1_PRTDSI__OUT_SEL1       (* (reg8 *) CS1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define CS1_PRTDSI__SYNC_OUT       (* (reg8 *) CS1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(CS1__SIO_CFG)
    #define CS1_SIO_HYST_EN        (* (reg8 *) CS1__SIO_HYST_EN)
    #define CS1_SIO_REG_HIFREQ     (* (reg8 *) CS1__SIO_REG_HIFREQ)
    #define CS1_SIO_CFG            (* (reg8 *) CS1__SIO_CFG)
    #define CS1_SIO_DIFF           (* (reg8 *) CS1__SIO_DIFF)
#endif /* (CS1__SIO_CFG) */

/* Interrupt Registers */
#if defined(CS1__INTSTAT)
    #define CS1_INTSTAT            (* (reg8 *) CS1__INTSTAT)
    #define CS1_SNAP               (* (reg8 *) CS1__SNAP)
    
	#define CS1_0_INTTYPE_REG 		(* (reg8 *) CS1__0__INTTYPE)
#endif /* (CS1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_CS1_H */


/* [] END OF FILE */
