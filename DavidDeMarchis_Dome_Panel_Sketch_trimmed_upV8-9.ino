//-----------------------------------------------------------------------------------------------
// Servo Expander "Dome Stealth Servo Expander" 
// 
// John Thompson (FrettedLogic) 9-2017
// "Shamelessly paraphrased from Chris James' foundation"
// Customized by David DeMarchis - September 2020
//
// Pie Panels Functional
// Lower Dome Panels Functional
// Pie Panel 1 Functional alone
// LCD screen functionality (with two videos)
// Now incorporating HP movement and lights under certain triggers  (not sure this is working properly)
// Merge of V8 and V8-1 (pie panels were giving me trouble so just copied low panels code to pie panels routine and changed the servos)  Seems to have worked!!!!
// Now that Pie Panels seem to be working using same routine as Low Panels....going to add in some randomness to each routines
// Added Single Low Dome Panel Wave (haven't tested yet as of 8-6-2020)
// Adding Fire Extinguisher (PIN #10)
// Removed all references to Pie panels as those are now on a separate Servo Expander 
// Added a Cantina dance routines
// Adding HP movement via Stealth Controller
// Added randomly chosen single low panel movement (9-30-2020)
// Added "Hello All" routine to open/close all panels mimicing saying "hello"  11-12-20
// Added "Rotate Scanner" - Spring 2021
// Added "Bubble Blower" - September 2022


//--------------------------------------------------------------

#include <Wire.h>
#include <VarSpeedServo.h>

// outgoing i2c Commands
String Packet;
int count = 0;
int randompie=1;
int randomlow=1;
byte sum;
#define DESTI2C 0 //Main Stealth board I2C = 0

// incoming i2c command
int i2cCommand = 0;
#define MYI2ADDR 10      //I2C Address for DOME SERVO EXPANDER = 10

unsigned long loopTime; // Time variable

#define STATUS_LED 13

// Dome panel names for our Arduino pins, USE THESE PHYSICAL PINS FOR SPECIFIC DOME SERVO
// Board Pins #2-#13. Panels marked based on Club Spec Panel numbering. PP=Pie Panels, P=Lower Panels, DT=Dome Topper


#define Scanner1_SERVO_PIN  4
#define HP_SERVO_PIN 5  // TOPHp  Top Holo projector
#define P1_SERVO_PIN 6   //  P1    Low Panel        
#define P2_SERVO_PIN 7    //  P2    Low Panel        
#define P3_SERVO_PIN 8    //  P3    Low Panel        
#define P4_SERVO_PIN 9   //  P4    Low Panel      
#define FIRE_PIN 10  //  Linear Actuator for Fire Extinguisher

  



// Create an array of VarSpeedServo type, containing 5 elements/entries. 
// Note: Arrays are zero indexed. See http://arduino.cc/en/Reference/array

#define NBR_SERVOS 7  // Number of Servos (6)
VarSpeedServo Servos[NBR_SERVOS];

// Note: These reference internal numbering not physical pins for array use


#define P1 0    // P1    Low Panel        
#define P2 1    // P2    Low Panel        
#define P3 2    // P3    Low Panel        
#define P4 3    // P4    Low Panel  
#define FIRE 4
#define TopHP 5  // TopHP  
#define Scanner1 6  // Scanner Servo   

const int bubble_relay_pin = 3;
const int relaypin=11;      
const int second_relay_pin=12;

  
//   My Servos(frettedlogic):  Yours will be different, see variables defined...
//  Servos in open position = 555ms ( 0 degrees )  
//  Servos in closed position = 1950ms ( 180 degrees ) 

#define FIRST_SERVO_PIN 4 //First Arduino Pin for Servos

// Servo degrees may be different for other peoples dome servos, use positive and/or negative numbers as applicable (frettedlogic)

// Dome Panel Open/Close values, can use servo value typically 1000-2000ms or position 0-180 degress. (I REVERESED THESE OPEN AND CLOSE VALUES BASED ON MY SERVOS FROM THE PREVIOUS REVISION OF THIS SKETCH
#define PANEL_OPEN 1900    //  (40)  0=open position, 555 us                          
#define PANEL_CLOSE 525   //   (170) 180=close position, 1900 us                        
#define PANEL_HALFWAY 1200  // 90=midway point 1500ms, use for animations etc. 
#define PANEL_PARTOPEN 1650  //Partially Open, approximately 1/4   
#define FIRE_ENGAGED 1401   // Spray Fire
#define FIRE_DISENGAGED 1185  // Stop Spraying Fire
 
           
// Panel Speed Variables 0-255, higher number equals faster movement
#define SPEED 200            
#define FASTSPEED 255    
#define OPENSPEED 230   
#define CLOSESPEED 230

// Variables so program knows where things are
boolean PiesOpen=false;
boolean AllOpen=true;
boolean LowOpen=true;
boolean OpenClose=false;
boolean PiePanel1Open=false;

//----------------------------------------------------------------------------
//  i2c Commands
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {

  Serial.begin(57600); // DEBUG
  
  Serial.println("Stealth Dome Server Expander - 2017");

  Serial.println("My i2c address: ");
  Serial.println(MYI2ADDR);
  Wire.begin(MYI2ADDR);                // Start I2C Bus as a Slave (Device Number 10)
  Wire.onReceive(receiveEvent); // register i2c receive event
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
//  resetServos();

  pinMode(relaypin, OUTPUT);
  pinMode(second_relay_pin, OUTPUT);
  pinMode(bubble_relay_pin, OUTPUT);
  Serial.println("Waiting for i2c command");
  
}

//------------------------------------------------------
// receive Event

void receiveEvent(int howMany) {
  i2cCommand = Wire.read();    // receive i2c command
  Serial.print("Command Code:");
  Serial.println(i2cCommand);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                                                //      
//                                                                         Command Sequences                                                                      //
//                                                                                                                                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
////////////////HP MOVEMENT ROUTINE/////////////////////////
////////////////////////////////////////////////////////////

void HPMove()
{

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board   
  Serial.print("HP Move");
  
  Servos[TopHP].attach(HP_SERVO_PIN);
  Servos[TopHP].write(2500,10);
  delay(5000);
  Servos[TopHP].write(1999,12);

  
  delay(5000);
  Servos[TopHP].write(2250,15);
  delay(5000);
  Servos[TopHP].write(1999,20);
  delay(5000);
  Servos[TopHP].write(2500,10);
  delay(5000);
  Servos[TopHP].write(1999,10);
  delay(5000);
  Servos[TopHP].detach();

  digitalWrite(STATUS_LED, LOW);


  i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command

  
}



////////////////////////////////////////////////////////////////
////////////LCD Screen Operation - Death Star Plans/////////////
////////////////////////////////////////////////////////////////

void LCDScreenDeathStar()

{

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board   
  Serial.print("LCD Screen Death Star");
  sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds) (NEED TO CHANGE THIS TO SCARED SOUND)

  Serial.println("Open Single Low Panel for LCD");
  
  // Do some waves with the low panels

  Servos[P1].attach(P1_SERVO_PIN); 
  Servos[P2].attach(P2_SERVO_PIN);
  Servos[P3].attach(P3_SERVO_PIN);
  
           

  sendI2Ccmd("$01");  // Stealth sound folder one (general sounds)
      
    // This is a "for" loop, see Arduino site for ref.  Basically it allows you to repeat things between {curly braces} as many times as you want
    for (int i=0; i<4; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
  
    // open lows in wave
       
    Servos[P1].write(1400,FASTSPEED,true);
    Servos[P2].write(1400,FASTSPEED,true);
    Servos[P3].write(1400,FASTSPEED,true);
          
    // close lows in opposite wave
       
    Servos[P1].write(750,FASTSPEED,true);
    Servos[P2].write(750,FASTSPEED,true);
    Servos[P3].write(750,FASTSPEED,true);
       
    }

  

  delay (300);
       
  // .detach disables servos      
  Servos[P1].detach();
  Servos[P2].detach();
  Servos[P3].detach();
  
      

      // send I2C to holo board
     Wire.beginTransmission(25); // transmit to device #25 HP Board
     Wire.write("T0063\r");  // see Flthy's ref for command specifics
     Wire.endTransmission();    // stop transmitting

     
      Wire.beginTransmission(25); // transmit to device #25 HP Board
      Wire.write("T103\r"); // sends to up 
      Wire.endTransmission();    // stop transmitting

      
  digitalWrite (relaypin, HIGH); // send power to LCD screen 
  delay (2000);
  Servos[P4].attach(P4_SERVO_PIN,850,2000);
      
      Servos[P4].write(2000,FASTSPEED,true);

      sendI2Ccmd("$11"); // sends an I2C command to Stealth folder 11 to play music


      // Delay for 1:22 seconds to let video finish

      delay(82000);

      //close panel
      Servos[P4].write(600,FASTSPEED);
      delay(2000);
      // turn off LCD
      
      digitalWrite(STATUS_LED, LOW);

      digitalWrite (relaypin, LOW); // cut power to LCD screen
      Servos[P4].detach();

      i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command

      
      
      
       
}

////////////////////////////////////////////////
//////////END LCD Screen Routine////////////////
////////////////////////////////////////////////





////////////////////////////////////////////////////////////////
////////////LCD Screen Operation - Leia Message ////////////////
////////////////////////////////////////////////////////////////

void LCDScreenLeiaMessage()

{

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board   
  Serial.print("LCD Screen Leia Message");


  Serial.println("Open Single Low Panel for LCD");

  Servos[P1].attach(P1_SERVO_PIN); 
      Servos[P2].attach(P2_SERVO_PIN);
      Servos[P3].attach(P3_SERVO_PIN);
      sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds) 
     Wire.beginTransmission(25); // transmit to device #25 HP Board
     Wire.write("A002\r");  // see Flthy's ref for command specifics
     Wire.endTransmission();    // stop transmitting

     
      Wire.beginTransmission(25); // transmit to device #25 HP Board
      Wire.write("F104\r"); // wags
      Wire.endTransmission();    // stop transmitting

  sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds) 


  for (int i=0; i<3; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
 
     
       // open lows in wave
       
       Servos[P1].write(1400,FASTSPEED,true);
       Servos[P2].write(1400,FASTSPEED,true);
       Servos[P3].write(1400,FASTSPEED,true);
       

       
       // close lows in opposite wave
       
       Servos[P1].write(750,FASTSPEED,true);
       Servos[P2].write(750,FASTSPEED,true);
       Servos[P3].write(750,FASTSPEED,true);
       
  

       }

     
      // Now trigger LCD screen to turn on 

       digitalWrite (relaypin, HIGH); // send power to LCD screen 
       delay (1000);
       digitalWrite (second_relay_pin, HIGH); // switch from Death Star to Leia video
       delay (1311);
       
       digitalWrite (second_relay_pin, LOW); //  end switch

      delay (1500);
       Servos[P4].attach(P4_SERVO_PIN);
      
       Servos[P4].write(2000,FASTSPEED,true);

       delay (925);

       sendI2Ccmd("$04"); // sends an I2C command to Stealth folder 4 to long leia message




      // Delay for 40 seconds to let video finish

      delay(39000);

      //close panel
      Servos[P4].write(600,FASTSPEED,true);
      delay(2000);
      // turn off LCD
      
      digitalWrite(STATUS_LED, LOW);

      digitalWrite (relaypin, LOW); // cut power to LCD screen

      Servos[P1].detach();
      Servos[P2].detach();
      Servos[P3].detach();
      Servos[P4].detach();
      
      
      i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
       
}

////////////////////////////////////////////////
//////////END LCD LEIA Screen Routine////////////////
////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
////////////Fire Extinguisher ////////////////
////////////////////////////////////////////////////////////////

void Fire()

{

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board   
  Serial.print("Fire");
  
  //////////////////////////////////////////////////////////////////////
  ////Following Code was Just a test to see if I could call second SE//
  ///in dome from within this SE.  It worked!!/////////////////////////
  //Wire.beginTransmission(20); // transmit to device #20 (Dome SE #2)
  //Wire.write(3); // Perform case 3 from device #20
  //Wire.endTransmission();    // stop transmitting
  //////////////////////////////////////////////////////////////////////

  Serial.println("Open Single Low Panel for Fire Extinguisher");

  Servos[P1].attach(P1_SERVO_PIN); 
       
  Servos[P1].write(1400,FASTSPEED,true);

  Servos[FIRE].attach(FIRE_PIN);
  Servos[FIRE].write(FIRE_ENGAGED, FASTSPEED, true);

  delay (1000);

  Servos[FIRE].write(FIRE_DISENGAGED, FASTSPEED, true);

  delay (1000);

  Servos[P1].write(750,FASTSPEED,true);
      
  Servos[P1].detach();

  //Wire.beginTransmission(20); // transmit to device #20 (Dome SE #2)
  //Wire.write(3); // Perform case 3 from device #20
  //Wire.endTransmission();    // stop transmitting


  digitalWrite(STATUS_LED, LOW);
      
  i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
       
}

////////////////////////////////////////////////
//////////END FIRE/////////////////////////////
////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
////////////Bubble Blower    //////////////////////////////////
////////////////////////////////////////////////////////////////

void Bubble()

{

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board   
  Serial.print("Bubble");
  
    Serial.println("Open Single Low Panel for Bubble Blower");

  Servos[P3].attach(P3_SERVO_PIN); 
       
  Servos[P3].write(1400,FASTSPEED,true);

  delay (1000);

  //turn relay on to supply power to bubble blower

  digitalWrite (bubble_relay_pin, HIGH); // send power to Bubble Blower
  delay (4000); // blow for 4 seconds
  digitalWrite (bubble_relay_pin, LOW); // cut power to Bubble Blower
  
  delay (1000);
  
  Servos[P3].write(750,FASTSPEED,true);
      
  Servos[P3].detach();

  
  digitalWrite(STATUS_LED, LOW);
      
  i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
       
}

////////////////////////////////////////////////
//////////END BUBBLE/////////////////////////////
////////////////////////////////////////////////





///////////////////////////////////////////////////
/////Single Wave of Low Dome Panels////////////////
///////////////////////////////////////////////////

void SingleWaveLow()  {

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board   
  Serial.print("Single Wave of Low Dome Panels");



  Servos[P1].attach(P1_SERVO_PIN); 
  Servos[P2].attach(P2_SERVO_PIN);
  Servos[P3].attach(P3_SERVO_PIN);
  Servos[P4].attach(P4_SERVO_PIN);


 // send I2C to holo board
     Wire.beginTransmission(25); // transmit to device #25 HP Board
     Wire.write("T0063\r");  // see Flthy's ref for command specifics
     Wire.endTransmission();    // stop transmitting

     
      Wire.beginTransmission(25); // transmit to device #25 HP Board
      Wire.write("T103\r"); // sends to up 
      Wire.endTransmission();    // stop transmitting


  sendI2Ccmd("$01");  // Stealth sound folder one (general sounds)
      
    // This is a "for" loop, see Arduino site for ref.  Basically it allows you to repeat things between {curly braces} as many times as you want
    for (int i=0; i<2; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
  
    // open lows in wave
       
    Servos[P1].write(1400,FASTSPEED,true);
    Servos[P2].write(1400,FASTSPEED,true);
    Servos[P3].write(1400,FASTSPEED,true);
    Servos[P4].write(1400,FASTSPEED,true);
          
    // close lows in opposite wave
       
    Servos[P1].write(750,FASTSPEED,true);
    Servos[P2].write(750,FASTSPEED,true);
    Servos[P3].write(750,FASTSPEED,true);
    Servos[P4].write(750,FASTSPEED,true);   
    }

    Servos[P1].detach();
    Servos[P2].detach();
    Servos[P3].detach();
    Servos[P4].detach();

  digitalWrite(STATUS_LED, LOW);      
  i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hello All Dome Panels////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void HelloAll()
{

digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
    //Open or close  All Pie Panels, each command will trigger an open or close command
    
    Serial.print("All Panels:");
    
   
      //.attach allows servos to operate
      Servos[P1].attach(P1_SERVO_PIN); 
      Servos[P2].attach(P2_SERVO_PIN);
      Servos[P3].attach(P3_SERVO_PIN);
      Servos[P4].attach(P4_SERVO_PIN);

      //call to attach piepanel servos

            Wire.beginTransmission(20); // transmit to device #20
            Wire.write(16);  // 
            Wire.endTransmission();    // stop transmitting


      sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)
      
      //Call to open Pies
            Wire.beginTransmission(20); // transmit to device #20
            Wire.write(18);  // 
            Wire.endTransmission();    // stop transmitting

       //Open lows
            Servos[P1].write(1100,FASTSPEED);
            Servos[P2].write(1100,FASTSPEED);
            Servos[P3].write(1100,FASTSPEED);
            Servos[P4].write(1100,FASTSPEED,true);

       delay(100);
     
       //Call to close Pies   
            Wire.beginTransmission(20); // transmit to device #20
            Wire.write(19);  // 
            Wire.endTransmission();    // stop transmitting

       //Close lows     
   
            Servos[P1].write(750,FASTSPEED);
            Servos[P2].write(750,FASTSPEED);
            Servos[P3].write(750,FASTSPEED);
            Servos[P4].write(750,FASTSPEED,true);

      delay(300);
      
       //Call to open Pies
            Wire.beginTransmission(20); // transmit to device #20
            Wire.write(18);  // 
            Wire.endTransmission();    // stop transmitting

       //Open lows
            Servos[P1].write(1100,FASTSPEED);
            Servos[P2].write(1100,FASTSPEED);
            Servos[P3].write(1100,FASTSPEED);
            Servos[P4].write(1100,FASTSPEED,true);

       delay(100); 
     
       //Call to close Pies   
            Wire.beginTransmission(20); // transmit to device #20
            Wire.write(19);  // 
            Wire.endTransmission();    // stop transmitting

       //Close lows     
   
            Servos[P1].write(750,FASTSPEED);
            Servos[P2].write(750,FASTSPEED);
            Servos[P3].write(750,FASTSPEED);
            Servos[P4].write(750,FASTSPEED,true);

       delay(300);
    
    Servos[P1].detach();
    Servos[P2].detach();

    Servos[P3].detach();
    Servos[P4].detach();

    
    //call to detach piepanel servos
            Wire.beginTransmission(20); // transmit to device #20
            Wire.write(17);  // 
            Wire.endTransmission();    // stop transmitting
  


digitalWrite(STATUS_LED, LOW);      
  i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  
  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////End Hello All///////End Hello All//////End Hello All////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close Low panels///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenCloseLow() { 

     digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
    //Open or close  All Pie Panels, each command will trigger an open or close command
    
    Serial.print("Low Panels:");
    
    if (LowOpen) { // Close the Low Panels if PiesOpen is true
      Serial.println("Closing");
      LowOpen=false;
      

   
      //.attach allows servos to operate
      Servos[P1].attach(P1_SERVO_PIN); 
      Servos[P2].attach(P2_SERVO_PIN);
      Servos[P3].attach(P3_SERVO_PIN);
      Servos[P4].attach(P4_SERVO_PIN);
      

      sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)
      
      //closes panels, "true" statement allows servo to reach position before executing next .write
      Servos[P1].write(750,SPEED,true); // the "true" uses servo postion to ensure full position prior to next .write line. Good for smooth sequence verses delays
      Servos[P2].write(750,SPEED,true);
      Servos[P3].write(750,SPEED,true);
      Servos[P4].write(850,SPEED,true);
      
      
      delay(800);
      // .detach disables servos      
      Servos[P1].detach();
      Servos[P2].detach();
      Servos[P3].detach();
      Servos[P4].detach();
      
      
      Serial.println("Closed");
       
    } else { // Open Low Panels
      Serial.println("Opening Low Panels");
      LowOpen=true;
      randomlow=random(1,5);
      


       //.attach allows servos to operate
      Servos[P1].attach(P1_SERVO_PIN); 
      Servos[P2].attach(P2_SERVO_PIN);
      Servos[P3].attach(P3_SERVO_PIN);
      Servos[P4].attach(P4_SERVO_PIN);
      

      sendI2Ccmd("$01");  // Stealth sound folder one (general sounds)

      if (randomlow==1) {
       // This is a "for" loop, see Arduino site for ref.  Basically it allows you to repeat things between {curly braces} as many times as you want
       for (int i=0; i<2; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
 
     
       // open lows in wave
       
       Servos[P1].write(2000,FASTSPEED,true);
       Servos[P2].write(2000,FASTSPEED,true);
       Servos[P3].write(2000,FASTSPEED,true);
       Servos[P4].write(2000,FASTSPEED,true);

       
       // close lows in opposite wave
       
       Servos[P1].write(750,FASTSPEED,true);
       Servos[P2].write(750,FASTSPEED,true);
       Servos[P3].write(750,FASTSPEED,true);
       Servos[P4].write(850,FASTSPEED,true);

       }
        

       // reopen lows
       Servos[P4].write(2000,FASTSPEED,true);
       Servos[P3].write(2000,FASTSPEED,true);
       Servos[P2].write(2000,FASTSPEED,true);
       Servos[P1].write(2000,FASTSPEED,true);
       
      
  
       


      delay(1000);

      }

      else if (randomlow==2) {
        
      
      // go into open routine #2//

    Servos[P3].write(750,FASTSPEED);
       Servos[P4].write(750,FASTSPEED);

      for (int i=0; i<4; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
 
     
       // open lows in pairs
       
       

       delay (300);
       
       Servos[P1].write(750,FASTSPEED);
       Servos[P2].write(750,FASTSPEED);
       Servos[P3].write(2000,FASTSPEED);
       Servos[P4].write(2000,FASTSPEED);

       delay (300);
       
       Servos[P1].write(2000,FASTSPEED);
       Servos[P2].write(2000,FASTSPEED);
       Servos[P3].write(750,FASTSPEED);
       Servos[P4].write(750,FASTSPEED);

      }
       Servos[P3].write(2000,FASTSPEED);
       Servos[P4].write(2000,FASTSPEED);

      delay(1000);

      }

      else if (randomlow==3)  {
        
      
      // go into open routine #3//


       Servos[P1].write(750,FASTSPEED);
       Servos[P3].write(750,FASTSPEED);
       
       for (int i=0; i<4; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
 
     
       // open lows in pairs
       
       

       delay (300);
       
       Servos[P1].write(2000,FASTSPEED);
       Servos[P3].write(2000,FASTSPEED);
       Servos[P2].write(750,FASTSPEED);
       Servos[P4].write(750,FASTSPEED);

       delay (300);
       
       Servos[P1].write(750,FASTSPEED);
       Servos[P3].write(750,FASTSPEED);
       Servos[P2].write(2000,FASTSPEED);
       Servos[P4].write(2000,FASTSPEED);

      }
       
       delay (300);
       Servos[P2].write(750,FASTSPEED);
       Servos[P4].write(750,FASTSPEED);

       
       delay(1000);

      }

      else if (randomlow==4) {
      //FINAL THING
       
       Servos[P1].write(2000,FASTSPEED);
       Servos[P2].write(2000,FASTSPEED);
       Servos[P3].write(2000,FASTSPEED);
       Servos[P4].write(2000,FASTSPEED);

      delay(300);
      
       Servos[P1].write(750,FASTSPEED);
       Servos[P2].write(750,FASTSPEED);
       Servos[P3].write(750,FASTSPEED);
       Servos[P4].write(850,FASTSPEED);

      delay(1000);

       Servos[P1].write(2000,FASTSPEED,true);
       Servos[P2].write(2000,FASTSPEED,true);
       Servos[P3].write(2000,FASTSPEED,true);
       Servos[P4].write(2000,FASTSPEED,true);

       delay(1000);
       }
      

    }    
      // .detach disables servos      
      Servos[P1].detach();
      Servos[P2].detach();
      Servos[P3].detach();
      Servos[P4].detach();
      

      Serial.println("Opened Lows");
    
    
    
    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    digitalWrite(STATUS_LED, LOW);

    
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// END of Open/Close Low panels///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
///////////////////Low Dome Panel #1 Hello////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void LowDomePanel1Hello(){

  digitalWrite(STATUS_LED, HIGH);
  randomlow=random(1,4);
  if (randomlow==1) {

  
        Servos[P1].attach(P1_SERVO_PIN); 

        sendI2Ccmd("$01");  // Stealth sound folder one (general sounds)
      
        // This is a "for" loop, see Arduino site for ref.  Basically it allows you to repeat things between {curly braces} as many times as you want
        for (int i=0; i<3; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
  
            
            Servos[P1].write(1400,FASTSPEED,true);
           
       
            Servos[P1].write(750,FASTSPEED,true);
         }

    Servos[P1].detach();
  }  //end if

  else if (randomlow==2) {

          Servos[P2].attach(P2_SERVO_PIN); 

        sendI2Ccmd("$01");  // Stealth sound folder one (general sounds)
      
        // This is a "for" loop, see Arduino site for ref.  Basically it allows you to repeat things between {curly braces} as many times as you want
        for (int i=0; i<3; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
  
            
            Servos[P2].write(1400,FASTSPEED,true);
            
            Servos[P2].write(750,FASTSPEED,true);
         }

    Servos[P2].detach();
  }  //end if




  else if (randomlow==3)  {

      Servos[P1].attach(P1_SERVO_PIN); 
      Servos[P2].attach(P2_SERVO_PIN); 

        sendI2Ccmd("$01");  // Stealth sound folder one (general sounds)
      
        // This is a "for" loop, see Arduino site for ref.  Basically it allows you to repeat things between {curly braces} as many times as you want
        for (int i=0; i<3; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
  
           
       
            Servos[P1].write(1400,FASTSPEED);
            Servos[P2].write(1400,FASTSPEED,true);
          
       
            Servos[P1].write(750,FASTSPEED);
            Servos[P2].write(750,FASTSPEED,true);
         }

    Servos[P1].detach();
    Servos[P2].detach();
  }   //end if




  
  


  
    i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    digitalWrite(STATUS_LED, LOW);

}


/////////////////////////////////
////END LOW DOME PANEL#1 HELLO///
/////////////////////////////////



///////////////////////////////////////////////////////////////////////////
////////////////////Cantina Routine #3/////////////////////////////////////
////////////////////Calls to Second Servo Expander for Pie Panels//////////
///////////////////////////////////////////////////////////////////////////


void Cantina3()
{

    digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
    //Open or close  All Pie Panels, each command will trigger an open or close command
    
    Serial.print("Cantina Routine:");

    sendI2Ccmd("$14"); // sends an I2C command to Stealth folder 14 (cantina music)

     //.attach allows servos to operate
      Servos[P1].attach(P1_SERVO_PIN); 
      Servos[P2].attach(P2_SERVO_PIN);
      Servos[P3].attach(P3_SERVO_PIN);
      Servos[P4].attach(P4_SERVO_PIN);

      sendI2Ccmd("tmprnd=60");
  delay(100);
      
     for (int i=0; i<4; i++) {   //Loop  6  times (i<6;)Increase or decrease number to change looped times
 
     
       // open lows in pairs
       
       

       delay (480);
       
       Servos[P1].write(750,FASTSPEED);
       Servos[P2].write(750,FASTSPEED);
       Servos[P3].write(2000,FASTSPEED);
       Servos[P4].write(2000,FASTSPEED);

       delay (480);
       
       Servos[P1].write(2000,FASTSPEED);
       Servos[P2].write(2000,FASTSPEED);
       Servos[P3].write(750,FASTSPEED);
       Servos[P4].write(750,FASTSPEED);

      }



    Wire.beginTransmission(20); // transmit to device #20
     Wire.write(13);  // 
     Wire.endTransmission();    // stop transmitting

     
      for (int i=0; i<4; i++) {   //Loop  6  times (i<6;)Increase or decrease number to change looped times
 
     
       // open lows in pairs
       
       

       delay (480);
       
       Servos[P1].write(750,FASTSPEED);
       Servos[P3].write(750,FASTSPEED);
       Servos[P2].write(2000,FASTSPEED);
       Servos[P4].write(2000,FASTSPEED);

       delay (480);
       
       Servos[P1].write(2000,FASTSPEED);
       Servos[P3].write(2000,FASTSPEED);
       Servos[P2].write(750,FASTSPEED);
       Servos[P4].write(750,FASTSPEED);

      }

      for (int i=0; i<3; i++) {   //Loop  6  times (i<6;)Increase or decrease number to change looped times
 
     
       // open lows in pairs
       
       

       delay (480);
       
       Servos[P1].write(750,FASTSPEED);
       Servos[P4].write(750,FASTSPEED);
       Servos[P2].write(2000,FASTSPEED);
       Servos[P3].write(2000,FASTSPEED);

       delay (480);
       
       Servos[P1].write(2000,FASTSPEED);
       Servos[P4].write(2000,FASTSPEED);
       Servos[P2].write(750,FASTSPEED);
       Servos[P3].write(750,FASTSPEED);

      }

   delay (480);
   Servos[P1].write(750,FASTSPEED);
       Servos[P4].write(750,FASTSPEED);
       Servos[P2].write(2000,FASTSPEED);
       Servos[P3].write(750,FASTSPEED);

       delay (480);
   Servos[P1].write(2000,FASTSPEED);
       Servos[P4].write(750,FASTSPEED);
       Servos[P2].write(750,FASTSPEED);
       Servos[P3].write(750,FASTSPEED);

       delay (480);
   Servos[P1].write(750,FASTSPEED);
       Servos[P4].write(750,FASTSPEED);
       Servos[P2].write(750,FASTSPEED);
       Servos[P3].write(200,FASTSPEED);

       delay (480);
   Servos[P1].write(750,FASTSPEED);
       Servos[P4].write(2000,FASTSPEED);
       Servos[P2].write(750,FASTSPEED);
       Servos[P3].write(750,FASTSPEED);


       
 Wire.beginTransmission(20); // transmit to device #20
     Wire.write(14);  // 
     Wire.endTransmission();    // stop transmitting


   // Do Final Close

  
   
     delay (480);
   
   // This is a "for" loop, see Arduino site for ref.  Basically it allows you to repeat things between {curly braces} as many times as you want
    for (int i=0; i<2; i++) {   //Loop 2 times (i<2;)Increase or decrease number to change looped times
  
    // open lows in wave
       
    Servos[P1].write(1400,FASTSPEED,true);
    Servos[P2].write(1400,FASTSPEED,true);
    Servos[P3].write(1400,FASTSPEED,true);
    Servos[P4].write(1400,FASTSPEED,true);
          
    // close lows in opposite wave
       
    Servos[P1].write(750,FASTSPEED,true);
    Servos[P2].write(750,FASTSPEED,true);
    Servos[P3].write(750,FASTSPEED,true);
    Servos[P4].write(750,FASTSPEED,true);   
    }



  // Detatch Servos
  delay(480);
  Servos[P1].detach();
      Servos[P2].detach();
      Servos[P3].detach();
      Servos[P4].detach();
  
   

  i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
    digitalWrite(STATUS_LED, LOW);

}



//////////////////END CANTINA3/////////////////




void RotateScanner(){

 digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board     
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
 
 Servos[Scanner1].write(2500,25);




 delay(9000);
      
 Servos[Scanner1].detach();

 i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
 digitalWrite(STATUS_LED, LOW);

  
}









//RESET SERVOS, HOLOS, MAGIC PANEL  ================================================================================================

void resetServos () {  //Sends Close Command to All Dome Panels

      digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board 
   
      Serial.println("Reset Dome Panels/Holos");

      // Attach
      
      Servos[P1].attach(P1_SERVO_PIN);
      Servos[P2].attach(P2_SERVO_PIN);
      Servos[P3].attach(P3_SERVO_PIN);
      Servos[P4].attach(P4_SERVO_PIN);

      
      // Close
      
     
     
      // Write low panels 
      Servos[P1].write(PANEL_CLOSE,SPEED);
      Servos[P2].write(PANEL_CLOSE,SPEED);
      Servos[P3].write(PANEL_CLOSE,SPEED);
      Servos[P4].write(PANEL_CLOSE,SPEED);

      

      delay(1000); // wait for servos to close
      // Detach servos

      
      Servos[P1].detach();
      Servos[P2].detach();
      Servos[P3].detach();
      Servos[P4].detach();
      
     Serial.print("Dome Panels Closed,Reset");

     i2cCommand=-1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
     digitalWrite(STATUS_LED, LOW);
    
}

    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////Main Loop////////////

//  Use Stealth Config.txt file on USB to trigger I2C and specific "Switch/Case" below for routine. See Stealth Manual for config info
//  Example for Stealth: Button: b=1,4,10,2 (Button One, I2C, Expander Address 10, Case 2 (OpenClose Pies))
//  Example for Stealth: Gesture: g=2,4,10,4 (Gesture Up once, I2C, Expander Address 10, Case 4 (OpenCloseAll))

void loop() {

     delay(50);
  loopTime = millis(); 
  

 // Check for new i2c command
  switch(i2cCommand) {
    
   case 1: // RESET can be triggered also using this case number, will reset and close see above 
          resetServos();
          break; // break used to end each case

   case 3: 
          OpenCloseLow();  
          break;

   case 4:
          HelloAll();
          break;

   case 5:
          Bubble();
          break;


   case 6:
          LCDScreenDeathStar();
          break;
          

   case 7:
          LCDScreenLeiaMessage();
          break;

   case 8:
          Fire();
          break;
          
    
   case 9:
          SingleWaveLow();
          break;

    
    case 12:
          Cantina3();
          break;

    case 13:
          LowDomePanel1Hello();
          break;

    case 14:
          HPMove();
          break;

    case 16:
          RotateScanner();
          break;

    
               
  default: // Catch All
    case 0: 
          digitalWrite(STATUS_LED, LOW);
          i2cCommand=-1;    
          break;
  }
 
}
