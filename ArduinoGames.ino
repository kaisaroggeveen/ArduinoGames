#include "FastLED.h"
#define NUM_LEDS 4
#define LEDS_PER_STRIP 1

//button and led pins
#define LED_1_DATA 3
#define LED_2_DATA 6
#define LED_3_DATA 9
#define LED_4_DATA 10

unsigned int BUTTON_PIN[NUM_LEDS] = {2, 4, 7, 8};

enum State {MatchColours, Led1Green, HoldDownAllButtons, Level2Passed, Simon};
State currentState = MatchColours;

// Define the array of leds
CRGB leds[NUM_LEDS][LEDS_PER_STRIP];

unsigned int redValue[NUM_LEDS] = {0, 0, 0, 0};
unsigned int greenValue[NUM_LEDS] = {0, 0, 0, 0};
unsigned int blueValue[NUM_LEDS] = {0, 0, 0, 0};

unsigned int buttonState[NUM_LEDS] = {0, 0, 0, 0};
unsigned int lastButtonState[NUM_LEDS] = {0, 0, 0, 0};
unsigned int buttonPressCount[NUM_LEDS] = {0, 0, 0, 0};

unsigned long lastDebounceTime[NUM_LEDS] = {0, 0, 0, 0};
#define DEBOUNCE_DELAY 15

long int simonSequence[5];
void populateSimonSequence()
{
  for(int i = 0; i < 5; i++)
  {
    simonSequence[i] = random(0, 4);
  }
}

void flashLedAsItsColour(int index)
{
   turnOffAllLeds();
   switch(index)
   {
    case 0:
      blueValue[0] = 64;
      break;
    case 1:
      redValue[1] = 64; 
      break;
    case 2:
      redValue[2] = 64; greenValue[2] = 64;
      break;
    case 3:
      greenValue[3] = 64;
      break;
   }
   leds[index][0].setRGB(redValue[index], greenValue[index], blueValue[index]);
   FastLED.show();
   delay(10);
}

void initializeLed(unsigned int index)
{
  redValue[index] = 0; greenValue[index] = 0; blueValue[index] = 0;
  leds[index][0].setRGB(redValue[index], greenValue[index], blueValue[index]);
  FastLED.show();
  delay(10);
}

void setup() {
  FastLED.addLeds<WS2812B, LED_1_DATA, RGB>(leds[0], 1);
  FastLED.addLeds<WS2812B, LED_2_DATA, RGB>(leds[1], 1);
  FastLED.addLeds<WS2812B, LED_3_DATA, RGB>(leds[2], 1);
  FastLED.addLeds<WS2812B, LED_4_DATA, RGB>(leds[3], 1);
  
  pinMode(BUTTON_PIN[0], INPUT);
  pinMode(BUTTON_PIN[1], INPUT);
  pinMode(BUTTON_PIN[2], INPUT);
  pinMode(BUTTON_PIN[3], INPUT);
  randomSeed(analogRead(0));;

  initializeLed(0);
  initializeLed(1);
  initializeLed(2);
  initializeLed(3);
  noTone(5);
}

bool wasButtonPushed(unsigned int index)
{
    // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON_PIN[index]);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState[index]) {
    // reset the debouncing timer
    lastDebounceTime[index] = millis();
  }

  if ((millis() - lastDebounceTime[index]) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState[index]) {
      buttonState[index] = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState[index] == HIGH) {
        return true;
      }
    }
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState[index] = reading;
  return false;
}

bool isButtonHeld(unsigned int index)
{
  int reading = digitalRead(BUTTON_PIN[index]);
  if(reading == HIGH)
  {
    delay(50);
    return true;
  }
  delay(50);
  return false;
}

void turnOffAllLeds()
{
   redValue[0] = 0; redValue[1] = 0; redValue[2] = 0; redValue[3] = 0;
   greenValue[0] = 0; greenValue[1] = 0; greenValue[2] = 0; greenValue[3] = 0;
   blueValue[0] = 0; blueValue[1] = 0; blueValue[2] = 0; blueValue[3] = 0;
   leds[0][0].setRGB(redValue[0], greenValue[0], blueValue[0]);
   leds[1][0].setRGB(redValue[1], greenValue[1], blueValue[1]);
   leds[2][0].setRGB(redValue[2], greenValue[2], blueValue[2]);
   leds[3][0].setRGB(redValue[3], greenValue[3], blueValue[3]);
   FastLED.show();
   delay(1);
 }

void cycleThroughColoursOnButtonPressed(unsigned int index)
{
  bool buttonPressedVariable = wasButtonPushed(index);
  if (buttonPressedVariable && buttonPressCount[index] == 0) 
  {
    redValue[index] = 0; greenValue[index] = 0; blueValue[index] = 64; 
    leds[index][0].setRGB(redValue[index], greenValue[index], blueValue[index]);
    FastLED.show();
    delay(1);
    // blue
    
    tone(5, 200, 100);
    buttonPressCount[index] = 1;
  } 
  else if (buttonPressedVariable && buttonPressCount[index] == 1) 
  {
    redValue[index] = 0; greenValue[index] = 64; blueValue[index] = 0;
    leds[index][0].setRGB(redValue[index], greenValue[index], blueValue[index]);
    FastLED.show();
    delay(10);
    // green
    tone(5, 300, 100);
    buttonPressCount[index] = 2;
  }
  else if (buttonPressedVariable && buttonPressCount[index] == 2)
  {
    redValue[index] = 64; greenValue[index] = 0; blueValue[index] = 0;
    leds[index][0].setRGB(redValue[index], greenValue[index], blueValue[index]);
    FastLED.show();
    delay(10);
    // red
    tone(5, 350, 100);
    buttonPressCount[index] = 3;
  }
  else if (buttonPressedVariable && buttonPressCount[index] == 3)
  {
    redValue[index] = 64; greenValue[index] =  64; blueValue[index] = 0;
    leds[index][0].setRGB(redValue[index], greenValue[index], blueValue[index]);
    FastLED.show();
    delay(10);
    // yellow
    tone(5, 250, 100);
    buttonPressCount[index] = 4;
  }
  else if (buttonPressedVariable && buttonPressCount[index] == 4)
  {
    redValue[index] = 0; greenValue[index] = 0; blueValue[index] = 0;
    leds[index][0].setRGB(redValue[index], greenValue[index], blueValue[index]);
    FastLED.show();
    delay(10);
    // off
    noTone(5);
    buttonPressCount[index] = 0;
  }

}

void whiteOnButtonHeld(unsigned int index)
{
      if(isButtonHeld(index))
      {
        redValue[index] = 64; greenValue[index] = 64; blueValue[index] = 64;
        leds[index][0].setRGB(redValue[index], greenValue[index], blueValue[index]);
        FastLED.show();
        delay(10);
      }
      else
      { 
        redValue[index] = 0; greenValue[index] = 0; blueValue[index] = 0;
        leds[index][0].setRGB(redValue[index], greenValue[index], blueValue[index]);
        FastLED.show();
        delay(10);
      }
}

bool checkLedsMatchButtonColours()
{
  if(redValue[0] != 0 || greenValue[0] != 0 || blueValue[0] != 64)
  {
    return false;
  }
  if(redValue[1] != 64 || greenValue[1] != 0 || blueValue[1] != 0)
  {
    return false;
  }
  if(redValue[2] != 64 || greenValue[2] != 64 || blueValue[2] != 0)
  {
   return false;  
  }
  if(redValue[3] != 0 || greenValue[3] != 64 || blueValue[3] != 0)
  {
    return false;
  }
  return true;
}
  
void loop() 
{

  switch(currentState)
  {
    case MatchColours:
     cycleThroughColoursOnButtonPressed(0);
     cycleThroughColoursOnButtonPressed(1);
     cycleThroughColoursOnButtonPressed(2);
     cycleThroughColoursOnButtonPressed(3);

     if(checkLedsMatchButtonColours())
     {
      currentState = Led1Green;
      tone(5, 300, 250);
      delay(250);
      tone(5, 350, 250);
      delay(250);
      tone(5, 370, 250);
      delay(250);
     }
     break;

    case Led1Green:   
      redValue[0] = 0; redValue[1] = 64; redValue[2] = 64; redValue[3] = 64;
      greenValue[0] = 64; greenValue[1] = 0; greenValue[2] = 0; greenValue[3] = 0;
      blueValue[0] = 0; blueValue[1] = 0; blueValue[2] = 0; blueValue[3] = 0;
      leds[0][0].setRGB(redValue[0], greenValue[0], blueValue[0]);
      leds[1][0].setRGB(redValue[1], greenValue[1], blueValue[1]);
      leds[2][0].setRGB(redValue[2], greenValue[2], blueValue[2]);
      leds[3][0].setRGB(redValue[3], greenValue[3], blueValue[3]);
      FastLED.show();
      delay(3000);
      turnOffAllLeds();
      currentState = HoldDownAllButtons;
      break;

    case HoldDownAllButtons:
      whiteOnButtonHeld(0);
      whiteOnButtonHeld(1);      
      whiteOnButtonHeld(2);      
      whiteOnButtonHeld(3);      
      
      if(isButtonHeld(0) && isButtonHeld(1) && isButtonHeld(2) && isButtonHeld(3) == true)
        {
        currentState = Level2Passed;
        tone(5, 300, 250);
        delay(250);
        tone(5, 350, 250);
        delay(250);
        tone(5, 370, 250);
        delay(250);
        }
        break;

      case Level2Passed:
        redValue[0] = 0; redValue[1] = 0; redValue[2] = 64; redValue[3] = 64;
        greenValue[0] = 64; greenValue[1] = 64; greenValue[2] = 0; greenValue[3] = 0;
        blueValue[0] = 0; blueValue[1] = 0; blueValue[2] = 0; blueValue[3] = 0;
        leds[0][0].setRGB(redValue[0], greenValue[0], blueValue[0]);
        leds[1][0].setRGB(redValue[1], greenValue[1], blueValue[1]);
        leds[2][0].setRGB(redValue[2], greenValue[2], blueValue[2]);
        leds[3][0].setRGB(redValue[3], greenValue[3], blueValue[3]);
        FastLED.show();
        delay(3000);
        turnOffAllLeds();
        currentState = Simon;
        break;

      case Simon:
        populateSimonSequence();
        for(int i = 0; i < 5; i++)
        {
          flashLedAsItsColour(simonSequence[i]);
          delay(1000);
        }
      break;
  }
}
