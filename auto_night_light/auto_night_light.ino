
#include <Ultrasonic.h>

Ultrasonic ultrasonic(8, 9);

int led1 = 0;

int roomSensorDistance;
int roomDistance = 100;

bool lightIsOn = false;

int MAIN_CYCLE_DELAY = 500;
int TIMER_MULTIPLIER = 2;

int LIGHT_OFF_TIMER_VALUE = 5000;
int lightOffTimer = 0;


void setup() {
  //Serial.begin(9600);
  
  pinMode(led1, OUTPUT);
  
}


void loop() {
 // Serial.print("Distance in CM: ");
 // Serial.println(ultrasonic.distanceRead());
  
  roomSensorDistance = ultrasonic.distanceRead();

  if (roomSensorDistance < roomDistance) {

    digitalWrite(led1, HIGH);
    lightIsOn = true;
    lightOffTimer = LIGHT_OFF_TIMER_VALUE;

  } else {

  	if (lightOffTimer > 0) {
  		lightOffTimer = lightOffTimer - (MAIN_CYCLE_DELAY * TIMER_MULTIPLIER);
  	} else {
    	digitalWrite(led1, LOW);
    	lightIsOn = false;
  	}

  }
  
  delay(MAIN_CYCLE_DELAY);
}
