#include <FastLED.h>
#include <stdio.h>
#include <ctype.h>

#define DATA_PIN 18

#define BRIGHTNESS 75
#define SEGMENT_LENGTH 18
#define NUM_LEDS 72

#define FRONT_LEFT 0
#define FRONT_RIGHT SEGMENT_LENGTH
#define BACK_RIGHT 2 * SEGMENT_LENGTH
#define BACK_LEFT 3 * SEGMENT_LENGTH

//signals for wave
#define LEFT 0
#define RIGHT 1
#define MS 125



CRGB leds[NUM_LEDS];

int operation;
String new_op;
unsigned long INTERVAL = 5000;
unsigned long CURRENTms;
unsigned long PREVms = 0;

enum MODES
{
  STANDBY = 0,
  MOVE_MANUALLY = 1,
  AUTONOMOUS = 2,
  AUTONOMOUS_RIGHT = 3,
  AUTONOMOUS_LEFT = 4,
  OBSTACLE_SLOW = 5,
  OBSTACLE_STOP = 6,
  OBSTACLE_RIGHT = 7,
  OBSTACLE_LEFT = 8,
  AUTONOMOUS_FAIL = 9,
  ALERT_STOP = 10,
  ALERT_ONE = 11,
  ALERT_TWO = 12,
  ALERT_THREE = 13,
  ALERT_FOUR = 14
};



void setup()
{
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);

  // Wait for serial bus to be open.
  while (!Serial)
     ;

  Serial.println("======================================");
  Serial.println("Please select an LED mode.");
  Serial.println("\t0) Stanby");
  Serial.println("\t1) Move manually");
  Serial.println("\t2) Move autonomously");
  Serial.println("\t3) Turn right autonomously");
  Serial.println("\t4) Turn left autonomously");
  Serial.println("\t5) Obstacle in slowdown area");
  Serial.println("\t6) Obstacle in stopping area");
  Serial.println("\t7) Turn right to avoid obstacle");
  Serial.println("\t8) Turn left to avoid obstacle");
  Serial.println("\t9)Failed autonomous move");
  Serial.println("\t10)Alert stop");
  Serial.println("\t11)Alert reserved 1");
  Serial.println("\t12)Alert reserved 2");
  Serial.println("\t13)Alert reserved 3");
  Serial.println("\t14)Alert reserved 4");
  Serial.println("======================================");
  Serial.println();

  Serial.println("Please select an option ");

}
3
void clear(){

for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }
  FastLED.show();

}

void fillLeds(CRGB color, int numLeds, int startPos)
{
  for (int i = startPos; i < startPos + numLeds; i++)
  {
    leds[i] = color;
  }
  FastLED.show();
}

void blink(CRGB color, int ms, int numLeds, int startPos)
{
  fillLeds(color, numLeds, startPos);
  delay(ms);
  fillLeds(CRGB::Black, numLeds, startPos);
  delay(ms);
}


void wave(CRGB color, int speed, int numLeds, int FstartPos, int BstartPos, int signal)
{

  int Bcounter = BACK_LEFT;
  
  if(signal == 1){
   for(int i = FstartPos; i < FstartPos + numLeds; i++) 
   { 

    leds[i] = color;
    leds[Bcounter] = color;
    FastLED.show();
    Bcounter--;
    delay(speed);
    }
  } 

  if(signal == 0){

    Bcounter = BACK_LEFT;
   for(int i = FstartPos + numLeds; i > FstartPos; i--) 
   {
    leds[i] = color;
    leds[Bcounter] = color;
    FastLED.show();
    Bcounter++;
    delay(speed);
    }
  }

}

void wave_right(CRGB color, int speed, int numLeds, int FstartPos){
  for(int i = FstartPos; i < FstartPos + numLeds; i++) 
   { 
    leds[i] = color;
    FastLED.show();
    delay(speed);
    }
}

void wave_left(CRGB color, int speed, int numLeds, int FstartPos){
  for(int i = FstartPos + numLeds; i > FstartPos; i--) 
   {
    leds[i] = color;
    FastLED.show();
    delay(speed);
    }
}

//takes "new_op" and converts to int
int getOperation(String new_op) {

  return new_op.toInt();
}


void loop()
{
   


  CURRENTms = millis();

  //if no signal has been received within 5s, automatically go to standby mode
  if(CURRENTms - PREVms >= INTERVAL){
    operation = 0;
  }

  //didnt use this because parseInt takes sometime to grab data and check.
  // if(Serial.available()){
  //   operation = Serial.parseInt();
  //   PREVms = CURRENTms;
  //   Serial.print("Mode selected:");
  //   Serial.println(operation);
  // }

  while(Serial.available()) {

    //if there is data available it will read the input until "\n" (enter key) then 
    //it will save the new operation into the variable operation
    char character = Serial.read();

    if(character == '\n') {
    Serial.print("Mode selected: ");
    Serial.println(new_op);

    operation = getOperation(new_op);
    new_op = "";
    
    PREVms = CURRENTms;
    } else {

    new_op.concat(character);
    }
    
    
  }

  switch (operation)
  {

  case (MODES::STANDBY):
    fillLeds(CRGB::White, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::White, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::White, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::White, SEGMENT_LENGTH, BACK_RIGHT);
    break;
  
  case(MODES::MOVE_MANUALLY):
    fillLeds(CRGB::Blue, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Blue, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Blue, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Blue, SEGMENT_LENGTH, BACK_RIGHT);
    break;

  case(MODES::AUTONOMOUS):
    blink(CRGB::Blue, MS, NUM_LEDS, FRONT_LEFT);
   break;

  case (MODES::AUTONOMOUS_RIGHT):
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_RIGHT);
    fillLeds(CRGB::Blue, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Blue, SEGMENT_LENGTH, BACK_LEFT);
    wave(CRGB::Blue, 25, SEGMENT_LENGTH, FRONT_RIGHT, BACK_RIGHT, RIGHT);
    delay(MS);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_RIGHT);
    
    break;

  case (MODES::AUTONOMOUS_LEFT):
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Blue, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Blue, SEGMENT_LENGTH, BACK_RIGHT);
    wave(CRGB::Blue, 25, SEGMENT_LENGTH, FRONT_LEFT, BACK_LEFT, LEFT);
    delay(MS);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_RIGHT);
    
    break;

  case (MODES::OBSTACLE_SLOW):
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, BACK_RIGHT);
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, BACK_LEFT);
    delay(MS);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_RIGHT);
    delay(MS);
    break;

  case (MODES::OBSTACLE_STOP):
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, BACK_RIGHT);
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, BACK_LEFT);
    break;

  case (MODES::OBSTACLE_RIGHT):
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_RIGHT);
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, BACK_LEFT);
    wave(CRGB::Yellow, 25, SEGMENT_LENGTH, FRONT_RIGHT, BACK_RIGHT, RIGHT);
    delay(MS);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_RIGHT);
    delay(MS);
    break;
  

  case (MODES::OBSTACLE_LEFT):
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Yellow, SEGMENT_LENGTH, BACK_RIGHT);
    wave(CRGB::Yellow, 25, SEGMENT_LENGTH, FRONT_LEFT, BACK_LEFT, LEFT);
    delay(MS);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_RIGHT);
    delay(MS);
    break;
  
  case (MODES::AUTONOMOUS_FAIL):
 
    fillLeds(CRGB::Red, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, BACK_RIGHT);
    delay(MS);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_RIGHT);
    delay(MS);
    break;
  
  case (MODES::ALERT_STOP):
    fillLeds(CRGB::Red, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, BACK_RIGHT);
    break;
  
  case (MODES::ALERT_ONE):
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, BACK_RIGHT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, BACK_LEFT);
    wave_left(CRGB::Red, 50, SEGMENT_LENGTH, FRONT_LEFT);
    delay(MS);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_LEFT);
    break;
  
  
  case (MODES::ALERT_TWO):
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, BACK_RIGHT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, BACK_LEFT);
    wave_right(CRGB::Red, 50, SEGMENT_LENGTH, FRONT_RIGHT);
    delay(MS);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, FRONT_RIGHT);
    break;
  
  case (MODES::ALERT_THREE):
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_LEFT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, BACK_RIGHT);
    wave_right(CRGB::Red, 50, SEGMENT_LENGTH, BACK_LEFT);
    delay(MS);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_LEFT);
    break;
  
  case (MODES::ALERT_FOUR):
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_RIGHT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, FRONT_LEFT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, FRONT_RIGHT);
    fillLeds(CRGB::Red, SEGMENT_LENGTH, BACK_LEFT);
    wave_left(CRGB::Red, 50, SEGMENT_LENGTH, BACK_RIGHT);
    delay(MS);
    fillLeds(CRGB::Black, SEGMENT_LENGTH, BACK_RIGHT);
    break;
  
  }
  


}


