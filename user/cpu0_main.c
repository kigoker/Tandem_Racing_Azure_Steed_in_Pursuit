#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"
// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU0��RAM��

// ͼ�񱸷����飬�ڷ���ǰ��ͼ�񱸷��ٽ��з��ͣ��������Ա���ͼ�����˺�ѵ�����
uint8 image_copy[MT9V03X_H][MT9V03X_W];

uint8 data_buffer[32];
uint8 data_len;
int Threshold1=0;
double steer =0 ;
double steer1 =0 ;
int core0_main(void)
{
    clock_init();                   // ��ȡʱ��Ƶ��<��ر���>
    debug_init();                   // ��ʼ��Ĭ�ϵ��Դ���
    // �˴���д�û����� ���������ʼ�������
    GANJIANG_Init();
    // �˴���д�û����� ���������ʼ�������
    cpu_wait_event_ready();         // �ȴ����к��ĳ�ʼ�����
    while (TRUE)
    {
        //system_start ();
        GANJIANG_Flyer();
        if(mt9v03x_finish_flag)
        {
            mt9v03x_finish_flag = 0;
            image_process();
            // �ڷ���ǰ��ͼ�񱸷��ٽ��з��ͣ��������Ա���ͼ�����˺�ѵ�����
            memcpy(image_copy[0], Pixle[0], MT9V03X_IMAGE_SIZE);
            // ����ͼ��
            if(WIFI_SPI_OPEN)
            {
               seekfree_assistant_camera_send();
            }
        }
        // �˴���д��Ҫѭ��ִ�еĴ���

//        int time_date = system_getval();
//        printf("%d\n",time_date);
    }
}

#pragma section all restore

// **************************** �������� ****************************
