///*
// * Control.h
// *
// *  Created on: 2025��2��22��
// *      Author: zhuji
// */
//
//#ifndef CODE_CONTROL_H_
//#define CODE_CONTROL_H_
//
//#include "zf_common_headfile.h"
//
//// ȫ�ֲ���
//#define IMAGE_HEIGHT   120      // ͼ��߶�
//#define WIDTH          188     // ͼ����
//#define TARGET         (WIDTH / 2)
//#define MAX_INTEGRAL   500.0f  // �����޷�
//#define KP              2.0     // ���������
//#define KI              0.02    // ����������
//#define KD              0.3     // ����΢�������ӳ�
//#define ROI_START_ROW   72      // ����ͼ��߶�60��ROI�ӵ�36�п�ʼ����40%��
//
//#define Constant_Speed 1000
//
//#define MAX_SPEED 1900
//#define MIN_SPEED 1100
//#define MAX_DEVIATION 25
//
//#define err_max 20
//#define err_min -20
//
////#define WHEEL_DISTANCE 0.18  // �־ࣨ��λ���ף�
////#define WHEEL_BASE 0.15      // ��ࣨ��λ���ף�
////#define MAX_SPEED 1.0       // ����ٶȣ���λ����/�룩
//
////extern uint8 center_line[image_h];
//extern float ratio ;
//extern int Speed_init_L;
//extern int Speed_init_R;
//extern int flag;
//extern int flag1;
//extern int flag_t;
//extern int Increase1;
//extern int Increase2;
//extern double steer;
//extern double steer1 ;
//extern float err_road;
////extern int *Control_Speed_left;
////extern int *Control_Speed_right;
//extern uint8 quhang;
//float weighted_error_exp(int8 *errors);
//
///* �����������ṹ��ֻ������Ҫ���� */
//typedef struct {
//    int look_ahead_y;      // ǰհ�У���λ�����أ�����300��
//    double kp_angle;       // ǰհ�Ƕ�����
//    double kp_offset;      // �ײ�ƫ������
//    double smoothing_factor; // ����Ƕ�ƽ�����ӣ���Χ0~1��Խ����ƽ����Խ��
//    double last_steer;     // ��һ֡���ת��ǣ���λ�����ȣ�
//} LaneController;
//
//
//void Control_car(void);
//
//#endif /* CODE_CONTROL_H_ */
