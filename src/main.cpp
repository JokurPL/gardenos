#include <Arduino.h>
#include <SoftwareSerial.h>

// CONSTANTS
#define LED 10
#define firstAnalogPin 54

int moistureSensorsAmonut = 1;
float averageMoisture;

char incoming_value;

SoftwareSerial hc06(2, 3); // 2 - Rx, 3 - Tx | Arduino Rx -> HC Tx # Arduino Tx -> Hc Rx by divider 

// ################################ FUNCTIONS ################################

void initAnalogs(int moistureSensorsAmonut)
{
  if (moistureSensorsAmonut > 0)
  {
    int endIteration = firstAnalogPin + moistureSensorsAmonut;

    for (int i = firstAnalogPin; i < endIteration; i++)
    {
      pinMode(i, INPUT);
    }
  }
}

float toAverage(int moistureSensorsAmonut)
{
  float avg = 0;

  if (moistureSensorsAmonut > 0)
  {
    float sum;

    int endIteration = firstAnalogPin + moistureSensorsAmonut;

    for (int i = firstAnalogPin; i < endIteration; i++)
    {
      sum += analogRead(uint8_t(i));
    }

    avg = sum / moistureSensorsAmonut;
  }
  return avg;
}

void setup() {
  Serial.begin(9600);
  hc06.begin(9600);
  
  Serial.println("ENTER AT Commands:"); 
  
  initAnalogs(moistureSensorsAmonut);

  pinMode(LED, OUTPUT); // simple LED
}

void loop() {
  //Write data from HC06 to Serial Monitor
  if (hc06.available()) {

    incoming_value = hc06.read();
    Serial.write(incoming_value);
    
    switch (incoming_value)
    {
      // simple turn on/off LED
    case 'a':
      if (digitalRead(LED) == 1) 
      {
        digitalWrite(LED, LOW);
      }
      else 
      {
        digitalWrite(LED, HIGH);
      }

      break;
    
    default:
      break;
    }

    // if (incoming_value == 'a' && digitalRead(10) == LOW)
    //   digitalWrite(10, HIGH);
    // else if (incoming_value == 'a' && digitalRead(10) == HIGH)
    //   digitalWrite(10, LOW);
  }

  //Write from Serial Monitor to HC06
  if (Serial.available()) {
    Serial.read();
    hc06.write(incoming_value);
  }
}