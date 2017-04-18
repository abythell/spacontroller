Spa / Hot Tub Controller
========================
Arduino sketch for a temperature controlled relay to driving a spa heating
element.  Uses DS18200 for precise temperature control and XBee radio for
remote monitoring.

Operation
---------
A temperature reading is taken every second, with the call-for-heat decision 
made every 5 minutes based on the mean temperature.  Monitoring has shown this
to be effective at keeping a particular spa between 102 and 103 degrees without
excessive cycling or overshoot.  Your results may vary based on the size of the
spa, climate, and the size of the averaging buffer.

The mean temperature, along with the heater state (on/off), is also transmitted
over the XBee network approximately every 5 minutes.  The timing is not highly
critical or precise, using millis() to control the intervals to avoid timer
conflicts with SoftwareSerial or OneWire.

Hardware
--------
* 12V NO Relay driven by a 2N222 transitor via pin 8
* DS1820 temperature sensor on pin 7, with 4K7 pullup to VCC
* XBee connected via SofwareSerial, pins 10 & 11

Software
--------
The sketch uses the following Arduino libraries:

* SoftwareSerial
* Average
* OneWire
* DallasTemperature
* XBee

Add these to your libraries folder, or point your Arduino sketch folder at
the `spacontroller` folder to use the included libraries.

Settings
-----
* DS1820: 1-wire data bus pin
* RELAY: output pin to drive relay (call for heat when HIGH) 
* TNORM: desired temperature
* TDIFF: degrees to add to sensor reading, if different from actual water temp.
* REPORTING_PERIOD: when to drive relay, transmit via XBee, and size of buffer.

About
-----
* Copyright 2015-2017 Andrew Bythell, abythell@ieee.org
* http://angryelectron.com
