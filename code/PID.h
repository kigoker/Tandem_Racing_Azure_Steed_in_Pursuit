/*
 * PID.h
 *
 *  Created on: 2025年1月25日
 *      Author: zhuji
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

#include "zf_common_headfile.h"

//#define SPEED_MAX   3000  //电机速度限幅，正
//#define SPEED_MIN  -3000  //电机速度限幅，负


typedef struct
{
        float P;
        float I;
        float D;

        float LastError;
        float PrevError;

}PID_Datatypedef;
typedef struct
{
        float kP;
        float kI;
        float kD;

        float LastError1;
        float PrevError1;

}PID_Angeltypedef;
typedef struct
{
        float KP_1;
        float KD_1;
        float GKD;
        float KP_2;
        float lasterror;
        float integrator;   // 积分项初始化为0
}PID_imu_Datatypedef;
extern float erspeed;
extern int Increase1;
extern int Increase2;
extern int speed1;
extern int speed2;
extern int setspeed1;
extern int basespeed;
extern int setspeed2;
extern float divertion;
extern float qulv;
extern float Error;
extern PID_imu_Datatypedef imu;
extern PID_Datatypedef sptr1,sptr2;
extern PID_Angeltypedef angel;
//extern float P_L;
//extern float I_L;
//extern float P_R;
//extern float I_R;

void PID_Init(PID_Datatypedef*sptr);
void PID_output(void);
//void PID_select(void);
float imuPID_Output(float erspeed,PID_imu_Datatypedef*imu);
int MotorPID_Output(PID_Datatypedef*sptr,float NowSpeed,int ExpectSpeed);
float imuPID_Output(float erspeed,PID_imu_Datatypedef*imu);
//int PID_L(int set_speed ,float speed);
//int PID_R(int set_speed ,float speed);




#endif /* CODE_PID_H_ */
