/*******************************************************************************
* File Name: CS2.h  
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

#if !defined(CY_PINS_CS2_H) /* Pins CS2_H */
#define CY_PINS_CS2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "CS2_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 CS2__PORT == 15 && ((CS2__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    CS2_Write(uint8 value);
void    CS2_SetDriveMode(uint8 mode);
uint8   CS2_ReadDataReg(void);
uint8   CS2_Read(void);
void    CS2_SetInterruptMode(uint16 position, uint16 mode);
uint8   CS2_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the CS2_SetDriveMode() function.
     *  @{
     */
        #define CS2_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define CS2_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define CS2_DM_RES_UP          PIN_DM_RES_UP
        #define CS2_DM_RES_DWN         PIN_DM_RES_DWN
        #define CS2_DM_OD_LO           PIN_DM_OD_LO
        #define CS2_DM_OD_HI           PIN_DM_OD_HI
        #define CS2_DM_STRONG          PIN_DM_STRONG
        #define CS2_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define CS2_MASK               CS2__MASK
#define CS2_SHIFT              CS2__SHIFT
#define CS2_WIDTH              1u

/* Interrupt constants */
#if defined(CS2__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in CS2_SetInterruptMode() function.
     *  @{
     */
        #define CS2_INTR_NONE      (uint16)(0x0000u)
        #define CS2_INTR_RISING    (uint16)(0x0001u)
        #define CS2_INTR_FALLING   (uint16)(0x0002u)
        #define CS2_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define CS2_INTR_MASK      (0x01u) 
#endif /* (CS2__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define CS2_PS                     (* (reg8 *) CS2__PS)
/* Data Register */
#define CS2_DR                     (* (reg8 *) CS2__DR)
/* Port Number */
#define CS2_PRT_NUM                (* (reg8 *) CS2__PRT) 
/* Connect to Analog Globals */                                                  
#define CS2_AG                     (* (reg8 *) CS2__AG)                       
/* Analog MUX bux enable */
#define CS2_AMUX                   (* (reg8 *) CS2__AMUX) 
/* Bidirectional Enable */                                                        
#define CS2_BIE                    (* (reg8 *) CS2__BIE)
/* Bit-mask for Aliased Register Access */
#define CS2_BIT_MASK               (* (reg8 *) CS2__BIT_MASK)
/* Bypass Enable */
#define CS2_BYP                    (* (reg8 *) CS2__BYP)
/* Port wide control signals */                                                   
#define CS2_CTL                    (* (reg8 *) CS2__CTL)
/* Drive Modes */
#define CS2_DM0                    (* (reg8 *) CS2__DM0) 
#define CS2_DM1                    (* (reg8 *) CS2__DM1)
#define CS2_DM2                    (* (reg8 *) CS2__DM2) 
/* Input Buffer Disable Override */
#define CS2_INP_DIS                (* (reg8 *) CS2__INP_DIS)
/* LCD Common or Segment Drive */
#define CS2_LCD_COM_SEG            (* (reg8 *) CS2__LCD_COM_SEG)
/* Enable Segment LCD */
#define CS2_LCD_EN                 (* (reg8 *) CS2__LCD_EN)
/* Slew Rate Control */
#define CS2_SLW                    (* (reg8 *) CS2__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define CS2_PRTDSI__CAPS_SEL       (* (reg8 *) CS2__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define CS2_PRTDSI__DBL_SYNC_IN    (* (reg8 *) CS2__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define CS2_PRTDSI__OE_SEL0        (* (reg8 *) CS2__PRTDSI__OE_SEL0) 
#define CS2_PRTDSI__OE_SEL1        (* (reg8 *) CS2__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define CS2_PRTDSI__OUT_SEL0       (* (reg8 *) CS2__PRTDSI__OUT_SEL0) 
#define CS2_PRTDSI__OUT_SEL1       (* (reg8 *) CS2__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define CS2_PRTDSI__SYNC_OUT       (* (reg8 *) CS2__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(CS2__SIO_CFG)
    #define CS2_SIO_HYST_EN        (* (reg8 *) CS2__SIO_HYST_EN)
    #define CS2_SIO_REG_HIFREQ     (* (reg8 *) CS2__SIO_REG_HIFREQ)
    #define CS2_SIO_CFG            (* (reg8 *) CS2__SIO_CFG)
    #define CS2_SIO_DIFF           (* (reg8 *) CS2__SIO_DIFF)
#endif /* (CS2__SIO_CFG) */

/* Interrupt Registers */
#if defined(CS2__INTSTAT)
    #define CS2_INTSTAT            (* (reg8 *) CS2__INTSTAT)
    #define CS2_SNAP               (* (reg8 *) CS2__SNAP)
    
	#define CS2_0_INTTYPE_REG 		(* (reg8 *) CS2__0__INTTYPE)
#endif /* (CS2__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_CS2_H */


/* [] END OF FILE */
