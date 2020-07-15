const int BUTTON_1 = 2;
const int BUTTON_2 = 3;
const int BUTTON_3 = 4;
const int BUTTON_4 = 5;

const int LED_1 = 6;
const int LED_2 = 7;
const int LED_3 = 8;
const int LED_4 = 9;

//Length of out software pwm cycle in micro-seconds
const int PWM_CYCLE_LENGTH = 2000;

short tHigh = 0;
short tLow = 0;

const short MAX_CYCLE_TIME = 1000;
const short MIN_CYCLE_TIME = 0;

//The Duty Cycle stored as a time [0 ... PWM_CYCLE_LENGTH / 2]
short t1 = 500;
short t2 = 0;
short t3 = 0;
short t4 = 0;

void setup(){

  //Configure button input pinouts
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);

  //Enable internal pullup resistors for our inputs
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(BUTTON_4, INPUT_PULLUP);

  //Configure led output pinouts
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
}

short clamp(short low, short high, short value){
  if(value > high)
    value = high;
  if(value < low)
    value = low;
  return value;
}

bool buttonDown(int btnPin){
  return (digitalRead(btnPin) == LOW);
}

void modulateT(int ledPin, int t){
  tHigh = t;
  tLow = PWM_CYCLE_LENGTH;

  digitalWrite(ledPin, HIGH);
  delayMicroseconds(tHigh);
  digitalWrite(ledPin, LOW);
  delayMicroseconds(tLow);
}

void modulateDC(int ledPin, int dc){
  int scale = PWM_CYCLE_LENGTH / 100;

  tHigh = dc * scale;
  tLow = PWM_CYCLE_LENGTH - tHigh;

  digitalWrite(ledPin, HIGH);
  delayMicroseconds(tHigh);
  digitalWrite(ledPin, LOW);
  delayMicroseconds(tLow);
}

short step(short time){
  short t = time + 5;
  t %= MAX_CYCLE_TIME;
  return t;
}

void reset(){
  t1 = 0;
  t2 = 0;
  t3 = 0;
  t4 = 0;  
}

void loop() {
  if(buttonDown(BUTTON_1)){
    t1 = step(t1);
  }

  if(buttonDown(BUTTON_2)){
    t2 = step(t2);
  }

  if(buttonDown(BUTTON_3)){
    t3 = step(t3);  
  }

  if(buttonDown(BUTTON_4)){
    reset();
  }

  modulateT(LED_1, t1);
  modulateT(LED_2, t2);
  modulateT(LED_3, t3);
  modulateT(LED_4, t4);
  
}
