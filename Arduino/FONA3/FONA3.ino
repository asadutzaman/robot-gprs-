/*Copyright Jorge Crespo 2016
 * Redistributable under the terms of the license GPLv3
 * Parts of the code are derived from the development of
 * Tony DiCola for Adafruit Industries
 * shared under the terms of the MIT license
 */
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_FONA.h"


// Arduino Pins
#define FONA_RX 2
#define FONA_TX 10
#define FONA_RST 4

//PIC Parallel Pins
#define P0 22
#define P1 23
#define P2 24
#define P3 25
#define P4 26
#define P5 27
#define P6 28
#define P7 29

//Functions headers
void PICSpeedRight(int velocity);
void PICSpeedLeft(int velocity);
void MQTT_connect();

//Global Variables
byte SpeedRightByte=0x05,  SpeedLeftByte=0x05;
signed int  SpeedRightInt=0,   SpeedLeftInt=0;

// Configure Adafruit IO access.
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "jorgecrce"
#define AIO_KEY         "5c9f4f8e308a307c1594a197b4e6e028410dc3e1"

//APN settings
#define FONA_APN       "hs.vodafone.ie"
#define FONA_USERNAME  ""
#define FONA_PASSWORD  ""

// Create a software serial instance to talk to FONA.
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);//Inverted, it means that the wire goes to fona tx and fona rx port
// Create a FONA library instance.
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);


/************ Global State (you don't need to change this!) ******************/

// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the FONA MQTT class by passing in the FONA class and MQTT server and login details.
Adafruit_MQTT_FONA mqtt(&fona, MQTT_SERVER, AIO_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }


// FONAconnect is a helper function that sets up the FONA and connects to the GPRS Network
boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password);

/****************************** Feeds ***************************************/
// Setup a feed called 'RightSpeed' for subscribing to changes.
const char RightSpeed_FEED[] PROGMEM = AIO_USERNAME "/feeds/RightSpeed";
Adafruit_MQTT_Subscribe RightSpeed = Adafruit_MQTT_Subscribe(&mqtt, RightSpeed_FEED);
// Setup a feed called 'LeftSpeedFeed' for subscribing to changes.
const char LeftSpeed_FEED[] PROGMEM = AIO_USERNAME "/feeds/LeftSpeed";
Adafruit_MQTT_Subscribe LeftSpeed = Adafruit_MQTT_Subscribe(&mqtt, LeftSpeed_FEED);

// How many transmission failures in a row we're willing to be ok with before reset
uint8_t txfailures = 0;
#define MAXTXFAILURES 5

void setup() {
  //Setup Pins
  pinMode (P0, OUTPUT);
  pinMode (P1, OUTPUT);
  pinMode (P2, OUTPUT);
  pinMode (P3, OUTPUT);
  pinMode (P4, OUTPUT);
  pinMode (P5, OUTPUT);
  pinMode (P6, OUTPUT);
  pinMode (P7, OUTPUT);

  // Setup serial port access.
  Serial.begin(115200);
  Serial.println(F("Adafruit IO ROVER Control"));

  // Setup FONA software serial port access.
  fonaSS.begin(4800);

    mqtt.subscribe(&LeftSpeed);
    mqtt.subscribe(&RightSpeed);

    delay(2500);  // wait a few seconds to stabilize connection

  //Initialize the fona using FONAconnect from fonahelper
   while (! FONAconnect(F(FONA_APN), F(FONA_USERNAME), F(FONA_PASSWORD))) {
    Serial.println("Retrying FONA");
  }
  Serial.println(F("Ready!"));

  //Delay to estabilize connection
  delay(4000);

}

void loop() {

  MQTT_connect();


  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &RightSpeed) {
      Serial.print(F("Velocidad Derecha: "));
      Serial.println((char *)RightSpeed.lastread);
      //RightSpeedFeed.lastread.intValue(&SpeedRightInt);
    }
    if (subscription == &LeftSpeed) {
      Serial.print(F("Velocidad izquierda: "));
      Serial.println((char *)LeftSpeed.lastread);
      //LeftSpeedFeed.lastread.intValue(&SpeedLeftInt);
    }
  }

  PICSpeedRight(SpeedRightInt);
  PICSpeedLeft(SpeedLeftInt);
}

void PICSpeedRight(int velocity){//Less significant bits. P3 - P2 - P1 - P0
  switch (velocity){
    case -100:
      digitalWrite(P3, LOW);
      digitalWrite(P2, LOW);
      digitalWrite(P1, LOW);
      digitalWrite(P0, HIGH);
      //return '1';
      break;
    case -80:
      digitalWrite(P3, LOW);
      digitalWrite(P2, LOW);
      digitalWrite(P1, HIGH);
      digitalWrite(P0, LOW);
      //return '2';
      break;
    case -60:
      digitalWrite(P3, LOW);
      digitalWrite(P2, LOW);
      digitalWrite(P1, HIGH);
      digitalWrite(P0, HIGH);
      //return '3';
      break;
    case -40:
      digitalWrite(P3, LOW);
      digitalWrite(P2, HIGH);
      digitalWrite(P1, LOW);
      digitalWrite(P0, LOW);
      //return '4';
      break;
    case -20:
      digitalWrite(P3, LOW);
      digitalWrite(P2, HIGH);
      digitalWrite(P1, LOW);
      digitalWrite(P0, HIGH);
      //return '5';
      break;
    case 0:
      digitalWrite(P3, LOW);
      digitalWrite(P2, HIGH);
      digitalWrite(P1, HIGH);
      digitalWrite(P0, LOW);
      //return '6';
      break;
    case 20:
      digitalWrite(P3, LOW);
      digitalWrite(P2, HIGH);
      digitalWrite(P1, HIGH);
      digitalWrite(P0, HIGH);
      //return '7';
      break;
    case 40:
      digitalWrite(P3, HIGH);
      digitalWrite(P2, LOW);
      digitalWrite(P1, LOW);
      digitalWrite(P0, LOW);
      //return '8';
      break;
    case 60:
      digitalWrite(P3, HIGH);
      digitalWrite(P2, LOW);
      digitalWrite(P1, LOW);
      digitalWrite(P0, HIGH);
      //return '9';
      break;
    case 80:
      digitalWrite(P3, HIGH);
      digitalWrite(P2, LOW);
      digitalWrite(P1, HIGH);
      digitalWrite(P0, LOW);
      //return 'a';
      break;
    case 100:
      digitalWrite(P3, HIGH);
      digitalWrite(P2, LOW);
      digitalWrite(P1, HIGH);
      digitalWrite(P0, HIGH);
      //return 'b';
      break;
    default:
      digitalWrite(P3, LOW);
      digitalWrite(P2, LOW);
      digitalWrite(P1, LOW);
      digitalWrite(P0, LOW);
      //return '6'; //(Speed=0)
      break;
  }
}
void PICSpeedLeft(int velocity){//Less significant bits. P7 - P6 - P5 - P4
  switch (velocity){
    case -100:
      digitalWrite(P7, LOW);
      digitalWrite(P6, LOW);
      digitalWrite(P5, LOW);
      digitalWrite(P4, HIGH);
      //return '1';
      break;
    case -80:
      digitalWrite(P7, LOW);
      digitalWrite(P6, LOW);
      digitalWrite(P5, HIGH);
      digitalWrite(P4, LOW);
      //return '2';
      break;
    case -60:
      digitalWrite(P7, LOW);
      digitalWrite(P6, LOW);
      digitalWrite(P5, HIGH);
      digitalWrite(P4, HIGH);
      //return '3';
      break;
    case -40:
      digitalWrite(P7, LOW);
      digitalWrite(P6, HIGH);
      digitalWrite(P5, LOW);
      digitalWrite(P4, LOW);
      //return '4';
      break;
    case -20:
      digitalWrite(P7, LOW);
      digitalWrite(P6, HIGH);
      digitalWrite(P5, LOW);
      digitalWrite(P4, HIGH);
      //return '5';
      break;
    case 0:
      digitalWrite(P7, LOW);
      digitalWrite(P6, HIGH);
      digitalWrite(P5, HIGH);
      digitalWrite(P4, LOW);
      //return '6';
      break;
    case 20:
      digitalWrite(P7, LOW);
      digitalWrite(P6, HIGH);
      digitalWrite(P5, HIGH);
      digitalWrite(P4, HIGH);
      //return '7';
      break;
    case 40:
      digitalWrite(P7, HIGH);
      digitalWrite(P6, LOW);
      digitalWrite(P5, LOW);
      digitalWrite(P4, LOW);
      //return '8';
      break;
    case 60:
      digitalWrite(P7, HIGH);
      digitalWrite(P6, LOW);
      digitalWrite(P5, LOW);
      digitalWrite(P4, HIGH);
      //return '9';
      break;
    case 80:
      digitalWrite(P7, HIGH);
      digitalWrite(P6, LOW);
      digitalWrite(P5, HIGH);
      digitalWrite(P4, LOW);
      //return 'a';
      break;
    case 100:
      digitalWrite(P7, HIGH);
      digitalWrite(P6, LOW);
      digitalWrite(P5, HIGH);
      digitalWrite(P4, HIGH);
      //return 'b';
      break;
    default:
      digitalWrite(P7, LOW);
      digitalWrite(P6, LOW);
      digitalWrite(P5, LOW);
      digitalWrite(P4, LOW);
      //return '6'; //(Speed=0)
      break;
  }
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  return;

  Serial.print("Connecting to MQTT... ");

  /*
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
  }*/
  ret = mqtt.connect();
  delay(5000);
  Serial.println("MQTT Connected!");
}
