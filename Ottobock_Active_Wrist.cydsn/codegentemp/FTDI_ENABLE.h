/*******************************************************************************
* File Name: FTDI_ENABLE.h  
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

#if !defined(CY_PINS_FTDI_ENABLE_H) /* Pins FTDI_ENABLE_H */
#define CY_PINS_FTDI_ENABLE_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "FTDI_ENABLE_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 FTDI_ENABLE__PORT == 15 && ((FTDI_ENABLE__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    FTDI_ENABLE_Write(uint8 value);
void    FTDI_ENABLE_SetDriveMode(uint8 mode);
uint8   FTDI_ENABLE_ReadDataReg(void);
uint8   FTDI_ENABLE_Read(void);
void    FTDI_ENABLE_SetInterruptMode(uint16 position, uint16 mode);
uint8   FTDI_ENABLE_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the FTDI_ENABLE_SetDriveMode() function.
     *  @{
     */
        #define FTDI_ENABLE_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define FTDI_ENABLE_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define FTDI_ENABLE_DM_RES_UP          PIN_DM_RES_UP
        #define FTDI_ENABLE_DM_RES_DWN         PIN_DM_RES_DWN
        #define FTDI_ENABLE_DM_OD_LO           PIN_DM_OD_LO
        #define FTDI_ENABLE_DM_OD_HI           PIN_DM_OD_HI
        #define FTDI_ENABLE_DM_STRONG          PIN_DM_STRONG
        #define FTDI_ENABLE_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define FTDI_ENABLE_MASK               FTDI_ENABLE__MASK
#define FTDI_ENABLE_SHIFT              FTDI_ENABLE__SHIFT
#define FTDI_ENABLE_WIDTH              1u

/* Interrupt constants */
#if defined(FTDI_ENABLE__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in FTDI_ENABLE_SetInterruptMode() function.
     *  @{
     */
        #define FTDI_ENABLE_INTR_NONE      (uint16)(0x0000u)
        #define FTDI_ENABLE_INTR_RISING    (uint16)(0x0001u)
        #define FTDI_ENABLE_INTR_FALLING   (uint16)(0x0002u)
        #define FTDI_ENABLE_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define FTDI_ENABLE_INTR_MASK      (0x01u) 
#endif /* (FTDI_ENABLE__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define FTDI_ENABLE_PS                     (* (reg8 *) FTDI_ENABLE__PS)
/* Data Register */
#define FTDI_ENABLE_DR                     (* (reg8 *) FTDI_ENABLE__DR)
/* Port Number */
#define FTDI_ENABLE_PRT_NUM                (* (reg8 *) FTDI_ENABLE__PRT) 
/* Connect to Analog Globals */                                                  
#define FTDI_ENABLE_AG                     (* (reg8 *) FTDI_ENABLE__AG)                       
/* Analog MUX bux enable */
#define FTDI_ENABLE_AMUX                   (* (reg8 *) FTDI_ENABLE__AMUX) 
/* Bidirectional Enable */                                                        
#define FTDI_ENABLE_BIE                    (* (reg8 *) FTDI_ENABLE__BIE)
/* Bit-mask for Aliased Register Access */
#define FTDI_ENABLE_BIT_MASK               (* (reg8 *) FTDI_ENABLE__BIT_MASK)
/* Bypass Enable */
#define FTDI_ENABLE_BYP                    (* (reg8 *) FTDI_ENABLE__BYP)
/* Port wide control signals */                                                   
#define FTDI_ENABLE_CTL                    (* (reg8 *) FTDI_ENABLE__CTL)
/* Drive Modes */
#define FTDI_ENABLE_DM0                    (* (reg8 *) FTDI_ENABLE__DM0) 
#define FTDI_ENABLE_DM1                    (* (reg8 *) FTDI_ENABLE__DM1)
#define FTDI_ENABLE_DM2                    (* (reg8 *) FTDI_ENABLE__DM2) 
/* Input Buffer Disable Override */
#define FTDI_ENABLE_INP_DIS                (* (reg8 *) FTDI_ENABLE__INP_DIS)
/* LCD Common or Segment Drive */
#define FTDI_ENABLE_LCD_COM_SEG            (* (reg8 *) FTDI_ENABLE__LCD_COM_SEG)
/* Enable Segment LCD */
#define FTDI_ENABLE_LCD_EN                 (* (reg8 *) FTDI_ENABLE__LCD_EN)
/* Slew Rate Control */
#define FTDI_ENABLE_SLW                    (* (reg8 *) FTDI_ENABLE__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define FTDI_ENABLE_PRTDSI__CAPS_SEL       (* (reg8 *) FTDI_ENABLE__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define FTDI_ENABLE_PRTDSI__DBL_SYNC_IN    (* (reg8 *) FTDI_ENABLE__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define FTDI_ENABLE_PRTDSI__OE_SEL0        (* (reg8 *) FTDI_ENABLE__PRTDSI__OE_SEL0) 
#define FTDI_ENABLE_PRTDSI__OE_SEL1        (* (reg8 *) FTDI_ENABLE__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define FTDI_ENABLE_PRTDSI__OUT_SEL0       (* (reg8 *) FTDI_ENABLE__PRTDSI__OUT_SEL0) 
#define FTDI_ENABLE_PRTDSI__OUT_SEL1       (* (reg8 *) FTDI_ENABLE__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define FTDI_ENABLE_PRTDSI__SYNC_OUT       (* (reg8 *) FTDI_ENABLE__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(FTDI_ENABLE__SIO_CFG)
    #define FTDI_ENABLE_SIO_HYST_EN        (* (reg8 *) FTDI_ENABLE__SIO_HYST_EN)
    #define FTDI_ENABLE_SIO_REG_HIFREQ     (* (reg8 *) FTDI_ENABLE__SIO_REG_HIFREQ)
    #define FTDI_ENABLE_SIO_CFG            (* (reg8 *) FTDI_ENABLE__SIO_CFG)
    #define FTDI_ENABLE_SIO_DIFF           (* (reg8 *) FTDI_ENABLE__SIO_DIFF)
#endif /* (FTDI_ENABLE__SIO_CFG) */

/* Interrupt Registers */
#if defined(FTDI_ENABLE__INTSTAT)
    #define FTDI_ENABLE_INTSTAT            (* (reg8 *) FTDI_ENABLE__INTSTAT)
    #define FTDI_ENABLE_SNAP               (* (reg8 *) FTDI_ENABLE__SNAP)
    
	#define FTDI_ENABLE_0_INTTYPE_REG 		(* (reg8 *) FTDI_ENABLE__0__INTTYPE)
#endif /* (FTDI_ENABLE__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_FTDI_ENABLE_H */


/* [] END OF FILE */
