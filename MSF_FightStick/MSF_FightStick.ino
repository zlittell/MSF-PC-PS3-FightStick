/*
    Mechanical Squid Factory presents to you:
    Open Source Fight Stick of Awesomesauce
    Compatible w/ PC & PS3
    
    Developer: Zack "Reaper" Littell
    Email: zlittell@gmail.com
    
    Cheap and awesome were the goals so hopefully that works out
    Uses the Teensy-LC
    
    This is used in a box with a joystick, 8 buttons on top, and a start select on the front.
    Also used in a similar setup but with a "hit box" layout.
    Press start and select together for PS home.
    
    Attempted to save PWM capable pins for LEDs if needed in the future.
    But some were sacrificed for ease of layout
    
    Some code exists out there.  I am hoping to combine code to make a complete solution specifically for fight sticks.
    I also was hoping to reduce confusing code and clutter.
    
    Shout outs:
    Teensy Examples - for being awesome
    Hamaluik.com - for the refresher course on descriptors
    jiGGaK @ SlashDev - For compiling some badass code that allows use of the homekey
*/

//Includes
#include <Bounce.h>

//Declarations
const int milliDebounce = 20;  //Debounce time in milliseconds
const int numButtons = 14;  //Number of all buttons
const int numButtonsOnly = 10; //Number of just buttons
//Joystick Pin Declarations
const int pinUP = 5;  //Up on stick is pin 6
const int pinDN = 6;  //Down on stick is pin 7
const int pinLT = 7;  //Left on stick is pin 8
const int pinRT = 8;  //Right on stick is pin 9
//Top Row of Buttons Pin Declarations
const int pinB1 = 9;  //Button 1 is pin 10
const int pinB2 = 10;  //Button 2 is pin 11
const int pinB3 = 11;  //Button 3 is pin 12
const int pinB4 = 12;  //Button 4 is pin 13
//Bottom Row of Buttons Pin Declarations
const int pinB5 = 14;  //Button 5 is pin 14
const int pinB6 = 15;  //Button 6 is pin 15
const int pinB7 = 16;  //Button 7 is pin 16
const int pinB8 = 17;  //Button 8 is pin 17
//Front Panel Pin Declarations
const int pinST = 18;  //Start Button is pin 18 Button 12
const int pinSL = 19;  //Select Button is pin 19
//Button Position in Array
const int posUP = 0;
const int posDN = 1;
const int posLT = 2;
const int posRT = 3;
const int posB1 = 4;
const int posB2 = 5;
const int posB3 = 6;
const int posB4 = 7;
const int posB5 = 8;
const int posB6 = 9;
const int posB7 = 10;
const int posB8 = 11;
const int posST = 12;
const int posSL = 13;
//Array holds locations of button status to convert to correct order working with MKX on PC
//Uses a mix of PS3 and XBOX360 Button mapping lol.  Don't ask why.
//1)Triangle, 2)Circle, 3)Cross, 4)Square, 5)L1, 6)R1, 7)L2, 8)R2, 9)Select, 10)Start
//Our current order has buttons wired as 1234, 5678
//We need that to match Square Triangle R1 L1, Cross Circle R2 L2
//4 1 6 5
//3 2 8 7
//10 9
const int correctOrder[numButtonsOnly] = {4,1,6,5,3,2,8,7,10,9};

//Global Variables
byte buttonStatus[numButtons];
int stickPosition = 15;
uint8_t usbData[3] = {0,0,0};
//Setup Button Debouncing
Bounce joystickUP = Bounce(pinUP, milliDebounce);
Bounce joystickDOWN = Bounce(pinDN, milliDebounce);
Bounce joystickLEFT = Bounce(pinLT, milliDebounce);
Bounce joystickRIGHT = Bounce(pinRT, milliDebounce);
Bounce button1 = Bounce(pinB1, milliDebounce);
Bounce button2 = Bounce(pinB2, milliDebounce);
Bounce button3 = Bounce(pinB3, milliDebounce);
Bounce button4 = Bounce(pinB4, milliDebounce);
Bounce button5 = Bounce(pinB5, milliDebounce);
Bounce button6 = Bounce(pinB6, milliDebounce);
Bounce button7 = Bounce(pinB7, milliDebounce);
Bounce button8 = Bounce(pinB8, milliDebounce);
Bounce buttonSTART = Bounce(pinST, milliDebounce);
Bounce buttonSELECT = Bounce(pinSL, milliDebounce);

//void Configure Inputs and Outputs
void setupPins()
{
    //Configure the direction of the pins
    //All inputs with internal pullups enabled
    pinMode(pinUP, INPUT_PULLUP);
    pinMode(pinDN, INPUT_PULLUP);
    pinMode(pinLT, INPUT_PULLUP);
    pinMode(pinRT, INPUT_PULLUP);
    pinMode(pinB1, INPUT_PULLUP);
    pinMode(pinB2, INPUT_PULLUP);
    pinMode(pinB3, INPUT_PULLUP);
    pinMode(pinB4, INPUT_PULLUP);
    pinMode(pinB5, INPUT_PULLUP);
    pinMode(pinB6, INPUT_PULLUP);
    pinMode(pinB7, INPUT_PULLUP);
    pinMode(pinB8, INPUT_PULLUP);
    pinMode(pinST, INPUT_PULLUP);
    pinMode(pinSL, INPUT_PULLUP);  
}

//Update Buttons
void buttonUpdate()
{
  if (joystickUP.update()) {buttonStatus[posUP] = joystickUP.fallingEdge();}
  if (joystickDOWN.update()) {buttonStatus[posDN] = joystickDOWN.fallingEdge();}
  if (joystickLEFT.update()) {buttonStatus[posLT] = joystickLEFT.fallingEdge();}
  if (joystickRIGHT.update()) {buttonStatus[posRT] = joystickRIGHT.fallingEdge();}
  if (button1.update()) {buttonStatus[posB1] = button1.fallingEdge();}
  if (button2.update()) {buttonStatus[posB2] = button2.fallingEdge();}
  if (button3.update()) {buttonStatus[posB3] = button3.fallingEdge();}
  if (button4.update()) {buttonStatus[posB4] = button4.fallingEdge();}
  if (button5.update()) {buttonStatus[posB5] = button5.fallingEdge();}
  if (button6.update()) {buttonStatus[posB6] = button6.fallingEdge();}
  if (button7.update()) {buttonStatus[posB7] = button7.fallingEdge();}
  if (button8.update()) {buttonStatus[posB8] = button8.fallingEdge();}
  if (buttonSTART.update()) {buttonStatus[posST] = buttonSTART.fallingEdge();}
  if (buttonSELECT.update()) {buttonStatus[posSL] = buttonSELECT.fallingEdge();}
}

//Process all the inputs and load them into the correct bytes to be loaded to usb
//Byte1 = 8 buttons, Byte2 5 buttons and 3 fillers, Byte3 DPAD and 4 fillers
//(Byte1) B8,B7,B6,B5,B4,B3,B2,B1
//(Byte2) x,x,x,B13,B12,B11,B10,B9
//(Byte3) x,x,x,x,D,D,D,D
void processInputs()
{
  //Clear USB Data Holder Bytes
  usbData[0]=0x00;
  usbData[1]=0x00;
  usbData[2]=0x00;
  
  //Set up a variable to hold the correct button values
  int rearrangedButtons[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  
  //Move button statuses into the correct order
  for (int i=0; i<numButtonsOnly; i++)
  {
    //look at button number from index i of correctorder array
    //then set that button in the array for rearranged buttons
    //to its value from button status
    //sorry this is so crazy
    int tempIndex = correctOrder[i] - 1;
    rearrangedButtons[tempIndex] = buttonStatus[i+4];
  }
  
  //Oh bitmasking here we go
  //First byte (B8-B1)
  for (int i=0; i<8; i++)
  {
    rearrangedButtons[i] <<= i;
    usbData[0] |= rearrangedButtons[i];
  }
  
  //Second byte
  for (int i=0; i<8; i++)
  {
    rearrangedButtons[i+8] <<= i;
    usbData[1] |= rearrangedButtons[i+8];
  }
  
  
  //Process the angle of the joystick
  //Process the primary directions first (up and down)
  //Nest processing secondaries
  //Then process secondaries individually
  //Variable to hold Position of Hat
  int hatPosition;
  //Up + Right = 1
  if (buttonStatus[posUP] & buttonStatus[posRT]) {hatPosition = 1;}
  //Up + Left = 7
  else if (buttonStatus[posUP] & buttonStatus[posLT]) {hatPosition = 7;}
  //Up = 0
  else if (buttonStatus[posUP]) {hatPosition = 0;}
  //Down + Right = 3
  else if (buttonStatus[posDN] & buttonStatus[posRT]) {hatPosition = 3;}
  //Down + Left = 5
  else if (buttonStatus[posDN] & buttonStatus[posLT]) {hatPosition = 5;}
  //Down = 4
  else if (buttonStatus[posDN]) {hatPosition = 4;}
  //Left = 6
  else if (buttonStatus[posLT]) {hatPosition = 6;}
  //Right = 2
  else if (buttonStatus[posRT]) {hatPosition = 2;}
  //nothing
  else {hatPosition = 15;}
  
  //Man I hope they can just be equals
  usbData[2] = hatPosition;
}

//Setup
void setup() 
{
    setupPins();
}

void loop() 
{
  //Poll Buttons
  buttonUpdate();
  
  //Process all inputs and load up the usbData registers correctly
  processInputs();
  
  //Update dat joystick SONNNNNN
  FightStick.send(usbData);
  
  //Delays are lame
}
