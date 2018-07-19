/*******************************************************************************
* File Name: CS_SUPPORT_PIN.h  
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

#if !defined(CY_PINS_CS_SUPPORT_PIN_H) /* Pins CS_SUPPORT_PIN_H */
#define CY_PINS_CS_SUPPORT_PIN_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "CS_SUPPORT_PIN_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 CS_SUPPORT_PIN__PORT == 15 && ((CS_SUPPORT_PIN__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    CS_SUPPORT_PIN_Write(uint8 value);
void    CS_SUPPORT_PIN_SetDriveMode(uint8 mode);
uint8   CS_SUPPORT_PIN_ReadDataReg(void);
uint8   CS_SUPPORT_PIN_Read(void);
void    CS_SUPPORT_PIN_SetInterruptMode(uint16 position, uint16 mode);
uint8   CS_SUPPORT_PIN_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the CS_SUPPORT_PIN_SetDriveMode() function.
     *  @{
     */
        #define CS_SUPPORT_PIN_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define CS_SUPPORT_PIN_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define CS_SUPPORT_PIN_DM_RES_UP          PIN_DM_RES_UP
        #define CS_SUPPORT_PIN_DM_RES_DWN         PIN_DM_RES_DWN
        #define CS_SUPPORT_PIN_DM_OD_LO           PIN_DM_OD_LO
        #define CS_SUPPORT_PIN_DM_OD_HI           PIN_DM_OD_HI
        #define CS_SUPPORT_PIN_DM_STRONG          PIN_DM_STRONG
        #define CS_SUPPORT_PIN_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define CS_SUPPORT_PIN_MASK               CS_SUPPORT_PIN__MASK
#define CS_SUPPORT_PIN_SHIFT              CS_SUPPORT_PIN__SHIFT
#define CS_SUPPORT_PIN_WIDTH              1u

/* Interrupt constants */
#if defined(CS_SUPPORT_PIN__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in CS_SUPPORT_PIN_SetInterruptMode() function.
     *  @{
     */
        #define CS_SUPPORT_PIN_INTR_NONE      (uint16)(0x0000u)
        #define CS_SUPPORT_PIN_INTR_RISING    (uint16)(0x0001u)
        #define CS_SUPPORT_PIN_INTR_FALLING   (uint16)(0x0002u)
        #define CS_SUPPORT_PIN_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define CS_SUPPORT_PIN_INTR_MASK      (0x01u) 
#endif /* (CS_SUPPORT_PIN__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define CS_SUPPORT_PIN_PS                     (* (reg8 *) CS_SUPPORT_PIN__PS)
/* Data Register */
#define CS_SUPPORT_PIN_DR                     (* (reg8 *) CS_SUPPORT_PIN__DR)
/* Port Number */
#define CS_SUPPORT_PIN_PRT_NUM                (* (reg8 *) CS_SUPPORT_PIN__PRT) 
/* Connect to Analog Globals */                                                  
#define CS_SUPPORT_PIN_AG                     (* (reg8 *) CS_SUPPORT_PIN__AG)                       
/* Analog MUX bux enable */
#define CS_SUPPORT_PIN_AMUX                   (* (reg8 *) CS_SUPPORT_PIN__AMUX) 
/* Bidirectional Enable */                                                        
#define CS_SUPPORT_PIN_BIE                    (* (reg8 *) CS_SUPPORT_PIN__BIE)
/* Bit-mask for Aliased Register Access */
#define CS_SUPPORT_PIN_BIT_MASK               (* (reg8 *) CS_SUPPORT_PIN__BIT_MASK)
/* Bypass Enable */
#define CS_SUPPORT_PIN_BYP                    (* (reg8 *) CS_SUPPORT_PIN__BYP)
/* Port wide control signals */                                                   
#define CS_SUPPORT_PIN_CTL                    (* (reg8 *) CS_SUPPORT_PIN__CTL)
/* Drive Modes */
#define CS_SUPPORT_PIN_DM0                    (* (reg8 *) CS_SUPPORT_PIN__DM0) 
#define CS_SUPPORT_PIN_DM1                    (* (reg8 *) CS_SUPPORT_PIN__DM1)
#define CS_SUPPORT_PIN_DM2                    (* (reg8 *) CS_SUPPORT_PIN__DM2) 
/* Input Buffer Disable Override */
#define CS_SUPPORT_PIN_INP_DIS                (* (reg8 *) CS_SUPPORT_PIN__INP_DIS)
/* LCD Common or Segment Drive */
#define CS_SUPPORT_PIN_LCD_COM_SEG            (* (reg8 *) CS_SUPPORT_PIN__LCD_COM_SEG)
/* Enable Segment LCD */
#define CS_SUPPORT_PIN_LCD_EN                 (* (reg8 *) CS_SUPPORT_PIN__LCD_EN)
/* Slew Rate Control */
#define CS_SUPPORT_PIN_SLW                    (* (reg8 *) CS_SUPPORT_PIN__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define CS_SUPPORT_PIN_PRTDSI__CAPS_SEL       (* (reg8 *) CS_SUPPORT_PIN__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define CS_SUPPORT_PIN_PRTDSI__DBL_SYNC_IN    (* (reg8 *) CS_SUPPORT_PIN__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define CS_SUPPORT_PIN_PRTDSI__OE_SEL0        (* (reg8 *) CS_SUPPORT_PIN__PRTDSI__OE_SEL0) 
#define CS_SUPPORT_PIN_PRTDSI__OE_SEL1        (* (reg8 *) CS_SUPPORT_PIN__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define CS_SUPPORT_PIN_PRTDSI__OUT_SEL0       (* (reg8 *) CS_SUPPORT_PIN__PRTDSI__OUT_SEL0) 
#define CS_SUPPORT_PIN_PRTDSI__OUT_SEL1       (* (reg8 *) CS_SUPPORT_PIN__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define CS_SUPPORT_PIN_PRTDSI__SYNC_OUT       (* (reg8 *) CS_SUPPORT_PIN__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(CS_SUPPORT_PIN__SIO_CFG)
    #define CS_SUPPORT_PIN_SIO_HYST_EN        (* (reg8 *) CS_SUPPORT_PIN__SIO_HYST_EN)
    #define CS_SUPPORT_PIN_SIO_REG_HIFREQ     (* (reg8 *) CS_SUPPORT_PIN__SIO_REG_HIFREQ)
    #define CS_SUPPORT_PIN_SIO_CFG            (* (reg8 *) CS_SUPPORT_PIN__SIO_CFG)
    #define CS_SUPPORT_PIN_SIO_DIFF           (* (reg8 *) CS_SUPPORT_PIN__SIO_DIFF)
#endif /* (CS_SUPPORT_PIN__SIO_CFG) */

/* Interrupt Registers */
#if defined(CS_SUPPORT_PIN__INTSTAT)
    #define CS_SUPPORT_PIN_INTSTAT            (* (reg8 *) CS_SUPPORT_PIN__INTSTAT)
    #define CS_SUPPORT_PIN_SNAP               (* (reg8 *) CS_SUPPORT_PIN__SNAP)
    
	#define CS_SUPPORT_PIN_0_INTTYPE_REG 		(* (reg8 *) CS_SUPPORT_PIN__0__INTTYPE)
#endif /* (CS_SUPPORT_PIN__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_CS_SUPPORT_PIN_H */


/* [] END OF FILE */
