#define BOARD_TEST "BTEST_UNKNOWN"
#define BOARD "B_UNKNOWN"

//#ifdef ARDUINO_AVR_UNO
//  #define BOARD "B_UNO" 
//#endif
#ifdef ARDUINO_ESP8266_NODEMCU
  #define BOARD "B_ARDUINO_NODEMCU_ESP8266"
#endif
#ifdef ESP8266_NODEMCU
  #define BOARD "B_NODEMCU_ESP8266"
#endif
#ifdef ESP8266
  #define BOARD "B_ESP8266"

//#ifdef ARDUINO_ESP8266_NODEMCU
//  #define BOARD "B_NODEMCU_ESP8266"

  #define RELAY 13 // D7
  
  #define LED_INDICATOR_G 5  // D1
  #define LED_INDICATOR_R 4  // D2
  #define LED_INDICATOR_B 0  // D3  
  
  #define LED_BRIGHTNESS HIGH
  
  //#define SENSOR_MOISTURE    A0
  //#define SENSOR_WATER_LEVEL 9 // Find an open/close sensor, like floating ball? 
  
  #define BTN_MANUAL_OFF 3  // D9
  #define BTN_MANUAL_ON  15 // D8
//#else
//  #define RELAY 13
//  
//  #define LED_INDICATOR_B 10
//  #define LED_INDICATOR_G 9
//  #define LED_INDICATOR_R 6
//    
//  #define LED_BRIGHTNESS 127
//
//  //#define SENSOR_MOISTURE    10
//  //#define SENSOR_WATER_LEVEL 9 
//  
//  #define BTN_MANUAL_OFF 4
//  #define BTN_MANUAL_ON  2 
#endif

#define LED_FADE_VALUE 2
#define ISR_DEBOUNCE_TIME 500

#define MSEC_24_HOURS 86400000
#define MSEC_12_HOURS 43200000
#define MSEC_5_MIN_TEST 300000
#define MIN_WATER_LEVEL 255
#define RUN_TIME 10000 //ms = 10 Seconds
// #define COLOR_RED   0
// #define COLOR_BLUE  1
// #define COLOR_GREEN 2
// #define COLOR_RED   3

// unsigned int moistureRead; //This variable stores the value received from Soil moisture sensor.
unsigned int waterReading;
unsigned long lastEndTime;
unsigned long currentTime;
unsigned long msTimerVar;
unsigned long startTime;
bool isWaterLevelGood;
bool isInErrorState;
bool timerHasLapsed;
bool isLedLit;
volatile int flag_manualWatering;
volatile int flag_emergencyStop;
unsigned long ms_lastStopIsrTime;
//unsigned long currentMillis;

// 0 = Manually triggered
// 1 = Timer lapsed
// 2 = Soil sensor 
enum EVENT_TRIGGER {
  EVENT_TRIGGER_MANUAL,
  EVENT_TRIGGER_TIMER,
  EVENT_TRIGGER_SOIL,
};

enum LED_COLOR {
  LED_COLOR_RED,
  LED_COLOR_GREEN,
  LED_COLOR_BLUE,
  LED_COLOR_WHITE,
  LED_COLOR_CYAN,
  LED_COLOR_PURPLE
};

void msec_sleep(unsigned int msecWaitTime) {
  unsigned long current_msec = millis();
  while ((millis() - current_msec) < msecWaitTime) { 
    yield(); 
  }
}

void isr_activateManualWatering() {
  flag_manualWatering = 1;
  return;
}

void isr_emergencyStop() {
  flag_emergencyStop = !flag_emergencyStop;
  return;
}

//bool activateManualWatering() {
//  int val = digitalRead(BTN_MANUAL_ON);
//  if (val == LOW) {
//    Serial.print("Activating manual watering because BTN_MANTUAL_ON == ");
//    Serial.print(val);
//    Serial.println("!");
//    return true; // Start from Manual.
//  }
//
//  return false;
//}

//void checkManualWatering() {
//  if (flag_manual_watering == 1) {
//    Serial.println("Activating manual watering!");
//  }
//}

void checkEmergencyState() {
  if (flag_emergencyStop == 1) {
    if ((millis() - ms_lastStopIsrTime) > ISR_DEBOUNCE_TIME) {
      if (isInErrorState == false) {
        Serial.println("Emergency Water Shutoff!");
        isInErrorState = true;
      }
      else {
        Serial.println("Clearing error state");
        isInErrorState = false;
        turnOffLed();
      }
      flag_emergencyStop = 0;
    }
  }
  return;
}

///TODO: This.
bool needsWatering() {
//  Serial.println("checking to see if needs watering...");
  msec_sleep(RUN_TIME);

//  Serial.println("...nope.");
  return false;
  // if (analogRead(SENSOR_MOISTURE) < 127) { // I dunno. Check values.
  //   Serial.println("Watering must occur!");
  //   return true; // Start from Sensor.
  // }
  // return false;
}

///TODO: This.
void updateWaterReading() {
  // waterReading = analogRead(SENSOR_WATER_LEVEL);
  // isWaterLevelGood = (waterReading > MIN_WATER_LEVEL);
  
  waterReading = MIN_WATER_LEVEL + 10; ///TESTING
  isWaterLevelGood = true;
  return;
}

//void turnOnLedWhite() {
//  Serial.println("LED ON WHITE");
//  analogWrite(LED_INDICATOR_G, LED_BRIGHTNESS);
//  analogWrite(LED_INDICATOR_B, LED_BRIGHTNESS);
//  analogWrite(LED_INDICATOR_R, LED_BRIGHTNESS);
//  isLedLit = true;
//}

void fadeOnOffLed(LED_COLOR ledColor) {
  Serial.println("fadeOnOffLed()");
  fadeOnLed(ledColor);
  fadeOffLed(ledColor);
  isLedLit = false;
}

void fadeOnLed(LED_COLOR ledColor) {
  Serial.println("fadeOnLed()");
  turnOffLed();
  
  for (int brightness = 0; brightness < LED_BRIGHTNESS; brightness += LED_FADE_VALUE) {
    
    switch(ledColor) {
      case LED_COLOR_WHITE:
        analogWrite(LED_INDICATOR_G, brightness);
        analogWrite(LED_INDICATOR_B, brightness);
        analogWrite(LED_INDICATOR_R, brightness);
        break;
      case LED_COLOR_RED:
        analogWrite(LED_INDICATOR_R, brightness);
        analogWrite(LED_INDICATOR_G, 0);
        analogWrite(LED_INDICATOR_B, 0);
        break;
      case LED_COLOR_GREEN:
        analogWrite(LED_INDICATOR_R, 0);
        analogWrite(LED_INDICATOR_G, brightness);
        analogWrite(LED_INDICATOR_B, 0);
        break;
      case LED_COLOR_BLUE:
        analogWrite(LED_INDICATOR_R, 0);
        analogWrite(LED_INDICATOR_G, 0);
        analogWrite(LED_INDICATOR_B, brightness);
        break;
      case LED_COLOR_CYAN:
        analogWrite(LED_INDICATOR_R, 0);
        analogWrite(LED_INDICATOR_G, brightness);
        analogWrite(LED_INDICATOR_B, brightness);
        break;
      case LED_COLOR_PURPLE:
        analogWrite(LED_INDICATOR_R, brightness);
        analogWrite(LED_INDICATOR_G, 0);
        analogWrite(LED_INDICATOR_B, brightness);
        break;
      default:
        return;
        break;
    }
  }
  isLedLit = true;
}

void fadeOffLed(LED_COLOR ledColor) {
  Serial.println("fadeOffLed()");

  for (int brightness = LED_BRIGHTNESS; brightness >= 0; brightness -= LED_FADE_VALUE) {
    msec_sleep(10);
    if (brightness <= LED_FADE_VALUE) { brightness = 0; }
    switch(ledColor) {
      case LED_COLOR_WHITE:
        analogWrite(LED_INDICATOR_G, brightness);
        analogWrite(LED_INDICATOR_B, brightness);
        analogWrite(LED_INDICATOR_R, brightness);
        break;
      case LED_COLOR_RED:
        analogWrite(LED_INDICATOR_R, brightness);
        analogWrite(LED_INDICATOR_G, 0);
        analogWrite(LED_INDICATOR_B, 0);
        break;
      case LED_COLOR_GREEN:
        analogWrite(LED_INDICATOR_R, 0);
        analogWrite(LED_INDICATOR_G, brightness);
        analogWrite(LED_INDICATOR_B, 0);
        break;
      case LED_COLOR_BLUE:
        analogWrite(LED_INDICATOR_R, 0);
        analogWrite(LED_INDICATOR_G, 0);
        analogWrite(LED_INDICATOR_B, brightness);
        break;
      case LED_COLOR_CYAN:
        analogWrite(LED_INDICATOR_R, 0);
        analogWrite(LED_INDICATOR_G, brightness);
        analogWrite(LED_INDICATOR_B, brightness);
        break;
      case LED_COLOR_PURPLE:
        analogWrite(LED_INDICATOR_R, brightness);
        analogWrite(LED_INDICATOR_G, 0);
        analogWrite(LED_INDICATOR_B, brightness);
        break;
      default:
        return;
        break;
    }
  }
  isLedLit = false;
}

void turnOnLedDigital(LED_COLOR ledColor) {
  Serial.println("turnOnLed()");
  switch(ledColor) {
    case LED_COLOR_WHITE:
      digitalWrite(LED_INDICATOR_G, LED_BRIGHTNESS);
      digitalWrite(LED_INDICATOR_B, LED_BRIGHTNESS);
      digitalWrite(LED_INDICATOR_R, LED_BRIGHTNESS);
      break;
    case LED_COLOR_RED:
      digitalWrite(LED_INDICATOR_R, LED_BRIGHTNESS);
      digitalWrite(LED_INDICATOR_G, 0);
      digitalWrite(LED_INDICATOR_B, 0);
      break;
    case LED_COLOR_GREEN:
      digitalWrite(LED_INDICATOR_R, 0);
      digitalWrite(LED_INDICATOR_G, LED_BRIGHTNESS);
      digitalWrite(LED_INDICATOR_B, 0);
      break;
    case LED_COLOR_BLUE:
      digitalWrite(LED_INDICATOR_R, 0);
      digitalWrite(LED_INDICATOR_G, 0);
      digitalWrite(LED_INDICATOR_B, LED_BRIGHTNESS);
      break;
    case LED_COLOR_CYAN:
      digitalWrite(LED_INDICATOR_R, 0);
      digitalWrite(LED_INDICATOR_G, LED_BRIGHTNESS);
      digitalWrite(LED_INDICATOR_B, LED_BRIGHTNESS);
      break;
    case LED_COLOR_PURPLE:
      digitalWrite(LED_INDICATOR_R, LED_BRIGHTNESS);
      digitalWrite(LED_INDICATOR_G, 0);
      digitalWrite(LED_INDICATOR_B, LED_BRIGHTNESS);
      break;
    default:
      return;
      break;
  }

  isLedLit = true;  
}

void turnOnLedAnalog(LED_COLOR ledColor) {
  Serial.println("turnOnLed()");
  switch(ledColor) {
    case LED_COLOR_WHITE:
      analogWrite(LED_INDICATOR_G, LED_BRIGHTNESS);
      analogWrite(LED_INDICATOR_B, LED_BRIGHTNESS);
      analogWrite(LED_INDICATOR_R, LED_BRIGHTNESS);
      break;
    case LED_COLOR_RED:
      analogWrite(LED_INDICATOR_R, LED_BRIGHTNESS);
      analogWrite(LED_INDICATOR_G, 0);
      analogWrite(LED_INDICATOR_B, 0);
      break;
    case LED_COLOR_GREEN:
      analogWrite(LED_INDICATOR_R, 0);
      analogWrite(LED_INDICATOR_G, LED_BRIGHTNESS);
      analogWrite(LED_INDICATOR_B, 0);
      break;
    case LED_COLOR_BLUE:
      analogWrite(LED_INDICATOR_R, 0);
      analogWrite(LED_INDICATOR_G, 0);
      analogWrite(LED_INDICATOR_B, LED_BRIGHTNESS);
      break;
    case LED_COLOR_CYAN:
      analogWrite(LED_INDICATOR_R, 0);
      analogWrite(LED_INDICATOR_G, LED_BRIGHTNESS);
      analogWrite(LED_INDICATOR_B, LED_BRIGHTNESS);
      break;
    case LED_COLOR_PURPLE:
      analogWrite(LED_INDICATOR_R, LED_BRIGHTNESS);
      analogWrite(LED_INDICATOR_G, 0);
      analogWrite(LED_INDICATOR_B, LED_BRIGHTNESS);
      break;
    default:
      return;
      break;
  }

  isLedLit = true;  
}

//void turnOnLedRed() {
//  Serial.println("LED ON RED");
//  analogWrite(LED_INDICATOR_G, 0);
//  analogWrite(LED_INDICATOR_B, 0);
//  analogWrite(LED_INDICATOR_R, LED_BRIGHTNESS);
//  isLedLit = true;
//}
//
//void turnOnLedGreen() {
//  Serial.println("LED ON GREEN");
//  analogWrite(LED_INDICATOR_R, 0);
//  analogWrite(LED_INDICATOR_B, 0);
//  analogWrite(LED_INDICATOR_G, LED_BRIGHTNESS);
//  isLedLit = true;
//}
//
//void turnOnLedCyan() {
//  Serial.println("LED ON CYAN");
//  analogWrite(LED_INDICATOR_G, LED_BRIGHTNESS);
//  analogWrite(LED_INDICATOR_R, 0);
//  analogWrite(LED_INDICATOR_B, LED_BRIGHTNESS);
//  isLedLit = true;
//}
//
//void turnOnLedBlue() {
//  Serial.println("LED ON BLUE");
//  analogWrite(LED_INDICATOR_G, 0);
//  analogWrite(LED_INDICATOR_R, 0);
//  analogWrite(LED_INDICATOR_B, LED_BRIGHTNESS);
//  isLedLit = true;
//}

void turnOffLed() {
  Serial.println("LED OFF");
  analogWrite(LED_INDICATOR_G, 0);
  analogWrite(LED_INDICATOR_R, 0);
  analogWrite(LED_INDICATOR_B, 0);
  isLedLit = false;
}

bool isWaterLow() {
  if (waterReading < MIN_WATER_LEVEL) {
    return true;
  }
  return false;
}

int endWatering() {
  Serial.println("End watering! Noice.");
  digitalWrite(RELAY, LOW);
  lastEndTime = millis();
  flag_emergencyStop = 0;
  turnOffLed();
  return 0; // Watering Aborted.
}


// Event Trigger means "what triggered this watering?"
// 0 = Manually triggered
// 1 = Timer lapsed
// 2 = Soil sensor 
int doWatering(EVENT_TRIGGER eventTrigger) {
  Serial.println("Start watering! Yeh boiiiiiiiiiiiiiiiiiiiii!");
  // startTime = millis();
  // currentTime = millis();
  // lastEndTime = millis();
  startTime = currentTime = msTimerVar = lastEndTime = millis(); 
  
  Serial.print("Start time: ");
  Serial.print(startTime);
  Serial.println("ms");
  
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.println("ms");
    
  Serial.print("Last End time: ");
  Serial.print(lastEndTime);
  Serial.println("ms");

  digitalWrite(RELAY, HIGH);
  while ((currentTime - startTime) < RUN_TIME) {
    #ifdef ARDUINO_NODEMCU_ESP8266
      turnOnLedDigital(LED_COLOR_BLUE);
    #else
      fadeOnLed(LED_COLOR_BLUE);
    #endif
    
    //fadeOnOffLed(LED_COLOR_BLUE);
    checkEmergencyState();
    if (isInErrorState) {
      Serial.println("Error state detected! Ending watering now!");
      endWatering();
      return -3; // Water is low.
    }
    currentTime = millis();
    updateWaterReading();
    if (isWaterLow()) {
      Serial.println("Water is low! Ending watering now!");
      endWatering();
      return -3; // Water is low.
    }
    if (currentTime < startTime) { // Millisecond Timer Overflow.
      Serial.print("Timer overflowed! Total elapsed time = ");
      Serial.print(((0xFFFFFFFF - currentTime) + millis()));
      Serial.println("ms");
      if (((0xFFFFFFFF - currentTime) + millis()) >= RUN_TIME) {
        return endWatering();
      }
      continue; 
    }

    if ((currentTime - msTimerVar) > 999) {
      Serial.print("Total elapsed time = ");
      Serial.print((currentTime - startTime));
      Serial.println("ms");
      msTimerVar = currentTime;
    }
    fadeOffLed(LED_COLOR_BLUE);
    switch (eventTrigger) {
      case EVENT_TRIGGER_MANUAL:
        Serial.println("Manually triggered");
        #ifdef ARDUINO_NODEMCU_ESP8266
          turnOnLedDigital(LED_COLOR_WHITE);
        #else
          fadeOnLed(LED_COLOR_WHITE);
        #endif
        break;
      case EVENT_TRIGGER_TIMER:
        Serial.println("Timer triggered");
        #ifdef ARDUINO_NODEMCU_ESP8266
          turnOnLedDigital(LED_COLOR_CYAN);
        #else
          fadeOnLed(LED_COLOR_CYAN);
        #endif
        break;
      case EVENT_TRIGGER_SOIL:
        Serial.println("Moisture sensor triggered");
        #ifdef ARDUINO_NODEMCU_ESP8266
          turnOnLedDigital(LED_COLOR_GREEN);
        #else
          fadeOnLed(LED_COLOR_GREEN);
        #endif
        default:
      break;
    }
    checkEmergencyState();
    if (isInErrorState) {
      Serial.println("Error state detected! Ending watering now!");
      endWatering();
      return -3; // Water is low.
    }
    currentTime = millis();
    updateWaterReading();
    if (isWaterLow()) {
      Serial.println("Water is low! Ending watering now!");
      endWatering();
      return -3; // Water is low.
    }
    switch (eventTrigger) {
      case EVENT_TRIGGER_MANUAL:
        Serial.println("Manually triggered");
        #ifdef ARDUINO_NODEMCU_ESP8266
          turnOnLedDigital(LED_COLOR_WHITE);
        #else
          fadeOnLed(LED_COLOR_WHITE);
        #endif
        break;
      case EVENT_TRIGGER_TIMER:
        Serial.println("Timer triggered");
        #ifdef ARDUINO_NODEMCU_ESP8266
          turnOnLedDigital(LED_COLOR_CYAN);
        #else
          fadeOnLed(LED_COLOR_CYAN);
        #endif
        break;
      case EVENT_TRIGGER_SOIL:
        Serial.println("Moisture sensor triggered");
        #ifdef ARDUINO_NODEMCU_ESP8266
          turnOnLedDigital(LED_COLOR_GREEN);
        #else
          fadeOnLed(LED_COLOR_GREEN);
        #endif
        default:
      break;
    }
  }

  return endWatering();
  //return 0; //
}

void doSetupLedSequence() {
  
  for (int lightIdx = 0; lightIdx < 4; ++lightIdx) {
//    for (int vez = 0; vez < 3; ++vez) {
    if (lightIdx == 0) { 
      #ifdef ARDUINO_NODEMCU_ESP8266
        turnOnLedDigital(LED_COLOR_RED);
      #else
        fadeOnLed(LED_COLOR_RED);
      #endif 
    }
    if (lightIdx == 2) { 
      #ifdef ARDUINO_NODEMCU_ESP8266
        turnOnLedDigital(LED_COLOR_BLUE);
      #else
        fadeOnLed(LED_COLOR_BLUE);
      #endif
    }
    if (lightIdx == 1) { 
      #ifdef ARDUINO_NODEMCU_ESP8266
        turnOnLedDigital(LED_COLOR_GREEN);
      #else
        fadeOnLed(LED_COLOR_GREEN);
      #endif
    }
    if (lightIdx == 3) { 
      #ifdef ARDUINO_NODEMCU_ESP8266
        turnOnLedDigital(LED_COLOR_WHITE);
      #else
        fadeOnLed(LED_COLOR_WHITE);
      #endif
    }
    msec_sleep(250);

    turnOffLed();
//    }
  }
  return;
}

void updateTimeKeeping() {
  unsigned long totalElapsedTime = 0;
  unsigned long theNowTime = millis();

  timerHasLapsed = false;

  if (theNowTime < lastEndTime) { // 32-bit millisecond timer has overflowed.
    totalElapsedTime = (0xFFFFFFFF - lastEndTime) + theNowTime; 
//    if (totalElapsedTime >= MSEC_24_HOURS) {
    if (totalElapsedTime >= MSEC_5_MIN_TEST) {
      timerHasLapsed = true;
    }
  }
//  else if ((theNowTime - lastEndTime) >= MSEC_24_HOURS) {  
  else if ((theNowTime - lastEndTime) >= MSEC_5_MIN_TEST) {  
    timerHasLapsed = true;
  }

  if (timerHasLapsed) {
    turnOnLedDigital(LED_COLOR_BLUE);
  }

  return;
}

void setup() {
  pinMode(RELAY,OUTPUT); 
  pinMode(LED_INDICATOR_R, OUTPUT);
  pinMode(LED_INDICATOR_G, OUTPUT);
  pinMode(LED_INDICATOR_B, OUTPUT);
//  pinMode(BTN_MANUAL_OFF, INPUT);
  pinMode(BTN_MANUAL_ON, INPUT);

//  attachInterrupt(digitalPinToInterrupt(BTN_MANUAL_ON), isr_activateManualWatering, FALLING);
//  attachInterrupt(digitalPinToInterrupt(BTN_MANUAL_OFF), isr_emergencyStop, FALLING);

//  digitalWrite(BTN_MANUAL_OFF, LOW);
//  digitalWrite(BTN_MANUAL_ON, LOW);
//  pinMode(SENSOR_MOISTURE,INPUT); 
//  pinMode(SENSOR_WATER_LEVEL, INPUT); 

  startTime = millis();
  currentTime = millis();
  lastEndTime = millis();
//  updateWaterReading();
//  doSetupLedSequence();
  isInErrorState = false;
  timerHasLapsed = false;
  flag_manualWatering = 0;
  flag_emergencyStop = 0;

  //Initialize serial and wait for port to open:
  Serial.begin(115200); // opens serial port, sets data rate 

  while (! Serial) { // wait for serial port to connect. Needed for native USB
    Serial.println("Waiting for serial connection...");
    msec_sleep(100);
  }
  
  if (BOARD == "B_UNO") {
    Serial.println("Arduino UNO detected!");
  }
  else {
    Serial.println("Not UNO!");
  }
  if (BOARD == "B_NODEMCU_ESP8266") {
    Serial.println("NodeMCU ESP8266 detected!");
  }
  else {
    Serial.println("Not NodeMCU ESP8266!");
  }
  if (BOARD_TEST == "B_ESP8266") {
    Serial.println("Generic ESP8266 detected!");    
  }
  else {
    Serial.println("Not Generic ESP8266!");
  }

  digitalWrite(LED_INDICATOR_R, HIGH);
  msec_sleep(1000);
  digitalWrite(LED_INDICATOR_G, HIGH);
  msec_sleep(1000);
  digitalWrite(LED_INDICATOR_B, HIGH);
  msec_sleep(1000);
  digitalWrite(LED_INDICATOR_R, LOW);
  msec_sleep(1000);
  digitalWrite(LED_INDICATOR_G, LOW);
  msec_sleep(1000);
  digitalWrite(LED_INDICATOR_B, LOW);
  
}

void loop() {
//  updateWaterReading();
//  updateTimeKeeping();
//  checkEmergencyState();
  
  if (isWaterLevelGood && (isInErrorState == false)) {
//    if (needsWatering()) {
//      Serial.println("NEEDS WATERING DETECTED!");
//      doWatering(EVENT_TRIGGER_SOIL);
//    }
//    if (timerHasLapsed) {
//      Serial.println("TIMER HAS LAPSED!");
//      doWatering(EVENT_TRIGGER_TIMER);
//    }
//    if (flag_manualWatering == 1) {
//      Serial.println("MANUAL WATERING BUTTON PRESSED!");
//      if (isInErrorState == false) {
//        doWatering(EVENT_TRIGGER_MANUAL);
//      }
//      else {
//        Serial.println("...BUT ERROR STATE DETECTED!");
//      }
//      flag_manualWatering = 0;  
//    }
  }
  else {
    if (isLedLit) {
      turnOffLed();
      msec_sleep(150);
    }
    else {
      if (isWaterLevelGood = false) {
        Serial.println("WATER LOW!");
        turnOnLedDigital(LED_COLOR_BLUE);
        msec_sleep(150);
      }
      if (isInErrorState) {
        Serial.println("ERROR STATE!");
        turnOnLedDigital(LED_COLOR_RED);
        msec_sleep(150);
      }
    }
  }
}
