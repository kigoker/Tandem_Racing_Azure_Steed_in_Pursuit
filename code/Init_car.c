/*
 * Init_car.c
 *
 *  Created on: 2025年1月11日
 *      Author: zhuji
 */
#include "Init_car.h"
#include "PID.h"
//决策速度
int Speed_Setleft = 2000;
int Speed_SetRight = 2000;


//设置速度 决策后使用
int *Set_left = &Speed_Setleft;
int *Set_Right = &Speed_SetRight;

void infrared_Init(void)   //红外灯初始化
{
    gpio_init(P10_3, GPO, 1, GPO_PUSH_PULL);
}



void set_wheel_Speed(int *Set_left,int *Set_Right, int left_speed, int right_speed, int16 Speed_Left_Real, int16 Speed_Right_Real)
{
//    *Set_left = PID_L(left_speed,Speed_Left_Real);
//    *Set_Right = PID_R(right_speed,Speed_Right_Real);
    Motor_Left(*Set_left);
    Motor_Right(*Set_Right);
}


void GANJIANG_Init(void)
{
   Motor_Init();
   //初始化WIFI_SPI之前，初始化摄像头
   mt9v03x_init();

   imu660ra_init();
   Zero_Point_Detect();
#if(1 == WIFI_SPI_OPEN )
   WIFI_SPI_Init();
#endif
   //无线串口初始化
   wireless_uart_init();
   //红外灯初始化
   infrared_Init();
   //编码器初始化
   Encoder_Init();
   //屏幕初始化
   ips200_init(IPS200_TYPE_SPI);

   //ADC_init 初始化A11引脚
   adc_init(ADC0_CH11_A11, ADC_12BIT);

   //定时器初始化
   //定时器 0
   //pit_ms_init(CCU60_CH0, 5);

   //定时器 1  编码器Count清零
   pit_ms_init(CCU60_CH1, 10);
   wireless_uart_init();
   wireless_uart_send_string("SEEKFREE wireless uart demo.\r\n");
}

void GANJIANG_Flyer(void)
{
    //Motor_Left(Speed_Setleft);
    //Motor_Right(Speed_SetRight);
//    PID_Init(&sptr1);
//    PID_Init(&sptr2);
//    //PID初始化
//    //增量式PID
//    imu_PID_Init(&imu);
//    Control_car();
//    Motor_Left(2000);
//    Motor_Right(2000);

//    ips200_displayimage03x(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
    //Wireless_uart_readAndsent();
}



void Wireless_uart_readAndsent(void)
{
    data_len = (uint8)wireless_uart_read_buffer(data_buffer, 32);             // 查看是否有消息 默认缓冲区是 WIRELESS_UART_BUFFER_SIZE 总共 64 字节
            if(data_len != 0)                                                       // 收到了消息 读取函数会返回实际读取到的数据个数
            {
                wireless_uart_send_buffer(data_buffer, data_len);                     // 将收到的消息发送回去
                memset(data_buffer, 0, 32);
                func_uint_to_str((char *)data_buffer, data_len);
                wireless_uart_send_string("\r\ndata len:");                                 // 显示实际收到的数据信息
                wireless_uart_send_buffer(data_buffer, strlen((const char *)data_buffer));    // 显示收到的数据个数
                wireless_uart_send_string(".\r\n");
            }
}
void WIFI_SPI_Init(void)
{
    while(wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST))
        {
            printf("\r\n connect wifi failed. \r\n");
            system_delay_ms(100);                                                   // 初始化失败 等待 100ms
        }

        printf("\r\n module version:%s",wifi_spi_version);                          // 模块固件版本
        printf("\r\n module mac    :%s",wifi_spi_mac_addr);                         // 模块 MAC 信息
        printf("\r\n module ip     :%s",wifi_spi_ip_addr_port);                     // 模块 IP 地址

        // zf_device_wifi_spi.h 文件内的宏定义可以更改模块连接(建立) WIFI 之后，是否自动连接 TCP 服务器、创建 UDP 连接
        if(1 != WIFI_SPI_AUTO_CONNECT)                                              // 如果没有开启自动连接 就需要手动连接目标 IP
        {
            while(wifi_spi_socket_connect(                                          // 向指定目标 IP 的端口建立 TCP 连接
                "TCP",                                                              // 指定使用TCP方式通讯
                WIFI_SPI_TARGET_IP,                                                 // 指定远端的IP地址，填写上位机的IP地址
                WIFI_SPI_TARGET_PORT,                                               // 指定远端的端口号，填写上位机的端口号，通常上位机默认是8080
                WIFI_SPI_LOCAL_PORT))                                               // 指定本机的端口号
            {
                // 如果一直建立失败 考虑一下是不是没有接硬件复位
                printf("\r\n Connect TCP Servers error, try again.");
                system_delay_ms(100);                                               // 建立连接失败 等待 100ms
            }
        }
   // 推荐先初始化摄像头，后初始化逐飞助手
#if(0 == INCLUDE_BOUNDARY_TYPE)

   // 逐飞助手初始化 数据传输使用高速WIFI SPI
   seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
    // 发送总钻风图像信息(仅包含原始图像信息)
    seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, image_copy[0], LCDW, LCDH);
#elif(1 == INCLUDE_BOUNDARY_TYPE)
    // 发送总钻风图像信息(并且包含三条边界信息，边界信息含有横纵轴坐标)
//    // 这样的方式可以实现对于有回弯的边界显示
//    ImageDealDatatypedef ImageDeal[60];             //记录单行信息的结构体数组

    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
    seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, image_copy[0], LCDW, LCDH);
    seekfree_assistant_camera_boundary_config(X_BOUNDARY, LCDH,LeftBorder_copy, RightBorder_copy, center_copy, NULL, NULL, NULL);     // 图像发送时包含三条边线，边线只有横坐标

#endif
}

