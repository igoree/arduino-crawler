#ifndef  _PROTOCOL_H_
#define  _PROTOCOL_H_
#include  "Arduino.h"

#define PROTOCOL_START_CODE 0xAA
#define PROTOCOL_END_CODE   0x55

typedef enum
{
    E_BATTERY = 1,
    E_LED,
    E_BUZZER_MODE,
    E_INFO,
    E_ROBOT_CONTROL_DIRECTION,
    E_ROBOT_CONTROL_SPEED,
    E_TEMPERATURE,
    E_INFRARED_TRACKING,
    E_ULTRASONIC_MODE,
    E_INFRARED_REMOTE,
    E_INFRARED_AVOIDANCE,
    E_CONTROL_MODE,  //12
    E_BUTTON = 13,
    E_LED_MAXTRIX,
    E_CMD_LINE,
    E_VERSION,
    E_UPGRADE,
    E_PHOTORESISTOR,
    E_SERVER_DEGREE,
    E_CONTOROL_CODE_MAX,
} E_CONTOROL_FUNC;

typedef struct
{
    byte start_code ;   // 8bit 0xAA
    byte len;           // protocol data length
    //E_TYPE type;
    byte addr;
    byte function;      // 8 bit
    byte *data;         // n bit
    uint16_t sum;       // check sum 16bit
    byte end_code;      // 8bit 0x55
} ST_PROTOCOL;

#endif // _PROTOCOL_H_