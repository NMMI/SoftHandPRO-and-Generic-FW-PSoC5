#include "QB_lib/commands.h"
#include "QB_lib/qbmove_communications.h"
#include "QB_lib/cp_commands.h"
#include "QB_lib/cp_communications.h"
#include "iostream"
#include <math.h>
#include <fstream>
#include <signal.h>
#include <sys/time.h>
#include <windows.h>
#include <unistd.h>
#include <chrono>
#include <ctime>
    short int measurements[4];  
comm_settings comm_settings_t;
//uint8_t aux_string[5000];
//uint8_t PARAM_SLOT_BYTES = 50;
//int num_of_params;
//int n_imu;
//uint8_t* ids;
//uint8_t* imu_table;
//uint8_t* mag_cal;
//float* imu_values;
//uint8_t num_imus_id_params = 7;
//uint8_t num_mag_cal_params = 0;
//uint8_t first_imu_parameter = 2;
//uint8_t new_board = 1;
int i = 0;
struct timeval newstart, stop;
int device_id = 1;
 int sensor_num = 0;

int main(){

	
    openRS485 ( &comm_settings_t , "COM14", 2000000) ;       // QB Board Opening

	usleep(500000);
	
	 while(1) {
            sensor_num = commGetMeasurements(&comm_settings_t, device_id, measurements);
sensor_num=1;
            if(sensor_num < 0 || sensor_num > 4) {
                printf("An error occurred or the device is not supported\n");
                break;
            }
            else {
                printf("measurements:     ");
                for (i = 0; i < sensor_num; i++) {
                    printf("%d  ", (int)measurements[i]);
                }
                printf("\n");

                usleep(100000);
            }
        }

    return 0;
}
