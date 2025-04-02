/*
 * Control.h
 *
 *  Created on: 2025年2月22日
 *      Author: zhuji
 */

#ifndef CODE_CONTROL_H_
#define CODE_CONTROL_H_

#include "zf_common_headfile.h"

// 全局参数
#define IMAGE_HEIGHT   120      // 图像高度
#define WIDTH          188     // 图像宽度
#define TARGET         (WIDTH / 2)
#define MAX_INTEGRAL   500.0f  // 积分限幅
#define KP              2.0     // 增大比例项
#define KI              0.02    // 有条件积分
#define KD              0.3     // 引入微分抑制延迟
#define ROI_START_ROW   72      // 假设图像高度60，ROI从第36行开始（下40%）

#define Constant_Speed 1000

#define err_max 20
#define err_min -20

//#define WHEEL_DISTANCE 0.18  // 轮距（单位：米）
//#define WHEEL_BASE 0.15      // 轴距（单位：米）
#define MAX_SPEED 1.0       // 最大速度（单位：米/秒）

//extern uint8 center_line[image_h];

extern int Speed_init_L;
extern int Speed_init_R;
extern int flag;
extern int flag1;
extern int flag_t;
extern int Increase1;
extern int Increase2;
extern double steer;

//extern int *Control_Speed_left;
//extern int *Control_Speed_right;


/* 控制器参数结构，只保留必要参数 */
typedef struct {
    int look_ahead_y;      // 前瞻行，单位：像素（例如300）
    double kp_angle;       // 前瞻角度增益
    double kp_offset;      // 底部偏移增益
    double smoothing_factor; // 输出角度平滑因子，范围0~1，越大则平滑性越高
    double last_steer;     // 上一帧输出转向角（单位：弧度）
} LaneController;


void Control_car(void);

#endif /* CODE_CONTROL_H_ */
