#include "PinChangeInt.h"
#include "Arduino.h"
#include <Wire.h>

#include "Emakefun_MotorDriver.h"
#include "Emakefun_MS_PWMServoDriver.h"
#if defined(ARDUINO_SAM_DUE)
#define WIRE Wire1
#else
#define WIRE Wire
#endif

Emakefun_MotorDriver::Emakefun_MotorDriver(uint8_t addr, uint8_t version) {
	_version = version;
	_addr = addr;
	_pwm = Emakefun_MS_PWMServoDriver(_addr);
}

void Emakefun_MotorDriver::begin(uint16_t freq) {
	// init PWM w/_freq
	WIRE.begin();
	_pwm.begin();
	_freq = freq;
	_pwm.setPWMFreq(_freq);  // This is the maximum PWM frequency
	for (uint8_t i = 0; i < 16; i++)
		_pwm.setPWM(i, 0, 0);
}

void Emakefun_MotorDriver::setPWM(uint8_t pin, uint16_t value) {
	if (value > 4095) {
		_pwm.setPWM(pin, 4096, 0);
	}
	else
		_pwm.setPWM(pin, 0, value);
}
void Emakefun_MotorDriver::setPin(uint8_t pin, boolean value) {
	if (value == LOW)
		_pwm.setPWM(pin, 0, 0);
	else
		_pwm.setPWM(pin, 4096, 0);
}

Emakefun_DCMotor* Emakefun_MotorDriver::getMotor(uint8_t num) {
	if (num > 4) return NULL;
	num--;
	if (dcmotors[num].motornum == 0) {
		// not init'd yet!
		dcmotors[num].motornum = num;
		dcmotors[num].MC = this;
		uint8_t pwm, in1, in2;
		if (num == 0) {
			if (_version == MOTOR_DRIVER_BOARD_V3) {
				pwm = 8; in2 = 9; in1 = 10;
			}
			else if (_version == MOTOR_DRIVER_BOARD_V4) {
				pwm = 8; in1 = 10;
			}
			else if (_version == MOTOR_DRIVER_BOARD_V5) {
				in1 = 8; in2 = 10;
			}
		}
		else if (num == 1) {
			if (_version == MOTOR_DRIVER_BOARD_V3) {
				pwm = 13; in2 = 12; in1 = 11;
			}
			else if (_version == MOTOR_DRIVER_BOARD_V4) {
				pwm = 13; in1 = 11;
			}
			else if (_version == MOTOR_DRIVER_BOARD_V5) {
				in1 = 11; in2 = 13;
			}
		}
		else if (num == 2) {
			if (_version == MOTOR_DRIVER_BOARD_V3) {
				pwm = 2; in2 = 3; in1 = 4;
			}
			else if (_version == MOTOR_DRIVER_BOARD_V4) {
				pwm = 2; in1 = 4;
			}
			else if (_version == MOTOR_DRIVER_BOARD_V5) {
				in1 = 4; in2 = 2;
			}
		}
		else if (num == 3) {
			if (_version == MOTOR_DRIVER_BOARD_V3) {
				pwm = 7; in2 = 6; in1 = 5;
			}
			else if (_version == MOTOR_DRIVER_BOARD_V4) {
				pwm = 7; in1 = 5;
			}
			else if (_version == MOTOR_DRIVER_BOARD_V5) {
				in1 = 5; in2 = 7;
			}
		}
		dcmotors[num].PWMpin = pwm;
		dcmotors[num].IN1pin = in1;
		dcmotors[num].IN2pin = in2;
	}

	return &dcmotors[num];
}

Emakefun_Servo* Emakefun_MotorDriver::getServo(uint8_t num) {
	if (num > 8) return NULL;

	num--;

	if (servos[num].servonum == 0) {
		// not init'd yet!
		servos[num].servonum = num;
		servos[num].MC = this;
		uint8_t pwm;
		if (num == 0) {
			pwm = 0;
		}
		else if (num == 1) {
			pwm = 1;
		}
		else if (num == 2) {
			if (_version == MOTOR_DRIVER_BOARD_V4) {
				pwm = 3;
			}
			else {
				pwm = 14;
			}
		}
		else if (num == 3) {
			if (_version == MOTOR_DRIVER_BOARD_V4) {
				pwm = 6;
			}
			else {
				pwm = 15;
			}
		}
		if (_version == MOTOR_DRIVER_BOARD_V4 || _version == MOTOR_DRIVER_BOARD_V5) {
			if (num == 4) {
				pwm = 9;
			}
			else if (num == 5) {
				pwm = 12;
			}
			else if (num == 6) {
				if (_version == MOTOR_DRIVER_BOARD_V4) {
					pwm = 14;
				}
				else if (_version == MOTOR_DRIVER_BOARD_V5) {
					pwm = 3;
				}
			}
			else if (num == 7) {
				if (_version == MOTOR_DRIVER_BOARD_V4) {
					pwm = 15;
				}
				else if (_version == MOTOR_DRIVER_BOARD_V5) {
					pwm = 6;
				}
			}
			servos[num].PWMpin = pwm;
		}
	}
	return &servos[num];
}

/******************************************
			   SERVOS
******************************************/

Emakefun_Servo::Emakefun_Servo(void) {
	MC = NULL;
	servonum = 0;
	PWMpin = 0;
	currentAngle = 0;
}

constexpr auto SERVO_PULSE_LENGTH = 1000000.0 / 50.0 / 4096.0; // 1,000,000 us per second / 50 Hz / 12 bits of resolution

void Emakefun_Servo::setServoPulse(double pulse) {
	MC->setPWM(PWMpin, pulse * 1000 / SERVO_PULSE_LENGTH);
}

void Emakefun_Servo::writeServo(uint8_t angle) {
	setServoPulse(0.5 + angle / 90.0);
	currentAngle = angle;
}

uint8_t Emakefun_Servo::readDegrees() {
	return currentAngle;
}

/******************************************
			   MOTORS
******************************************/

Emakefun_DCMotor::Emakefun_DCMotor(void) {
	MC = NULL;
	motornum = 0;
	PWMpin = IN1pin = IN2pin = 0;
}

void Emakefun_DCMotor::run(uint8_t cmd) {
	switch (cmd) {
	case FORWARD:
		if (MC->_version != MOTOR_DRIVER_BOARD_V4) MC->setPin(IN2pin, LOW);
		if (MC->_version == MOTOR_DRIVER_BOARD_V5) {
			MC->setPWM(IN1pin, DcSpeed);
		}
		else {
			MC->setPin(IN1pin, HIGH);
		}
		break;
	case BACKWARD:
		MC->setPin(IN1pin, LOW);
		if (MC->_version == MOTOR_DRIVER_BOARD_V5) {
			MC->setPWM(IN2pin, DcSpeed);
		}
		else if (MC->_version == MOTOR_DRIVER_BOARD_V3) {
			MC->setPin(IN2pin, HIGH);
		}
		break;
	case BRAKE:
		if (MC->_version != MOTOR_DRIVER_BOARD_V5) {
			MC->setPWM(PWMpin, 0);
		}
		if (MC->_version != MOTOR_DRIVER_BOARD_V4) MC->setPin(IN2pin, HIGH);
		MC->setPin(IN1pin, HIGH);
		break;
	case RELEASE:
		if (MC->_version == 4) {
			MC->setPin(IN1pin, LOW);
		}
		else {
			MC->setPin(IN1pin, LOW);
			MC->setPin(IN2pin, LOW);
		}
		break;
	}
}

void Emakefun_DCMotor::setSpeed(uint8_t speed) {
	if (MC->_version == 5) {
		DcSpeed = (speed * 16);
	}
	else {
		MC->setPWM(PWMpin, speed * 16);
	}
}

/******************************************
			   SENSOR
******************************************/

Emakefun_Sensor::Emakefun_Sensor(void) {
	MC = NULL;
	mIrRecv = NULL;
	mBuzzer = NULL;
	mRgb = NULL;
	IrPin = BuzzerPin = RgbPin = EchoPin = TrigPin = 0;
}

Emakefun_Sensor::~Emakefun_Sensor()
{
	delete mIrRecv;
	delete mBuzzer;
	delete mRgb;
}

uint16_t Emakefun_Sensor::GetUltrasonicDistance(void)
{
	uint16_t FrontDistance;
	digitalWrite(TrigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(TrigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(TrigPin, LOW);
	FrontDistance = pulseIn(EchoPin, HIGH) / 58.00;
	return FrontDistance;
}

void* Emakefun_MotorDriver::getSensor(E_SENSOR_INDEX n)
{
	//Serial.print("E_SENSOR_INDEX is ");
	//Serial.println(n);
	if (n == E_RGB) {
		if (sensors.mRgb == NULL) {
			sensors.RgbPin = RGB_PIN;
			pinMode(RGB_PIN, OUTPUT);
			sensors.mRgb = new RGBLed(RGB_PIN, 2);
		}
		return sensors.mRgb;
	}
	if (n == E_IR) {
		if (sensors.mIrRecv == NULL) {
			sensors.IrPin = IR_PIN;
			pinMode(IR_PIN, INPUT);
			sensors.mIrRecv = new IRRemote(IR_PIN);
		}
		return sensors.mIrRecv;
	}
	if (n == E_BUZZER) {
		if (sensors.mBuzzer == NULL) {
			sensors.BuzzerPin = BUZZER_PIN;
			if (_version == 3) sensors.BuzzerPin = 9;
			sensors.mBuzzer = new Buzzer(sensors.BuzzerPin);
		}
		return sensors.mBuzzer;
	}
	if (n == E_ULTRASONIC) {
		// Serial.println("E_ULTRASONIC");
		sensors.EchoPin = ECHO_PIN;
		sensors.TrigPin = TRIG_PIN;
		pinMode(ECHO_PIN, INPUT);
		pinMode(TRIG_PIN, OUTPUT);
		return NULL;
	}
	sensors.MC = this;
	return &sensors;
}