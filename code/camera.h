/*
 * camera.h
 *
 *  Created on: 2025��3��21��
 *      Author: ydc
 */

#ifndef CODE_CAMERA_H_
#define CODE_CAMERA_H_

#include "zf_common_headfile.h"

#define LCDH 60                              //����ͼ�����ͼ��߶ȣ��У�
#define LCDW 94                              //����ͼ�����ͼ���ȣ��У�
#define ImageSensorMid 46                    //ͼ�����Ļ�е�
#define USE_IMAGE_SIZE LCDH * LCDW
#define Image_white 255
#define Image_black 0
#define LimitL(L) (L = ((L < 1) ? 1 : L))    //�޷��޷�
#define LimitH(H) (H = ((H > 92) ? 92 : H))  //�޷��޷�
#define Garage_Number 2                      //�������
extern int Mid_line[60];                   //�ʼд��Get_SideLine�ѱ��ߺ�����������������ߵ�����
extern int Left_line[60];                  //�ʼд��Get_SideLine�ѱ��ߺ������������������ߵ�����
extern int Right_line[60];                 //�ʼд��Get_SideLine�ѱ��ߺ���������������ұ��ߵ�����
extern int Out_Garage_Help_Flag;   // 1 :��߳�����   2 :�ұ߳�����
extern int Out_Garage_Help_enable;   // 1 :������   2 :�ѳ�����
extern int RoadBlock_length;
extern uint8 Pixle[LCDH][LCDW];
extern uint8* Image_Use[LCDH][LCDW];
extern uint8 Threshold;
extern uint16 threshold1,threshold2,threshold3,block_yuzhi;
extern uint16 yuzhi1,yuzhi2,yuzhi3;
extern uint16 Ramp_cancel,circle_stop,block_num,duan_num;
extern int Black;
extern uint8 Auto_Through_RoadBlock ; // �Զ�ѡ��ͨ��·�Ϸ�ʽ   1 : ��ͨ��·��   2 : ��ͨ��·��
extern uint8 RoadBlock_Flag ; //·��λ��  1 : ��һ�δ��  2 : �����������н�·��  3 : �ڶ��δ�ǻص�����  4 : Ϊ����ĳ�����������ص�д������
extern uint8 RoadBlock_Thruough_Flag ; //·��ͨ����ʽ  1 : ��ͨ��  2 : ��ͨ��
extern uint8 RoadBlock_Thruough_Flag_Record,Bend_Thruough_RoadBlock_Flag,ICM20602_Clear_Flag ;
extern uint8 Regular_RoadBlock;
//ÿһ�е�����
typedef struct {
    //�ݶ�ɨ������
  uint8 IsRightFind;      //�ұ��б߱�־
  uint8 IsLeftFind;       //����б߱�־
  int Wide;               //�߽���
  int LeftBorder;         //����� �ݶ�ɨ��
  int RightBorder;        //�ұ���
  int Center;              //����

  int TopBorder;              //��������
    //�����ַ���ɨ������
  int LeftBoundary_First;  //��߽� �����һ������
  int RightBoundary_First; //�ұ߽� �����һ������
  int LeftBoundary;        //��߽� �������һ������
  int RightBoundary;       //�ұ߽� �������һ������
} ImageDealDatatypedef;

typedef struct {
  //ͼ����Ϣ
  int TowPoint;             //�����У�Ҳ������˵��ǰհ

  int16 OFFLine;            //ͼ�񶥱�
  int16 WhiteLine;          //˫�߶�����
    int16 OFFLineBoundary;   //�������ֹ��
    int16 Miss_Left_lines;                      //���߶�ʧ���ݶ�ɨ�ߴ������ߡ�W�����͵ĵ�
  int16 Miss_Right_lines;                     //���߶�ʧ
    int Det_True;
    float MU_P;
    float MU_D;
    //�����ַ���ɨ������
  int16 WhiteLine_L;        //��߶�����
  int16 WhiteLine_R;        //�ұ߶�����
} ImageStatustypedef;

typedef struct {
  int point;
  uint8 type;
} JumpPointtypedef;

typedef struct {
    int16 zafawfwagsg;
    int16 Bend_Road;                            /*0 :��               1 :�����     2 :�����*/
    int16 image_element_rings;                  /*0 :��Բ��          1 :��Բ��       2 :��Բ��*/
    int16 ring_big_small;                       /*0:��                     1 :��Բ��       2 :СԲ��*/
    int16 zafawfgwa;
    int16 image_element_rings_flag;             /*Բ������*/
    int16 straight_long;                        /*��ֱ����־λ*/
    int16 straight_xie;
    int16 Garage_Location;                      /*0 :�޳���          1 :�󳵿�       2 :�ҳ���*/
    int16 Zebra_Flag;                           /*0 :�ް�����       1 �󳵿�       2 :�ҳ���*/
    int16 Ramp;                                  /*0 :���µ�          1���µ�*/
    int16 RoadBlock_Flag;                        /*0 :��·��            1 :·��*/
    int16 Out_Road;                               /*0 :�޶�·      1 :��·*/
} ImageFlagtypedef;

extern ImageDealDatatypedef ImageDeal[60];
extern ImageStatustypedef ImageStatus;
extern ImageFlagtypedef ImageFlag;

extern float center_line_error;

void image_process(void);
void ips200_displayimage032_zoom(void);

#endif /* CODE_CAMERA_H_ */
