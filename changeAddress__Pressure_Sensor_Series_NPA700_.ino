/* 
 *  Created By: Manan Patel
 */
#include <Wire.h>
#include <string.h>
#include <Stream.h>

/* PROVIDE CURRENT ADDRESS BELOW! */
int curAddress = 0x28;
/* PROVIDE CURRENT ADDRESS ABOVE! ^ */

/* PROVIDE NEW ADDRESS BELOW! */
byte newAddress = 0x25;
/* PROVIDE NEW ADDRESS ABOVE! ^ */


int ledPin = 13;
int sensorPower = 9;
uint8_t mask = 0x3f;
byte command[3] = {0xA0, 0x00, 0x00};  //command to put sensor in command mode
byte cmdRead[3]= {0x02, 0x00, 0x00};  //command to read EEPROM word 2 from sensor
byte bufRead[3] = {0x5A, 0x00, 0x00};  // buffer to store EEPROM word 2 first byte should be 5A
byte bufWrite[3]= {0x42, 0x00, 0x00};  // Write data to send with new address first byte is 0x42 and other  2 bytes will be the modified word 2 with new address
byte exitCmd[3]= {0x80, 0x00, 0x00};  //exit command after writing address, if address isnt written properly it will break the board
int readAddr;                         //int to store address read from word 2 to confirm it matches the expected address

void setup() {
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
    Wire.begin(); // join i2c bus (address optional for master)
    Serial.begin(9600);    
}

void loop() {
    digitalWrite(ledPin, HIGH);
    changeAddress(newAddress);
    digitalWrite(ledPin, HIGH);
    Serial.print("Sensor Address was Changed Successfully to ");
    Serial.print(newAddress, HEX);
    Serial.println();
    delay(500);
    exit(0);
}

void changeAddress(byte newAddress) {
      //begin command mode transmission
      digitalWrite(ledPin, LOW); //give power to the sensor
      delay(2);
      
      Wire.beginTransmission(curAddress);
      for(int i=0; i<3; i++){
        Wire.write(command[i]);        
      } 
      
      //Wire.write(command, 3);
      Wire.endTransmission();
    
      //begin EEPROM word 2 request transmission
      Wire.beginTransmission(curAddress); 
      for(int i=0; i<3; i++){
        Wire.write(cmdRead[i]);        
      }
      Wire.endTransmission();   
      
      //now request the EEPROM word 2 data. The first 
      //byte should be 0x5A. The next two bytes are word 2.
      //byte 1 is bits 15:8 and byte 2 is bits 7:0
      //The I2C address is held in 7 bits [9:3]
      int i=0;
      Wire.requestFrom(curAddress, 3);
      while(Wire.available())    
      { 
        bufRead[i] = Wire.read(); 
        i++;
      }
        Serial.println("Buffer contents");
        Serial.println(bufRead[0],HEX);
        Serial.println(bufRead[1],BIN);
        Serial.println(bufRead[2],BIN);
      
        readAddr =(bufRead[2] & 0xF8) >> 3; // least significant 5 bits of the I2C address
        readAddr += (bufRead[1] & 0x03) << 5; // most significant 2 bits of the I2C address
        Serial.println(readAddr);
        if (readAddr == newAddress) {
            Serial.println("The new address is already set.");            
            delay(100);
            exit(0);
        }
        //Bits [9:3] will be put into an address we can read to verify it matches the current address of the board
      
        //When we confirm that byte 0 is 0x5A and the address in bits [9:3] 
        // match the sensors current address, we can continue to write the new address
        if(bufRead[0]==0x5A && readAddr == curAddress){
            Serial.println("matched");
            bufWrite[2] = bufRead[2] & 0x07; // copy the parts of the buffer that aren't address
            bufWrite[1] = bufRead[1] & ~(0x03); // for both bytes
            bufWrite[2] += newAddress << 3;
            bufWrite[1] += newAddress >> 5;
            Serial.println(bufWrite[1]);
            Serial.println(bufWrite[2]);            
            Wire.beginTransmission(curAddress);
            for(int i=0; i<3; i++){
                Wire.write(bufWrite[i]);      
            }
            Wire.endTransmission();
    
            Wire.beginTransmission(curAddress);
            for(int i=0; i<3; i++){
              Wire.write(exitCmd[i]);      
            }
            Wire.endTransmission();
        } else {
            Serial.println("Error, buffer contents dont match expected values");
            Serial.println("Please unplug the Vcc connection (+3.3V), press the reset button on the arduino, and try again");
        }
}



























