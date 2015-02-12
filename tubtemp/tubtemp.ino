#include <Average.h>

/**
 * Hardware Settings
 */
#define TMP36 A0
#define TMP36_VOLTAGE  5.0 /* define as float */
#define RELAY 8
#define HEAT_ON  digitalWrite(RELAY, HIGH); digitalWrite(13,HIGH);
#define HEAT_OFF digitalWrite(RELAY, LOW); digitalWrite(13,LOW);

#define TNORM 103
#define THYST 1
#define TLOW  5

/**
 * Globals
 */
static Average<float> tempBuffer(10);

/**
 * Setup
 */
void setup() {
  Serial.begin(9600);  
  pinMode(RELAY, OUTPUT);   
  pinMode(13, OUTPUT); 
}

/**
 * Read the temperature.
 * Uses averaging over 5 samples to stabilize readings from the
 * TMP36 and returns a value in degrees F rounded to the nearest
 * degree.
 */
float temperature() {  
  float voltage = analogRead(TMP36) * TMP36_VOLTAGE / 1023.0;
  float c = (voltage - 0.5) * 100;
  float f = (c * 9.0 / 5.0) + 32.0;
  tempBuffer.push(f);
  return round(tempBuffer.mean());
}

/**
 * Loop
 */
void loop() {  
  float tempOn = TNORM - THYST;
  float tempOff = TNORM + THYST;
  float t = temperature();
 
  if (t <= tempOn) {
    HEAT_ON;
  } else if (t >= tempOff) {
    HEAT_OFF;
  }
    
  Serial.print(temperature());
  Serial.print(" ");
  Serial.print(digitalRead(RELAY));
  Serial.print("\n");
   
  delay(1000);  
}
