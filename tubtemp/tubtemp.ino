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
#define TDELTA 0    /* delta between on and off temperatures */
#define TDIFF 1
#define SAMPLE_PERIOD  300 
#define REPORTING_PERIOD 300


/**
 * Globals
 */
Average<int> tempData(SAMPLE_PERIOD);
SoftwareSerial softSerial(10, 11);
OneWire oneWire(DS1820);
DallasTemperature ds1820(&oneWire);
XBee xbee = XBee();
XBeeAddress64 BROADCAST = XBeeAddress64(0x0, BROADCAST_ADDRESS);
long reportingTime = REPORTING_PERIOD; // force report on boot
long sampleTime = SAMPLE_PERIOD;  // force sample on boot

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

  // fill buffer with start-up temperature
  int startupTemperature = temperature();
  for (int i=0; i<SAMPLE_PERIOD; i++) {
    tempData.push(startupTemperature);
  }
}

/**
 * Read the temperature from the first DS1820 sensor on the
 * bus.  The temperature reading is further adjusted by
 * TDELTA to compensate for the difference in temperature
 * between the water in the tub and the water at the sensor.
 */
int temperature() {
  ds1820.requestTemperatures();
  float f = ds1820.getTempFByIndex(0);
  return round(f) + TDIFF;
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
  int state = digitalRead(RELAY);
  
  /**
   * Control heater.
   */
  if (sampleTime >= SAMPLE_PERIOD) {
    if (t < (TNORM - TDELTA)) {
      HEAT_ON;
    }
    else {
      HEAT_OFF;
    }
    sampleTime = 0;
  }

  /**
   * Log data.
   */
  if (reportingTime >= REPORTING_PERIOD) {
    transmit(t, state);
    reportingTime = 0;
  }

  /**
  * Output to console for debugging.
  */
  Serial.print(temperature()); // actual temp
  Serial.print(", ");
  Serial.print(t);  // mean temp
  Serial.print(", ");
  Serial.println(state);

  reportingTime++;
  sampleTime++;
  delay(1000);
}

