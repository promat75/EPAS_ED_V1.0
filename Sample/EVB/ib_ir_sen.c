
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2011.
	- Date		: 2016-11-15
	- Version		: V2.6

	- File			: ib_ir_sen.c
	-                       : 적외선 온도센서 TMP007 제어 코드
*******************************************************************************/


#include "ib_main_sen.h"
//#include "ib_ir_sen.h"

INT16 IR_Temp;

// Acceleration Sensor Init//
void TMP007_Init(void)
{
	UINT16 u16read_data;
	
	u8I2CSenStatus = 0;
	// 0x02, 0x1940
	u8I2CSenStatus = I2C_uInt16Write(TMP007_I2CADDR, TMP007_CONFIG, 
	                                                       (TMP007_CFG_MODEON | TMP007_CFG_ALERTEN |  TMP007_CFG_TRANSC | TMP007_CFG_1SAMPLE));
	// 0x05,  0xC000                                                   
	u8I2CSenStatus = I2C_uInt16Write(TMP007_I2CADDR, TMP007_STATMASK, 
	                                                       (TMP007_STAT_ALERTEN |TMP007_STAT_CRTEN));
 	
	u8I2CSenStatus= I2C_uInt16Read(TMP007_I2CADDR, TMP007_DEVID, &u16read_data);
  	if (u16read_data != 0x78) 
		zPrintf(1, "\n >> TMP007 I2C Initializse Fail !!!");

	#if _ACCEL_USE
	if (u8I2CSenStatus & 0x80) {
		zPrintf(1, "\n >> TMP007  I2C function Fail !!!");
		ACCELSEN_RESET = RST_ON;
		AppLib_Delay(100);			// 100ms ; 핀연결이 되어 있지 않음
		ACCELSEN_RESET = RST_OFF;
	}		
	#endif
  	
  	IR_Temp= ReadIRSensor(TMP007_TOBJ);
    	zPrintf(1, "\n >> IR_Temp = %3.1f", (float)IR_Temp/2);
	
    	IR_Temp= ReadIRSensor(TMP007_TDIE);
   	zPrintf(1, "\n >> IR_Temp = %3.1f", (float)IR_Temp/2);
  		
}


INT16 ReadIRSensor(UINT8 u8cmd)
{
	UINT16 u16read_data;
	float fcelsius = 0.5f;

  	u8I2CSenStatus= I2C_uInt16Read(TMP007_I2CADDR, u8cmd, &u16read_data);
	
	if (u16read_data & 0x8000) {
		// negatvie temp
	         u16read_data = u16read_data ^ 0x7FFF + 0x02;	// 2's compliments를 위해 +1온도값을  +2배 온도값으로 
            	//temp_c_shift  = ((temp_c_2s)>>2);
            	fcelsius = -0.5f;								// 2배 온도값으로 
        }    	
            	
        fcelsius = ((float)u16read_data * fcelsius) * 0.03125f;		// 2배 온도값으로 

	return ((INT16)fcelsius);
}


UINT8 I2C_uInt16Write(UINT8 addr, UINT8 u8cmd, UINT16 word_data)
{
	UINT8 u8buffer[2];
		
	u8buffer[0] = (UINT8)(word_data >> 8);                // MSB first       
	u8buffer[1] = (UINT8)word_data;             	// LSB
	
	return (HAL_I2CMaster_RandomWrite(addr, u8cmd, u8buffer, 2));  

}	


UINT8 I2C_uInt16Read(UINT8 addr, UINT8 u8cmd, UINT16* u16read_data)
{
	UINT8 u8buffer[2];
	UINT8 u8status;

	u8status = HAL_I2CMaster_RandomRead(addr, u8cmd, u8buffer, 0x02); 
	
	*u16read_data = (UINT16)u8buffer[0] << 8 | u8buffer[1];                // MSB first       
	
	return u8status;
}	


#if 0

#include "mbed.h"
 
#define    TMP007_Voltage     0x00 
#define    TMP007_LocalTemp   0x01
#define    TMP007_Conf        0x02
#define    TMP007_ObjTemp     0x03
 
#define    TMP007_ADDR 0x80
 
I2C i2c(PA_10, PA_9);
 
//DigitalOut myled(LED1);
 
Serial pc(PA_2, PA_3);
 
int16_t temp_c, temp_c_2s, voltage_tot, voltage_h, voltage_2s, voltage_tot_a;
float celsius, farenheit, voltage;
char temp_write[3];
char temp_read[2];
uint8_t temp_h, temp_l, temp_h_and, voltage_l, voltage_h_and; 
int sign;
 
//char TMP007_ObjTemp = 0x03;
 
 
int main()
{
 
 
 
    /* Configure the Temp Sensor:
 
    */
    temp_write[0] = TMP007_Conf;
    temp_write[2] = 0x40;   //Mode On, Alert, TC, 16 samples
    temp_write[1] = 0x19;   //check order of these (endianness)
    i2c.write(TMP007_ADDR, temp_write, 3, 0);
    
    while (1) {
        // Read Temp Sensor
        
        //Tell the temp sensor to take a one shot temperature measurement
        //temp_write[0] = LM75B_Conf;
        //temp_write[1] = 0x50;   //One shot, low power mode
        //i2c.write(LM75B_ADDR, temp_write, 2, 0);
        
        //Read Temperature Register
        temp_write[0] = TMP007_ObjTemp;
        i2c.write(TMP007_ADDR, temp_write, 1, false); // no stop (unsure)
        i2c.read(TMP007_ADDR, temp_read, 2, 0);
        
        temp_h = temp_read[0];
        temp_l = temp_read[1];
        
        pc.printf("Object Temperature\n");
        pc.printf("high bit = %x\n", temp_h);
        pc.printf("low bit = %x\n", temp_l);
        
        temp_c = ((temp_h) << 8) | temp_l;
        
        //int tempval = (int)((int)data_read[0] << 8) | data_read[1];
        pc.printf("combined bit = %x\n", temp_c);
        temp_h_and = temp_h & 0x80;
        //check for negative number        
        if (temp_h_and == 0x80)
        {
            
            //flip bits and add 1
            temp_c_2s = temp_c ^ 0x7FFF + 0x04;
            //temp_c_shift  = ((temp_c_2s)>>2);
            celsius = ((float)temp_c_2s * -0.25f) *.03125f;
            pc.printf("negative\n");
            pc.printf("anded high bit = %x\n", temp_h_and);         
         }       
        else 
        {
            //temp_c_shift = ((temp_c)>>2);
            //pc.printf("shifted bits: %x\n", temp_c_shift);
            celsius = ((float)temp_c * 0.25f) *.03125f;
            pc.printf("positive\n");
        }
        pc.printf("Capsule Temperature is: %f C\n", celsius);    
        farenheit = 1.8*celsius + 32;
        pc.printf("Capsule Temperature is: %f F\n\n", farenheit);
        
        //Read Internal Temperature Register
        temp_write[0] = TMP007_LocalTemp;
        i2c.write(TMP007_ADDR, temp_write, 1, false); // no stop (unsure)
        i2c.read(TMP007_ADDR, temp_read, 2, 0);
        
        temp_h = temp_read[0];
        temp_l = temp_read[1];
        
        pc.printf("Internal Temperature\n");
        pc.printf("high bit = %x\n", temp_h);
        pc.printf("low bit = %x\n", temp_l);
        
        temp_c = ((temp_h) << 8) | temp_l;
        
        //int tempval = (int)((int)data_read[0] << 8) | data_read[1];
        pc.printf("combined bit = %x\n", temp_c);
        temp_h_and = temp_h & 0x80;
        //check for negative number        
        if (temp_h_and == 0x80)
        {
            
            //flip bits and add 1
            temp_c_2s = temp_c ^ 0x7FFF + 0x04;
            //temp_c_shift  = ((temp_c_2s)>>2);
            //scale
            celsius = ((float)temp_c_2s * -0.25f) *.03125f;
            pc.printf("negative\n");
            //pc.printf("anded high bit = %x\n", temp_h_and);         
         }       
        else 
        {
            //temp_c_shift = ((temp_c)>>2);
            //pc.printf("shifted bits: %x\n", temp_c_shift);
            //scale
            celsius = ((float)temp_c * 0.25f) *.03125f;
            pc.printf("positive\n");
        }
        pc.printf("Internal Temperature is: %f C\n", celsius);    
        farenheit = 1.8*celsius + 32;
        pc.printf("Internal Temperature is: %f F\n\n", farenheit);
        
        //Read Voltage Register
        temp_write[0] = TMP007_Voltage;
        i2c.write(TMP007_ADDR, temp_write, 1, false); // no stop (unsure)
        i2c.read(TMP007_ADDR, temp_read, 2, 0);
        
        voltage_h = temp_read[0];
        voltage_l = temp_read[1];
        
        pc.printf("Sensor Voltage\n");
        pc.printf("high bit = %x\n", voltage_h);
        pc.printf("low bit = %x\n", voltage_l);
        
        voltage_tot = ((voltage_h) << 8) | voltage_l;
        voltage_tot_a = voltage_tot & 0x0000FFFF;
        
        //int tempval = (int)((int)data_read[0] << 8) | data_read[1];
        pc.printf("combined bit = %x\n", voltage_tot_a);
        voltage_h_and = voltage_h & 0x80;
        //check for negative number        
        if (voltage_h_and == 0x80)
        {
            
            //flip bits and add 1
            //voltage_2s = voltage_tot ^ 0x7FFF + 0x01;
            voltage_2s = ~voltage_tot_a + 0x01;
            pc.printf("2's compliment conversion: %x\n", voltage_2s);
            //temp_c_shift  = ((temp_c_2s)>>2);
            voltage = ((float)voltage_2s * -1.0f) *.00015625f;
            pc.printf("negative\n");
            //pc.printf("anded high bit = %x\n", temp_h_and);         
         }       
        else 
        {
            //temp_c_shift = ((temp_c)>>2);
            //pc.printf("shifted bits: %x\n", temp_c_shift);
            voltage = ((float)voltage_tot_a * 1.0f) *.00015625f;
            pc.printf("positive\n");
        }
        pc.printf("Sensor Voltage is: %f mV\n\n\n", voltage);    
        
        
        
        
        wait(4);    
    }
}


//----------------------------------------------------------------------------------------------

#define TMP007_I2C_ADDR   0x80                               // TMP007 I2C address (ADD0 and ADD1 pins are connected to ground)

float Temperature_read;
char txt1[10];
char old_txt[10];
char lab1[10] = {0};
char lab2[10] = {0};
char res;

// Initialize LCD display
// Lcd pinout settings
sbit LCD_RS at LATB4_bit;
sbit LCD_EN at LATB5_bit;
sbit LCD_D7 at LATB3_bit;
sbit LCD_D6 at LATB2_bit;
sbit LCD_D5 at LATB1_bit;
sbit LCD_D4 at LATB0_bit;

// Pin direction
sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D7_Direction at TRISB3_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D4_Direction at TRISB0_bit;
// End Initialize LCD display


/*******************************************************************************
* Function Get_Object_Temperature()
* ------------------------------------------------------------------------------
* Overview: Function gets temperature data from TMP007 sensor
* Input: Nothing
* Output: Temperature data
*******************************************************************************/
float Get_Object_Temperature() {
  char tmp_data[2];
  int TemperatureSum;
  float Temperature;

  I2C1_Start();                                              // Issue I2C start signal
  I2C1_Wr(TMP007_I2C_ADDR);                                  // Send Slave Address Byte
  I2C1_Wr(0x03);                                             // Pointer Register Byte
  I2C1_Repeated_Start();                                     // Issue I2C signal repeated start
  I2C1_Wr(TMP007_I2C_ADDR+1);                                // Slave Address Byte incremented for one address
  tmp_data[0] = I2C1_Rd(1);                                  // Read High value from register address (0x03) and store it in tmp_data and send
                                                             // acknowledge bit to enable reading second byte from the same reg
  tmp_data[1] = I2C1_Rd(_I2C_NACK);                          // Read Low value from register address (0x03)
  I2C1_Stop();                                               // Issue I2C stop signal

  TemperatureSum = ((tmp_data[0] << 8) | tmp_data[1]) >> 2;  // Justify temperature values

  if(TemperatureSum & (1 << 13))                             // Test negative bit
    TemperatureSum |= 0xE000;                                // Set bits 13 to 15 to logic 1 to get this reading into real two complement

  Temperature = (float)TemperatureSum * 0.03125;             // Multiply temperature value with 0.03125 (value per bit)

  return Temperature;                                        // Return temperature data
}



//----------------------------------------------------------------------------------------------

/*************************************************** 
  This is a library for the TMP007 Temp Sensor

  Designed specifically to work with the Adafruit TMP007 Breakout 
  ----> https://www.adafruit.com/products/2023

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include "Wire.h"

// uncomment for debugging!
//#define TMP007_DEBUG 1

#define TMP007_VOBJ       0x00
#define TMP007_TDIE       0x01
#define TMP007_CONFIG     0x02
#define TMP007_TOBJ       0x03
#define TMP007_STATUS     0x04
#define TMP007_STATMASK   0x05

#define TMP007_CFG_RESET    0x8000
#define TMP007_CFG_MODEON   0x1000
#define TMP007_CFG_1SAMPLE  0x0000
#define TMP007_CFG_2SAMPLE  0x0200
#define TMP007_CFG_4SAMPLE  0x0400
#define TMP007_CFG_8SAMPLE  0x0600
#define TMP007_CFG_16SAMPLE 0x0800
#define TMP007_CFG_ALERTEN  0x0100
#define TMP007_CFG_ALERTF   0x0080
#define TMP007_CFG_TRANSC   0x0040

#define TMP007_STAT_ALERTEN 0x8000
#define TMP007_STAT_CRTEN   0x4000

#define TMP007_I2CADDR 0x40
#define TMP007_DEVID 0x1F



class Adafruit_TMP007  {
 public:
  Adafruit_TMP007(uint8_t addr = TMP007_I2CADDR);
  boolean begin(uint8_t samplerate = TMP007_CFG_16SAMPLE);  // by default go highres

  int16_t readRawDieTemperature(void);
  int16_t readRawVoltage(void);
  double readObjTempC(void);
  double readDieTempC(void);

 private:
  uint8_t _addr;
  uint16_t read16(uint8_t addr);
  void write16(uint8_t addr, uint16_t data);
};



/*************************************************** 
  This is a library for the TMP007 Temp Sensor

  Designed specifically to work with the Adafruit TMP007 Breakout 
  ----> https://www.adafruit.com/products/2023

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_TMP007.h"
#include <util/delay.h>

//#define TESTDIE 0x0C78
//#define TESTVOLT 0xFEED

Adafruit_TMP007::Adafruit_TMP007(uint8_t i2caddr) {
  _addr = i2caddr;
}


boolean Adafruit_TMP007::begin(uint8_t samplerate) {
  Wire.begin();

  write16(TMP007_CONFIG, TMP007_CFG_MODEON | TMP007_CFG_ALERTEN | 
	  TMP007_CFG_TRANSC | samplerate);
  write16(TMP007_STATMASK, TMP007_STAT_ALERTEN |TMP007_STAT_CRTEN);
  // enable conversion ready alert

  uint16_t did;
  did = read16(TMP007_DEVID);
#ifdef TMP007_DEBUG
  Serial.print("did = 0x"); Serial.println(did, HEX);
#endif
  if (did != 0x78) return false;
  return true;
}

//////////////////////////////////////////////////////

double Adafruit_TMP007::readDieTempC(void) {
   double Tdie = readRawDieTemperature();
   Tdie *= 0.03125; // convert to celsius
#ifdef TMP007_DEBUG
   Serial.print("Tdie = "); Serial.print(Tdie); Serial.println(" C");
#endif
   return Tdie;
}

double Adafruit_TMP007::readObjTempC(void) {
  int16_t raw = read16(TMP007_TOBJ);
  // invalid
  if (raw & 0x1) return NAN;
  raw >>=2;

  double Tobj = raw;
  Tobj *= 0.03125; // convert to celsius
#ifdef TMP007_DEBUG
   Serial.print("Tobj = "); Serial.print(Tobj); Serial.println(" C");
#endif
   return Tobj;
}



int16_t Adafruit_TMP007::readRawDieTemperature(void) {
  int16_t raw = read16(TMP007_TDIE);

#if TMP007_DEBUG == 1

#ifdef TESTDIE
  raw = TESTDIE;
#endif

  Serial.print("Raw Tambient: 0x"); Serial.print (raw, HEX);
  

  float v = raw/4;
  v *= 0.03125;
  Serial.print(" ("); Serial.print(v); Serial.println(" *C)");
#endif
  raw >>= 2;
  return raw;
}

int16_t Adafruit_TMP007::readRawVoltage(void) {
  int16_t raw;

  raw = read16(TMP007_VOBJ);

#if TMP007_DEBUG == 1

#ifdef TESTVOLT
  raw = TESTVOLT;
#endif

  Serial.print("Raw voltage: 0x"); Serial.print (raw, HEX);
  float v = raw;
  v *= 156.25;
  v /= 1000;
  Serial.print(" ("); Serial.print(v); Serial.println(" uV)");
#endif
  return raw; 
}


/*********************************************************************/

uint16_t Adafruit_TMP007::read16(uint8_t a) {
  uint16_t ret;

  Wire.beginTransmission(_addr); // start transmission to device 
#if (ARDUINO >= 100)
  Wire.write(a); // sends register address to read from
#else
  Wire.send(a); // sends register address to read from
#endif
  Wire.endTransmission(); // end transmission
  
  Wire.beginTransmission(_addr); // start transmission to device 
  Wire.requestFrom(_addr, (uint8_t)2);// send data n-bytes read
#if (ARDUINO >= 100)
  ret = Wire.read(); // receive DATA
  ret <<= 8;
  ret |= Wire.read(); // receive DATA
#else
  ret = Wire.receive(); // receive DATA
  ret <<= 8;
  ret |= Wire.receive(); // receive DATA
#endif
  Wire.endTransmission(); // end transmission

  return ret;
}

void Adafruit_TMP007::write16(uint8_t a, uint16_t d) {
  Wire.beginTransmission(_addr); // start transmission to device 
#if (ARDUINO >= 100)
  Wire.write(a); // sends register address to read from
  Wire.write(d>>8);  // write data
  Wire.write(d);  // write data
#else
  Wire.send(a); // sends register address to read from
  Wire.send(d>>8);  // write data
  Wire.send(d);  // write data
#endif
  Wire.endTransmission(); // end transmission
}

#endif

