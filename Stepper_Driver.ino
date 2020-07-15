const int ENCODER_BUTTON = 2;       //Encoder tactile button
const int ENCODER_A = 3;            //Encoder channel A
const int ENCODER_B = 4;            //Encoder channel B
const int STEPPER_DIR = 5;          //Stepper Driver direction
const int STEPPER_PULSE = 6;        //Stepper Driver pulse
const int STEPPER_ENABLE = 7;       //Stepper Driver enable toggle

const int DIRECTION_CCW = HIGH;     //STEPPER DIRECTIONS
const int DIRECTION_CW = LOW;

const int STEPPER_DELAY_uS = 1500;  //Time between each step while moving

boolean isLocked = false;

//Encoder button state
byte prevButton = 0;
byte currButton = 0;

//Encoder channel A / B state
byte seqA = 0b00000000;
byte seqB = 0b00000000;
byte prevA = 0;
byte prevB = 0;
byte currA = 0;
byte currB = 0;

//Stepper motor movement state
int step_dir = DIRECTION_CW;
int encoderSteps = 0;
int encoderVelocity = 0;

//Time state
long prevTime = 0;
long currTime = 0;
long elapsedTime = 0;

void setup() {
  //Configure I/O
  pinMode(ENCODER_BUTTON, INPUT);
  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);
  pinMode(STEPPER_DIR, OUTPUT);
  pinMode(STEPPER_PULSE, OUTPUT);
  pinMode(STEPPER_ENABLE, OUTPUT);
  
  //Enable internal pullups
  digitalWrite(ENCODER_BUTTON, HIGH);
  digitalWrite(ENCODER_A, HIGH);
  digitalWrite(ENCODER_B, HIGH);
  digitalWrite(STEPPER_ENABLE, LOW);

  Serial.begin(9600);
}

void toggleLock(){
  isLocked = !isLocked;
  int s = (isLocked) ? LOW : HIGH;
  digitalWrite(STEPPER_ENABLE, s);  
}

void step_motor(){
  digitalWrite(STEPPER_DIR, step_dir);
  digitalWrite(STEPPER_PULSE, LOW);
  digitalWrite(STEPPER_PULSE, HIGH);
  delayMicroseconds(STEPPER_DELAY_uS);
}

void loop(){

  prevButton = currButton;
  currButton = digitalRead(ENCODER_BUTTON);

  //Toggle movement lock on button FALLING edge
  if(currButton == HIGH && prevButton == LOW){
    delay(1);
    Serial.print(encoderSteps, DEC);
    toggleLock();
  }

  //Lock
  if(isLocked)
    return;
  
  prevA = currA;
  prevB = currB;
  currA = digitalRead(ENCODER_A);
  currB = digitalRead(ENCODER_B);

  //On encoder state change
  if(prevA != currA || prevB != currB){
    
    seqA <<= 1;
    seqA |= currA;      //Write 1 if value is HIGH

    seqB <<= 1;
    seqB |= currB;      //Write 1 if value is HIGH

    seqA &= 0b00001111; //Masks out unused bits
    seqB &= 0b00001111;

    //Clockwise direction
    if(seqA == 0b00001001 && seqB == 0b00000011){
      step_dir = DIRECTION_CW;
      encoderSteps++;
      Serial.print(encoderSteps, DEC);
    }

    //Anti-Clockwise direction
    if(seqA == 0b00000011 && seqB == 0b00001001){
      step_dir = DIRECTION_CCW;
      encoderSteps++;
      Serial.print(encoderSteps, DEC);
    }
  }

  //Update encoder velocity
  prevTime = currTime;
  currTime = millis();
  elapsedTime += (currTime - prevTime);
  if(elapsedTime > 20){
    encoderVelocity = (encoderSteps) * (encoderSteps + 1) * (encoderSteps + 2) * 2.5;
    encoderSteps = 0;
    elapsedTime = 0;
  }

  //Step motor
  if(encoderVelocity != 0){
    while(elapsedTime < encoderVelocity){
      prevTime = currTime;
      currTime = millis();
      elapsedTime += currTime - prevTime;
      step_motor();
    }
    elapsedTime = 0;
    encoderVelocity = 0;
  }
}
