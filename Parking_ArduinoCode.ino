#include<SPI.h>
#include<RF24.h>

// defines pins numbers
const int trigPin = 4;
const int echoPin = 3;
const int led1 = 9;
const int led2 = 10;

// defines variables
long duration;
int distance;

//slot name and status
String slot= "N1";
String availability;
String last;

// ce, csn pins
RF24 radio(7,8);
 
void setup(void){
        Serial.begin(9600);
	radio.begin();
	radio.setPALevel(RF24_PA_MAX);
        radio.setDataRate(RF24_250KBPS);
	radio.setChannel(0x76);
	radio.openWritingPipe(0xF0F0F0F0E1LL);
	radio.enableDynamicPayloads();
	radio.powerUp();

        pinMode(led1, OUTPUT);
        pinMode(led2, OUTPUT);
        pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
        pinMode(echoPin, INPUT); // Sets the echoPin as an Input
 
}
 
void loop(void){
  // Clears the trigPin
digitalWrite(trigPin, LOW);
digitalWrite(led1, LOW);
digitalWrite(led2, LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
//delayMicroseconds(10);
//digitalWrite(trigPin, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
Serial.print("Status: ");
//Serial.println(distance);
if(distance>=10){
  digitalWrite(led1, HIGH);
  Serial.println("Available");
  availability="Y";
}
else if(distance>=0 && distance<10){
  digitalWrite(led2, HIGH);
  Serial.println("Occupied");
  availability="N";
} 
  
      
        String pl = slot+"||"+availability;
        char payload[6];
        pl.toCharArray(payload, 6);
	      radio.write(&payload, sizeof(payload));
        Serial.print("Payload :");
        Serial.println(payload);
        delay(1000);

        	
 
}
