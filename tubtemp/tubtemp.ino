/**
 * Hot Tub Temperature Controller with XBee output
 * Copyright 2015 Andrew Bythell <abythell@ieee.org>
 * http://angryelectron.com
 */
 
#include <SoftwareSerial.h>
#include <Average.h>
#include <XBee.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/**
 * Hardware Settings
 */
#define DS1820  7    
#define RELAY 8 
#define HEAT_ON  digitalWrite(RELAY, HIGH); digitalWrite(13,HIGH);
#define HEAT_OFF digitalWrite(RELAY, LOW); digitalWrite(13,LOW);

/**
 * User Settings
 */
#define TNORM 103  /* desired temperature (degrees F) */
#define THYST 1    /* difference between on and off temps */
#define TDELTA  4  /* difference between sensor and actual */
#define UPDATE_PERIOD  60  /* seconds between transmissions */

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
OneWire oneWire(DS1820);
DallasTemperature ds1820(&oneWire);
XBee xbee = XBee();
XBeeAddress64 BROADCAST = XBeeAddress64(0x0, BROADCAST_ADDRESS);
long lastUpdateTime = 0;

/**
 * Setup
 */
void setup() {
  Serial.begin(9600);  
  ds1820.begin();
  softSerial.begin(9600);
  xbee.setSerial(softSerial);
  pinMode(RELAY, OUTPUT);   
  pinMode(13, OUTPUT); /* onboard LED indicates call for heat */
}

/**
 * Read the temperature from the first DS1820 sensor on the
 * bus.  The temperature reading is further adjusted by
 * TDELTA to compensate for the difference in temperature 
 * between the water in the tub and the wajjjter at the sensor.
 */
int temperature() {  
  ds1820.requestTemperatures();
  float f = ds1820.getTempFByIndex(0);
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

