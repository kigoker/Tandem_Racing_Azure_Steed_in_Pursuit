/*
 * camera.c
 *
 *  Created on: 2025��3��21��
 *      Author: ydc
 */
#include "camera.h"

uint8* Image_Use[LCDH][LCDW];      //�����洢ѹ��֮��Ҷ�ͼ��Ķ�ά����
uint8 Pixle[LCDH][LCDW];          //ͼ����ʱ��������Ķ�ֵ��ͼ������
//uint8 All_Sobel_Image[LCDH][LCDW];
uint8 Threshold;                                //ͨ����򷨼�������Ķ�ֵ����ֵ
uint16 threshold1, threshold2, threshold3, block_yuzhi=60;
uint16 yuzhi1,yuzhi2,yuzhi3;
uint16 Ramp_cancel,circle_stop,block_num,duan_num;
int ImageScanInterval = 5;                        //ɨ�ߵķ�Χ
static uint8* PicTemp;                          //һ�����浥��ͼ���ָ�����
static int IntervalLow = 0, IntervalHigh = 0;   //ɨ������������ޱ���
static int Ysite = 0, Xsite = 0;                //Ysite����ͼ����У�Xsite����ͼ����С�
static int BottomBorderRight = 93,              //59�е��ұ߽�
BottomBorderLeft = 0,                           //59�е���߽�
BottomCenter = 0;                               //59�е��е�
uint8 ExtenLFlag = 0;                           //������Ƿ���Ҫ���ߵı�־����
uint8 ExtenRFlag = 0;                           //�ұ����Ƿ���Ҫ���ߵı�־����
uint8 Ring_Help_Flag = 0;                       //����������־
int Right_RingsFlag_Point1_Ysite, Right_RingsFlag_Point2_Ysite; //��Բ���жϵ�����������
int Left_RingsFlag_Point1_Ysite, Left_RingsFlag_Point2_Ysite;   //��Բ���жϵ�����������
int Point_Xsite,Point_Ysite;                   //�յ��������
int Repair_Point_Xsite,Repair_Point_Ysite;     //���ߵ��������
int Point_Ysite1;                               //�жϴ�СԲ��ʱ�õ�������
int Black;                                      //�жϴ�СԲ��ʱ�õĺڵ�����
int Less_Big_Small_Num;                         //�жϴ�СԲ��ʱ�õĶ���
int left_difference_num;                        //ʮ����������׼����39����ĺͣ�40-20�У�
int right_difference_num;                       //ʮ���ұ������׼����39����ĺͣ�40-20�У�
uint8 Garage_Location_Flag = 0;                 //�жϿ�Ĵ���
float Big_Small_Help_Gradient;               //��СԲ���ĸ����ж�б��
static int ytemp = 0;                           //����е���ʱ����
static int TFSite = 0, left_FTSite = 0,right_FTSite = 0;              //���߼���б�ʵ�ʱ����Ҫ�õĴ���еı�����
static float DetR = 0, DetL = 0;                //��Ų���б�ʵı���
ImageDealDatatypedef ImageDeal[60];             //��¼������Ϣ�Ľṹ������
ImageDealDatatypedef ImageDeal1[80];             //��¼������Ϣ�Ľṹ������
ImageStatustypedef ImageStatus;                 //ͼ����ĵ�ȫ�ֱ���
ImageFlagtypedef ImageFlag;
uint64 Gray_Value=0;
float Mh = MT9V03X_H;
float Lh = LCDH;
float Mw = MT9V03X_W;
float Lw = LCDW;

uint8 Half_Road_Wide[60] =                      //ֱ���������
{ 2,3,3,4,4,5,5,6,6,7,
  7,8,8,9,9,10,10,11,11,12,
  12,13,13,14,14,15,15,15,16,17,
  17,18,19,19,20,21,21,22,23,23,
  24,25,25,26,27,28,29,29,29,31,
  31,32,33,33,34,36,36,37,37,39,
};
uint8 Half_Bend_Wide[60] =                      //����������
{   33,33,33,33,33,33,33,33,33,33,
    33,33,32,32,31,31,30,30,29,28,
    28,27,27,26,26,25,25,24,24,23,
    23,22,22,23,23,23,24,24,24,24,
    25,25,25,26,26,26,27,27,28,28,
    28,29,29,30,30,31,31,32,32,33,
};
//
//uint16 block_num=0, outroad_num=0, circle_num, position;
/*****************ֱ���ж�******************/
float Straight_Judge(uint8 dir, uint8 start, uint8 end)     //���ؽ��С��1��Ϊֱ��
{
    int i;
    float S = 0, Sum = 0, Err = 0, k = 0;
    switch (dir)
    {
    case 1:k = (float)(ImageDeal[start].LeftBorder - ImageDeal[end].LeftBorder) / (start - end);
        for (i = 0; i < end - start; i++)
        {
            Err = (ImageDeal[start].LeftBorder + k * i - ImageDeal[i + start].LeftBorder) * (ImageDeal[start].LeftBorder + k * i - ImageDeal[i + start].LeftBorder);
            Sum += Err;
        }
        S = Sum / (end - start);
        break;
    case 2:k = (float)(ImageDeal[start].RightBorder - ImageDeal[end].RightBorder) / (start - end);
        for (i = 0; i < end - start; i++)
        {
            Err = (ImageDeal[start].RightBorder + k * i - ImageDeal[i + start].RightBorder) * (ImageDeal[start].RightBorder + k * i - ImageDeal[i + start].RightBorder);
            Sum += Err;
        }
        S = Sum / (end - start);
        break;
    }
    return S;
}

void Straight_long_judge(void)     //���ؽ��С��1��Ϊֱ��
{
    if( ImageFlag.Bend_Road ||  ImageFlag.Zebra_Flag || ImageFlag.Out_Road == 1 || ImageFlag.RoadBlock_Flag == 1
            || ImageFlag.image_element_rings == 1  || ImageFlag.image_element_rings == 2 ) return;
    if((Straight_Judge(1,10,50) < 1) && (Straight_Judge(2,10,50) < 1) && ImageStatus.OFFLine < 3 && ImageStatus.Miss_Left_lines < 2 && ImageStatus.Miss_Right_lines < 2)
    {
        ImageFlag.straight_long = 1;
        //Stop=1;
//        Statu = Straight_long;
    }
}

void Straight_long_handle(void)     //���ؽ��С��1��Ϊֱ��
{

    if(ImageFlag.straight_long)
    {
        if((Straight_Judge(1,10,50) > 1) || (Straight_Judge(2,10,50) > 1) || (ImageStatus.OFFLine >= 3)||ImageStatus.Miss_Left_lines >= 2||ImageStatus.Miss_Right_lines>=2)
        {
            ImageFlag.straight_long = 0;
        }
    }
}

//���ڱ������ֱ�����
float sum1=0,sum2=0;
void Straight_xie_judge(void)
{
    float S = 0, Sum = 0, Err = 0 , midd_k=0 ;
    int i;
    if (ImageFlag.Zebra_Flag != 0 || ImageFlag.image_element_rings != 0 || ImageFlag.Ramp == 1  )
        return;

    ImageFlag.straight_xie = 0;

    midd_k = (float)(ImageDeal[55].Center - ImageDeal[ImageStatus.OFFLine + 1].Center) / (55 - ImageStatus.OFFLine - 1);
    for (i = 0 ; i < 55 - ImageStatus.OFFLine - 1; i++)
    {
        Err = (ImageDeal[ImageStatus.OFFLine + 1].Center + midd_k * i - ImageDeal[i + ImageStatus.OFFLine + 1].Center) * (ImageDeal[ImageStatus.OFFLine + 1].Center + midd_k * i - ImageDeal[i + ImageStatus.OFFLine + 1].Center);
        Sum += Err;
    }
    S = Sum / (55 - ImageStatus.OFFLine - 1);
//    ips200_show_float(160,200,midd_k,3,2);
//    ips200_show_float(160,240,S,3,2);
    if (S < 1 && ImageStatus.OFFLine < 10 && (ImageStatus.Miss_Left_lines > 30 || ImageStatus.Miss_Right_lines > 30))
    {
        ImageFlag.straight_xie = 1;
//        Statu = Straight_xie;
    }

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Image_CompressInit
//  @brief          ԭʼ�Ҷ�ͼ��ѹ������ ѹ����ʼ��
//  @brief          ���þ��ǽ�ԭʼ�ߴ�ĻҶ�ͼ��ѹ����������Ҫ�Ĵ�С���������ǰ�ԭʼ80��170�еĻҶ�ͼ��ѹ����60��80�еĻҶ�ͼ��
//  @brief          ΪʲôҪѹ������Ϊ�Ҳ���Ҫ��ô�����Ϣ��60*80ͼ����չʾ����Ϣԭ�����Ѿ��㹻��ɱ��������ˣ���Ȼ����Ը����Լ�������޸ġ�
//  @parameter      void
//  @return         void
//  @time           2022��2��18��
//  @Author
//  Sample usage:   Image_CompressInit();
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void Image_CompressInit(void)
{
  int i, j, row, line;
  const float div_h = Mh / Lh, div_w = Mw / Lw;         //����ԭʼ��ͼ��ߴ��������Ҫ��ͼ��ߴ�ȷ����ѹ��������
  for (i = 0; i < LCDH; i++)                            //����ͼ���ÿһ�У��ӵ����е���59�С�
  {
    row = i * div_h;
    for (j = 0; j < LCDW; j++)                          //����ͼ���ÿһ�У��ӵ����е���79�С�
    {
      line = j * div_w;
      Image_Use[i][j] = &mt9v03x_image[row][line];       //mt9v03x_image����������ԭʼ�Ҷ�ͼ��Image_Use����洢������֮��Ҫ��ȥ�����ͼ�񣬵���Ȼ�ǻҶ�ͼ��Ŷ��ֻ��ѹ����һ�¶��ѡ�
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           get_Threshold  //ָ��
//  @brief          �Ż�֮��ĵĴ�򷨡���򷨾���һ���ܹ����һ��ͼ����ѵ��Ǹ��ָ���ֵ��һ���㷨��
//  @brief          ����������ǿ������ʵ�ڲ�������ֱ�������ã�ʲô�����������޸ģ�ֻҪû�й���Ӱ�죬��ô������������ֵ��һ�����Եõ�һ��Ч��������Ķ�ֵ��ͼ��
//  @parameter      image  ԭʼ�ĻҶ�ͼ������
//  @parameter      clo    ͼ��Ŀ�ͼ����У�
//  @parameter      row    ͼ��ĸߣ�ͼ����У�
//  @return         uint8
//  @time           2022��2��17��
//  @Author
//  Sample usage:   Threshold = Threshold_deal(Image_Use[0], 80, 60); �Ѵ��60��80�еĶ�άͼ������Image_Use��������������ͼ�����ֵ�����������ֵ����Threshold��
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8 get_Threshold(uint8* image[][LCDW],uint16 col, uint16 row)
{
  #define GrayScale 256
  uint16 width = col;
  uint16 height = row;
  int pixelCount[GrayScale];
  float pixelPro[GrayScale];
  int i, j, pixelSum = width * height;
  uint8 threshold = 0;
  for (i = 0; i < GrayScale; i++)                    //�Ȱ�pixelCount��pixelPro��������Ԫ��ȫ����ֵΪ0
  {
    pixelCount[i] = 0;
    pixelPro[i] = 0;
  }
  uint32 gray_sum = 0;
  /**************************************ͳ��ÿ���Ҷ�ֵ(0-255)������ͼ���г��ֵĴ���**************************************/
  for (i = 0; i < height; i += 1)                   //����ͼ���ÿһ�У��ӵ����е���59�С�
  {
    for (j = 0; j < width; j += 1)                  //����ͼ���ÿһ�У��ӵ����е���79�С�
    {
      pixelCount[*image[i][j]]++;       //����ǰ�����ص������ֵ���Ҷ�ֵ����Ϊ����������±ꡣ
      gray_sum += *image[i][j];         //���������Ҷ�ͼ��ĻҶ�ֵ�ܺ͡�
    }
  }
  /**************************************ͳ��ÿ���Ҷ�ֵ(0-255)������ͼ���г��ֵĴ���**************************************/
  /**************************************����ÿ������ֵ���Ҷ�ֵ���������Ҷ�ͼ������ռ�ı���*************************************************/
  for (i = 0; i < GrayScale; i++)
  {
      pixelPro[i] = (float)pixelCount[i] / pixelSum;
  }
  /**************************************����ÿ������ֵ���Ҷ�ֵ���������Ҷ�ͼ������ռ�ı���**************************************************/
  /**************************************��ʼ��������ͼ��ĻҶ�ֵ��0-255������һ��Ҳ�Ǵ����������һ��***************************/
  /*******************Ϊʲô˵������⣿��Ϊ��Ҳ�ǲ���⣡�������������һ����ѧ���⣬��������Ϊ��ѧ��ʽ��***************************/
  float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
  w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
  for (j = 0; j < GrayScale; j++)
  {
    w0 += pixelPro[j];                          //�����������ÿ���Ҷ�ֵ�����ص���ռ�ı���֮�ͣ����������ֵı�����
    u0tmp += j * pixelPro[j];

    w1 = 1 - w0;
    u1tmp = gray_sum / pixelSum - u0tmp;

    u0 = u0tmp / w0;                            //����ƽ���Ҷ�
    u1 = u1tmp / w1;                            //ǰ��ƽ���Ҷ�
    u = u0tmp + u1tmp;                          //ȫ��ƽ���Ҷ�
    deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
    if (deltaTmp > deltaMax)
    {
      deltaMax = deltaTmp;
      threshold = (uint8)j;
    }
    if (deltaTmp < deltaMax)
    {
      break;
    }
  }
  /**************************************��ʼ��������ͼ��ĻҶ�ֵ��0-255������һ��Ҳ�Ǵ����������һ��***************************/
  /*******************Ϊʲô˵������⣿��Ϊ��Ҳ�ǲ���⣡�������������һ����ѧ���⣬��������Ϊ��ѧ��ʽ��***************************/

  return threshold;                             //��������ô���д������������ֵ��return��ȥ��
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Get_BinaryImage
//  @brief          �Ҷ�ͼ���ֵ������
//  @brief          ����˼·���ǣ��ȵ���Get_Threshold���������õ���ֵ��Ȼ�����ԭʼ�Ҷ�ͼ���ÿһ�����ص㣬��ÿһ�����ص�ĻҶ�ֵ������ֵ�ƽϡ�
//  @brief          ������ֵ����Ͱ����Ǹ����ص��ֵ��ֵΪ1����Ϊ�׵㣩������͸�ֵΪ0����Ϊ�ڵ㣩����Ȼ����԰������ֵ��������ֻҪ���Լ����1��0˭�����˭����׾��С�
//  @brief          ������ǰ���ᵽ��60*80�������Ǿ�Ӧ��������ʲô��˼�˰ɣ��������ص��һ����80�����ص㣬һ��60�У�Ҳ����ѹ�����ÿһ��ͼ����4800�����ص㡣
//  @parameter      void
//  @return         void
//  @time           2022��2��17��
//  @Author
//  Sample usage:   Get_BinaryImage();
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void Get_BinaryImage(void)
{
//    if(ImageFlag.Out_Road == 1)
//    {
//        if(duan_num==0)
//        {
//            Threshold  = block_yuzhi ;
//        }
//        else if(duan_num==1)
//        {
//            Threshold  = threshold1 ;
//        }
//        else if(duan_num==2)
//        {
//            Threshold  = threshold2 ;
//        }
//        else if(duan_num==3)
//        {
//            Threshold  = threshold3 ;
//        }
//    }
//    else if(ImageFlag.RoadBlock_Flag == 1)
//    {
//        if(block_num==1)
//        {
//            Threshold  = yuzhi1 ;
//        }
//        else if(block_num==2)
//        {
//            Threshold  = yuzhi2 ;
//        }
//        else if(block_num==3)
//        {
//            Threshold  = yuzhi3 ;
//        }
//    }
//    else {
        Threshold = get_Threshold(Image_Use, LCDW, LCDH);      //������һ���������ã�ͨ���ú������Լ����һ��Ч���ܲ���Ķ�ֵ����ֵ��
//    }
  uint8 i, j = 0;
  for (i = 0; i < LCDH; i++)                                //������ά�����ÿһ��
  {
    for (j = 0; j < LCDW; j++)                              //������ά�����ÿһ��
    {
      if (*Image_Use[i][j] > Threshold)                      //��������ĻҶ�ֵ������ֵThreshold
          Pixle[i][j] = Image_white;                                  //��ô������ص�ͼ�Ϊ�׵�
      else                                                  //��������ĻҶ�ֵС����ֵThreshold
          Pixle[i][j] = Image_black;                                  //��ô������ص�ͼ�Ϊ�ڵ�
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Get_Border_And_SideType
//  @brief          �õ����ߺͱ��ߵ����ͣ�������������߷�Ϊ���������ͣ�T���͡�W���ͺ�H���͡��ֱ�����������ߡ��ޱ߱��ߺʹ�������ߡ�
//  @brief          ������һ����Ҫ�뿴���ҵ��߼���ǰ����Ҫ�㶮T��W��H�������͵ı��ߵ�����ʲô���ӵġ�
//  @parameter      p        ָ�򴫽��������һ��ָ�������
//  @parameter      type     ֻ����L������R���ֱ����ɨ����ߺ�ɨ�ұ��ߡ�
//  @parameter      L        ɨ����������� ��Ҳ���Ǵ���һ�п�ʼɨ��
//  @parameter      H        ɨ����������� ��Ҳ����һֱɨ����һ�С�
//  @parameter      Q        ��һ���ṹ��ָ��������Լ�����ȥ��������ṹ������ĳ�Ա��
//  @time           2022��2��20��
//  @Author
//  Sample usage:   Get_SideType_And_Border(PicTemp, 'R', IntervalLow, IntervalHigh,&JumpPoint[1]);
//  Sample usage:   ��PicTemp(PicTemp�Ǹ�ָ�룬ָ��һ������)��IntervalLow�п�ʼɨ��ɨ��IntervalHigh�У�Ȼ��ѵõ��ı������ڵ��кͱ������ͼ�¼��JumpPoint�ṹ���С�
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void Get_Border_And_SideType(uint8* p,uint8 type,int L,int H,JumpPointtypedef* Q)
{
  int i = 0;
  if (type == 'L')                              //���Type��L(Left),��ɨ������ߡ�
  {
    for (i = H; i >= L; i--)                    //��������ɨ
    {
      if (*(p + i) == Image_white && *(p + i - 1) != Image_white)   //����кڰ�����    1�ǰ� 0�Ǻ�
      {
        Q->point = i;                           //�ǾͰ�����м�¼������Ϊ�����
        Q->type = 'T';                          //���Ұ���һ�е������������䣬�������ͼ�ΪT��������������
        break;                                  //�ҵ��˾�����ѭ��������
      }
      else if (i == (L + 1))                    //Ҫ��ɨ�����û�ҵ�
      {
        if (*(p + (L + H) / 2) != 0)            //����ɨ��������м��ǰ����ص�
        {
          Q->point = (L + H) / 2;               //��ô����Ϊ��һ�е�������Ǵ�����ɨ��������е㡣
          Q->type = 'W';                        //���Ұ���һ�е����Ƿ��������䣬�������ͼ�ΪW�����ޱ��С�
          break;                                //����ѭ��������
        }
        else                                    //Ҫ��ɨ�����û�ҵ�������ɨ��������м��Ǻ����ص�
        {
          Q->point = H;                         //��ô����Ϊ��һ�е�������Ǵ�����ɨ��������������ޡ�
          Q->type = 'H';                        //����Ҳ����һ�е����Ƿ��������䣬�����������ͼ�ΪH�����������С�
          break;                                //����ѭ��������
        }
      }
    }
  }
  else if (type == 'R')                         //���Type��R(Right),��ɨ���ұ��ߡ�
  {
    for (i = L; i <= H; i++)                    //��������ɨ
    {
      if (*(p + i) == Image_white && *(p + i + 1) != Image_white)   //����кڰ�����    1�ǰ� 0�Ǻ�
      {
        Q->point = i;                           //�ǾͰ�����м�¼������Ϊ�ұ���
        Q->type = 'T';                          //���Ұ���һ�е������������䣬�������ͼ�ΪT��������������
        break;                                  //�ҵ��˾�����ѭ��������
      }
      else if (i == (H - 1))                    //Ҫ��ɨ�����û�ҵ�
      {
        if (*(p + (L + H) / 2) != 0)            //����ɨ��������м��ǰ����ص�
        {
          Q->point = (L + H) / 2;               //��ô����Ϊ��һ�е��ұ����Ǵ�����ɨ��������е㡣
          Q->type = 'W';                        //���Ұ���һ�е����Ƿ��������䣬�������ͼ�ΪW�����ޱ��С�
          break;
        }
        else                                    //Ҫ��ɨ�����û�ҵ�������ɨ��������м��Ǻ����ص�
        {
          Q->point = L;                         //��ô����Ϊ��һ�е��ұ����Ǵ�����ɨ��������������ޡ�
          Q->type = 'H';                        //����Ҳ����һ�е����Ƿ��������䣬�����������ͼ�ΪH�����������С�
          break;                                //����ѭ��������
        }
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Get_BaseLine
//  @brief          �ñ����ķ����õ�ͼ����������У�59-55�У��ı��ߺ�������Ϣ�������б��ߺ�������Ϣ��׼ȷ�ȷǳ�����Ҫ��ֱ��Ӱ�쵽����ͼ��Ĵ�������
//  @brief          Get_BaseLine����������Get_AllLine�����Ļ�����ǰ�ᣬGet_AllLine��������Get_BaseLineΪ�����ġ�������Ӧ��Ҳ�ܿ����԰ɣ�һ���еõ������ߣ�һ���еõ������ߡ�
//  @brief          Get_BaseLine������Get_AllLine��������һ��Ҳ����������Ż�֮����ѱ����㷨��
//  @parameter      void
//  @time           2022��2��21��
//  @Author
//  Sample usage:   Get_BaseLine();
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void Get_BaseLine(void)
{
    /**************************************��������ͼ������У�59�У����ұ��ߴӶ�ȷ�����ߵĹ��� ********************************************************************/
    /****************************************************Begin*****************************************************************************/

    PicTemp = Pixle[59];                                                //��PicTemp���ָ�����ָ��ͼ�������Pixle[59]
    for (Xsite = ImageSensorMid; Xsite < 93; Xsite++)                   //����39�������У��������п�ʼһ��һ�е����ұ������ұ���
    {
      if (*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite + 1) == 0)       //������������������ڵ㣬˵û�ҵ��˱��ߡ�
      {
        BottomBorderRight = Xsite;                                      //����һ�м�¼������Ϊ��һ�е��ұ���
        break;                                                          //����ѭ��
      }
      else if (Xsite == 92)                                             //����ҵ��˵�58�ж���û���ֺڵ㣬˵����һ�еı��������⡣
      {
        BottomBorderRight = 93;                                         //����������Ĵ�����ǣ�ֱ�Ӽ���ͼ�����ұߵ���һ�У���79�У�������һ�е��ұ��ߡ�
        break;                                                          //����ѭ��
      }
    }

    for (Xsite = ImageSensorMid; Xsite > 0; Xsite--)                    //����39�������У��������п�ʼһ��һ�е���������������
    {
      if (*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite - 1) == 0)       //������������������ڵ㣬˵û�ҵ��˱��ߡ�
      {
        BottomBorderLeft = Xsite;                                       //����һ�м�¼������Ϊ��һ�е������
        break;                                                          //����ѭ��
      }
      else if (Xsite == 1)                                              //����ҵ��˵�1�ж���û���ֺڵ㣬˵����һ�еı��������⡣
      {
        BottomBorderLeft = 0;                                           //����������Ĵ�����ǣ�ֱ�Ӽ���ͼ������ߵ���һ�У���0�У�������һ�е�����ߡ�
        break;                                                          //����ѭ��
      }
    }

    BottomCenter =(BottomBorderLeft + BottomBorderRight) / 2;           //�������ұ߽�������59�е�����
    ImageDeal[59].LeftBorder = BottomBorderLeft;                        //�ѵ�59�е���߽�洢�����飬ע�⿴ImageDeal������ֵ��±꣬�ǲ������ö�Ӧ59��
    ImageDeal[59].RightBorder = BottomBorderRight;                      //�ѵ�59�е��ұ߽�洢�����飬ע�⿴ImageDeal������ֵ��±꣬�ǲ������ö�Ӧ59��
    ImageDeal[59].Center = BottomCenter;                                //�ѵ�59�е����ߴ洢�����飬    ע�⿴ImageDeal������ֵ��±꣬�ǲ������ö�Ӧ59��
    ImageDeal[59].Wide = BottomBorderRight - BottomBorderLeft;          //�ѵ�59�е�������ȴ洢���飬ע�⿴ImageDeal������ֵ��±꣬�ǲ������ö�Ӧ59��
    ImageDeal[59].IsLeftFind = 'T';                                     //��¼��59�е����������ΪT���������ҵ�����ߡ�
    ImageDeal[59].IsRightFind = 'T';                                    //��¼��59�е��ұ�������ΪT���������ҵ��ұ��ߡ�

    /****************************************************End*******************************************************************************/
    /**************************************��������ͼ������У�59�У����ұ��ߴӶ�ȷ�����ߵĹ��� ********************************************************************/
    /**************************************�ڵ�59�������Ѿ�ȷ���������ȷ��58-54���������ߵĹ��� ******************************************/
    /****************************************************Begin*****************************************************************************/
    /*
         * ���漸�еĵ����߹����ҾͲ���׸���ˣ������ҵ�ע�Ͱѵ�59�е����߹������ã�
         * ��ô58��54�е����߾���ȫû���⣬��һģһ�����߼��͹��̡�
     */
    for (Ysite = 58; Ysite > 54; Ysite--)
    {
        PicTemp = Pixle[Ysite];
        for(Xsite = ImageDeal[Ysite + 1].Center; Xsite < 93; Xsite++)
        {
          if(*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite + 1) == 0)
          {
            ImageDeal[Ysite].RightBorder = Xsite;
            break;
          }
          else if (Xsite == 92)
          {
            ImageDeal[Ysite].RightBorder = 93;
            break;
          }
        }
        for (Xsite = ImageDeal[Ysite + 1].Center; Xsite > 0;Xsite--)
        {
          if (*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite - 1) == 0)
          {
            ImageDeal[Ysite].LeftBorder = Xsite;
            break;
          }
          else if (Xsite == 1)
          {
            ImageDeal[Ysite].LeftBorder = 0;
            break;
          }
        }
        ImageDeal[Ysite].IsLeftFind  = 'T';
        ImageDeal[Ysite].IsRightFind = 'T';
        ImageDeal[Ysite].Center =(ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder)/2;
        ImageDeal[Ysite].Wide   = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
    }
    /****************************************************End*****************************************************************************/
    /**************************************�ڵ�59�������Ѿ�ȷ���������ȷ��58-54���������ߵĹ��� ****************************************/
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Get_AllLine
//  @brief          ��Get_BaseLine�Ļ����ϣ���Բ����������������һЩ����Ĵ����㷨�õ�ʣ���еı��ߺ�������Ϣ��
//  @brief          �������Ӧ����ĿǰΪֹ�Ҵ���������������һ�����ˣ�һ��Ҫ������ʱ�侲������ȥ��������ʱ����������Ҫ���Ǹ���ֵ���ڰ�ͼ��
//  @brief          �������һ�����Ŷ�ֵ���ڰ�ͼ��һ����˼���Ҵ�����߼��Ļ����ܶ�ط��������������ˣ���Ҫ�ⶢ���Ǹ�����һֱ������������û�õģ��мɣ�
//  @brief          �ද��˼������������������ǿ϶�Ҳ���Եġ�������̻�ܿ�������㶼��������֮����ĳ������Ѿ�������ֱ��������ˡ�
//  @parameter      void
//  @time           2023��2��21��
//  @Author
//  Sample usage:   Get_AllLine();
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void Get_AllLine(void)
{
  uint8 L_Found_T  = 'F';    //ȷ���ޱ�б�ʵĻ�׼�б����Ƿ��ҵ��ı�־
  uint8 Get_L_line = 'F';    //�ҵ���һ֡ͼ��Ļ�׼��б�ʣ�Ϊʲô����Ҫ��ΪF����������Ĵ����֪���ˡ�
  uint8 R_Found_T  = 'F';    //ȷ���ޱ�б�ʵĻ�׼�б����Ƿ��ҵ��ı�־
  uint8 Get_R_line = 'F';    //�ҵ���һ֡ͼ��Ļ�׼��б�ʣ�Ϊʲô����Ҫ��ΪF����������Ĵ����֪���ˡ�
  float D_L = 0;             //������ӳ��ߵ�б��
  float D_R = 0;             //�ұ����ӳ��ߵ�б��
  int ytemp_W_L;             //��ס�״��󶪱���
  int ytemp_W_R;             //��ס�״��Ҷ�����
  ExtenRFlag = 0;            //��־λ��0
  ExtenLFlag = 0;            //��־λ��0
  ImageStatus.OFFLine=2;     //����ṹ���Ա��֮���������︳ֵ������Ϊ��ImageStatus�ṹ������ĳ�Ա̫���ˣ�������ʱ��ֻ�õ���OFFLine�������������õ��������ĸ�ֵ��
  ImageStatus.Miss_Right_lines = 0;
  ImageStatus.WhiteLine = 0;
  ImageStatus.Miss_Left_lines = 0;
  for (Ysite = 54 ; Ysite > ImageStatus.OFFLine; Ysite--)                            //ǰ5����Get_BaseLine()���Ѿ�������ˣ����ڴ�55�д����Լ��趨�Ĳ�������OFFLine��
  {                                                                                  //��Ϊ̫ǰ���ͼ��ɿ��Բ��㣬����OFFLine�����ú��б�Ҫ��û��Ҫһֱ����ɨ����0�С�
    PicTemp = Pixle[Ysite];
    JumpPointtypedef JumpPoint[2];                                                   // JumpPoint[0]��������ߣ�JumpPoint[1]�����ұ��ߡ�

  /******************************ɨ�豾�е��ұ���******************************/
    IntervalLow  = ImageDeal[Ysite + 1].RightBorder  - ImageScanInterval;               //����һ�е��ұ��߼Ӽ�Interval��Ӧ���п�ʼɨ�豾�У�Intervalһ��ȡ5����Ȼ��Ϊ�˱���������԰����ֵ�ĵĴ�һ�㡣
    IntervalHigh = ImageDeal[Ysite + 1].RightBorder + ImageScanInterval;              //���������ֻ��Ҫ�����б�������5�Ļ����ϣ����10�е�������䣩ȥɨ�ߣ�һ������ҵ����еı����ˣ��������ֵ��ʵ����̫��
    LimitL(IntervalLow);                                                             //������ǶԴ���GetJumpPointFromDet()������ɨ���������һ���޷�������
    LimitH(IntervalHigh);                                                            //������һ�еı����ǵ�2�У�����2-5=-3��-3�ǲ��Ǿ�û��ʵ�������ˣ���ô����-3���أ�
    Get_Border_And_SideType(PicTemp, 'R', IntervalLow, IntervalHigh,&JumpPoint[1]);  //ɨ���õ�һ���Ӻ������Լ�����ȥ�������߼���
  /******************************ɨ�豾�е��ұ���******************************/

  /******************************ɨ�豾�е������******************************/
    IntervalLow =ImageDeal[Ysite + 1].LeftBorder  -ImageScanInterval;                //����һ�е�����߼Ӽ�Interval��Ӧ���п�ʼɨ�豾�У�Intervalһ��ȡ5����Ȼ��Ϊ�˱���������԰����ֵ�ĵĴ�һ�㡣
    IntervalHigh =ImageDeal[Ysite + 1].LeftBorder +ImageScanInterval;                //���������ֻ��Ҫ�����б�������5�Ļ����ϣ����10�е�������䣩ȥɨ�ߣ�һ������ҵ����еı����ˣ��������ֵ��ʵ����̫��
    LimitL(IntervalLow);                                                             //������ǶԴ���GetJumpPointFromDet()������ɨ���������һ���޷�������
    LimitH(IntervalHigh);                                                            //������һ�еı����ǵ�2�У�����2-5=-3��-3�ǲ��Ǿ�û��ʵ�������ˣ���ô����-3���أ�
    Get_Border_And_SideType(PicTemp, 'L', IntervalLow, IntervalHigh,&JumpPoint[0]);  //ɨ���õ�һ���Ӻ������Լ�����ȥ�������߼���
  /******************************ɨ�豾�е������******************************/

  /*
       ����Ĵ���Ҫ���뿴�����������ҵ����ڸ���ħ�Ļ���
        ����ذ�GetJumpPointFromDet()����������߼�������
        ������������������棬��T������W������H��������־����ʲô��
        һ��Ҫ�㶮!!!��Ȼ�Ļ������鲻Ҫ���¿��ˣ���Ҫ��ĥ�Լ�!!!
  */
    if (JumpPoint[0].type =='W')                                                     //������е���������ڲ��������䣬����10���㶼�ǰ׵㡣
    {
      ImageDeal[Ysite].LeftBorder =ImageDeal[Ysite + 1].LeftBorder;                  //��ô���е�����߾Ͳ�����һ�еı��ߡ�
    }
    else                                                                             //������е����������T������H���
    {
      ImageDeal[Ysite].LeftBorder = JumpPoint[0].point;                              //��ôɨ�赽�ı����Ƕ��٣��Ҿͼ�¼�����Ƕ��١�
    }

    if (JumpPoint[1].type == 'W')                                                    //������е��ұ������ڲ��������䣬����10���㶼�ǰ׵㡣
    {
      ImageDeal[Ysite].RightBorder =ImageDeal[Ysite + 1].RightBorder;                //��ô���е��ұ��߾Ͳ�����һ�еı��ߡ�
    }
    else                                                                             //������е��ұ�������T������H���
    {
      ImageDeal[Ysite].RightBorder = JumpPoint[1].point;                             //��ôɨ�赽�ı����Ƕ��٣��Ҿͼ�¼�����Ƕ��١�
    }

    ImageDeal[Ysite].IsLeftFind =JumpPoint[0].type;                                  //��¼�����ҵ�����������ͣ���T����W������H��������ͺ��������õģ���Ϊ�һ�Ҫ��һ������
    ImageDeal[Ysite].IsRightFind = JumpPoint[1].type;                                //��¼�����ҵ����ұ������ͣ���T����W������H��������ͺ��������õģ���Ϊ�һ�Ҫ��һ������
  /*
        ����Ϳ�ʼ��W��H���͵ı��߷ֱ���д��� ΪʲôҪ����
        ����㿴����GetJumpPointFromDet�����߼���������T W H�������ͷֱ��Ӧʲô�����
        �����Ӧ��֪��W��H���͵ı��߶����ڷ��������ͣ������ǲ���Ҫ����
        ��һ���ֵĴ���˼·��Ҫ�Լ�������ʱ��úõ�ȥ��ĥ������ע������û������˵����ġ���
        ʵ���벻ͨ�������Ұɣ�
  */
    /************************************����ȷ��������(��H��)�ı߽�*************************************/

    if (( ImageDeal[Ysite].IsLeftFind == 'H' || ImageDeal[Ysite].IsRightFind == 'H'))
    {
      /**************************��������ߵĴ�����***************************/
      if (ImageDeal[Ysite].IsLeftFind == 'H')
      {
        for (Xsite = (ImageDeal[Ysite].LeftBorder + 1); Xsite <= (ImageDeal[Ysite].RightBorder - 1); Xsite++)                                                           //���ұ���֮������ɨ��
        {
          if ((*(PicTemp + Xsite) == 0) && (*(PicTemp + Xsite + 1) != 0))//�ڰ�����
          {
            ImageDeal[Ysite].LeftBorder =Xsite;
            ImageDeal[Ysite].IsLeftFind = 'T';
            break;
          }
          else if (*(PicTemp + Xsite) != 0)
            break;
          else if (Xsite ==(ImageDeal[Ysite].RightBorder - 1))
          {
            ImageDeal[Ysite].IsLeftFind = 'T';
            break;
          }
        }
      }
      /**************************��������ߵĴ�����***************************/


      /**************************�����ұ��ߵĴ�����***************************/
      if (ImageDeal[Ysite].IsRightFind == 'H')
      {
        for (Xsite = (ImageDeal[Ysite].RightBorder - 1);Xsite >= (ImageDeal[Ysite].LeftBorder + 1); Xsite--)
        {
          if ((*(PicTemp + Xsite) == 0) && (*(PicTemp + Xsite - 1) != 0))//�ڰ�����
          {
            ImageDeal[Ysite].RightBorder =Xsite;
            ImageDeal[Ysite].IsRightFind = 'T';
            break;
          }
          else if (*(PicTemp + Xsite) != 0)
            break;
          else if (Xsite == (ImageDeal[Ysite].LeftBorder + 1))
          {
            ImageDeal[Ysite].RightBorder = Xsite;
            ImageDeal[Ysite].IsRightFind = 'T';
            break;
          }
         }
       }
     }
    /**************************�����ұ��ߵĴ�����***************************/

  /*****************************����ȷ��������ı߽�******************************/



 /************************************����ȷ���ޱ��У���W�ࣩ�ı߽�****************************************************************/
    int ysite = 0;
    uint8 L_found_point = 0;
    uint8 R_found_point = 0;
    /**************************��������ߵ��ޱ���***************************/
    if (ImageDeal[Ysite].IsRightFind == 'W' && Ysite > 10 && Ysite < 50)
    {
      if (Get_R_line == 'F')
      {
        Get_R_line = 'T';
        ytemp_W_R = Ysite + 2;
        for (ysite = Ysite + 1; ysite < Ysite + 15; ysite++)
        {
          if (ImageDeal[ysite].IsRightFind =='T')
          {
              R_found_point++;
          }
        }
        if (R_found_point >8)
        {
          D_R = ((float)(ImageDeal[Ysite + R_found_point].RightBorder - ImageDeal[Ysite + 3].RightBorder)) /((float)(R_found_point - 3));
          if (D_R > 0)
          {
            R_Found_T ='T';
          }
          else
          {
            R_Found_T = 'F';
            if (D_R < 0)
            {
                ExtenRFlag = 'F';
            }
          }
        }
      }
      if (R_Found_T == 'T')
      {
        ImageDeal[Ysite].RightBorder =ImageDeal[ytemp_W_R].RightBorder -D_R * (ytemp_W_R - Ysite);  //����ҵ��� ��ô�Ի�׼�����ӳ���
      }
      LimitL(ImageDeal[Ysite].RightBorder);  //�޷�
      LimitH(ImageDeal[Ysite].RightBorder);  //�޷�
    }
    /**************************��������ߵ��ޱ���***************************/


    /**************************�����ұ��ߵ��ޱ���***************************/
    if (ImageDeal[Ysite].IsLeftFind == 'W' && Ysite > 10 && Ysite < 50 )
    {
      if (Get_L_line == 'F')
      {
        Get_L_line = 'T';
        ytemp_W_L = Ysite + 2;
        for (ysite = Ysite + 1; ysite < Ysite + 15; ysite++)
        {
          if (ImageDeal[ysite].IsLeftFind == 'T')
            {
              L_found_point++;
            }
        }
        if (L_found_point > 8)              //�ҵ���׼б�ʱ�  ���ӳ�������ȷ���ޱ�
        {
          D_L = ((float)(ImageDeal[Ysite + 3].LeftBorder -ImageDeal[Ysite + L_found_point].LeftBorder)) /((float)(L_found_point - 3));
          if (D_L > 0)
          {
            L_Found_T = 'T';
          }
          else
          {
            L_Found_T = 'F';
            if (D_L < 0)
            {
                ExtenLFlag = 'F';
            }
          }
        }
      }

      if (L_Found_T == 'T')
      {
          ImageDeal[Ysite].LeftBorder =ImageDeal[ytemp_W_L].LeftBorder + D_L * (ytemp_W_L - Ysite);
      }

      LimitL(ImageDeal[Ysite].LeftBorder);  //�޷�
      LimitH(ImageDeal[Ysite].LeftBorder);  //�޷�
    }

    /**************************�����ұ��ߵ��ޱ���***************************/
    /************************************����ȷ���ޱ��У���W�ࣩ�ı߽�****************************************************************/


    /************************************��������֮��������һЩ������������*************************************************/
      if (ImageDeal[Ysite].IsLeftFind == 'W' && ImageDeal[Ysite].IsRightFind == 'W')
      {
          ImageStatus.WhiteLine++;  //Ҫ�����Ҷ��ޱߣ�������+1
      }
     if (ImageDeal[Ysite].IsLeftFind == 'W'&&Ysite<55)
     {
          ImageStatus.Miss_Left_lines++;
     }
     if (ImageDeal[Ysite].IsRightFind == 'W'&&Ysite<55)
     {
          ImageStatus.Miss_Right_lines++;
     }

      LimitL(ImageDeal[Ysite].LeftBorder);   //�޷�
      LimitH(ImageDeal[Ysite].LeftBorder);   //�޷�
      LimitL(ImageDeal[Ysite].RightBorder);  //�޷�
      LimitH(ImageDeal[Ysite].RightBorder);  //�޷�

      ImageDeal[Ysite].Wide = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
      ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;

      if (ImageDeal[Ysite].Wide <= 7)        //�����ֵ�ɵ�
      {
          ImageStatus.OFFLine = Ysite + 1;
          break;
      }
      else if (ImageDeal[Ysite].RightBorder <= 10 || ImageDeal[Ysite].LeftBorder >= 70)
      {
          ImageStatus.OFFLine = Ysite + 1;
          break;
      }
      /************************************��������֮��������һЩ������������*************************************************/
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Get_ExtensionLine
//  @brief          ������������þ��ǲ��ߣ�
//  @brief          ʮ��·�����������˵����ֱ�ж԰ɣ����������������ͷɨ�ߵ�ʱ���ǲ��ǻ����ɨ�������ߵ��������Ϊ�Ǽ��ж��ǰ�ɫ����Ҳ����ڰ�����㡣
//  @brief          ���԰���������ѱ����㷨��������ǲ�������������㷨����Ļ���������Щ�е����ұ߽��ǲ��ǾͲ����ˣ���Ӧ�������ǲ���Ҳ�����ˣ������ܱ�֤С����ֱ���
//  @brief          ��Ȼ��֤���ˣ��������ʱ��С�����ܾͻ��������������ߣ�ֱ����ת������ת�ˣ��ǲ���Υ�����������ˣ����ǲ��Ǿͼ��ˣ�����˵�����Ƿǳ���Ҫ��һ����
//  @parameter      void
//  @time           2023��2��21��
//  @Author
//  Sample usage:   Get_ExtensionLine();
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void Get_ExtensionLine(void)
{
    //ImageStatus.OFFLine=5;                                                  //����ṹ���Ա��֮���������︳ֵ������Ϊ��ImageStatus�ṹ������ĳ�Ա̫���ˣ�������ʱ��ֻ�õ���OFFLine�������������õ��������ĸ�ֵ��
    /************************************����ߵĲ��ߴ���*************************************************/
    if (ImageStatus.WhiteLine >= 8)                                       //��������е���������8
        TFSite = 55;                                                      //�Ǿ͸�TFSite��ֵΪ55����������Ǵ����㲹��б�ʵ�һ��������
    left_FTSite=0;
    right_FTSite=0;
    if (ExtenLFlag != 'F')                                                //���ExtenLFlag��־��������F���ǾͿ�ʼ���в��߲�����
        for (Ysite = 54; Ysite >= (ImageStatus.OFFLine + 4);Ysite--)        //�ӵ�54��ʼ����ɨ��һֱɨ���������漸�С�
        {
            PicTemp = Pixle[Ysite];
            if (ImageDeal[Ysite].IsLeftFind =='W')                            //������е������������W���ͣ�Ҳ�����ޱ������͡�
            {
                if (ImageDeal[Ysite + 1].LeftBorder >= 80)                      //�����߽絽�˵�70���ұ�ȥ�ˣ��Ǵ���ʾ��Ǽ��������˵���Ѿ�����ˡ�
                {
                  ImageStatus.OFFLine = Ysite + 1;                              //���������õĴ��������ǲ�����ֱ������ѭ����
                  break;
                }
                while (Ysite >= (ImageStatus.OFFLine + 4))                      //�����߽��������Ǿͽ���whileѭ�����ţ�ֱ������ѭ������������
                {
                    Ysite--;                                                      //��������
                    if (ImageDeal[Ysite].IsLeftFind == 'T'
                      &&ImageDeal[Ysite - 1].IsLeftFind == 'T'
                      &&ImageDeal[Ysite - 2].IsLeftFind == 'T'
                      &&ImageDeal[Ysite - 2].LeftBorder > 0
                      &&ImageDeal[Ysite - 2].LeftBorder <80
                      )                                                         //���ɨ�����ޱ��е������������ж�����������
                    {
                      left_FTSite = Ysite - 2;                                  //�ǾͰ�ɨ������һ�е��������д���FTsite����
                    break;                                                      //����whileѭ��
                    }
                }
                DetL =((float)(ImageDeal[left_FTSite].LeftBorder - ImageDeal[TFSite].LeftBorder)) /((float)(left_FTSite - TFSite));  //��������ߵĲ���б��
                if (left_FTSite > ImageStatus.OFFLine)                              //���FTSite�洢����һ����ͼ�񶥱�OFFline������
                    for (ytemp = TFSite; ytemp >= left_FTSite; ytemp--)               //��ô�Ҿʹӵ�һ��ɨ������߽������ڶ��е�λ�ÿ�ʼ����һֱ���ߣ�����FTSite�С�
                    {
//                        LimitL((int)(DetL * ((float)(ytemp - TFSite))) + ImageDeal[TFSite].LeftBorder);//��ydc���Լ�д���޷��������������ʾ��ʵҲû��Ҫ�޷�
//                        LimitH((int)(DetL * ((float)(ytemp - TFSite))) + ImageDeal[TFSite].LeftBorder);
                        ImageDeal[ytemp].LeftBorder = (int)(DetL * ((float)(ytemp - TFSite))) +ImageDeal[TFSite].LeftBorder;     //������Ǿ���Ĳ��߲�����
                    }
            }
            else                                                              //ע�⿴������else���ĸ�if��һ�ԣ�������߼���ϵ��
                TFSite = Ysite + 2;                                             //����ΪʲôҪYsite+2����û����ע�����潲������Լ����ɡ�
        }
    /************************************����ߵĲ��ߴ���*************************************************/


    /************************************�ұ��ߵĲ��ߴ���������ߴ���˼·һģһ����ע����*************************************************/
    if (ImageStatus.WhiteLine >= 8)
    TFSite = 55;
    if (ExtenRFlag != 'F')
    for (Ysite = 54; Ysite >= (ImageStatus.OFFLine + 4);Ysite--)
    {
      PicTemp = Pixle[Ysite];  //�浱ǰ��
      if (ImageDeal[Ysite].IsRightFind =='W')
      {
        if (ImageDeal[Ysite + 1].RightBorder <= 10)
        {
          ImageStatus.OFFLine =Ysite + 1;
          break;
        }
        while (Ysite >= (ImageStatus.OFFLine + 4))
        {
          Ysite--;
          if (ImageDeal[Ysite].IsRightFind == 'T'
              &&ImageDeal[Ysite - 1].IsRightFind == 'T'
              &&ImageDeal[Ysite - 2].IsRightFind == 'T'
              &&ImageDeal[Ysite - 2].RightBorder < 79
              &&ImageDeal[Ysite - 2].RightBorder > 10
              )
          {
              right_FTSite = Ysite - 2;
            break;
          }
        }

        DetR =((float)(ImageDeal[right_FTSite].RightBorder -ImageDeal[TFSite].RightBorder)) /((float)(right_FTSite - TFSite));
        if (right_FTSite > ImageStatus.OFFLine)
          for (ytemp = TFSite; ytemp >= right_FTSite;ytemp--)
          {
//              LimitL((int)(DetR * ((float)(ytemp - TFSite))) +ImageDeal[TFSite].RightBorder);//��ydc���Լ�д���޷��������������ʾ��ʵҲû��Ҫ�޷�
//              LimitH((int)(DetR * ((float)(ytemp - TFSite))) +ImageDeal[TFSite].RightBorder);
              ImageDeal[ytemp].RightBorder = (int)(DetR * ((float)(ytemp - TFSite))) +ImageDeal[TFSite].RightBorder;
          }
      }
      else
        TFSite =Ysite +2;
    }
      /************************************�ұ��ߵĲ��ߴ���������ߴ���˼·һģһ����ע����*************************************************/
}

/*�Ͻ��������ַ���ɨ�ߣ���Ϊ����Բ�����ж�Ԫ�صĵڶ�����*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Search_Bottom_Line_OTSU
//  @brief          ��ȡ�ײ����ұ���
//  @param          imageInput[IMAGE_ROW][IMAGE_COL]        �����ͼ������
//  @param          Row                                     ͼ���Ysite
//  @param          Col                                     ͼ���Xsite
//  @return         Bottonline                              �ױ���ѡ��
//  @time           2022��10��9��
//  @Author         ������
//  Sample usage:   Search_Bottom_Line_OTSU(imageInput, Row, Col, Bottonline);
//--------------------------------------------------------------------------------------------------------------------------------------------

void Search_Bottom_Line_OTSU(uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{

    //Ѱ����߽߱�
    for (int Xsite = Col / 2-2; Xsite > 1; Xsite--)
    {
        if (imageInput[Bottonline][Xsite] == Image_white && imageInput[Bottonline][Xsite - 1] == 0)
        {
            ImageDeal[Bottonline].LeftBoundary = Xsite;//��ȡ�ױ������
            break;
        }
    }
    for (int Xsite = Col / 2+2; Xsite < LCDW-1; Xsite++)
    {
        if (imageInput[Bottonline][Xsite] == Image_white && imageInput[Bottonline][Xsite + 1] == 0)
        {
            ImageDeal[Bottonline].RightBoundary = Xsite;//��ȡ�ױ��ұ���
            break;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Search_Left_and_Right_Lines
//  @brief          ͨ��sobel��ȡ���ұ���
//  @param          imageInput[IMAGE_ROW][IMAGE_COL]        �����ͼ������
//  @param          Row                                     ͼ���Ysite
//  @param          Col                                     ͼ���Xsite
//  @param          Bottonline                              �ױ���ѡ��
//  @return         ��
//  @time           2022��10��7��
//  @Author         ������
//  Sample usage:   Search_Left_and_Right_Lines(imageInput, Row, Col, Bottonline);
//--------------------------------------------------------------------------------------------------------------------------------------------

void Search_Left_and_Right_Lines(uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{
    //����С�˵ĵ�ǰ����״̬λ��Ϊ �� �� �� �� һ��Ҫ�� �ϣ����Ϊ��ɫ ���ϱ�Ϊ��ɫ �£��ұ�Ϊɫ  �ң������к�ɫ
/*  ǰ�������壺
                *   0
                * 3   1
                *   2
*/
/*Ѱ�����������*/
    uint8 Left_Rule[2][8] = {
                                  {0,-1,1,0,0,1,-1,0 },//{0,-1},{1,0},{0,1},{-1,0},  (x,y )
                                  {-1,-1,1,-1,1,1,-1,1} //{-1,-1},{1,-1},{1,1},{-1,1}
    };
    /*Ѱ�����������*/
    int Right_Rule[2][8] = {
                              {0,-1,1,0,0,1,-1,0 },//{0,-1},{1,0},{0,1},{-1,0},
                              {1,-1,1,1,-1,1,-1,-1} //{1,-1},{1,1},{-1,1},{-1,-1}
    };
    int num=0;
    uint8 Left_Ysite = Bottonline;
    uint8 Left_Xsite = (uint8)ImageDeal[Bottonline].LeftBoundary;
    uint8 Left_Rirection = 0;//��߷���
    uint8 Pixel_Left_Ysite = Bottonline;
    uint8 Pixel_Left_Xsite = 0;

    uint8 Right_Ysite = Bottonline;
    uint8 Right_Xsite = (uint8)ImageDeal[Bottonline].RightBoundary;
    uint8 Right_Rirection = 0;//�ұ߷���
    uint8 Pixel_Right_Ysite = Bottonline;
    uint8 Pixel_Right_Xsite = 0;
    uint8 Ysite = Bottonline;
    ImageStatus.OFFLineBoundary = 5;
    while (1)
    {
        num++;
        if(num>400)
        {
            ImageStatus.OFFLineBoundary = Ysite;
            break;
        }
        if (Ysite >= Pixel_Left_Ysite && Ysite >= Pixel_Right_Ysite)
        {
            if (Ysite < ImageStatus.OFFLineBoundary)
            {
                ImageStatus.OFFLineBoundary = Ysite;
                break;
            }
            else
            {
                Ysite--;
            }
        }
        /*********���Ѳ��*******/
        if ((Pixel_Left_Ysite > Ysite) || Ysite == ImageStatus.OFFLineBoundary)//�ұ�ɨ��
        {
            /*����ǰ������*/
            Pixel_Left_Ysite = Left_Ysite + Left_Rule[0][2 * Left_Rirection + 1];
            Pixel_Left_Xsite = Left_Xsite + Left_Rule[0][2 * Left_Rirection];

            if (imageInput[Pixel_Left_Ysite][Pixel_Left_Xsite] == 0)//ǰ���Ǻ�ɫ
            {
                //˳ʱ����ת90
                if (Left_Rirection == 3)
                    Left_Rirection = 0;
                else
                    Left_Rirection++;
            }
            else//ǰ���ǰ�ɫ
            {
                /*������ǰ������*/
                Pixel_Left_Ysite = Left_Ysite + Left_Rule[1][2 * Left_Rirection + 1];
                Pixel_Left_Xsite = Left_Xsite + Left_Rule[1][2 * Left_Rirection];

                if (imageInput[Pixel_Left_Ysite][Pixel_Left_Xsite] == 0)//��ǰ��Ϊ��ɫ
                {
                    //���򲻱�  Left_Rirection
                    Left_Ysite = Left_Ysite + Left_Rule[0][2 * Left_Rirection + 1];
                    Left_Xsite = Left_Xsite + Left_Rule[0][2 * Left_Rirection];
                    if (ImageDeal[Left_Ysite].LeftBoundary_First == 0)
                        ImageDeal[Left_Ysite].LeftBoundary_First = Left_Xsite;
                    ImageDeal[Left_Ysite].LeftBoundary = Left_Xsite;
                }
                else//��ǰ��Ϊ��ɫ
                {
                    // �������ı� Left_Rirection  ��ʱ��90��
                    Left_Ysite = Left_Ysite + Left_Rule[1][2 * Left_Rirection + 1];
                    Left_Xsite = Left_Xsite + Left_Rule[1][2 * Left_Rirection];
                    if (ImageDeal[Left_Ysite].LeftBoundary_First == 0)
                        ImageDeal[Left_Ysite].LeftBoundary_First = Left_Xsite;
                    ImageDeal[Left_Ysite].LeftBoundary = Left_Xsite;
                    if (Left_Rirection == 0)
                        Left_Rirection = 3;
                    else
                        Left_Rirection--;
                }
            }
        }
        /*********�ұ�Ѳ��*******/
        if ((Pixel_Right_Ysite > Ysite) || Ysite == ImageStatus.OFFLineBoundary)//�ұ�ɨ��
        {
            /*����ǰ������*/
            Pixel_Right_Ysite = Right_Ysite + Right_Rule[0][2 * Right_Rirection + 1];
            Pixel_Right_Xsite = Right_Xsite + Right_Rule[0][2 * Right_Rirection];

            if (imageInput[Pixel_Right_Ysite][Pixel_Right_Xsite] == 0)//ǰ���Ǻ�ɫ
            {
                //��ʱ����ת90
                if (Right_Rirection == 0)
                    Right_Rirection = 3;
                else
                    Right_Rirection--;
            }
            else//ǰ���ǰ�ɫ
            {
                /*������ǰ������*/
                Pixel_Right_Ysite = Right_Ysite + Right_Rule[1][2 * Right_Rirection + 1];
                Pixel_Right_Xsite = Right_Xsite + Right_Rule[1][2 * Right_Rirection];

                if (imageInput[Pixel_Right_Ysite][Pixel_Right_Xsite] == 0)//��ǰ��Ϊ��ɫ
                {
                    //���򲻱�  Right_Rirection
                    Right_Ysite = Right_Ysite + Right_Rule[0][2 * Right_Rirection + 1];
                    Right_Xsite = Right_Xsite + Right_Rule[0][2 * Right_Rirection];
                    if (ImageDeal[Right_Ysite].RightBoundary_First == 93)
                        ImageDeal[Right_Ysite].RightBoundary_First = Right_Xsite;
                    ImageDeal[Right_Ysite].RightBoundary = Right_Xsite;
                }
                else//��ǰ��Ϊ��ɫ
                {
                    // �������ı� Right_Rirection  ��ʱ��90��
                    Right_Ysite = Right_Ysite + Right_Rule[1][2 * Right_Rirection + 1];
                    Right_Xsite = Right_Xsite + Right_Rule[1][2 * Right_Rirection];
                    if (ImageDeal[Right_Ysite].RightBoundary_First == 93)
                        ImageDeal[Right_Ysite].RightBoundary_First = Right_Xsite;
                    ImageDeal[Right_Ysite].RightBoundary = Right_Xsite;
                    if (Right_Rirection == 3)
                        Right_Rirection = 0;
                    else
                        Right_Rirection++;
                }
            }
        }
        if (abs(Pixel_Right_Xsite - Pixel_Left_Xsite) < 3)//Ysite<80��Ϊ�˷��ڵײ��ǰ�����ɨ�����  3 && Ysite < 30
        {
            ImageStatus.OFFLineBoundary = Ysite;
            break;
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Search_Border_OTSU
//  @brief          ͨ��OTSU��ȡ���� ����Ϣ
//  @param          imageInput[IMAGE_ROW][IMAGE_COL]        �����ͼ������
//  @param          Row                                     ͼ���Ysite
//  @param          Col                                     ͼ���Xsite
//  @param          Bottonline                              �ױ���ѡ��
//  @return         ��
//  @time           2022��10��7��
//  @Author         ������
//  Sample usage:   Search_Border_OTSU(mt9v03x_image, IMAGE_ROW, IMAGE_COL, IMAGE_ROW-8);
//--------------------------------------------------------------------------------------------------------------------------------------------

void Search_Border_OTSU(uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{
    ImageStatus.WhiteLine_L = 0;
    ImageStatus.WhiteLine_R = 0;
    //ImageStatus.OFFLine = 1;
    /*�����±߽紦��*/
    for (int Xsite = 0; Xsite < LCDW; Xsite++)
    {
        imageInput[0][Xsite] = 0;
        imageInput[Bottonline + 1][Xsite] = 0;
    }
    /*�����ұ߽紦��*/
    for (int Ysite = 0; Ysite < LCDH; Ysite++)
    {
            ImageDeal[Ysite].LeftBoundary_First = 0;
            ImageDeal[Ysite].RightBoundary_First = 93;

            imageInput[Ysite][0] = 0;
            imageInput[Ysite][LCDW - 1] = 0;
    }
    /********��ȡ�ײ�����*********/
    Search_Bottom_Line_OTSU(imageInput, Row, Col, Bottonline);
    /********��ȡ���ұ���*********/
    Search_Left_and_Right_Lines(imageInput, Row, Col, Bottonline);

    for (int Ysite = Bottonline; Ysite > ImageStatus.OFFLineBoundary + 1; Ysite--)
    {
        if (ImageDeal[Ysite].LeftBoundary < 3)
        {
            ImageStatus.WhiteLine_L++;
        }
        if (ImageDeal[Ysite].RightBoundary > LCDW - 3)
        {
            ImageStatus.WhiteLine_R++;
        }
    }
}

//--------------------------------------------------------------
//  @name           Element_Judgment_Left_Rings()
//  @brief          ����ͼ���жϵ��Ӻ����������ж���Բ������.
//  @parameter      void
//  @time
//  @Author         MRCHEN
//  Sample usage:   Element_Judgment_Left_Rings();
//--------------------------------------------------------------
void Element_Judgment_Left_Rings()//�����и�ȱ�ݾ�������ӻ����������Ҳ������½ǵ�ͻ���
{
    if (   ImageStatus.Miss_Right_lines > 5 || ImageStatus.Miss_Left_lines < 13
        || ImageStatus.OFFLine > 10 || Straight_Judge(2, 10, 55) > 1 || ImageStatus.WhiteLine > 4
        || ImageDeal[52].IsLeftFind == 'W'
        || ImageDeal[53].IsLeftFind == 'W'
        || ImageDeal[54].IsLeftFind == 'W'
        || ImageDeal[55].IsLeftFind == 'W'
        || ImageDeal[56].IsLeftFind == 'W'
        || ImageDeal[57].IsLeftFind == 'W'
        || ImageDeal[58].IsLeftFind == 'W'
        )
        return;
    int ring_ysite = 25;
    Left_RingsFlag_Point1_Ysite = 0;
    Left_RingsFlag_Point2_Ysite = 0;
    for (int Ysite = 58; Ysite > ring_ysite; Ysite--)//�������������ߵĹյ��ж�
    {
        if (ImageDeal[Ysite].LeftBoundary_First - ImageDeal[Ysite - 1].LeftBoundary_First > 4)
        {
            Left_RingsFlag_Point1_Ysite = Ysite;
            break;
        }
    }
    for (int Ysite = 58; Ysite > ring_ysite; Ysite--)
    {
        if (ImageDeal[Ysite + 1].LeftBoundary - ImageDeal[Ysite].LeftBoundary > 4)
        {
            Left_RingsFlag_Point2_Ysite = Ysite;
            break;
        }
    }
    for (int Ysite = Left_RingsFlag_Point1_Ysite; Ysite > ImageStatus.OFFLine; Ysite--)
    {
        if (ImageDeal[Ysite + 6].LeftBorder < ImageDeal[Ysite+3].LeftBorder
            && ImageDeal[Ysite + 5].LeftBorder < ImageDeal[Ysite+3].LeftBorder
            && ImageDeal[Ysite+3].LeftBorder > ImageDeal[Ysite + 2].LeftBorder
            && ImageDeal[Ysite+3].LeftBorder > ImageDeal[Ysite + 1].LeftBorder)
        {
            Ring_Help_Flag = 1;
            break;
        }
    }
    if(Left_RingsFlag_Point2_Ysite > Left_RingsFlag_Point1_Ysite+3 && Ring_Help_Flag == 0)
    {
        if(ImageStatus.Miss_Left_lines>10)
            Ring_Help_Flag = 1;
    }
    if (Left_RingsFlag_Point2_Ysite > Left_RingsFlag_Point1_Ysite+3 && Ring_Help_Flag == 1)
    {
        ImageFlag.image_element_rings = 1;
        ImageFlag.image_element_rings_flag = 1;
        ImageFlag.ring_big_small=1;
    }
    Ring_Help_Flag = 0;
}



//--------------------------------------------------------------
//  @name           Element_Judgment_Right_Rings()
//  @brief          ����ͼ���жϵ��Ӻ����������ж���Բ������.
//  @parameter      void
//  @time
//  @Author         MRCHEN
//  Sample usage:   Element_Judgment_Right_Rings();
//--------------------------------------------------------------
void Element_Judgment_Right_Rings()//�����и�ȱ�ݾ�������ӻ����������Ҳ������½ǵ�ͻ���
{
    if (   ImageStatus.Miss_Left_lines > 5 || ImageStatus.Miss_Right_lines < 13
        || ImageStatus.OFFLine > 10 || Straight_Judge(1, 10, 55) > 1 || ImageStatus.WhiteLine > 4
        || ImageDeal[52].IsRightFind == 'W'
        || ImageDeal[53].IsRightFind == 'W'
        || ImageDeal[54].IsRightFind == 'W'
        || ImageDeal[55].IsRightFind == 'W'
        || ImageDeal[56].IsRightFind == 'W'
        || ImageDeal[57].IsRightFind == 'W'
        || ImageDeal[58].IsRightFind == 'W'
        )
        return;
    int ring_ysite = 25;
    Right_RingsFlag_Point1_Ysite = 0;
    Right_RingsFlag_Point2_Ysite = 0;
    for (int Ysite = 58; Ysite > ring_ysite; Ysite--)
    {
        if (ImageDeal[Ysite - 1].RightBoundary_First - ImageDeal[Ysite].RightBoundary_First > 4)//����ͻ�䣬����ǽ��������½ǵ�ͻ���
        {
            Right_RingsFlag_Point1_Ysite = Ysite;
            break;
        }
    }
    for (int Ysite = 58; Ysite > ring_ysite; Ysite--)
    {
        if (ImageDeal[Ysite].RightBoundary - ImageDeal[Ysite + 1].RightBoundary > 4)//����ͻ��ĵ㣬����ǻ�����ڴ���ͻ���
        {
            Right_RingsFlag_Point2_Ysite = Ysite;
            break;
        }
    }
    for (int Ysite = Right_RingsFlag_Point1_Ysite; Ysite > 10; Ysite--)//��Բ���ϵĵ�
    {
        if (   ImageDeal[Ysite + 6].RightBorder > ImageDeal[Ysite + 3].RightBorder
            && ImageDeal[Ysite + 5].RightBorder > ImageDeal[Ysite + 3].RightBorder
            && ImageDeal[Ysite + 3].RightBorder < ImageDeal[Ysite + 2].RightBorder
            && ImageDeal[Ysite + 3].RightBorder < ImageDeal[Ysite + 1].RightBorder
           )
        {
            Ring_Help_Flag = 1;
            break;
        }
    }
    if(Right_RingsFlag_Point2_Ysite > Right_RingsFlag_Point1_Ysite+3 && Ring_Help_Flag == 0)//Ӧ���Ǳ��մ�ʩ
    {
        if(ImageStatus.Miss_Right_lines>10)
            Ring_Help_Flag = 1;
    }
    if (Right_RingsFlag_Point2_Ysite > Right_RingsFlag_Point1_Ysite+3 && Ring_Help_Flag == 1)
    {
        ImageFlag.image_element_rings = 2;
        ImageFlag.image_element_rings_flag = 1;
        ImageFlag.ring_big_small=1;     //С��
    }
    Ring_Help_Flag = 0;
}

//--------------------------------------------------------------
//  @name           Element_Handle_Left_Rings()
//  @brief          ����ͼ������Ӻ���������������Բ������.
//  @parameter      void
//  @time
//  @Author         MRCHEN
//  Sample usage:   Element_Handle_Left_Rings();
//-------------------------------------------------------------
void Element_Handle_Left_Rings()
{
    /***************************************�ж�**************************************/
    int num = 0;
    for (int Ysite = 55; Ysite > 30; Ysite--)
    {
        if(ImageDeal[Ysite].IsLeftFind == 'W')
        {
            num++;
        }
        if(    ImageDeal[Ysite+3].IsLeftFind == 'W' && ImageDeal[Ysite+2].IsLeftFind == 'W'
            && ImageDeal[Ysite+1].IsLeftFind == 'W' && ImageDeal[Ysite].IsLeftFind == 'T' )
            break;
    }
    //׼������
    if (ImageFlag.image_element_rings_flag == 1 && num>10)
    {
        ImageFlag.image_element_rings_flag = 2;
    }
    if (ImageFlag.image_element_rings_flag == 2 && num<5)
    {
        ImageFlag.image_element_rings_flag = 5;
    }
    //����
    if(ImageFlag.image_element_rings_flag == 5 && ImageStatus.Miss_Right_lines>15)
    {
        ImageFlag.image_element_rings_flag = 6;
    }
        //����СԲ��
    if(ImageFlag.image_element_rings_flag == 6 && ImageStatus.Miss_Right_lines<7)
    {
        ImageFlag.image_element_rings_flag = 7;
    }
    //���� ��Բ���ж�
    if (ImageFlag.image_element_rings_flag == 7)
    {
        Point_Ysite = 0;
        Point_Xsite = 0;
        for (int Ysite = 45; Ysite > ImageStatus.OFFLine + 3; Ysite--)
        {
            if (    ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite + 2].RightBorder
                 && ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite - 2].RightBorder
                 && ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite + 1].RightBorder
                 && ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite - 1].RightBorder
                 && ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite + 4].RightBorder
                 && ImageDeal[Ysite].RightBorder <= ImageDeal[Ysite - 4].RightBorder)
            {
                Point_Xsite = ImageDeal[Ysite].RightBorder;
                Point_Ysite = Ysite;
                break;
            }
        }
        if (Point_Ysite > 22)
          ImageFlag.image_element_rings_flag = 8;
    }
    //������
    if (ImageFlag.image_element_rings_flag == 8)
    {
        if (    Straight_Judge(2, ImageStatus.OFFLine+15, 45) < 1
             && ImageStatus.Miss_Right_lines < 8
             && ImageStatus.OFFLine < 13)    //�ұ�Ϊֱ���ҽ�ֹ�У�ǰհֵ����С
            ImageFlag.image_element_rings_flag = 9;
    }
    //����Բ������
    if (ImageFlag.image_element_rings_flag == 9/* && ImageFlag.ring_big_small==1*/)
    {
        int num=0;
        for (int Ysite = 40; Ysite > 10; Ysite--)
        {
            if(ImageDeal[Ysite].IsLeftFind == 'W' )
                num++;
        }
        if( num<5 )
        {
            ImageFlag.image_element_rings_flag = 0;
            ImageFlag.image_element_rings = 0;
            ImageFlag.ring_big_small = 0;
        }
    }
    /***************************************����**************************************/
        //׼������  �����
    if (   ImageFlag.image_element_rings_flag == 1
        || ImageFlag.image_element_rings_flag == 2
        || ImageFlag.image_element_rings_flag == 3
        || ImageFlag.image_element_rings_flag == 4)
    {
        for (int Ysite = 59; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].Center = ImageDeal[Ysite].RightBorder - Half_Road_Wide[Ysite];
        }
    }
    //����  ����
    if( ImageFlag.image_element_rings_flag == 5
        ||ImageFlag.image_element_rings_flag == 6)
    {
        int  flag_Xsite_1=0;
        int flag_Ysite_1=0;
        float Slope_Rings=0;
        for(Ysite=55;Ysite>ImageStatus.OFFLine;Ysite--)//���满��
        {
            for(Xsite=ImageDeal[Ysite].LeftBorder + 1;Xsite<ImageDeal[Ysite].RightBorder - 1;Xsite++)
            {
                if(  Pixle[Ysite][Xsite] == Image_white && Pixle[Ysite][Xsite + 1] == 0)
                 {
                   flag_Ysite_1 = Ysite;
                   flag_Xsite_1 = Xsite;
                   Slope_Rings=(float)(93 - flag_Xsite_1)/(float)(59 - flag_Ysite_1);
                   break;
                 }
            }
            if(flag_Ysite_1 != 0)
            {
                break;
            }
        }
        if(flag_Ysite_1 == 0)
        {
            for(Ysite=ImageStatus.OFFLine+1;Ysite<30;Ysite++)
            {
                if(ImageDeal[Ysite].IsLeftFind=='T'&&ImageDeal[Ysite+1].IsLeftFind=='T'&&ImageDeal[Ysite+2].IsLeftFind=='W'
                    &&abs(ImageDeal[Ysite].LeftBorder-ImageDeal[Ysite+2].LeftBorder)>10
                  )
                {
                    flag_Ysite_1=Ysite;
                    flag_Xsite_1=ImageDeal[flag_Ysite_1].LeftBorder;
                    ImageStatus.OFFLine=(int16)Ysite;
                    Slope_Rings=(float)(93-flag_Xsite_1)/(float)(59-flag_Ysite_1);
                    break;
                }
            }
        }
        //����
        if(flag_Ysite_1 != 0)
        {
            for(Ysite=flag_Ysite_1;Ysite<60;Ysite++)
            {
                ImageDeal[Ysite].RightBorder=flag_Xsite_1+Slope_Rings*(Ysite-flag_Ysite_1);
                    ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder)/2;
                if(ImageDeal[Ysite].Center < 0)
                    ImageDeal[Ysite].Center = 0;
            }
            ImageDeal[flag_Ysite_1].RightBorder=flag_Xsite_1;
            for(Ysite=flag_Ysite_1-1;Ysite>10;Ysite--) //A���Ϸ�����ɨ��
            {
                for(Xsite=ImageDeal[Ysite+1].RightBorder-10;Xsite<ImageDeal[Ysite+1].RightBorder+2;Xsite++)
                {
                    if(Pixle[Ysite][Xsite]==Image_white && Pixle[Ysite][Xsite+1]==0)
                    {
                        ImageDeal[Ysite].RightBorder=Xsite;
                        ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder)/2;
                        if(ImageDeal[Ysite].Center<0)
                            ImageDeal[Ysite].Center = 0;
                        ImageDeal[Ysite].Wide=ImageDeal[Ysite].RightBorder-ImageDeal[Ysite].LeftBorder;
                        break;
                    }
                }
                if(ImageDeal[Ysite].Wide>8 &&ImageDeal[Ysite].RightBorder< ImageDeal[Ysite+2].RightBorder)
                {
                    continue;
                }
                else
                {
                    ImageStatus.OFFLine=Ysite+2;
                    break;
                }
            }
        }
    }
    //���� С���������� �󻷲���
    if (ImageFlag.image_element_rings_flag == 7)
    {

    }
    //��Բ������ ����
    if (ImageFlag.image_element_rings_flag == 8)    //��Բ��
    {
        Repair_Point_Xsite = 20;
        Repair_Point_Ysite = 0;
        for (int Ysite = 40; Ysite > 5; Ysite--)
        {
            if (Pixle[Ysite][28] == Image_white && Pixle[Ysite-1][28] == 0)//30
            {
                Repair_Point_Xsite = 28;
                Repair_Point_Ysite = Ysite-1;
                ImageStatus.OFFLine = Ysite + 1;  //��ֹ�����¹滮
                break;
            }
        }
        for (int Ysite = 57; Ysite > Repair_Point_Ysite-3; Ysite--)         //����
        {
            ImageDeal[Ysite].RightBorder = (ImageDeal[58].RightBorder - Repair_Point_Xsite) * (Ysite - 58) / (58 - Repair_Point_Ysite)  + ImageDeal[58].RightBorder;
            ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;
        }
    }
    //�ѳ��� �����
    if (ImageFlag.image_element_rings_flag == 9)
    {
        for (int Ysite = 59; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].Center = ImageDeal[Ysite].RightBorder - Half_Road_Wide[Ysite];
        }
    }
}

//--------------------------------------------------------------
//  @name           Element_Handle_Right_Rings()
//  @brief          ����ͼ������Ӻ���������������Բ������.
//  @parameter      void
//  @time
//  @Author         MRCHEN
//  Sample usage:   Element_Handle_Right_Rings();
//-------------------------------------------------------------
void Element_Handle_Right_Rings()
{
    /****************�ж�*****************/
    int num =0 ;
    for (int Ysite = 55; Ysite > 30; Ysite--)
    {
        if(ImageDeal[Ysite].IsRightFind == 'W')
        {
            num++;
        }
        if(    ImageDeal[Ysite+3].IsRightFind == 'W' && ImageDeal[Ysite+2].IsRightFind == 'W'
            && ImageDeal[Ysite+1].IsRightFind == 'W' && ImageDeal[Ysite].IsRightFind == 'T' )
            break;
    }
    //׼������
    if (ImageFlag.image_element_rings_flag == 1 && num>10)
    {
        ImageFlag.image_element_rings_flag = 2;
    }
    if (ImageFlag.image_element_rings_flag == 2 && num<5)
    {
        ImageFlag.image_element_rings_flag = 5;
    }
    //����
    if(ImageFlag.image_element_rings_flag == 5 && ImageStatus.Miss_Left_lines>15)
    {
        ImageFlag.image_element_rings_flag = 6;
    }
    //����СԲ��
    if(ImageFlag.image_element_rings_flag == 6 && ImageStatus.Miss_Left_lines<7)
    {
        ImageFlag.image_element_rings_flag = 7;
    }
    if (ImageFlag.image_element_rings_flag == 7)
    {
        Point_Xsite = 0;
        Point_Ysite = 0;
        for (int Ysite = 45; Ysite > ImageStatus.OFFLine+3; Ysite--)
        {
            if (    ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite + 2].LeftBorder
                 && ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite - 2].LeftBorder
                 && ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite + 1].LeftBorder
                 && ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite - 1].LeftBorder
                 && ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite + 4].LeftBorder
                 && ImageDeal[Ysite].LeftBorder >= ImageDeal[Ysite - 4].LeftBorder
                )
            {
                Point_Xsite = ImageDeal[Ysite].LeftBorder;
                Point_Ysite = Ysite;
                break;
            }
        }
        if (Point_Ysite > 22)
        {
            ImageFlag.image_element_rings_flag = 8;
        }
    }
    //������
    if (ImageFlag.image_element_rings_flag == 8)
    {
         if (   Straight_Judge(1, ImageStatus.OFFLine+15, 45) < 1
             && ImageStatus.Miss_Left_lines < 8
             && ImageStatus.OFFLine < 10)    //�ұ�Ϊֱ���ҽ�ֹ�У�ǰհֵ����С
            {ImageFlag.image_element_rings_flag = 9;}
    }
    if(ImageFlag.image_element_rings_flag == 9 )
    {
        int num=0;
        for (int Ysite = 40; Ysite > 10; Ysite--)
        {
            if(ImageDeal[Ysite].IsRightFind == 'W' )
            {
                num++;
            }
        }
        if(num < 5)
        {
            ImageFlag.image_element_rings_flag = 0;
            ImageFlag.image_element_rings = 0;
            ImageFlag.ring_big_small = 0;
        }
    }
    /***************************************����**************************************/
         //׼������  �����
    if (   ImageFlag.image_element_rings_flag == 1
        || ImageFlag.image_element_rings_flag == 2
        || ImageFlag.image_element_rings_flag == 3
        || ImageFlag.image_element_rings_flag == 4)
    {
        for (int Ysite = 59; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].Center = ImageDeal[Ysite].LeftBorder + Half_Road_Wide[Ysite];
        }
    }
    //����  ����
    if (   ImageFlag.image_element_rings_flag == 5
        || ImageFlag.image_element_rings_flag == 6)
    {
        int flag_Xsite_1=0;
        int  flag_Ysite_1=0;
        float Slope_Right_Rings = 0;
        for(Ysite=55;Ysite>ImageStatus.OFFLine;Ysite--)
        {
            for(Xsite=ImageDeal[Ysite].LeftBorder + 1;Xsite<ImageDeal[Ysite].RightBorder - 1;Xsite++)
            {
                if(Pixle[Ysite][Xsite] == Image_white && Pixle[Ysite][Xsite+1] == 0)
                {
                    flag_Ysite_1=Ysite;
                    flag_Xsite_1=Xsite;
                    Slope_Right_Rings=(float)(0-flag_Xsite_1)/(float)(59-flag_Ysite_1);
                    break;
                }
            }
            if(flag_Ysite_1!=0)
            {
              break;
            }
        }
        if(flag_Ysite_1==0)
        {
            for(Ysite=ImageStatus.OFFLine+1;Ysite<30;Ysite++)
            {
             if(ImageDeal[Ysite].IsRightFind=='T'&&ImageDeal[Ysite+1].IsRightFind=='T'&&ImageDeal[Ysite+2].IsRightFind=='W'
                &&abs(ImageDeal[Ysite].RightBorder-ImageDeal[Ysite+2].RightBorder)>10)
             {
                 flag_Ysite_1=Ysite;
                 flag_Xsite_1=ImageDeal[flag_Ysite_1].RightBorder;
                 ImageStatus.OFFLine = (int16)Ysite;
                 Slope_Right_Rings=(float)(0-flag_Xsite_1)/(float)(59-flag_Ysite_1);
                 break;
             }
            }
        }
        //����
        if(flag_Ysite_1!=0)
        {
            for(Ysite=flag_Ysite_1;Ysite<60;Ysite++)
            {
                ImageDeal[Ysite].LeftBorder=flag_Xsite_1+Slope_Right_Rings*(Ysite-flag_Ysite_1);
                ImageDeal[Ysite].Center=(ImageDeal[Ysite].LeftBorder+ImageDeal[Ysite].RightBorder)/2;//���
                if(ImageDeal[Ysite].Center > 93)
                    ImageDeal[Ysite].Center = 93;
            }
            ImageDeal[flag_Ysite_1].LeftBorder=flag_Xsite_1;
            for(Ysite=flag_Ysite_1-1;Ysite>10;Ysite--) //A���Ϸ�����ɨ��
            {
                for(Xsite=ImageDeal[Ysite+1].LeftBorder+8; Xsite>ImageDeal[Ysite+1].LeftBorder-4; Xsite--)
                {
                    if(Pixle[Ysite][Xsite] == Image_white && Pixle[Ysite][Xsite-1] == 0)
                    {
                     ImageDeal[Ysite].LeftBorder=Xsite;
                     ImageDeal[Ysite].Wide=ImageDeal[Ysite].RightBorder-ImageDeal[Ysite].LeftBorder;
                         ImageDeal[Ysite].Center=(ImageDeal[Ysite].LeftBorder+ImageDeal[Ysite].RightBorder)/2;//���
                     if(ImageDeal[Ysite].Center > 93)
                         ImageDeal[Ysite].Center = 93;
                     break;
                    }
                }
                if(ImageDeal[Ysite].Wide>8 && ImageDeal[Ysite].LeftBorder>  ImageDeal[Ysite+2].LeftBorder)
                {
                    continue;
                }
                else
                {
                    ImageStatus.OFFLine=Ysite+2;
                    break;
                }
            }
        }
    }
    //���ڲ�����
    if (ImageFlag.image_element_rings_flag == 7)
    {

    }
    //��Բ������ ����
    if (ImageFlag.image_element_rings_flag == 8)  //��Բ��
    {
        Repair_Point_Xsite = 60;
        Repair_Point_Ysite = 0;
        for (int Ysite = 40; Ysite > 5; Ysite--)
        {
            if (Pixle[Ysite][58] == Image_white && Pixle[Ysite-1][58] == 0)
            {
                Repair_Point_Xsite = 58;
                Repair_Point_Ysite = Ysite-1;
                ImageStatus.OFFLine = Ysite + 1;  //��ֹ�����¹滮
                break;
            }
        }
        for (int Ysite = 57; Ysite > Repair_Point_Ysite-3; Ysite--)         //����
        {
            ImageDeal[Ysite].LeftBorder = (ImageDeal[58].LeftBorder - Repair_Point_Xsite) * (Ysite - 58) / (58 - Repair_Point_Ysite)  + ImageDeal[58].LeftBorder;
            ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;
        }
    }
    //�ѳ��� �����
    if (ImageFlag.image_element_rings_flag == 9)
    {
        for (int Ysite = 59; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].Center = ImageDeal[Ysite].LeftBorder + Half_Road_Wide[Ysite];
        }
    }
}

/*Ԫ���жϺ���*/
void Scan_Element()
{
    if (ImageFlag.image_element_rings == 0 )
    {
        Element_Judgment_Left_Rings();      //��Բ��
        Element_Judgment_Right_Rings();     //��Բ��
    }
}

/*Ԫ�ش�����*/
void Element_Handle()
{
    if (ImageFlag.image_element_rings == 1)
        Element_Handle_Left_Rings();
    else if (ImageFlag.image_element_rings == 2)
        Element_Handle_Right_Rings();
    else if(ImageStatus.WhiteLine >= 8) //ʮ�ִ���
        Get_ExtensionLine();
}

float weighted_error_exp(int8 *error_center, uint8 cnt);
float center_line_error = 0.0;
int8 error_center[40];
void image_process(void)
{
    Image_CompressInit();   //�ڳ�ʼ�����              //ͼ��ѹ������ԭʼ��80*188��ͼ��ѹ����60*80��,��Ϊ����Ҫ��ô�����Ϣ��60*80�ܴ���õĻ��Ѿ��㹻�ˡ�
    Get_BinaryImage();                                //ͼ���ֵ�������Ѳɼ�����ԭʼ�Ҷ�ͼ���ɶ�ֵ��ͼ��Ҳ���Ǳ�ɺڰ�ͼ��

    Get_BaseLine();                                   //�Ż�֮��������㷨���õ�һ��ͼ��Ļ������ߣ�Ҳ������������еı�����Ϣ��������������
    Get_AllLine();                                    //�Ż�֮��������㷨���õ�һ��ͼ���ȫ�����ߡ�

    if(!ImageFlag.Ramp && !ImageFlag.RoadBlock_Flag && !ImageFlag.Out_Road)
        Search_Border_OTSU(Pixle, LCDH, LCDW, LCDH - 2);//58��λ����
    else
        ImageStatus.OFFLineBoundary = 5;

    Scan_Element();
    Element_Handle();
    if(ImageFlag.image_element_rings_flag){
//        buzzer_on();
    }
    else
//        buzzer_off();
    center_line_error = weighted_error_exp(error_center, 40);
}

// Ԥ����õ�ָ��Ȩ�ر�k = 0.05�����ȿɵ�����
const float weight_table[40] = {
    1.0000f, 0.9512f, 0.9048f, 0.8607f, 0.8187f, 0.7788f, 0.7408f, 0.7047f,
    0.6703f, 0.6376f, 0.6065f, 0.5769f, 0.5488f, 0.5220f, 0.4966f, 0.4724f,
    0.4493f, 0.4274f, 0.4066f, 0.3867f, 0.3679f, 0.3499f, 0.3329f, 0.3166f,
    0.3012f, 0.2865f, 0.2725f, 0.2592f, 0.2466f, 0.2346f, 0.2231f, 0.2122f,
    0.2019f, 0.1920f, 0.1827f, 0.1738f, 0.1653f, 0.1572f, 0.1496f, 0.1423f
};

// �����Ȩ�����ʽ��
float weighted_error_exp(int8 *error_center, uint8 cnt) {
    float weighted_sum = 0.0f;
    float weight_total = 0.0f;
    for(uint8 i = 59; i > 59 - cnt; i--){
        error_center[59 - i] = ImageDeal[i].Center - ImageSensorMid;
    }
    for (uint8 i = 0; i < cnt; i++) {
        weighted_sum += weight_table[i] * (float)error_center[i];
        weight_total += weight_table[i];
    }
    return weighted_sum / weight_total;
}

//��ʾͼ��   �ĳ����Լ��ľ��� �Ⱥ����㹻�����ˣ���ʾ�ص�����ʾ��ͦռ��Դ��
void ips200_displayimage032_zoom(void)
{
    for (uint16 i = 59; i > ImageStatus.OFFLine; i--)
    {
        ips200_draw_point((uint16)ImageDeal[i].RightBorder, i, RGB565_RED);//��ʾ��� ��ʾ����
        ips200_draw_point((uint16)ImageDeal[i].Center, i, RGB565_BLUE);//��ʾ��� ��ʾ�����
        ips200_draw_point((uint16)ImageDeal[i].LeftBorder, i, RGB565_RED);//��ʾ��� ��ʾ�ұ���
    }
}
