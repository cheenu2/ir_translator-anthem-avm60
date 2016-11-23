void initSerial () {
  Serial.begin(115200); // open serial port, and set data rate
  while (!Serial) ; // wait for serial port to connect
}

void initPins () {
  // all pins to output and LOW
  for (byte i = 0; i < 20; i++) {
    if (i != PD1) {
      digitalWrite (i, LOW);
      pinMode (i, OUTPUT);
    }
  }
  // however the PD1 pin is an input
  pinMode (PD1, INPUT);
}
