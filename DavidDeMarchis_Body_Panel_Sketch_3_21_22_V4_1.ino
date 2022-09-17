//-----------------------------------------------------------------------------------------------
// Servo Expander "Body Stealth Servo Expander" vc
// Added Flap BP 4/5  June 2020
// Added code for circular saw  June 2020
// Fixed bug regarding Data Panel/CBI lights turning off if both doors are open and then just one closes June 2020
// Added in routines to open/close only the upper utility arm to use as part of card dispenser routine (REMOVED THIS AS OF 3-21-22
// Removed Left Hand Long Body Panel from this Servo Expander (now in SE#2 as part of Gripper Arm Routine)
//--------------------------------------------------------------

#include <Wire.h>
#include <VarSpeedServo.h>
// cvg
// outgoing i2c Commands
String Packet;
int count = 0;



byte sum;
#define DESTI2C 0 //Main Stealth board I2C = 0

// incoming i2c command
int i2cCommand = 0;
#define MYI2ADDR 11      //I2C Address for BODY SERVO EXPANDER = 11

unsigned long loopTime; // Time variable

#define STATUS_LED 13


// Body panel names for our Arduino pins, USE THESE PHYSICAL PINS FOR SPECIFIC BODY SERVO
// Board Pins #2-#8.
//#define BP1_SERVO_PIN 2   //  BP1   Right hand long Body Panel         
#define BP2_SERVO_PIN 3   //  BP2   CBI Body Panel      
#define BP3_SERVO_PIN 4   //  BP3   Saw Panel        
#define BP4_SERVO_PIN 5   //  BP4   Small Middle body panel         
#define BP5_SERVO_PIN 6    //  BP5    Small Left hand body Panel 
#define BP6_SERVO_PIN 7    //  BP6    Data Panel Body Panel        
//#define BP7_SERVO_PIN 8    //  BP7    Left hand long body panel
#define BP8_SERVO_PIN 9   // BP8    Upper Utility Arm
#define BP9_SERVO_PIN 10   // BP9   Lower Utility Arm
const int relaypin=11;    //CBI and Data Panel Lights
const int relaypin2=12;  //Saw Blade Motor
int DOORSEQ = 1; // Variable to hold door sequence

//#define TriggerPinNumber 12   //can I remove this to add the relaypin for the saw?
#define SawBlade_Actuator_PIN 13


// Create an array of VarSpeedServo type, containing 5 elements/entries.
// Note: Arrays are zero indexed. See http://arduino.cc/en/Reference/array

#define NBR_SERVOS 10// Number of Servos (10)
VarSpeedServo Servos[NBR_SERVOS];

// Note: These reference internal numbering not physical pins for array use

//#define BP1 0    //  BP1   Right hand long Body Panel         
#define BP2 1    //  BP2   CBI Body Panel         
#define BP3 2    //  BP3   Long narrow Panel     
#define BP4 3    //  BP4  Small Middle Body Panel    
#define BP5 4    // BP5    Small Left hand body Panel 
#define BP6 5    // BP6    Data Panel Body Panel      
//#define BP7 6    // BP7    Left Hand Long Body Panel
#define BP8 7   //BP8 Upper Utility Arm
#define BP9 8   //  BP9  Lower Utility Arm
#define SawBlade_Actuator 9   // Saw Blade Actuator
#define LowerUAopen 1525
#define LowerUAclosed 2500
#define UpperUAopen 1600
#define UpperUAclosed 2400


//   My Servos(frettedlogic):  Yours will be different, see variables defined...
//  Servos in open position = 600ms ( 0 degrees )
//  Servos in closed position = 1900ms ( 180 degrees )

#define FIRST_SERVO_PIN 2 //First Arduino Pin for Servos

// Servo degrees may be different for other peoples dome servos, use positive and/or negative numbers as applicable (frettedlogic)

// Dome Panel Open/Close values, can use servo value typically 1000-2000ms or position 0-180 degress.
//#define PANEL_OPEN 600   //  (40)  0=open position, 600 us                          
//#define PANEL_CLOSE 1900   //   (170) 180=close position, 1900 us                        
//#define PANEL_HALFWAY 1400  // 90=midway point 1400ms, use for animations etc. 
//#define PANEL_PARTOPEN 1650  //Partially Open, approximately 1/4    

// Panel Speed Variables 0-255, higher number equals faster movement
#define SPEED 200
#define FASTSPEED 255
#define OPENSPEED 230
#define CLOSESPEED 230

// Variables so program knows where things are
boolean BodiesOpen = false;
//boolean BP1Open = false;
boolean BP2Open = false;
boolean BP3Open = false;
boolean BP4Open = false;
boolean BP5Open = false;
boolean BP6Open = false;
//boolean BP7Open = false;
boolean BP8BP9Open = false;
boolean OpenClose = false;
boolean relayopen = true;




//----------------------------------------------------------------------------
//  i2c Commands
//----------------------------------------------------------------------------
void sendI2Ccmd(String cmd) {

  sum = 0;
  Wire.beginTransmission(DESTI2C);
  for (int i = 0; i < cmd.length(); i++) {

    Wire.write(cmd[i]);
    sum += byte(cmd[i]);
  }
  Wire.write(sum);
  Wire.endTransmission();

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){

  Serial.begin(57600); // DEBUG

  Serial.println("Stealth Body Servo Expander - 2017");

  Serial.println("My i2c address: ");
  Serial.println(MYI2ADDR);
  Wire.begin(MYI2ADDR);                // Start I2C Bus as a Slave (Device Number 10)
  Wire.onReceive(receiveEvent); // register i2c receive event
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  pinMode(relaypin, OUTPUT);
  pinMode(relaypin2, OUTPUT);
   
    
  resetServos();

  
  Serial.println("Waiting for i2c command");
  i2cCommand = 0;

  
  // Activate and Center all the servos
  Serial.print("Activating Servos");
  
  //prepare card dispenser
//  pinMode(TriggerPinNumber, OUTPUT);
//  digitalWrite(TriggerPinNumber, HIGH);

}

//------------------------------------------------------
// receive Event
//----------------------------------------------------
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


/////////////////////////////////////////////
//Extend Saw Blade//////////////////////////
////////////////////////////////////////////

void Extend_Saw() {
digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board

  Serial.print("Saw:");

// open saw blade door

Servos[BP3].attach(BP3_SERVO_PIN);
Servos[BP3].write(1500, SPEED, true);

// extend saw blade

Servos[SawBlade_Actuator].attach(SawBlade_Actuator_PIN);
Servos[SawBlade_Actuator].write(1900, SPEED, true);


Servos[BP3].detach();
Servos[SawBlade_Actuator].detach();

i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
digitalWrite(STATUS_LED, LOW);
}



/////////////////////////////////////////////
//Retract Saw Blade//////////////////////////
////////////////////////////////////////////

void Retract_Saw() {
digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board

  Serial.print("Saw:");
 //Retract saw blade
 Servos[SawBlade_Actuator].attach(SawBlade_Actuator_PIN);
Servos[SawBlade_Actuator].write(996, SPEED, true);
delay (8200);  //delay 5.2 seconds before closing door

//Close door

Servos[BP3].attach(BP3_SERVO_PIN);
Servos[BP3].write(902, SPEED, true);

Servos[BP3].detach();
Servos[SawBlade_Actuator].detach();

i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
digitalWrite(STATUS_LED, LOW);
}




//////////////////////////////////////////////
////////Saw/////////Saw///////////////////////
//////////////////////////////////////////////

void Saw() {
digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board

  Serial.print("Saw:");

// open saw blade door

Servos[BP3].attach(BP3_SERVO_PIN);
Servos[BP3].write(1500, SPEED, true);

// extend saw blade

Servos[SawBlade_Actuator].attach(SawBlade_Actuator_PIN);
Servos[SawBlade_Actuator].write(1900, SPEED, true);

delay (5000);  //delay 5 seconds before starting saw
//start up saw and sound

digitalWrite (relaypin2, HIGH); // turn saw on
sendI2Ccmd("$15"); // sends an I2C command to Stealth folder sixteen (saw blade sound)

delay (7000);
//Turn off saw

digitalWrite (relaypin2, LOW); // turn saw off

delay (7000);  // delay 10 seconds before retracting
//Retract saw blade
Servos[SawBlade_Actuator].write(996, SPEED, true);

delay (8200);  //delay 5.2 seconds before closing door
//Close door
Servos[BP3].write(902, SPEED, true);

Servos[BP3].detach();
Servos[SawBlade_Actuator].detach();

i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
digitalWrite(STATUS_LED, LOW);


}




//////////////////////////////////////////////
//////////Dispense Card///////////////////////
//////////////////////////////////////////////

//void DispenseCard() {
//digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
//digitalWrite(TriggerPinNumber, LOW);

//Serial.print(9,1);

//i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
//digitalWrite(STATUS_LED, LOW);
//digitalWrite(TriggerPinNumber, HIGH);


//////////////////////////////////////////////
///another option if the above doesn't work///
//////////////////////////////////////////////
//Wire.beginTransmission(9); // transmit to device #9
//Wire.send(1); // sends 1
//Wire.endTransmission(); // stop transmitting


//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close All Body Panels////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenCloseAllBodyPanels() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
  //Open or close  All Body Panels, each command will trigger an open or close command

  Serial.print("Body Panels:");


    
  if (BodiesOpen) { // Close the Body Panels if BodiesOpen is true
    Serial.println("Closing");
    BodiesOpen = false;
  //  BP1Open=false;
    BP2Open=false;
    BP3Open=false;
    BP4Open=false;
    BP5Open=false;
    BP6Open=false;
  //  BP7Open=false;
    
    
    sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)

    //.attach allows servos to operate
    //closes panels, "true" statement allows servo to reach position before executing next .write

    //Decide which close routine to use

    if (DOORSEQ==1) {

    //Servos[BP1].attach(BP1_SERVO_PIN);
    //Servos[BP1].write(1510, SPEED, true); // the "true" uses servo postion to ensure full position prior to next .write line. Good for smooth sequence verses delays
    Servos[BP2].attach(BP2_SERVO_PIN);
    Servos[BP2].write(1900, SPEED, true);
    Servos[BP3].attach(BP3_SERVO_PIN);
    Servos[BP3].write(800, SPEED, true);
    Servos[BP4].attach(BP4_SERVO_PIN);
    Servos[BP4].write(2000, SPEED, true);
    Servos[BP5].attach(BP5_SERVO_PIN);
    Servos[BP5].write(1207, SPEED, true);
    Servos[BP6].attach(BP6_SERVO_PIN);
    Servos[BP6].write(2000, SPEED, true);
    //Servos[BP7].attach(BP7_SERVO_PIN);
    //Servos[BP7].write(2100, SPEED, true);
    delay(800);

    // .detach disables servos
    //Servos[BP1].detach();
    Servos[BP2].detach();
    Servos[BP3].detach();
    Servos[BP4].detach();
    Servos[BP5].detach();
    Servos[BP6].detach();
    //Servos[BP7].detach();

    digitalWrite (relaypin,LOW); // turn CBI and DataPanel lights off
    
    Serial.println("Closed");

    

    }

    if (DOORSEQ==2) {

    //Servos[BP1].attach(BP1_SERVO_PIN);
    //Servos[BP1].write(1510, SPEED); // the "true" uses servo postion to ensure full position prior to next .write line. Good for smooth sequence verses delays
    //Servos[BP7].attach(BP7_SERVO_PIN);
    //Servos[BP7].write(2100, SPEED, true);
    delay(500);
    Servos[BP2].attach(BP2_SERVO_PIN);
    Servos[BP2].write(1900, SPEED);
    Servos[BP6].attach(BP6_SERVO_PIN);
    Servos[BP6].write(2000, SPEED, true);
    delay(500);
    Servos[BP3].attach(BP3_SERVO_PIN);
    Servos[BP3].write(800, SPEED);
    Servos[BP4].attach(BP4_SERVO_PIN);
    Servos[BP4].write(2000, SPEED);
    Servos[BP5].attach(BP5_SERVO_PIN);
    Servos[BP5].write(1207, SPEED, true);
    
    
    delay(800);

    // .detach disables servos
    //Servos[BP1].detach();
    Servos[BP2].detach();
    Servos[BP3].detach();
    Servos[BP4].detach();
    Servos[BP5].detach();
    Servos[BP6].detach();
    //Servos[BP7].detach();

    digitalWrite (relaypin,LOW); // turn CBI and DataPanel lights off
    
    Serial.println("Closed");

    

    }

    if (DOORSEQ==3) {
    Servos[BP5].attach(BP5_SERVO_PIN);
    Servos[BP5].write(1207, SPEED, true);
    //Servos[BP1].attach(BP1_SERVO_PIN);
    //Servos[BP1].write(1510, SPEED, true); // the "true" uses servo postion to ensure full position prior to next .write line. Good for smooth sequence verses delays
    Servos[BP6].attach(BP6_SERVO_PIN);
    Servos[BP6].write(2000, SPEED, true);
    Servos[BP2].attach(BP2_SERVO_PIN);
    Servos[BP2].write(1900, SPEED, true);
    Servos[BP3].attach(BP3_SERVO_PIN);
    Servos[BP3].write(800, SPEED, true);
    //Servos[BP7].attach(BP7_SERVO_PIN);
    //Servos[BP7].write(2100, SPEED, true);
    Servos[BP4].attach(BP4_SERVO_PIN);
    Servos[BP4].write(2000, SPEED, true);
    
    
    
    delay(800);

    // .detach disables servos
    //Servos[BP1].detach();
    Servos[BP2].detach();
    Servos[BP3].detach();
    Servos[BP4].detach();
    Servos[BP5].detach();
    Servos[BP6].detach();
    //Servos[BP7].detach();

    digitalWrite (relaypin,LOW); // turn CBI and DataPanel lights off
    
    Serial.println("Closed");

    

    }

    if (DOORSEQ==4) {
    Servos[BP5].attach(BP5_SERVO_PIN);
    Servos[BP5].write(1207, SPEED, true);
    //Servos[BP1].attach(BP1_SERVO_PIN);
    //Servos[BP1].write(1510, SPEED, true); // the "true" uses servo postion to ensure full position prior to next .write line. Good for smooth sequence verses delays
    Servos[BP6].attach(BP6_SERVO_PIN);
    Servos[BP6].write(2000, SPEED, true);
    Servos[BP2].attach(BP2_SERVO_PIN);
    Servos[BP2].write(1900, SPEED, true);
    Servos[BP3].attach(BP3_SERVO_PIN);
    Servos[BP3].write(800, SPEED, true);
    //Servos[BP7].attach(BP7_SERVO_PIN);
    //Servos[BP7].write(2100, SPEED, true);
    Servos[BP4].attach(BP4_SERVO_PIN);
    Servos[BP4].write(2000, SPEED, true);
    
    
    
    delay(800);

    // .detach disables servos
    //Servos[BP1].detach();
    Servos[BP2].detach();
    Servos[BP3].detach();
    Servos[BP4].detach();
    Servos[BP5].detach();
    Servos[BP6].detach();
    //Servos[BP7].detach();

    digitalWrite (relaypin,LOW); // turn CBI and DataPanel lights off
    
    Serial.println("Closed");

    

    }




    
    
    } 
   else { //   Open Body Panels
    Serial.println("Opening Body Panels");
    BodiesOpen = true;
    //BP1Open = true;
    BP2Open = true;
    BP3Open = true;
    BP4Open = true;
    BP5Open = true;
    BP6Open = true;
    //BP7Open = true;
    
    sendI2Ccmd("$02");  // Stealth sound folder one (general sounds)

    
    //Decide which Open routine to use 


    DOORSEQ=random(1,5);    


   
    digitalWrite (relaypin, HIGH); // turn CBI and DataPanel lights on
 
  
    if (DOORSEQ==1) {
    
    //Servos[BP1].attach(BP1_SERVO_PIN);
    //Servos[BP1].write(2370, SPEED, true);
    Servos[BP2].attach(BP2_SERVO_PIN);
    Servos[BP2].write(1000, SPEED, true);
    Servos[BP3].attach(BP3_SERVO_PIN);
    Servos[BP3].write(1500, SPEED, true);
    Servos[BP4].attach(BP4_SERVO_PIN);
    Servos[BP4].write(1200, SPEED, true);
    Servos[BP5].attach(BP5_SERVO_PIN);
    Servos[BP5].write(2200, SPEED, true);
    Servos[BP6].attach(BP6_SERVO_PIN);
    Servos[BP6].write(1250, SPEED, true);
    //Servos[BP7].attach(BP7_SERVO_PIN);
    //Servos[BP7].write(1300, SPEED, true);

    delay(1000);

    // .detach disables servos
    //Servos[BP1].detach();
    Servos[BP2].detach();
    Servos[BP3].detach();
    Servos[BP4].detach();
    Servos[BP5].detach();
    Servos[BP6].detach();
    //Servos[BP7].detach();

  
  }

  else if (DOORSEQ==2) {

    //Servos[BP1].attach(BP1_SERVO_PIN);
    //Servos[BP1].write(2370, SPEED);
    //Servos[BP7].attach(BP7_SERVO_PIN);
    //Servos[BP7].write(1300, SPEED, true);
    delay(500);
    Servos[BP2].attach(BP2_SERVO_PIN);
    Servos[BP2].write(1000, SPEED);
    Servos[BP6].attach(BP6_SERVO_PIN);
    Servos[BP6].write(1250, SPEED, true);
    delay(500);
    Servos[BP3].attach(BP3_SERVO_PIN);
    Servos[BP3].write(1500, SPEED);
    Servos[BP4].attach(BP4_SERVO_PIN);
    Servos[BP4].write(1200, SPEED);
    Servos[BP5].attach(BP5_SERVO_PIN);
    Servos[BP5].write(2200, SPEED, true);
   
    delay(1000);

    // .detach disables servos
    //Servos[BP1].detach();
    Servos[BP2].detach();
    Servos[BP3].detach();
    Servos[BP4].detach();
    Servos[BP5].detach();
    Servos[BP6].detach();
    //Servos[BP7].detach();

    

  }

  else if (DOORSEQ==3) {

    Servos[BP5].attach(BP5_SERVO_PIN);
    Servos[BP5].write(2200, SPEED, true);
    //Servos[BP1].attach(BP1_SERVO_PIN);
    //Servos[BP1].write(2370,SPEED, true);
    Servos[BP6].attach(BP6_SERVO_PIN);
    Servos[BP6].write(1250, SPEED, true);
    Servos[BP2].attach(BP2_SERVO_PIN);
    Servos[BP2].write(1000, SPEED, true);
    Servos[BP3].attach(BP3_SERVO_PIN);
    Servos[BP3].write(1500, SPEED, true);
    //Servos[BP7].attach(BP7_SERVO_PIN);
    //Servos[BP7].write(1300, SPEED, true);
    Servos[BP4].attach(BP4_SERVO_PIN);
    Servos[BP4].write(1200, SPEED, true);
    
    
    

    delay(1000);

    // .detach disables servos
    //Servos[BP1].detach();
    Servos[BP2].detach();
    Servos[BP3].detach();
    Servos[BP4].detach();
    Servos[BP5].detach();
    Servos[BP6].detach();
    //Servos[BP7].detach();
    
  }



 else if (DOORSEQ==4) {

    
    //Servos[BP1].attach(BP1_SERVO_PIN);
    //Servos[BP1].write(2370, SPEED, true);
    Servos[BP6].attach(BP6_SERVO_PIN);
    Servos[BP6].write(1250, SPEED, true);
    Servos[BP2].attach(BP2_SERVO_PIN);
    Servos[BP2].write(1000, SPEED, true);
    Servos[BP3].attach(BP3_SERVO_PIN);
    Servos[BP3].write(1500, SPEED, true);
    //Servos[BP7].attach(BP7_SERVO_PIN);
    //Servos[BP7].write(1300, SPEED, true);

    Servos[BP5].attach(BP5_SERVO_PIN);
    Servos[BP5].write(2200, FASTSPEED, true);
    Servos[BP4].attach(BP4_SERVO_PIN);
    Servos[BP4].write(1200, FASTSPEED, true);
    
    Servos[BP5].write(1207, FASTSPEED, true);
    
    Servos[BP4].write(2000, FASTSPEED, true);
    
    Servos[BP5].write(1800, FASTSPEED, true);
    
    Servos[BP4].write(1200, FASTSPEED, true);
    
    Servos[BP5].write(1207, FASTSPEED, true);
    
    Servos[BP4].write(2000, FASTSPEED, true);
    
    Servos[BP5].write(1800, FASTSPEED, true);
    
    Servos[BP4].write(1200, FASTSPEED, true);
        
    
    
    

    delay(1000);

    // .detach disables servos
    //Servos[BP1].detach();
    Servos[BP2].detach();
    Servos[BP3].detach();
    Servos[BP4].detach();
    Servos[BP5].detach();
    Servos[BP6].detach();
    //Servos[BP7].detach();
    
  }
  
  }

  Serial.println("Opened Body Panels");


  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

}



/////////////////////////////////////////////////////////
////Flap BP 4 and BP 5//////////////////////////////////
////////////////////////////////////////////////////////

void Flap45 () {


digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
  

  Serial.print("Flap 4 and 5:");


    
    
    sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)

    //.attach allows servos to operate

  
    Servos[BP5].attach(BP5_SERVO_PIN);
    Servos[BP5].write(2000, FASTSPEED, true);
    Servos[BP4].attach(BP4_SERVO_PIN);
    Servos[BP4].write(1200, FASTSPEED, true);

   for (int i = 0; i < 6; i++) {

    Servos[BP5].write(1207, FASTSPEED, true);
    Servos[BP4].write(2000, FASTSPEED, true);
    Servos[BP5].write(1900, FASTSPEED, true);
    Servos[BP4].write(1200, FASTSPEED, true);
    
   }
    Servos[BP5].write(1207, FASTSPEED, true);
    Servos[BP4].write(2000, FASTSPEED, true);
    delay (1000);
    Servos[BP4].detach();
    Servos[BP5].detach();


  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

}

/////////////////////////////////////////////////////
//////End Flap45////////////////////////////////////
////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close Body Panel 1////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void OpenCloseBodyPanel1() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide

  //digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
  //Open or close  All Body Panels, each command will trigger an open or close command

  //Serial.print("Body Panel 1:");

  //if (BP1Open) { // Close the Body Panel if BP1Open is true
    //Serial.println("Closing");

    //BP1Open = false;


    //.attach allows servos to operate

    //Servos[BP1].attach(BP1_SERVO_PIN);

    //Servos[BP1].write(1510, SPEED);


    //delay(800);
    // .detach disables servos

    //Servos[BP1].detach();

    //Serial.println("Closed");

  //} else { // Open Body Panel 1
    //Serial.println("Opening Body Panel 1");

    //BP1Open = true;


    //Servos[BP1].attach(BP1_SERVO_PIN);



  
  //Servos[BP1].write(2370, SPEED);



    //delay(1000);



    //Servos[BP1].detach();

  //}

  //Serial.println("Opened BP1");

  //i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  //digitalWrite(STATUS_LED, LOW);

//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close Body Panel 2////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenCloseBodyPanel2() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
  //Open or close  All Body Panels, each command will trigger an open or close command

  Serial.print("Body Panel 2:");

  if (BP2Open) { // Close the Body Panel if BP2Open is true
    Serial.println("Closing");

    BP2Open = false;


    //.attach allows servos to operate

    Servos[BP2].attach(BP2_SERVO_PIN);

    Servos[BP2].write(1900, SPEED);

    sendI2Ccmd("$02");
    delay(800);
    // .detach disables servos

    Servos[BP2].detach();


    //check to see if Data Panel door is still open...if it is, don't turn off relay so that data panel lights will remain on
    if (BP6Open) {}
    else{

    digitalWrite (relaypin, LOW); // turn CBI and DataPanel lights off
    }
    Serial.println("Closed");

  } else { // Open Body Panel 2
    Serial.println("Opening Body Panel 2");

    BP2Open = true;

    digitalWrite (relaypin, HIGH); // turn CBI and DataPanel lights on


    Servos[BP2].attach(BP2_SERVO_PIN);



    Servos[BP2].write(1000, SPEED);



    delay(1000);



    Servos[BP2].detach();

  }

  Serial.println("Opened BP2");

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close Body Panel 3////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenCloseBodyPanel3() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
  //Open or close  All Body Panels, each command will trigger an open or close command

  Serial.print("Body Panel 3:");

  if (BP3Open) { // Close the Body Panel if BP3Open is true
    Serial.println("Closing");

    BP3Open = false;


    //.attach allows servos to operate

    Servos[BP3].attach(BP3_SERVO_PIN);

    Servos[BP3].write(800, SPEED);


    delay(800);
    // .detach disables servos

    Servos[BP3].detach();

    Serial.println("Closed");

  } else { // Open Body Panel 3
    Serial.println("Opening Body Panel 3");

    BP3Open = true;


    Servos[BP3].attach(BP3_SERVO_PIN);



    Servos[BP3].write(1500, SPEED);



    delay(1000);



    Servos[BP3].detach();

  }

  Serial.println("Opened BP3");

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close Body Panel 4////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenCloseBodyPanel4() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
  //Open or close  All Body Panels, each command will trigger an open or close command

  Serial.print("Body Panel 4:");

  if (BP4Open) { // Close the Body Panel if BP4Open is true
    Serial.println("Closing");

    BP4Open = false;


    //.attach allows servos to operate

    Servos[BP4].attach(BP4_SERVO_PIN);

    Servos[BP4].write(2000, SPEED);


    delay(800);
    // .detach disables servos

    Servos[BP4].detach();

    Serial.println("Closed");

  } else { // Open Body Panel 4
    Serial.println("Opening Body Panel 4");

    BP4Open = true;


    Servos[BP4].attach(BP4_SERVO_PIN);



    Servos[BP4].write(1200, SPEED);



    delay(1000);



    Servos[BP4].detach();

  }

  Serial.println("Opened BP4");

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close Body Panel 5////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenCloseBodyPanel5() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
  //Open or close  All Body Panels, each command will trigger an open or close command

  Serial.print("Body Panel 5:");

  if (BP5Open) { // Close the Body Panel if BP5Open is true
    Serial.println("Closing");

    BP5Open = false;


    //.attach allows servos to operate

    Servos[BP5].attach(BP5_SERVO_PIN);

    Servos[BP5].write(1207, SPEED);


    delay(800);
    // .detach disables servos

    Servos[BP5].detach();

    Serial.println("Closed");

  } else { // Open Body Panel 5
    Serial.println("Opening Body Panel 5");

    BP5Open = true;


    Servos[BP5].attach(BP5_SERVO_PIN);



    Servos[BP5].write(2200, SPEED);



    delay(1000);



    Servos[BP5].detach();

  }

  Serial.println("Opened BP5");

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close Body Panel 6////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenCloseBodyPanel6() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
  //Open or close  All Body Panels, each command will trigger an open or close command

  Serial.print("Body Panel 6:");

  if (BP6Open) { // Close the Body Panel if BP6Open is true
    Serial.println("Closing");

    BP6Open = false;


    //.attach allows servos to operate

    Servos[BP6].attach(BP6_SERVO_PIN);

    Servos[BP6].write(2000, SPEED);


    delay(800);
    // .detach disables servos

    Servos[BP6].detach();

    Serial.println("Closed");

    //check to see if Charge Bay  door is still open...if it is, don't turn off relay so that Charge Bay lights will remain on
    if (BP2Open) {}
    else{

    digitalWrite (relaypin, LOW); // turn CBI and DataPanel lights off
    }

    

  } else { // Open Body Panel 6
    Serial.println("Opening Body Panel 6");

    BP6Open = true;

    digitalWrite (relaypin, HIGH); // turn CBI and DataPanel lights on


    Servos[BP6].attach(BP6_SERVO_PIN);



    Servos[BP6].write(1250, SPEED);



    delay(1000);



    Servos[BP6].detach();

  }

  Serial.println("Opened BP6");

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close Body Panel 7////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void OpenCloseBodyPanel7() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide

//  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
  //Open or close  All Body Panels, each command will trigger an open or close command

//  Serial.print("Body Panel 7:");

//  if (BP7Open) { // Close the Body Panel if BP7Open is true
//    Serial.println("Closing");

//    BP7Open = false;


    //.attach allows servos to operate

//    Servos[BP7].attach(BP7_SERVO_PIN);

//    Servos[BP7].write(2100, SPEED);


//    delay(800);
    // .detach disables servos

//    Servos[BP7].detach();

//    Serial.println("Closed");

//  } else { // Open Body Panel 7
//    Serial.println("Opening Body Panel 7");

//    BP7Open = true;


//    Servos[BP7].attach(BP7_SERVO_PIN);



//    Servos[BP7].write(1300, SPEED);



//    delay(1000);



//    Servos[BP7].detach();

//  }

//  Serial.println("Opened BP7");

//  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
//  digitalWrite(STATUS_LED, LOW);

//}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Turn Off/On Relay  ////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TurnRelayOnOff (){

digitalWrite(STATUS_LED, HIGH);

// if (relayopen) {
  digitalWrite (relaypin, HIGH);
//  relayopen = false;
// }
  delay(2000);
// else {
  digitalWrite (relaypin, LOW);
//  relayopen = true;
  delay(2000);
// }
 i2cCommand = -1;
 digitalWrite(STATUS_LED, LOW);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////Open Upper Utility Arm////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenUpperUtilityArm(){

digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
 
  Serial.print("Open Upper Utility Arm:");

  Servos[BP8].attach(BP8_SERVO_PIN);
  Servos[BP8].write(UpperUAopen, SPEED); //upper Utility Arm

  delay (1000);
  
  Servos[BP8].detach();

digitalWrite(STATUS_LED, LOW); // turn on STATUS LED so we can visually see we got the command on the board
 
  
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////Close Upper Utility Arm////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CloseUpperUtilityArm(){

digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
 
  Serial.print("Close Upper Utility Arm:");

  Servos[BP8].attach(BP8_SERVO_PIN);
  Servos[BP8].write(UpperUAclosed, SPEED); //upper Utility Arm

  delay (1000);
  
  Servos[BP8].detach();

digitalWrite(STATUS_LED, LOW); // turn on STATUS LED so we can visually see we got the command on the board
 
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close Utility Arms////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenCloseUtilityArms() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
 
  Serial.print("Utility Arms:");

  if (BP8BP9Open) { // Close the Utility Arm if BP8BP9Open is true
    Serial.println("Closing");

    BP8BP9Open = false;


    //attach allows servos to operate
    
    Servos[BP9].attach(BP9_SERVO_PIN);

    Servos[BP9].write(LowerUAclosed, SPEED);  //lower Utility Arm

    Servos[BP8].attach(BP8_SERVO_PIN);

    Servos[BP8].write(UpperUAclosed, SPEED); //upper Utility Arm

    delay(800);
     //detach disables servos

    Servos[BP8].detach();
    Servos[BP9].detach();
Serial.println("Closed");

  } //end if
  
  else { // Open Both Utility Arms
    Serial.println("Opening Utility Arms");

    BP8BP9Open = true;


    
Servos[BP9].attach(BP9_SERVO_PIN);

    Servos[BP9].write(LowerUAopen, SPEED);  //lower Utility Arm


Servos[BP8].attach(BP8_SERVO_PIN);

    Servos[BP8].write(UpperUAopen, SPEED); //upper Utility Arm

    delay(1000);


    
    Servos[BP9].detach();
  } //end else

  Serial.println("Opened BP8BP9");

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
digitalWrite(STATUS_LED, LOW);

}  // end routine



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open and Close Body Panel 2 for Zapper Arm Routine////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenCloseZapperArmDoor() {  // Note: This may seem backwards but the Close command ("if") is first and then the Open ("else")second, see Arduino reference guide

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board
  
  Serial.print("Body Panel 2:");
  //Open Zapper Door
  Servos[BP2].attach(BP2_SERVO_PIN);
  Servos[BP2].write(1000, SPEED);

  //  sendI2Ccmd("$02");
    delay(800);
  
    Servos[BP2].detach();

    delay (1000);

///////////////////////////////////////////////////////////////////////////////
// Make Call to Servo Expander #2 to extend zapper and to ignite///////////////
////////////////////////////////////////////////////////////////////////////////
            Wire.beginTransmission(12); // transmit to device #12
            Wire.write(3);  // Routine 3 extends zapper arm, fires it, then retracts it
            Wire.endTransmission();    // stop transmitting
///////////////////////////////////////////////////////////////////////////////
// End Call to Servo Expander #2 to extend zapper and to ignite///////////////
////////////////////////////////////////////////////////////////////////////////

    delay (12000);

  //Close Zapper Door
    Servos[BP2].attach(BP2_SERVO_PIN);
    Servos[BP2].write(1900, SPEED);

    delay(1000);

    Servos[BP2].detach();

  


  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RESET SERVOS, HOLOS, MAGIC PANEL  ================================================================================================
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void resetServos() {  //Sends Close Command to All Dome Panels

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board

  Serial.println("Reset Body panels");

  // Attach
  // Close
  //Servos[BP1].attach(BP1_SERVO_PIN);
 //Servos[BP1].write(1510, SPEED);
  Servos[BP2].attach(BP2_SERVO_PIN);
  Servos[BP2].write(1900, SPEED);
  Servos[BP3].attach(BP3_SERVO_PIN);
  Servos[BP3].write(800, SPEED);
  Servos[BP4].attach(BP4_SERVO_PIN);
  Servos[BP4].write(2000, SPEED);
  Servos[BP5].attach(BP5_SERVO_PIN);
  Servos[BP5].write(1207, SPEED);
  Servos[BP6].attach(BP6_SERVO_PIN);
  Servos[BP6].write(2000, SPEED);
  //Servos[BP7].attach(BP7_SERVO_PIN);
  //Servos[BP7].write(2100, SPEED);


  delay(1000); // wait for servos to close

  // Detach servos

 //Servos[BP1].detach();
  Servos[BP2].detach();
  Servos[BP3].detach();
  Servos[BP4].detach();
  Servos[BP5].detach();
  Servos[BP6].detach();
  //Servos[BP7].detach();

  Serial.print("Body Panels Closed,Reset");

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////
/////Main Loop////////////
///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  Use Stealth Config.txt file on USB to trigger I2C and specific "Switch/Case" below for routine. See Stealth Manual for config info
//  Example for Stealth: Button: b=1,4,9,2 (Button One, I2C, Expander Address 9, Case 2 (OpenClose Body Panels))
//  Example for Stealth: Gesture: g=2,4,9,3 (Gesture Up once, I2C, Expander Address 9, Case 4 (OpenClose Body Panel 6))

void loop() {

  delay(50);
  loopTime = millis();


  // Check for new i2c command
  switch (i2cCommand) {

    case 1: // RESET can be triggered also using this case number, will reset and close see above
      resetServos();
      digitalWrite(STATUS_LED, HIGH);
      i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
      break;

    case 2:
      OpenCloseAllBodyPanels();
      break;

    //case 3:
    //  OpenCloseBodyPanel1();
     // break;

    case 4:
      OpenCloseBodyPanel2();
      break;

    case 5:
      OpenCloseBodyPanel3();
      break;

    case 6:
      OpenCloseBodyPanel4();
      break;

    case 7:
      OpenCloseBodyPanel5();
      break;

    case 8:
      OpenCloseBodyPanel6();
      break;

    //case 9:
     // OpenCloseBodyPanel7();
     // break;

   case 10:
     OpenCloseUtilityArms();
     break;    

    
  //  case 12:
  //    DispenseCard();
  //    break;
    
    case 13:
      Saw();
      break;

    case 14:
        Flap45();
        break;

    case 15:
        OpenCloseZapperArmDoor();
        break;

    case 16:
        OpenUpperUtilityArm();
        break;

    case 17:
        CloseUpperUtilityArm();
        break;


    case 18:  
        Extend_Saw();
        break;

    case 19:
        Retract_Saw();
        break;

        
    default: // Catch All
    case 0:
      digitalWrite(STATUS_LED, LOW);
      i2cCommand = -1;
      break;
  }

}
