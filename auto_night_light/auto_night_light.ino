
#include <Ultrasonic.h>

Ultrasonic ultrasonic(8, 9);

int led1 = 3;

int roomSensorDistance;
int roomDistance = 200;

bool lightIsOn = false;

int MAIN_CYCLE_DELAY = 100;
// int TIMER_MULTIPLIER = 4;
int LIGHT_OFF_TIMER_VALUE = 3000;
int lightOffTimer = 0;
int counter = 0;


void setup() {
  Serial.begin(9600);
  
  pinMode(led1, OUTPUT);
  digitalWrite(led1, LOW);
}


void loop() {
  Serial.print("Distance in CM: ");
  roomSensorDistance = ultrasonic.distanceRead();
  Serial.println(roomSensorDistance);

  if (roomSensorDistance < roomDistance) {

  	if (counter < 2) {
  		counter = counter + 1;
  	} else {

	  	if (!lightIsOn) {
		    digitalWrite(led1, HIGH);
		    lightIsOn = true;
	  	}

	    lightOffTimer = LIGHT_OFF_TIMER_VALUE;
	    counter = 0;

  	}


  } else {

  	if (lightIsOn) {

	  	if (lightOffTimer > 0) {
	  		lightOffTimer = lightOffTimer - MAIN_CYCLE_DELAY;
	  	} else {
	    	digitalWrite(led1, LOW);
	    	lightIsOn = false;
	  	}
  	}
  	counter = 0;

  }
  
  delay(MAIN_CYCLE_DELAY);
}
