#include <ST7735_t3.h> // Hardware-specific library
#include <ST7789_t3.h> // Hardware-specific library
#include <SPI.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

#include "effect_vocoder.h"

#define TFT_MISO  12
#define TFT_MOSI  11  //a12
#define TFT_SCK   13  //a13
#define TFT_DC   4
#define TFT_CS   5  
#define TFT_RST  3

ST7789_t3 tft = ST7789_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
const int myInput = AUDIO_INPUT_MIC;

AudioInputI2S          audioInput;         // audio shield: mic or line-in
AudioAnalyzeFFT1024    myFFT;
AudioOutputI2S         audioOutput;   // audio shield: headphones & line-out

AudioConnection c1(audioInput, myFFT);
AudioConnection c2(audioInput, 0, audioOutput, 0);
AudioConnection c3(audioInput, 0, audioOutput, 1);
AudioControlSGTL5000 audioShield;

void setup(){
  Serial.begin(9600);
  Serial.print("hello!");
  tft.init(127, 127);
  tft.invertDisplay(false);
  tft.fillScreen(ST7735_WHITE);
  AudioMemory(20);
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(0.5);
  myFFT.windowFunction(AudioWindowHanning1024);
}

void loop(){
  static unsigned long last_reset = 0;
  float n;
  int i;  
  if (myFFT.available()) {
    // each time new FFT data is available
    // print it all to the Arduino Serial Monitor
    Serial.print("FFT: ");
    for (i=0; i<512; i = i + 2) {
        n = myFFT.read(i);
        if (n >= 0.01) {
          Serial.print(n * 500);
          Serial.print(" ");
          //tft.fillRect(127 - (i >> 2), 0, 1, n * 500, ST7735_RED);
        } else {
          Serial.print("  -  "); // don't print "0.00"
          //tft.fillRect(127 - (i >> 2), 0, 1, 128, ST7735_WHITE);
        }
      }
    Serial.println();
  }
}
