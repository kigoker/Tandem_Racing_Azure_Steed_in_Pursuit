/*
 * Init_car.h
 *
 *  Created on: 2025��1��11��
 *      Author: zhuji
 */

#ifndef CODE_INIT_CAR_H_
#define CODE_INIT_CAR_H_

#include "zf_common_headfile.h"

extern int Speed_Setleft;
extern int Speed_SetRight;

extern int *Set_left;
extern int *Set_Right;

//����޷�
#define SPEED_MAX   10000  //����ٶ��޷�����
#define SPEED_MIN  -10000  //����ٶ��޷�����

//WIFI_SPI ��������
#define WIFI_SPI_OPEN 1  //0�ر�WIFI_SPI 1����
#define INCLUDE_BOUNDARY_TYPE 0 //����ͷͼ����ģʽ 0Ϊ�����ͱ߽�  1Ϊ���ͱ߽�

extern uint8 image_copy[MT9V03X_H][MT9V03X_W];

#define WIFI_SSID_TEST          "411"    //����WIFI����

#define WIFI_PASSWORD_TEST      "cszwz411"  // �����Ҫ���ӵ�WIFI û����������Ҫ�� ���� �滻Ϊ NULL

extern uint8 data_buffer[32];
extern uint8 data_len;

void set_wheel_Speed(int *Set_left,int *Set_Right, int left_speed, int right_speed, int16 Speed_Left_Real, int16 Speed_Right_Real);

void GANJIANG_Init(void);
void GANJIANG_Flyer(void);

void Wireless_uart_readAndsent(void);
void WIFI_SPI_Init(void);

#endif /* CODE_INIT_CAR_H_ */
