/*
 * camera.c
 *
 *  Created on: 2025年3月21日
 *      Author: ydc
 */
#include "image_dealwith.h"

uint8* Image_Use[LCDH][LCDW];      //用来存储压缩之后灰度图像的二维数组
uint8 Pixle[LCDH][LCDW];          //图像处理时真正处理的二值化图像数组
//uint8 All_Sobel_Image[LCDH][LCDW];
uint8 Threshold;                                //通过大津法计算出来的二值化阈值
uint16 threshold1, threshold2, threshold3, block_yuzhi=60;
uint16 yuzhi1,yuzhi2,yuzhi3;
uint16 Ramp_cancel,circle_stop,block_num,duan_num;
int ImageScanInterval = 5;                        //扫边的范围
static uint8* PicTemp;                          //一个保存单行图像的指针变量
static int IntervalLow = 0, IntervalHigh = 0;   //扫描区间的上下限变量
static int Ysite = 0, Xsite = 0;                //Ysite就是图像的行，Xsite就是图像的列。
static int BottomBorderRight = 93,              //59行的右边界
BottomBorderLeft = 0,                           //59行的左边界
BottomCenter = 0;                               //59行的中点
uint8 ExtenLFlag = 0;                           //左边线是否需要补线的标志变量
uint8 ExtenRFlag = 0;                           //右边线是否需要补线的标志变量
uint8 Ring_Help_Flag = 0;                       //进环辅助标志
int Right_RingsFlag_Point1_Ysite, Right_RingsFlag_Point2_Ysite; //右圆环判断的两点纵坐标
int Left_RingsFlag_Point1_Ysite, Left_RingsFlag_Point2_Ysite;   //左圆环判断的两点纵坐标
int Point_Xsite,Point_Ysite;                   //拐点横纵坐标
int Repair_Point_Xsite,Repair_Point_Ysite;     //补线点横纵坐标
int Point_Ysite1;                               //判断大小圆环时用的纵坐标
int Black;                                      //判断大小圆环时用的黑点数量
int Less_Big_Small_Num;                         //判断大小圆环时用的丢线
int left_difference_num;                        //十字左边线与标准中线39作差的和（40-20行）
int right_difference_num;                       //十字右边线与标准中线39作差的和（40-20行）
uint8 Garage_Location_Flag = 0;                 //判断库的次数
float Big_Small_Help_Gradient;               //大小圆环的辅助判断斜率
static int ytemp = 0;                           //存放行的临时变量
static int TFSite = 0, left_FTSite = 0,right_FTSite = 0;              //补线计算斜率的时候需要用的存放行的变量。
static float DetR = 0, DetL = 0;                //存放补线斜率的变量
ImageDealDatatypedef ImageDeal[60];             //记录单行信息的结构体数组
ImageDealDatatypedef ImageDeal1[80];             //记录单列信息的结构体数组
ImageStatustypedef ImageStatus;                 //图像处理的的全局变量
ImageFlagtypedef ImageFlag;
uint64 Gray_Value=0;
float Mh = MT9V03X_H;
float Lh = LCDH;
float Mw = MT9V03X_W;
float Lw = LCDW;

uint8 Half_Road_Wide[60] =                      //直到赛道半宽
{ 2,3,3,4,4,5,5,6,6,7,
  7,8,8,9,9,10,10,11,11,12,
  12,13,13,14,14,15,15,15,16,17,
  17,18,19,19,20,21,21,22,23,23,
  24,25,25,26,27,28,29,29,29,31,
  31,32,33,33,34,36,36,37,37,39,
};
uint8 Half_Bend_Wide[60] =                      //弯道赛道半宽
{   33,33,33,33,33,33,33,33,33,33,
    33,33,32,32,31,31,30,30,29,28,
    28,27,27,26,26,25,25,24,24,23,
    23,22,22,23,23,23,24,24,24,24,
    25,25,25,26,26,26,27,27,28,28,
    28,29,29,30,30,31,31,32,32,33,
};
//
//uint16 block_num=0, outroad_num=0, circle_num, position;
/*****************直线判断******************/
float Straight_Judge(uint8 dir, uint8 start, uint8 end)     //返回结果小于1即为直线
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

void Straight_long_judge(void)     //返回结果小于1即为直线
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

void Straight_long_handle(void)     //返回结果小于1即为直线
{

    if(ImageFlag.straight_long)
    {
        if((Straight_Judge(1,10,50) > 1) || (Straight_Judge(2,10,50) > 1) || (ImageStatus.OFFLine >= 3)||ImageStatus.Miss_Left_lines >= 2||ImageStatus.Miss_Right_lines>=2)
        {
            ImageFlag.straight_long = 0;
        }
    }
}

//用于变参数的直道检测
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
//  @brief          原始灰度图像压缩处理 压缩初始化
//  @brief          作用就是将原始尺寸的灰度图像压缩成你所需要的大小，这里我是把原始80行170列的灰度图像压缩成60行80列的灰度图像。
//  @brief          为什么要压缩？因为我不需要那么多的信息，60*80图像所展示的信息原则上已经足够完成比赛任务了，当然你可以根据自己的理解修改。
//  @parameter      void
//  @return         void
//  @time           2022年2月18日
//  @Author
//  Sample usage:   Image_CompressInit();
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void Image_CompressInit(void)
{
  int i, j, row, line;
  const float div_h = Mh / Lh, div_w = Mw / Lw;         //根据原始的图像尺寸和你所需要的图像尺寸确定好压缩比例。
  for (i = 0; i < LCDH; i++)                            //遍历图像的每一行，从第零行到第59行。
  {
    row = i * div_h;
    for (j = 0; j < LCDW; j++)                          //遍历图像的每一列，从第零列到第79列。
    {
      line = j * div_w;
      Image_Use[i][j] = &mt9v03x_image[row][line];       //mt9v03x_image数组里面是原始灰度图像，Image_Use数组存储的是我之后要拿去处理的图像，但依然是灰度图像哦！只是压缩了一下而已。
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           get_Threshold  //指针
//  @brief          优化之后的的大津法。大津法就是一种能够算出一幅图像最佳的那个分割阈值的一种算法。
//  @brief          这个东西你们可以如果实在不能理解就直接拿来用，什么参数都不用修改，只要没有光照影响，那么算出来的这个阈值就一定可以得到一幅效果还不错的二值化图像。
//  @parameter      image  原始的灰度图像数组
//  @parameter      clo    图像的宽（图像的列）
//  @parameter      row    图像的高（图像的行）
//  @return         uint8
//  @time           2022年2月17日
//  @Author
//  Sample usage:   Threshold = Threshold_deal(Image_Use[0], 80, 60); 把存放60行80列的二维图像数组Image_Use传进来，求出这幅图像的阈值，并将这个阈值赋给Threshold。
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
  for (i = 0; i < GrayScale; i++)                    //先把pixelCount和pixelPro两个数组元素全部赋值为0
  {
    pixelCount[i] = 0;
    pixelPro[i] = 0;
  }
  uint32 gray_sum = 0;
  /**************************************统计每个灰度值(0-255)在整幅图像中出现的次数**************************************/
  for (i = 0; i < height; i += 1)                   //遍历图像的每一行，从第零行到第59行。
  {
    for (j = 0; j < width; j += 1)                  //遍历图像的每一列，从第零列到第79列。
    {
      pixelCount[*image[i][j]]++;       //将当前的像素点的像素值（灰度值）作为计数数组的下标。
      gray_sum += *image[i][j];         //计算整幅灰度图像的灰度值总和。
    }
  }
  /**************************************统计每个灰度值(0-255)在整幅图像中出现的次数**************************************/
  /**************************************计算每个像素值（灰度值）在整幅灰度图像中所占的比例*************************************************/
  for (i = 0; i < GrayScale; i++)
  {
      pixelPro[i] = (float)pixelCount[i] / pixelSum;
  }
  /**************************************计算每个像素值（灰度值）在整幅灰度图像中所占的比例**************************************************/
  /**************************************开始遍历整幅图像的灰度值（0-255），这一步也是大津法最难理解的一步***************************/
  /*******************为什么说他难理解？因为我也是不理解！！反正好像就是一个数学问题，你可以理解为数学公式。***************************/
  float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
  w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
  for (j = 0; j < GrayScale; j++)
  {
    w0 += pixelPro[j];                          //求出背景部分每个灰度值的像素点所占的比例之和，即背景部分的比例。
    u0tmp += j * pixelPro[j];

    w1 = 1 - w0;
    u1tmp = gray_sum / pixelSum - u0tmp;

    u0 = u0tmp / w0;                            //背景平均灰度
    u1 = u1tmp / w1;                            //前景平均灰度
    u = u0tmp + u1tmp;                          //全局平均灰度
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
  /**************************************开始遍历整幅图像的灰度值（0-255），这一步也是大津法最难理解的一步***************************/
  /*******************为什么说他难理解？因为我也是不理解！！反正好像就是一个数学问题，你可以理解为数学公式。***************************/

  return threshold;                             //把上面这么多行代码算出来的阈值给return出去。
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Get_BinaryImage
//  @brief          灰度图像二值化处理
//  @brief          整体思路就是：先调用Get_Threshold（）函数得到阈值，然后遍历原始灰度图像的每一个像素点，用每一个像素点的灰度值来跟阈值计较。
//  @brief          大于阈值的你就把它那个像素点的值赋值为1（记为白点），否则就赋值为0（记为黑点）。当然你可以把这个赋值反过来，只要你自己清楚1和0谁代表黑谁代表白就行。
//  @brief          所以我前面提到的60*80现在你们就应该明白是什么意思了吧！就是像素点嘛，一行有80个像素点，一共60行，也就是压缩后的每一幅图像有4800个像素点。
//  @parameter      void
//  @return         void
//  @time           2022年2月17日
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
        Threshold = get_Threshold(Image_Use, LCDW, LCDH);      //这里是一个函数调用，通过该函数可以计算出一个效果很不错的二值化阈值。
//    }
  uint8 i, j = 0;
  for (i = 0; i < LCDH; i++)                                //遍历二维数组的每一行
  {
    for (j = 0; j < LCDW; j++)                              //遍历二维数组的每一列
    {
      if (*Image_Use[i][j] > Threshold)                      //如果这个点的灰度值大于阈值Threshold
          Pixle[i][j] = Image_white;                                  //那么这个像素点就记为白点
      else                                                  //如果这个点的灰度值小于阈值Threshold
          Pixle[i][j] = Image_black;                                  //那么这个像素点就记为黑点
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Get_Border_And_SideType
//  @brief          得到边线和边线的类型，我在这里给边线分为了三种类型，T类型、W类型和H类型。分别代表正常边线、无边边线和大跳变边线。
//  @brief          和上面一样，要想看懂我的逻辑，前提是要搞懂T、W、H三种类型的边线到底是什么样子的。
//  @parameter      p        指向传进来数组的一个指针变量。
//  @parameter      type     只能是L或者是R，分别代表扫左边线和扫右边线。
//  @parameter      L        扫描的区间下限 ，也就是从哪一列开始扫。
//  @parameter      H        扫描的区间上限 ，也就是一直扫到哪一列。
//  @parameter      Q        是一个结构体指针变量，自己跳过去看看这个结构体里面的成员。
//  @time           2022年2月20日
//  @Author
//  Sample usage:   Get_SideType_And_Border(PicTemp, 'R', IntervalLow, IntervalHigh,&JumpPoint[1]);
//  Sample usage:   从PicTemp(PicTemp是个指针，指向一个数组)的IntervalLow列开始扫，扫到IntervalHigh列，然后把得到的边线所在的列和边线类型记录到JumpPoint结构体中。
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void Get_Border_And_SideType(uint8* p,uint8 type,int L,int H,JumpPointtypedef* Q)
{
  int i = 0;
  if (type == 'L')                              //如果Type是L(Left),则扫描左边线。
  {
    for (i = H; i >= L; i--)                    //从右往左扫
    {
      if (*(p + i) == Image_white && *(p + i - 1) != Image_white)   //如果有黑白跳变    1是白 0是黑
      {
        Q->point = i;                           //那就把这个列记录下来作为左边线
        Q->type = 'T';                          //并且把这一行当作是正常跳变，边线类型记为T，即边线正常。
        break;                                  //找到了就跳出循环不找了
      }
      else if (i == (L + 1))                    //要是扫到最后都没找到
      {
        if (*(p + (L + H) / 2) != 0)            //并且扫描区间的中间是白像素点
        {
          Q->point = (L + H) / 2;               //那么就认为这一行的左边线是传进来扫描区间的中点。
          Q->type = 'W';                        //并且把这一行当作是非正常跳变，边线类型记为W，即无边行。
          break;                                //跳出循环不找了
        }
        else                                    //要是扫到最后都没找到，并且扫描区间的中间是黑像素点
        {
          Q->point = H;                         //那么就认为这一行的左边线是传进来扫描区间的区间上限。
          Q->type = 'H';                        //并且也把这一行当作是非正常跳变，不过边线类型记为H，即大跳变行。
          break;                                //跳出循环不找了
        }
      }
    }
  }
  else if (type == 'R')                         //如果Type是R(Right),则扫描右边线。
  {
    for (i = L; i <= H; i++)                    //从左往右扫
    {
      if (*(p + i) == Image_white && *(p + i + 1) != Image_white)   //如果有黑白跳变    1是白 0是黑
      {
        Q->point = i;                           //那就把这个列记录下来作为右边线
        Q->type = 'T';                          //并且把这一行当作是正常跳变，边线类型记为T，即边线正常。
        break;                                  //找到了就跳出循环不找了
      }
      else if (i == (H - 1))                    //要是扫到最后都没找到
      {
        if (*(p + (L + H) / 2) != 0)            //并且扫描区间的中间是白像素点
        {
          Q->point = (L + H) / 2;               //那么就认为这一行的右边线是传进来扫描区间的中点。
          Q->type = 'W';                        //并且把这一行当作是非正常跳变，边线类型记为W，即无边行。
          break;
        }
        else                                    //要是扫到最后都没找到，并且扫描区间的中间是黑像素点
        {
          Q->point = L;                         //那么就认为这一行的右边线是传进来扫描区间的区间下限。
          Q->type = 'H';                        //并且也把这一行当作是非正常跳变，不过边线类型记为H，即大跳变行。
          break;                                //跳出循环不找了
        }
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Get_BaseLine
//  @brief          用遍历的方法得到图像最底下五行（59-55行）的边线和中线信息。这五行边线和中线信息的准确度非常的重要，直接影响到整幅图像的处理结果。
//  @brief          Get_BaseLine函数是下面Get_AllLine函数的基础和前提，Get_AllLine函数是以Get_BaseLine为基础的。看名字应该也能看出对吧，一个叫得到基础线，一个叫得到所有线。
//  @brief          Get_BaseLine函数和Get_AllLine函数加在一起，也就组成了我优化之后的搜边线算法。
//  @parameter      void
//  @time           2022年2月21日
//  @Author
//  Sample usage:   Get_BaseLine();
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void Get_BaseLine(void)
{
    /**************************************遍历搜索图像最底行（59行）左右边线从而确定中线的过程 ********************************************************************/
    /****************************************************Begin*****************************************************************************/

    PicTemp = Pixle[59];                                                //让PicTemp这个指针变量指向图像数组的Pixle[59]
    for (Xsite = ImageSensorMid; Xsite < 93; Xsite++)                   //假设39是中心列，从中心列开始一列一列的往右边搜索右边线
    {
      if (*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite + 1) == 0)       //如果连续出现了两个黑点，说没找到了边线。
      {
        BottomBorderRight = Xsite;                                      //把这一列记录下来作为这一行的右边线
        break;                                                          //跳出循环
      }
      else if (Xsite == 92)                                             //如果找到了第58列都还没出现黑点，说明这一行的边线有问题。
      {
        BottomBorderRight = 93;                                         //所以我这里的处理就是，直接假设图像最右边的那一列（第79列）就是这一行的右边线。
        break;                                                          //跳出循环
      }
    }

    for (Xsite = ImageSensorMid; Xsite > 0; Xsite--)                    //假设39是中心列，从中心列开始一列一列的往左边搜索左边线
    {
      if (*(PicTemp + Xsite) == 0 && *(PicTemp + Xsite - 1) == 0)       //如果连续出现了两个黑点，说没找到了边线。
      {
        BottomBorderLeft = Xsite;                                       //把这一列记录下来作为这一行的左边线
        break;                                                          //跳出循环
      }
      else if (Xsite == 1)                                              //如果找到了第1列都还没出现黑点，说明这一行的边线有问题。
      {
        BottomBorderLeft = 0;                                           //所以我这里的处理就是，直接假设图像最左边的那一列（第0列）就是这一行的左边线。
        break;                                                          //跳出循环
      }
    }

    BottomCenter =(BottomBorderLeft + BottomBorderRight) / 2;           //根据左右边界计算出第59行的中线
    ImageDeal[59].LeftBorder = BottomBorderLeft;                        //把第59行的左边界存储进数组，注意看ImageDeal这个数字的下标，是不是正好对应59。
    ImageDeal[59].RightBorder = BottomBorderRight;                      //把第59行的右边界存储进数组，注意看ImageDeal这个数字的下标，是不是正好对应59。
    ImageDeal[59].Center = BottomCenter;                                //把第59行的中线存储进数组，    注意看ImageDeal这个数字的下标，是不是正好对应59。
    ImageDeal[59].Wide = BottomBorderRight - BottomBorderLeft;          //把第59行的赛道宽度存储数组，注意看ImageDeal这个数字的下标，是不是正好对应59。
    ImageDeal[59].IsLeftFind = 'T';                                     //记录第59行的左边线类型为T，即正常找到左边线。
    ImageDeal[59].IsRightFind = 'T';                                    //记录第59行的右边线类型为T，即正常找到右边线。

    /****************************************************End*******************************************************************************/
    /**************************************遍历搜索图像最底行（59行）左右边线从而确定中线的过程 ********************************************************************/
    /**************************************在第59行中线已经确定的情况下确定58-54这四行中线的过程 ******************************************/
    /****************************************************Begin*****************************************************************************/
    /*
         * 下面几行的的搜线过程我就不再赘述了，根据我的注释把第59行的搜线过程理解好，
         * 那么58到54行的搜线就完全没问题，是一模一样的逻辑和过程。
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
    /**************************************在第59行中线已经确定的情况下确定58-54这四行中线的过程 ****************************************/
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Get_AllLine
//  @brief          在Get_BaseLine的基础上，针对部分特殊情况，利用一些特殊的处理算法得到剩余行的边线和中线信息。
//  @brief          这个函数应该是目前为止我代码里面最难理解的一部分了，一定要花大量时间静下心来去看，看的时候脑子里面要有那个二值化黑白图像。
//  @brief          如果你能一边想着二值化黑白图像，一边来思考我代码的逻辑的话，很多地方你就能慢慢理解了，不要光盯着那个代码一直看啊，那样是没用的，切忌！
//  @brief          多动脑思考，我能想出来，你们肯定也可以的。这个过程会很枯燥，但是你都理解清楚了之后，你的车车就已经可以跑直道和弯道了。
//  @parameter      void
//  @time           2023年2月21日
//  @Author
//  Sample usage:   Get_AllLine();
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void Get_AllLine(void)
{
  uint8 L_Found_T  = 'F';    //确定无边斜率的基准有边行是否被找到的标志
  uint8 Get_L_line = 'F';    //找到这一帧图像的基准左斜率，为什么这里要置为F，看了下面的代码就知道了。
  uint8 R_Found_T  = 'F';    //确定无边斜率的基准有边行是否被找到的标志
  uint8 Get_R_line = 'F';    //找到这一帧图像的基准右斜率，为什么这里要置为F，看了下面的代码就知道了。
  float D_L = 0;             //左边线延长线的斜率
  float D_R = 0;             //右边线延长线的斜率
  int ytemp_W_L;             //记住首次左丢边行
  int ytemp_W_R;             //记住首次右丢边行
  ExtenRFlag = 0;            //标志位清0
  ExtenLFlag = 0;            //标志位清0
  ImageStatus.OFFLine=2;     //这个结构体成员我之所以在这里赋值，是因为我ImageStatus结构体里面的成员太多了，但是暂时又只用到了OFFLine，所以我在哪用到它就在哪赋值。
  ImageStatus.Miss_Right_lines = 0;
  ImageStatus.WhiteLine = 0;
  ImageStatus.Miss_Left_lines = 0;
  for (Ysite = 54 ; Ysite > ImageStatus.OFFLine; Ysite--)                            //前5行在Get_BaseLine()中已经处理过了，现在从55行处理到自己设定的不处理行OFFLine。
  {                                                                                  //因为太前面的图像可靠性不搞，所以OFFLine的设置很有必要，没必要一直往上扫到第0行。
    PicTemp = Pixle[Ysite];
    JumpPointtypedef JumpPoint[2];                                                   // JumpPoint[0]代表左边线，JumpPoint[1]代表右边线。

  /******************************扫描本行的右边线******************************/
    IntervalLow  = ImageDeal[Ysite + 1].RightBorder  - ImageScanInterval;               //从上一行的右边线加减Interval对应的列开始扫描本行，Interval一般取5，当然你为了保险起见可以把这个值改的大一点。
    IntervalHigh = ImageDeal[Ysite + 1].RightBorder + ImageScanInterval;              //正常情况下只需要在上行边线左右5的基础上（差不多10列的这个区间）去扫线，一般就能找到本行的边线了，所以这个值其实不用太大。
    LimitL(IntervalLow);                                                             //这里就是对传给GetJumpPointFromDet()函数的扫描区间进行一个限幅操作。
    LimitH(IntervalHigh);                                                            //假如上一行的边线是第2列，那你2-5=-3，-3是不是就没有实际意义了？怎么会有-3列呢？
    Get_Border_And_SideType(PicTemp, 'R', IntervalLow, IntervalHigh,&JumpPoint[1]);  //扫线用的一个子函数，自己跳进去看明白逻辑。
  /******************************扫描本行的右边线******************************/

  /******************************扫描本行的左边线******************************/
    IntervalLow =ImageDeal[Ysite + 1].LeftBorder  -ImageScanInterval;                //从上一行的左边线加减Interval对应的列开始扫描本行，Interval一般取5，当然你为了保险起见可以把这个值改的大一点。
    IntervalHigh =ImageDeal[Ysite + 1].LeftBorder +ImageScanInterval;                //正常情况下只需要在上行边线左右5的基础上（差不多10列的这个区间）去扫线，一般就能找到本行的边线了，所以这个值其实不用太大。
    LimitL(IntervalLow);                                                             //这里就是对传给GetJumpPointFromDet()函数的扫描区间进行一个限幅操作。
    LimitH(IntervalHigh);                                                            //假如上一行的边线是第2列，那你2-5=-3，-3是不是就没有实际意义了？怎么会有-3列呢？
    Get_Border_And_SideType(PicTemp, 'L', IntervalLow, IntervalHigh,&JumpPoint[0]);  //扫线用的一个子函数，自己跳进去看明白逻辑。
  /******************************扫描本行的左边线******************************/

  /*
       下面的代码要是想看懂，想搞清楚我到底在赣神魔的话，
        请务必把GetJumpPointFromDet()这个函数的逻辑看懂，
        尤其是在这个函数里面，“T”、“W”、“H”三个标志代表什么，
        一定要搞懂!!!不然的话，建议不要往下看了，不要折磨自己!!!
  */
    if (JumpPoint[0].type =='W')                                                     //如果本行的左边线属于不正常跳变，即这10个点都是白点。
    {
      ImageDeal[Ysite].LeftBorder =ImageDeal[Ysite + 1].LeftBorder;                  //那么本行的左边线就采用上一行的边线。
    }
    else                                                                             //如果本行的左边线属于T或者是H类别
    {
      ImageDeal[Ysite].LeftBorder = JumpPoint[0].point;                              //那么扫描到的边线是多少，我就记录下来是多少。
    }

    if (JumpPoint[1].type == 'W')                                                    //如果本行的右边线属于不正常跳变，即这10个点都是白点。
    {
      ImageDeal[Ysite].RightBorder =ImageDeal[Ysite + 1].RightBorder;                //那么本行的右边线就采用上一行的边线。
    }
    else                                                                             //如果本行的右边线属于T或者是H类别
    {
      ImageDeal[Ysite].RightBorder = JumpPoint[1].point;                             //那么扫描到的边线是多少，我就记录下来是多少。
    }

    ImageDeal[Ysite].IsLeftFind =JumpPoint[0].type;                                  //记录本行找到的左边线类型，是T？是W？还是H？这个类型后面是有用的，因为我还要进一步处理。
    ImageDeal[Ysite].IsRightFind = JumpPoint[1].type;                                //记录本行找到的右边线类型，是T？是W？还是H？这个类型后面是有用的，因为我还要进一步处理。
  /*
        下面就开始对W和H类型的边线分别进行处理， 为什么要处理？
        如果你看懂了GetJumpPointFromDet函数逻辑，明白了T W H三种类型分别对应什么情况，
        那你就应该知道W和H类型的边线都属于非正常类型，那我是不是要处理？
        这一部分的处理思路需要自己花大量时间好好的去琢磨，我在注释这里没法给你说清楚的。，
        实在想不通就来问我吧！
  */
    /************************************重新确定大跳变(即H类)的边界*************************************/

    if (( ImageDeal[Ysite].IsLeftFind == 'H' || ImageDeal[Ysite].IsRightFind == 'H'))
    {
      /**************************处理左边线的大跳变***************************/
      if (ImageDeal[Ysite].IsLeftFind == 'H')
      {
        for (Xsite = (ImageDeal[Ysite].LeftBorder + 1); Xsite <= (ImageDeal[Ysite].RightBorder - 1); Xsite++)                                                           //左右边线之间重新扫描
        {
          if ((*(PicTemp + Xsite) == 0) && (*(PicTemp + Xsite + 1) != 0))//黑白跳变
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
      /**************************处理左边线的大跳变***************************/


      /**************************处理右边线的大跳变***************************/
      if (ImageDeal[Ysite].IsRightFind == 'H')
      {
        for (Xsite = (ImageDeal[Ysite].RightBorder - 1);Xsite >= (ImageDeal[Ysite].LeftBorder + 1); Xsite--)
        {
          if ((*(PicTemp + Xsite) == 0) && (*(PicTemp + Xsite - 1) != 0))//黑白跳变
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
    /**************************处理右边线的大跳变***************************/

  /*****************************重新确定大跳变的边界******************************/



 /************************************重新确定无边行（即W类）的边界****************************************************************/
    int ysite = 0;
    uint8 L_found_point = 0;
    uint8 R_found_point = 0;
    /**************************处理左边线的无边行***************************/
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
        ImageDeal[Ysite].RightBorder =ImageDeal[ytemp_W_R].RightBorder -D_R * (ytemp_W_R - Ysite);  //如果找到了 那么以基准行做延长线
      }
      LimitL(ImageDeal[Ysite].RightBorder);  //限幅
      LimitH(ImageDeal[Ysite].RightBorder);  //限幅
    }
    /**************************处理左边线的无边行***************************/


    /**************************处理右边线的无边行***************************/
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
        if (L_found_point > 8)              //找到基准斜率边  做延长线重新确定无边
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

      LimitL(ImageDeal[Ysite].LeftBorder);  //限幅
      LimitH(ImageDeal[Ysite].LeftBorder);  //限幅
    }

    /**************************处理右边线的无边行***************************/
    /************************************重新确定无边行（即W类）的边界****************************************************************/


    /************************************都处理完之后，其他的一些数据整定操作*************************************************/
      if (ImageDeal[Ysite].IsLeftFind == 'W' && ImageDeal[Ysite].IsRightFind == 'W')
      {
          ImageStatus.WhiteLine++;  //要是左右都无边，丢边数+1
      }
     if (ImageDeal[Ysite].IsLeftFind == 'W'&&Ysite<55)
     {
          ImageStatus.Miss_Left_lines++;
     }
     if (ImageDeal[Ysite].IsRightFind == 'W'&&Ysite<55)
     {
          ImageStatus.Miss_Right_lines++;
     }

      LimitL(ImageDeal[Ysite].LeftBorder);   //限幅
      LimitH(ImageDeal[Ysite].LeftBorder);   //限幅
      LimitL(ImageDeal[Ysite].RightBorder);  //限幅
      LimitH(ImageDeal[Ysite].RightBorder);  //限幅

      ImageDeal[Ysite].Wide = ImageDeal[Ysite].RightBorder - ImageDeal[Ysite].LeftBorder;
      ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;

      if (ImageDeal[Ysite].Wide <= 7)        //宽度阈值可调
      {
          ImageStatus.OFFLine = Ysite + 1;
          break;
      }
      else if (ImageDeal[Ysite].RightBorder <= 10 || ImageDeal[Ysite].LeftBorder >= 70)
      {
          ImageStatus.OFFLine = Ysite + 1;
          break;
      }
      /************************************都处理完之后，其他的一些数据整定操作*************************************************/
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Get_ExtensionLine
//  @brief          这个函数的作用就是补线！
//  @brief          十字路口那里，按理来说车得直行对吧，但是这种情况摄像头扫线的时候，是不是会出现扫不到边线的情况？因为那几行都是白色的嘛，找不到黑白跳变点。
//  @brief          所以按照上面的搜边线算法，如果我们不对这种情况做算法处理的话，那我那些行的左右边界是不是就不对了？对应的中线是不是也不对了？那你能保证小车还直行嘛？
//  @brief          显然保证不了，所以这个时候小车可能就会根据算出来的中线，直接左转或者右转了，是不是违背比赛规则了，那是不是就寄了？所以说补线是非常重要的一环。
//  @parameter      void
//  @time           2023年2月21日
//  @Author
//  Sample usage:   Get_ExtensionLine();
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void Get_ExtensionLine(void)
{
    //ImageStatus.OFFLine=5;                                                  //这个结构体成员我之所以在这里赋值，是因为我ImageStatus结构体里面的成员太多了，但是暂时又只用到了OFFLine，所以我在哪用到它就在哪赋值。
    /************************************左边线的补线处理*************************************************/
    if (ImageStatus.WhiteLine >= 8)                                       //如果丢边行的数量大于8
        TFSite = 55;                                                      //那就给TFSite赋值为55，这个变量是待会算补线斜率的一个变量。
    left_FTSite=0;
    right_FTSite=0;
    if (ExtenLFlag != 'F')                                                //如果ExtenLFlag标志量不等于F，那就开始进行补线操作。
        for (Ysite = 54; Ysite >= (ImageStatus.OFFLine + 4);Ysite--)        //从第54开始往上扫，一直扫到顶边下面几行。
        {
            PicTemp = Pixle[Ysite];
            if (ImageDeal[Ysite].IsLeftFind =='W')                            //如果本行的左边线类型是W类型，也就是无边行类型。
            {
                if (ImageDeal[Ysite + 1].LeftBorder >= 80)                      //如果左边界到了第70列右边去了，那大概率就是极端情况，说明已经快寄了。
                {
                  ImageStatus.OFFLine = Ysite + 1;                              //这种情况最好的处理方法就是不处理，直接跳出循环。
                  break;
                }
                while (Ysite >= (ImageStatus.OFFLine + 4))                      //如果左边界正常，那就进入while循环卡着，直到满足循环结束条件。
                {
                    Ysite--;                                                      //行数减减
                    if (ImageDeal[Ysite].IsLeftFind == 'T'
                      &&ImageDeal[Ysite - 1].IsLeftFind == 'T'
                      &&ImageDeal[Ysite - 2].IsLeftFind == 'T'
                      &&ImageDeal[Ysite - 2].LeftBorder > 0
                      &&ImageDeal[Ysite - 2].LeftBorder <80
                      )                                                         //如果扫到的无边行的上面连续三行都是正常边线
                    {
                      left_FTSite = Ysite - 2;                                  //那就把扫到的这一行的上面两行存入FTsite变量
                    break;                                                      //跳出while循环
                    }
                }
                DetL =((float)(ImageDeal[left_FTSite].LeftBorder - ImageDeal[TFSite].LeftBorder)) /((float)(left_FTSite - TFSite));  //计算左边线的补线斜率
                if (left_FTSite > ImageStatus.OFFLine)                              //如果FTSite存储的那一行在图像顶边OFFline的下面
                    for (ytemp = TFSite; ytemp >= left_FTSite; ytemp--)               //那么我就从第一次扫到的左边界的下面第二行的位置开始往上一直补线，补到FTSite行。
                    {
//                        LimitL((int)(DetL * ((float)(ytemp - TFSite))) + ImageDeal[TFSite].LeftBorder);//（ydc）自己写的限幅，如果不用来显示其实也没必要限幅
//                        LimitH((int)(DetL * ((float)(ytemp - TFSite))) + ImageDeal[TFSite].LeftBorder);
                        ImageDeal[ytemp].LeftBorder = (int)(DetL * ((float)(ytemp - TFSite))) +ImageDeal[TFSite].LeftBorder;     //这里就是具体的补线操作了
                    }
            }
            else                                                              //注意看清楚这个else和哪个if是一对，搞清楚逻辑关系。
                TFSite = Ysite + 2;                                             //这里为什么要Ysite+2，我没法在注释里面讲清楚，自己领会吧。
        }
    /************************************左边线的补线处理*************************************************/


    /************************************右边线的补线处理（跟左边线处理思路一模一样）注释略*************************************************/
    if (ImageStatus.WhiteLine >= 8)
    TFSite = 55;
    if (ExtenRFlag != 'F')
    for (Ysite = 54; Ysite >= (ImageStatus.OFFLine + 4);Ysite--)
    {
      PicTemp = Pixle[Ysite];  //存当前行
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
//              LimitL((int)(DetR * ((float)(ytemp - TFSite))) +ImageDeal[TFSite].RightBorder);//（ydc）自己写的限幅，如果不用来显示其实也没必要限幅
//              LimitH((int)(DetR * ((float)(ytemp - TFSite))) +ImageDeal[TFSite].RightBorder);
              ImageDeal[ytemp].RightBorder = (int)(DetR * ((float)(ytemp - TFSite))) +ImageDeal[TFSite].RightBorder;
          }
      }
      else
        TFSite =Ysite +2;
    }
      /************************************右边线的补线处理（跟左边线处理思路一模一样）注释略*************************************************/
}

/*上交大左右手法则扫线，作为处理圆环等判断元素的第二依据*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Search_Bottom_Line_OTSU
//  @brief          获取底层左右边线
//  @param          imageInput[IMAGE_ROW][IMAGE_COL]        传入的图像数组
//  @param          Row                                     图像的Ysite
//  @param          Col                                     图像的Xsite
//  @return         Bottonline                              底边行选择
//  @time           2022年10月9日
//  @Author         陈新云
//  Sample usage:   Search_Bottom_Line_OTSU(imageInput, Row, Col, Bottonline);
//--------------------------------------------------------------------------------------------------------------------------------------------

void Search_Bottom_Line_OTSU(uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{

    //寻找左边边界
    for (int Xsite = Col / 2-2; Xsite > 1; Xsite--)
    {
        if (imageInput[Bottonline][Xsite] == Image_white && imageInput[Bottonline][Xsite - 1] == 0)
        {
            ImageDeal[Bottonline].LeftBoundary = Xsite;//获取底边左边线
            break;
        }
    }
    for (int Xsite = Col / 2+2; Xsite < LCDW-1; Xsite++)
    {
        if (imageInput[Bottonline][Xsite] == Image_white && imageInput[Bottonline][Xsite + 1] == 0)
        {
            ImageDeal[Bottonline].RightBoundary = Xsite;//获取底边右边线
            break;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Search_Left_and_Right_Lines
//  @brief          通过sobel提取左右边线
//  @param          imageInput[IMAGE_ROW][IMAGE_COL]        传入的图像数组
//  @param          Row                                     图像的Ysite
//  @param          Col                                     图像的Xsite
//  @param          Bottonline                              底边行选择
//  @return         无
//  @time           2022年10月7日
//  @Author         陈新云
//  Sample usage:   Search_Left_and_Right_Lines(imageInput, Row, Col, Bottonline);
//--------------------------------------------------------------------------------------------------------------------------------------------

void Search_Left_and_Right_Lines(uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{
    //定义小人的当前行走状态位置为 上 左 下 右 一次要求 上：左边为黑色 左：上边为褐色 下：右边为色  右：下面有黑色
/*  前进方向定义：
                *   0
                * 3   1
                *   2
*/
/*寻左线坐标规则*/
    uint8 Left_Rule[2][8] = {
                                  {0,-1,1,0,0,1,-1,0 },//{0,-1},{1,0},{0,1},{-1,0},  (x,y )
                                  {-1,-1,1,-1,1,1,-1,1} //{-1,-1},{1,-1},{1,1},{-1,1}
    };
    /*寻右线坐标规则*/
    int Right_Rule[2][8] = {
                              {0,-1,1,0,0,1,-1,0 },//{0,-1},{1,0},{0,1},{-1,0},
                              {1,-1,1,1,-1,1,-1,-1} //{1,-1},{1,1},{-1,1},{-1,-1}
    };
    int num=0;
    uint8 Left_Ysite = Bottonline;
    uint8 Left_Xsite = (uint8)ImageDeal[Bottonline].LeftBoundary;
    uint8 Left_Rirection = 0;//左边方向
    uint8 Pixel_Left_Ysite = Bottonline;
    uint8 Pixel_Left_Xsite = 0;

    uint8 Right_Ysite = Bottonline;
    uint8 Right_Xsite = (uint8)ImageDeal[Bottonline].RightBoundary;
    uint8 Right_Rirection = 0;//右边方向
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
        /*********左边巡线*******/
        if ((Pixel_Left_Ysite > Ysite) || Ysite == ImageStatus.OFFLineBoundary)//右边扫线
        {
            /*计算前方坐标*/
            Pixel_Left_Ysite = Left_Ysite + Left_Rule[0][2 * Left_Rirection + 1];
            Pixel_Left_Xsite = Left_Xsite + Left_Rule[0][2 * Left_Rirection];

            if (imageInput[Pixel_Left_Ysite][Pixel_Left_Xsite] == 0)//前方是黑色
            {
                //顺时针旋转90
                if (Left_Rirection == 3)
                    Left_Rirection = 0;
                else
                    Left_Rirection++;
            }
            else//前方是白色
            {
                /*计算左前方坐标*/
                Pixel_Left_Ysite = Left_Ysite + Left_Rule[1][2 * Left_Rirection + 1];
                Pixel_Left_Xsite = Left_Xsite + Left_Rule[1][2 * Left_Rirection];

                if (imageInput[Pixel_Left_Ysite][Pixel_Left_Xsite] == 0)//左前方为黑色
                {
                    //方向不变  Left_Rirection
                    Left_Ysite = Left_Ysite + Left_Rule[0][2 * Left_Rirection + 1];
                    Left_Xsite = Left_Xsite + Left_Rule[0][2 * Left_Rirection];
                    if (ImageDeal[Left_Ysite].LeftBoundary_First == 0)
                        ImageDeal[Left_Ysite].LeftBoundary_First = Left_Xsite;
                    ImageDeal[Left_Ysite].LeftBoundary = Left_Xsite;
                }
                else//左前方为白色
                {
                    // 方向发生改变 Left_Rirection  逆时针90度
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
        /*********右边巡线*******/
        if ((Pixel_Right_Ysite > Ysite) || Ysite == ImageStatus.OFFLineBoundary)//右边扫线
        {
            /*计算前方坐标*/
            Pixel_Right_Ysite = Right_Ysite + Right_Rule[0][2 * Right_Rirection + 1];
            Pixel_Right_Xsite = Right_Xsite + Right_Rule[0][2 * Right_Rirection];

            if (imageInput[Pixel_Right_Ysite][Pixel_Right_Xsite] == 0)//前方是黑色
            {
                //逆时针旋转90
                if (Right_Rirection == 0)
                    Right_Rirection = 3;
                else
                    Right_Rirection--;
            }
            else//前方是白色
            {
                /*计算右前方坐标*/
                Pixel_Right_Ysite = Right_Ysite + Right_Rule[1][2 * Right_Rirection + 1];
                Pixel_Right_Xsite = Right_Xsite + Right_Rule[1][2 * Right_Rirection];

                if (imageInput[Pixel_Right_Ysite][Pixel_Right_Xsite] == 0)//左前方为黑色
                {
                    //方向不变  Right_Rirection
                    Right_Ysite = Right_Ysite + Right_Rule[0][2 * Right_Rirection + 1];
                    Right_Xsite = Right_Xsite + Right_Rule[0][2 * Right_Rirection];
                    if (ImageDeal[Right_Ysite].RightBoundary_First == 93)
                        ImageDeal[Right_Ysite].RightBoundary_First = Right_Xsite;
                    ImageDeal[Right_Ysite].RightBoundary = Right_Xsite;
                }
                else//左前方为白色
                {
                    // 方向发生改变 Right_Rirection  逆时针90度
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
        if (abs(Pixel_Right_Xsite - Pixel_Left_Xsite) < 3)//Ysite<80是为了放在底部是斑马线扫描结束  3 && Ysite < 30
        {
            ImageStatus.OFFLineBoundary = Ysite;
            break;
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//  @name           Search_Border_OTSU
//  @brief          通过OTSU获取边线 和信息
//  @param          imageInput[IMAGE_ROW][IMAGE_COL]        传入的图像数组
//  @param          Row                                     图像的Ysite
//  @param          Col                                     图像的Xsite
//  @param          Bottonline                              底边行选择
//  @return         无
//  @time           2022年10月7日
//  @Author         陈新云
//  Sample usage:   Search_Border_OTSU(mt9v03x_image, IMAGE_ROW, IMAGE_COL, IMAGE_ROW-8);
//--------------------------------------------------------------------------------------------------------------------------------------------

void Search_Border_OTSU(uint8 imageInput[LCDH][LCDW], uint8 Row, uint8 Col, uint8 Bottonline)
{
    ImageStatus.WhiteLine_L = 0;
    ImageStatus.WhiteLine_R = 0;
    //ImageStatus.OFFLine = 1;
    /*封上下边界处理*/
    for (int Xsite = 0; Xsite < LCDW; Xsite++)
    {
        imageInput[0][Xsite] = 0;
        imageInput[Bottonline + 1][Xsite] = 0;
    }
    /*封左右边界处理*/
    for (int Ysite = 0; Ysite < LCDH; Ysite++)
    {
            ImageDeal[Ysite].LeftBoundary_First = 0;
            ImageDeal[Ysite].RightBoundary_First = 93;

            imageInput[Ysite][0] = 0;
            imageInput[Ysite][LCDW - 1] = 0;
    }
    /********获取底部边线*********/
    Search_Bottom_Line_OTSU(imageInput, Row, Col, Bottonline);
    /********获取左右边线*********/
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
//  @brief          整个图像判断的子函数，用来判断左圆环类型.
//  @parameter      void
//  @time
//  @Author         MRCHEN
//  Sample usage:   Element_Judgment_Left_Rings();
//--------------------------------------------------------------
void Element_Judgment_Left_Rings()//这里有个缺陷就是弯道接环岛，可能找不到右下角的突变点
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
    for (int Ysite = 58; Ysite > ring_ysite; Ysite--)//八邻域两次爬线的拐点判断
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
//  @brief          整个图像判断的子函数，用来判断右圆环类型.
//  @parameter      void
//  @time
//  @Author         MRCHEN
//  Sample usage:   Element_Judgment_Right_Rings();
//--------------------------------------------------------------
void Element_Judgment_Right_Rings()//这里有个缺陷就是弯道接环岛，可能找不到右下角的突变点
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
        if (ImageDeal[Ysite - 1].RightBoundary_First - ImageDeal[Ysite].RightBoundary_First > 4)//向右突变，这个是进环岛右下角的突变点
        {
            Right_RingsFlag_Point1_Ysite = Ysite;
            break;
        }
    }
    for (int Ysite = 58; Ysite > ring_ysite; Ysite--)
    {
        if (ImageDeal[Ysite].RightBoundary - ImageDeal[Ysite + 1].RightBoundary > 4)//向左突变的点，这个是环岛入口处的突变点
        {
            Right_RingsFlag_Point2_Ysite = Ysite;
            break;
        }
    }
    for (int Ysite = Right_RingsFlag_Point1_Ysite; Ysite > 10; Ysite--)//找圆弧上的点
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
    if(Right_RingsFlag_Point2_Ysite > Right_RingsFlag_Point1_Ysite+3 && Ring_Help_Flag == 0)//应该是保险措施
    {
        if(ImageStatus.Miss_Right_lines>10)
            Ring_Help_Flag = 1;
    }
    if (Right_RingsFlag_Point2_Ysite > Right_RingsFlag_Point1_Ysite+3 && Ring_Help_Flag == 1)
    {
        ImageFlag.image_element_rings = 2;
        ImageFlag.image_element_rings_flag = 1;
        ImageFlag.ring_big_small=1;     //小环
    }
    Ring_Help_Flag = 0;
}

//--------------------------------------------------------------
//  @name           Element_Handle_Left_Rings()
//  @brief          整个图像处理的子函数，用来处理左圆环类型.
//  @parameter      void
//  @time
//  @Author         MRCHEN
//  Sample usage:   Element_Handle_Left_Rings();
//-------------------------------------------------------------
void Element_Handle_Left_Rings()
{
    /***************************************判断**************************************/
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
    //准备进环
    if (ImageFlag.image_element_rings_flag == 1 && num>10)
    {
        ImageFlag.image_element_rings_flag = 2;
    }
    if (ImageFlag.image_element_rings_flag == 2 && num<5)
    {
        ImageFlag.image_element_rings_flag = 5;
    }
    //进环
    if(ImageFlag.image_element_rings_flag == 5 && ImageStatus.Miss_Right_lines>15)
    {
        ImageFlag.image_element_rings_flag = 6;
    }
        //进环小圆环
    if(ImageFlag.image_element_rings_flag == 6 && ImageStatus.Miss_Right_lines<7)
    {
        ImageFlag.image_element_rings_flag = 7;
    }
    //环内 大圆环判断
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
    //出环后
    if (ImageFlag.image_element_rings_flag == 8)
    {
        if (    Straight_Judge(2, ImageStatus.OFFLine+15, 45) < 1
             && ImageStatus.Miss_Right_lines < 8
             && ImageStatus.OFFLine < 13)    //右边为直线且截止行（前瞻值）很小
            ImageFlag.image_element_rings_flag = 9;
    }
    //结束圆环进程
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
    /***************************************处理**************************************/
        //准备进环  半宽处理
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
    //进环  补线
    if( ImageFlag.image_element_rings_flag == 5
        ||ImageFlag.image_element_rings_flag == 6)
    {
        int  flag_Xsite_1=0;
        int flag_Ysite_1=0;
        float Slope_Rings=0;
        for(Ysite=55;Ysite>ImageStatus.OFFLine;Ysite--)//下面弧点
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
        //补线
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
            for(Ysite=flag_Ysite_1-1;Ysite>10;Ysite--) //A点上方进行扫线
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
    //环内 小环弯道减半宽 大环不减
    if (ImageFlag.image_element_rings_flag == 7)
    {

    }
    //大圆环出环 补线
    if (ImageFlag.image_element_rings_flag == 8)    //大圆环
    {
        Repair_Point_Xsite = 20;
        Repair_Point_Ysite = 0;
        for (int Ysite = 40; Ysite > 5; Ysite--)
        {
            if (Pixle[Ysite][28] == Image_white && Pixle[Ysite-1][28] == 0)//30
            {
                Repair_Point_Xsite = 28;
                Repair_Point_Ysite = Ysite-1;
                ImageStatus.OFFLine = Ysite + 1;  //截止行重新规划
                break;
            }
        }
        for (int Ysite = 57; Ysite > Repair_Point_Ysite-3; Ysite--)         //补线
        {
            ImageDeal[Ysite].RightBorder = (ImageDeal[58].RightBorder - Repair_Point_Xsite) * (Ysite - 58) / (58 - Repair_Point_Ysite)  + ImageDeal[58].RightBorder;
            ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;
        }
    }
    //已出环 半宽处理
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
//  @brief          整个图像处理的子函数，用来处理右圆环类型.
//  @parameter      void
//  @time
//  @Author         MRCHEN
//  Sample usage:   Element_Handle_Right_Rings();
//-------------------------------------------------------------
void Element_Handle_Right_Rings()
{
    /****************判断*****************/
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
    //准备进环
    if (ImageFlag.image_element_rings_flag == 1 && num>10)
    {
        ImageFlag.image_element_rings_flag = 2;
    }
    if (ImageFlag.image_element_rings_flag == 2 && num<5)
    {
        ImageFlag.image_element_rings_flag = 5;
    }
    //进环
    if(ImageFlag.image_element_rings_flag == 5 && ImageStatus.Miss_Left_lines>15)
    {
        ImageFlag.image_element_rings_flag = 6;
    }
    //进环小圆环
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
    //出环后
    if (ImageFlag.image_element_rings_flag == 8)
    {
         if (   Straight_Judge(1, ImageStatus.OFFLine+15, 45) < 1
             && ImageStatus.Miss_Left_lines < 8
             && ImageStatus.OFFLine < 10)    //右边为直线且截止行（前瞻值）很小
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
    /***************************************处理**************************************/
         //准备进环  半宽处理
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
    //进环  补线
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
        //补线
        if(flag_Ysite_1!=0)
        {
            for(Ysite=flag_Ysite_1;Ysite<60;Ysite++)
            {
                ImageDeal[Ysite].LeftBorder=flag_Xsite_1+Slope_Right_Rings*(Ysite-flag_Ysite_1);
                ImageDeal[Ysite].Center=(ImageDeal[Ysite].LeftBorder+ImageDeal[Ysite].RightBorder)/2;//板块
                if(ImageDeal[Ysite].Center > 93)
                    ImageDeal[Ysite].Center = 93;
            }
            ImageDeal[flag_Ysite_1].LeftBorder=flag_Xsite_1;
            for(Ysite=flag_Ysite_1-1;Ysite>10;Ysite--) //A点上方进行扫线
            {
                for(Xsite=ImageDeal[Ysite+1].LeftBorder+8; Xsite>ImageDeal[Ysite+1].LeftBorder-4; Xsite--)
                {
                    if(Pixle[Ysite][Xsite] == Image_white && Pixle[Ysite][Xsite-1] == 0)
                    {
                     ImageDeal[Ysite].LeftBorder=Xsite;
                     ImageDeal[Ysite].Wide=ImageDeal[Ysite].RightBorder-ImageDeal[Ysite].LeftBorder;
                         ImageDeal[Ysite].Center=(ImageDeal[Ysite].LeftBorder+ImageDeal[Ysite].RightBorder)/2;//板块
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
    //环内不处理
    if (ImageFlag.image_element_rings_flag == 7)
    {

    }
    //大圆环出环 补线
    if (ImageFlag.image_element_rings_flag == 8)  //大圆环
    {
        Repair_Point_Xsite = 60;
        Repair_Point_Ysite = 0;
        for (int Ysite = 40; Ysite > 5; Ysite--)
        {
            if (Pixle[Ysite][58] == Image_white && Pixle[Ysite-1][58] == 0)
            {
                Repair_Point_Xsite = 58;
                Repair_Point_Ysite = Ysite-1;
                ImageStatus.OFFLine = Ysite + 1;  //截止行重新规划
                break;
            }
        }
        for (int Ysite = 57; Ysite > Repair_Point_Ysite-3; Ysite--)         //补线
        {
            ImageDeal[Ysite].LeftBorder = (ImageDeal[58].LeftBorder - Repair_Point_Xsite) * (Ysite - 58) / (58 - Repair_Point_Ysite)  + ImageDeal[58].LeftBorder;
            ImageDeal[Ysite].Center = (ImageDeal[Ysite].RightBorder + ImageDeal[Ysite].LeftBorder) / 2;
        }
    }
    //已出环 半宽处理
    if (ImageFlag.image_element_rings_flag == 9)
    {
        for (int Ysite = 59; Ysite > ImageStatus.OFFLine; Ysite--)
        {
            ImageDeal[Ysite].Center = ImageDeal[Ysite].LeftBorder + Half_Road_Wide[Ysite];
        }
    }
}

/*元素判断函数*/
void Scan_Element()
{
    if (ImageFlag.image_element_rings == 0 )
    {
        Element_Judgment_Left_Rings();      //左圆环
        Element_Judgment_Right_Rings();     //右圆环
    }
}

/*元素处理函数*/
void Element_Handle()
{
    if (ImageFlag.image_element_rings == 1)
        Element_Handle_Left_Rings();
    else if (ImageFlag.image_element_rings == 2)
        Element_Handle_Right_Rings();
    else if(ImageStatus.WhiteLine >= 8) //十字处理
        Get_ExtensionLine();
}

float weighted_error_exp(float *error_center, uint8 cnt);
float center_line_error = 0.0;
int8 error_center[40];
void image_process(void)
{
    Image_CompressInit();   //在初始化完成              //图像压缩，把原始的80*188的图像压缩成60*80的,因为不需要那么多的信息，60*80能处理好的话已经足够了。
    Get_BinaryImage();                                //图像二值化处理，把采集到的原始灰度图像变成二值化图像，也就是变成黑白图像。

    Get_BaseLine();                                   //优化之后的搜线算法：得到一副图像的基础边线，也就是最底下五行的边线信息，用来后续处理。
    Get_AllLine();                                    //优化之后的搜线算法：得到一副图像的全部边线。
    ips200_displayimage032_zoom();
    if(!ImageFlag.Ramp && !ImageFlag.RoadBlock_Flag && !ImageFlag.Out_Road)
        Search_Border_OTSU(Pixle, LCDH, LCDW, LCDH - 2);//58行位底行
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

// 预计算好的指数权重表（k = 0.05，精度可调整）
const float weight_table[40] = {
    1.0000f, 0.9512f, 0.9048f, 0.8607f, 0.8187f, 0.7788f, 0.7408f, 0.7047f,
    0.6703f, 0.6376f, 0.6065f, 0.5769f, 0.5488f, 0.5220f, 0.4966f, 0.4724f,
    0.4493f, 0.4274f, 0.4066f, 0.3867f, 0.3679f, 0.3499f, 0.3329f, 0.3166f,
    0.3012f, 0.2865f, 0.2725f, 0.2592f, 0.2466f, 0.2346f, 0.2231f, 0.2122f,
    0.2019f, 0.1920f, 0.1827f, 0.1738f, 0.1653f, 0.1572f, 0.1496f, 0.1423f
};

// 计算加权误差（查表方式）
float weighted_error_exp(float *error_center, uint8 cnt) {
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

//显示图像   改成你自己的就行 等后期足够自信了，显示关掉，显示屏挺占资源的
void ips200_displayimage032_zoom(void)
{
//    ips200_displayimage03x(image_copy[0], LCDW, LCDH);
    ips200_show_gray_image(0, 0, image_copy[0], LCDW, LCDH, LCDW, LCDH, 0);
    for (uint16 i = 59; i > ImageStatus.OFFLine; i--)
    {
        ips200_draw_point((uint16)ImageDeal[i].RightBorder, i, RGB565_RED);//显示起点 显示中线
        ips200_draw_point((uint16)ImageDeal[i].Center, i, RGB565_BLUE);//显示起点 显示左边线
        ips200_draw_point((uint16)ImageDeal[i].LeftBorder, i, RGB565_RED);//显示起点 显示右边线
    }
}
