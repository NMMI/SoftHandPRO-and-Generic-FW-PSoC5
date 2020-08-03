/*******************************************************************************
* File Name: MISO_ENCODER.h  
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

#if !defined(CY_PINS_MISO_ENCODER_H) /* Pins MISO_ENCODER_H */
#define CY_PINS_MISO_ENCODER_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "MISO_ENCODER_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 MISO_ENCODER__PORT == 15 && ((MISO_ENCODER__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    MISO_ENCODER_Write(uint8 value);
void    MISO_ENCODER_SetDriveMode(uint8 mode);
uint8   MISO_ENCODER_ReadDataReg(void);
uint8   MISO_ENCODER_Read(void);
void    MISO_ENCODER_SetInterruptMode(uint16 position, uint16 mode);
uint8   MISO_ENCODER_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the MISO_ENCODER_SetDriveMode() function.
     *  @{
     */
        #define MISO_ENCODER_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define MISO_ENCODER_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define MISO_ENCODER_DM_RES_UP          PIN_DM_RES_UP
        #define MISO_ENCODER_DM_RES_DWN         PIN_DM_RES_DWN
        #define MISO_ENCODER_DM_OD_LO           PIN_DM_OD_LO
        #define MISO_ENCODER_DM_OD_HI           PIN_DM_OD_HI
        #define MISO_ENCODER_DM_STRONG          PIN_DM_STRONG
        #define MISO_ENCODER_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define MISO_ENCODER_MASK               MISO_ENCODER__MASK
#define MISO_ENCODER_SHIFT              MISO_ENCODER__SHIFT
#define MISO_ENCODER_WIDTH              1u

/* Interrupt constants */
#if defined(MISO_ENCODER__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in MISO_ENCODER_SetInterruptMode() function.
     *  @{
     */
        #define MISO_ENCODER_INTR_NONE      (uint16)(0x0000u)
        #define MISO_ENCODER_INTR_RISING    (uint16)(0x0001u)
        #define MISO_ENCODER_INTR_FALLING   (uint16)(0x0002u)
        #define MISO_ENCODER_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define MISO_ENCODER_INTR_MASK      (0x01u) 
#endif /* (MISO_ENCODER__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define MISO_ENCODER_PS                     (* (reg8 *) MISO_ENCODER__PS)
/* Data Register */
#define MISO_ENCODER_DR                     (* (reg8 *) MISO_ENCODER__DR)
/* Port Number */
#define MISO_ENCODER_PRT_NUM                (* (reg8 *) MISO_ENCODER__PRT) 
/* Connect to Analog Globals */                                                  
#define MISO_ENCODER_AG                     (* (reg8 *) MISO_ENCODER__AG)                       
/* Analog MUX bux enable */
#define MISO_ENCODER_AMUX                   (* (reg8 *) MISO_ENCODER__AMUX) 
/* Bidirectional Enable */                                                        
#define MISO_ENCODER_BIE                    (* (reg8 *) MISO_ENCODER__BIE)
/* Bit-mask for Aliased Register Access */
#define MISO_ENCODER_BIT_MASK               (* (reg8 *) MISO_ENCODER__BIT_MASK)
/* Bypass Enable */
#define MISO_ENCODER_BYP                    (* (reg8 *) MISO_ENCODER__BYP)
/* Port wide control signals */                                                   
#define MISO_ENCODER_CTL                    (* (reg8 *) MISO_ENCODER__CTL)
/* Drive Modes */
#define MISO_ENCODER_DM0                    (* (reg8 *) MISO_ENCODER__DM0) 
#define MISO_ENCODER_DM1                    (* (reg8 *) MISO_ENCODER__DM1)
#define MISO_ENCODER_DM2                    (* (reg8 *) MISO_ENCODER__DM2) 
/* Input Buffer Disable Override */
#define MISO_ENCODER_INP_DIS                (* (reg8 *) MISO_ENCODER__INP_DIS)
/* LCD Common or Segment Drive */
#define MISO_ENCODER_LCD_COM_SEG            (* (reg8 *) MISO_ENCODER__LCD_COM_SEG)
/* Enable Segment LCD */
#define MISO_ENCODER_LCD_EN                 (* (reg8 *) MISO_ENCODER__LCD_EN)
/* Slew Rate Control */
#define MISO_ENCODER_SLW                    (* (reg8 *) MISO_ENCODER__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define MISO_ENCODER_PRTDSI__CAPS_SEL       (* (reg8 *) MISO_ENCODER__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define MISO_ENCODER_PRTDSI__DBL_SYNC_IN    (* (reg8 *) MISO_ENCODER__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define MISO_ENCODER_PRTDSI__OE_SEL0        (* (reg8 *) MISO_ENCODER__PRTDSI__OE_SEL0) 
#define MISO_ENCODER_PRTDSI__OE_SEL1        (* (reg8 *) MISO_ENCODER__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define MISO_ENCODER_PRTDSI__OUT_SEL0       (* (reg8 *) MISO_ENCODER__PRTDSI__OUT_SEL0) 
#define MISO_ENCODER_PRTDSI__OUT_SEL1       (* (reg8 *) MISO_ENCODER__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define MISO_ENCODER_PRTDSI__SYNC_OUT       (* (reg8 *) MISO_ENCODER__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(MISO_ENCODER__SIO_CFG)
    #define MISO_ENCODER_SIO_HYST_EN        (* (reg8 *) MISO_ENCODER__SIO_HYST_EN)
    #define MISO_ENCODER_SIO_REG_HIFREQ     (* (reg8 *) MISO_ENCODER__SIO_REG_HIFREQ)
    #define MISO_ENCODER_SIO_CFG            (* (reg8 *) MISO_ENCODER__SIO_CFG)
    #define MISO_ENCODER_SIO_DIFF           (* (reg8 *) MISO_ENCODER__SIO_DIFF)
#endif /* (MISO_ENCODER__SIO_CFG) */

/* Interrupt Registers */
#if defined(MISO_ENCODER__INTSTAT)
    #define MISO_ENCODER_INTSTAT            (* (reg8 *) MISO_ENCODER__INTSTAT)
    #define MISO_ENCODER_SNAP               (* (reg8 *) MISO_ENCODER__SNAP)
    
	#define MISO_ENCODER_0_INTTYPE_REG 		(* (reg8 *) MISO_ENCODER__0__INTTYPE)
#endif /* (MISO_ENCODER__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_MISO_ENCODER_H */


/* [] END OF FILE */
