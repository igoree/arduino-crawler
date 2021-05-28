#include "Coroutine.h"
#include "MPU6050_6Axis_MotionApps_V6_12.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "DebugOutput.h"

Coroutine _freeMemoryCoroutine("freeMemory");

int _lastFreeMemory(0);

MPU6050 mpu;
float angleX = 0;
float angleY = 0;
float angleZ = 0;
const float toDeg = 180.0 / M_PI;
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
// инициализация
void initDMP() {
	devStatus = mpu.dmpInitialize();
	mpu.setDMPEnabled(true);
	mpuIntStatus = mpu.getIntStatus();
	packetSize = mpu.dmpGetFIFOPacketSize();
}
// получение углов в angleX, angleY, angleZ
void getAngles() {
	if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
		mpu.dmpGetQuaternion(&q, fifoBuffer);
		mpu.dmpGetGravity(&gravity, &q);
		mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
		angleX = ypr[2] * toDeg;
		angleY = ypr[1] * toDeg;
		angleZ = ypr[0] * toDeg;
	}
}

void setup() 
{
	INIT_DEBUG();

	Wire.begin();
	Wire.setClock(400000);
	Serial.begin(9600);
	mpu.initialize();
	initDMP();
	DEBUG_INFO("init ok");

#if DEBUG_LEVEL <= DEBUG_LEVEL_INFO
	_freeMemoryCoroutine.start(CoroutineTask(&monitorFreeMemoryAsync));
#endif
}

CoroutineTaskResult* monitorFreeMemoryAsync(const CoroutineTaskContext* context)
{
	auto freeMemory = debug_freeMemory();
	if (freeMemory != _lastFreeMemory)
	{
		DEBUG_INFO("free memory: %d", freeMemory);

		_lastFreeMemory = freeMemory;
	}

	return context->delayThenRepeat(500);
}


void loop() 
{
	_freeMemoryCoroutine.continueExecution();

	getAngles();
	Serial.print(angleX); Serial.print(',');
	Serial.print(angleY); Serial.print(',');
	Serial.println(angleZ);
	delay(20);
}

