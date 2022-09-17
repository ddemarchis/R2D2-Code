//-----------------------------------------------------------------------------------------------
// Bad Motivator Expander Code
// Chris James
// 7-12-2015
// Modified by David DeMarchis
// Works in conjuction with Stealth RC Firmware 1.0.10
// Assumes original ordering of sound banks. Plus new "misc" sound bank for sparks sound
// 10-3-2018 - Added print statements for which ports are being used
// Bad Motivator Code
// Includes all Pie Panel Dome code
// Subroutines for Cantina Dance called by SE#10
// Added routine to do a double wave with a single randomly chosen Pie Panel
// Addded necessary subroutine called by SE#10 for HelloAll open/close ... incudes 4 separate routines for attaching, detaching, opening, and closing Pie Panels  11/7/2020
// Adding relay conrol for Scanner LED's - April 2021
// Now includs LFS lifter and rotation (aborted the Arduino Uno)
// Modified Setup code so that Life Form Scanner servo is reset to "starting position" each time droid is powered on
//--------------------------------------------------------------

#include <VarSpeedServo.h> 
#include <Wire.h>
#include <Stepper.h>

//////////////////////////////////
// New outgoing i2c Commands
//////////////////////////////////
String Packet;
//int count = 0;
byte sum;
#define DESTI2C 0
#define MYI2C 20
//////////////////////////////////

// Arduino Pins 
// The first four below are just for the Bad Motivator smoke routine programmed by Chris James
#define PIE1 2 
#define PIE2 3
#define PIE3 4
#define PIE4 5
// End Bad Motivator Pins

const int LFS_LED=6;
#define Scanner1_SERVO_PIN  7  //for scanner rotation

const int enPin = 8;  //for scanner elevator
const int stepPin = 9;  //for scanner elevator
const int dirPin = 10;  //for scanner elevator

//#define EXT_BUTTON 13 
#define BAD_MOTIVATOR 11
#define BAD_MOTIVATOR_LED 12
#define STATUS_LED 13

//the following refer to the constants I use for other Pie Panel animations
#define PP1_SERVO_PIN 2   //  PP1   Pie Panel         
#define PP2_SERVO_PIN 3   //  PP2   Pie Panel         
#define PP5_SERVO_PIN 4   //  PP5   Pie Panel         
#define PP6_SERVO_PIN 5   //  PP6   Pie Panel    

//Not Pins.....internal numbering only
#define PP1 0    //  PP1   Pie Panel         
#define PP2 1    //  PP2   Pie Panel         
#define PP5 2    //  PP5   Pie Panel        
#define PP6 3    //  PP6   Pie Panel 
#define Scanner1 6  // Scanner Servo   
      

#define NBR_SERVOS 12 
#define FIRST_SERVO_PIN 2 
VarSpeedServo Servos[NBR_SERVOS]; 
#define NEUTRALSERVO 750


int randompie;


#define SPEED 200            
#define FASTSPEED 255       
#define OPENSPEED 230   
#define CLOSESPEED 230

Stepper myStepper(200, 7, 8, 9, 10);  //Don't think this is actually needed

int stepCount = 0;  // number of steps the motor has taken  (NOT NEEDED?)


boolean PiesOpen=true;
boolean PiePanel1Open=false;


int i2cCommand = 0;

// External Momentary Switch/Button
unsigned long loopTime;
unsigned long nextCheck = 0;
int buttonVal = 0;

//----------------------------------------------------------------------------
// New outgoing i2c Commands
//----------------------------------------------------------------------------
void sendI2Ccmd(String cmd) {
  
  sum=0;
  Wire.beginTransmission(DESTI2C);
  for (int i=0;i<cmd.length();i++) {

    Wire.write(cmd[i]);
    sum+=byte(cmd[i]);
  }
  Wire.write(sum);
  Wire.endTransmission();  
}

//-----------------------------------------------------------
void setup() {

  // Bad motivator circuit Off
  pinMode(BAD_MOTIVATOR, OUTPUT);
  digitalWrite(BAD_MOTIVATOR, LOW);
  
  pinMode(BAD_MOTIVATOR_LED, OUTPUT);
  digitalWrite(BAD_MOTIVATOR_LED, LOW);  

  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  
 //pinMode(EXT_BUTTON, INPUT);
 //digitalWrite(EXT_BUTTON, HIGH);

  pinMode(LFS_LED, OUTPUT);

  Serial.begin(57600); // DEBUG
  Serial.println("Bad Motivator Stealth Expander - 10.3.18");

  Serial.print("My i2c address: ");
  Serial.println(MYI2C);
  Wire.begin(MYI2C);                // Start I2C Bus as a Slave (Device Number 10)
  Wire.onReceive(receiveEvent); // register i2c receive event

  // Close all pie servos  
  Serial.print("Activating Servos"); 
  for( int i =0; i <  NBR_SERVOS; i++) {
    Serial.print(".");
    Servos[i].attach(FIRST_SERVO_PIN +i);
    Serial.print(".");
    if (i==5) {
        Servos[i].write(1021,25);
    }
    else 
    {
    Servos[i].write(NEUTRALSERVO,120); 
    }
  }
  delay(1000);
  for( int i =0; i <  NBR_SERVOS; i++) {
    Serial.print(".");
    Servos[i].detach();
  }
  Serial.println("");  

  Serial.print("Bad Motivator Pin:");
  Serial.println(BAD_MOTIVATOR_LED);
  Serial.print("Flashing LED(s) Pin:");
  Serial.println(BAD_MOTIVATOR);
  Serial.print("External Trigger Pin:");
  //Serial.println(EXT_BUTTON);
  Serial.println("To test short trigger (S) pin to ground/G.");
  Serial.println("");  
  Serial.println("Setup done. Waiting for i2c command or trigger pin");
  
}

//------------------------------------------------------
// receive Event

void receiveEvent(int howMany) {
  i2cCommand = Wire.read();    // receive i2c command
  Serial.print("Command Code:");
  Serial.println(i2cCommand);
}

///////////////////////////////
// Smoke Machine///////////////
//////////////////////////////

void bad_motivator() {

  unsigned long tmp; // time
  
  digitalWrite(STATUS_LED, HIGH);

  Serial.println("Bad Motivator!!!");
  
  Serial.println("Play Short MP3");

  // Temp max volume and temp stop random sounds on main controller
  sendI2Ccmd("tmpvol=100,15");
  delay(100);
  sendI2Ccmd("tmprnd=60");
  delay(100);

  // Short Circuit MP3 - play sound bank 8
  sendI2Ccmd("$08");

  delay(500);
  
  //Allow smoke to build up in dome
  Serial.println("Smoke ON");
  digitalWrite(BAD_MOTIVATOR, HIGH);
  delay(3000);
  
  //Open pie panels
  Serial.println("Open Pie");
  for (int i=0; i<=3;i++) {
    Servos[i].attach(PIE1+i);
  }
  Servos[0].write(NEUTRALSERVO+1450,100,true);
  Servos[1].write(NEUTRALSERVO+750,100,true);
  Servos[2].write(NEUTRALSERVO+750,100,true);
   Servos[3].write(NEUTRALSERVO+750,100,true);


  
  delay(1000);

  // Electrical Crackle MP3 -  sound bank 10
  sendI2Ccmd("$10");

  Serial.println("Flash Smoke LEDs");
  tmp=millis();
  while (millis()<tmp+3000) {
     int rand=10+(10*random(3,12));
     digitalWrite(BAD_MOTIVATOR_LED, HIGH);
     delay(rand);
      digitalWrite(BAD_MOTIVATOR_LED, LOW);
      rand=rand=10+(10*random(2,6));
     delay(rand);
  }

  Serial.println("Smoke OFF");
  digitalWrite(BAD_MOTIVATOR, LOW);

  // BAD_MOTIVATOR LEDS OFF
  Serial.println("Smoke LEDs OFF");
  digitalWrite(BAD_MOTIVATOR_LED, LOW);

  for (int i=0; i<=3;i++) {
    Servos[i].detach();
  }   

  //Fake being dead
  delay(8000);
    
  Serial.println("Okay we're back, Play MP3");
  sendI2Ccmd("$0109");
  delay(500);

  Serial.println("Close Pies");   
  for (int i=0; i<=3;i++) {
    Servos[i].attach(PIE1+i);
    Servos[i].write(NEUTRALSERVO,25,true);
  }
  delay(1000);
  for (int i=0; i<=3;i++) {
    Servos[i].detach();
  }    

  i2cCommand=-1;
  digitalWrite(STATUS_LED, LOW);
}

//-----------------------------------------------------------
//END SMOKE MACHINE
//-----------------------------------------------------------

//////////////////////////////////////////////////////////////////////
//////Wave Random Pie////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void WaveRandomPie(){

digitalWrite(STATUS_LED, HIGH);

int i=random(0,4);

Servos[i].attach(PIE1+i);
Servos[i].write(NEUTRALSERVO+1050,255,true);
Servos[i].write(NEUTRALSERVO,255,true);  
Servos[i].write(NEUTRALSERVO+1050,255,true);
Servos[i].write(NEUTRALSERVO,255,true);
delay(500);
Servos[i].detach();
i2cCommand=-1;
  digitalWrite(STATUS_LED, LOW);

}


/////////////////////////////////////////////////////////////////////////
///////////Life Form Scanner Down - NO LONGER USED///////////////////////
/////////////////////////////////////////////////////////////////////////

void LifeFormScannerDown(){

  
  //move down

  digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 7500; x++) {
  digitalWrite(stepPin,HIGH);
  delayMicroseconds(500);
  digitalWrite(stepPin,LOW);
    delayMicroseconds(500);
  }
  delay(1000);

  //Close Pie Panel
  Wire.beginTransmission(20); // transmit to device #20
     Wire.write(11);  // 
     delay(1500);
     Wire.endTransmission();    // stop transmitting


  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
}


//////////////////////////////////////////////////////////////////////////////
////Life Form Scanner Up - NO LONGER USED/////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


void LifeFormScannerUp(){
  //move up

//Open Pie Panel
  Wire.beginTransmission(20); // transmit to device #20
     Wire.write(10);  // call specific routine to open pie panel
     delay(2000);
     Wire.endTransmission();    // stop transmitting
  
   
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 7500; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
  }
  delay(1000); // One second delay

 // Wire.beginTransmission(20); // transmit to device #20
   //  Wire.write(20);  // call specific routine to rotate scanner
     //delay(1500);
     //Wire.endTransmission();    // stop transmitting

  
  //delay(20000);  //20 second delay to let rotation happen

    i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  
}



//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////Entire LifeForm Scanner Routine/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


void EntireScannerRoutine(){



//Open Pie Panel
  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
      
      Servos[PP6].attach(PP6_SERVO_PIN); 
       
      Servos[PP6].write(2200,FASTSPEED);
      delay(1500);
      Servos[PP6].detach();     
  
//move scanner up 
  delay (1200);
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 7500; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
  }
  delay(1000); // One second delay

//Call to turn on flashing LED's
digitalWrite (LFS_LED, HIGH); // send power to LFS LED's


//Call to Rotate Scanner

Servos[Scanner1].attach(Scanner1_SERVO_PIN); 
 Servos[Scanner1].write(600,25,true);

 //delay (2000);

  
 Servos[Scanner1].write(2400,25,true);
 //delay (2000);

 Servos[Scanner1].write(600,25,true);
 //delay (2000);
  
 Servos[Scanner1].write(2400,25,true);
// delay (2000);
 
 Servos[Scanner1].write(600,25,true);

 Servos[Scanner1].write(2400,25,true);
// delay (2000);
 
 Servos[Scanner1].write(600,25,true);

 Servos[Scanner1].write(600,25,true);
// delay (2000);
 
 Servos[Scanner1].write(2400,25,true);

 Servos[Scanner1].write(600,25,true);
// delay (2000);
 
 Servos[Scanner1].write(1021,25,true);




 delay(2000);
      
 Servos[Scanner1].detach();

 //delay(31000);

 //Call to turn off flashing LED's
digitalWrite (LFS_LED, LOW); // send power to LFS LED's

 //move scanner down

 digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 7500; x++) {
  digitalWrite(stepPin,HIGH);
  delayMicroseconds(500);
  digitalWrite(stepPin,LOW);
    delayMicroseconds(500);
  }
  delay(2000);

  //Close Pie Panel
  
  Servos[PP6].attach(PP6_SERVO_PIN);    
  Servos[PP6].write(1380,FASTSPEED);
  delay (3300);
  Servos[PP6].detach();   


digitalWrite(STATUS_LED, LOW); // turn on STATUS LED so we can visually see we got the command on the board     

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
   
}



////////////////////////////////////////////////////////////////
//////Open Close Pie Panel 1////////////////////////////////////
//////////////////////////////////////////////////////////////////

void OpenClosePiePanel1() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide

    digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
    //Open or close  Pie Panel 1, each command will trigger an open or close command
    
    Serial.print("Pie Panel 1:");
    
    if (PiePanel1Open) { // Close the Pie Panel if PiePanel1Open is true
      Serial.println("Closing");
      PiePanel1Open=false;

   
      //.attach allows servo to operate
      Servos[PP1].attach(PP1_SERVO_PIN); 
      

      //sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)
      
      //closes panel, "true" statement allows servo to reach position before executing next .write
      Servos[PP1].write(750,SPEED,true); // the "true" uses servo postion to ensure full position prior to next .write line. Good for smooth sequence verses delays
      
      delay(800);
      // .detach disables servos      
      Servos[PP1].detach();
      Serial.println("Closed");
       
    } else { // Open Pie Panels
      Serial.println("Opening Pie Panel");
      PiePanel1Open=true;

      
       //.attach allows servos to operate
      Servos[PP1].attach(PP1_SERVO_PIN); 
      
      //sendI2Ccmd("$01");  // Stealth sound folder one (general sounds)
      
       // This is a "for" loop, see Arduino site for ref.  Basically it allows you to repeat things between {curly braces} as many times as you want
       //for (int i=0; i<2; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
 
     
             
       
       Servos[PP1].write(2000,FASTSPEED,true);
             
       
       
      delay(1000);
      // .detach disables servos      
      Servos[PP1].detach();
      
      Serial.println("Opened PP1");
    }
    // end "else" 
    
    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    digitalWrite(STATUS_LED, LOW);

    
}

////////////////////////////////////////////////////////////////
//////Quick Open Pie Panel 1////////////////////////////////////
//////////////////////////////////////////////////////////////////

void QuickOpenPiePanel1() 
{  
    digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
      
      Servos[PP1].attach(PP1_SERVO_PIN); 
       
      Servos[PP1].write(2000,FASTSPEED,true);

      Servos[PP1].detach();
          
    digitalWrite(STATUS_LED, LOW);

    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    
}


//////////////////////END NEW CODE////////////////////
///////////Quick Open Pie Panel 1/////////////////////
//////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//////Quick Close Pie Panel 1////////////////////////////////////
//////////////////////////////////////////////////////////////////

void QuickClosePiePanel1() 
{  
    digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
      
      Servos[PP1].attach(PP1_SERVO_PIN); 
       
      Servos[PP1].write(750,FASTSPEED,true);
             
         Servos[PP1].detach(); 
    digitalWrite(STATUS_LED, LOW);

    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    
}


//////////////////////END NEW CODE////////////////////
///////////Quick Close Pie Panel 1/////////////////////
//////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//////Quick Open Pie Panel 2////////////////////////////////////
//////////////////////////////////////////////////////////////////

void QuickOpenPiePanel2() 
{  
    digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
      
      Servos[PP2].attach(PP2_SERVO_PIN); 
       
      Servos[PP2].write(2000,FASTSPEED,true);
             
        Servos[PP2].detach();  
    digitalWrite(STATUS_LED, LOW);

    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    
}


//////////////////////END NEW CODE////////////////////
///////////Quick Open Pie Panel 2/////////////////////
//////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//////Quick Close Pie Panel 2////////////////////////////////////
//////////////////////////////////////////////////////////////////

void QuickClosePiePanel2() 
{  
    digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
      
      Servos[PP2].attach(PP2_SERVO_PIN); 
       
      Servos[PP2].write(750,FASTSPEED,true);
             
          
    digitalWrite(STATUS_LED, LOW);

Servos[PP2].detach();
    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    
}


//////////////////////END NEW CODE////////////////////
///////////Quick Close Pie Panel 2/////////////////////
//////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////
//////Quick Open Pie Panel 3////////////////////////////////////
//////////////////////////////////////////////////////////////////

void QuickOpenPiePanel3() 
{  
    digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
      
      Servos[PP5].attach(PP5_SERVO_PIN); 
       
      Servos[PP5].write(2000,FASTSPEED,true);
             
          Servos[PP5].detach();
    digitalWrite(STATUS_LED, LOW);

    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    
}


//////////////////////END NEW CODE////////////////////
///////////Quick Open Pie Panel 3/////////////////////
//////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//////Quick Close Pie Panel 3////////////////////////////////////
//////////////////////////////////////////////////////////////////

void QuickClosePiePanel3() 
{  
    digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
      
      Servos[PP5].attach(PP5_SERVO_PIN); 
       
      Servos[PP5].write(500,FASTSPEED,true);
             
          
    digitalWrite(STATUS_LED, LOW);
Servos[PP5].detach();
    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    
}


//////////////////////END NEW CODE////////////////////
///////////Quick Close Pie Panel 3/////////////////////
//////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//////Quick Open Pie Panel 4////////////////////////////////////
//////////////////////////////////////////////////////////////////

void QuickOpenPiePanel4() 
{  
    digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
      
      Servos[PP6].attach(PP6_SERVO_PIN); 
       
      Servos[PP6].write(2200,FASTSPEED);
      delay(1500);
        Servos[PP6].detach();     
          
    digitalWrite(STATUS_LED, LOW);

    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    
}


//////////////////////END NEW CODE////////////////////
///////////Quick Open Pie Panel 4/////////////////////
//////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//////Quick Close Pie Panel 4////////////////////////////////////
//////////////////////////////////////////////////////////////////

void QuickClosePiePanel4() 
{  
    digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
      
      Servos[PP6].attach(PP6_SERVO_PIN); 
       
      Servos[PP6].write(1380,FASTSPEED);

      delay (1900);
             
       Servos[PP6].detach();   
    digitalWrite(STATUS_LED, LOW);

    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    
}


//////////////////////END NEW CODE////////////////////
///////////Quick Close Pie Panel 4/////////////////////
//////////////////////////////////////////////////////



///////////////////////////////////////////////////
/////////Cantina Pie//////////////////////////////
///////////////////////////////////////////////////

void CantinaPie()
{

digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
    //Open or close  All Pie Panels, each command will trigger an open or close command
    
    Serial.print("Cantina Pie Panels:");
    
   
      //.attach allows servos to operate
      Servos[PP1].attach(PP1_SERVO_PIN); 
      Servos[PP2].attach(PP2_SERVO_PIN);
      Servos[PP5].attach(PP5_SERVO_PIN);
      Servos[PP6].attach(PP6_SERVO_PIN);

      for (int i=0; i<4; i++) {   //Loop  6  times (i<6;)Increase or decrease number to change looped times
 
     
       // open pies in pairs
       
       

       delay (480);
       
       Servos[PP1].write(750,FASTSPEED);
       Servos[PP2].write(750,FASTSPEED);
       Servos[PP5].write(2000,FASTSPEED);
       Servos[PP6].write(2000,FASTSPEED);

       delay (480);
       
       Servos[PP1].write(2000,FASTSPEED);
       Servos[PP2].write(2000,FASTSPEED);
       Servos[PP5].write(750,FASTSPEED);
       Servos[PP6].write(1380,FASTSPEED);

      }
      
      delay (480);

      Servos[PP1].write(750,FASTSPEED);
      Servos[PP2].write(750,FASTSPEED);

       delay (480);
       
 // .detach disables servos      
      Servos[PP1].detach();
      Servos[PP2].detach();
      Servos[PP5].detach();
      Servos[PP6].detach();
 i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    digitalWrite(STATUS_LED, LOW);

  
}




////////////////End Cantina Pies////////////////////////



///////////////////////////////////////////////////
/////////Cantina Pie2//////////////////////////////
///////////////////////////////////////////////////

void CantinaPie2()
{

digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
    //Open or close  All Pie Panels, each command will trigger an open or close command
    
    Serial.print("Cantina Pie Panels:");
    
   
      //.attach allows servos to operate
      Servos[PP1].attach(PP1_SERVO_PIN); 
      Servos[PP2].attach(PP2_SERVO_PIN);
      Servos[PP5].attach(PP5_SERVO_PIN);
      Servos[PP6].attach(PP6_SERVO_PIN);

      for (int i=0; i<2; i++) {   //Loop  6  times (i<6;)Increase or decrease number to change looped times
 
     
       // open pies in pairs
       
       

       delay (480);
       
       Servos[PP1].write(750,FASTSPEED);
       Servos[PP2].write(750,FASTSPEED);
       Servos[PP5].write(2000,FASTSPEED);
       Servos[PP6].write(2200,FASTSPEED);

       delay (480);
       
       Servos[PP1].write(2000,FASTSPEED);
       Servos[PP2].write(2000,FASTSPEED);
       Servos[PP5].write(750,FASTSPEED);
       Servos[PP6].write(1380,FASTSPEED);

      }

      delay (480);

      Servos[PP1].write(750,FASTSPEED);
       Servos[PP2].write(750,FASTSPEED);


      delay (480);
 // .detach disables servos      
      Servos[PP1].detach();
      Servos[PP2].detach();
      Servos[PP5].detach();
      Servos[PP6].detach();
 i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    digitalWrite(STATUS_LED, LOW);

  
}




////////////////End Cantina Pies2////////////////////////



//////////////////////////////////////////////////////////
/////////////Attach PiePanel Servos//////////////////////
/////////////////////////////////////////////////////////

void AttachPies()
{
        Servos[PP1].attach(PP1_SERVO_PIN); 
      Servos[PP2].attach(PP2_SERVO_PIN);
      Servos[PP5].attach(PP5_SERVO_PIN);
      Servos[PP6].attach(PP6_SERVO_PIN);

}

//////////////////////////////////////////////////////////
////////////Detach PiePanel Servos////////////////////////
/////////////////////////////////////////////////////////

void DetachPies()
{

      Servos[PP1].detach();
      Servos[PP2].detach();
      Servos[PP5].detach();
      Servos[PP6].detach();
}


////////////////////////////////////////////////
/////////////Hello All Open Pie Subroutine///////////
////////////////////////////////////////////////

void OpenAllPies()
{

    //.attach allows servos to operate
      Servos[PP1].attach(PP1_SERVO_PIN); 
      Servos[PP2].attach(PP2_SERVO_PIN);
      Servos[PP5].attach(PP5_SERVO_PIN);
      Servos[PP6].attach(PP6_SERVO_PIN);
  

       Servos[PP1].write(1500,FASTSPEED);
       Servos[PP2].write(1500,FASTSPEED);
       Servos[PP5].write(1500,FASTSPEED);
       Servos[PP6].write(2000,FASTSPEED);
}
 
       


////////////////////////////////////////////////
/////////////Hello All Close Pie Subroutine///////////
////////////////////////////////////////////////

void CloseAllPies()
{

    //.attach allows servos to operate
      Servos[PP1].attach(PP1_SERVO_PIN); 
      Servos[PP2].attach(PP2_SERVO_PIN);
      Servos[PP5].attach(PP5_SERVO_PIN);
      Servos[PP6].attach(PP6_SERVO_PIN);
       
      Servos[PP1].write(750,FASTSPEED); // the "true" uses servo postion to ensure full position prior to next .write line. Good for smooth sequence verses delays
      Servos[PP2].write(750,FASTSPEED);
      Servos[PP5].write(750,FASTSPEED);
      Servos[PP6].write(1380,FASTSPEED);
      
       

}




////////////////////////////////////////////////////////
// Open/Close Pie Panels////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void OpenClosePies() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide


         digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
    //Open or close  All Pie Panels, each command will trigger an open or close command
    
    Serial.print("Pie Panels:");
    
    if (PiesOpen) { // Close the Pie Panels if PiesOpen is true
      Serial.println("Closing");
      PiesOpen=false;
      

   
      //.attach allows servos to operate
      Servos[PP1].attach(PP1_SERVO_PIN); 
      Servos[PP2].attach(PP2_SERVO_PIN);
      Servos[PP5].attach(PP5_SERVO_PIN);
      Servos[PP6].attach(PP6_SERVO_PIN);
      

      sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)
      
      //closes panels, "true" statement allows servo to reach position before executing next .write
      Servos[PP1].write(750,SPEED,true); // the "true" uses servo postion to ensure full position prior to next .write line. Good for smooth sequence verses delays
      Servos[PP2].write(750,SPEED,true);
      Servos[PP5].write(750,SPEED,true);
      Servos[PP6].write(1380,SPEED,true);
      
      
      delay(800);
      // .detach disables servos      
      Servos[PP1].detach();
      Servos[PP2].detach();
      Servos[PP5].detach();
      Servos[PP6].detach();
      
      
      Serial.println("Closed");
       
    } else { // Open Pie Panels
      Serial.println("Opening Pie Panels");
      PiesOpen=true;
      randompie=random(1,4);
      


       //.attach allows servos to operate
      Servos[PP1].attach(PP1_SERVO_PIN); 
      Servos[PP2].attach(PP2_SERVO_PIN);
      Servos[PP5].attach(PP5_SERVO_PIN);
      Servos[PP6].attach(PP6_SERVO_PIN);
      

      sendI2Ccmd("$01");  // Stealth sound folder one (general sounds)

      if (randompie==1) {
      
      
       // This is a "for" loop, see Arduino site for ref.  Basically it allows you to repeat things between {curly braces} as many times as you want
       for (int i=0; i<2; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
 
     
       // open pies in wave
       
       Servos[PP1].write(1700,FASTSPEED,true);
       Servos[PP2].write(1700,FASTSPEED,true);
       Servos[PP5].write(1700,FASTSPEED,true);
       Servos[PP6].write(2000,FASTSPEED,true);

       
       // close pies in opposite wave
       
       Servos[PP1].write(750,FASTSPEED,true);
       Servos[PP2].write(750,FASTSPEED,true);
       Servos[PP5].write(750,FASTSPEED,true);
       Servos[PP6].write(1380,FASTSPEED,true);

       }
        

       // reopen pies
       Servos[PP6].write(1700,FASTSPEED,true);
       Servos[PP5].write(1700,FASTSPEED,true);
       Servos[PP2].write(1700,FASTSPEED,true);
       Servos[PP1].write(2000,FASTSPEED,true);
       
      delay(1000);

      }

      else if (randompie==2)  {
      
      

       Servos[PP1].write(1700,FASTSPEED);
       Servos[PP2].write(1700,FASTSPEED);

      for (int i=0; i<4; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
 
     
       // open pies in pairs
       
       

       delay (300);
       
       Servos[PP1].write(750,FASTSPEED);
       Servos[PP2].write(750,FASTSPEED);
       Servos[PP5].write(1700,FASTSPEED);
       Servos[PP6].write(2200,FASTSPEED);

       delay (300);
       
       Servos[PP1].write(1700,FASTSPEED);
       Servos[PP2].write(1700,FASTSPEED);
       Servos[PP5].write(750,FASTSPEED);
       Servos[PP6].write(1380,FASTSPEED);

      }
       Servos[PP5].write(1700,FASTSPEED);
       Servos[PP6].write(2200,FASTSPEED);

      delay(1000);

      }

      else if (randompie==3){

      
      
       Servos[PP2].write(1700,FASTSPEED);
       Servos[PP6].write(2100,FASTSPEED);
       
       for (int i=0; i<4; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
 
     
       // open pies in pairs
       
       

       delay (300);
       
       Servos[PP1].write(1700,FASTSPEED);
       Servos[PP5].write(1700,FASTSPEED);
       Servos[PP2].write(750,FASTSPEED);
       Servos[PP6].write(1380,FASTSPEED);

       delay (300);
       
       Servos[PP1].write(750,FASTSPEED);
       Servos[PP5].write(750,FASTSPEED);
       Servos[PP2].write(1700,FASTSPEED);
       Servos[PP6].write(2100,FASTSPEED);

      }
       
       delay (300);
       Servos[PP2].write(750,FASTSPEED);
       Servos[PP6].write(1380,FASTSPEED);
       delay (300);
       

      Servos[PP1].write(1700,FASTSPEED);
       Servos[PP2].write(1700,FASTSPEED);
       Servos[PP5].write(1700,FASTSPEED);
       Servos[PP6].write(2100,FASTSPEED);

       
       delay(1000);

      }
      
      
      
      
     
      // .detach disables servos      
      Servos[PP1].detach();
      Servos[PP2].detach();
      Servos[PP5].detach();
      Servos[PP6].detach();
      

      Serial.println("Opened Pies");
    
    }
    
    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    digitalWrite(STATUS_LED, LOW);

    
}




// Main Loop
void loop() {

  delay(100); 

  loopTime = millis(); 
  
  // Momentary Button/Switch on Pin 10 - connect Gnd+Signal to switch terminals
  //if (loopTime>nextCheck) {
    // buttonVal = digitalRead(EXT_BUTTON);
     //Serial.print("Trigger Pin Value (1=off, 0=on/triggered) = "); 
     //Serial.println(buttonVal);
     //if (buttonVal==0) {
      // bad_motivator();
    // }
    // nextCheck = loopTime+1000; // only check every second
  //}
  
  // Check for new i2c command

  switch(i2cCommand) {
    case 1: // RESET
          Serial.println("Got reset message");
          digitalWrite(STATUS_LED, HIGH);
          i2cCommand=-1; 
          break;

    case 2:
          OpenClosePies();
          break;

    case 3:
          OpenClosePiePanel1();
          break;  


    case 4:
          QuickOpenPiePanel1();
          break;

    case 5:
          QuickClosePiePanel1();
          break;

   case 6:
          QuickOpenPiePanel2();
          break;
   case 7:
          QuickClosePiePanel2();
          break;
   case 8:
          QuickOpenPiePanel3();
          break;
   case 9:
          QuickClosePiePanel3();
          break;
          

   case 10:
          QuickOpenPiePanel4();
          break;
   case 11:
          QuickClosePiePanel4();
          break;
                         
    case 12:
          bad_motivator();
          break;


    case 13:
          CantinaPie();
          break;

    case 14:
          CantinaPie2();
          break;

    case 15:
          WaveRandomPie();
          break;

    case 16:
          AttachPies();
          break;

    case 17:
          DetachPies();
          break;

    case 18:
          OpenAllPies();
          break;

    case 19:
          CloseAllPies();
          break;

   
    case 22:
          EntireScannerRoutine();
          break;
    

          
    default: // Catch All
    case 0: 
          digitalWrite(STATUS_LED, LOW);
          i2cCommand=-1;    
          break;
  }
 
}
