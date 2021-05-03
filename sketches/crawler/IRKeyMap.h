#ifndef _KEYMAY_H_
#define _KEYMAY_H_
#include <Arduino.h>

#define IR_KEY_COUNT 18

typedef struct
{
    String keyname;
    byte keycode;
} ST_IR_KEY_MAP;

typedef enum {
    IR_KEYCODE_1 = 0,
    IR_KEYCODE_2,
    IR_KEYCODE_3,
    IR_KEYCODE_4,
    IR_KEYCODE_5,
    IR_KEYCODE_6,
    IR_KEYCODE_7,
    IR_KEYCODE_8,
    IR_KEYCODE_9,
    IR_KEYCODE_0,
    IR_KEYCODE_STAR,      // *
    IR_KEYCODE_POUND,     // # 
    IR_KEYCODE_UP,
    IR_KEYCODE_DOWN,
    IR_KEYCODE_OK,
    IR_KEYCODE_LEFT,
    IR_KEYCODE_RIGHT,
} E_IR_KEYCODE;

extern ST_IR_KEY_MAP ir_keymap[];
#endif  /* _KEYMAY_H_ */
