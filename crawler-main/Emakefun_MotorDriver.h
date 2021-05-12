#ifndef _Emakefun_MotorDriver_h_
#define _Emakefun_MotorDriver_h_

#include <inttypes.h>
#include <Wire.h>
#include <SPI.h>
#include "Emakefun_MS_PWMServoDriver.h"
#include "IRRemote.h"
#include "Buzzer.h"
#include "RGBLed.h"
#include "nRF24L01.h"
//#define MOTORDEBUG

#define MOTOR_DRIVER_BOARD_V3 3
#define MOTOR_DRIVER_BOARD_V4 4
#define MOTOR_DRIVER_BOARD_V5 5

#define M1 1
#define M2 2
#define M3 3
#define M4 4

#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4

#define RGB_PIN A1
#define ECHO_PIN A3
#define TRIG_PIN A2
#define IR_PIN 8

#if (MOTOR_DRIVER_BOARD_VER == 3)
#define BUZZER_PIN 9
#else
#define BUZZER_PIN A0
#endif

#define NRF24L01_CE 10
#define NRF24L01_CSN 9

#define UL_LIMIT_MIN 16
#define UL_LIMIT_MID 20
#define UL_LIMIT_MAX 500

typedef enum
{
	E_RGB = 0,
	E_IR = 1,
	E_BUZZER = 2,
	// E_PS2X = 3,
	E_NRF24L01 = 4,
	E_ULTRASONIC = 5,
	E_EXTERN_PIN = 6,
	E_SENSOR_MAX,
} E_SENSOR_INDEX;

typedef void (*FuncPtr)(void);

class Emakefun_MotorDriver;

class Emakefun_Sensor
{
public:
	Emakefun_Sensor(void);
	friend class Emakefun_MotorDriver;
	IRRemote* mIrRecv;
	Buzzer* mBuzzer;
	RGBLed* mRgb;
	Nrf24l* mNrf24L01;
	uint16_t GetUltrasonicDistance(void);
	int  GetNrf24L01(char* RxaddrName);
	void sendNrf24l01(char* TxaddrName, int SendNrfData);
private:
	uint8_t IrPin;      // Infrared remoter pin
	uint8_t BuzzerPin;  // Buzzer pin
	uint8_t RgbPin;     // Rgb pin
	uint8_t EchoPin, TrigPin;
	int GetNrfData, SendNrfData;
	Emakefun_MotorDriver* MC;
};

class Emakefun_DCMotor
{
public:
	Emakefun_DCMotor(void);
	friend class Emakefun_MotorDriver;
	void run(uint8_t);
	void setSpeed(uint8_t);

private:
	uint8_t PWMpin, IN1pin, IN2pin;
	int DcSpeed;
	Emakefun_MotorDriver* MC;
	uint8_t motornum;
};

class Emakefun_Servo
{
public:
	Emakefun_Servo(void);
	friend class Emakefun_MotorDriver;
	void setServoPulse(double pulse);
	void writeServo(uint8_t angle);
	uint8_t readDegrees();
private:
	uint8_t PWMpin;
	Emakefun_MotorDriver* MC;
	uint8_t servonum, currentAngle;
};

class Emakefun_MotorDriver
{
public:
	Emakefun_MotorDriver(uint8_t addr = 0x60, uint8_t version = 4);
	uint8_t _version;
	void begin(uint16_t freq = 1600);
	void setPWM(uint8_t pin, uint16_t val);
	void setPin(uint8_t pin, boolean val);
	Emakefun_DCMotor* getMotor(uint8_t n);
	Emakefun_Servo* getServo(uint8_t n);
	void* getSensor(E_SENSOR_INDEX n);
private:
	uint8_t _addr;
	uint16_t _freq;
	Emakefun_DCMotor dcmotors[4];
	Emakefun_MS_PWMServoDriver _pwm;
#if (MOTOR_DRIVER_BOARD_VER == 3)
	Emakefun_Servo servos[4];
#else
	Emakefun_Servo servos[8];
#endif
	Emakefun_Sensor sensors;
};

#endif
