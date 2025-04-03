#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

// 图像备份数组，在发送前将图像备份再进行发送，这样可以避免图像出现撕裂的问题
uint8 image_copy[MT9V03X_H][MT9V03X_W];

uint8 data_buffer[32];
uint8 data_len;
int Threshold1=0;
double steer =0 ;
double steer1 =0 ;
int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等
    GANJIANG_Init();
    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {
        //system_start ();
        GANJIANG_Flyer();
        if(mt9v03x_finish_flag)
        {
            mt9v03x_finish_flag = 0;
            image_process();
            // 在发送前将图像备份再进行发送，这样可以避免图像出现撕裂的问题
            memcpy(image_copy[0], Pixle[0], USE_IMAGE_SIZE);
            // 发送图像
            if(WIFI_SPI_OPEN)
            {
               seekfree_assistant_camera_send();
            }
        }
        // 此处编写需要循环执行的代码

//        int time_date = system_getval();
//        printf("%d\n",time_date);
    }
}

#pragma section all restore

// **************************** 代码区域 ****************************
