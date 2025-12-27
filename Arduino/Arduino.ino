const int NUM_SERVOS = 8;
//const int servoPins[NUM_SERVOS] = {0,3,0,5,0,7,0,9};  // Turners
//const int servoPins[NUM_SERVOS] = {2,0,4,0,6,0,8,0};  // Sliders
const int servoPins[NUM_SERVOS] = {2,3,4,5,6,7,8,9};  // ALL

// Calibration values (microseconds)
int centerUS[NUM_SERVOS] = {760,1365,755,1460,2305,1535,695,1345};
int minUS[NUM_SERVOS]    = {500,340,500,490,500,485,500,400};
int maxUS[NUM_SERVOS]    = {2500,2400,2500,2500,2500,2530,2500,2450};

int selectedServo = 0;
char mode = 'c';                // current UI mode
char servoMode[NUM_SERVOS];     // last position of EACH servo
const int step = 5;             // microseconds per adjustment

void setup() {
  Serial.begin(9600);

  Serial.println("Servo calibration tool (manual PWM)");
  Serial.println("0-7 : select servo");
  Serial.println("c   : CENTER");
  Serial.println("l   : LEFT");
  Serial.println("r   : RIGHT");
  Serial.println("+/- : adjust value");
  Serial.println("p   : print calibration");
  Serial.println();

  for (int i = 0; i < NUM_SERVOS; i++) {
    pinMode(servoPins[i], OUTPUT);
    digitalWrite(servoPins[i], LOW);
    servoMode[i] = 'c';   // start all servos at center
  }

  printStatus();
}

void loop() {
  sendPWMAll();   // Drive all servos every 20 ms

  if (!Serial.available()) return;

  char c = Serial.read();

  if (c >= '0' && c <= '7') {
    selectedServo = c - '0';
    mode = servoMode[selectedServo];  // load last mode of that servo
    printStatus();
  }
  else if (c == 'c' || c == 'l' || c == 'r') {
    mode = c;
    servoMode[selectedServo] = c;     // store per-servo mode
    printStatus();
  }
  else if (c == '+') {
    adjust(step);
  }
  else if (c == '-') {
    adjust(-step);
  }
  else if (c == 'p') {
    printCalibration();
  }
}

void adjust(int delta) {
  if (mode == 'c') centerUS[selectedServo] += delta;
  else if (mode == 'l') minUS[selectedServo] += delta;
  else if (mode == 'r') maxUS[selectedServo] += delta;

  printStatus();
}

void sendPWMAll() {
  unsigned long frameStart = micros();

  for (int i = 0; i < NUM_SERVOS; i++) {
    int pulseWidth;

    if (servoMode[i] == 'c')      pulseWidth = centerUS[i];
    else if (servoMode[i] == 'l') pulseWidth = minUS[i];
    else                          pulseWidth = maxUS[i];

    digitalWrite(servoPins[i], HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(servoPins[i], LOW);
  }

  // Finish 20 ms servo frame
  unsigned long frameTime = micros() - frameStart;
  if (frameTime < 20000) {
    delayMicroseconds(20000 - frameTime);
  }
}

void printStatus() {
  Serial.print("Servo ");
  Serial.print(selectedServo);
  Serial.print(" | Mode: ");
  Serial.print(mode == 'c' ? "CENTER" : mode == 'l' ? "LEFT" : "RIGHT");
  Serial.print(" | C=");
  Serial.print(centerUS[selectedServo]);
  Serial.print(" L=");
  Serial.print(minUS[selectedServo]);
  Serial.print(" R=");
  Serial.println(maxUS[selectedServo]);
}

void printCalibration() {
  Serial.println("VALUES:");

  Serial.print("int centerUS[NUM_SERVOS] = {");
  for (int i = 0; i < NUM_SERVOS; i++) {
    Serial.print(centerUS[i]);
    if (i < NUM_SERVOS - 1) Serial.print(",");
  }
  Serial.println("};");

  Serial.print("int minUS[NUM_SERVOS] = {");
  for (int i = 0; i < NUM_SERVOS; i++) {
    Serial.print(minUS[i]);
    if (i < NUM_SERVOS - 1) Serial.print(",");
  }
  Serial.println("};");

  Serial.print("int maxUS[NUM_SERVOS] = {");
  for (int i = 0; i < NUM_SERVOS; i++) {
    Serial.print(maxUS[i]);
    if (i < NUM_SERVOS - 1) Serial.print(",");
  }
  Serial.println("};");
}
