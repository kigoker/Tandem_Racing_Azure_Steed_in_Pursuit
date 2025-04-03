///*
// * Control.c
// *
// *  Created on: 2025��2��22��
// *      Author: zhuji
// */
//#include "Control.h"
//
//#define constrain(value, min_val, max_val) ((value) < (min_val) ? (min_val) : ((value) > (max_val) ? (max_val) : (value)))
////
////int Speed_init_L = Constant_Speed;
////int Speed_init_R = Constant_Speed;
//int flag=0;
//int flag1=0;
//int flag_t=0;
//float ratio ;
//typedef struct{
//        int left;
//        int right;
//} Control_wheel_Speed;
//
////uint8 error1_[ERROR_COUNT];
//uint8 quhang;
//// ����ͷ���ص�����λ������
//
//double calculate_steering_angle(float dt) {
//    static float integral = 0;
//        static float prev_error = 0;
//
//        // 1. ������ ROI ����
//        float total_error = 0;
//        int valid_rows = 0;
////        for (int row = ROI_START_ROW; row < IMAGE_HEIGHT; row++) {
//////            float current_error = center_line[row] - TARGET;
////            total_error += current_error;
////            valid_rows++;
////        }
//        float error = total_error / valid_rows;
//
//        // 2. �����ԷŴ�С���
//        if (fabs(error) < 15.0f) {
//            error *= 1.5;
//        }
//
//        // 3. ���������֣���������ʱ�����ۻ���
//        if (fabs(error) < 20.0f) {
//            integral += error * dt;
//        } else {
//            integral = 0;
//        }
//        // �����޷�
//        integral = constrain(integral, -MAX_INTEGRAL, MAX_INTEGRAL);
//
//        // 4. ����΢����
//        float derivative = (error - prev_error) / dt;
//
//        // 5. PID ����
//        double steer_angle = KP * error + KI * integral + KD * derivative;
//        prev_error = error;
//
//        // 6. ���ת�����ƺ���Χ��
//        steer_angle = constrain(steer_angle, -30.0f, 30.0f);
//
//    return steer_angle;
//}
//
////Control_wheel_Speed steer_with_angle(double target_angle, double target_speed) {
////
//////    printf("%d\n",(int)target_angle);
////    Control_wheel_Speed Speed_wheel;
////
////    // ������飨��ֹ���㣩
////        const double WHEEL_BASE = 15.0;         // ��ࣨ��λ��cm����У׼��
////        const double WHEEL_DISTANCE = 20.0;     // �־ࣨ�ּ�࣬��λ��cm����У׼��
////
////    // ����Ŀ����ٶ�
////    double omega = target_angle / 0.2;  // ����ת��ʱ��Ϊ 1 ��
////
////    double  K_P = 0.2 ;
////
////    // �����������ٶ�
////    Speed_wheel.left = target_speed + (WHEEL_DISTANCE / (2 * WHEEL_BASE)) * omega *K_P;
////    Speed_wheel.right  = target_speed - (WHEEL_DISTANCE / (2 * WHEEL_BASE)) * omega *K_P;
////
////    // ���õ���ٶ�
////    return Speed_wheel;
////}
//
//
////
////uint8 error_[ERROR_COUNT] = {        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��10 ����19 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��20 ����29 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 2, 4, 6, 8,               //ͼ����Զ��30 ����39 ��Ȩ��
////        10,12,14,16,18,18,16,14,12,10,              //ͼ����Զ��40 ����49 ��Ȩ��
////        8, 6, 4, 2, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��50 ����59 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��60 ����69 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
////        };
////uint8_t error_[ERROR_COUNT] = {
////    64, 61, 58, 55, 52, 50, 47, 45,
////    43, 41, 39, 37, 35, 33, 32, 30,
////    28, 27, 26, 24, 23, 22, 21, 19,
////    18, 17, 16, 15, 15, 14, 13, 12,
////    18, 17, 16, 15, 15, 14, 13, 12,
////    18, 17, 16, 15, 15, 14, 13, 12,
////    7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 ,
////    4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 ,
////    1, 1, 1, 1, 1, 1, 1, 1,
////    1, 1, 1, 1, 1, 1, 1, 1
////};
//
//// Ԥ����õ�ָ��Ȩ�ر�k = 0.05�����ȿɵ�����
//const float weight_table[80] = {
//    1.0000f, 0.9512f, 0.9048f, 0.8607f, 0.8187f, 0.7788f, 0.7408f, 0.7047f,
//    0.6703f, 0.6376f, 0.6065f, 0.5769f, 0.5488f, 0.5220f, 0.4966f, 0.4724f,
//    0.4493f, 0.4274f, 0.4066f, 0.3867f, 0.3679f, 0.3501f, 0.3332f, 0.3173f,
//    0.3023f, 0.2882f, 0.2749f, 0.2624f, 0.2504f, 0.2392f, 0.2286f, 0.2187f,
//    0.2094f, 0.2005f, 0.1923f, 0.1845f, 0.1770f, 0.1700f, 0.1633f, 0.1570f,
//    0.1510f, 0.1454f, 0.1400f, 0.1350f, 0.1302f, 0.1257f, 0.1214f, 0.1173f,
//    0.1134f, 0.1098f, 0.1063f, 0.1030f, 0.0998f, 0.0968f, 0.0940f, 0.0912f,
//    0.0886f, 0.0861f, 0.0838f, 0.0815f, 0.0794f, 0.0774f, 0.0755f, 0.0736f,
//    0.0719f, 0.0702f, 0.0686f, 0.0671f, 0.0656f, 0.0642f, 0.0628f, 0.0615f,
//    0.0603f, 0.0591f, 0.0579f, 0.0568f, 0.0557f, 0.0547f, 0.0537f, 0.0527f
//};
//// �����Ȩ�����ʽ��
//float weighted_error_exp(int8 *errors) {
//    float weighted_sum = 0.0f;
//    float weight_total = 0.0f;
//    for(uint8 i = quhang; i < 80; i++){
//        errors[i] = center_line[119 - i] - 93;
//    }
//    for (int i = quhang; i < 80; i++) {
//        weighted_sum += weight_table[i] * (float)errors[i];
//        weight_total += weight_table[i];
//    }
//    return weighted_sum / weight_total;
//}
//////��Ȩ����
////const uint8 Weight00[MT9V03X_H]=
////{
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��10 ����19 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��10 ����19 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��10 ����19 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 3, 4, 5,              //ͼ����Զ��20 ����29 ��Ȩ��
////        6, 7, 9,11,13,15,17,19,20,20,              //ͼ����Զ��30 ����39 ��Ȩ��
////       19,17,15,13,11, 9, 7, 5, 3, 1,              //ͼ����Զ��40 ����49 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��50 ����59 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��60 ����69 ��Ȩ��
////};
////const uint8 Weight01[MT9V03X_H]=
////{
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��10 ����19 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��10 ����19 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��20 ����29 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 2, 4, 6, 8,               //ͼ����Զ��30 ����39 ��Ȩ��
////        10,12,14,16,18,18,16,14,12,10,              //ͼ����Զ��40 ����49 ��Ȩ��
////        8, 6, 4, 2, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��50 ����59 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��60 ����69 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
////};
////const uint8 Weight02[MT9V03X_H]=
////{
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��20 ����29 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��30 ����39 ��Ȩ��
////        1, 2, 4, 6, 8, 10,12,14,16,18,              //ͼ����Զ��40 ����49 ��Ȩ��
////        18,16,14,12,10,8, 6, 4, 2, 1,              //ͼ����Զ��50 ����59 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��60 ����69 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
////};
////const uint8 Weight03[MT9V03X_H]=
////{
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��10 ����19 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��20 ����29 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 2, 4, 6, 8,               //ͼ����Զ��30 ����39 ��Ȩ��
////        10,12,14,16,18,18,16,14,12,10,              //ͼ����Զ��40 ����49 ��Ȩ��
////        8, 6, 4, 2, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��50 ����59 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��60 ����69 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
////};
////const uint8 Weight04[MT9V03X_H]=
////{
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��20 ����29 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��30 ����39 ��Ȩ��
////        1, 2, 4, 6, 8, 10,12,14,16,18,              //ͼ����Զ��40 ����49 ��Ȩ��
////        18,16,14,12,10,8, 6, 4, 2, 1,              //ͼ����Զ��50 ����59 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��60 ����69 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
////};
////
////const uint8 huandao_Weight[MT9V03X_H]=
////{
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��10 ����19 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��10 ����19 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��10 ����19 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 3, 4, 5,              //ͼ����Զ��20 ����29 ��Ȩ��
////        6, 7, 9,11,13,15,17,19,20,20,              //ͼ����Զ��30 ����39 ��Ȩ��
////       19,17,15,13,11, 9, 7, 5, 3, 1,              //ͼ����Զ��40 ����49 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��50 ����59 ��Ȩ��
////        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��60 ����69 ��Ȩ��
////};
//
////uint8 error_[ERROR_COUNT] = {11, 11, 11, 11, 11, 11, 11, 11,
////                            10 , 10 , 10 , 10 , 10 , 10 , 10 ,10 ,
////                            9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 ,
////                            8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 ,
////                            7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 ,
////                            6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 ,
////                            4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 ,
////                            3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 ,
////                            2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
////                            1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 };
////uint8 error_[ERROR_COUNT] = {15, 15, 15, 15, 14, 14,14, 14,
////                            13 , 13 , 13 , 13 , 12 , 12 , 12 , 12,
////                            11 , 11 , 11 , 11 , 10 , 10 , 10 , 10 ,
////                            9 , 9 , 9 , 9 , 8 , 8 , 8 , 8 ,
////                            7 , 7 , 7 , 7 , 6 , 6 , 6 , 6 ,
////                            13 , 13 , 13 , 13 , 12 , 12 , 12 , 12,
////                            11 , 11 , 11 , 11 , 10 , 10 , 10 , 10 ,
////                            3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 ,
////                            2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
////                            1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 };
////double get_center_error(void)
////{
////    int16 center_error[ERROR_COUNT] = {0};
////    double error = 0.0;
////    for(uint8 i = 0; i < ERROR_COUNT; i++)
////    {
////        center_error[i] = center_line[116 - i] - 94;
////        error += (float)(center_error[i] * error_[i]);
////    }
////    error = error/440.0;
////    return error;
////}
//
//
///*
//    ���ζ���ʽ��ϣ�x = a2*y^2 + a1*y + a0��
//    ���룺
//      center_line������Ϊ count �� uint8_t ���飬ÿ��Ԫ����ĳ�е� x ����
//      count�����鳤�ȣ�һ��Ϊ IMAGE_H��
//    �����
//      coeff[0] = a2, coeff[1] = a1, coeff[2] = a0
//    ���أ�
//      0 �ɹ���-1 ����������������
//*/
////int polyfit_degree2_from_array(uint8_t *center_line, int count, double coeff[3]) {
////    if (count < 3) return -1;  // ��������
////
////    double S0 = 0, S1 = 0, S2 = 0, S3 = 0, S4 = 0;
////    double T0 = 0, T1 = 0, T2 = 0;
////    for (int y = 0; y < count; y++) {
////        int x = center_line[y];
////        S0 += 1;
////        S1 += y;
////        S2 += y * y;
////        S3 += y * y * y;
////        S4 += y * y * y * y;
////        T0 += x;
////        T1 += x * y;
////        T2 += x * y * y;
////    }
////    /* �������Է�����
////       [ S4  S3  S2 ] [a2] = [T2]
////       [ S3  S2  S1 ] [a1]   [T1]
////       [ S2  S1  S0 ] [a0]   [T0]
////    */
////    double A[3][3] = {
////        {S4, S3, S2},
////        {S3, S2, S1},
////        {S2, S1, S0}
////    };
////    double B[3] = {T2, T1, T0};
////
////    int n = 3;
////    // ��˹��Ԫ��
////    for (int i = 0; i < n; i++) {
////        int pivot = i;
////        double max_val = fabs(A[i][i]);
////        for (int r = i + 1; r < n; r++) {
////            double val = fabs(A[r][i]);
////            if (val > max_val) {
////                max_val = val;
////                pivot = r;
////            }
////        }
////        if (fabs(A[pivot][i]) < 1e-10) return -1; // �������
////        if (pivot != i) {
////            for (int j = i; j < n; j++) {
////                double temp = A[i][j];
////                A[i][j] = A[pivot][j];
////                A[pivot][j] = temp;
////            }
////            double temp = B[i];
////            B[i] = B[pivot];
////            B[pivot] = temp;
////        }
////        for (int r = i + 1; r < n; r++) {
////            double factor = A[r][i] / A[i][i];
////            for (int j = i; j < n; j++) {
////                A[r][j] -= factor * A[i][j];
////            }
////            B[r] -= factor * B[i];
////        }
////    }
////    double a[3];
////    for (int i = n - 1; i >= 0; i--) {
////        a[i] = B[i];
////        for (int j = i + 1; j < n; j++) {
////            a[i] -= A[i][j] * a[j];
////        }
////        a[i] /= A[i][i];
////    }
////    coeff[0] = a[0]; // a2
////    coeff[1] = a[1]; // a1
////    coeff[2] = a[2]; // a0
////    return 0;
////}
////
/////* �������ʽ�� y ����ֵ */
////double poly_value(double coeff[3], int y) {
////    return coeff[0] * y * y + coeff[1] * y + coeff[2];
////}
////
/////* �������ʽ�� y ����һ�׵�������ʽ��2*a2*y + a1 */
////double poly_derivative(double coeff[3], int y) {
////    return 2 * coeff[0] * y + coeff[1];
////}
////
/////* ��ǰհ�У�look_ahead_y�������� x ֵ�;ֲ�б��ת�ǣ���λ�����ȣ� */
////double compute_lookahead_angle(double coeff[3], int look_ahead_y, double *x_look) {
////    *x_look = poly_value(coeff, look_ahead_y);
////    double slope = poly_derivative(coeff, look_ahead_y);
////    return atan(slope);
////}
////
/////*
////    ���ǰհ�ǶȺ�ͼ��ײ�ƫ�Ƽ�������ת���
////    ���� center_line��uint8_t ���飬����Ϊ IMAGE_H����ʾ�������ߵ� x ����
////*/
////double get_steering_angle(LaneController *controller, uint8_t *center_line) {
////    int count = MT9V03X_H;
////    double coeff[3];
////    if (polyfit_degree2_from_array(center_line, count, coeff) != 0) {
////        return 0.0; // ���ʧ�ܣ����� 0 ת��
////    }
////
////    // 1. ��ǰհ�д�����Ƕ�
////    double x_look;
////    double angle_look = compute_lookahead_angle(coeff, controller->look_ahead_y, &x_look);
////
////    // 2. ��ͼ��ײ���Ӧ�� y��ȡ��� y��һ��Ϊ IMAGE_H-1��
////    int y_bottom = MT9V03X_H - 1;
////    double x_bottom = poly_value(coeff, y_bottom);
////    // ����ײ�ƫ�ƣ����� x ��ͼ������ (IMAGE_W/2) �Ĳ�ֵ
////    int offset = (int)x_bottom - (MT9V03X_W / 2);
////    // ��һ��ƫ�ƣ����� (IMAGE_W/2)�����Լ��[-1,1]��Χ��
////    double normalized_offset = (double)offset / (MT9V03X_W / 2);
////
////    // 3. �ۺ�ǰհ�ǶȺ�ƫ�ƣ�����ԭʼת���
////    double steer_raw = controller->kp_angle * angle_look + controller->kp_offset * normalized_offset;
////
////    // 4. ʹ��ָ��ƽ���˲�
////    double steer_out = controller->smoothing_factor * controller->last_steer +
////                         (1.0 - controller->smoothing_factor) * steer_raw;
////    controller->last_steer = steer_out;
////
////    return steer_out;  // ��λ������
////}
//
//
//void Control_car(void)
//{
////   RoadType type = recognize_road_type();
//
//   double base_speed = Constant_Speed;
//   Control_wheel_Speed speed;
////   err_road = get_center_error();
//   if(curvature<0.010)
//   {quhang=35;}
//   else if(curvature>0.010 && curvature<0.020)
//   {quhang=48;}
//   else if(curvature>0.020 && curvature<0.030)
//   {quhang=50;}
////   else if(curvature>0.030)
////   {quhang=45;}
//   else
//       quhang=55;
//   err_road=weighted_error_exp(error1_);
//   steer = err_road;
//   steer1 = fabsf(err_road);
//
//   if(steer1>25)
//       steer1=25;
//   ratio = steer1 / MAX_DEVIATION;  // ���������0.0~1.0��
//
//
//
////     imu.KP_2 = 9;
//
//   setspeed1=MAX_SPEED - (MAX_SPEED - MIN_SPEED) * ratio;
//   setspeed2=MAX_SPEED - (MAX_SPEED - MIN_SPEED) * ratio;
//
//   if((steer>=-15&steer<=-10)||(steer<=15&&steer>=8))
//             erspeed=12*steer;//7.3
//
////              else
//   else if(steer<8&&steer>-10)
//              erspeed=2*steer;
//
//   else if((steer>=-15&steer<=-30)||(steer<=30&&steer>=15))
//    erspeed=16.0*steer;//7.3
//
//
//   else if((steer>=-30&steer<=-40)||(steer<=40&&steer>=30))
//                erspeed=15.2*steer;//7.3
//
//              else
//                  erspeed=13.5*steer;
//}
//
//
//
