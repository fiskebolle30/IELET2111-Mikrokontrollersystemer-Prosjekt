/* Defines for what the different bytes in the Fan_reg array represent: */

#define CURR_TEMP_H 0
#define CURR_TEMP_L 1
        
#define CURR_INT_VOLTAGE_H 2
#define CURR_INT_VOLTAGE_L 3
#define CURR_EXT_VOLTAGE_H 4
#define CURR_EXT_VOLTAGE_L 5

#define CURR_FAN_0_SPEED_H 6 //Dependent on the measurement/logging period.
#define CURR_FAN_0_SPEED_L 7
#define CURR_FAN_1_SPEED_H 8
#define CURR_FAN_1_SPEED_L 9

#define ERROR_BYTE 10 //The different errors defined by the bits in the byte are defined below.
#define CLEAR_ERROR 11 //Writing to this byte will clear the written bits in ERROR_BYTE.

#define TEMP_ALARM_LEVEL_H 12  //thermistor ADC value that triggers error.
#define TEMP_ALARM_LEVEL_L 13
#define FAN_TIMEOUT 14

#define MEASUREMENT_PERIOD_H 15 //How long the fan counter can count per measurement.
#define MEASUREMENT_PERIOD_L 16
//Remember to extend FAN_REG_LENGTH!!!!! (max value + 1)

#define FAN_REG_LENGTH 17

#include <Wire.h>

void readFromFanReg(uint8_t pos, int amount)
{
  Wire.beginTransmission(58);
  Wire.write(pos);
  Wire.endTransmission();
  Wire.requestFrom(58, amount);
  Wire.endTransmission();
}

void setup() {
  Wire.begin();
  Serial.begin(9600); //
  Wire.beginTransmission(58);
  Wire.write(MEASUREMENT_PERIOD_H); 
  Wire.write(0x10);
  Wire.write(0x00); //Low byte is just 0. Written to trigger the update
  Wire.endTransmission();

  Wire.beginTransmission(58);
  Wire.write(FAN_TIMEOUT);
  Wire.write(3); 
  Wire.endTransmission();
}

void loop() {
  readFromFanReg(CURR_FAN_0_SPEED_H, 4);
  uint16_t received_val_0 = (Wire.read() << 8);
  received_val_0 |= Wire.read();
  uint16_t received_val_1 = (Wire.read() << 8);
  received_val_1 |= Wire.read();
  Serial.print("Fan 0: ");
  Serial.print(received_val_0);
  Serial.print(", Fan 1: ");
  Serial.print(received_val_1);
  
  readFromFanReg(ERROR_BYTE, 1);
  uint8_t error = Wire.read();

  readFromFanReg(CURR_TEMP_H, 2);
  uint16_t temp = (Wire.read() << 8);
  temp |= Wire.read();
  

/*  Wire.beginTransmission(58);
  Wire.write(CLEAR_ERROR);
  Wire.write(0b00000010); //test-clear fan 1.*/
  
  Wire.endTransmission();
  Serial.print(", errors: ");
  Serial.print(error);
  Serial.print(", current temp ADC reading: ");
  Serial.println(temp);
  delay(50);
}
