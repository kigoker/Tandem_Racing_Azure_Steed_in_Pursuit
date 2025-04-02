/*
 * Motor.c
 *
 *  Created on: 2025年1月11日
 *      Author: zhuji
 */
#include "Motor.h"

/*-------------------------------------------------------------------------------------------------------------------
  @brief     电机初始化函数
  @param     NULL
  @return    null
  Sample     Motor_Left(pwm_L);
  @note      电机最终输出函数，由其他函数调用
                                      在给电机前有限幅处理的
-------------------------------------------------------------------------------------------------------------------*/
void Motor_Init(void)
{
    pwm_init(MOTOR01_SPEED_PIN, 17000, 0);
    pwm_init(MOTOR02_SPEED_PIN, 17000, 0);
    pwm_set_duty(MOTOR01_SPEED_PIN,0);
    pwm_set_duty(MOTOR02_SPEED_PIN,0);
    gpio_init(MOTOR01_DIR_PIN, GPO, 0, GPO_PUSH_PULL);
    gpio_init(MOTOR02_DIR_PIN, GPO, 0, GPO_PUSH_PULL);
    PID_Init(&sptr1);
    PID_Init(&sptr2);
        //PID初始化
        //增量式PID
    imu_PID_Init(&imu);
        //右电机参数
        sptr1.P=2.5;////对应speed2编码器读数
        sptr1.I=1.2;
        sptr1.D=0;
        //左电机参数
        sptr2.P=2.5;//对应speed1编码器读数
        sptr2.I=1.18;
        sptr2.D=0;

            //imu转向环参数
        imu.KP_1 = 30;
        imu.KD_1 = 0;
     // imu.KP_2 = 0;
        imu.GKD = -1.5;


//    //学长版
//    //右电机参数
//    sptr1.P=2.5;////对应speed2编码器读数
//    sptr1.I=1.2;
//    sptr1.D=0;
//    //左电机参数
//    sptr2.P=2.5;//对应speed1编码器读数
//    sptr2.I=1.18;
//    sptr2.D=0;
//    //imu转向环参数
//    imu.KP_1 = 20;
//    imu.KD_1 = 20;
//    // imu.KP_2 = 0;
//    imu.GKD = -0.8;

}
    //       setspeed1 = 2000;//编码器对应速度2.7m/s
    //       setspeed2 = 2000;
    //       sptr1.P=1.5;////对应speed2编码器读数
    //           sptr1.P=1.5;////对应speed2编码器读数
    //           sptr1.I=1.24;
    //           sptr1.D=0;
    //
    //           sptr2.P=2;//对应speed1编码器读数
    //           sptr2.I=1.17;
    //           sptr2.D=0;

/*-------------------------------------------------------------------------------------------------------------------
  @brief     电机输出函数
  @param     pwm_L 电机输入占空比，可正可负
  @return    null
  Sample     Motor_Left(pwm_L);
  @note      电机最终输出函数，由其他函数调用
                                      在给电机前有限幅处理的
-------------------------------------------------------------------------------------------------------------------*/
void Motor_Left(int pwm_L)
{
    if(pwm_L>=SPEED_MAX)//限幅处理
        pwm_L=SPEED_MAX;
    else if(pwm_L<=SPEED_MIN)
             pwm_L=SPEED_MIN;

    if(pwm_L>=0)//正转
    {
        pwm_set_duty(MOTOR01_SPEED_PIN, pwm_L);//左电机正转
        gpio_set_level(MOTOR01_DIR_PIN, 0);//01左电机，D15,0反转，1正转
    }
    else
    {
        pwm_set_duty(MOTOR01_SPEED_PIN, -pwm_L);//左电机正转
        gpio_set_level(MOTOR01_DIR_PIN, 1);//01左电机，D15,0反转，1正转
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     电机输出函数
  @param     pwm_R 电机输入占空比，可正可负
  @return    null
  Sample     Motor_Right(pwm_R);
  @note      电机最终输出函数，由其他函数调用
                                      在给电机前有限幅处理的
-------------------------------------------------------------------------------------------------------------------*/
void Motor_Right(int pwm_R)
{
    if(pwm_R>=SPEED_MAX)//限幅处理
        pwm_R=SPEED_MAX;
    else if(pwm_R<=SPEED_MIN)
            pwm_R=SPEED_MIN;

    if(pwm_R>=0)//正转
    {
        pwm_set_duty(MOTOR02_SPEED_PIN, pwm_R);//右电机正转
        gpio_set_level(MOTOR02_DIR_PIN, 0);//02左电机，D14,0反转，1正转
    }
    else
   {
        pwm_set_duty(MOTOR02_SPEED_PIN, -pwm_R);//右电机正转
        gpio_set_level(MOTOR02_DIR_PIN, 1);//02左电机，D14,0反转，1正转
   }
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     设置左右轮转速
  @param     pwm_R 电机输入占空比，可正可负
  @return    null
  Sample     Motor_Right(pwm_R);
  @note      电机最终输出函数，由其他函数调用
                                      在给电机前有限幅处理的
-------------------------------------------------------------------------------------------------------------------*/
void wu_shua(void)
{
    pwm_set_duty(ATOM0_CH2_P21_4,1000);
    pwm_set_duty(ATOM1_CH3_P21_5,1000);


}

