/*
 * Control.c
 *
 *  Created on: 2025年2月22日
 *      Author: zhuji
 */
#include "Control.h"

#define constrain(value, min_val, max_val) ((value) < (min_val) ? (min_val) : ((value) > (max_val) ? (max_val) : (value)))
//
//int Speed_init_L = Constant_Speed;
//int Speed_init_R = Constant_Speed;
int flag=0;
int flag1=0;
int flag_t=0;

typedef struct{
        int left;
        int right;
} Control_wheel_Speed;



// 摄像头返回的中线位置数组

double calculate_steering_angle(float dt) {
    static float integral = 0;
        static float prev_error = 0;

        // 1. 仅计算 ROI 区域
        float total_error = 0;
        int valid_rows = 0;
//        for (int row = ROI_START_ROW; row < IMAGE_HEIGHT; row++) {
////            float current_error = center_line[row] - TARGET;
//            total_error += current_error;
//            valid_rows++;
//        }
        float error = total_error / valid_rows;

        // 2. 非线性放大小误差
        if (fabs(error) < 15.0f) {
            error *= 1.5;
        }

        // 3. 有条件积分（避免大误差时积分累积）
        if (fabs(error) < 20.0f) {
            integral += error * dt;
        } else {
            integral = 0;
        }
        // 积分限幅
        integral = constrain(integral, -MAX_INTEGRAL, MAX_INTEGRAL);

        // 4. 计算微分项
        float derivative = (error - prev_error) / dt;

        // 5. PID 计算
        double steer_angle = KP * error + KI * integral + KD * derivative;
        prev_error = error;

        // 6. 输出转向（限制合理范围）
        steer_angle = constrain(steer_angle, -30.0f, 30.0f);

    return steer_angle;
}

//Control_wheel_Speed steer_with_angle(double target_angle, double target_speed) {
//
////    printf("%d\n",(int)target_angle);
//    Control_wheel_Speed Speed_wheel;
//
//    // 参数检查（防止除零）
//        const double WHEEL_BASE = 15.0;         // 轴距（单位：cm，需校准）
//        const double WHEEL_DISTANCE = 20.0;     // 轮距（轮间距，单位：cm，需校准）
//
//    // 计算目标角速度
//    double omega = target_angle / 0.2;  // 假设转向时间为 1 秒
//
//    double  K_P = 0.2 ;
//
//    // 计算左右轮速度
//    Speed_wheel.left = target_speed + (WHEEL_DISTANCE / (2 * WHEEL_BASE)) * omega *K_P;
//    Speed_wheel.right  = target_speed - (WHEEL_DISTANCE / (2 * WHEEL_BASE)) * omega *K_P;
//
//    // 设置电机速度
//    return Speed_wheel;
//}



//uint8 error_[ERROR_COUNT] = {11, 11, 11, 11, 11, 11, 11, 11,
//                            10 , 10 , 10 , 10 , 10 , 10 , 10 ,10 ,
//                            9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 ,
//                            8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 ,
//                            7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 ,
//                            6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 ,
//                            4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 ,
//                            3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 ,
//                            2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
//                            1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 };
//uint8 error_[ERROR_COUNT] = {15, 15, 15, 15, 14, 14,14, 14,
//                            13 , 13 , 13 , 13 , 12 , 12 , 12 , 12,
//                            11 , 11 , 11 , 11 , 10 , 10 , 10 , 10 ,
//                            9 , 9 , 9 , 9 , 8 , 8 , 8 , 8 ,
//                            7 , 7 , 7 , 7 , 6 , 6 , 6 , 6 ,
//                            13 , 13 , 13 , 13 , 12 , 12 , 12 , 12,
//                            11 , 11 , 11 , 11 , 10 , 10 , 10 , 10 ,
//                            3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 ,
//                            2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
//                            1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 };
//double get_center_error(void)
//{
//    int16 center_error[ERROR_COUNT] = {0};
//    double error = 0.0;
//    for(uint8 i = 0; i < ERROR_COUNT; i++)
//    {
//        center_error[i] = center_line[116 - i] - 94;
//        error += (float)(center_error[i] * error_[i]);
//    }
//    error = error/440.0;
//    return error;
//}


/*
    二次多项式拟合（x = a2*y^2 + a1*y + a0）
    输入：
      center_line：长度为 count 的 uint8_t 数组，每个元素是某行的 x 坐标
      count：数组长度（一般为 IMAGE_H）
    输出：
      coeff[0] = a2, coeff[1] = a1, coeff[2] = a0
    返回：
      0 成功，-1 点数不足或矩阵奇异
*/
int polyfit_degree2_from_array(uint8_t *center_line, int count, double coeff[3]) {
    if (count < 3) return -1;  // 点数不足

    double S0 = 0, S1 = 0, S2 = 0, S3 = 0, S4 = 0;
    double T0 = 0, T1 = 0, T2 = 0;
    for (int y = 0; y < count; y++) {
        int x = center_line[y];
        S0 += 1;
        S1 += y;
        S2 += y * y;
        S3 += y * y * y;
        S4 += y * y * y * y;
        T0 += x;
        T1 += x * y;
        T2 += x * y * y;
    }
    /* 构造线性方程组
       [ S4  S3  S2 ] [a2] = [T2]
       [ S3  S2  S1 ] [a1]   [T1]
       [ S2  S1  S0 ] [a0]   [T0]
    */
    double A[3][3] = {
        {S4, S3, S2},
        {S3, S2, S1},
        {S2, S1, S0}
    };
    double B[3] = {T2, T1, T0};

    int n = 3;
    // 高斯消元法
    for (int i = 0; i < n; i++) {
        int pivot = i;
        double max_val = fabs(A[i][i]);
        for (int r = i + 1; r < n; r++) {
            double val = fabs(A[r][i]);
            if (val > max_val) {
                max_val = val;
                pivot = r;
            }
        }
        if (fabs(A[pivot][i]) < 1e-10) return -1; // 奇异矩阵
        if (pivot != i) {
            for (int j = i; j < n; j++) {
                double temp = A[i][j];
                A[i][j] = A[pivot][j];
                A[pivot][j] = temp;
            }
            double temp = B[i];
            B[i] = B[pivot];
            B[pivot] = temp;
        }
        for (int r = i + 1; r < n; r++) {
            double factor = A[r][i] / A[i][i];
            for (int j = i; j < n; j++) {
                A[r][j] -= factor * A[i][j];
            }
            B[r] -= factor * B[i];
        }
    }
    double a[3];
    for (int i = n - 1; i >= 0; i--) {
        a[i] = B[i];
        for (int j = i + 1; j < n; j++) {
            a[i] -= A[i][j] * a[j];
        }
        a[i] /= A[i][i];
    }
    coeff[0] = a[0]; // a2
    coeff[1] = a[1]; // a1
    coeff[2] = a[2]; // a0
    return 0;
}

/* 计算多项式在 y 处的值 */
double poly_value(double coeff[3], int y) {
    return coeff[0] * y * y + coeff[1] * y + coeff[2];
}

/* 计算多项式在 y 处的一阶导数，公式：2*a2*y + a1 */
double poly_derivative(double coeff[3], int y) {
    return 2 * coeff[0] * y + coeff[1];
}

/* 在前瞻行（look_ahead_y）处计算 x 值和局部斜率转角（单位：弧度） */
double compute_lookahead_angle(double coeff[3], int look_ahead_y, double *x_look) {
    *x_look = poly_value(coeff, look_ahead_y);
    double slope = poly_derivative(coeff, look_ahead_y);
    return atan(slope);
}

/*
    结合前瞻角度和图像底部偏移计算最终转向角
    参数 center_line：uint8_t 数组，长度为 IMAGE_H，表示各行中线的 x 坐标
*/
double get_steering_angle(LaneController *controller, uint8_t *center_line) {
    int count = MT9V03X_H;
    double coeff[3];
    if (polyfit_degree2_from_array(center_line, count, coeff) != 0) {
        return 0.0; // 拟合失败，返回 0 转向
    }

    // 1. 在前瞻行处计算角度
    double x_look;
    double angle_look = compute_lookahead_angle(coeff, controller->look_ahead_y, &x_look);

    // 2. 找图像底部对应的 y（取最大 y，一般为 IMAGE_H-1）
    int y_bottom = MT9V03X_H - 1;
    double x_bottom = poly_value(coeff, y_bottom);
    // 计算底部偏移：中线 x 与图像中心 (IMAGE_W/2) 的差值
    int offset = (int)x_bottom - (MT9V03X_W / 2);
    // 归一化偏移：除以 (IMAGE_W/2)，结果约在[-1,1]范围内
    double normalized_offset = (double)offset / (MT9V03X_W / 2);

    // 3. 综合前瞻角度和偏移，计算原始转向角
    double steer_raw = controller->kp_angle * angle_look + controller->kp_offset * normalized_offset;

    // 4. 使用指数平滑滤波
    double steer_out = controller->smoothing_factor * controller->last_steer +
                         (1.0 - controller->smoothing_factor) * steer_raw;
    controller->last_steer = steer_out;

    return steer_out;  // 单位：弧度
}

void Control_car(void)
{
   double base_speed = Constant_Speed;
   Control_wheel_Speed speed;
//   err_road = get_center_error();


   erspeed=center_line_error;

}



