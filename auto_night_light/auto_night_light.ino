
#include <Ultrasonic.h>


const int MAIN_CYCLE_DELAY = 100;
const int LIGHT_OFF_TIMER_VALUE = 3000;
const int FADEOUT_STEP = 5;
const int MAX_ALARM_COUNT = 3;
const int SENSORS_COUNT = 3;

const int KITCHEN_THRESHOLD_DISTANCE = 20;
const int ROOM_THRESHOLD_DISTANCE = 15;
const int HALL_THRESHOLD_DISTANCE = 25;

enum sensorStates {
	NORMAL,
	ALARM,
};

struct usSensor {
	// Ultrasonic ultrasonic;

	sensorStates state;
	int timer;
	int currentDistance;
	int thresholdDistance;
	int alarmCounter;
};

usSensor sensors[SENSORS_COUNT];

Ultrasonic us1(2, 3);
Ultrasonic us2(4, 5);
Ultrasonic us3(6, 7);

int led1 = 9;
int led2 = 10;
int led3 = 11;

enum {
	ON,
	FADEOUT,
	OFF,
} lightingState;

int brightness = 0;

void setCurrentBrightness() {

	analogWrite(led1, brightness);
	analogWrite(led2, brightness);
	analogWrite(led3, brightness);	
}

void lightOn() {

	brightness = 255;
	setCurrentBrightness();
	lightingState = ON;
}

void lightOff() {

	for (int i = 0; i < SENSORS_COUNT; i++) {
		if (sensors[i].state == ALARM) {
			return;
		}
	}

	switch (lightingState) {
		case ON:
			lightingState = FADEOUT;
			brightness = brightness - FADEOUT_STEP;
			setCurrentBrightness();
			break;

		case FADEOUT:
			brightness = brightness - FADEOUT_STEP;

			if (brightness <= 0) {
				brightness = 0;
				lightingState = OFF;
			}

			setCurrentBrightness();
			break;
	}
}

void greetingBlink() {

	for (int i = 0; i < 2; i++) {
		digitalWrite(led1, HIGH);
		delay(200);
		digitalWrite(led2, HIGH);
		digitalWrite(led1, LOW);
		delay(200);
		digitalWrite(led3, HIGH);
		digitalWrite(led2, LOW);
		delay(200);
		digitalWrite(led3, LOW);
	}

	delay(200);
	digitalWrite(led1, HIGH);
	digitalWrite(led2, HIGH);
	digitalWrite(led3, HIGH);
	delay(700);
	digitalWrite(led1, LOW);
	digitalWrite(led2, LOW);
	digitalWrite(led3, LOW);
}


void setup() {
	// Serial.begin(9600);

	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);
	pinMode(led3, OUTPUT);

	brightness = 0;
	setCurrentBrightness();
	lightingState = OFF;

	// кухня
	// sensors[0].ultrasonic(2, 3);
	// можно так инициализировать сенсоры
	// sensors[0] = (usSensor) {
	// 	NORMAL, 0, 0, KITCHEN_THRESHOLD_DISTANCE, 0
	// };
	// но так, кмк, более понятно сразу, нагляднее
	sensors[0].state = NORMAL;
	sensors[0].timer = 0;
	sensors[0].currentDistance = 0;
	sensors[0].thresholdDistance = KITCHEN_THRESHOLD_DISTANCE;
	sensors[0].alarmCounter = 0;

	// комната
	// sensors[1].ultrasonic(4, 5);
	sensors[1].state = NORMAL;
	sensors[1].timer = 0;
	sensors[1].currentDistance = 0;
	sensors[1].thresholdDistance = ROOM_THRESHOLD_DISTANCE;
	sensors[1].alarmCounter = 0;

	// коридор
	// sensors[2].ultrasonic(6, 7);
	sensors[2].state = NORMAL;
	sensors[2].timer = 0;
	sensors[2].currentDistance = 0;
	sensors[2].thresholdDistance = HALL_THRESHOLD_DISTANCE;
	sensors[2].alarmCounter = 0;

	greetingBlink();
}


void loop() {

	sensors[0].currentDistance = us1.distanceRead();
	sensors[1].currentDistance = us2.distanceRead();
	sensors[2].currentDistance = us3.distanceRead();

	// Serial.println(sensors[0].currentDistance);
	// Serial.println(sensors[1].currentDistance);
	// Serial.println(sensors[2].currentDistance);

	for (int i = 0; i < SENSORS_COUNT; i++) {
		// sensors[i].currentDistance = sensors[i].ultrasonic.distanceRead();

		// Serial.print("Sensor ");
		// Serial.print(i);
		// Serial.print(" = ");
		// Serial.println(sensors[i].currentDistance);

		if (sensors[i].currentDistance < sensors[i].thresholdDistance) {

			// если 0, то вероятно датчик в обрыве
			// пропускаем, иначе будет вечная тревога
			if (sensors[i].currentDistance == 0) {
				continue;
			}

			if (sensors[i].alarmCounter < MAX_ALARM_COUNT - 1) {
				sensors[i].alarmCounter = sensors[i].alarmCounter + 1;
			} else {

				lightOn();
				sensors[i].state = ALARM;
				sensors[i].timer = LIGHT_OFF_TIMER_VALUE;
				sensors[i].alarmCounter = 0;
			}
		} else {

			if (sensors[i].timer > 0) {
				sensors[i].timer = sensors[i].timer - MAIN_CYCLE_DELAY;
			} else {
				sensors[i].state = NORMAL;
			}

			sensors[i].alarmCounter = 0;
		}
	}

	lightOff();
	delay(MAIN_CYCLE_DELAY);
}
