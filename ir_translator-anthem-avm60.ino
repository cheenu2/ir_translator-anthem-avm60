/*
 * An IR LED must be connected to Arduino PWM pin 3.
 */
#include <IRremote.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <IRremote.h>

#define IR_RX_PIN 12
#define RS232_RX_PIN 2
#define RS232_TX_PIN 3

#define TIME_HEADER  "T"   // Header tag for serial time sync message
//#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

IRrecv irrecv(IR_RX_PIN);
SoftwareSerial anthemSerial(RS232_RX_PIN, RS232_TX_PIN); // RX, TX
IRsend irsend;

decode_results results;

void setup() {
  initSerial();
  initPins();

  pinMode(IR_RX_PIN, INPUT);   // signal from sensor
  pinMode(RS232_RX_PIN, INPUT);
  pinMode(RS232_TX_PIN, OUTPUT);

  irrecv.enableIRIn();
  irrecv.blink13(true);

  //setSyncProvider(requestTimeSync);  //set function to call when sync required
  Serial.println("to set date/time copy/paste output of 'echo \"T$(($(date +%s)-6*60*60))\"' terminal command in serial input above");
  Serial.println("IR LED must be connected to Arduino PWM pin 3");

  Serial.println("ready");

  anthemSerial.begin(115200); // open serial connection and set baud rate
}

void loop() {
  if (Serial.available()) {
    processTimeSyncMessage();
  }
  if (irrecv.decode(&results)) {
    printDateTime(now());
    Serial.print(" ");
    Serial.print(F("received from remote:0x"));
    Serial.print(results.value, HEX);
    //dump(&results);
    String anthemCommand;
    unsigned long irCommand;
    if (results.value == 0x490) {
      anthemCommand = "ZxSIM0025;";
      Serial.println("(volume up), sending to avm60 rs232:" + anthemCommand);
      anthemSerial.println(anthemCommand);

      irCommand = 0x9F;
      Serial.println("(volume up), sending to avm60 IR:" + irCommand);
      irsend.sendSony(irCommand, 12);
    } else if (results.value == 0xC90) {
      anthemCommand = "ZxSIM0026;";
      Serial.println("(volume down), sending to avm60 rs232:" + anthemCommand);
      anthemSerial.println(anthemCommand);

      irCommand = 0x9E;
      Serial.println("(volume down), sending to avm60 IR:" + irCommand);
      irsend.sendSony(irCommand, 12);
    } else if (results.value == 0x290) {
      anthemCommand = "ZxSIM0027;";
      Serial.println("(toggle mute), sending to avm60 rs232:" + anthemCommand);
      anthemSerial.println(anthemCommand);

      irCommand = 0x99;
      Serial.println("(toggle mute), sending to avm60 IR:" + irCommand);
      irsend.sendSony(irCommand, 12);
    } else {
      //Serial.print(results.value, HEX);
      Serial.println();
    }
    delay(175);
    irrecv.resume(); // receive the next value
  }
}

void printDateTime(time_t t) {
  Serial.print(padDigits(hour(t)));
  Serial.print(":");
  Serial.print(padDigits(minute(t)));
  Serial.print(":");
  Serial.print(padDigits(second(t)));
  Serial.print("-");
  Serial.print(month(t));
  Serial.print("/");
  Serial.print(day(t));
  Serial.print("/");
  Serial.print(year(t));
}

String padDigits(int digits) {
  String result;
  if (digits < 10) {
    result = "0" + String(digits);
  } else {
    result = String(digits);
  }
  return result;
}

void processTimeSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1451606400; // Jan 1 2016

  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    if (pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2016)
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
      Serial.print("date/time set to ");
      printDateTime(now());
      Serial.println();
    }
  }
}

//time_t requestTimeSync() {
//  //Serial.write(TIME_REQUEST);
//  return 0; // the time will be set later in response to serial mesg
//}

