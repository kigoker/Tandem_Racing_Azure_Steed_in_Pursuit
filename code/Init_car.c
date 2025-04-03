/*
 * Init_car.c
 *
 *  Created on: 2025��1��11��
 *      Author: zhuji
 */
#include "Init_car.h"
#include "PID.h"
//�����ٶ�
int Speed_Setleft = 2000;
int Speed_SetRight = 2000;


//�����ٶ� ���ߺ�ʹ��
int *Set_left = &Speed_Setleft;
int *Set_Right = &Speed_SetRight;

void infrared_Init(void)   //����Ƴ�ʼ��
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
   //��ʼ��WIFI_SPI֮ǰ����ʼ������ͷ
   mt9v03x_init();

   imu660ra_init();
   Zero_Point_Detect();
#if(1 == WIFI_SPI_OPEN )
   WIFI_SPI_Init();
#endif
   //���ߴ��ڳ�ʼ��
   wireless_uart_init();
   //����Ƴ�ʼ��
   infrared_Init();
   //��������ʼ��
   Encoder_Init();
   //��Ļ��ʼ��
   ips200_init(IPS200_TYPE_SPI);

   //ADC_init ��ʼ��A11����
   adc_init(ADC0_CH11_A11, ADC_12BIT);

   //��ʱ����ʼ��
   //��ʱ�� 0
   //pit_ms_init(CCU60_CH0, 5);

   //��ʱ�� 1  ������Count����
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
//    //PID��ʼ��
//    //����ʽPID
//    imu_PID_Init(&imu);
//    Control_car();
//    Motor_Left(2000);
//    Motor_Right(2000);

//    ips200_displayimage03x(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
    //Wireless_uart_readAndsent();
}



void Wireless_uart_readAndsent(void)
{
    data_len = (uint8)wireless_uart_read_buffer(data_buffer, 32);             // �鿴�Ƿ�����Ϣ Ĭ�ϻ������� WIRELESS_UART_BUFFER_SIZE �ܹ� 64 �ֽ�
            if(data_len != 0)                                                       // �յ�����Ϣ ��ȡ�����᷵��ʵ�ʶ�ȡ�������ݸ���
            {
                wireless_uart_send_buffer(data_buffer, data_len);                     // ���յ�����Ϣ���ͻ�ȥ
                memset(data_buffer, 0, 32);
                func_uint_to_str((char *)data_buffer, data_len);
                wireless_uart_send_string("\r\ndata len:");                                 // ��ʾʵ���յ���������Ϣ
                wireless_uart_send_buffer(data_buffer, strlen((const char *)data_buffer));    // ��ʾ�յ������ݸ���
                wireless_uart_send_string(".\r\n");
            }
}
void WIFI_SPI_Init(void)
{
    while(wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST))
        {
            printf("\r\n connect wifi failed. \r\n");
            system_delay_ms(100);                                                   // ��ʼ��ʧ�� �ȴ� 100ms
        }

        printf("\r\n module version:%s",wifi_spi_version);                          // ģ��̼��汾
        printf("\r\n module mac    :%s",wifi_spi_mac_addr);                         // ģ�� MAC ��Ϣ
        printf("\r\n module ip     :%s",wifi_spi_ip_addr_port);                     // ģ�� IP ��ַ

        // zf_device_wifi_spi.h �ļ��ڵĺ궨����Ը���ģ������(����) WIFI ֮���Ƿ��Զ����� TCP ������������ UDP ����
        if(1 != WIFI_SPI_AUTO_CONNECT)                                              // ���û�п����Զ����� ����Ҫ�ֶ�����Ŀ�� IP
        {
            while(wifi_spi_socket_connect(                                          // ��ָ��Ŀ�� IP �Ķ˿ڽ��� TCP ����
                "TCP",                                                              // ָ��ʹ��TCP��ʽͨѶ
                WIFI_SPI_TARGET_IP,                                                 // ָ��Զ�˵�IP��ַ����д��λ����IP��ַ
                WIFI_SPI_TARGET_PORT,                                               // ָ��Զ�˵Ķ˿ںţ���д��λ���Ķ˿ںţ�ͨ����λ��Ĭ����8080
                WIFI_SPI_LOCAL_PORT))                                               // ָ�������Ķ˿ں�
            {
                // ���һֱ����ʧ�� ����һ���ǲ���û�н�Ӳ����λ
                printf("\r\n Connect TCP Servers error, try again.");
                system_delay_ms(100);                                               // ��������ʧ�� �ȴ� 100ms
            }
        }
   // �Ƽ��ȳ�ʼ������ͷ�����ʼ���������
#if(0 == INCLUDE_BOUNDARY_TYPE)

   // ������ֳ�ʼ�� ���ݴ���ʹ�ø���WIFI SPI
   seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
    // ���������ͼ����Ϣ(������ԭʼͼ����Ϣ)
    seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, image_copy[0], MT9V03X_W, MT9V03X_H);
#elif(1 == INCLUDE_BOUNDARY_TYPE)
    // ���������ͼ����Ϣ(���Ұ��������߽���Ϣ���߽���Ϣ���к���������)
    // �����ķ�ʽ����ʵ�ֶ����л���ı߽���ʾ
    ImageDealDatatypedef ImageDeal[60];             //��¼������Ϣ�Ľṹ������
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
    seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, image_copy[0], MT9V03X_W, MT9V03X_H);
    seekfree_assistant_camera_boundary_config(X_BOUNDARY, MT9V03X_H, LeftBorder, RightBorder, Center, NULL, NULL, NULL);     // ͼ����ʱ�����������ߣ�����ֻ�к�����

#endif
}

