#include <SimpleModbusSlave.h>

/* 
   SimpleModbusSlaveV10 supports function 3, 6 & 16.
   
  
   Note:  
   The Arduino serial ring buffer is 64 bytes or 32 registers.
   Most of the time you will connect the arduino to a master via serial
   using a MAX485 or similar.
 
   In a function 3 request the master will attempt to read from your
   slave and since 5 bytes is already used for ID, FUNCTION, NO OF BYTES
   and two BYTES CRC the master can only request 58 bytes or 29 registers.
 
   In a function 16 request the master will attempt to write to your 
   slave and since a 9 bytes is already used for ID, FUNCTION, ADDRESS, 
   NO OF REGISTERS, NO OF BYTES and two BYTES CRC the master can only write
   54 bytes or 27 registers.
 
   Using a USB to Serial converter the maximum bytes you can send is 
   limited to its internal buffer which differs between manufactures. 
*/



// Using the enum instruction allows for an easy method for adding and 
// removing registers. Doing it this way saves you #defining the size 
// of your slaves register array each time you want to add more registers
// and at a glimpse informs you of your slaves register layout.

//////////////// registers of your slave ///////////////////
enum 
{     
  // The first register starts at address 0
  AI0, //0    
  AI1, //1
  AI2, //2
  AI3, //3
  AI4, //4
  AW0, //5    
  AW1, //6
  AW2, //7
  AW3, //8
  AW4, //9
  DI0, //10     
  DI1, //11
  DI2, //12
  DI3, //13
  DI4, //14
  DW0, //15     
  DW1, //16
  DW2, //17
  DW3, //18
  DW4, //19
  TOTAL_ERRORS, //20       
  HOLDING_REGS_SIZE //21 leave this one
  // total number of registers for function 3 and 16 share the same register array
  // i.e. the same address space
};


unsigned int Registers[HOLDING_REGS_SIZE]; // function 3 and 16 register array
unsigned int counter;

float Y = 500;
float U = 250;
////////////////////////////////////////////////////////////

//void(* resetFunc) (void) = 0;

void setup()
{
  /* parameters(HardwareSerial* SerialPort,
                long baudrate, 
                unsigned char byteFormat,
                unsigned char ID, 
                unsigned char transmit enable pin, 
                unsigned int holding registers size,
                unsigned int* holding register array)
  */
  
  /* Valid modbus byte formats are:
     SERIAL_8N2: 1 start bit, 8 data bits, 2 stop bits
     SERIAL_8E1: 1 start bit, 8 data bits, 1 Even parity bit, 1 stop bit
     SERIAL_8O1: 1 start bit, 8 data bits, 1 Odd parity bit, 1 stop bit
     
     You can use SERIAL_8N1 but this does not adhere to the
     Modbus specifications. That said, I have tested the SERIAL_8N1 option 
     on various commercial masters and slaves that were suppose to adhere
     to this specification and was always able to communicate.
     
     These byte formats are already defined in the Arduino global name space. 
  */
  
  modbus_configure(&Serial,9600, SERIAL_8N1, 1, 2, HOLDING_REGS_SIZE, Registers);

  // modbus_update_comms(baud, byteFormat, id) allows for easy update of the
  // port variables and slave id dynamically in any function.
  // modbus_update_comms(9600, SERIAL_8E2, 1);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Registers[AI0] = 0;
  delay(10);
}

void loop()
{
  
  // modbus_update() is the only method used in loop(). It returns the total error
  // count since the slave started. You don't have to use it but it's useful
  // for fault finding by the modbus master.
  Registers[AI0] = Registers[AI0] + 1; // update data to be read by the master
  Registers[AI1] = 100; // update data to be read by the master
  Registers[AI2] = 200; // update data to be read by the master
  U = Registers[AW0];
  Y = 0.9672*Y + 0.06557*U;
  Registers[AI3] = (unsigned int) Y; // update data to be read by the master
  Registers[AI4] = 400; // update data to be read by the master

  Registers[DI0] = 1; // update data to be read by the master
  Registers[DI2] = 1; // update data to be read by the master
  Registers[DI3] = 0; // update data to be read by the master
  Registers[DI4] = 1; // update data to be read by the master

  if (Registers[DW0] == 0){
    digitalWrite(LED_BUILTIN, LOW);
      Registers[DI1] = 0;
  }  
  else {
    digitalWrite(LED_BUILTIN, HIGH);
      Registers[DI1] = 1;
  }

Registers[TOTAL_ERRORS] = modbus_update(); 
delay(100);
}
