#ifndef CODE_KEY_H_
#define CODE_KEY_H_

#include "zf_common_headfile.h"

//°´¼üºê¶¨Òå
//#define key1 P21_2
//#define key2 P21_3
//#define key3 P11_6
//#define key4 P11_9

#define key1 P20_8
#define key2 P21_3

#define key3 P21_7
#define key4 P20_2
#define key5 P20_9
#define key6 P20_7
#define key7 P20_6



extern uint8 key1_flag;
extern uint8 key2_flag;
extern uint8 key3_flag;
extern uint8 key4_flag;
extern uint8 key5_flag;
extern uint8 key6_flag;
extern uint8 key7_flag;
extern uint8 key8_flag;

void mykey_init(void);
void Key_Num_show();
void key_set(void);
#endif /* CODE_KEY_H_ */
