#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>

#define PIN 2
#define MATRIX_HEIGHT 8
#define MATRIX_WIDTH 16

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
  MATRIX_WIDTH, MATRIX_HEIGHT,
  PIN,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB + NEO_KHZ800
);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.



//these values are substracetd from the generated values to give a shape to the animation
const unsigned char valueMask[MATRIX_HEIGHT][MATRIX_WIDTH]={
    {32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 },
    {64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 , 64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 },
    {96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 , 96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 },
    {128, 64 , 32 , 0  , 0  , 32 , 64 , 128, 128, 64 , 32 , 0  , 0  , 32 , 64 , 128},
    {160, 96 , 64 , 32 , 32 , 64 , 96 , 160, 160, 96 , 64 , 32 , 32 , 64 , 96 , 160},
    {192, 128, 96 , 64 , 64 , 96 , 128, 192, 192, 128, 96 , 64 , 64 , 96 , 128, 192},
    {255, 160, 128, 96 , 96 , 128, 160, 255, 255, 160, 128, 96 , 96 , 128, 160, 255},
    {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}
};

//these are the hues for the fire, 
//should be between 0 (red) to about 25 (yellow)
const unsigned char hueMask[MATRIX_HEIGHT][MATRIX_WIDTH]={
    {1 , 11, 19, 25, 25, 22, 11, 1 , 1 , 11, 19, 25, 25, 22, 11, 1 },
    {1 , 8 , 13, 19, 25, 19, 8 , 1 , 1 , 8 , 13, 19, 25, 19, 8 , 1 },
    {1 , 8 , 13, 16, 19, 16, 8 , 1 , 1 , 8 , 13, 16, 19, 16, 8 , 1 },
    {1 , 5 , 11, 13, 13, 13, 5 , 1 , 1 , 5 , 11, 13, 13, 13, 5 , 1 },
    {1 , 5 , 11, 11, 11, 11, 5 , 1 , 1 , 5 , 11, 11, 11, 11, 5 , 1 },
    {0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 , 0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 },
    {0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 , 0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 },
    {0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 }
};

unsigned char matrixValue[MATRIX_HEIGHT][MATRIX_WIDTH];
unsigned char line[MATRIX_WIDTH];
int pcnt = 0;

//Converts an HSV color to RGB color
uint16_t HSVtoRGB(uint8_t ih, uint8_t is, uint8_t iv) {
  float r, g, b, h, s, v; //this function works with floats between 0 and 1
  float f, p, q, t;
  int i;

  h = (float)(ih / 256.0);
  s = (float)(is / 256.0);
  v = (float)(iv / 256.0);

  //if saturation is 0, the color is a shade of grey
  if(s == 0.0) {
    b = v;
    g = b;
    r = g;
  }
  //if saturation > 0, more complex calculations are needed
  else
  {
    h *= 6.0; //to bring hue to a number between 0 and 6, better for the calculations
    i = (int)(floor(h)); //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
    f = h - i;//the fractional part of h

    p = (float)(v * (1.0 - s));
    q = (float)(v * (1.0 - (s * f)));
    t = (float)(v * (1.0 - (s * (1.0 - f))));

    switch(i)
    {
      case 0: r=v; g=t; b=p; break;
      case 1: r=q; g=v; b=p; break;
      case 2: r=p; g=v; b=t; break;
      case 3: r=p; g=q; b=v; break;
      case 4: r=t; g=p; b=v; break;
      case 5: r=v; g=p; b=q; break;
      default: r = g = b = 0; break;
    }
  }
  return matrix.Color(r * 255.0, g * 255.0, b * 255.0);
}

/**
 * Randomly generate the next line (matrix row)
 */
void generateLine(){
  for(uint8_t x=0; x<MATRIX_WIDTH; x++) {
    line[x] = random(64, 255);
  }
}

/**
 * shift all values in the matrix up one row
 */
void shiftUp() {
  for (uint8_t y=MATRIX_HEIGHT-1; y>0; y--) {
    for (uint8_t x=0; x<MATRIX_WIDTH; x++) {
      matrixValue[y][x] = matrixValue[y-1][x];
    }
  }
  
  for (uint8_t x=0; x<MATRIX_WIDTH; x++) {
    matrixValue[0][x] = line[x];
  }
}

/**
 * draw a frame, interpolating between 2 "key frames"
 * @param pcnt percentage of interpolation
 */
void drawFrame(int pcnt) {
  int nextv;
  
  //each row interpolates with the one before it
  for (unsigned char y=MATRIX_HEIGHT-1; y>0; y--) {
    for (unsigned char x=0; x<MATRIX_WIDTH; x++) {
      nextv = 
          (((100.0-pcnt)*matrixValue[y][x] 
        + pcnt*matrixValue[y-1][x])/100.0) 
        - valueMask[y][x];
      uint16_t color = HSVtoRGB(
        hueMask[y][x], // H
        255, // S
        (uint8_t)max(0, nextv) // V
      );

      matrix.drawPixel(x, y, color);
    }
  }
  
  //first row interpolates with the "next" line
  for(unsigned char x=0; x<MATRIX_WIDTH; x++) {
    uint16_t color = HSVtoRGB(
      hueMask[0][x], // H
      255,           // S
      (uint8_t)(((100.0-pcnt)*matrixValue[0][x] + pcnt*line[x])/100.0) // V
    );
    matrix.drawPixel(x, 0, color);
  }
}

void setup() {
  matrix.begin();
  randomSeed(analogRead(0));
  generateLine();
  //init all pixels to zero
  memset(matrixValue, 0, sizeof(matrixValue));
}

void loop() {
  if (pcnt >= 100) {
    shiftUp();
    generateLine();
    pcnt = 0;
  }
  drawFrame(pcnt);
  matrix.show();
  pcnt+=30;
}
