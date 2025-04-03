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
//#define border_min  1   //?????С?
//
//extern uint8 original_image[image_h][image_w];
//extern uint8 bin_image[image_h][image_w];//???????
//extern uint8 state ;//圆环状态
//extern uint8 crossroad_flag ;//十字状态
///** @brief 十字路口断点坐标  */
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
//void image_process(void); //??????ж??????????????????????????
//
//#define IMG_BLACK     0X00      //0x00???
//#define IMG_WHITE     0Xff      //0xff???
//
//// 道路类型枚举
//typedef enum {
//    STRAIGHT,
//    LEFT_CURVE,
//    RIGHT_CURVE,
//    UNKNOWN
//} RoadType;
//
//// 曲率计算结构体
//
//extern double a0;        // 常数项
//extern double a1;        // 一次项系数
//extern double a2;        // 二次项系数
//extern double curvature; // 曲率值
//
////typedef struct {
////    double curvature;  // 曲率
////    double a2;         // 二阶系数（决定弯曲方向）
////    int trend_score;   // 宽度趋势
////    int jump_direction; // 边界跳变方向
////} SegmentInfo;
////#define ERROR_COUNT1 120  // 数组长度为 120
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
//// 道路识别参数配置
//#define CURVATURE_THRESHOLD     0.005  // 曲率阈值（调试后设定）
//#define WIDTH_VARIATION_THRESH  50     // 宽度变化阈值
//#define MIN_VALID_ROWS          100     // 最小有效行数
//#define LOOK_AHEAD_Y            50     // 前瞻行（中部）
//#define STRAIGHT_CONFIRM_COUNT  5      // 直线确认计数
//
//
////Type：0=直线，1=左弯，2=右弯，3=未知。
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
//extern uint8_t up_and_down[2][MT9V03X_W];//上下边线0shang1xia
//extern uint8_t Roadwide[MT9V03X_H];//赛宽
//extern uint8 hightest;//最高点
//extern uint8_t leftState, rightState; // 单调性标志
//extern uint8_t l_monotone_inc;  // 左边线是否全程递增
//extern uint8_t r_monotone_dec;  // 右边线是否全程递减
//extern uint8 road_flag;
//extern uint8 cross_state; // 0: 远十字, 1: 十字中
//extern uint8 mid_cross_type; // 0: 对称, 1: 左斜入, 2: 右斜入
//#endif /* CODE_IMAGE_DEALWITH_H_ */
