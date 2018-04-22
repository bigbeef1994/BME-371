#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to -1!
#define TFT_DC     8

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//Declare Variables
char user_input;
int x;

int dur = 30; //initialize to 60 seconds
int freq = 1; //initialize to 1 Hz

int count;
int ts = 2; //initalize to 2 time steps (SimpleBlink)
int tsdel;

const int analogIntePin = A0;  // Analog input pin that the potentiometer is attached to
const int analogFreqPin = A1;
const int analogDurPin = A2;
const int analogOutPin = 12; // Analog output pin that the LED is attached to
const int buttonPin = 6;
int buttonState = 0;

int isensorValue;        // value read from the pot
int fsensorValue;        // value read from the pot
int dsensorValue;        // value read from the pot
       
int tempdur;
int tempfreq;
int outputValue; 

int previousVals[4] = {0,0,0,0};  // contains parameter values from previous interrupt
int changes[4] = {0,0,0,0};       // indicates if a parameter has been changed since the last interrupt
int hasChanged = 0;               // indicates that a parameter has been changed

void mainMenu();
// the setup function runs once when you press reset or power the board
void setup() {

  Serial.begin(9600); //Open Serial connection for debugging
  //  Serial.println("Welcome!");
  //  Serial.println();
  //  Serial.println("Enter number for control option:");
  //  Serial.println("1. Change Intensity");
  //  Serial.println("2. Change Frequency");
  //  Serial.println("3. Change Duration");
  //  Serial.println("4. Start Treatment");
  //  Serial.println();

  // initialize digital anlogOutPin as an output
  // and buttonPin as input.
  pinMode(analogOutPin , OUTPUT);
  //pinMode(buttonPin,INPUT);
  
  // disable interrupts
  noInterrupts();
 
  fsensorValue = analogRead(analogFreqPin);
  isensorValue = analogRead(analogIntePin);
  dsensorValue = analogRead(analogDurPin);
  

  tempfreq = map(fsensorValue, 0, 1023, 0, 60);
  tempdur = map(dsensorValue, 0, 1023, 0, 120);
  outputValue = map(isensorValue, 0, 1023, 0, 100);
  
  // initialize LCD
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  //tft.setRotation(tft.getRotation()+1);
  tft.setCursor(30,60);
  tft.print("Welcome to the");
  tft.setCursor(20,75);
  tft.print("Photic Stimulator");
  delay(500000);

  mainMenu();
  
  // initialize TIMER0
  // Set TCCR0A and TCCR0B registers to 0
  TCCR1A = 0;                         
  TCCR1B = 0;
  // initialize counter to 0
  TCNT1 = 0;
  // set compare match register for 100 Hz
  OCR1A = 156;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS01 and CS00 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1<< OCIE1A); 



  // allow interrupts
  interrupts();
  
}

void mainMenu(){
  // show menu with all four parameters
  tft.fillScreen(ST7735_BLACK);
  
  // print intensity
  tft.setCursor(5,30);
  tft.print("Intensity");
  tft.setCursor(80,30);
  tft.print(String(outputValue));
  tft.print(" ");
  

  // print frequency
  tft.setCursor(5,70);
  tft.print("Frequency");  
  tft.setCursor(80,70);
  tft.print(String(tempfreq));

  // print duration
  tft.setCursor(5,100);
  tft.print("Duration");
  tft.setCursor(80,100);
  tft.print(String(tempdur));
  tft.print(" ");

  tft.setCursor(5,130);
  tft.print("Start");  
}

// the loop function runs over and over again forever
void loop() {
  
  // constantly update the freq, dur, and val
  fsensorValue = analogRead(analogFreqPin);
  isensorValue = analogRead(analogIntePin);
  dsensorValue = analogRead(analogDurPin);

  tempfreq = map(fsensorValue, 0, 1023, 0, 60);
  tempdur = map(dsensorValue, 0, 1023, 0, 120);
  outputValue = map(isensorValue, 0, 1023, 0, 100);
   
   if(hasChanged = 1){
    
     // if intensity is what has been changed
     if(changes[0] == 1){
      // update intensity on LCD 
      tft.fillRect(80,30,20,10,ST7735_BLACK);
      tft.setCursor(80,30);
      tft.print(previousVals[0]);
      changes[0] = 0;      
     }
     
     // if frequency has been changed
     if(changes[1] == 1){
      // update frequency on LCD
      tft.fillRect(80,70,15,10,ST7735_BLACK);
      tft.setCursor(80,70);
      tft.print(previousVals[1]);
      changes[1] = 0;
     }
     
     // if duration has been changed
     if(changes[2] == 1){
      // update duration on LCD
      tft.fillRect(80,100,20,10,ST7735_BLACK);
      tft.setCursor(80,100);
      tft.print(previousVals[2]);
      changes[2] = 0;
     }
     
     hasChanged = 0;
    
   }
//   doesnt work rn but will eventually uncomment this lol
//   buttonState = digitalRead(buttonPin);
//   // if start button has been pressed deactivate interrupts
//   // and wait for duration to hit zero
//   if(buttonState == HIGH){
//        tft.fillScreen(ST7735_BLACK);
//        tft.setCursor(0,75);
//        tft.print("Treatment has begun");
//        CalculateValues();
//        for(x=1; x<count+1; x++)
//        {
//          SimpleBlink();
//        }
//        tft.fillScreen(ST7735_BLACK);
//        tft.setCursor(0,75);
//        tft.print("Treatment is finished");
//        buttonState = digitalRead(buttonPin);
//        while(buttonState == LOW){buttonState = digitalRead(buttonPin);}
//        mainMenu();
//        delay(500);
//   }
   
    
  
//  while(Serial.available()==0){}
      user_input = Serial.read();
//      if (user_input =='1')
//      { 
//        Serial.println("Changing Intensity");
//        Serial.println("Adjust Dial until desired intensity in % and press key and enter");
//        Serial.println();
//        ChangeIntensity();
//      }
//      else if (user_input == '2')
//      {
//       Serial.println("Changing Frequency");
//        Serial.println("Adjust Dial until desired frequency in hertz and press key and enter");
//        Serial.println();
//        ChangeFrequency();
//
//      }
//      else if (user_input == '3')
//      {
//        Serial.println("Changing Duration");
//        Serial.println("Adjust Dial until desired duration in seconds and press key and enter");
//        Serial.println();
//        ChangeDuration();
//
//      }
//     else if (user_input == '4')
      if (user_input == '4')
      {
        noInterrupts();
        Serial.println("Starting Treatment");
        CalculateValues();
        tft.fillScreen(ST7735_BLACK);
        tft.setCursor(0,75);
        tft.print("Treatment has begun");
        for(x=1; x<count+1; x++)
        {
          SimpleBlink();
        }
        tft.fillScreen(ST7735_BLACK);
        tft.setCursor(0,75);
        tft.print("Treatment is finished");
//        buttonState = digitalRead(buttonPin);
//        while(buttonState == LOW){buttonState = digitalRead(buttonPin);}
        mainMenu();
        delay(500);
        interrupts();
      }
//       Serial.read();
//       Serial.println();
//       Serial.println("Enter number for control option:");
//       Serial.println("1. Change Intensity");
//       Serial.println("2. Change Frequency");
//       Serial.println("3. Change Duration");
//       Serial.println("4. Start Treatment");
//       Serial.println();
      
}

void CalculateValues()
{
  count=dur*freq; //adjust for number of pattern repetitions
  tsdel=(1000/freq)/ts; //set timestep delay 
}
void SimpleBlink()
{
  //analogWrite(analogOutPin, outputValue);
  ts=2; // set time step values
  CalculateValues(); //recalculate delay values
  digitalWrite(12, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(tsdel);                       // wait for delay
  //analogWrite(analogOutPin, 0);
  digitalWrite(12, LOW);    // turn the LED off by making the voltage LOW
  delay(tsdel);                       // wait for delay
  Serial.println("blink");
}

void DarkBlink()
{
  ts=3; // set time step values
  CalculateValues(); // recalculate delay
  digitalWrite(12, HIGH); //turn LED on
  delay(tsdel); //1 time step on
  digitalWrite(12, LOW);
  delay(tsdel);
  delay(tsdel); //2 time steps off
  Serial.println("dark");  
}

void BrightBlink()
{
  ts=3;
  CalculateValues();
  digitalWrite(12, HIGH);
  delay(tsdel);
  delay(tsdel); //2 time steps on
  digitalWrite(12, LOW);
  delay(tsdel); //1 time step off
  Serial.println("bright");
}

// timer0 interrupts at 100 Hz 
ISR(TIMER1_COMPA_vect){
 
  // if the intensity has been changed
  if(previousVals[0] != outputValue){
    // acknowledge that is has been changed
    changes[0] = 1;
    hasChanged = 1;     
    previousVals[0] = outputValue;
  }

  if(previousVals[1] != tempfreq){
    changes[1] = 1;
    hasChanged = 1;  
    freq = tempfreq;
    previousVals[1] = tempfreq;
  }

  if(previousVals[2] != tempdur){
    changes[2] = 1;
    hasChanged = 1;  
    dur = tempdur;
    previousVals[2] = tempdur;
  }

}
//void ChangeIntensity()
//{
//  while(Serial.available()==0)  //Loop the forward step until key strike
//  {
//  // read the analog in value:
//  isensorValue = analogRead(analogIntePin);
//  // map it to the range of the analog out:
//  outputValue = map(isensorValue, 0, 1023, 0, 100);
//
//  Serial.print("Intensity = ");
//  Serial.print(outputValue);
//  Serial.print(" ");
//  Serial.print("\r");
//  }
//
//  Serial.println();
//}
//void ChangeFrequency()
//{
//  while(Serial.available()==0)  //Loop the forward step until key strike
//  {
//  // read the analog in value:
//  fsensorValue = analogRead(analogFreqPin);
//  // map it to the range of the analog out:
//  tempfreq = map(fsensorValue, 0, 1023, 1, 60);
//
//  Serial.print("Frequency = ");
//  Serial.print(tempfreq);
//  Serial.print("\r");
//  }
//  Serial.println();
//  freq = tempfreq;
//  
//}
//void ChangeDuration()
//{
//    while(Serial.available()==0)  //Loop the forward step until key strike
//  {
//  // read the analog in value:
//  dsensorValue = analogRead(analogDurPin);
//  // map it to the range of the analog out:
//  tempdur = map(dsensorValue, 0, 1023, 0, 120);
//
//  Serial.print("Duration = ");
//  Serial.print(tempdur);
//  Serial.print(" ");
//  Serial.print("\r");
//  }
//  Serial.println();
//  dur = tempdur;
//}


