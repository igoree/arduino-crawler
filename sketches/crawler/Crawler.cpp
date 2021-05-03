#include "Crawler.h"
#include "ProtocolParser.h"
#include "Sounds.h"
#include "debug.h"

//MotorDriverBoard V4.0
Crawler::Crawler(ProtocolParser *protocolParser)
{
  Addr = 0x01;
  mStatus = E_STOP;
  BatteryValue = 0;
  Speed = 0 ;
  Degree = 0;
  LeftFoward = RightFoward = LeftBackward = RightBackward = NULL;
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
  mProtocolPackage = protocolParser;
}

Crawler::~Crawler()
{
  delete LeftFoward;
  delete RightFoward;
  delete LeftBackward;
  delete RightBackward;
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

void Crawler::Init(int leftward, int rightfoward)
{
  MotorDriver = Emakefun_MotorDriver(0x60, MOTOR_DRIVER_BOARD_V5);
  Sensors = (Emakefun_Sensor *)MotorDriver.getSensor(E_SENSOR_MAX);
  LeftFoward = MotorDriver.getMotor(leftward);
  RightFoward = MotorDriver.getMotor(rightfoward);
  delay(200);
  MotorDriver.begin(50);
}


void Crawler::Move(int directions = 1)
{
  if (directions == 1) {
    GoForward();
  } else if (directions == 2) {
    GoBack();
  } else if (directions == 3) {
    TurnLeft();
  } else if (directions == 4) {
    TurnRight();
  } else {
    KeepStop();
  }
}

void Crawler::DriveSpeed(int s)
{
  if (s >= 0 && s <= 100) {
    LeftFoward->setSpeed((s / 10) * 25.5);
    RightFoward->setSpeed((s / 10) * 25.5);
  }
}

void Crawler::GoForward(void)
{
  SetStatus(E_FORWARD);
  DriveSpeed(Speed);
  LeftFoward->run(FORWARD);
  RightFoward->run(FORWARD);
}

void Crawler::GoBack(void)
{
  SetStatus(E_BACK);
  DriveSpeed(Speed);
  LeftFoward->run(BACKWARD);  
  RightFoward->run(BACKWARD);
}
void Crawler::KeepStop(void)
{
  SetStatus(E_STOP);
  LeftFoward->run(BRAKE);
  RightFoward->run(BRAKE);
}

void Crawler::TurnLeft(void)
{
  int s;
  s = (Speed / 10) * 25.5;
  SetStatus(E_LEFT);
  LeftFoward->setSpeed(s / 3);
  RightFoward->setSpeed(s);
  LeftFoward->run(FORWARD);
  RightFoward->run(FORWARD);
}

void Crawler::TurnRight(void)
{
  int s;
  SetStatus(E_RIGHT);
  s = (Speed / 10) * 25.5;
  LeftFoward->setSpeed(s);
  RightFoward->setSpeed(s / 3);
  LeftFoward->run(FORWARD);
  RightFoward->run(FORWARD);
}

void Crawler::TurnLeftRotate(void)
{
  SetStatus(E_LEFT_ROTATE);
  DriveSpeed(Speed);
  LeftFoward->run(BACKWARD);
  RightFoward->run(FORWARD);
}

void Crawler::TurnRightRotate(void)
{
  SetStatus(E_RIGHT_ROTATE);
  DriveSpeed(Speed);
  LeftFoward->run(FORWARD);
  RightFoward->run(BACKWARD);
}

void Crawler::Drive(void)
{
  Drive(Degree);
}

void Crawler::Drive(int degree)
{
  DEBUG_LOG(DEBUG_LEVEL_INFO, "degree = %d speed = %d\n", degree, Speed);
  byte value = (Speed / 10) * 25.5;	 //app contol hbot_speed is 0 ~ 100 ,pwm is 0~255
  float f;
  if (degree >= 0 && degree <= 90) {
    f = (float)(degree) / 90;
    if (degree <= 5) {
      SetStatus(E_RIGHT);
    } else  if (degree >= 85) {
      SetStatus(E_FORWARD);
    }
    LeftFoward->setSpeed(value);
    RightFoward->setSpeed((float)(value * f));
    LeftFoward->run(FORWARD);
    RightFoward->run(FORWARD);
  } else if (degree > 90 && degree <= 180) {
    f = (float)(180 - degree) / 90;
    if (degree <= 95) {
      SetStatus(E_FORWARD);
    } else  if (degree >= 175) {
      SetStatus(E_LEFT);
    }
    LeftFoward->setSpeed((float)(value * f));
    RightFoward->setSpeed(value);
    LeftFoward->run(FORWARD);
    RightFoward->run(FORWARD);
  } else if (degree > 180 && degree <= 270) {
    f = (float)(degree - 180) / 90;
    if (degree <= 185) {
      SetStatus(E_LEFT);
    } else  if (degree >= 265) {
      SetStatus(E_BACK);
    }
    LeftFoward->setSpeed((float)(value * f));
    RightFoward->setSpeed(value);
    LeftFoward->run(BACKWARD);
    RightFoward->run(BACKWARD);
  } else if (degree > 270 && degree <= 360) {
    f = (float)(360 - degree) / 90;
    if (degree <= 275) {
      SetStatus(E_BACK);
    } else  if (degree >= 355) {
      SetStatus(E_RIGHT);
    }
    LeftFoward->setSpeed(value);
    RightFoward->setSpeed((float)(value * f));
    LeftFoward->run(BACKWARD);
    RightFoward->run(BACKWARD);
  }
  else {
    KeepStop();
    return;
  }
}

void Crawler::SetSpeed(int8_t s)
{
    // DEBUG_LOG(DEBUG_LEVEL_INFO, "SetSpeed =%d \n", s);
    if (s > 100) {
        Speed = 100;
        return;
    } else if (s < 0) {
        Speed = 0;
        return;
    }
    Speed = s;
}

int Crawler::GetSpeed(void)
{
    return Speed;
}

void Crawler::SpeedUp(int8_t Duration = 5)
{
    SetSpeed(Speed + Duration);
    mStatus = E_SPEED_UP;
}

void Crawler::SpeedDown(int8_t Duration = 5)
{
    SetSpeed(Speed - Duration);
    mStatus = E_SPEED_DOWN;
}

void Crawler::SetStatus(E_CRAWLER_STATUS status=0)
{
    mStatus = status;
}

E_CRAWLER_STATUS Crawler::GetStatus(void)
{
    return mStatus;
}

uint8_t Crawler::GetBattery(void)
{
    return BatteryValue;
}

void Crawler::InitIr(void)
{
  IR = MotorDriver.getSensor(E_IR);
}

void Crawler::InitBuzzer(void)
{
  Buzzer = MotorDriver.getSensor(E_BUZZER);
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
  Rgb = MotorDriver.getSensor(E_RGB);
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
  MotorDriver.getSensor(E_ULTRASONIC);
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
  mServo1 = MotorDriver.getServo(1);
  mServo2 = MotorDriver.getServo(2);
  mServo3 = MotorDriver.getServo(3);
  mServo4 = MotorDriver.getServo(4);
  mServo5 = MotorDriver.getServo(5);
  mServo6 = MotorDriver.getServo(6);
}

void Crawler::SetServoBaseDegree(uint8_t base)
{
  ServoBaseDegree = base;
}

void Crawler::SetServoDegree(byte pin, byte angle)
{
  int servo_degree;
  if (angle > 360) {
    return;
  }
  if (angle == 90 || angle == 270) {
    servo_degree = ServoBaseDegree;
  } else if (angle >= 0 && angle <= 180) {
    servo_degree = ServoBaseDegree - 90 + angle;   // 180-degree-diff
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
  Nrf24L01 = MotorDriver.getSensor(E_NRF24L01);
  Nrf24L01->setRADDR(Rxaddr);
}
