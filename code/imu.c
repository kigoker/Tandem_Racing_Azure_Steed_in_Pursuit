/*
 * imu.c
 *
 *  Created on: 2025年3月6日
 *      Author: 赵贺嘉
 */
#include "imu.h"

float X, Y, Z;                  // 陀螺仪零漂校准值
float acc_x, acc_y, acc_z;      // 加速度计滤波值 (单位: g)
float gyro_x, gyro_y, gyro_z;   // 陀螺仪角速度 (单位: rad/s)
float Q0 = 1, Q1 = 0, Q2 = 0, Q3 = 0; // 四元数，初始化为 1 0 0 0
float I_ex, I_ey, I_ez;         // 误差积分
float icm_kp = 0.17;            // 加速度计收敛速率比例增益
float icm_ki = 0.004;           // 陀螺仪收敛速率积分增益
float pitch, roll, yaw;         // 欧拉角 (单位: 度)
float ang_z = 0;                // Z 轴角速度积分角度

uint8 GyroOn = 0;               // 是否解算标志
uint8 GyroINT = 1;              // 中断触发标志

// IMU660RA 原始数据 (假设加速度和陀螺仪数据都从 IMU660RA 获取)
extern int16 imu660ra_acc_x, imu660ra_acc_y, imu660ra_acc_z;  // 加速度计原始数据
extern int16 imu660ra_gyro_x, imu660ra_gyro_y, imu660ra_gyro_z; // 陀螺仪原始数据

void imu660_Read(void)
{
    float alpha = 0.3;  // 一阶低通滤波系数

    // 获取 IMU660RA 数据（假设已定义 imu660ra_get_acc 函数）
    imu660ra_get_gyro();  // 获取陀螺仪数据
    // imu660ra_get_acc();  // 获取加速度数据（需自行实现）

    // 一阶低通滤波，单位 g (假设量程 ±8g，灵敏度 4096 LSB/g)
    acc_x = (((float)imu660ra_acc_x) * alpha) / 4096 + acc_x * (1 - alpha);
    acc_y = (((float)imu660ra_acc_y) * alpha) / 4096 + acc_y * (1 - alpha);
    acc_z = (((float)imu660ra_acc_z) * alpha) / 4096 + acc_z * (1 - alpha);

    // 陀螺仪角速度转换为弧度制 (量程 ±2000°/s，灵敏度 16.4 LSB/(°/s))
    gyro_x = ((float)imu660ra_gyro_x - X) * PI / 180 / 16.4f;
    gyro_y = ((float)imu660ra_gyro_y - Y) * PI / 180 / 16.4f;
    gyro_z = ((float)imu660ra_gyro_z - Z) * PI / 180 / 16.4f;
//    imu660ra_gyro_transition(imu660ra_gyro_x) ;
//    imu660ra_gyro_transition(imu660ra_gyro_y) ;
//    imu660ra_gyro_transition(imu660ra_gyro_z) ;
//    imu660ra_gyro_x=imu660ra_gyro_x-1;
//    imu660ra_gyro_y=imu660ra_gyro_y-4;
//    imu660ra_gyro_z=imu660ra_gyro_z+1;
//    imu660ra_acc_x=imu660ra_acc_x+43.2;
//    imu660ra_acc_y=imu660ra_acc_y+125.5;
//    imu660ra_acc_z=imu660ra_acc_z+4100.5;
//    ips200_show_float(0,90,imu660ra_gyro_z,3,2);
//ips200_show_int(0, 190, imu660ra_gyro_x, 3);
//ips200_show_int(7, 210, imu660ra_gyro_y, 3);
//ips200_show_int(14, 230, imu660ra_gyro_z, 3);
//ips200_show_int(0, 250, imu660ra_acc_x, 3);
//ips200_show_int(7, 270, imu660ra_acc_y, 3);
//ips200_show_int(14, 290, imu660ra_acc_z, 3);
//   put_int32(3, imu660ra_gyro_x);
//   put_int32(4, imu660ra_gyro_y);
//    put_int32(5, imu660ra_gyro_z);
//   put_int32(6, imu660ra_acc_x);
//   put_int32(7, imu660ra_acc_y);
//   put_int32(8, imu660ra_acc_z);
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     初始化 IMU660RA 陀螺仪零漂
// 参数说明     void
// 返回参数     void
// 使用示例     gyroOffsetInit();  // 调用后 X, Y, Z 将存储陀螺仪零漂值
// 备注信息     通过 100 次采样平均计算零漂
//-------------------------------------------------------------------------------------------------------------------
void gyroOffsetInit(void)
{
    uint8 i;
    X = -1;
    Y = -2;
    Z = -1;
    for (i = 0; i < 100; ++i)
    {
        imu660ra_get_gyro(); // 获取陀螺仪数据
        X += imu660ra_gyro_x;
        Y += imu660ra_gyro_y;
        Z += imu660ra_gyro_z;
        system_delay_ms(5);  // 5ms 采样间隔，频率约 200Hz
    }

    X /= 100;  // 计算平均零漂
    Y /= 100;
    Z /= 100;
}
float myRsqrt(float num)
{
    float halfx = 0.5f * num;
    float y = num;
    long i = *(long*)&y;
    i = 0x5f375a86 - (i >> 1);

    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));

    return y;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     更新 IMU660RA 四元数 (AHRS 算法)
// 参数说明     void
// 返回参数     void
// 使用示例     ImuAHRSupdate();  // 更新 Q0-Q3
// 备注信息     需定义全局变量 delta_T (采样周期，单位: 秒)
//-------------------------------------------------------------------------------------------------------------------
void ImuAHRSupdate(void)
{
    float halfT = 0.5f * delta_T;  // 采样周期一半
    float vx, vy, vz;              // 重力分量
    float ex, ey, ez;              // 误差向量
    float q0 = Q0, q1 = Q1, q2 = Q2, q3 = Q3; // 当前四元数
    float q0q0 = q0 * q0, q0q1 = q0 * q1, q0q2 = q0 * q2, q0q3 = q0 * q3;
    float q1q1 = q1 * q1, q1q2 = q1 * q2, q1q3 = q1 * q3;
    float q2q2 = q2 * q2, q2q3 = q2 * q3, q3q3 = q3 * q3;
    float norm;

    // 加速度归一化
    norm = myRsqrt(acc_x * acc_x + acc_y * acc_y + acc_z * acc_z);
    acc_x = acc_x * norm;
    acc_y = acc_y * norm;
    acc_z = acc_z * norm;

    // 重力分量（载体坐标系）
    vx = 2 * (q1q3 - q0q2);
    vy = 2 * (q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3;

    // 叉乘误差
    ex = acc_y * vz - acc_z * vy;
    ey = acc_z * vx - acc_x * vz;
    ez = acc_x * vy - acc_y * vx;

    // 误差积分
    I_ex += halfT * ex;
    I_ey += halfT * ey;
    I_ez += halfT * ez;

    // PI 控制器校正陀螺仪
    gyro_x = gyro_x + icm_kp * ex + icm_ki * I_ex;
    gyro_y = gyro_y + icm_kp * ey + icm_ki * I_ey;
    gyro_z = gyro_z + icm_kp * ez + icm_ki * I_ez;

    // 一阶龙格库塔更新四元数
    q0 = q0 + (-q1 * gyro_x - q2 * gyro_y - q3 * gyro_z) * halfT;
    q1 = q1 + (q0 * gyro_x + q2 * gyro_z - q3 * gyro_y) * halfT;
    q2 = q2 + (q0 * gyro_y - q1 * gyro_z + q3 * gyro_x) * halfT;
    q3 = q3 + (q0 * gyro_z + q1 * gyro_y - q2 * gyro_x) * halfT;

    // 四元数归一化
    norm = myRsqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    Q0 = q0 * norm;
    Q1 = q1 * norm;
    Q2 = q2 * norm;
    Q3 = q3 * norm;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取 IMU660RA 陀螺仪数据并转换为物理单位 (°/s)
// 参数说明     void
// 返回参数     void
// 使用示例     get_gyro();  // 更新 ang_z
// 备注信息     对 Z轴数据限幅 (±2500 LSB)，并转换为 °/s
//-------------------------------------------------------------------------------------------------------------------
void get_gyro(void)
{
    imu660ra_get_gyro();  // 获取陀螺仪数据
//    imu660ra_gyro_x=imu660ra_gyro_x;
    // 限幅 (±2500 LSB，具体限幅值需根据量程调整)
//    if (imu660ra_gyro_z > 2500) imu660ra_gyro_z = 2500;
//    else if (imu660ra_gyro_z < -2500) imu660ra_gyro_z = -2500;
//
//    // 转换为物理单位 (°/s)
//    ang_z = imu660ra_gyro_transition(imu660ra_gyro_z);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     从四元数计算欧拉角
// 参数说明     void
// 返回参数     void
// 使用示例     ReadGyro();  // 更新 pitch, roll, yaw
// 备注信息     输出单位为度
//-------------------------------------------------------------------------------------------------------------------
void ReadGyro(void)
{
    pitch = asin(2 * Q0 * Q2 - 2 * Q1 * Q3) * 180 / PI;                   // 俯仰角
    roll = atan2(2 * Q2 * Q3 + 2 * Q0 * Q1, -2 * Q1 * Q1 - 2 * Q2 * Q2 + 1) * 180 / PI; // 横滚角
    yaw = atan2(2 * Q1 * Q2 + 2 * Q0 * Q3, -2 * Q2 * Q2 - 2 * Q3 * Q3 + 1) * 180 / PI;  // 偏航角
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     重置四元数和误差积分
// 参数说明     void
// 返回参数     void
// 使用示例     ClearGyro();
// 备注信息     将四元数和误差积分清零
//-------------------------------------------------------------------------------------------------------------------
void ClearGyro(void)
{
    Q0 = 1;
    Q1 = 0;
    Q2 = 0;
    Q3 = 0;
    I_ex = 0;
    I_ey = 0;
    I_ez = 0;
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IMU660RA 四元数姿态解算
// 参数说明     void
// 返回参数     void
// 使用示例     GyroResolve();  // 在中断或主循环中调用
// 备注信息     依赖 GyroINT 触发解算
//-------------------------------------------------------------------------------------------------------------------
void GyroResolve(void)
{
    if (GyroINT)
    {
        get_gyro();

        if (GyroOn)
        {
            imu660_Read();
            ImuAHRSupdate();
            ReadGyro();
        }
        GyroINT = 0;
    }
}

