// ----------------------------------------------------------------------------
// BSD 3-Clause License

// Copyright (c) 2016, qbrobotics
// Copyright (c) 2017-2020, Centro "E.Piaggio"
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.

// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

/**
* \file         utils.c
*
* \brief        Definition of utility functions.
* \date         March 20th, 2020
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
*/

#include "utils.h"

//==============================================================================
//                                                            Current Estimation
//==============================================================================

int32 curr_estim ( uint8 idx, int32 pos, int32 vel, int32 ref ) {

        static int32 virtual_pos_friction[NUM_OF_MOTORS]; 	// Virtual position used to estimate friction.
        static int32 err_pos_dt[NUM_OF_MOTORS];			    // Temporal evolution of error position.
											// Needed to model the current transients due to reference steps.
        int32 curr_estimate;

        if (pos < ZERO_TOL) 
			virtual_pos_friction[idx] = pos;
        else {
			if ((pos - virtual_pos_friction[idx]) > ZMAX) 
				virtual_pos_friction[idx] =  pos - ZMAX;
        	else { 
				if ((pos - virtual_pos_friction[idx]) < -ZMAX) 
					virtual_pos_friction[idx] =  pos + ZMAX;
			}
		}        
       
        curr_estimate = pos * g_mem.motor[idx].curr_lookup[0] * (1 + pos * g_mem.motor[idx].curr_lookup[1]) + (pos - virtual_pos_friction[idx]) * (g_mem.motor[idx].curr_lookup[2] / ZMAX) + vel * g_mem.motor[idx].curr_lookup[3] * (1 + abs(vel) * g_mem.motor[idx].curr_lookup[4]) + (ref - err_pos_dt[idx]) * g_mem.motor[idx].curr_lookup[5];
        
        if (curr_estimate > c_mem.motor[idx].current_limit) 
            curr_estimate = c_mem.motor[idx].current_limit;
        else {
			if (curr_estimate < -c_mem.motor[idx].current_limit)
            	curr_estimate = -c_mem.motor[idx].current_limit;
		}

        err_pos_dt[idx] = (REFSPEED * ref + (1024 - REFSPEED) * err_pos_dt[idx]) / 1024;         
        
        return curr_estimate; 
    
}

//==============================================================================
//                                                                General filter
//==============================================================================
int32 filter(int32 new_value, struct st_filter *f) {

    int32 aux;

    aux = (f->old_value * (1024 - f->gain) + (new_value << 6) * (f->gain)) /1024;

    f->old_value = aux;

    return (aux / 64);
}

//==============================================================================
//                                                                        MODULE
//==============================================================================

uint32 my_mod(int32 val, int32 divisor) {

    if (val >= 0) {
        return (int32)(val % divisor);
    } else {
        return (int32)((divisor - (-val % divisor)) % divisor);
    }
}


//==============================================================================
//                                                                     CALIBRATE
//==============================================================================

void calibration(void) {
    static uint8 direction;                 //0 closing, 1 opening.
    static uint16 closure_counter = 0;          //Range [0 - 2^16].
    //static CYBIT count_one = 1;             //Used to count only one cycle, when inputs are generated
                                            //and pass the threshold, having the condition verified.

    // closing
    if (direction == 0) {
        g_refNew[0].pos += (calib.speed << g_mem.enc[g_mem.motor[0].encoder_line].res[0]);
        
        if ((g_refNew[0].pos) > g_mem.motor[0].pos_lim_sup) {
            direction = 1;
        }
    } else { //opening
        g_refNew[0].pos -= (calib.speed << g_mem.enc[g_mem.motor[0].encoder_line].res[0]);
        if (SIGN(g_refNew[0].pos) != 1) {
            direction = 0;
            closure_counter++;
//            count_one = 1;          // Enables back the cycles counter
            if (closure_counter == calib.repetitions) {
                closure_counter = 0;
                calib.enabled = FALSE;
            }
        }
    }
}


//==============================================================================
//                                                      DOUBLE ENCODER CALC TURN
//==============================================================================

// Use this matlab function to calculate I1 and I2
//
// function [inv_a, inv_b] = mod_mul_inv(a, b)
//     a = int32(a);
//     b = int32(b);
//     if b == 0
//         inv_a = 1;
//         inv_b = 0;
//         return
//     else
//         q = idivide(a,b,'floor');
//         r = mod(a,b);
//         [s, t] = mod_mul_inv(b, r);
//     end
//     inv_a = t;
//     inv_b = s - q * t;
// return

// Number of ticks per turn
#define M 65536          ///< Number of encoder ticks per turn.

int calc_turns_fcn_SH(const int32 pos1, const int32 pos2, const int N1, const int N2, const int I1) { 
    
    int32 x;
    int32 aux;
    
    switch (c_mem.dev.right_left){
        case RIGHT_HAND:
            x = (my_mod( - N2*(pos2) - N1*pos1, M*N2) + M/2) / M;
            
            aux = my_mod(x*I1, N2);       // Module between x*I1 and N2.
            
            // Wrap turns
            aux -= N2;      //N2 (teeth of the second encoder wheel)
            if (aux == -N2){
                aux = 0;
            }
            
            break;
        case LEFT_HAND:
            x = (my_mod( - N2*(-pos2) - N1*pos1, M*N2) + M/2) / M;
            
            aux = my_mod(x*I1, N2);       // Module between x*I1 and N2.
            
            break;
    }
  
    // SoftHand Pro firmware turns computation does not have to take into account turns shift like previous SoftHand 1.2 firmware
    return aux; 

}

int calc_turns_fcn(const int32 pos1, const int32 pos2, const int N1, const int N2, const int I1) { 

    if (c_mem.dev.dev_type == SOFTHAND_PRO) {
        return calc_turns_fcn_SH(pos1, pos2, N1, N2, I1);   
    }
    else {
        // This is the same computation and formulas used in previous firmwares
        int32 x = (my_mod( - N2*pos2 - N1*pos1, M*N2) + M/2) / M;

        int32 aux = my_mod(x*I1, N2);

        return (my_mod(aux + N2/2, N2) - N2/2);
    }
}

//==============================================================================
//                                                                 REST POSITION
//==============================================================================
void check_rest_position(void) {     // 100 Hz frequency.
    
    static uint32 count = 0;        // Range [0 - 2^31].
    static uint8 flag_count = 1;
    static uint8 first_time = 1;
    static float m = 0;
    static int32 abs_err_thr = 0;
    static int32 delta_inc = 0;
    int32 act_pos = 0;
    static int32 rest_error;
    float rest_vel_ticks_ms = ((float)g_mem.SH.rest_vel)/1000.0;    //[ticks/s] -> [ticks/ms]
    
    if (first_time){
        count = 0;
        first_time = 0;
    }
    
    act_pos = (int32)(g_meas[g_mem.motor[0].encoder_line].pos[0] >> g_mem.enc[g_mem.motor[0].encoder_line].res[0]);
    
    if ( ( ((c_mem.motor[0].input_mode == INPUT_MODE_EMG_PROPORTIONAL || c_mem.motor[0].input_mode == INPUT_MODE_EMG_INTEGRAL || c_mem.motor[0].input_mode == INPUT_MODE_EMG_FCFS ||
             c_mem.motor[0].input_mode == INPUT_MODE_EMG_FCFS_ADV || c_mem.motor[0].input_mode == INPUT_MODE_EMG_PROPORTIONAL_NC) && g_adc_meas.emg[0] < 200 && g_adc_meas.emg[1] < 200) ) && act_pos < 10000){
        if (flag_count == 1){
            count = count + 1;
        }
    }
    else {
        count = 0;
        rest_enabled = 0;
        flag_count = 1;
    }
  
    /********** Velocity closure procedure *************
    * m = error/time
    * m = (g_mem.rest_pos - init_pos)/time
    * time = g_mem.rest_pos/rest_vel_ticks_ms (rest_vel_ticks_ms is in [ticks/ms])
    ***************************************************/

    if (count == (uint32)(g_mem.SH.rest_delay/CALIBRATION_DIV)){ 
        // This routine is executed every 10 firmware cycles -> g_mem.rest_delay must be major than 10 ms
        rest_enabled = 1;
        rest_pos_curr_ref = g_meas[g_mem.motor[0].encoder_line].pos[0];
        
        // Ramp angular coefficient
        m = ((float)(g_mem.SH.rest_pos - g_meas[g_mem.motor[0].encoder_line].pos[0])/((float)g_mem.SH.rest_pos))*(rest_vel_ticks_ms);
        
        // Stop condition threshold is related to m coefficient by REST_RATIO value
        abs_err_thr = (int32)( (int32)(((float)REST_POS_ERR_THR_GAIN)*m*((float)CALIBRATION_DIV)) << g_mem.enc[g_mem.motor[0].encoder_line].res[0]);
        abs_err_thr = (abs_err_thr>0)?abs_err_thr:(0-abs_err_thr);
        
        rest_error = g_mem.SH.rest_pos - g_meas[g_mem.motor[0].encoder_line].pos[0];    
        
        delta_inc = (int32)( ((int32)(m*(float)CALIBRATION_DIV)) << g_mem.enc[g_mem.motor[0].encoder_line].res[0] );
        delta_inc = (delta_inc>0)?delta_inc:(0-delta_inc);
        
        count = 0;
        flag_count = 0;
    }
    
    if (rest_enabled){

        if (rest_error < abs_err_thr && rest_error > -abs_err_thr){
            // Stop condition
            rest_pos_curr_ref = g_mem.SH.rest_pos;
            
            if (c_mem.motor[0].input_mode == INPUT_MODE_EMG_PROPORTIONAL || c_mem.motor[0].input_mode == INPUT_MODE_EMG_INTEGRAL || c_mem.motor[0].input_mode == INPUT_MODE_EMG_FCFS ||
                c_mem.motor[0].input_mode == INPUT_MODE_EMG_FCFS_ADV || c_mem.motor[0].input_mode == INPUT_MODE_EMG_PROPORTIONAL_NC)   // EMG input mode
                forced_open = 1; 
            
            count = 0;
        }
        else {
            rest_error = g_mem.SH.rest_pos - g_meas[g_mem.motor[0].encoder_line].pos[0];        

            if (rest_error > 0){
                rest_pos_curr_ref = rest_pos_curr_ref + delta_inc;
            }
            if (rest_error < 0){
                rest_pos_curr_ref = rest_pos_curr_ref - delta_inc;
            }
        } 
    }
    
    // Position limit saturation
    if (c_mem.motor[0].pos_lim_flag) {
        if (rest_pos_curr_ref < c_mem.motor[0].pos_lim_inf) 
            rest_pos_curr_ref = c_mem.motor[0].pos_lim_inf;
        if (rest_pos_curr_ref > c_mem.motor[0].pos_lim_sup) 
            rest_pos_curr_ref = c_mem.motor[0].pos_lim_sup;
    }

}

//==============================================================================
//                                                                   LED CONTROL
//==============================================================================

void LED_control(uint8 mode) {
    
    switch (mode) {
        
        case 0:     //All LEDs off
            LED_CTRL_Write(0);
            BLINK_CTRL_EN_Write(0);
            break;
        case 1:     // Green fixed light
            LED_CTRL_Write(2);
            BLINK_CTRL_EN_Write(0);
            break;
            
        case 2:     // Yellow flashing light @ 0.5 Hz
            LED_CTRL_Write(0);
            BLINK_CTRL_EN_Write(1);        
            break;
            
        case 3:     // Red flashing light @ 2.5 Hz
            LED_CTRL_Write(0);
            BLINK_CTRL_EN_Write(2);        
            break;
            
        case 4:     // Yellow fixed light - maintenance
            LED_CTRL_Write(3);
            BLINK_CTRL_EN_Write(0);        
            break;
        
        case 5:     // Red fixed light - uUSB power
            LED_CTRL_Write(1);
            BLINK_CTRL_EN_Write(0);        
            break; 
            
        default:
            break;
    }   
}


//==============================================================================
//                                                            BATTERY MANAGEMENT
//==============================================================================
void battery_management() {
    static uint32 v_count_lb = 0;
    
    // LED handling procedure
    // The board LED blinks if attached battery is not fully charged

    if (battery_low_SoC) {
        
        //red light - blink @ 2.5 Hz
        LED_control(3);
                
        rest_enabled = 0;
        if (v_count_lb >= 11000){
            // Disable motor because we are sure of not fully charged battery and terminal device has opened
            g_refNew[0].onoff = 0x00;
            enable_motor(0, g_refNew[0].onoff);
            g_refNew[1].onoff = 0x00;
            enable_motor(1, g_refNew[1].onoff);     // Deactivate motor
            v_count_lb = 0;
        }
        else
            v_count_lb = v_count_lb + 1;
        
    }
    else {

        // The board LED is still or blinks depending on attached battery State of Charge
        if (tension_valid == TRUE && (( c_mem.motor[0].input_mode == INPUT_MODE_EXTERNAL || c_mem.motor[0].input_mode == INPUT_MODE_ENCODER3 || c_mem.motor[0].input_mode == INPUT_MODE_JOYSTICK )  ||
            ( (c_mem.motor[0].input_mode == INPUT_MODE_EMG_PROPORTIONAL || c_mem.motor[0].input_mode == INPUT_MODE_EMG_INTEGRAL || c_mem.motor[0].input_mode == INPUT_MODE_EMG_FCFS ||
               c_mem.motor[0].input_mode == INPUT_MODE_EMG_FCFS_ADV || c_mem.motor[0].input_mode == INPUT_MODE_EMG_PROPORTIONAL_NC) && 
                emg_1_status == NORMAL && emg_2_status == NORMAL)) ){ 
            dev_tension_f[0] = filter(dev_tension[0], &filt_v[0]);            
            if (c_mem.dev.use_2nd_motor_flag == TRUE) {
                dev_tension_f[1] = filter(dev_tension[1], &filt_v[1]);
            }
            
            // Note: Sometimes pow_tension is not well computed when using power supply
            // Parrot/Renata/(2 cells @ 2000 mAh Ossur): 0.9, 0.87
            // ARTS: 0.87, 0.8
            if (dev_tension_f[0] > 0.95 * pow_tension[0] && (c_mem.dev.use_2nd_motor_flag == FALSE || dev_tension_f[1] > 0.95 * pow_tension[1])){
                //fixed
                if (!maintenance_flag)
                    LED_control(0);     // NO LIGHT - all leds off
                else
                    LED_control(4);     // yellow light - fixed
            }
            
            else {
                if (dev_tension_f[0] > 0.9 * pow_tension[0] && (c_mem.dev.use_2nd_motor_flag == FALSE || dev_tension_f[1] > 0.9 * pow_tension[1])) {
                    //yellow light - blink @ 0.5 Hz
                    //LED_control(2);   
                    
                    if (!maintenance_flag)
                        LED_control(0);     // NO LIGHT - all leds off
                    else
                        LED_control(4);     // yellow light - fixed
                    
                    v_count_lb = 0;    // Reset counter whenever battery is not totally out of charge
                }
                else {
                    //red light - blink @ 2.5 Hz
                    //LED_control(3);
                
                    if (v_count_lb >= 10000){                    
                        
                        if (c_mem.dev.dev_type == SOFTHAND_2_MOTORS) {
                            // [MP] For the moment, it is active only on the SOFTHAND_2_MOTORS device
                            battery_low_SoC = TRUE;
                        }
                        
                        //red light - blink @ 2.5 Hz
                        LED_control(3);
                        
                        rest_enabled = 0;
                    }
                    else{
                        v_count_lb = v_count_lb + 1;
                    }                
                }
            }
            
        }
        else {
            if ((c_mem.motor[0].input_mode == INPUT_MODE_EXTERNAL || c_mem.motor[0].input_mode == INPUT_MODE_ENCODER3 || c_mem.motor[0].input_mode == INPUT_MODE_JOYSTICK) || 
                ((c_mem.motor[0].input_mode == INPUT_MODE_EMG_PROPORTIONAL || c_mem.motor[0].input_mode == INPUT_MODE_EMG_INTEGRAL || c_mem.motor[0].input_mode == INPUT_MODE_EMG_FCFS ||
                  c_mem.motor[0].input_mode == INPUT_MODE_EMG_FCFS_ADV || c_mem.motor[0].input_mode == INPUT_MODE_EMG_PROPORTIONAL_NC) && emg_1_status == NORMAL && emg_2_status == NORMAL)){
                LED_control(5);     // Default - red light
            }
        }
    }  
}

//==============================================================================
//                                                         ADC SET CHANNELS USED
//==============================================================================
void ADC_Set_N_Channels() {
    // Set right number of ADC channels to sample according to chosen flags
    
    NUM_OF_ANALOG_INPUTS = 4;       // Voltage_Sense_1
                                    // Current_Sense_1
                                    // EMG_A
                                    // EMG_B

#ifdef GENERIC_FW    
    if (c_mem.dev.use_2nd_motor_flag == TRUE){
        // add 2nd power
        NUM_OF_ANALOG_INPUTS = 6;   // Voltage_Sense_2
                                    // Current_Sense_2
    }
    
    if (c_mem.exp.read_ADC_sensors_port_flag == TRUE){
        // add all emg channels
        NUM_OF_ANALOG_INPUTS = 12;  // EMG_1
                                    // EMG_2
                                    // EMG_3
                                    // EMG_4
                                    // EMG_5
                                    // EMG_6
    }
#endif

    ADC_N_CHANNELS_USED_Write(NUM_OF_ANALOG_INPUTS - 1);
    
}

//==============================================================================
//                                                                  ENABLE MOTOR
//==============================================================================
void enable_motor(uint8 idx, uint8 val){
    // Enables motor idx, according to val value
    if (idx == 0) {
        MOTOR_ON_OFF_1_Write(val);
    }   
    if (idx == 1) {
        MOTOR_ON_OFF_2_Write(val);
    }
}



//==============================================================================
//                                                              RESET COUNTERS
//==============================================================================
void reset_counters() {
    uint8 i;
    
    // Initialize counters        
    g_mem.cnt.emg_act_counter[0] = g_mem.cnt.emg_act_counter[1] = 0;
    for(i = 0; i< 10; i++){
        g_mem.cnt.position_hist[i] = 0;
    }
    for(i = 0; i< 4; i++){
        g_mem.cnt.current_hist[i] = 0;
    }
    g_mem.cnt.rest_counter = 0;
    g_mem.cnt.wire_disp = 0;
    g_mem.cnt.total_runtime = 0;
    g_mem.cnt.total_time_rest = 0; 
    g_mem.cnt.power_cycles = 0;
    g_mem.cnt.excessive_signal_activity[0] = g_mem.cnt.excessive_signal_activity[1] = 0;
    g_mem.cnt.motion_counter[0] = g_mem.cnt.motion_counter[1] = 0;
}


//==============================================================================
//                                                                       INVSQRT
//==============================================================================
float invSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

//==============================================================================
//                                                                  V3_NORMALIZE
//==============================================================================
void v3_normalize(float v3_in[3]){
    // Vector 3 normalization
    float norm = invSqrt(v3_in[0] * v3_in[0] + v3_in[1] * v3_in[1] + v3_in[2] * v3_in[2]);
    
    v3_in[0] = v3_in[0]*norm;
    v3_in[1] = v3_in[1]*norm;
    v3_in[2] = v3_in[2]*norm;
}
 
//==============================================================================
//                                                                  V4_NORMALIZE
//==============================================================================
void v4_normalize(float v4_in[4]){
    // Vector 4 normalization
    float norm = invSqrt(v4_in[0] * v4_in[0] + v4_in[1] * v4_in[1] + v4_in[2] * v4_in[2] + v4_in[3] * v4_in[3]);
    
    v4_in[0] = v4_in[0]*norm;
    v4_in[1] = v4_in[1]*norm;
    v4_in[2] = v4_in[2]*norm;
    v4_in[3] = v4_in[3]*norm;
}

//==============================================================================
//                                                            UPDATE_EMG_HISTORY
//==============================================================================
void update_EMG_history(){
    
    static int32 last_counter_value = 6000;
    
    uint32 current_counter_value = (uint32)CYCLES_TIMER_ReadCounter();
    int32 counter_diff = last_counter_value - current_counter_value;
    
    if ( counter_diff >= 10 || counter_diff <= -10 ){
        // Run this part only after 10 CYCLES_TIMER timer steps (50Hz timer -> 5 Hz)
        
        
        for (int i = 0; i < NUM_OF_INPUT_EMGS; i++){
            emg_history[emg_history_next_idx][i] = g_adc_meas.emg[i];
        }        
        
        last_counter_value = current_counter_value;
        
        emg_history_next_idx++;
        if (emg_history_next_idx >= SAMPLES_FOR_EMG_HISTORY){
            emg_history_next_idx = 0;
        }
    }
}

//==============================================================================
//                                                         SET MOTOR DRIVER TYPE
//==============================================================================
void set_motor_driver_type(){
    // Set Motor Driver Type Control Register according to the two motors drivers
    uint8 aux[NUM_OF_MOTORS] = {0,0};
    
    for (int i=0; i < NUM_OF_MOTORS; i++){
        if (g_mem.motor[i].motor_driver_type == DRIVER_MC33887){
            aux[i] = 0;
        }
        else {      // Valid for both DRIVER_VNH5019 and DRIVER_BRUSHLESS
            aux[i] = 1;
        }
    }
    
    MOTOR_DRIVER_TYPE_Write((aux[1] << 1) | aux[0]);    // Note: leave numeric indices (not parameteric)        
    
    // Change CLOCK_PWM divider in case a Maxon ESC for Brushless motor is used and restart the timer
    // NOTE: if at least one Brushless is used, the PWM frequency will change for all motors
    if (g_mem.motor[0].motor_driver_type == DRIVER_BRUSHLESS  || 
       (g_mem.dev.use_2nd_motor_flag == TRUE && g_mem.motor[1].motor_driver_type == DRIVER_BRUSHLESS)){
        //CLOCK_PWM_SetDividerValue(90);      // 48 Mhz / 90 = 533.333 KHz (nearest value to 536 Khz desired frequency for Maxon ESC)
        CLOCK_PWM_SetDividerValue(3);                   // 48 Mhz / 3 = 16 MHz
        PWM_MOTORS_WritePeriod(PWM_MAX_VALUE_ESC);      // 16 MHz / 2985 = 5.36 KHz (5.36 Khz desired frequency for Maxon ESC)
        dev_pwm_sat[0] = PWM_MAX_VALUE_ESC;
        dev_pwm_sat[1] = PWM_MAX_VALUE_ESC;
    }
    else {
        CLOCK_PWM_SetDividerValue(24);       // 48 MHz / 24 = 2 MHz
        PWM_MOTORS_WritePeriod(PWM_MAX_VALUE_DC);
        dev_pwm_sat[0] = PWM_MAX_VALUE_DC;
        dev_pwm_sat[1] = PWM_MAX_VALUE_DC;
    }   
}
/* [] END OF FILE */