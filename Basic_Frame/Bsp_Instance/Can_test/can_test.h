#ifndef CAN_TEST_H
#define CAN_TEST_H

#include "bsp_can.h"

extern CAN_HandleTypeDef hcan1;

void motor1_init(void);

void motor1_con(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
void CAN_cmd(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);	
void can_filter_init(void);
#endif // CAN_TEST_H
