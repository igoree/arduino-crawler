#include "debug.h"
#include "Emakefun_MotorDriver.h"
#include "Crawler.h"
#include "ProtocolParser.h"
#include "BluetoothHandle.h"
#include "IRKeyMap.h"

ProtocolParser *mProtocol = new ProtocolParser();
Crawler mCrawler(mProtocol);
byte count = 0;

void setup()
{
  Serial.begin(9600);
  mCrawler.init(M2, M1);
  mCrawler.SetControlMode(E_INFRARED_REMOTE_CONTROL);
  mCrawler.InitServo();
  mCrawler.InitRgb();
  mCrawler.InitBuzzer();
  mCrawler.SetSpeed(50);
  mCrawler.SetServoBaseDegree(90);
  mCrawler.SetServoDegree(1, 90);
  mCrawler.InitUltrasonic();
  Serial.println("init ok");
  mCrawler.sing(S_connection);
}

void HandleBluetoothRemote(bool recv_flag)
{
  if (recv_flag) {
    switch (mProtocol->GetRobotControlFun()) {
      case E_BUTTON:
        switch (mProtocol->GetBluetoothButton())
        {
          case BT_PAD_UP:
            mCrawler.GoForward();
            break;
          case BT_PAD_DOWN:
            mCrawler.GoBack();
            break;
          case BT_PAD_LEFT:
            mCrawler.Drive(160);
            break;
          case BT_PAD_RIGHT:
            mCrawler.Drive(20);
            break;
          case BT_PINK:
            mCrawler.TurnLeftRotate();
            break;
          case BT_RED:
            mCrawler.TurnRightRotate();
            break;
          case BT_GREEN:
            mCrawler.sing(S_connection);
            mCrawler.SetRgbColor(E_RGB_ALL, mCrawler.GetSpeed() * 2.5);
            mCrawler.SpeedUp(10);
            break;
          case BT_BLUE:
            mCrawler.sing(S_disconnection);
            mCrawler.SetRgbColor(E_RGB_ALL, mCrawler.GetSpeed() * 2.5);
            mCrawler.SpeedDown(10);
            break;
        }
        break;
      case E_ROBOT_CONTROL_DIRECTION:
        mCrawler.Drive(mProtocol->GetRobotDegree());
        break;
      case E_ROBOT_CONTROL_SPEED:
        mCrawler.SetSpeed(mProtocol->GetRobotSpeed());
        break ;
      case E_CONTROL_MODE:
        mCrawler.SetControlMode(mProtocol->GetControlMode());
        break;
      case E_LED:
        mCrawler.SetRgbColor(E_RGB_ALL, mProtocol->GetRgbValue());
        break;
      case E_SERVER_DEGREE:
        mCrawler.SetServoDegree(mProtocol->GetServoDegreeNum(), mProtocol->GetServoDegree());
        break;
      case E_BUZZER_MODE:
        if (mProtocol->GetBuzzerMode() == E_SOUND) {
          mCrawler.sing(mProtocol->GetBuzzerSound());
        }
        break;
    }
    mCrawler.LightOff();
  }
}

void HandleInfaredRemote (byte irKeyCode)
{
  switch ((E_IR_KEYCODE)mCrawler.IR->getIrKey(irKeyCode)) {
    case IR_KEYCODE_STAR:
      mCrawler.sing(S_connection);
      mCrawler.SetRgbColor(E_RGB_ALL, mCrawler.GetSpeed() * 2.5);
      mCrawler.SpeedUp(10);
      DEBUG_LOG(DEBUG_LEVEL_INFO, "mCrawler.Speed = %d \n", mCrawler.Speed);
      break;
    case IR_KEYCODE_POUND:
      DEBUG_LOG(DEBUG_LEVEL_INFO, " start Degree = %d \n", mCrawler.Degree);
      mCrawler.sing(S_disconnection);
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
    mCrawler.SendUltrasonicData();
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
  uint16_t UlFrontDistance =  mCrawler.GetUltrasonicValue(FRONT);
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
  static byte mode;
  static bool recv_flag;
  mProtocol->RecevData();
  if (recv_flag = mProtocol->ParserPackage()) {
    if (mProtocol->GetRobotControlFun() == E_CONTROL_MODE) {
      mCrawler.SetControlMode(mProtocol->GetControlMode());
      return;
    }
  }
  switch (mCrawler.GetControlMode()) {
    case E_BLUETOOTH_CONTROL:
      HandleBluetoothRemote(recv_flag);
      DEBUG_LOG(DEBUG_LEVEL_INFO, "E_BLUETOOTH_CONTROL \n");
      break;
    case E_INFRARED_REMOTE_CONTROL:
      byte irKeyCode;
      if (mode != E_INFRARED_REMOTE_CONTROL) {
        mCrawler.InitIr();
        mode = E_INFRARED_REMOTE_CONTROL;
      }
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
      break;
    case E_ULTRASONIC_AVOIDANCE:
      if (mode != E_ULTRASONIC_AVOIDANCE) {
        mCrawler.InitUltrasonic();
        mCrawler.SetServoDegree(1, 90);
        mode = E_ULTRASONIC_AVOIDANCE;
      }
      HandleUltrasonicAvoidance();
      break;
    case E_ULTRASONIC_FOLLOW_MODE:
      if (mode != E_ULTRASONIC_FOLLOW_MODE) {
        mCrawler.InitUltrasonic();
        mCrawler.SetServoDegree(1, 90);
        mode = E_ULTRASONIC_FOLLOW_MODE;
      }
      UltrasonicFollow();
      break;
    case E_PIANO_MODE:
      if (recv_flag) {
        if (mProtocol->GetRobotControlFun() == E_BUZZER_MODE) {
          switch (mProtocol->GetBuzzerMode())
          {
            case E_NOTE:
              mCrawler.Sensors->mBuzzer->_tone(mProtocol->GetBuzzerNote(), 120, 2);
              break;
            case E_SOUND:
              mCrawler.sing(mProtocol->GetBuzzerSound());
              break;
            case E_MUSIC:
              mCrawler.PianoSing(mProtocol->GetBuzzerMusic());
              break;
          }
        }
      }
      break;
    case E_RGB_MODE:
      if (recv_flag) {
        if (mProtocol->GetRobotControlFun() == E_LED) {
          if (mProtocol->GetRgbMode() == 1) {
            mCrawler.SetRgbColor(E_RGB_ALL, mProtocol->GetRgbValue());
          } else {
            mCrawler.SetRgbEffect(E_RGB_ALL, mProtocol->GetRgbValue(), mProtocol->GetRgbEffect());
          }
        }
      }
      return;
    default:
      break;
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
