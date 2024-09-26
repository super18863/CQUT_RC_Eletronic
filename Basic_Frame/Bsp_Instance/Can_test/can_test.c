#include "can_test.h"

static CAN_Config motor1;
static CAN_Instance *motor1_ins;

static CAN_TxHeaderTypeDef  tx_message;
static uint8_t              can_send_data[8];

void motor1_init(void)
{
	motor1.can_handle = &hcan1;
	motor1.tx_id = 0x200;
	motor1.rx_id = 0x00;
	motor1.can_module_callback = NULL;
	motor1_ins = CANRegister(&motor1);
}

void motor1_con(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
    motor1_ins->tx_buff[0] = motor1 >> 8;
    motor1_ins->tx_buff[1] = motor1;
    motor1_ins->tx_buff[2] = motor2 >> 8;
    motor1_ins->tx_buff[3] = motor2;
    motor1_ins->tx_buff[4] = motor3 >> 8;
    motor1_ins->tx_buff[5] = motor3;
    motor1_ins->tx_buff[6] = motor4 >> 8;
    motor1_ins->tx_buff[7] = motor4;
		CANTransmit(motor1_ins, 0);
}
	
