/*
 * imu.h
 *
 *  Created on: 2025年3月6日
 *      Author: 赵贺嘉
 */

#ifndef CODE_IMU_H_
#define CODE_IMU_H_
#include "zf_common_headfile.h"


/* 灵敏度 */
#define   GYRO_SENS             1/16.4
#define   ACCE_SENS             90.0/4096
#define   DT                    0.01

/**********申明变量*************/
extern volatile float FJ_Pitch;//偏航角
extern volatile float FJ_Angle;//俯仰角

void Gyroscope_Init(void);
void Gyroscope_GetData(void);
void Zero_Point_Detect(void);
void Get_Gyroscope_Angle(void);
void Clear_Gyroscope_Angle(void);
void Get_Gyroscope_Pitch(void);
void Clear_Gyroscope_Pitch(void);





#endif /* CODE_IMU_H_ */
