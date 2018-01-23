#include <Arduino.h>
#include <Ultrasonic.h>


Ultrasonic ultrasonic1(14,12);

void setup() {
    Serial.begin(9600);

}

void loop() {
    Serial.print("Sensor 01: ");
    Serial.print(ultrasonic1.distanceRead()); // Prints the distance on the default unit (centimeters)
    Serial.println("cm");
    delay(1000);
}
