/*
 * KEY.c
 *
 *  Created on: 2025年3月21日
 *      Author: zhuji
 */

#include "KEY.h"
void mykey_init(void)
{
    gpio_init(key1,GPI,0,GPI_FLOATING_IN);
    gpio_init(key2,GPI,0,GPI_FLOATING_IN);
    gpio_init(key3,GPI,0,GPI_PULL_UP);
    gpio_init(key4,GPI,0,GPI_PULL_UP);
    gpio_init(key5,GPI,0,GPI_PULL_UP);
    gpio_init(key6,GPI,0,GPI_PULL_UP);
    gpio_init(key7,GPI,0,GPI_PULL_UP);
}

//按键判断一些过程变量
uint8 key1_status = 1;
uint8 key1_last_status;
uint8 key1_flag;

uint8 key2_status = 1;
uint8 key2_last_status;
uint8 key2_flag;

uint8 key3_status = 1;
uint8 key3_last_status;
uint8 key3_flag;

uint8 key4_status = 1;
uint8 key4_last_status;
uint8 key4_flag;

uint8 key5_status = 1;
uint8 key5_last_status;
uint8 key5_flag;

uint8 key6_status = 1;
uint8 key6_last_status;
uint8 key6_flag;

uint8 key7_status = 1;
uint8 key7_last_status;
uint8 key7_flag;



void Key_Num_show(){
    uint8 Key_Flag[7] = {key1_flag,key2_flag,key3_flag,key4_flag,key5_flag,key6_flag,key7_flag};
    for(int i = 0;i<7;i++)
    {
        if(i<4)
        {
            ips200_show_int(i*20,120,Key_Flag[i],1);
        }
        else{
            ips200_show_int((i-4)*20,140,Key_Flag[i],1);
        }

    }

}

//按键查询
void key_set(void)
{
    //key1拨码开关查询
    key1_status = gpio_get_level(key1);
    if(key1_status == 0)
    {
        key1_flag = 0;

    }
    else
    {
        key1_flag = 1;
    }

    //key2拨码开关查询
    key2_status = gpio_get_level(key2);
    if(key2_status == 0)
    {
        key2_flag = 0;
    }
    else
    {
        key2_flag = 1;
    }

    key3_last_status = key3_status;
    key3_status = gpio_get_level(key3);
    if(key3_status && !key3_last_status)
    {
        key3_flag = 1;

    }

    key4_last_status = key4_status;
    key4_status = gpio_get_level(key4);
    if(key4_status && !key4_last_status)
    {
        key4_flag = 1;

    }


    //key3按键查询
    key5_last_status = key5_status;
    key5_status = gpio_get_level(key5);
    if(key5_status && !key5_last_status)
    {
        key5_flag = 1;

    }

    //key6按键查询
    key6_last_status = key6_status;
    key6_status = gpio_get_level(key6);
    if(key6_status && !key6_last_status)
    {
        key6_flag = 1;

    }

    //key7按键查询
    key7_last_status = key7_status;
    key7_status = gpio_get_level(key7);
    if(key7_status && !key7_last_status)
    {
        key7_flag = 1;

    }



}


