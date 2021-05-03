#include "debug.h"
#include "Emakefun_MotorDriver.h"
#include "Crawler.h"
#include "ProtocolParser.h"
#include "IRKeyMap.h"

ProtocolParser *mProtocol = new ProtocolParser();
Crawler mCrawler(mProtocol);
byte count = 0;

void setup()
{
  Serial.begin(9600);
  mCrawler.Init(M2, M1);
  mCrawler.InitServo();
  mCrawler.InitRgb();
  mCrawler.InitBuzzer();
  mCrawler.InitIr();
  mCrawler.SetSpeed(50);
  mCrawler.SetServoBaseDegree(90);
  mCrawler.SetServoDegree(ULTRASONIC_SERVO, 90);
  mCrawler.InitUltrasonic();
  Serial.println("init ok");
  mCrawler.Sing(S_connection);
}

void HandleInfaredRemote (byte irKeyCode)
{
  switch ((E_IR_KEYCODE)mCrawler.IR->getIrKey(irKeyCode)) {
    case IR_KEYCODE_STAR:
      mCrawler.Sing(S_connection);
      mCrawler.SetRgbColor(E_RGB_ALL, mCrawler.GetSpeed() * 2.5);
      mCrawler.SpeedUp(10);
      DEBUG_LOG(DEBUG_LEVEL_INFO, "Speed = %d \n", mCrawler.GetSpeed());
      break;
    case IR_KEYCODE_POUND:
      mCrawler.Sing(S_disconnection);
      mCrawler.SetRgbColor(E_RGB_ALL, mCrawler.GetSpeed() * 2.5);
      mCrawler.SpeedDown(10);
      break;
    case IR_KEYCODE_UP:
      mCrawler.GoForward();
      break;
    case IR_KEYCODE_DOWN:
      mCrawler.GoBack();
      break;
    case IR_KEYCODE_OK:
      mCrawler.KeepStop();
      break;
    case IR_KEYCODE_LEFT:
      mCrawler.TurnLeft();
      break;
    case IR_KEYCODE_RIGHT:
      mCrawler.TurnRight();
      break;
    default:
      break;
  }
}

void HandleUltrasonicAvoidance(void)
{
  uint16_t UlFrontDistance, UlLeftDistance, UlRightDistance;
  UlFrontDistance = mCrawler.GetUltrasonicValue(0);
  if (count++ > 50) {
    //mCrawler.SendUltrasonicData();
    count = 0;
  }
  DEBUG_LOG(DEBUG_LEVEL_INFO, "UlFrontDistance = %d \n", UlFrontDistance);
  if (UlFrontDistance < UL_LIMIT_MIN)
  {
    mCrawler.SetSpeed(80);
    mCrawler.GoBack();
    delay(200);
  }
  if (UlFrontDistance < UL_LIMIT_MID)
  {
    mCrawler.KeepStop();
    delay(100);
    UlRightDistance = mCrawler.GetUltrasonicValue(2);
    delay(50);
    UlLeftDistance = mCrawler.GetUltrasonicValue(1);
    if ((UlRightDistance > UL_LIMIT_MIN) && (UlRightDistance < UL_LIMIT_MAX)) {
      mCrawler.SetSpeed(100);
      mCrawler.TurnRight();
      delay(400);
    } else if ((UlLeftDistance > UL_LIMIT_MIN) && (UlLeftDistance < UL_LIMIT_MAX)) {
      mCrawler.SetSpeed(100);
      mCrawler.TurnLeft();
      delay(400);
    } else if ((UlRightDistance < UL_LIMIT_MIN) && (UlLeftDistance < UL_LIMIT_MIN) ) {
      mCrawler.SetSpeed(400);
      mCrawler.TurnLeft();
      delay(800);
    }
  } else {
    mCrawler.SetSpeed(80);
    mCrawler.GoForward();
  }
}

void UltrasonicFollow()
{
  mCrawler.SetSpeed(40);
  uint16_t UlFrontDistance =  mCrawler.GetUltrasonicValue(E_SERVO_FRONT);
  delay(10);
  if (UlFrontDistance < 10) {
    mCrawler.GoBack();
  } else if (UlFrontDistance > 14) {
    mCrawler.GoForward();
  } else if (10 <= UlFrontDistance <= 14) {
    mCrawler.KeepStop();
  }
}

void loop()
{
  static bool recv_flag;
  mProtocol->RecevData();
  if (recv_flag = mProtocol->ParserPackage()) {
    /*if (mProtocol->GetRobotControlFun() == E_CONTROL_MODE) {
    }*/
  }

  byte irKeyCode;
  if (irKeyCode = mCrawler.IR->getCode())
  {
    DEBUG_LOG(DEBUG_LEVEL_INFO, "irKeyCode = %lx \n", irKeyCode);
    HandleInfaredRemote(irKeyCode);
    delay(110);
  } else {
    if (mCrawler.GetStatus() != E_STOP ) {
      mCrawler.KeepStop();
    }
  }
  
  switch (mCrawler.GetStatus()) {
    case E_FORWARD:
      mCrawler.SetRgbColor(E_RGB_ALL, RGB_WHITE);
      break;

    case E_LEFT_ROTATE:
      mCrawler.SetRgbColor(E_RGB_LEFT, RGB_WHITE);
      break;
      
    case E_RIGHT_ROTATE:
      mCrawler.SetRgbColor(E_RGB_RIGHT, RGB_WHITE);
      break;
      
    case E_BACK:
      mCrawler.SetRgbColor(E_RGB_ALL, RGB_RED);
      break;
      
    case E_STOP:
      mCrawler.LightOff();
      break;
      
    default:
      break;
  }
}
