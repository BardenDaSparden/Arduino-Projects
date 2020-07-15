const int RED_CHANNEL = 6;
const int GREEN_CHANNEL = 5;
const int BLUE_CHANNEL = 3;

const int LED_OUT = 9;
const int LED_OUT_2 = 10;
const int LED_OUT_3 = 11;
const int LED_OUT_4 = 12;

const int BRIGHTNESS_BUTTON = 2;

const int LED_1 = B00000001;
const int LED_2 = B00000010;
const int LED_3 = B00000100;
const int LED_4 = B00001000;

//comment out if using common cathode RGB LED's
#define COMMON_ANODE;

typedef struct RGBColor{
  short r;
  short g;
  short b;
};
RGBColor color = {0, 0, 0};

int counter = 0;
int interval = 15;

int ledMask = LED_1;
int idx = 0;

const int NUM_PATTERNS = 4;
int patterns[NUM_PATTERNS] = {
  LED_1 | LED_2,
  LED_2 | LED_3,
  LED_3 | LED_4,
  LED_4 | LED_1
};

int RGBFromHSL[] = {0, 0, 0};
float hue = 0.0f;
float brightness = 0.5f;

void setup(){

  //configure outputs
  pinMode(RED_CHANNEL, OUTPUT);
  pinMode(GREEN_CHANNEL, OUTPUT);
  pinMode(BLUE_CHANNEL, OUTPUT);

  pinMode(LED_OUT, OUTPUT);
  pinMode(LED_OUT_2, OUTPUT);
  pinMode(LED_OUT_3, OUTPUT);
  pinMode(LED_OUT_4, OUTPUT);

  //configure inputs
  pinMode(BRIGHTNESS_BUTTON, INPUT);
  pinMode(BRIGHTNESS_BUTTON, INPUT_PULLUP);
  
  //setup serial writer for debugging @ 9600 bps
  //Serial.begin(9600);
}

void loop() {

//  if(digitalRead(BRIGHTNESS_BUTTON) == LOW){
//    if(brightness < 0.05f)
//      brightness = 0.05f;
//
//    brightness *= 1.4f;
//    
//    if(brightness > 0.5f)
//      brightness = 0.5f;
//    
//  } else {
//   brightness *= 0.93f;  
//  }

  

  HSLtoRGB(hue, 1, brightness);
  hue += 1.0f / 256;
  if(hue > 1.0) hue = 0.0;
  setColor(RGBFromHSL[0], RGBFromHSL[1], RGBFromHSL[2]);
  
  if(counter % 6 == 0){
    idx = (idx + 1) % NUM_PATTERNS;
    setPattern(patterns[idx]);  
  }
  
  _update();
}

void HSLtoRGB(float h, float s, float l){ 
  float r, g, b;

  if (s == 0.0f)
      r = g = b = l;

  else {
      float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
      float p = 2.0f * l - q;
      r = HUEtoRGB(p, q, h + 1.0f / 3.0f);
      g = HUEtoRGB(p, q, h);
      b = HUEtoRGB(p, q, h - 1.0f / 3.0f);
  }

  RGBFromHSL[0] = (int)(r * 255);
  RGBFromHSL[1] = (int)(g * 255);
  RGBFromHSL[2] = (int)(b * 255);
}

float HUEtoRGB(float p, float q, float t)
{
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

void clamp(short low, short high, short* value){
  if(*value < low)
    *value = low;
  if(*value > high)
    *value = high;  
}

void _update(){
  clamp(0, 255, &color.r);
  clamp(0, 255, &color.g);
  clamp(0, 255, &color.b);
  
  writeColor();
  writePattern();
  delay(30);
  counter++;  
}

void setColor(short r, short g, short b){
  color.r = r;
  color.g = g;
  color.b = b;
}

void setPattern(int mask){
  ledMask = mask;
}

void writeColor(){
  short red = color.r;
  short green = color.g;
  short blue = color.b;
  
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(RED_CHANNEL, red);
  analogWrite(GREEN_CHANNEL, green);
  analogWrite(BLUE_CHANNEL, blue);
}

void writeLED(bool on, short pin){
  if(on)
    digitalWrite(pin, HIGH);
  else
    digitalWrite(pin, LOW);
}

void writePattern(){
  bool led1 = ((ledMask >> 3) & 0x01) == 0x01;
  bool led2 = ((ledMask >> 2) & 0x01) == 0x01;
  bool led3 = ((ledMask >> 1) & 0x01) == 0x01;
  bool led4 = ((ledMask) & 0x01) == 0x01;
  writeLED(led1, LED_OUT);
  writeLED(led2, LED_OUT_2);
  writeLED(led3, LED_OUT_3);
  writeLED(led4, LED_OUT_4);
}
