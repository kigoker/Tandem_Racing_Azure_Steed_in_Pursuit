/*
 * PID.c
 *
 *  Created on: 2025年1月25日
 *      Author: zhuji
 */
#include "PID.h"

int basespeed=1000;
int setspeed1=1000;
int setspeed2=1000;
int speed1;//左电机
int speed2;
int Increase1=0;
int Increase2=0;
float divertion;
float erspeed=0;

float gyro_filtered = 0; // 滤波后的陀螺仪值

PID_Datatypedef sptr1,sptr2;
PID_imu_Datatypedef imu;
PID_Angeltypedef angel;
//float P_L=0.1;
//float I_L=0;
//float P_R=0.1;
//float I_R=0;

void PID_Init(PID_Datatypedef*sptr)
{
    sptr->P=0;
    sptr->I=0;
    sptr->D=0;
    sptr->LastError=0;
    sptr->PrevError=0;
}
void imu_PID_Init(PID_imu_Datatypedef*imu)
{
    imu->KP_1=0;
    imu->KD_1=0;
    imu->KP_2=0;
    imu->GKD=0;
    imu->lasterror=0;

    imu->integrator = 0;   // 积分项初始化为0
}
int MotorPID_Output(PID_Datatypedef*sptr,float NowSpeed,int ExpectSpeed)
{
    int Increase;
    int iError;
    iError=ExpectSpeed-NowSpeed;
    Increase=(int)(sptr->P*(iError-sptr->LastError)+sptr->I*iError+sptr->D*(iError-2*sptr->LastError+sptr->PrevError));
    sptr->PrevError=sptr->LastError;
    sptr->LastError=iError;
    return Increase;
}
float imuPID_Output(float erspeed,PID_imu_Datatypedef*imu)
{
    float imu_out;
//    imu_out=erspeed*imu->KP_1+(erspeed-imu->lasterror)*imu->KD_1-imu660ra_gyro_x*imu->GKD;
//    imu->lasterror=erspeed;
    imu_out = (erspeed * imu->KP_1) + (erspeed * fabsf(erspeed) * imu->KP_2) + (erspeed - imu->lasterror) * imu->KD_1 + imu660ra_gyro_x * imu->GKD;
    imu->lasterror = erspeed; // 更新上一次误差
    if (imu_out > 2500) imu_out =2500;
    else if (imu_out < -2500) imu_out = -2500;
    return imu_out;

}
//int ang_pid(float b,int c)
//{
//    int t;
//    int temp_speed;
//
//    temp_speed=(speed1+speed2)/2;
//
//    t=angel.kP*(temp_speed-b)+angel.kD*(temp_speed-angel.LastError1);
//    angel.LastError1=temp_speed-b;
//
//    return t;
//
//}
void PID_output(void)
{

//    static float last_Increase1 = 0;
//    static float last_Increase2 = 0;

//            增量式PID控制小车直行
    Increase1=MotorPID_Output(&sptr1,speed1,setspeed1);
    Increase2=MotorPID_Output(&sptr2,speed2,setspeed2);
            //方向环直接扭转小车运行方向
//    Increase1=Increase1-divertion;
//    Increase2=Increase2+divertion;
//    Increase1=-divertion;
//    Increase2=+divertion;
    divertion=imuPID_Output(erspeed,&imu);
    Increase1=Increase1-divertion;
    Increase2=Increase2+divertion;
    // 限制输出，防止反转或过大
//    if (Increase1 < 0) Increase1 = 0.02*Increase2;
//    if (Increase2 < 0) Increase2 = 0.02*Increase1;
//    if (Increase1 > 2500) Increase1 = 3000;
//    if (Increase2 > 2500) Increase2 = 3000;
    Motor_Left(Increase1);
    Motor_Right(Increase2);
//    Motor_Left(1000);
//    Motor_Right(1000);

}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     PID控制
  @param     int set_speed ,int speed,期望值，实际值
  @return    电机占空比
  Sample     pwm_R= PID_R(set_speed_right,right_wheel);//pid控制电机转速
             pwm_L= PID_L(set_speed_left,left_wheel );//pid控制电机转速
  @note      调参呗
-------------------------------------------------------------------------------------------------------------------*/
//int PID_L(int set_speed ,float speed)//pid控制电机转速
//{
//    volatile static int out;
//    volatile static int out_increment;
//    volatile static int ek,ek1;
//    volatile static int speed_bb;
//
//    ek1 = ek;
//    ek = set_speed - speed;
//
//    if(ek>80) speed_bb=SPEED_MAX;
//    else if(ek<-80) speed_bb=SPEED_MIN;
//    else speed_bb=0;
//
//    out_increment= (int)(P_L*(ek-ek1) + I_L*ek + speed_bb);
//    out+= out_increment;
//
//    if(out>=SPEED_MAX)//限幅处理
//        out=SPEED_MAX;
//    else if(out<=SPEED_MIN)
//        out=SPEED_MIN;
//    return (int) out;
//}
//
///*-------------------------------------------------------------------------------------------------------------------
//  @brief     PID控制
//  @param     int set_speed ,int speed,期望值，实际值
//  @return    电机占空比
//  Sample     pwm_R= PID_R(set_speed_right,right_wheel);//pid控制电机转速
//             pwm_L= PID_L(set_speed_left,left_wheel );//pid控制电机转速
//  @note      调参呗
//-------------------------------------------------------------------------------------------------------------------*/
//int PID_R(int set_speed ,float speed)//pid控制电机转速
//{
//    volatile static int  out;
//    volatile static int  out_increment;
//    volatile static int  ek,ek1;
//    volatile static int speed_bb;
//
//    ek1 = ek;
//    ek = set_speed - speed;
//
//    if(ek>80) speed_bb=SPEED_MAX;
//    else if(ek<-80) speed_bb=SPEED_MIN;
//    else speed_bb=0;
//
//    out_increment= (int)(P_R*(ek-ek1) + I_R*ek + speed_bb);
//    out+= out_increment;
//
//    if(out>=SPEED_MAX)//限幅处理
//        out=SPEED_MAX;
//    else if(out<=SPEED_MIN)
//        out=SPEED_MIN;
//    return (int) out;
//}



