#ifndef _PANTHER_TANK_H_
#define _PANTHER_TANK_H_
#include <stdint.h>
#include "SPI.h"
#include "Arduino.h"
#include "Emakefun_MotorDriver.h"
#include "ProtocolParser.h"

#define ULTRASONIC_SERVO 1

typedef enum : uint8_t
{
    E_FORWARD = 0,
    E_BACK,
    E_LEFT,
    E_RIGHT,
    E_RIGHT_ROTATE,
    E_LEFT_ROTATE,
    E_STOP,
    E_RUNNING,
    E_SPEED_UP,
    E_SPEED_DOWN,
    E_LOW_POWER,
} E_CRAWLER_STATUS;

typedef enum : uint8_t
{
  E_SERVO_FRONT,
  E_SERVO_LEFT,
  E_SERVO_RIGHT
} E_SERVO_DIRECTION;

typedef enum : uint8_t
{
  E_EFFECT_BREATHING = 0,
  E_EFFECT_ROTATE = 1,
  E_EFFECT_FLASH = 2
} E_RGB_EFFECT;

class Crawler {

  private :
    byte mAddr;
    E_CRAWLER_STATUS mStatus;
    uint8_t mBateryPin;
    uint8_t mBatteryValue;
    uint8_t mServoBaseDegree;
    ST_PROTOCOL mSendData;
    ProtocolParser *mProtocolParser;
    uint8_t mSpeed;
    int mDegree;
    Emakefun_MotorDriver mMotorDriver;
    Emakefun_DCMotor *mLeftDrive, *mRightDrive;
    Emakefun_Servo *mServo1, *mServo2, *mServo3, *mServo4, *mServo5, *mServo6;

    void StopDrive(Emakefun_DCMotor* drive);
    void RunDrive(Emakefun_DCMotor* drive, uint8_t speed, uint8_t direction);
    void SetStatus(E_CRAWLER_STATUS status);

  public :
    IRremote *IR;
    Buzzer *Buzzer;
    RGBLed *Rgb;
    Emakefun_Sensor *Sensors;
    Nrf24l *Nrf24L01;
    
    Crawler(ProtocolParser *protocolParser);
    ~Crawler(void);
    void Init(int leftDrive, int rightDrive);
    void GoForward(void);
    void GoBack(void);
    void TurnLeft(void);
    void TurnRight(void);
    void TurnLeftRotate(void);
    void TurnRightRotate(void);
    void Stop(void);
    void Drive(void);
    void Drive(int degree);
    void SetSpeed(int8_t s);
    bool SpeedUp(int8_t delta = 5);
    bool SpeedDown(int8_t delta = 5);
    int  GetSpeed(void);
    E_CRAWLER_STATUS GetStatus(void);
    uint8_t GetBattery(void);
    
    void InitIr(void);
    
    void InitBuzzer(void);
    void Sing(byte songName);
    void PianoSing(ST_MUSIC_TYPE music);
    
    void InitRgb(void);
    void SetRgbColor(E_RGB_INDEX index , long Color);
    void SetRgbEffect(E_RGB_INDEX index, long Color, uint8_t effect);
    void LightOff(void);
    
    void InitUltrasonic(void);
    byte GetUltrasonicValue(E_SERVO_DIRECTION);
    
    void InitServo(void);
    void SetServoBaseDegree(uint8_t base);
    void SetServoDegree(byte pin, byte angle);
    
    void InitNrf24L01(char *Rxaddr);
};

#endif  /* _AURORARACING_H_ */
