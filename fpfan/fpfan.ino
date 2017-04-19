/**
 * Fireplace Fan Control
 * Copyright 2016 Andrew Bythell <abythell@ieee.org>
 * http://angryelectron.com
 */


/**
 * Hardware Settings
 */
#define SWITCH 2
#define RELAY 8
#define FAN_ON  digitalWrite(RELAY, HIGH); digitalWrite(13,HIGH);
#define FAN_OFF digitalWrite(RELAY, LOW); digitalWrite(13,LOW);

/**
 * Setup
 */
void setup() {
  Serial.begin(9600);
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(RELAY, OUTPUT);
  pinMode(13, OUTPUT); /* onboard LED indicates call for fan */
  FAN_OFF;
  attachInterrupt(digitalPinToInterrupt(SWITCH), fanControl, CHANGE);
}

void fanControl() {
  if (digitalRead(SWITCH)) {
    FAN_OFF;
  } else {
    FAN_ON;
  }
}

/**
 * Loop
 */
void loop() {
  /*
  if (digitalRead(SWITCH)) {
    FAN_OFF;
  } else {
    FAN_ON;
  }
  */
}

