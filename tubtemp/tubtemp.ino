/**
   Hot Tub Temperature Controller with XBee output
   Copyright 2015 Andrew Bythell <abythell@ieee.org>
   http://angryelectron.com
*/

#include <SoftwareSerial.h>
#include <Average.h>
#include <XBee.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/**
   Hardware Settings
*/
#define DS1820  7
#define RELAY 8
#define HEAT_ON  digitalWrite(RELAY, HIGH); digitalWrite(13,HIGH);
#define HEAT_OFF digitalWrite(RELAY, LOW); digitalWrite(13,LOW);

/**
   User Settings
*/
#define TNORM 103  /* desired temperature (degrees F) */
#define TDIFF 1
#define REPORTING_PERIOD 300


/**
   Globals
*/
Average<int> tempData(REPORTING_PERIOD);
SoftwareSerial softSerial(10, 11);
OneWire oneWire(DS1820);
DallasTemperature ds1820(&oneWire);
XBee xbee = XBee();
XBeeAddress64 BROADCAST = XBeeAddress64(0x0, BROADCAST_ADDRESS);
unsigned long sampleTime = 0;
unsigned long reportingTime = 0;

/**
   Setup
*/
void setup() {
  Serial.begin(9600);
  ds1820.begin();
  softSerial.begin(9600);
  xbee.setSerial(softSerial);
  pinMode(RELAY, OUTPUT);
  pinMode(13, OUTPUT); /* onboard LED indicates call for heat */

  // fill buffer with start-up temperature
  int startupTemperature = temperature();
  for (int i = 0; i < REPORTING_PERIOD; i++) {
    tempData.push(startupTemperature);
  }
}

/**
   Read the temperature from the first DS1820 sensor on the
   bus.  The temperature reading is further adjusted by
   TDIFF to compensate for the difference in temperature
   between the water in the tub and the water at the sensor.
*/
int temperature() {
  ds1820.requestTemperatures();
  float f = ds1820.getTempFByIndex(0);
  return round(f) + TDIFF;
}

/**
   Transmit temperature and heater state over XBee network.
*/
void transmit(int t, byte state) {
  uint8_t payload[4] = {'D', 'F', (uint8_t) t, state};
  ZBTxRequest request = ZBTxRequest(BROADCAST, payload, sizeof(payload));
  xbee.send(request);
}

/**
   Loop
*/
void loop() {

  unsigned long currentTime = millis();

  if ( currentTime > sampleTime + 1000) {
    int t = temperature();
    tempData.push(t);
    Serial.println(t);
    sampleTime = currentTime;
  }

  if (currentTime > reportingTime + REPORTING_PERIOD * 1000) {
    int t = tempData.mean();
    if (t < TNORM) {
      HEAT_ON;
    }
    else {
      HEAT_OFF;
    }
    Serial.println("Transmitting");
    transmit(tempData.mean(), digitalRead(RELAY));
    reportingTime = currentTime;
  }
}

