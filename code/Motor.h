/*
 * Motor.h
 *
 *  Created on: 2025年1月11日
 *      Author: zhuji
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_common_headfile.h"

#define MOTOR01_SPEED_PIN     ATOM0_CH4_P02_4 //01左电机
#define MOTOR01_DIR_PIN       P02_5                   //01左电机，1反转，0正转
#define MOTOR02_SPEED_PIN     ATOM0_CH6_P02_6 //02右电机
#define MOTOR02_DIR_PIN       P02_7                   //02右电机，1反转，0正转

//电机初始化
void Motor_Init(void);

//电机速度设置
void Motor_Left(int pwm_L);
void Motor_Right(int pwm_R);


#endif /* CODE_MOTOR_H_ */
