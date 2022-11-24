#include <Adafruit_NeoPixel.h>
#include<Wire.h>
#include <MPU6050_light.h>

#define ul unsigned long 
#define BUTTON_PIN 6
#define SIGNAL_PIN 7
#define BRAKE_PIN 3

// CONSTANTS

const int lightsEdge = 9;
const int lightsMid = 18;
const int lightsTot = 4 * lightsEdge + lightsMid;
const int brakeLightsTot = 3 * 9;

const int MPU_addr=0x68;  // I2C address of the MPU-6050

// Thread Constants
const ul lightTimer = 20;
const ul buttonTimer = 50;
const ul accelTimer = 20;

// LED Object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(lightsTot, SIGNAL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel brake = Adafruit_NeoPixel(brakeLightsTot, BRAKE_PIN, NEO_GRB + NEO_KHZ800);

// VARIABLES

// Thread Variables
ul prevLightTime = 0;
ul prevButtonTime = 0;
ul prevAccelTime = 0;

// Light Variables
int lightState = 0;
int lightCnt = 0;
uint32_t brakeColour = strip.Color(255, 0, 0);
uint32_t turnColour = strip.Color(236, 150, 0);
uint32_t blank = strip.Color(0, 0, 0);

// Button Variables
int buttonInput = 0;
int pressed = 0;
int signal = 1;

//Accelerometer Variables
int brakeState = 0;
MPU6050 mpu(Wire);
int count =0;
float previousHeading = 0;

void setup() {

  // Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // LED Strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(100);

  brake.begin();
  brake.show();
  brake.setBrightness(100);

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

// Helper function to clear the signal LEDs
void signalWipe() {
  for(uint16_t i=0; i<strip.numPixels();i++) {
      strip.setPixelColor(i, blank);
      strip.show();
  }
}

// Helper function to clear the signal LEDs
void brakeWipe() {
  for(uint16_t i=0; i<brake.numPixels();i++) {
      brake.setPixelColor(i, blank);
      brake.show();
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

    strip.setPixelColor(lightsMid - 1 - lightCnt, turnColour); // Sets current light to brakeColour, note middle strip starts at index 0
    strip.show();
    lightCnt++;
    // At the halfway point, we switch states
    if (lightCnt == lightsMid/2){
      lightState = 1;
      lightCnt = 0;
    }
  }


  if (lightState == 1){
    strip.setPixelColor(lightsTot - lightsEdge - 1 - lightCnt, turnColour); 
    strip.setPixelColor(lightsMid + lightsEdge + lightCnt, turnColour);
    strip.setPixelColor(lightsMid/2 - 1 - lightCnt, turnColour);
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

    strip.setPixelColor(lightCnt, turnColour); // Sets current light to turnColour, note middle strip starts at index 0
    strip.show();
    lightCnt++;
    // At the halfway point, we switch states
    if (lightCnt == lightsMid/2){
      lightState = 1;
      lightCnt = 0;
    }
  }


  if (lightState == 1){
    strip.setPixelColor(lightsTot - lightsEdge + lightCnt, turnColour); 
    strip.setPixelColor(lightsMid + lightsEdge - 1 - lightCnt, turnColour);
    strip.setPixelColor(lightsMid/2 + lightCnt, turnColour);
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

// SignalBreak State Animation
void signalBrake(){
  if (brakeState == 1){
    for (int i = 0; i < brakeLightsTot; i++){
      brake.setPixelColor(i, brakeColour);
    }
    brake.show();
  }
}

void readButton(){
  /*buttonInput
  0 is not pressed
  1 is pressed
  */

  buttonInput = digitalRead(BUTTON_PIN) ^ 1;

  if (buttonInput == 1 && pressed == 0){
    signal = (signal + 1) % 3; // reverses signal from 0 to 1 and vice versa

  //if the left or right turn signal is activated, record the heading of the accelerometer
    if (signal==1 || signal==2){
      previousHeading= mpu.getAngleZ(); 
    }
    //Reset variables
    lightState = 0;
    lightCnt = 0;
    signalWipe();

    pressed = 1;
  }

  if (buttonInput == 0){
    pressed = 0;
  }


}

void checkBraking(){
  float pitch = (mpu.getAngleY()*71)/4068; //gets the pitch and converts to radians
  //Serial.print("pitch: ");
  //Serial.print(pitch);

  if(mpu.getAccX()*cos(pitch)+mpu.getAccZ()*sin(pitch)<-0.3){
    // Serial.println("braking");
    brakeState = 1;
    count=0;
  }
  else if(mpu.getAccX()*cos(pitch)+mpu.getAccZ()*sin(pitch)>-0.05){
    count++;
  }

  if (brakeState != 1){
    count = 0;
  }
  if (count>40){
    brakeState = 0;
    count=0;
    brakeWipe();
  }
  
}

void checkTurned(){
  float heading =mpu.getAngleZ();
  //Serial.print("Heading: ");
  //Serial.println(heading);

  //checks if the current heading differs from the recorded heading by more than 45, if so deactivate turn signal
  if (signal==1){
    if (heading - previousHeading>45){
      signal = 0;
      lightState = 0;
      lightCnt = 0;
      signalWipe();      
    }
  }
  else if (signal ==2){
    if (heading -previousHeading<-45){
      signal = 0;
      lightState = 0;
      lightCnt = 0;
      signalWipe();   
    }
  }
}

void runThread(){
    // LED Thread
    ul curLightTime = millis();
    if (curLightTime - prevLightTime > lightTimer){
      prevLightTime = curLightTime;

      //Signal == 0 is the neutral state
      if (signal == 1){
        signalLeft();
      }
      else if (signal == 2){
        signalRight();
      }
      
      // Signal Brake
      if (brakeState == 1){
        signalBrake();
      }
    }

    // Button Thread
    ul curButtonTime = millis();
    if (curButtonTime - prevButtonTime > buttonTimer){
      prevButtonTime = curButtonTime;
      readButton();
    }

    // Accelerometer Thread
    mpu.update(); //update accelerometer values constantly for better accuracy
    //Serial.print("AccelX: ");
    //Serial.println(mpu.getAccX()*cos((mpu.getAngleY()*71)/4068)+mpu.getAccZ()*sin((mpu.getAngleY()*71)/4068));

    ul curAccelTime = millis();
    if (curAccelTime - prevAccelTime > accelTimer){
      prevAccelTime = curAccelTime;
      checkBraking();
      checkTurned();
    }
  // Serial.print("r ");
  // Serial.print(buttonInput);
  // Serial.print(" p ");
  // Serial.print(pressed);
  //Serial.print(" s ");
  //Serial.println(signal);
  
}

void loop() {
  runThread();
  delay(1);
}
