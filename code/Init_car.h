/*
 * Init_car.h
 *
 *  Created on: 2025年1月11日
 *      Author: zhuji
 */

#ifndef CODE_INIT_CAR_H_
#define CODE_INIT_CAR_H_

#include "zf_common_headfile.h"

extern int Speed_Setleft;
extern int Speed_SetRight;

extern int *Set_left;
extern int *Set_Right;

//电机限幅
#define SPEED_MAX   10000  //电机速度限幅，正
#define SPEED_MIN  -10000  //电机速度限幅，负

//WIFI_SPI 参数设置
#define WIFI_SPI_OPEN 1  //0关闭WIFI_SPI 1开启
#define INCLUDE_BOUNDARY_TYPE 0 //摄像头图像传输模式 0为不发送边界  1为发送边界

extern uint8 image_copy[MT9V03X_H][MT9V03X_W];

#define WIFI_SSID_TEST          "411"    //连接WIFI名称

#define WIFI_PASSWORD_TEST      "cszwz411"  // 如果需要连接的WIFI 没有密码则需要将 这里 替换为 NULL

extern uint8 data_buffer[32];
extern uint8 data_len;

void set_wheel_Speed(int *Set_left,int *Set_Right, int left_speed, int right_speed, int16 Speed_Left_Real, int16 Speed_Right_Real);

void GANJIANG_Init(void);
void GANJIANG_Flyer(void);

void Wireless_uart_readAndsent(void);
void WIFI_SPI_Init(void);

#endif /* CODE_INIT_CAR_H_ */
