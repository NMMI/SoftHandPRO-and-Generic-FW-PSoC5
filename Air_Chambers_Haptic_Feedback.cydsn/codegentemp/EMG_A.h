/*******************************************************************************
* File Name: EMG_A.h  
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

#if !defined(CY_PINS_EMG_A_H) /* Pins EMG_A_H */
#define CY_PINS_EMG_A_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "EMG_A_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 EMG_A__PORT == 15 && ((EMG_A__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    EMG_A_Write(uint8 value);
void    EMG_A_SetDriveMode(uint8 mode);
uint8   EMG_A_ReadDataReg(void);
uint8   EMG_A_Read(void);
void    EMG_A_SetInterruptMode(uint16 position, uint16 mode);
uint8   EMG_A_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the EMG_A_SetDriveMode() function.
     *  @{
     */
        #define EMG_A_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define EMG_A_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define EMG_A_DM_RES_UP          PIN_DM_RES_UP
        #define EMG_A_DM_RES_DWN         PIN_DM_RES_DWN
        #define EMG_A_DM_OD_LO           PIN_DM_OD_LO
        #define EMG_A_DM_OD_HI           PIN_DM_OD_HI
        #define EMG_A_DM_STRONG          PIN_DM_STRONG
        #define EMG_A_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define EMG_A_MASK               EMG_A__MASK
#define EMG_A_SHIFT              EMG_A__SHIFT
#define EMG_A_WIDTH              1u

/* Interrupt constants */
#if defined(EMG_A__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in EMG_A_SetInterruptMode() function.
     *  @{
     */
        #define EMG_A_INTR_NONE      (uint16)(0x0000u)
        #define EMG_A_INTR_RISING    (uint16)(0x0001u)
        #define EMG_A_INTR_FALLING   (uint16)(0x0002u)
        #define EMG_A_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define EMG_A_INTR_MASK      (0x01u) 
#endif /* (EMG_A__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define EMG_A_PS                     (* (reg8 *) EMG_A__PS)
/* Data Register */
#define EMG_A_DR                     (* (reg8 *) EMG_A__DR)
/* Port Number */
#define EMG_A_PRT_NUM                (* (reg8 *) EMG_A__PRT) 
/* Connect to Analog Globals */                                                  
#define EMG_A_AG                     (* (reg8 *) EMG_A__AG)                       
/* Analog MUX bux enable */
#define EMG_A_AMUX                   (* (reg8 *) EMG_A__AMUX) 
/* Bidirectional Enable */                                                        
#define EMG_A_BIE                    (* (reg8 *) EMG_A__BIE)
/* Bit-mask for Aliased Register Access */
#define EMG_A_BIT_MASK               (* (reg8 *) EMG_A__BIT_MASK)
/* Bypass Enable */
#define EMG_A_BYP                    (* (reg8 *) EMG_A__BYP)
/* Port wide control signals */                                                   
#define EMG_A_CTL                    (* (reg8 *) EMG_A__CTL)
/* Drive Modes */
#define EMG_A_DM0                    (* (reg8 *) EMG_A__DM0) 
#define EMG_A_DM1                    (* (reg8 *) EMG_A__DM1)
#define EMG_A_DM2                    (* (reg8 *) EMG_A__DM2) 
/* Input Buffer Disable Override */
#define EMG_A_INP_DIS                (* (reg8 *) EMG_A__INP_DIS)
/* LCD Common or Segment Drive */
#define EMG_A_LCD_COM_SEG            (* (reg8 *) EMG_A__LCD_COM_SEG)
/* Enable Segment LCD */
#define EMG_A_LCD_EN                 (* (reg8 *) EMG_A__LCD_EN)
/* Slew Rate Control */
#define EMG_A_SLW                    (* (reg8 *) EMG_A__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define EMG_A_PRTDSI__CAPS_SEL       (* (reg8 *) EMG_A__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define EMG_A_PRTDSI__DBL_SYNC_IN    (* (reg8 *) EMG_A__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define EMG_A_PRTDSI__OE_SEL0        (* (reg8 *) EMG_A__PRTDSI__OE_SEL0) 
#define EMG_A_PRTDSI__OE_SEL1        (* (reg8 *) EMG_A__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define EMG_A_PRTDSI__OUT_SEL0       (* (reg8 *) EMG_A__PRTDSI__OUT_SEL0) 
#define EMG_A_PRTDSI__OUT_SEL1       (* (reg8 *) EMG_A__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define EMG_A_PRTDSI__SYNC_OUT       (* (reg8 *) EMG_A__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(EMG_A__SIO_CFG)
    #define EMG_A_SIO_HYST_EN        (* (reg8 *) EMG_A__SIO_HYST_EN)
    #define EMG_A_SIO_REG_HIFREQ     (* (reg8 *) EMG_A__SIO_REG_HIFREQ)
    #define EMG_A_SIO_CFG            (* (reg8 *) EMG_A__SIO_CFG)
    #define EMG_A_SIO_DIFF           (* (reg8 *) EMG_A__SIO_DIFF)
#endif /* (EMG_A__SIO_CFG) */

/* Interrupt Registers */
#if defined(EMG_A__INTSTAT)
    #define EMG_A_INTSTAT            (* (reg8 *) EMG_A__INTSTAT)
    #define EMG_A_SNAP               (* (reg8 *) EMG_A__SNAP)
    
	#define EMG_A_0_INTTYPE_REG 		(* (reg8 *) EMG_A__0__INTTYPE)
#endif /* (EMG_A__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_EMG_A_H */


/* [] END OF FILE */
