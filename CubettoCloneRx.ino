/* 
 *  
  Lots of help referenced here:
  http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
  
  and here:
  http://shanes.net/another-nrf24l01-sketch-string-sendreceive/
  
  Uses the RF24 Library by TMRH2o here:
  https://github.com/TMRh20/RF24

  Uses the AccelStepper Library linked on this page:
  https://arduino-info.wikispaces.com/SmallSteppers#2motor
 */
 
#include <AccelStepper.h>
#include <SPI.h> 
#include "RF24.h"

#define FULLSTEP 4
#define HALFSTEP 8

// motor pins
#define motorPin1  3     // Blue   - 28BYJ48 pin 1
#define motorPin2  4     // Pink   - 28BYJ48 pin 2
#define motorPin3  5     // Yellow - 28BYJ48 pin 3
#define motorPin4  6     // Orange - 28BYJ48 pin 4
                        // Red    - 28BYJ48 pin 5 (VCC)
                        
#define motorPin5  7     // Blue   - 28BYJ48 pin 1
#define motorPin6  8     // Pink   - 28BYJ48 pin 2
#define motorPin7  14    // Yellow - 28BYJ48 pin 3
#define motorPin8  15    // Orange - 28BYJ48 pin 4
                        // Red    - 28BYJ48 pin 5 (VCC)
#define stepsToTake 2048

RF24 myRadio (9, 10); // "myRadio" is the identifier you will use in following methods

// NOTE: The sequence 1-3-2-4 is required for proper sequencing of 28BYJ48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper2(HALFSTEP, motorPin5, motorPin7, motorPin6, motorPin8);

byte addresses[][6] = {"CNode"}; // Create address for 1 pipe.

int commandPosition = 0;
int len = 0;
String commandSet = "";
char message[1];

void setup()
{
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(1000.0);
  stepper1.setSpeed(1000);
  
  stepper2.setMaxSpeed(1000.0);
  stepper2.setAcceleration(1000.0);
  stepper2.setSpeed(-1000);

  myRadio.begin();  // Start up the physical nRF24L01 Radio
  myRadio.setChannel(108);  // Above most Wifi Channels
  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  myRadio.setPALevel(RF24_PA_MIN);
  //  myRadio.setPALevel(RF24_PA_MAX);  // Uncomment for more power
  myRadio.openReadingPipe(1, addresses[0]); // Use the first entry in array 'addresses' (Only 1 right now)
  myRadio.startListening();
}


void loop()
{ 
  // While there is data ready
  while (myRadio.available())
    {
      //read each character individually
      myRadio.read( message, 1);
      Serial.println(message[0]);

      //keep adding to the set until the 1 character is received
      if(message[0] != '1'){
        commandSet.concat(message[0]);
      }else{
        len = commandSet.length();
        Serial.println(commandSet);
      }
    }
    
//if we haven't read every character that was sent, keep reading
 if(len > 0 && len > commandPosition){
      //if the motors aren't moving get the next command
      if(stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0){
        char com = commandSet[commandPosition];
        
        commandPosition++;

        switch (com){
          case 'f':
            MoveForward();
            break;
          case 'r':
            TurnRight();
            break;
          case 'l':
            TurnLeft();
            break;
          case 'b':
            Reverse();
            break;
        }

        //if we're through the list of commands, clear everything out
        if(commandPosition == len){
          commandPosition = 0;
          commandSet = "";
          len = 0;
        }
      }
    }

  stepper1.run();
  stepper2.run();
}

void Move(int s1Steps, int s2Steps){
  stepper1.moveTo(s1Steps);
  stepper2.moveTo(s2Steps);
}

void MoveForward()
{
  Move(stepper1.currentPosition() + stepsToTake, stepper2.currentPosition() - stepsToTake);
}

void Reverse()
{
  Move(stepper1.currentPosition() - stepsToTake, stepper2.currentPosition() + stepsToTake);
}

void TurnRight()
{
  Move(stepper1.currentPosition() - stepsToTake, stepper2.currentPosition() - stepsToTake);
}

void TurnLeft()
{
  Move(stepper1.currentPosition() + stepsToTake, stepper2.currentPosition() + stepsToTake);
}

