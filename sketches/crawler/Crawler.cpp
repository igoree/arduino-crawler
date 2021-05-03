#include "Crawler.h"
#include "ProtocolParser.h"
#include "Sounds.h"
#include "debug.h"

//MotorDriverBoard V4.0
Crawler::Crawler(ProtocolParser *protocolParser)
{
  mAddr = 0x01;
  mStatus = E_STOP;
  mBatteryValue = 0;
  mSpeed = 0 ;
  mDegree = 0;
  mLeftDrive = mRightDrive = NULL;
  IR = NULL;
  Buzzer  = NULL;
  Rgb = NULL;
  Nrf24L01 = NULL;
  mServo1 = NULL;
  mServo2 = NULL;
  mServo3 = NULL;
  mServo4 = NULL;
  mServo5 = NULL;
  mServo6 = NULL;
  SetStatus(E_STOP);
  mProtocolParser = protocolParser;
}

Crawler::~Crawler()
{
  delete mLeftDrive;
  delete mRightDrive;
  delete IR;
  delete Buzzer;
  delete Rgb;
  delete Nrf24L01;
  delete mServo1;
  delete mServo2;
  delete mServo3;
  delete mServo4;
  delete mServo5;
  delete mServo6;
}

void Crawler::Init(int leftDrive, int rightDrive)
{
  mMotorDriver = Emakefun_MotorDriver(0x60, MOTOR_DRIVER_BOARD_V5);
  Sensors = (Emakefun_Sensor *)mMotorDriver.getSensor(E_SENSOR_MAX);
  mLeftDrive = mMotorDriver.getMotor(leftDrive);
  mRightDrive = mMotorDriver.getMotor(rightDrive);
  delay(200);
  mMotorDriver.begin(50);
}

void Crawler::StopDrive(Emakefun_DCMotor* drive) 
{
  drive->run(BRAKE);
}

void Crawler::RunDrive(Emakefun_DCMotor* drive, uint8_t speed, uint8_t direction)
{
  if (speed >= 0 && speed <= 100) {
    drive->setSpeed((speed / 10) * 25.5);
    drive->run(direction);
    return;
  }
  
  DEBUG_ERR("Invalid speed: %d\r\n", speed);
}

void Crawler::GoForward(void)
{
  SetStatus(E_FORWARD);
  RunDrive(mLeftDrive, mSpeed, FORWARD);
  RunDrive(mRightDrive, mSpeed, FORWARD);
}

void Crawler::GoBack(void)
{
  SetStatus(E_BACK);
  RunDrive(mLeftDrive, mSpeed, BACKWARD);
  RunDrive(mRightDrive, mSpeed, BACKWARD);
}

void Crawler::Stop(void)
{
  SetStatus(E_STOP);
  StopDrive(mLeftDrive);
  StopDrive(mRightDrive);
}

void Crawler::TurnLeft(void)
{
  SetStatus(E_LEFT);
  RunDrive(mLeftDrive, mSpeed / 3, FORWARD);
  RunDrive(mRightDrive, mSpeed, FORWARD);
}

void Crawler::TurnRight(void)
{
  SetStatus(E_RIGHT);
  RunDrive(mLeftDrive, mSpeed, FORWARD);
  RunDrive(mRightDrive, mSpeed / 3, FORWARD);
}

void Crawler::TurnLeftRotate(void)
{
  SetStatus(E_LEFT_ROTATE);
  RunDrive(mLeftDrive, mSpeed, BACKWARD);
  RunDrive(mRightDrive, mSpeed, FORWARD);
}

void Crawler::TurnRightRotate(void)
{
  SetStatus(E_RIGHT_ROTATE);
  RunDrive(mLeftDrive, mSpeed, FORWARD);
  RunDrive(mRightDrive, mSpeed, BACKWARD);
}

void Crawler::Drive(void)
{
  Drive(mDegree);
}

void Crawler::Drive(int degree)
{
  DEBUG_INFO( "degree = %d speed = %d\n", degree, mSpeed);
  byte value = (mSpeed / 10) * 25.5;	 //app contol hbot_speed is 0 ~ 100 ,pwm is 0~255
  float f;
  if (degree >= 0 && degree <= 90) {
    f = (float)(degree) / 90;
    if (degree <= 5) {
      SetStatus(E_RIGHT);
    } else  if (degree >= 85) {
      SetStatus(E_FORWARD);
    }
    RunDrive(mLeftDrive, value, FORWARD);
    RunDrive(mRightDrive, value * f, FORWARD);
  } else if (degree > 90 && degree <= 180) {
    f = (float)(180 - degree) / 90;
    if (degree <= 95) {
      SetStatus(E_FORWARD);
    } else  if (degree >= 175) {
      SetStatus(E_LEFT);
    }
    RunDrive(mLeftDrive, value * f, FORWARD);
    RunDrive(mRightDrive, value, FORWARD);
  } else if (degree > 180 && degree <= 270) {
    f = (float)(degree - 180) / 90;
    if (degree <= 185) {
      SetStatus(E_LEFT);
    } else  if (degree >= 265) {
      SetStatus(E_BACK);
    }
    RunDrive(mLeftDrive, value * f, BACKWARD);
    RunDrive(mRightDrive, value, BACKWARD);
  } else if (degree > 270 && degree <= 360) {
    f = (float)(360 - degree) / 90;
    if (degree <= 275) {
      SetStatus(E_BACK);
    } else  if (degree >= 355) {
      SetStatus(E_RIGHT);
    }
    RunDrive(mLeftDrive, value, BACKWARD);
    RunDrive(mRightDrive, value * f, BACKWARD);
  }
  else {
    Stop();
    return;
  }
}

void Crawler::SetSpeed(int8_t s)
{
  if (s > 100) {
    mSpeed = 100;
    return;
  } else if (s < 0) {
    mSpeed = 0;
    return;
  }
  mSpeed = s;
}

int Crawler::GetSpeed(void)
{
   return mSpeed;
}

bool Crawler::SpeedUp(int8_t delta = 5)
{
  mStatus = E_SPEED_UP;
  uint8_t oldSpeed = mSpeed;
  SetSpeed(mSpeed + delta);

  return oldSpeed != mSpeed;
}

bool Crawler::SpeedDown(int8_t delta = 5)
{
  mStatus = E_SPEED_DOWN;  
  uint8_t oldSpeed = mSpeed;
  SetSpeed(mSpeed - delta);

  return oldSpeed != mSpeed;
}

void Crawler::SetStatus(E_CRAWLER_STATUS status)
{
    mStatus = status;
}

E_CRAWLER_STATUS Crawler::GetStatus(void)
{
    return mStatus;
}

uint8_t Crawler::GetBattery(void)
{
    return mBatteryValue;
}

void Crawler::InitIr(void)
{
  IR = mMotorDriver.getSensor(E_IR);
}

void Crawler::InitBuzzer(void)
{
  Buzzer = mMotorDriver.getSensor(E_BUZZER);
}

void Crawler::Sing(byte songName)
{
  Sensors->Sing(songName);
}

void Crawler::PianoSing(ST_MUSIC_TYPE music)
{
  Buzzer->_tone(music.note, music.beat, 2);
}

void Crawler::InitRgb(void)
{
  Rgb = mMotorDriver.getSensor(E_RGB);
}

void Crawler::SetRgbColor(E_RGB_INDEX index , long Color)
{
  Sensors->SetRgbColor(index, Color);
}

void Crawler::LightOff(void)
{
  Sensors->SetRgbColor(E_RGB_ALL, RGB_BLACK);
}

void Crawler::SetRgbEffect(E_RGB_INDEX index, long Color, uint8_t effect)
{
  if (Rgb != NULL) {
    switch ((E_RGB_EFFECT)effect) {
      case E_EFFECT_BREATHING:
        for (long i = 0; i < 256; i++) {
          SetRgbColor(index, (i << 16) | (i << 8) | i);
          delay((i < 18) ? 18 : (256 / i));
        }
        for (long i = 255; i >= 0; i--) {
          SetRgbColor(index, (i << 16) | (i << 8) | i);
          delay((i < 18) ? 18 : (256 / i));
        }
        break;
      case E_EFFECT_FLASH:
        for (byte i = 0; i < 6; i++) {
          SetRgbColor(index, Color);
          delay(100);
          SetRgbColor(index, 0);
          delay(100);
        }
        break;
    }
  }
}

void Crawler::InitUltrasonic(void)
{
  mMotorDriver.getSensor(E_ULTRASONIC);
}

byte Crawler::GetUltrasonicValue(E_SERVO_DIRECTION direction)
{
  byte distance;
  if (direction == E_SERVO_FRONT) {
    SetServoDegree(ULTRASONIC_SERVO, 90);
    distance = Sensors->GetUltrasonicDistance();
  } else if (direction == E_SERVO_LEFT) {
    SetServoDegree(ULTRASONIC_SERVO, 180);
    distance = Sensors->GetUltrasonicDistance();
    delay(400);
    SetServoDegree(ULTRASONIC_SERVO, 90);
  } else if (direction == E_SERVO_RIGHT) {
    SetServoDegree(ULTRASONIC_SERVO, 15);
    distance = Sensors->GetUltrasonicDistance();
    delay(400);
    SetServoDegree(ULTRASONIC_SERVO, 90);
  }
  return distance;
}

void Crawler::InitServo(void)
{
  mServo1 = mMotorDriver.getServo(1);
  mServo2 = mMotorDriver.getServo(2);
  mServo3 = mMotorDriver.getServo(3);
  mServo4 = mMotorDriver.getServo(4);
  mServo5 = mMotorDriver.getServo(5);
  mServo6 = mMotorDriver.getServo(6);
}

void Crawler::SetServoBaseDegree(uint8_t base)
{
  mServoBaseDegree = base;
}

void Crawler::SetServoDegree(byte pin, byte angle)
{
  int servo_degree;
  if (angle > 360) {
    return;
  }
  if (angle == 90 || angle == 270) {
    servo_degree = mServoBaseDegree;
  } else if (angle >= 0 && angle <= 180) {
    servo_degree = mServoBaseDegree - 90 + angle;   // 180-degree-diff
  }
  if (pin == 1)
    mServo1->writeServo(servo_degree);
  else if (pin == 2)
    mServo2->writeServo(servo_degree);
  else if (pin == 3)
    mServo3->writeServo(servo_degree);
  else if (pin == 4)
    mServo4->writeServo(servo_degree);
  else if (pin == 5)
    mServo5->writeServo(servo_degree);
  else if (pin == 6)
    mServo6->writeServo(servo_degree);
}

void Crawler::InitNrf24L01(char *Rxaddr)
{
  Nrf24L01 = mMotorDriver.getSensor(E_NRF24L01);
  Nrf24L01->setRADDR(Rxaddr);
}
