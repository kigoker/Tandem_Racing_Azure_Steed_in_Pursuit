//
///*
// * iamge_dealwith.c
// *
// *  Created on: 2025骞�2鏈�16鏃�
// *      Author: zhuji
// */
//
//#include "image_dealwith.h"
//
// const uint8 Standard_Road_Wide1[MT9V03X_H]=
// { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//   39, 41, 43, 44, 45, 46, 47, 48, 49, 50,
//   51, 52, 54, 55, 56, 57, 58, 59, 60, 61,
//   62, 63, 65, 66, 67, 68, 69, 70, 71, 72,
//   73, 74, 76, 77, 79, 80, 81, 82, 83, 84,
//   85, 86, 87, 88, 89, 90, 91, 92, 93, 95,
//   96, 97, 98, 99, 100, 101, 102, 103, 105, 106,
//   107, 108, 109, 110,111,112,113,114,116,117};
//
// const uint8 Standard_Road_Wide02[MT9V03X_H]=//鏍囧噯璧涘
// { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//   10, 12, 14, 16, 18, 20, 22, 24, 26, 28,
//   30, 32, 34, 36, 38, 40, 42, 44, 46, 48,
//   50, 52, 54, 56, 58, 60, 62, 64, 66, 68,
//   70, 72, 74, 76, 78, 80, 82, 84, 86, 88,
//   90, 92, 94, 96, 98,100,102,104,106,108,
//  110,112,114,116,118,120,122,124,126,128,
//  130,132,134,136,138,140,142,144,146,148};
//
//uint8 Threshold = 0 ;
//uint8 state = 0 ;//圆环状态
//uint8_t leftState = 0, rightState = 0; // 单调性标志
////uint8 crossroad_flag = 0 ;//十字状态
//uint8 road_flag;
//uint8_t l_monotone_inc = 1;  // 左边线是否全程递增
//uint8_t r_monotone_dec = 1;  // 右边线是否全程递减
///** @brief 十字路口断点坐标  */
//uint8_t leftup[2] ;
//uint8_t rightup[2] ;
//double a0;        // 常数项
//double a1;        // 一次项系数
//double a2;        // 二次项系数
//double curvature; // 曲率值
//extern uint8_t l_border[MT9V03X_H];//左边线数组
//extern uint8_t r_border[MT9V03X_H];//右边线数组
//extern uint8_t up_and_down[2][MT9V03X_W];//上下边线数组
//extern uint8_t center_line[MT9V03X_H];
//extern uint16 data_stastics_l, data_stastics_r;
//extern uint8 hightest;
//extern float err_road, steer, erspeed,steer1;
//static RoadType current_road_type = STRAIGHT;
//static uint8_t straight_counter = 0;
// //-------------------------------------------------------------------------------------------------------------------
// //  @brief      闃堝€煎鐞嗙畻娉�
// //  @param      image  鍥惧儚鏁扮粍
// //  @param      clo    瀹�
// //  @param      row    楂�
// //  @param      pixel_threshold 闃堝€煎垎绂�
// //  @return     uint8
// //  @since      2021.6.23
// //  @note       鍔ㄦ€侀槇鍊硷紝澶ф触娉�
// //-------------------------------------------------------------------------------------------------------------------
// static uint8 Threshold_deal(uint8* image,
//                      uint16 col,
//                      uint16 row,
//                      uint32 pixel_threshold) {
// #define GrayScale 256
//   uint16 width = col;
//   uint16 height = row;
//   int pixelCount[GrayScale];    //鐏板害鐩存柟鍥�
//   float pixelPro[GrayScale];    //涓嶅悓鐏板害鍊煎湪鍥惧儚涓墍鍗犳瘮渚�
//   int i, j, pixelSum = width * height;     //pixelSum涓烘暣骞呭浘鍍忓儚绱犳€诲拰
//   uint8 threshold = 0;
//   uint8* data = image;  //鎸囧悜鍍忕礌鏁版嵁鐨勬寚閽�
//   for (i = 0; i < GrayScale; i++) {
//     pixelCount[i] = 0;
//     pixelPro[i] = 0;
//   }
//   uint32 gray_sum = 0;
//   //缁熻鐏板害绾т腑姣忎釜鍍忕礌鍦ㄦ暣骞呭浘鍍忎腑鐨勪釜鏁�
//   for (i = 0; i < height; i += 1) {
//     for (j = 0; j < width; j += 1) {
//       pixelCount[(int)data[i * width + j]]++;  //灏嗗綋鍓嶇殑鐐圭殑鍍忕礌鍊间綔涓鸿鏁版暟缁勭殑涓嬫爣
//       gray_sum += (int)data[i * width + j];  //鐏板害鍊兼€诲拰
//     }
//   }
//   //璁＄畻姣忎釜鍍忕礌鍊肩殑鐐瑰湪鏁村箙鍥惧儚涓殑姣斾緥
//   for (i = 0; i < GrayScale; i++) {
//     pixelPro[i] = (float)pixelCount[i] / pixelSum;
//   }
//   //閬嶅巻鐏板害绾0,255]
//   float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
//   w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
//   for (j = 0; j < pixel_threshold; j++) {
//     w0 +=
//         pixelPro[j];  //鑳屾櫙閮ㄥ垎姣忎釜鐏板害鍊肩殑鍍忕礌鐐规墍鍗犳瘮渚嬩箣鍜� 鍗宠儗鏅儴鍒嗙殑姣斾緥
//     u0tmp += j * pixelPro[j];  //鑳屾櫙閮ㄥ垎 姣忎釜鐏板害鍊肩殑鐐圭殑姣斾緥 *鐏板害鍊�
//     w1 = 1 - w0;
//     u1tmp = gray_sum / pixelSum - u0tmp;
//     u0 = u0tmp / w0;    //鑳屾櫙骞冲潎鐏板害
//     u1 = u1tmp / w1;    //鍓嶆櫙骞冲潎鐏板害
//     u = u0tmp + u1tmp;  //鍏ㄥ眬骞冲潎鐏板害
//     deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
//     if (deltaTmp > deltaMax) {
//       deltaMax = deltaTmp;
//       threshold = (uint8)j;
//     }
//     if (deltaTmp < deltaMax) {
//       break;
//     }
//   }
//   return threshold;
// }
// //-------------------------------------------------------------------------------------------------------------------
// //  @brief      鑷€傚簲闃堝€�
// //  @param      img_data  杈撳叆鍥惧儚鏁扮粍
// //  @param      width    瀹�
// //  @param      height    楂�
// //  @param      block 灏忓潡鍖哄煙鐨勫ぇ灏� 闈㈢Нblock*block 鍗冲鐞嗗浘鍍忕殑娆℃暟 蹇呴』涓哄鏁帮紒锛侊紒
// //  @param      clip_value  涓€涓粡楠屽€间竴鑸负 2-5
// //  @since      2025.2.16
// //  @note       閭ｄ箞鑷劧鎴戜滑寰楀嚭涓€涓畝鍗曠殑澶勭悊鏂规硶锛屽浜庢瘡涓儚绱犵偣锛屽彇瀹冨懆鍥翠竴灏忕墖鍖哄煙璁＄畻寰楀嚭涓€涓簩鍊煎寲闃堝€硷紝骞跺杩欎釜鍍忕礌杩涜浜屽€煎寲銆傚弬鑰冧箣鍓嶇殑璁茶繃鐨勫ぇ娲ユ硶锛屼竴灏忕墖鍖哄煙鐨勪簩鍊煎寲闃堝€煎彲浠ラ€氳繃涓綅鏁帮紝骞冲潎鏁版垨鑰呭ぇ娲ユ硶纭畾銆傚湪鎴戜滑鐨勪唬鐮佸疄鐜颁腑锛岄噰鐢ㄤ簡7x7鐨勫尯鍩熷拰骞冲潎鏁扮殑纭畾鏂规硶鍙﹀锛屽綋杩欎竴灏忕墖鍖哄煙浜害宸紓涓嶅ぇ鏃讹紝鎴戜滑鍙兘骞朵笉甯屾湜绠楁硶鈥滃己琛屸€濇壘鍒颁竴涓槇鍊硷紝灏嗚繖鐗囧尯鍩熷垎鍓插紑鏉ワ紝鎵€浠ュ浜庣畻娉曟壘鍑虹殑灞€閮ㄩ槇鍊硷紝鎴戜滑灏嗗叾鍑忓幓涓€涓粡楠屾€х殑鍙傛暟锛堥€氬父鍙�2-5鍗冲彲锛夛紝閬垮厤绠楁硶灏嗗叾寮鸿鍒嗗壊銆�
// //-------------------------------------------------------------------------------------------------------------------
// void adaptiveThreshold(uint8_t* img_data, uint8_t* output_data, int width, int height, int block, uint8_t clip_value){
//
//   int half_block = block / 2;
//   for(int y=half_block; y<height-half_block; y++){
//     for(int x=half_block; x<width-half_block; x++){
//       // 璁＄畻灞€閮ㄩ槇鍊�
//       int thres = 0;
//       for(int dy=-half_block; dy<=half_block; dy++){
//         for(int dx=-half_block; dx<=half_block; dx++){
//           thres += img_data[(x+dx)+(y+dy)*width];
//         }
//       }
//       thres = thres / (block * block) - clip_value;
//       // 杩涜浜屽€煎寲
//       output_data[x+y*width] = img_data[x+y*width]>thres ? 255 : 0;
//     }
//   }
// }
// //-------------------------------------------------------------------------------------------------------------------
// //  @brief      鍥惧儚浜屽€煎寲
// //  @since      2025.2.16
// //  @note       model璁剧疆 0 涓哄ぇ娲ユ硶 1 涓轰笂浜よ嚜閫傚簲闃堝€肩畻娉�
// //-------------------------------------------------------------------------------------------------------------------
//
//void Binarization_Algorithm(void)
//{
//    static uint8 model = 0 ;
//    if(model == 0)
//    {
//        uint8 i = 0 ,j = 0;
//        Threshold = Threshold_deal(image_copy[0], MT9V03X_W, MT9V03X_H, 190);
//        for (i =0 ;i<MT9V03X_H ;i++){
//            for (j = 0; j < MT9V03X_W; j++) {
//               if (image_copy[i][j] >= (Threshold))         //鏁板€艰秺澶э紝鏄剧ず鐨勫唴瀹硅秺澶氾紝杈冩祬鐨勫浘鍍忎篃鑳芥樉绀哄嚭鏉�
//                   image_dealwith[i][j] = IMG_WHITE;  //鐧�
//               else
//                   image_dealwith[i][j] = IMG_BLACK;  //榛�
//             }
//        }
//    }
//    else
//    {
//        adaptiveThreshold(image_copy[0], image_dealwith[0],MT9V03X_W, MT9V03X_H, 7, 2);
//    }
//
//}
//
///*
//鍑芥暟鍚嶇О锛歩nt my_abs(int value)
//鍔熻兘璇存槑锛氭眰缁濆鍊�
//鍙傛暟璇存槑锛�
//鍑芥暟杩斿洖锛氱粷瀵瑰€�
//淇敼鏃堕棿锛�2022骞�9鏈�8鏃�
//澶�    娉細
//example锛�  my_abs( x)锛�
// */
//int my_abs(int value)
//{
//if(value>=0) return value;
//else return -value;
//}
//
//
//
//int16 limit_a_b(int16 x, int a, int b)
//{
//    if(x<a) x = a;
//    if(x>b) x = b;
//    return x;
//}
//
///*
//鍑芥暟鍚嶇О锛歩nt16 limit(int16 x, int16 y)
//鍔熻兘璇存槑锛氭眰x,y涓殑鏈€灏忓€�
//鍙傛暟璇存槑锛�
//鍑芥暟杩斿洖锛氳繑鍥炰袱鍊间腑鐨勬渶灏忓€�
//淇敼鏃堕棿锛�2022骞�9鏈�8鏃�
//澶�    娉細
//example锛�  limit( x,  y)
// */
//int16 limit1(int16 x, int16 y)
//{
//    if (x > y)             return y;
//    else if (x < -y)       return -y;
//    else                return x;
//}
//
//
///*鍙橀噺澹版槑*/
//uint8 image_thereshold;//鍥惧儚鍒嗗壊闃堝€�
////------------------------------------------------------------------------------------------------------------------
////  @brief      鑾峰緱涓€鍓伆搴﹀浘鍍�
////  @since      v1.0
////------------------------------------------------------------------------------------------------------------------
//void Get_image(uint8(*image_copy)[image_w])
//{
//#define use_num     1   //1灏辨槸涓嶅帇缂╋紝2灏辨槸鍘嬬缉涓€鍊�
//    uint8 i = 0, j = 0, row = 0, line = 0;
//    for (i = 0; i < image_h; i += use_num)          //
//    {
//        for (j = 0; j <image_w; j += use_num)     //
//        {
//            original_image[row][line] = mt9v03x_image[i][j];//杩欓噷鐨勫弬鏁板～鍐欎綘鐨勬憚鍍忓ご閲囬泦鍒扮殑鍥惧儚
//            line++;
//        }
//        line = 0;
//        row++;
//    }
//}
////------------------------------------------------------------------------------------------------------------------
////  @brief     鍔ㄦ€侀槇鍊�
////  @since      v1.0
////------------------------------------------------------------------------------------------------------------------
//uint8 otsuThreshold(uint8 *image, uint16 col, uint16 row)
//{
//#define GrayScale 256
//    uint16 Image_Width  = col;
//    uint16 Image_Height = row;
//    int X; uint16 Y;
//    uint8* data = image;
//    int HistGram[GrayScale] = {0};
//
//    uint32 Amount = 0;
//    uint32 PixelBack = 0;
//    uint32 PixelIntegralBack = 0;
//    uint32 PixelIntegral = 0;
//    int32 PixelIntegralFore = 0;
//    int32 PixelFore = 0;
//    double OmegaBack=0, OmegaFore=0, MicroBack=0, MicroFore=0, SigmaB=0, Sigma=0; // 绫婚棿鏂瑰樊;
//    uint8 MinValue=0, MaxValue=0;
//    uint8 Threshold = 0;
//
//
//    for (Y = 0; Y <Image_Height; Y++) //Y<Image_Height鏀逛负Y =Image_Height锛涗互渚胯繘琛� 琛屼簩鍊煎寲
//    {
//        //Y=Image_Height;
//        for (X = 0; X < Image_Width; X++)
//        {
//        HistGram[(int)data[Y*Image_Width + X]]++; //缁熻姣忎釜鐏板害鍊肩殑涓暟淇℃伅
//        }
//    }
//
//
//
//
//    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;        //鑾峰彇鏈€灏忕伆搴︾殑鍊�
//    for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--) ; //鑾峰彇鏈€澶х伆搴︾殑鍊�
//
//    if (MaxValue == MinValue)
//    {
//        return MaxValue;          // 鍥惧儚涓彧鏈変竴涓鑹�
//    }
//    if (MinValue + 1 == MaxValue)
//    {
//        return MinValue;      // 鍥惧儚涓彧鏈変簩涓鑹�
//    }
//
//    for (Y = MinValue; Y <= MaxValue; Y++)
//    {
//        Amount += HistGram[Y];        //  鍍忕礌鎬绘暟
//    }
//
//    PixelIntegral = 0;
//    for (Y = MinValue; Y <= MaxValue; Y++)
//    {
//        PixelIntegral += HistGram[Y] * Y;//鐏板害鍊兼€绘暟
//    }
//    SigmaB = -1;
//    for (Y = MinValue; Y < MaxValue; Y++)
//    {
//          PixelBack = PixelBack + HistGram[Y];    //鍓嶆櫙鍍忕礌鐐规暟
//          PixelFore = Amount - PixelBack;         //鑳屾櫙鍍忕礌鐐规暟
//          OmegaBack = (double)PixelBack / Amount;//鍓嶆櫙鍍忕礌鐧惧垎姣�
//          OmegaFore = (double)PixelFore / Amount;//鑳屾櫙鍍忕礌鐧惧垎姣�
//          PixelIntegralBack += HistGram[Y] * Y;  //鍓嶆櫙鐏板害鍊�
//          PixelIntegralFore = PixelIntegral - PixelIntegralBack;//鑳屾櫙鐏板害鍊�
//          MicroBack = (double)PixelIntegralBack / PixelBack;//鍓嶆櫙鐏板害鐧惧垎姣�
//          MicroFore = (double)PixelIntegralFore / PixelFore;//鑳屾櫙鐏板害鐧惧垎姣�
//          Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
//          if (Sigma > SigmaB)//閬嶅巻鏈€澶х殑绫婚棿鏂瑰樊g
//          {
//              SigmaB = Sigma;
//              Threshold = (uint8)Y;
//          }
//    }
//   return Threshold;
//}
////------------------------------------------------------------------------------------------------------------------
////  @brief      鍥惧儚浜屽€煎寲锛岃繖閲岀敤鐨勬槸澶ф触娉曚簩鍊煎寲銆�
////  @since      v1.0
////------------------------------------------------------------------------------------------------------------------
//
//void turn_to_bin(void)
//{
//  uint8 i,j;
// image_thereshold = otsuThreshold(original_image[0], image_w, image_h);
//  for(i = 0;i<image_h;i++)
//  {
//      for(j = 0;j<image_w;j++)
//      {
//          if(original_image[i][j]>image_thereshold)bin_image[i][j] = white_pixel;
//          else bin_image[i][j] = black_pixel;
//      }
//  }
//}
//
//
///*
//鍑芥暟鍚嶇О锛歷oid get_start_point(uint8 start_row)
//鍔熻兘璇存槑锛氬鎵句袱涓竟鐣岀殑杈圭晫鐐逛綔涓哄叓閭诲煙寰幆鐨勮捣濮嬬偣
//鍙傛暟璇存槑锛氳緭鍏ヤ换鎰忚鏁�
//鍑芥暟杩斿洖锛氭棤
//淇敼鏃堕棿锛�2022骞�9鏈�8鏃�
//澶�    娉細
//example锛�  get_start_point(image_h-2)
// */
//uint8 start_point_l[2] = { 0 };//宸﹁竟璧风偣鐨剎锛寉鍊�
//uint8 start_point_r[2] = { 0 };//鍙宠竟璧风偣鐨剎锛寉鍊�
//uint8 get_start_point(uint8 start_row)
//{
//    uint8 i = 0,l_found = 0,r_found = 0;
//    //娓呴浂
//    start_point_l[0] = 0;//x
//    start_point_l[1] = 0;//y
//
//    start_point_r[0] = 0;//x
//    start_point_r[1] = 0;//y
//
//        //浠庝腑闂村線宸﹁竟锛屽厛鎵捐捣鐐�
//    for (i = image_w / 2; i > border_min; i--)
//    {
//        start_point_l[0] = i;//x
//        start_point_l[1] = start_row;//y
//        if (bin_image[start_row][i] == 255 && bin_image[start_row][i - 1] == 0)
//        {
//            //printf("鎵惧埌宸﹁竟璧风偣image[%d][%d]\n", start_row,i);
//            l_found = 1;
//            break;
//        }
//    }
//
//    for (i = image_w / 2; i < border_max; i++)
//    {
//        start_point_r[0] = i;//x
//        start_point_r[1] = start_row;//y
//        if (bin_image[start_row][i] == 255 && bin_image[start_row][i + 1] == 0)
//        {
//            //printf("鎵惧埌鍙宠竟璧风偣image[%d][%d]\n",start_row, i);
//            r_found = 1;
//            break;
//        }
//    }
//
//    if(l_found&&r_found)return 1;
//    else {
//        //printf("鏈壘鍒拌捣鐐筡n");
//        return 0;
//    }
//}
//
///*
//鍑芥暟鍚嶇О锛歷oid search_l_r(uint16 break_flag, uint8(*image)[image_w],uint16 *l_stastic, uint16 *r_stastic,
//                            uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y,uint8*hightest)
//
//鍔熻兘璇存槑锛氬叓閭诲煙姝ｅ紡寮€濮嬫壘鍙宠竟鐐圭殑鍑芥暟锛岃緭鍏ュ弬鏁版湁鐐瑰锛岃皟鐢ㄧ殑鏃跺€欎笉瑕佹紡浜嗭紝杩欎釜鏄乏鍙崇嚎涓€娆℃€ф壘瀹屻€�
//鍙傛暟璇存槑锛�
//break_flag_r            锛氭渶澶氶渶瑕佸惊鐜殑娆℃暟
//(*image)[image_w]       锛氶渶瑕佽繘琛屾壘鐐圭殑鍥惧儚鏁扮粍锛屽繀椤绘槸浜屽€煎浘,濉叆鏁扮粍鍚嶇О鍗冲彲
//                       鐗瑰埆娉ㄦ剰锛屼笉瑕佹嬁瀹忓畾涔夊悕瀛椾綔涓鸿緭鍏ュ弬鏁帮紝鍚﹀垯鏁版嵁鍙兘鏃犳硶浼犻€掕繃鏉�
//*l_stastic              锛氱粺璁″乏杈规暟鎹紝鐢ㄦ潵杈撳叆鍒濆鏁扮粍鎴愬憳鐨勫簭鍙峰拰鍙栧嚭寰幆娆℃暟
//*r_stastic              锛氱粺璁″彸杈规暟鎹紝鐢ㄦ潵杈撳叆鍒濆鏁扮粍鎴愬憳鐨勫簭鍙峰拰鍙栧嚭寰幆娆℃暟
//l_start_x               锛氬乏杈硅捣鐐规í鍧愭爣
//l_start_y               锛氬乏杈硅捣鐐圭旱鍧愭爣
//r_start_x               锛氬彸杈硅捣鐐规í鍧愭爣
//r_start_y               锛氬彸杈硅捣鐐圭旱鍧愭爣
//hightest                锛氬惊鐜粨鏉熸墍寰楀埌鐨勬渶楂橀珮搴�
//鍑芥暟杩斿洖锛氭棤
//淇敼鏃堕棿锛�2022骞�9鏈�25鏃�
//澶�    娉細
//example锛�
//    search_l_r((uint16)USE_num,image,&data_stastics_l, &data_stastics_r,start_point_l[0],
//                start_point_l[1], start_point_r[0], start_point_r[1],&hightest);
// */
//#define USE_num image_h*3   //瀹氫箟鎵剧偣鐨勬暟缁勬垚鍛樹釜鏁版寜鐞嗚300涓偣鑳芥斁涓嬶紝浣嗘槸鏈変簺鐗规畩鎯呭喌纭疄闅鹃《锛屽瀹氫箟浜嗕竴鐐�
//
// //瀛樻斁鐐圭殑x锛寉鍧愭爣
//uint16 points_l[(uint16)USE_num][2] = { {  0 } };//宸︾嚎
//uint16 points_r[(uint16)USE_num][2] = { {  0 } };//鍙崇嚎
//uint16 dir_r[(uint16)USE_num] = { 0 };//鐢ㄦ潵瀛樺偍鍙宠竟鐢熼暱鏂瑰悜
//uint16 dir_l[(uint16)USE_num] = { 0 };//鐢ㄦ潵瀛樺偍宸﹁竟鐢熼暱鏂瑰悜
//uint16 data_stastics_l = 0;//缁熻宸﹁竟鎵惧埌鐐圭殑涓暟
//uint16 data_stastics_r = 0;//缁熻鍙宠竟鎵惧埌鐐圭殑涓暟
//uint8 hightest = 0;//鏈€楂樼偣
//void search_l_r(uint16 break_flag, uint8(*image)[image_w], uint16 *l_stastic, uint16 *r_stastic, uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest)
//{
//
//    uint8 i = 0, j = 0;
//
//    //宸﹁竟鍙橀噺
//    uint8 search_filds_l[8][2] = { {  0 } };
//    uint8 index_l = 0;
//    uint8 temp_l[8][2] = { {  0 } };
//    uint8 center_point_l[2] = {  0 };
//    uint16 l_data_statics;//缁熻宸﹁竟
//    //瀹氫箟鍏釜閭诲煙
//    static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
//    //{-1,-1},{0,-1},{+1,-1},
//    //{-1, 0},       {+1, 0},
//    //{-1,+1},{0,+1},{+1,+1},
//    //杩欎釜鏄『鏃堕拡
//
//    //鍙宠竟鍙橀噺
//    uint8 search_filds_r[8][2] = { {  0 } };
//    uint8 center_point_r[2] = { 0 };//涓績鍧愭爣鐐�
//    uint8 index_r = 0;//绱㈠紩涓嬫爣
//    uint8 temp_r[8][2] = { {  0 } };
//    uint16 r_data_statics;//缁熻鍙宠竟
//    //瀹氫箟鍏釜閭诲煙
//    static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
//    //{-1,-1},{0,-1},{+1,-1},
//    //{-1, 0},       {+1, 0},
//    //{-1,+1},{0,+1},{+1,+1},
//    //杩欎釜鏄€嗘椂閽�
//
//    l_data_statics = *l_stastic;//缁熻鎵惧埌浜嗗灏戜釜鐐癸紝鏂逛究鍚庣画鎶婄偣鍏ㄩ儴鐢诲嚭鏉�
//    r_data_statics = *r_stastic;//缁熻鎵惧埌浜嗗灏戜釜鐐癸紝鏂逛究鍚庣画鎶婄偣鍏ㄩ儴鐢诲嚭鏉�
//
//    //绗竴娆℃洿鏂板潗鏍囩偣  灏嗘壘鍒扮殑璧风偣鍊间紶杩涙潵
//    center_point_l[0] = l_start_x;//x
//    center_point_l[1] = l_start_y;//y
//    center_point_r[0] = r_start_x;//x
//    center_point_r[1] = r_start_y;//y
//
//        //寮€鍚偦鍩熷惊鐜�
//    while (break_flag--)
//    {
//
//        //宸﹁竟
//        for (i = 0; i < 8; i++)//浼犻€�8F鍧愭爣
//        {
//            search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x
//            search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
//        }
//        //涓績鍧愭爣鐐瑰～鍏呭埌宸茬粡鎵惧埌鐨勭偣鍐�
//        points_l[l_data_statics][0] = center_point_l[0];//x
//        points_l[l_data_statics][1] = center_point_l[1];//y
//        l_data_statics++;//绱㈠紩鍔犱竴
//
//        //鍙宠竟
//        for (i = 0; i < 8; i++)//浼犻€�8F鍧愭爣
//        {
//            search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
//            search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
//        }
//        //涓績鍧愭爣鐐瑰～鍏呭埌宸茬粡鎵惧埌鐨勭偣鍐�
//        points_r[r_data_statics][0] = center_point_r[0];//x
//        points_r[r_data_statics][1] = center_point_r[1];//y
//
//        index_l = 0;//鍏堟竻闆讹紝鍚庝娇鐢�
//        for (i = 0; i < 8; i++)
//        {
//            temp_l[i][0] = 0;//鍏堟竻闆讹紝鍚庝娇鐢�
//            temp_l[i][1] = 0;//鍏堟竻闆讹紝鍚庝娇鐢�
//        }
//
//        //宸﹁竟鍒ゆ柇
//        for (i = 0; i < 8; i++)
//        {
//            if (image[search_filds_l[i][1]][search_filds_l[i][0]] == 0
//                && image[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 255)
//            {
//                temp_l[index_l][0] = search_filds_l[(i)][0];
//                temp_l[index_l][1] = search_filds_l[(i)][1];
//                index_l++;
//                dir_l[l_data_statics - 1] = (i);//璁板綍鐢熼暱鏂瑰悜
//            }
//
//            if (index_l)
//            {
//                //鏇存柊鍧愭爣鐐�
//                center_point_l[0] = temp_l[0][0];//x
//                center_point_l[1] = temp_l[0][1];//y
//                for (j = 0; j < index_l; j++)
//                {
//                    if (center_point_l[1] > temp_l[j][1])
//                    {
//                        center_point_l[0] = temp_l[j][0];//x
//                        center_point_l[1] = temp_l[j][1];//y
//                    }
//                }
//            }
//
//        }
//        if ((points_r[r_data_statics][0]== points_r[r_data_statics-1][0]&& points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
//            && points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
//            ||(points_l[l_data_statics-1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics-1][0] == points_l[l_data_statics - 3][0]
//                && points_l[l_data_statics-1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics-1][1] == points_l[l_data_statics - 3][1]))
//        {
//            //printf("涓夋杩涘叆鍚屼竴涓偣锛岄€€鍑篭n");
//            break;
//        }
//        if (my_abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
//            && my_abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1] < 2)
//            )
//        {
//            //printf("\n宸﹀彸鐩搁亣閫€鍑篭n");
//            *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//鍙栧嚭鏈€楂樼偣
//            //printf("\n鍦▂=%d澶勯€€鍑篭n",*hightest);
//            break;
//        }
//        if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))
//        {
//            //printf("\n濡傛灉宸﹁竟姣斿彸杈归珮浜嗭紝宸﹁竟绛夊緟鍙宠竟\n");
//            continue;//濡傛灉宸﹁竟姣斿彸杈归珮浜嗭紝宸﹁竟绛夊緟鍙宠竟
//        }
//        if (dir_l[l_data_statics - 1] == 7
//            && (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//宸﹁竟姣斿彸杈归珮涓斿凡缁忓悜涓嬬敓闀夸簡
//        {
//            //printf("\n宸﹁竟寮€濮嬪悜涓嬩簡锛岀瓑寰呭彸杈癸紝绛夊緟涓�... \n");
//            center_point_l[0] = points_l[l_data_statics - 1][0];//x
//            center_point_l[1] = points_l[l_data_statics - 1][1];//y
//            l_data_statics--;
//        }
//        r_data_statics++;//绱㈠紩鍔犱竴
//
//        index_r = 0;//鍏堟竻闆讹紝鍚庝娇鐢�
//        for (i = 0; i < 8; i++)
//        {
//            temp_r[i][0] = 0;//鍏堟竻闆讹紝鍚庝娇鐢�
//            temp_r[i][1] = 0;//鍏堟竻闆讹紝鍚庝娇鐢�
//        }
//
//        //鍙宠竟鍒ゆ柇
//        for (i = 0; i < 8; i++)
//        {
//            if (image[search_filds_r[i][1]][search_filds_r[i][0]] == 0
//                && image[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255)
//            {
//                temp_r[index_r][0] = search_filds_r[(i)][0];
//                temp_r[index_r][1] = search_filds_r[(i)][1];
//                index_r++;//绱㈠紩鍔犱竴
//                dir_r[r_data_statics - 1] = (i);//璁板綍鐢熼暱鏂瑰悜
//                //printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
//            }
//            if (index_r)
//            {
//
//                //鏇存柊鍧愭爣鐐�
//                center_point_r[0] = temp_r[0][0];//x
//                center_point_r[1] = temp_r[0][1];//y
//                for (j = 0; j < index_r; j++)
//                {
//                    if (center_point_r[1] > temp_r[j][1])
//                    {
//                        center_point_r[0] = temp_r[j][0];//x
//                        center_point_r[1] = temp_r[j][1];//y
//                    }
//                }
//
//            }
//        }
//
//
//    }
//
//
//    //鍙栧嚭寰幆娆℃暟
//    *l_stastic = l_data_statics;
//    *r_stastic = r_data_statics;
//
//}
///*
//鍑芥暟鍚嶇О锛歷oid get_left(uint16 total_L)
//鍔熻兘璇存槑锛氫粠鍏偦鍩熻竟鐣岄噷鎻愬彇闇€瑕佺殑杈圭嚎
//鍙傛暟璇存槑锛�
//total_L 锛氭壘鍒扮殑鐐圭殑鎬绘暟
//鍑芥暟杩斿洖锛氭棤
//淇敼鏃堕棿锛�2022骞�9鏈�25鏃�
//澶�    娉細
//example锛� get_left(data_stastics_l );
// */
//
//void get_left(uint16 total_L)
//{
//    uint8 i = 0;
//    uint16 j = 0;
//    uint8 h = 0;
//    //鍒濆鍖�
//    for (i = 0;i<image_h;i++)
//    {
//        l_border[i] = border_min;
//    }
//    h = image_h - 2;
//    //宸﹁竟
//    for (j = 0; j < total_L; j++)
//    {
//        //printf("%d\n", j);
//        if (points_l[j][1] == h)
//        {
//            l_border[h] = points_l[j][0]+1;
//        }
//        else continue; //姣忚鍙彇涓€涓偣锛屾病鍒颁笅涓€琛屽氨涓嶈褰�
//        h--;
//        if (h == 0)
//        {
//            break;//鍒版渶鍚庝竴琛岄€€鍑�
//        }
//    }
//}
///*
//鍑芥暟鍚嶇О锛歷oid get_right(uint16 total_R)
//鍔熻兘璇存槑锛氫粠鍏偦鍩熻竟鐣岄噷鎻愬彇闇€瑕佺殑杈圭嚎
//鍙傛暟璇存槑锛�
//total_R  锛氭壘鍒扮殑鐐圭殑鎬绘暟
//鍑芥暟杩斿洖锛氭棤
//淇敼鏃堕棿锛�2022骞�9鏈�25鏃�
//澶�    娉細
//example锛歡et_right(data_stastics_r);
// */
//void get_right(uint16 total_R)
//{
//    uint8 i = 0;
//    uint16 j = 0;
//    uint8 h = 0;
//    for (i = 0; i < image_h; i++)
//    {
//        r_border[i] = border_max;//鍙宠竟绾垮垵濮嬪寲鏀惧埌鏈€鍙宠竟锛屽乏杈圭嚎鏀惧埌鏈€宸﹁竟锛岃繖鏍峰叓閭诲煙闂悎鍖哄煙澶栫殑涓嚎灏变細鍦ㄤ腑闂达紝涓嶄細骞叉壈寰楀埌鐨勬暟鎹�
//    }
//    h = image_h - 2;
//    //鍙宠竟
//    for (j = 0; j < total_R; j++)
//    {
//        if (points_r[j][1] == h)
//        {
//            r_border[h] = points_r[j][0] - 1;
//        }
//        else continue;//姣忚鍙彇涓€涓偣锛屾病鍒颁笅涓€琛屽氨涓嶈褰�
//        h--;
//        if (h == 0)break;//鍒版渶鍚庝竴琛岄€€鍑�
//    }
//}
//
////瀹氫箟鑶ㄨ儉鍜岃厫铓€鐨勯槇鍊煎尯闂�
//#define threshold_max   255*5//姝ゅ弬鏁板彲鏍规嵁鑷繁鐨勯渶姹傝皟鑺�
//#define threshold_min   255*2//姝ゅ弬鏁板彲鏍规嵁鑷繁鐨勯渶姹傝皟鑺�
//void image_filter(uint8(*bin_image)[image_w])//褰㈡€佸婊ゆ尝锛岀畝鍗曟潵璇村氨鏄啫鑳€鍜岃厫铓€鐨勬€濇兂
//{
//    uint16 i, j;
//    uint32 num = 0;
//
//
//    for (i = 1; i < image_h - 1; i++)
//    {
//        for (j = 1; j < (image_w - 1); j++)
//        {
//            //缁熻鍏釜鏂瑰悜鐨勫儚绱犲€�
//            num =
//                bin_image[i - 1][j - 1] + bin_image[i - 1][j] + bin_image[i - 1][j + 1]
//                + bin_image[i][j - 1] + bin_image[i][j + 1]
//                + bin_image[i + 1][j - 1] + bin_image[i + 1][j] + bin_image[i + 1][j + 1];
//
//
//            if (num >= threshold_max && bin_image[i][j] == 0)
//            {
//
//                bin_image[i][j] = 255;//鐧�  鍙互鎼炴垚瀹忓畾涔夛紝鏂逛究鏇存敼
//
//            }
//            if (num <= threshold_min && bin_image[i][j] == 255)
//            {
//
//                bin_image[i][j] = 0;//榛�
//
//            }
//
//        }
//    }
//
//}
//
//
///*
//鍑芥暟鍚嶇О锛歷oid image_draw_rectan(uint8(*image)[image_w])
//鍔熻兘璇存槑锛氱粰鍥惧儚鐢讳竴涓粦妗�
//鍙傛暟璇存槑锛歶int8(*image)[image_w] 鍥惧儚棣栧湴鍧€
//鍑芥暟杩斿洖锛氭棤
//淇敼鏃堕棿锛�2022骞�9鏈�8鏃�
//澶�    娉細
//example锛� image_draw_rectan(bin_image);
// */
//void image_draw_rectan(uint8(*image)[image_w])
//{
//
//    uint8 i = 0;
//    for (i = 0; i < image_h; i++)
//    {
//        image[i][0] = 0;
//        image[i][1] = 0;
//        image[i][image_w - 1] = 0;
//        image[i][image_w - 2] = 0;
//
//    }
//    for (i = 0; i < image_w; i++)
//    {
//        image[0][i] = 0;
//        image[1][i] = 0;
//        //image[image_h-1][i] = 0;
//
//    }
//}
///*!
// * @brief    获取赛道宽度
// *
// * @param    l_border  : 左边线数组
// * @param    r_border  : 右边线数组
// * @param    Roadwide  : 赛道宽度数组
// *
// * @return   是否丢线（0 表示未丢线）
// *
// * @note     思路：右边线数组减去左边线数组，适配 MT9V03X 摄像头，分辨率 188×120
// *
// * @see      GetRoadWide(l_border, r_border, Roadwide);
// *
// * @date     2025年3月21日
// */
//uint8_t GetRoadWide(uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], uint8_t Roadwide[MT9V03X_H])
//{
//    uint8_t i = 0;
//    for (i = 10; i <= MT9V03X_H - 5; i++)
//    {
//        Roadwide[i] = 0;
//        if (r_border[i] > l_border[i])
//        {
//            Roadwide[i] = r_border[i] - l_border[i];
//        }
//        else
//        {
//            Roadwide[i] = MT9V03X_W; // 160 替换为 188
//        }
//    }
//    return 0;
//}
//
///*
// * 函数名称：uint8_t UpdownSideGet(uint8_t imageInput[MT9V03X_H][MT9V03X_W], uint8_t imageOut[2][MT9V03X_W])
// * 功能说明：从图像中间行开始，扫描每一列的上下边界点，生成上下边界数组
// * 参数说明：
// *   imageInput：输入的二值化图像数组
// *   imageOut：输出的上下边界数组，imageOut[0] 为上边界，imageOut[1] 为下边界
// * 返回值：0（成功）
// * 修改时间：2025年3月20日
// * 备注：适配 MT9V03X 摄像头，分辨率 188×120
// * 示例：UpdownSideGet(bin_image, border_lines);
// */
//uint8_t UpdownSideGet(uint8_t imageInput[MT9V03X_H][MT9V03X_W], uint8_t imageOut[2][MT9V03X_W])
//{
//    uint8_t i = 0, j = 0;
//    uint8_t last = MT9V03X_H / 2;  // 中间行
//
//    // 初始化边界数组的最右侧值
//    imageOut[0][MT9V03X_W - 1] = 0;          // 上边界初始为顶部
//    imageOut[1][MT9V03X_W - 1] = MT9V03X_H - 1;  // 下边界初始为底部
//
//    // 处理中间列的上下边界
//    for (i = last; i > 0; i--)  // 从中间向上扫描
//    {
//        if (imageInput[i][MT9V03X_W / 2] == 0)  // 遇到黑色像素（边界）
//        {
//            imageOut[0][MT9V03X_W / 2] = i;
//            break;
//        }
//    }
//    for (i = last; i < MT9V03X_H; i++)  // 从中间向下扫描
//    {
//        if (imageInput[i][MT9V03X_W / 2] == 0)
//        {
//            imageOut[1][MT9V03X_W / 2] = i;
//            break;
//        }
//    }
//
//    // 从中间向左扫描每一列
//    for (i = MT9V03X_W / 2 - 1; i > 0; i--)
//    {
//        imageOut[0][i] = 0;          // 上边界初始为顶部
//        imageOut[1][i] = MT9V03X_H - 1;  // 下边界初始为底部
//
//        // 上边界扫描，从右侧列的上边界 +5 开始向上
//        for (j = imageOut[0][i + 1] + 5; j > 0; j--)
//        {
//            if (imageInput[j][i] == 0)
//            {
//                imageOut[0][i] = j;
//                break;
//            }
//        }
//        // 下边界扫描，从右侧列的下边界 -5 开始向下
//        for (j = imageOut[1][i + 1] - 5; j < MT9V03X_H; j++)
//        {
//            if (imageInput[j][i] == 0)
//            {
//                imageOut[1][i] = j;
//                break;
//            }
//        }
//    }
//
//    // 从中间向右扫描每一列
//    for (i = MT9V03X_W / 2 + 1; i < MT9V03X_W - 1; i++)
//    {
//        imageOut[0][i] = 0;          // 上边界初始为顶部
//        imageOut[1][i] = MT9V03X_H - 1;  // 下边界初始为底部
//
//        // 上边界扫描，从左侧列的上边界 +5 开始向上
//        for (j = imageOut[0][i - 1] + 5; j > 0; j--)
//        {
//            if (imageInput[j][i] == 0)
//            {
//                imageOut[0][i] = j;
//                break;
//            }
//        }
//        // 下边界扫描，从左侧列的下边界 -5 开始向下
//        for (j = imageOut[1][i - 1] - 5; j < MT9V03X_H; j++)
//        {
//            if (imageInput[j][i] == 0)
//            {
//                imageOut[1][i] = j;
//                break;
//            }
//        }
//    }
//
//    return 0;
//}
///*
///*
// * 函数名称：void CheckRoadMonotone(uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], uint8_t *flag)
// * 功能说明：检测中间 60 行数据中左右边线是否全程单调（左边递增，右边递减），若不是则设置标志位
// * 参数说明：
// *   l_border：左边线数组，索引对应 y 坐标
// *   r_border：右边线数组，索引对应 y 坐标
// *   flag：输出参数，0 表示全程单调（直道），1 表示非全程单调（非直道）
// * 返回值：无
// * 修改时间：2025年3月22日
// * 备注：适配 MT9V03X 摄像头，分辨率 188×120，仅检查 y=30 到 y=89 的 60 行数据
// * 示例：CheckRoadMonotone(l_border, r_border, &road_flag);
// */
//void CheckRoadMonotone(uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], uint8_t *flag)
//{
//    uint8_t i;
//    uint8_t l_monotone_inc = 1;  // 左边线是否全程递增（x 变大）
//    uint8_t r_monotone_dec = 1;  // 右边线是否全程递减（x 变小）
//
//    *flag = 0;  // 默认设为直道
//
////    // 检查中间 60 行，从 y=30 到 y=89（MT9V03X_H=120，居中取 60 行）
////    for (i = 89; i > 30; i--)  // 从 y=89 到 y=31（包含 60 行）
////    {
////        if (l_border[i] == border_min) continue;  // 跳过丢线点
////        if (l_border[i] < l_border[i + 1] && l_border[i] != l_border[i + 1])        // 如果当前点比下一行靠左（x 变小），非递增
////        {
////            l_monotone_inc = 0;
////            break;
////        }
////    }
////
////    for (i = 89; i > 30; i--)  // 从 y=89 到 y=31
////    {
////        if (r_border[i] == border_max) continue;  // 跳过丢线点
////        if (r_border[i] > r_border[i + 1] && r_border[i] != r_border[i + 1])        // 如果当前点比下一行靠右（x 变大），非递减
////        {
////            r_monotone_dec = 0;
////            break;
////        }
////    }
//    for (i = 100; i > 15; i--) {
//        if (l_border[i] == border_min) {  // 检测到左边丢线
//            l_monotone_inc = 0;           // 直接判断为非单调递增
//            break;                        // 退出循环
//        }
//        if (l_border[i] < l_border[i + 1] && l_border[i] != l_border[i + 1]) {
//            l_monotone_inc = 0;           // 非单调递增
//            break;
//        }
//    }
//
//    for (i = 100; i > 15; i--) {
//        if (r_border[i] == border_max) {  // 检测到右边丢线
//            r_monotone_dec = 0;           // 直接判断为非单调递减
//            break;                        // 退出循环
//        }
//        if (r_border[i] > r_border[i + 1] && r_border[i] != r_border[i + 1]) {
//            r_monotone_dec = 0;           // 非单调递减
//            break;
//        }
//    }
//
//    // 如果左右边线不是全程单调，设置标志位为 1
//    if (!l_monotone_inc && !r_monotone_dec)
//    {
//        *flag = 1;  // 非直道
//    }
//}
///*
// * 函数名称：void GetLostLineFlags(uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], int Left_Lost_Flag[MT9V03X_H], int Right_Lost_Flag[MT9V03X_H])
// * 功能说明：根据左右边线数组检测丢线情况，生成丢线标志数组
// * 参数说明：
// *   l_border：左边线数组，索引对应 y 坐标
// *   r_border：右边线数组，索引对应 y 坐标
// *   Left_Lost_Flag：左丢线标志数组，丢线置1，没丢线置0
// *   Right_Lost_Flag：右丢线标志数组，丢线置1，没丢线置0
// * 返回值：无
// * 修改时间：2025年3月28日
// * 备注：适配 MT9V03X 摄像头，分辨率 188×120
// * 示例：GetLostLineFlags(l_border, r_border, Left_Lost_Flag, Right_Lost_Flag);
// */
//void GetLostLineFlags(uint8_t l_border[MT9V03X_H], uint8_t r_border[MT9V03X_H], int Left_Lost_Flag[MT9V03X_H], int Right_Lost_Flag[MT9V03X_H])
//{
//    uint8_t i;
//
//    // 遍历每一行，检测左右边线是否丢线
//    for (i = 0; i < MT9V03X_H; i++)
//    {
//        // 左边线丢线检测
//        if (l_border[i] == border_min)
//        {
//            Left_Lost_Flag[i] = 1;  // 丢线
//        }
//        else
//        {
//            Left_Lost_Flag[i] = 0;  // 未丢线
//        }
//
//        // 右边线丢线检测
//        if (r_border[i] == border_max)
//        {
//            Right_Lost_Flag[i] = 1;  // 丢线
//        }
//        else
//        {
//            Right_Lost_Flag[i] = 0;  // 未丢线
//        }
//    }
//}
//
//
//
//void image_process(void)
//{
//uint16 i;
//uint8 hightest = 0;//瀹氫箟涓€涓渶楂樿锛宼ip锛氳繖閲岀殑鏈€楂樻寚鐨勬槸y鍊肩殑鏈€灏�
//uint8 hightest_local = 0;
///*杩欐槸绂荤嚎璋冭瘯鐢ㄧ殑*/
//Get_image(mt9v03x_image);
//turn_to_bin();
//
//image_filter(bin_image);
//image_draw_rectan(bin_image);
//
///*鎻愬彇璧涢亾杈圭晫*/
//image_filter(bin_image);//婊ゆ尝
//image_draw_rectan(bin_image);//棰勫鐞�
//UpdownSideGet(bin_image, up_and_down);
//GetRoadWide(l_border, r_border, Roadwide);
//
////娓呴浂
//data_stastics_l = 0;
//data_stastics_r = 0;
//if (get_start_point(image_h - 2))//鎵惧埌璧风偣浜嗭紝鍐嶆墽琛屽叓棰嗗煙锛屾病鎵惧埌灏变竴鐩存壘
//{
//    //printf("姝ｅ湪寮€濮嬪叓棰嗗煙\n");
//    search_l_r((uint16)USE_num, bin_image, &data_stastics_l, &data_stastics_r, start_point_l[0], start_point_l[1], start_point_r[0], start_point_r[1], &hightest);
//
//    //printf("鍏偦鍩熷凡缁撴潫\n");
//    // 浠庣埇鍙栫殑杈圭晫绾垮唴鎻愬彇杈圭嚎 锛� 杩欎釜鎵嶆槸鏈€缁堟湁鐢ㄧ殑杈圭嚎
//    get_left(data_stastics_l);
//    get_right(data_stastics_r);
////    Island_Detect();
//    //澶勭悊鍑芥暟鏀捐繖閲岋紝涓嶈鏀惧埌if澶栭潰鍘讳簡锛屼笉瑕佹斁鍒癷f澶栭潰鍘讳簡锛屼笉瑕佹斁鍒癷f澶栭潰鍘讳簡锛岄噸瑕佺殑浜嬭涓夐亶
////    RoadIsRoundabout(l_border, r_border, 10, &state);
////    CheckRoadMonotone(l_border, r_border, &road_flag);
//    // 计算丢线标志
//    GetLostLineFlags(l_border, r_border, Left_Lost_Flag, Right_Lost_Flag);
//    Island_Detect();
////    RoundaboutProcess(bin_image, l_border, r_border, up_and_down, &state);
//
//
//    for (i = hightest_local; i < image_h - 1; i++) {
//        center_line[i] = (l_border[i] + r_border[i]) >> 1;
//    }
//
//    ips200_displayimage03x(bin_image[0], MT9V03X_W, MT9V03X_H);
//    for (i = 0; i < data_stastics_l; i++) {
//        ips200_draw_point(limit_a_b(points_l[i][0] + 2, 0, MT9V03X_W - 1),
//                         limit_a_b(points_l[i][1], 0, MT9V03X_H - 1), uesr_BLUE);
//    }
//    for (i = 0; i < data_stastics_r; i++) {
//        ips200_draw_point(limit_a_b(points_r[i][0] - 2, 0, MT9V03X_W - 1),
//                         limit_a_b(points_r[i][1], 0, MT9V03X_H - 1), uesr_RED);
//    }
//    for (i = hightest_local; i < image_h - 1; i++) {
//        ips200_draw_point(center_line[i], i, uesr_GREEN);
//    }
//
//}
//
//
////鏄剧ず鍥惧儚   鏀规垚浣犺嚜宸辩殑灏辫 绛夊悗鏈熻冻澶熻嚜淇′簡锛屾樉绀哄叧鎺夛紝鏄剧ず灞忔尯鍗犺祫婧愮殑
////ips154_displayimage032_zoom(bin_image[0], image_w, image_h, image_w, image_h,0,0);
//ips200_displayimage03x(bin_image[0], MT9V03X_W, MT9V03X_H);
//    //鏍规嵁鏈€缁堝惊鐜鏁扮敾鍑鸿竟鐣岀偣
//    for (i = 0; i < data_stastics_l; i++)
//    {
//        ips200_draw_point(limit_a_b(points_l[i][0]+2, 0, MT9V03X_W - 1), limit_a_b(points_l[i][1], 0, MT9V03X_H - 1), uesr_BLUE);//鏄剧ず璧风偣
//    }
//    for (i = 0; i < data_stastics_r; i++)
//    {
//        ips200_draw_point(limit_a_b(points_r[i][0]-2, 0 , MT9V03X_W - 1), limit_a_b(points_r[i][1], 0, MT9V03X_H - 1), uesr_RED);//鏄剧ず璧风偣
//    }
//
//    for (i = hightest; i < image_h-1; i++)
//    {
//        center_line[i] = (l_border[i] + r_border[i]) >> 1;//姹備腑绾�
//        //姹備腑绾挎渶濂芥渶鍚庢眰锛屼笉绠℃槸琛ョ嚎杩樻槸鍋氱姸鎬佹満锛屽叏绋嬫渶濂戒娇鐢ㄤ竴缁勮竟绾匡紝涓嚎鏈€鍚庢眰鍑猴紝涓嶈兘骞叉壈鏈€鍚庣殑杈撳嚭
//        //褰撶劧涔熸湁澶氱粍杈圭嚎鐨勬壘娉曪紝浣嗘槸涓汉鎰熻寰堢箒鐞愶紝涓嶅缓璁�
//        ips200_draw_point(center_line[i], i, uesr_GREEN);//鏄剧ず璧风偣 鏄剧ず涓嚎
////        ips200_draw_point(l_border[i], i, uesr_GREEN);//鏄剧ず璧风偣 鏄剧ず宸﹁竟绾�
////        ips200_draw_point(r_border[i], i, uesr_GREEN);//鏄剧ず璧风偣 鏄剧ず鍙宠竟绾�
//    }
//
//
//}
//
//
//int get_curve_info(uint8_t* center_line, int start_row, int end_row, double* a0, double* a1, double* a2, double* curvature) {
//    if (end_row - start_row < 3) return -1;
//
//    double coeff[3];
//    int count = end_row - start_row + 1;
//    double S0 = 0, S1 = 0, S2 = 0, S3 = 0, S4 = 0;
//    double T0 = 0, T1 = 0, T2 = 0;
//
//    for (int y = start_row; y <= end_row; y++) {
//        int x = center_line[y];
//        S0 += 1;
//        S1 += y;
//        S2 += y * y;
//        S3 += y * y * y;
//        S4 += y * y * y * y;
//        T0 += x;
//        T1 += x * y;
//        T2 += x * y * y;
//    }
//
//    double A[3][3] = {{S4, S3, S2}, {S3, S2, S1}, {S2, S1, S0}};
//    double B[3] = {T2, T1, T0};
//    int n = 3;
//
//    for (int i = 0; i < n; i++) {
//        int pivot = i;
//        double max_val = fabs(A[i][i]);
//        for (int r = i + 1; r < n; r++) {
//            if (fabs(A[r][i]) > max_val) {
//                max_val = fabs(A[r][i]);
//                pivot = r;
//            }
//        }
//        if (fabs(A[pivot][i]) < 1e-10) return -1;
//        if (pivot != i) {
//            for (int j = i; j < n; j++) {
//                double temp = A[i][j];
//                A[i][j] = A[pivot][j];
//                A[pivot][j] = temp;
//            }
//            double temp = B[i];
//            B[i] = B[pivot];
//            B[pivot] = temp;
//        }
//        for (int r = i + 1; r < n; r++) {
//            double factor = A[r][i] / A[i][i];
//            for (int j = i; j < n; j++) {
//                A[r][j] -= factor * A[i][j];
//            }
//            B[r] -= factor * B[i];
//        }
//    }
//    for (int i = n - 1; i >= 0; i--) {
//        coeff[i] = B[i];
//        for (int j = i + 1; j < n; j++) {
//            coeff[i] -= A[i][j] * coeff[j];
//        }
//        coeff[i] /= A[i][i];
//    }
//
//    *a2 = coeff[0];
//    *a1 = coeff[1];
//    *a0 = coeff[2];
//
//    double slope = 2 * (*a2) * LOOK_AHEAD_Y + (*a1);
//    *curvature = fabs(2 * (*a2)) / pow(1 + slope * slope, 1.5);
//    return 0;
//}
//
//// 閬撹矾瀹藉害瓒嬪娍鍒嗘瀽
//int analyze_width_trend(int* trend_score) {
//    *trend_score = 0;
//    int valid_samples = 0;
//    const uint8_t sample_interval = 5;
//
//    for (uint8_t y = MT9V03X_H - 1; y > hightest + sample_interval; y -= sample_interval) {
//        int width1 = r_border[y] - l_border[y];
//        int width2 = r_border[y - sample_interval] - l_border[y - sample_interval];
//        int diff = width2 - width1;
//        int std_diff = abs(width1 - Standard_Road_Wide1[y]);
//
//        if (std_diff < WIDTH_VARIATION_THRESH) continue;
//        *trend_score += diff;
//        valid_samples++;
//    }
//    if (valid_samples > 0) {
//        *trend_score /= valid_samples;
//        return valid_samples;
//    }
//    return 0;
//}
//
//// 杈圭晫璺冲彉妫€娴�
//int detect_border_jump(int* direction) {
//    *direction = 0;
//    int left_jump = 0, right_jump = 0;
//    const uint8_t jump_threshold = 25;
//
//    for (uint8_t y = MT9V03X_H - 20; y > hightest+40; y -= 5) {
//        if (y + 5 >= MT9V03X_H) continue;
//        int left_diff = abs(l_border[y] - l_border[y + 5]);
//        int right_diff = abs(r_border[y] - r_border[y + 5]);
//        if (left_diff > jump_threshold) left_jump++;
//        if (right_diff > jump_threshold) right_jump++;
//    }
//
//    if (left_jump > 2 && right_jump <= 1) {
//        *direction = -1;
//        return 1;
//    }
//    if (right_jump > 2 && left_jump <= 1) {
//        *direction = 1;
//        return 1;
//    }
//    return 0;
//}
//
//
//RoadType recognize_road_type(void) {
//    if (data_stastics_l < MIN_VALID_ROWS || data_stastics_r < MIN_VALID_ROWS) {
//        return current_road_type;
//    }
//    if (get_curve_info(center_line, hightest, MT9V03X_H - 30, &a0, &a1, &a2, &curvature) != 0) {
//        return current_road_type;
//    }
//
//    int trend_score;
//    int valid_samples = analyze_width_trend(&trend_score);
//
//    int jump_direction;
//    int has_jump = detect_border_jump(&jump_direction);
//
//    if (curvature < CURVATURE_THRESHOLD &&
//        abs(trend_score) < 15 &&
//        !has_jump) {
//        if (++straight_counter > STRAIGHT_CONFIRM_COUNT) {
//            current_road_type = STRAIGHT;
//        }
//    } else {
//        straight_counter = 0;
//        if (curvature > CURVATURE_THRESHOLD || valid_samples > 3 || has_jump) {
//            if (a2 > 0 || trend_score < -15 || jump_direction == -1) {
//                current_road_type = LEFT_CURVE;
//            } else if (a2 < 0 || trend_score > 15 || jump_direction == 1) {
//                current_road_type = RIGHT_CURVE;
//            } else {
//                current_road_type = UNKNOWN;
//            }
//        }
//    }
////    printf("Type: %d, Curvature: %f\n", current_road_type,curve.curvature);
//    return current_road_type;
//}
//
//
//
///*
//
//杩欓噷鏄捣鐐癸紙0.0锛�***************鈥斺€�>*************x鍊兼渶澶�
//************************************************************
//************************************************************
//************************************************************
//************************************************************
//******************鍋囧杩欐槸涓€鍓浘鍍�*************************
//***********************************************************
//***********************************************************
//***********************************************************
//***********************************************************
//***********************************************************
//***********************************************************
//y鍊兼渶澶�*******************************************图像大小(188.120)
//
//*/
