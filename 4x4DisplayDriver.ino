const int R_OUT = 2;
const int R2_OUT = 3;
const int R3_OUT = 4;
const int R4_OUT = 5;

const int C_OUT = 8;
const int C2_OUT = 9;
const int C3_OUT = 10;
const int C4_OUT = 11;

const int DISPLAY_HERTZ = 60;
const int DISPLAY_WIDTH = 4;
const int DISPLAY_HEIGHT = 4;
const int SCAN_INTERVAL_US = (int)(1000000.0) * (1.0 / DISPLAY_HERTZ) * (1.0 / DISPLAY_HEIGHT);

//stored with respect to TOP LEFT origin
//arrays used for rendering to the display
bool pixels[DISPLAY_WIDTH * DISPLAY_HEIGHT];
bool scanline[DISPLAY_WIDTH];

//used for timestepping the loop in order to obtain ~60 Updates per second
const int TIMESTEP_US = (int)(1000000.0) * (1.0 / DISPLAY_HERTZ);
long currentUS = 0;
long previousUS = 0;
long elapsedUS = 0;
long frames = 0;

const int PATTERN_BASE_ROW =   0B0001 | 
                              0B0010 |
                              0B0100 |
                               0B1000;

const int NUM_FRAMES = 6;

int patterns[NUM_FRAMES] = {
  (
    1 << 4 | 
    1 << 5 | 
    1 << 10 | 
    1 << 11  
  ),
  
  (
    1 |
    1 << 5 |
    1 << 10 |
    1 << 15    
  ),
  
  (
    1 << 1 |
    1 << 5 | 
    1 << 10 |
    1 << 14
  ),

  
  (
    1 << 2 |
    1 << 6 | 
    1 << 9 |
    1 << 13 
  ),
  
  (
    1 << 3 | 
    1 << 6 | 
    1 << 9 | 
    1 << 12
  ),

  (
    1 << 6 | 
    1 << 7 |
    1 << 8 | 
    1 << 9 
  )
};

const int FRAME_INTERVAL = 300;

int patternIdx = 0;

void setup() {
  //configure outputs
  pinMode(R_OUT, OUTPUT);
  pinMode(R2_OUT, OUTPUT);
  pinMode(R3_OUT, OUTPUT);
  pinMode(R4_OUT, OUTPUT);
  pinMode(C_OUT, OUTPUT);
  pinMode(C2_OUT, OUTPUT);
  pinMode(C3_OUT, OUTPUT);
  pinMode(C4_OUT, OUTPUT);


  //configure default framebuffer
  for(int j = 0; j < DISPLAY_HEIGHT; j++){
    for(int i = 0; i < DISPLAY_WIDTH; i++){
        pixels[(j * DISPLAY_WIDTH) + i] = true;
    }
  }

  for(int i = 0; i < DISPLAY_WIDTH; i++){
    scanline[i] = true;
  }
  
}

void scan(int rowNum){
  int rowIdx = rowNum * DISPLAY_WIDTH;
  for(int i = 0; i < DISPLAY_WIDTH; i++){
    scanline[i] = pixels[rowIdx + i];
  }
}

void clearOutputs(){
  for(int j = 0; j < DISPLAY_HEIGHT; j++){
    digitalWrite(R_OUT + j, LOW);  
  }

  for(int i = 0; i < DISPLAY_WIDTH; i++){
    digitalWrite(C_OUT + i, HIGH);    
  }
}

void writeLine(int rowNum){
  for(int i = 0; i < DISPLAY_WIDTH; i++){
    bool pixel = scanline[i];
    int state = (pixel) ? LOW : HIGH;  
    digitalWrite(C_OUT + i, state);
  }

  for(int j = 0; j < DISPLAY_HEIGHT; j++){
    digitalWrite(R_OUT + j, LOW);  
  }

  digitalWrite(R_OUT + rowNum, HIGH);
}

void clearDisplay(){
  for(int j = 0; j < DISPLAY_HEIGHT; j++){
    for(int i = 0; i < DISPLAY_WIDTH; i++){
      pixels[j * DISPLAY_HEIGHT + i] = false;
    }
  }  
}

void drawInt4x4(int iBuffer){
  for(int i = 0; i < 16; i++){
    bool pixel = (iBuffer >> i) & (0x01);
    pixels[i] = pixel;
  }
}

void drawPoint(int x, int y){
  pixels[y * DISPLAY_HEIGHT + x] = true;
}

void updateDisplay(){
  for(int j = 0; j < DISPLAY_HEIGHT; j++){
    clearOutputs();
    scan(j);   
    writeLine(j);
    delayMicroseconds(SCAN_INTERVAL_US);
  }
}

void clamp(int low, int high, double* value){
  if(*value < low)
    *value = low;
  if(*value > high)
    *value = high;
}

void clampToScreen(double* x, double* y){
  if(*x > DISPLAY_WIDTH)
    *x = 0;
  if(*x < 0)
    *x = DISPLAY_WIDTH - 1;

  if(*y > DISPLAY_HEIGHT)
    *y = 0;
  if(*y < 0)
    *y = DISPLAY_HEIGHT - 1;
}

void update(){
  frames++;
  if(frames == FRAME_INTERVAL){
    patternIdx = (patternIdx + 1) % NUM_FRAMES;
    frames -= FRAME_INTERVAL;
  }
}

void render(){
  clearDisplay();
  drawInt4x4(patterns[patternIdx]);
  updateDisplay();  
}

void loop(){
  previousUS = currentUS;
  currentUS = micros();
  elapsedUS += currentUS - previousUS;
  
  while(elapsedUS - TIMESTEP_US > 0){
    update();
    elapsedUS -= TIMESTEP_US;
  }

  render();
}
