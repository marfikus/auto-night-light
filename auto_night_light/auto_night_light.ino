
#include <Ultrasonic.h>


const int MAIN_CYCLE_DELAY = 100;
const int LIGHT_OFF_TIMER_VALUE = 3000;
const int FADEOUT_STEP = 5;
const int MAX_ALARM_COUNT = 3;

const int KITCHEN_THRESHOLD_DISTANCE = 20;
const int ROOM_THRESHOLD_DISTANCE = 150;
const int HALL_THRESHOLD_DISTANCE = 250;

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

usSensor sensors[1];

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

	for (int i = 0; i < sizeof(sensors); i++) {
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


void setup() {
  Serial.begin(9600);

	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);
	pinMode(led3, OUTPUT);

	brightness = 0;
	setCurrentBrightness();
	lightingState = OFF;

	// кухня
	// sensors[0].ultrasonic(2, 3);
	// sensors[0] = (usSensor) {
	// 	NORMAL, 0, 0, KITCHEN_THRESHOLD_DISTANCE, 0
	// };
	sensors[0].state = NORMAL;
	sensors[0].timer = 0;
	sensors[0].currentDistance = 0;
	sensors[0].thresholdDistance = KITCHEN_THRESHOLD_DISTANCE;
	sensors[0].alarmCounter = 0;

	// // комната
	// // sensors[1].ultrasonic(4, 5);
	// sensors[1].state = NORMAL;
	// sensors[1].timer = 0;
	// sensors[1].currentDistance = 0;
	// sensors[1].thresholdDistance = ROOM_THRESHOLD_DISTANCE;
	// sensors[1].alarmCounter = 0;

	// // коридор
	// // sensors[2].ultrasonic(6, 7);
	// sensors[2].state = NORMAL;
	// sensors[2].timer = 0;
	// sensors[2].currentDistance = 0;
	// sensors[2].thresholdDistance = HALL_THRESHOLD_DISTANCE;
	// sensors[2].alarmCounter = 0;
}


void loop() {

	sensors[0].currentDistance = us1.distanceRead();
	// sensors[1].currentDistance = us2.distanceRead();
	// sensors[2].currentDistance = us3.distanceRead();

	// Serial.print("============");
	// Serial.println(i);
	Serial.println(sensors[0].currentDistance);		
	// Serial.println("============");

	for (int i = 0; i < sizeof(sensors); i++) {
		// sensors[i].currentDistance = sensors[i].ultrasonic.distanceRead();

		if (sensors[i].currentDistance < sensors[i].thresholdDistance) {

			if (sensors[i].alarmCounter < MAX_ALARM_COUNT - 1) {
				sensors[i].alarmCounter = sensors[i].alarmCounter + 1;
			} else {

				lightOn();
				sensors[i].state = ALARM;
				sensors[i].timer = LIGHT_OFF_TIMER_VALUE;
				sensors[i].alarmCounter = 0;

				// switch (sensors[i].state) {
				// 	case NORMAL:
				// 		lightOn();
				// 		sensors[i].state = ALARM;
				// 		sensors[i].timer = LIGHT_OFF_TIMER_VALUE;
				// 		break;

				// 	case ALARM:
				// 		lightOn();
				// 		sensors[i].timer = LIGHT_OFF_TIMER_VALUE;
				// 		break;
				// }
			
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



  // // Serial.print("Distance in CM: ");
  // roomSensorDistance = ultrasonic.distanceRead();
  // Serial.println(roomSensorDistance);

  // if (roomSensorDistance < roomDistance) {

  // 	if (counter < 2) {
  // 		counter = counter + 1;
  // 	} else {

	 //  	if (!lightIsOn) {
		//     digitalWrite(led1, HIGH);
		//     lightIsOn = true;
	 //  	}

	 //    lightOffTimer = LIGHT_OFF_TIMER_VALUE;
	 //    counter = 0;

  // 	}


  // } else {

  // 	if (lightIsOn) {

	 //  	if (lightOffTimer > 0) {
	 //  		lightOffTimer = lightOffTimer - MAIN_CYCLE_DELAY;
	 //  	} else {
	 //    	digitalWrite(led1, LOW);
	 //    	lightIsOn = false;
	 //  	}
  // 	}
  // 	counter = 0;

  // }
  
	delay(MAIN_CYCLE_DELAY);
}
