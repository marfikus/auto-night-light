
#include <Ultrasonic.h>

Ultrasonic ultrasonic(8, 9);

int led1 = 0;

int roomSensorDistance;
int roomDistance = 100;


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
  } else {
    digitalWrite(led1, LOW);
  }
  
  delay(250);
}
