#include <SoftwareSerial.h>
SoftwareSerial sim800l(10, 11);
#include <SoftwareSerial.h>
#include <TinyGPS.h>
SoftwareSerial mySerial(4, 3);
TinyGPS gps;


#define echoPin 1 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 2 //attach pin D3 Arduino to pin Trig of HC-SR04
#define echoPin1 8 // attach pin D8 Arduino to pin Echo of HC-SR04
#define trigPin1 9 //attach pin D9 Arduino to pin Trig of HC-SR04


// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

long duration1; // variable for the duration of sound wave travel
int distance1; // variable for the distance measurement







void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);
void setup()
{


  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin1, INPUT); // Sets the echoPin as an INPUT
  
  sim800l.begin(9600);
  mySerial.begin(9600);
  Serial.begin(9600);   
  delay(1000);
}
 
void loop()
{


   if (distance >= 5 || distance <= 26)
  {
    // Clears the trigPin condition
    digitalWrite(trigPin, LOW);
    delayMicroseconds(20);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(20);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Displays the distance on the Serial Monitor
    Serial.print("DustBin 1 : ");
    Serial.print(distance);
    Serial.println(" cm");

    
     bool newdata = false;
    unsigned long start = millis();
    // Every 5 seconds we print an update
    while (millis() - start < 5000) 
    {
      if (mySerial.available()) 
    
      {
        char c = mySerial.read();
        //Serial.print(c);  // uncomment to see raw GPS data
        if (gps.encode(c)) 
        {
          newdata = true;
          break;  // uncomment to print new data immediately!
        }
     }
    }
  
    if (newdata) 
    {
      gpsdump(gps);
      Serial.println();
    }

  }
  else{

       if (newdata) 
       {
           gpsdump(gps);
           Serial.println();
        }

    
  }
  
}
void gpsdump(TinyGPS &gps)
{
  long lat, lon;
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
    Serial.println("Sending SMS...");
  sim800l.print("AT+CMGF=1\r");
  delay(100);
  sim800l.print("AT+CMGS=\"YOUR PHONE NUMBER\"\r");//EX +919876543210
  delay(500);
  sim800l.print("http://maps.google.com/maps?q=loc:");
  sim800l.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
  sim800l.print(","); 
  sim800l.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  sim800l.print((char)26);
  sim800l.print("\nDustbin Full");
  delay(500);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(5000);
}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.00) 
  {
     Serial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.9990, 2) prints as "2.000"
  double rounding = 0.50;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.00;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 00)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) 
  {
    remainder *= 10.00;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint;
  }
}
