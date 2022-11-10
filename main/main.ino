#include <Adafruit_NeoPixel.h>
#include<Wire.h>
#include <MPU6050_light.h>

#define ul unsigned long 
#define BUTTON_PIN 6
#define LED_PIN 7

// CONSTANTS

const int lightsEdge = 9;
const int lightsMid = 18;
const int lightsTot = 4 * lightsEdge + lightsMid;
const int MPU_addr=0x68;  // I2C address of the MPU-6050

// Thread Constants
const ul lightTimer = 10;
const ul buttonTimer = 50;
const ul accelTimer = 100;

// LED Object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(lightsTot, LED_PIN, NEO_GRB + NEO_KHZ800);


// VARIABLES

// Thread Variables
ul prevLightTime = 0;
ul prevButtonTime = 0;
ul prevAccelTime = 0;

// Light Variables
int lightState = 0;
int lightCnt = 0;
uint32_t brakeColour = strip.Color(255, 0, 0);
uint32_t blank = strip.Color(0, 0, 0);

// Button Variables
int buttonInput = 0;
int pressed = 0;
int signal = 1;

//Accelerometer Variables
MPU6050 mpu(Wire);

void setup() {

  // Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);



  // LED Strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(100);

  //Accelerometer
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) { } // stop everything if could not connect to MPU6050
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");

  Serial.begin(9600);
}

// Helper function to clear the LEDs
void colorWipe() {
  for(uint16_t i=0; i<strip.numPixels();i++) {
      strip.setPixelColor(i, blank);
      strip.show();
  }
}

// SignalLeft State Animation
void signalLeft(){
  /*
  lightState: 
  0 - turning on first half
  1 - turning on arrow half
  2 - turning off first half
  3 - turning off arrow half
  */

  if (lightState == 0){
    // Serial.print("State 0 LightCnt ");
    // Serial.println(lightCnt);

    strip.setPixelColor(lightsMid - 1 - lightCnt, brakeColour); // Sets current light to brakeColour, note middle strip starts at index 0
    strip.show();
    lightCnt++;
    // At the halfway point, we switch states
    if (lightCnt == lightsMid/2){
      lightState = 1;
      lightCnt = 0;
    }
  }


  if (lightState == 1){
    strip.setPixelColor(lightsTot - lightsEdge - 1 - lightCnt, brakeColour); 
    strip.setPixelColor(lightsMid + lightsEdge + lightCnt, brakeColour);
    strip.setPixelColor(lightsMid/2 - 1 - lightCnt, brakeColour);
    strip.show();
    lightCnt++;

    // After displaying the remaining lights, switch states
    if (lightCnt == lightsMid/2){
      lightState = 2;
      lightCnt = 0;
    }
  }
  
  if (lightState == 2){
    strip.setPixelColor(lightsMid - 1 - lightCnt, blank); // Sets current light to blank, note middle strip starts at index 0
    strip.show();

    lightCnt++;

    // At the halfway point, we switch states
    if (lightCnt == lightsMid/2){
      lightState = 3;
      lightCnt = 0;
    }
  }

  if (lightState == 3){
    strip.setPixelColor(lightsTot - lightsEdge - 1 - lightCnt, blank); 
    strip.setPixelColor(lightsMid + lightsEdge + lightCnt, blank);
    strip.setPixelColor(lightsMid/2 - 1 - lightCnt, blank);
    strip.show();
    lightCnt++;

    // After displaying the remaining lights, switch states
    if (lightCnt == lightsMid/2){
      lightState = 0;
      lightCnt = 0;
    }
  }

}

// SignalRight State Animation
void signalRight(){
  /*
  lightState: 
  0 - turning on first half
  1 - turning on arrow half
  2 - turning off first half
  3 - turning off arrow half
  */

  if (lightState == 0){
    // Serial.print("State 0 LightCnt ");
    // Serial.println(lightCnt);

    strip.setPixelColor(lightCnt, brakeColour); // Sets current light to brakeColour, note middle strip starts at index 0
    strip.show();
    lightCnt++;
    // At the halfway point, we switch states
    if (lightCnt == lightsMid/2){
      lightState = 1;
      lightCnt = 0;
    }
  }


  if (lightState == 1){
    strip.setPixelColor(lightsTot - lightsEdge + lightCnt, brakeColour); 
    strip.setPixelColor(lightsMid + lightsEdge - 1 - lightCnt, brakeColour);
    strip.setPixelColor(lightsMid/2 + lightCnt, brakeColour);
    strip.show();
    lightCnt++;

    // After displaying the remaining lights, switch states
    if (lightCnt == lightsMid/2){
      lightState = 2;
      lightCnt = 0;
    }
  }
  
  if (lightState == 2){
    strip.setPixelColor(lightCnt, blank); // Sets current light to blank, note middle strip starts at index 0
    strip.show();

    lightCnt++;

    // At the halfway point, we switch states
    if (lightCnt == lightsMid/2){
      lightState = 3;
      lightCnt = 0;
    }
  }

  if (lightState == 3){
    strip.setPixelColor(lightsTot - lightsEdge + lightCnt, blank); 
    strip.setPixelColor(lightsMid + lightsEdge - 1 - lightCnt, blank);
    strip.setPixelColor(lightsMid/2 + lightCnt, blank);
    strip.show();
    lightCnt++;

    // After displaying the remaining lights, switch states
    if (lightCnt == lightsMid/2){
      lightState = 0;
      lightCnt = 0;
    }
  }

}

void readButton(){
  /*buttonInput
  0 is not pressed
  1 is pressed
  */

  buttonInput = digitalRead(BUTTON_PIN) ^ 1;

  if (buttonInput == 1 && pressed == 0){
    signal = signal ^ 1; // reverses signal from 0 to 1 and vice versa

    //Reset variables
    lightState = 0;
    lightCnt = 0;
    colorWipe();

    pressed = 1;
  }

  if (buttonInput == 0){
    pressed = 0;
  }


}

void checkBraking(){
  float pitch = (mpu.getAngleY()*71)/4068; //gets the pitch and converts to radians
  if((mpu.getAccX()+sin(pitch))/cos(pitch)<-0.5){
    signal = signal ^ 1;
    lightState = 0;
    lightCnt = 0;
    colorWipe();
  }
  
}

void runThread(){
    // LED Thread
    ul curLightTime = millis();
    if (curLightTime - prevLightTime > lightTimer){
      prevLightTime = curLightTime;
      // Later on handle signals
      if (signal) signalLeft();
      else signalRight();
    }

    // Button Thread
    ul curButtonTime = millis();
    if (curButtonTime - prevButtonTime > buttonTimer){
      prevButtonTime = curButtonTime;
      readButton();
    }

    // Accelerometer Thread
    mpu.update(); //update accelerometer values constantly for better accuracy
    ul curAccelTime = millis();
    if (curAccelTime - prevAccelTime > accelTimer){
      prevAccelTime = curAccelTime;
      checkBraking();
    }
  // Serial.print("r ");
  // Serial.print(buttonInput);
  // Serial.print(" p ");
  // Serial.print(pressed);
  // Serial.print(" s ");
  // Serial.println(signal);
}

void loop() {
  runThread();
  delay(1);
}
