
#include "UI.h"

uint8 Cursor_Postion = 0 ;   //初始化光标位置
uint8 UI_choose_flag = 1 ;
void UI_Show_Frist()
{
    ips200_show_string(Show_startLine,0,"Image_show");
}
void UI_Choose()   //选择一二级界面
{
    if(key1_flag == 0)
    {
        if(UI_choose_flag == 2)
        {
            ips200_clear();
        }
        UI_choose_flag = 1;
        UI_Show_Frist();

    }
    else   //二级菜单
    {
        UI_choose_flag = 2;
        if(Cursor_Postion == 0)  //显示图像
        {
            ips200_show_gray_image(Show_startLine, 0, image_copy[0], LCDW, LCDH, LCDW, LCDH, 0);
            ips200_show_gray_image(Show_startLine, 70, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
        }
    }
}
//光标显示
void Cursor_show()
{

    ips200_show_char(0,Cursor_Postion*Height_data , '>');
    ips200_show_char(7, Cursor_Postion*Height_data, '>');
    if (key3_flag == 1)
    {
       ips200_clear();
       Cursor_Postion+=1;
       key3_flag = 0 ;
    }
    if (key4_flag == 1)
    {
       if(Cursor_Postion>0)
       {
          ips200_clear();
          Cursor_Postion-=1;
       }

          key4_flag = 0 ;
     }
}

void UI_init()
{
    key_set();
    Cursor_show();
//    Key_Num_show();
    UI_Choose();
}
