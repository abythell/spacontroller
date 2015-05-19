/**
 * Hot Tub Temperature Controller with XBee output
 * Copyright 2015 Andrew Bythell <abythell@ieee.org>
 * http://angryelectron.com
 */
 
#include <SoftwareSerial.h>
#include <Average.h>
#include <XBee.h>

/**
 * Hardware Settings
 */
#define TMP36 A0
#define TMP36_VOLTAGE  5.0
#define RELAY 8
#define HEAT_ON  digitalWrite(RELAY, HIGH); digitalWrite(13,HIGH);
#define HEAT_OFF digitalWrite(RELAY, LOW); digitalWrite(13,LOW);

/**
 * User Settings
 */
#define TNORM 103
#define THYST 2
#define TDELTA  4
#define UPDATE_PERIOD  60

/**
 * Macros
 */
#define TEMP_ON (TNORM - THYST)
#define TEMP_OFF  (TNORM)

/**
 * Globals
 */
Average<int> tempData(UPDATE_PERIOD);
SoftwareSerial softSerial(10, 11);
XBee xbee = XBee();
XBeeAddress64 BROADCAST = XBeeAddress64(0x0, BROADCAST_ADDRESS);
long lastUpdateTime = 0;

/**
 * Setup
 */
void setup() {
  Serial.begin(9600);  
  softSerial.begin(9600);
  xbee.setSerial(softSerial);
  pinMode(RELAY, OUTPUT);   
  pinMode(13, OUTPUT); /* onboard LED indicates call for heat */
}

/**
 * Read the temperature.
 * The temperature reading is further adjusted by
 * TDELTA to compensate for the difference in temperature 
 * between the water in the tub and the water at the sensor.
 */
int temperature() {  
  float voltage = analogRead(TMP36) * TMP36_VOLTAGE / 1024.0;
  float c = (voltage - 0.5) * 100;
  float f = (c * 9.0 / 5.0) + 32.0;
  return round(f) + TDELTA;
}

/**
 * Transmit temperature and heater state over XBee network.
 */
void transmit(int t, byte state) {
  uint8_t payload[4] = {'D', 'F', (uint8_t) t, state};
  ZBTxRequest request = ZBTxRequest(BROADCAST, payload, sizeof(payload));
  xbee.send(request);
}

/**
 * Loop
 */
void loop() {  
  
  tempData.push(temperature());
  int t = tempData.mean();
  
  /**
   * Control heater
   */
  if (t < TEMP_ON) {
    HEAT_ON;
  } 
  else if (t >= TEMP_OFF) {
    HEAT_OFF;
  }

  /**
   * Log data every UPDATE_PERIOD seconds.
   */
  int state = digitalRead(RELAY);
  if (millis() > (lastUpdateTime + UPDATE_PERIOD * 1000)) {
    transmit(t, state);
    lastUpdateTime = millis();
  }

  /**
  * Output to console for debugging.
  */  
  Serial.print(t);
  Serial.print(" ");
  Serial.println(state);

  delay(1000);  
}

