#include "debug.h"
#include "Crawler.h"
#include "ProtocolParser.h"
#include "IRKeyMap.h"

ProtocolParser* mProtocol = new ProtocolParser();
Crawler mCrawler(mProtocol);
byte count = 0;
bool secondSoundGroup = false;

void setup()
{
	INIT_DEBUG();
	mCrawler.Init(M2, M1);
	mCrawler.InitServo();
	mCrawler.InitRgb();
	mCrawler.InitBuzzer();
	mCrawler.InitIr();
	mCrawler.SetSpeed(50);
	mCrawler.SetServoBaseDegree(90);
	mCrawler.SetServoDegree(ULTRASONIC_SERVO, 90);
	mCrawler.InitUltrasonic();
	DEBUG_INFO("init ok");
	mCrawler.Sing(S_connection);
}

void SingSound(uint8_t soundIndex) {
	if (secondSoundGroup) {
		soundIndex = soundIndex + 9;
	}
	mCrawler.Sing(soundIndex);
}

void HandleInfaredRemote(IRKeyCode irKeyCode)
{
	switch (irKeyCode) {
	case IRKeyCode::Star:
		mCrawler.Sing(S_connection);
		mCrawler.SetRgbColor(E_RGB_ALL, mCrawler.GetSpeed() * 2.5);
		mCrawler.SpeedUp(10);
		DEBUG_INFO("Speed = %d \n", mCrawler.GetSpeed());
		break;
	case IRKeyCode::Pound:
		mCrawler.Sing(S_disconnection);
		mCrawler.SetRgbColor(E_RGB_ALL, mCrawler.GetSpeed() * 2.5);
		mCrawler.SpeedDown(10);
		break;
	case IRKeyCode::Up:
		mCrawler.GoForward();
		break;
	case IRKeyCode::Down:
		mCrawler.GoBack();
		break;
	case IRKeyCode::Ok:
		mCrawler.Stop();
		break;
	case IRKeyCode::Left:
		mCrawler.TurnLeft();
		break;
	case IRKeyCode::Right:
		mCrawler.TurnRight();
		break;

	case IRKeyCode::Button1:
		SingSound(2);
		break;

	case IRKeyCode::Button2:
		SingSound(3);
		break;

	case IRKeyCode::Button3:
		SingSound(4);
		break;

	case IRKeyCode::Button4:
		SingSound(5);
		break;

	case IRKeyCode::Button5:
		SingSound(6);
		break;

	case IRKeyCode::Button6:
		SingSound(7);
		break;

	case IRKeyCode::Button7:
		SingSound(8);
		break;

	case IRKeyCode::Button8:
		SingSound(9);
		break;

	case IRKeyCode::Button9:
		SingSound(10);
		break;

	case IRKeyCode::Button0:
		secondSoundGroup = !secondSoundGroup;
		break;

	default:
		break;
	}
}

void HandleUltrasonicAvoidance(void)
{
	uint16_t UlFrontDistance, UlLeftDistance, UlRightDistance;
	UlFrontDistance = mCrawler.GetUltrasonicValue(E_SERVO_FRONT);
	if (count++ > 50) {
		//mCrawler.SendUltrasonicData();
		count = 0;
	}
	DEBUG_INFO("UlFrontDistance = %d \n", UlFrontDistance);
	if (UlFrontDistance < UL_LIMIT_MIN)
	{
		mCrawler.SetSpeed(80);
		mCrawler.GoBack();
		delay(200);
	}
	if (UlFrontDistance < UL_LIMIT_MID)
	{
		mCrawler.Stop();
		delay(100);
		UlRightDistance = mCrawler.GetUltrasonicValue(E_SERVO_RIGHT);
		delay(50);
		UlLeftDistance = mCrawler.GetUltrasonicValue(E_SERVO_LEFT);
		if ((UlRightDistance > UL_LIMIT_MIN) && (UlRightDistance < UL_LIMIT_MAX)) {
			mCrawler.SetSpeed(100);
			mCrawler.TurnRight();
			delay(400);
		}
		else if ((UlLeftDistance > UL_LIMIT_MIN) && (UlLeftDistance < UL_LIMIT_MAX)) {
			mCrawler.SetSpeed(100);
			mCrawler.TurnLeft();
			delay(400);
		}
		else if ((UlRightDistance < UL_LIMIT_MIN) && (UlLeftDistance < UL_LIMIT_MIN)) {
			mCrawler.SetSpeed(400);
			mCrawler.TurnLeft();
			delay(800);
		}
	}
	else {
		mCrawler.SetSpeed(80);
		mCrawler.GoForward();
	}
}

void UltrasonicFollow()
{
	mCrawler.SetSpeed(40);
	uint16_t UlFrontDistance = mCrawler.GetUltrasonicValue(E_SERVO_FRONT);
	delay(10);
	if (UlFrontDistance < 10) {
		mCrawler.GoBack();
	}
	else if (UlFrontDistance > 14) {
		mCrawler.GoForward();
	}
	else if (10 <= UlFrontDistance <= 14) {
		mCrawler.Stop();
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

	IRKeyCode irKeyCode = (IRKeyCode)mCrawler.IR->getCode();
	if (irKeyCode != IRKeyCode::Unknown)
	{
		DEBUG_INFO("irKeyCode = %x", (uint8_t) irKeyCode);
		HandleInfaredRemote(irKeyCode);
		delay(110);
	}
	else
	{
		if (mCrawler.GetStatus() != E_STOP) 
		{
			mCrawler.Stop();
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