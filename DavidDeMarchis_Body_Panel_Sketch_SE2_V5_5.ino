//-----------------------------------------------------------------------------------------------
// Servo Expander "Body Stealth Servo Expander #2"
//
// Base Code: John Thompson (FrettedLogic) and Chris James
// Customized by David DeMarchis
// Routine for Toy R2 Arm and body panel.....changed to Long Right Hand Body Panel  March 2022
// New Routine for Gripper Arm.....Long Left Hand Body Panel added to this Servo Expander, for Gripper Arm
// Now includes Zapper arm routine (begins with panel opening from Servo Expander #1)....updated as of 6/10/22...removed real zap, changed to LED zap. 
// Now includes Card Dispenser Code  
// Updated Card Dispenser for timing issues....now kills power to card dispenser after xx seconds so that belt is properly positioned for next card...DOESNT WORK YET
// -------------------------------------------------------------

#include <Wire.h>
#include <VarSpeedServo.h>
#include <Stepper.h>
#include <LedControl.h>

// outgoing i2c Commands
String Packet;
int count = 0;

byte sum;
#define DESTI2C 0 //Main Stealth board I2C = 0

// incoming i2c command
int i2cCommand = 0;
#define MYI2ADDR 12     //I2C Address for BODY SERVO EXPANDER = 12

unsigned long loopTime; // Time variable

#define STATUS_LED 13


// Body panel names for our Arduino pins, USE THESE PHYSICAL PINS FOR SPECIFIC BODY SERVO
// Board Pins #2-#8.
#define BP1_SERVO_PIN 2   //  BP1   Right hand long Body Panel         
#define TOY_SERVO_PIN 3   //  TR2   Toy R2 Arm   
#define BP7_SERVO_PIN 4    //  BP7    Left hand long body panel
#define Gripper_Actuator 12  // 
#define Grip_Pinch 13 //
//const int cutcarddispenserpower = 4; //relay needed to cut power to card dispenser to align nub properly for next card
const int cutdomemotorpower = 9; // relay needed to cut dome motor power during zap
const int zapperpin = 10;  // spark for zapper

boolean GripperArmDoorOpen = false;




// Create an array of VarSpeedServo type, containing 2 elements/entries.
// Note: Arrays are zero indexed. See http://arduino.cc/en/Reference/array

#define NBR_SERVOS 13// Number of Servos (13)
VarSpeedServo Servos[NBR_SERVOS];

// Note: These reference internal numbering not physical pins for array use

#define BP1 0    //  BP1   Left hand long Body Panel    
#define TOY_SERVO 1   // TOY_SERVO   
#define BP7 2
#define GripperActuator 3
#define PinchServo 4
#define enA 9
#define in1 6
#define in2 7


#define FIRST_SERVO_PIN 2 //First Arduino Pin for Servos


// Panel Speed Variables 0-255, higher number equals faster movement
#define SPEED 200
#define FASTSPEED 255


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
void setup() {

  Serial.begin(57600); // DEBUG

  Serial.println("Stealth Body Servo Expander #2 - 2017");

  Serial.println("My i2c address: ");
  Serial.println(MYI2ADDR);
  Wire.begin(MYI2ADDR);                // Start I2C Bus as a Slave (Device Number 12)
  Wire.onReceive(receiveEvent); // register i2c receive event
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  analogWrite(enA, 100); // Send PWM signal to L298N Enable pin
  pinMode(zapperpin, OUTPUT);
  pinMode(cutdomemotorpower, OUTPUT);
//  pinMode (cutcarddispenserpower, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  // Sets the two pins as Outputs for Life Form Scanner
  // pinMode(stepPin,OUTPUT);
  //pinMode(dirPin,OUTPUT);

  //pinMode(enPin,OUTPUT);
  //digitalWrite(enPin,LOW);

  //prepare card dispenser
 // pinMode(CardTriggerPinNumber, OUTPUT);
 // digitalWrite(CardTriggerPinNumber, HIGH);


  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  resetServos();


  Serial.println("Waiting for i2c command");
  i2cCommand = 0;

  // Activate and Center all the servos
  Serial.print("Activating Servos");

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
////OpenClose Gripper Arm Door///////////////////
////////////////////////////////////////////

void OpenCloseGripperArmDoor(){

digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board

  Serial.print("Gripper Arm:");

  sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)

  // open Gripper Arm Door


  if (GripperArmDoorOpen) { // Close the Body Panel if GripperArmDoorOpen is true
    Serial.println("Closing");

    GripperArmDoorOpen = false;


  Servos[BP7].attach(BP7_SERVO_PIN);
  Servos[BP7].write(1960, SPEED);
  delay(1000);
  Servos[BP7].detach();

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  }

  else
  {

GripperArmDoorOpen = true;


  Servos[BP7].attach(BP7_SERVO_PIN);
  Servos[BP7].write(1100, SPEED);
  delay(1000);
  Servos[BP7].detach();

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command


  }
  
  digitalWrite(STATUS_LED, LOW);
  
}


//////////////////////////////////////////////
////////Gripper Arm///////////////////////////
//////////////////////////////////////////////

void GripperArm(){

digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board

  Serial.print("Gripper Arm:");

  sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)

  // open Gripper Arm Door

  Servos[BP7].attach(BP7_SERVO_PIN);
  Servos[BP7].write(1100, SPEED);
  delay(1200);
  Servos[BP7].detach();

  // Raise Gripper Arm
  
  Servos[GripperActuator].attach(Gripper_Actuator);
  Servos[GripperActuator].write(1400, 125);
  delay(5000);
  Servos[GripperActuator].detach();


 
  // Pinch several times

  Servos[PinchServo].attach(Grip_Pinch);
  Servos[PinchServo].write(2000, FASTSPEED);
  delay(300);
  Servos[PinchServo].write(2500, FASTSPEED);
  delay(300);
  Servos[PinchServo].write(2000, FASTSPEED);
  delay(300);
  Servos[PinchServo].write(2500, FASTSPEED);
  delay(300);
  Servos[PinchServo].write(2000, FASTSPEED);
  delay(1500);
  Servos[PinchServo].write(2500, FASTSPEED);
  delay(300);
  Servos[PinchServo].write(2000, FASTSPEED);
  delay(300);
  Servos[PinchServo].write(2500, FASTSPEED);
  delay(300);
  Servos[PinchServo].detach();
  

  
  
  // Lower Gripper Arm
  
  Servos[GripperActuator].attach(Gripper_Actuator);
  Servos[GripperActuator].write(2480, 125);
  delay(8000);
  Servos[GripperActuator].detach();

  //close Gripper Arm Door
   Servos[BP7].attach(BP7_SERVO_PIN);
  Servos[BP7].write(1900, SPEED);
  delay(1000);
  Servos[BP7].detach();

  


  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

  sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)


}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////
//////////Dispense Card///////////////////////
//////////////////////////////////////////////

void DispenseCard() {
  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board


  //open upper utility arm
  Wire.beginTransmission(11); // transmit to device #11
  Wire.write(16);  // Routine 16 opens upper utility arm
  Wire.endTransmission();    // stop transmitting

  delay(2000);

  //dispense card
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(1);
  Wire.endTransmission(); // stop transmitting

  //delay for just the right amount of time for proper nub alignment before shutting off power
  delay(1000);
//digitalWrite (cutcarddispenserpower, HIGH);  //Cut Card Dispenser Power
  delay (250);
//  digitalWrite (cutcarddispenserpower, LOW);  //Restore Card Dispenser Power

  delay(3000);  //delay so that person will take card before arm closes....should be 15000 to 20000 normally

  //close upper utility arm
  Wire.beginTransmission(11); // transmit to device #11
  Wire.write(17);  // 
  Wire.endTransmission();    // stop transmitting
  delay(2000);

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

}



//////////////////////////////////////////////
////////Give Toy//////////////////////////////
//////////////////////////////////////////////

void GiveToy() {
  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board

  Serial.print("GiveToy:");

  sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)

  // open toy door

  Servos[BP1].attach(BP1_SERVO_PIN);
  Servos[BP1].write(2500, SPEED);
  delay(1000);

  // extend toy arm

  Servos[TOY_SERVO].attach(TOY_SERVO_PIN);
  Servos[TOY_SERVO].write(2010, SPEED);


  sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)

  // wait 10 seconds for kid to take toy

  delay(9999);

  // retract arm, then close door
  Servos[TOY_SERVO].write(983, SPEED);
  delay(5000);

  //close toy door
  Servos[BP1].write(1450, SPEED);
  delay(2000);

  Servos[BP1].detach();
  Servos[TOY_SERVO].detach();


  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

  sendI2Ccmd("$02"); // sends an I2C command to Stealth folder two (chat sounds)


}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
/////extend and zap zapper/////////////////////////////////
///////////////////////////////////////////////////////////

void Extend_and_zap() {

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board


  //Extend Arm for 2.5 seconds
  analogWrite(enA, 100); // Send PWM signal to L298N Enable pin
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  delay(2500);

  //Stop Extending Arm
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  //cut power to dome motor
  //digitalWrite (cutdomemotorpower, HIGH);  //Cut Dome Motor Power
  //delay (250);

  //NOW ZAP 3 TIMES//
  sendI2Ccmd("$18"); // sends an I2C command to Soundbank #18 in Stealth folder (zap sound effect)
  delay(200);
  digitalWrite (zapperpin, HIGH); // turn zapper on
  //sendI2Ccmd("$18"); // sends an I2C command to Soundbank #18 in Stealth folder (zap sound effect)
  delay (800);
  digitalWrite (zapperpin, LOW); // turn zapper off
  delay (500);
  sendI2Ccmd("$18"); // sends an I2C command to Soundbank #18 in Stealth folder (zap sound effect)
  delay(200);
  digitalWrite (zapperpin, HIGH); // turn zapper on
  //sendI2Ccmd("$18"); // sends an I2C command to Soundbank #18 in Stealth folder (zap sound effect)
  delay (800);
  digitalWrite (zapperpin, LOW); // turn zapper off
  delay (500);
  sendI2Ccmd("$18"); // sends an I2C command to Soundbank #18 in Stealth folder (zap sound effect)
  delay(200);
  digitalWrite (zapperpin, HIGH); // turn zapper on
  //sendI2Ccmd("$18"); // sends an I2C command to Soundbank #18 in Stealth folder (zap sound effect)
  delay (800);
  digitalWrite (zapperpin, LOW); // turn zapper off
  delay (500);
  


  

  //digitalWrite (cutdomemotorpower, LOW);  //RESTORE Dome Motor Power


  //Retract Arm for 2.5 seconds
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  delay(2500);

  //Stop Retracting Arm
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);



  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);


}


///////////////////////////
///////////////////////////
///////////////////////////
///////////////////////////



//////////////////////////////////////////////////////
/////SPECIAL SITUATION RETRACT ZAPPER ARM/////////////
//////////////////////////////////////////////////////

void specialsituation() {

  //Retrack Arm for 2.5 seconds
  analogWrite(enA, 100); // Send PWM signal to L298N Enable pin
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  delay(2500);

  //Stop Retracting Arm
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);



  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);


}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RESET SERVOS ================================================================================================
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void resetServos() {  //Sends Close Command to All Dome Panels

  digitalWrite(STATUS_LED, HIGH); // turn on STATUS LED so we can visually see we got the command on the board

  Serial.println("Reset Body panels");

  Servos[BP1].attach(BP1_SERVO_PIN);
  Servos[PinchServo].attach(Grip_Pinch);
  Servos[BP7].attach(BP7_SERVO_PIN);
  Servos[BP1].write(1450);
  Servos[PinchServo].write(2500);
  Servos[BP7].write(1940);
  
  delay(1000); // wait for servos to close

  // Detach servos

  Servos[BP1].detach();
  Servos[BP7].detach();
  Servos[PinchServo].detach();
   
  Serial.print("Body Panels Closed,Reset");

  i2cCommand = -1; // always reset i2cCommand to -1, so we don't repeatedly do the same command
  digitalWrite(STATUS_LED, LOW);

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////Main Loop//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
      GiveToy();
      break;

    case 3:
      Extend_and_zap();
      break;

    case 4:
      specialsituation();
      break;

    case 5:
      DispenseCard();
      break;

    case 6:
      OpenCloseGripperArmDoor();
      break;
    
    case 7:
      GripperArm();
      break;

    default: // Catch All
    case 0:
      digitalWrite(STATUS_LED, LOW);
      i2cCommand = -1;
      break;
  }

}
