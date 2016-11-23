#include <IRremote.h>
#include <SoftwareSerial.h>

const int IR_RX_PIN = 12;
const int RS232_RX_PIN = 2;
const int RS232_TX_PIN = 3;

IRrecv irrecv(IR_RX_PIN);
SoftwareSerial anthemSerial(RS232_RX_PIN, RS232_TX_PIN); // RX, TX

decode_results results;

void setup() {
  initSerial();
  initPins();

  pinMode(IR_RX_PIN, INPUT);   // signal from sensor
  pinMode(RS232_RX_PIN, INPUT);
  pinMode(RS232_TX_PIN, OUTPUT);

  irrecv.enableIRIn();
  irrecv.blink13(true);

  Serial.println("ready");

  anthemSerial.begin(115200); // open serial connection and set baud rate
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    //dump(&results);
    String anthemCommand;
    if (results.value == 0x490) {
      anthemCommand = "ZxSIM0025;";
      Serial.println("volume up, sending " + anthemCommand);
      anthemSerial.println(anthemCommand);
    } else if (results.value == 0xC90) {
      anthemCommand = "ZxSIM0026;";
      Serial.println("volume down, sending " + anthemCommand);
      anthemSerial.println(anthemCommand);
    } else if (results.value == 0x290) {
      anthemCommand = "ZxSIM0027;";
      Serial.println("mute toggle, sending " + anthemCommand);
      anthemSerial.println(anthemCommand);
    } else {
      //Serial.println(results.value, HEX);
    }
    delay(175);
    irrecv.resume(); // receive the next value
  }
}

