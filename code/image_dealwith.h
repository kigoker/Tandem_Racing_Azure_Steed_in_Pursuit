///*
// * image_dealwith.h
// *
// *  Created on: 2025??2??16??
// *      Author: zhuji
// */
//
//#ifndef CODE_IMAGE_DEALWITH_H_
//#define CODE_IMAGE_DEALWITH_H_
//#include "zf_common_headfile.h"
//
////???????  ???????????????????????????????????
//#define uesr_RED     0XF800    //???
//#define uesr_GREEN   0X07E0    //???
//#define uesr_BLUE    0X001F    //???
//
//#define white_pixel 255
//#define black_pixel 0
//
////????
//#define image_h 120//?????
//#define image_w 188//??????
//
//#define ERROR_COUNT 80
//
//#define bin_jump_num    1//?????????
//#define border_max  image_w-2 //???????
//#define border_min  1   //?????��?
//
//extern uint8 original_image[image_h][image_w];
//extern uint8 bin_image[image_h][image_w];//???????
//extern uint8 state ;//Բ��״̬
//extern uint8 crossroad_flag ;//ʮ��״̬
///** @brief ʮ��·�ڶϵ�����  */
//extern uint8_t leftup[2] ;
//extern uint8_t rightup[2] ;
//uint8_t GetRoadWide(uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], uint8_t Roadwide[MT9V03X_H]);
//uint8_t UpdownSideGet(uint8_t imageInput[MT9V03X_H][MT9V03X_W], uint8_t imageOut[2][MT9V03X_W]);
//uint8_t RoundaboutGetArc(uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], uint8_t status, uint8_t num, uint8_t* index);
//void ImageAddingLine(uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], uint8_t status, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY);
//uint8_t RoadUpSide_Mono(uint8_t X1, uint8_t X2, uint8_t up_border[MT9V03X_W]);
//void X_Find_Point(uint8_t start_point, uint8_t end_point, uint8_t up_and_down[2][MT9V03X_W], uint8_t pointup[2]);
//void Crossroad_Find(uint8_t up_and_down[2][MT9V03X_W], uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], uint8_t Roadwide[MT9V03X_H], uint8_t *Flag);
//void CheckSkewEntry(uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], uint8_t* type);
//void Crossroad_Process(uint8_t up_and_down[2][MT9V03X_W], uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], uint8_t Roadwide[MT9V03X_H], uint8_t* flag);
//void image_process(void); //??????��??????????????????????????
//
//#define IMG_BLACK     0X00      //0x00???
//#define IMG_WHITE     0Xff      //0xff???
//
//// ��·����ö��
//typedef enum {
//    STRAIGHT,
//    LEFT_CURVE,
//    RIGHT_CURVE,
//    UNKNOWN
//} RoadType;
//
//// ���ʼ���ṹ��
//
//extern double a0;        // ������
//extern double a1;        // һ����ϵ��
//extern double a2;        // ������ϵ��
//extern double curvature; // ����ֵ
//
////typedef struct {
////    double curvature;  // ����
////    double a2;         // ����ϵ����������������
////    int trend_score;   // �������
////    int jump_direction; // �߽����䷽��
////} SegmentInfo;
////#define ERROR_COUNT1 120  // ���鳤��Ϊ 120
////
////uint8 jiaquan_[ERROR_COUNT1] = {
////    10, 10, 10, 10, 10, 10, 10, 10,  // 119-112
////    9,  9,  9,  9,  9,  9,  9,  9,   // 111-104
////    8,  8,  8,  8,  8,  8,  8,  8,   // 103-96
////    7,  7,  7,  7,  7,  7,  7,  7,   // 95-88
////    6,  6,  6,  6,  6,  6,  6,  6,   // 87-80
////    5,  5,  5,  5,  5,  5,  5,  5,   // 79-72
////    4,  4,  4,  4,  4,  4,  4,  4,   // 71-64
////    3,  3,  3,  3,  3,  3,  3,  3,   // 63-56
////    2,  2,  2,  2,  2,  2,  2,  2,   // 55-48
////    1,  1,  1,  1,  1,  1,  1,  1,   // 47-40
////    1,  1,  1,  1,  1,  1,  1,  1,   // 39-32
////    1,  1,  1,  1,  1,  1,  1,  1,   // 31-24
////    1,  1,  1,  1,  1,  1,  1,  1,   // 23-16
////    1,  1,  1,  1,  1,  1,  1,  1,   // 15-8
////    1,  1,  1,  1,  1,  1,  1,  1    // 7-0
////};
//// ��·ʶ���������
//#define CURVATURE_THRESHOLD     0.005  // ������ֵ�����Ժ��趨��
//#define WIDTH_VARIATION_THRESH  50     // ��ȱ仯��ֵ
//#define MIN_VALID_ROWS          100     // ��С��Ч����
//#define LOOK_AHEAD_Y            50     // ǰհ�У��в���
//#define STRAIGHT_CONFIRM_COUNT  5      // ֱ��ȷ�ϼ���
//
//
////Type��0=ֱ�ߣ�1=���䣬2=���䣬3=δ֪��
//extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
//extern uint8 image_dealwith[MT9V03X_H][MT9V03X_W];
//int get_curve_info(uint8_t* center_line, int start_row, int end_row, double* a0, double* a1, double* a2, double* curvature);
//int analyze_width_trend(int* trend_score);
//int detect_border_jump(int* direction);
//void CheckRoadMonotone(uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], uint8_t *flag);
//void GetLostLineFlags(uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], int Left_Lost_Flag[MT9V03X_H], int Right_Lost_Flag[MT9V03X_H]);
//RoadType recognize_road_type(void);
//
//
//void Binarization_Algorithm(void);
//
//float get_center_error(void);
//
//extern uint8 l_border[image_h];//????????
//extern uint8 r_border[image_h];//????????
//extern uint8 center_line[image_h];//????????
//extern uint8_t up_and_down[2][MT9V03X_W];//���±���0shang1xia
//extern uint8_t Roadwide[MT9V03X_H];//����
//extern uint8 hightest;//��ߵ�
//extern uint8_t leftState, rightState; // �����Ա�־
//extern uint8_t l_monotone_inc;  // ������Ƿ�ȫ�̵���
//extern uint8_t r_monotone_dec;  // �ұ����Ƿ�ȫ�̵ݼ�
//extern uint8 road_flag;
//extern uint8 cross_state; // 0: Զʮ��, 1: ʮ����
//extern uint8 mid_cross_type; // 0: �Գ�, 1: ��б��, 2: ��б��
//#endif /* CODE_IMAGE_DEALWITH_H_ */
