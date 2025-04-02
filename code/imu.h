/*
 * imu.h
 *
 *  Created on: 2025年3月6日
 *      Author: 赵贺嘉
 */

#ifndef CODE_IMU_H_
#define CODE_IMU_H_
#include "zf_common_headfile.h"

#define delta_T 0.00003f //1ms
extern float pitch, roll, yaw,ang_z;                // Z 轴角速度积分角度        // 欧拉角 (单位: 度)

void imu660_Read(void);
void gyroOffsetInit(void);
float myRsqrt(float num);
void ImuGetValues(void);
void ImuAHRSupdate(void);
void ReadGyro(void);
void ClearGyro(void);
void GyroResolve(void);



#endif /* CODE_IMU_H_ */
