#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <tristarMaster.h>
#include <SimpleTimer.h>
#include <EEPROM.h>
SimpleTimer t;

/*
This example code shows a quick and dirty way to get an
arduino to talk to a modbus master device with a
device ID of 1 at 9600 baud.
*/

//Setup the brewtrollers register bank
//All of the data accumulated will be stored here
modbusDevice regBank;
//Create the modbus slave protocol handler
tristarMaster master;
const bool prnt=true;

void setup()
{   

int i;
//Assign the modbus device ID.  
  regBank.setId(1);


//Add Analog Input registers 30001-10010 to the register bank
for(i = 0 ; i < 28 ; i++) regBank.add(30001+i);

  regBank.set(30001,0x0002);
  regBank.set(30002,0x0023);
  regBank.set(30003,0x0030);
  regBank.set(30004,0x0043);
  regBank.set(30005,0x0053);
  regBank.set(30006,0x0063);
  regBank.set(30007,0x0001);
  regBank.set(30008,0x1003);
  regBank.set(30009,0x1003);
  regBank.set(30010,0x1013);
  regBank.set(30011,0x0003);
  regBank.set(30012,0x0003);
  regBank.set(30013,0x0003);
  regBank.set(30014,0x0003);
  regBank.set(30015,0x0003);
  regBank.set(30016,0x0003);
  regBank.set(30017,0x0003);
  regBank.set(30018,0x0003);
  regBank.set(30019,0x0003);
  regBank.set(30020,0x0003);
  regBank.set(30021,0x002c);
  regBank.set(30022,0x0011);
  regBank.set(30023,0x0000);
  regBank.set(30024,0x0003);
  regBank.set(30025,0x0003);
  regBank.set(30026,0x0003);
  regBank.set(30027,0x0003);
  regBank.set(30028,0x0003);
  regBank.set(30029,0x0003);
  

  
//Add EEPROM registers to the register bank
  regBank.add(57344);
  regBank.add(57345);
  regBank.add(57346);
  regBank.add(57347);
  regBank.add(57348);
  regBank.add(57349);
  
 //Add Serial registers to the register bank
  regBank.add(0xF000); 
  regBank.add(0xF001);
  regBank.add(0xF002);
  regBank.add(0xF003);

/*
Assign the modbus device object to the protocol handler
This is where the protocol handler will look to read and write
register data.  Currently, a modbus slave protocol handler may
only have one device assigned to it.
*/
  master._device = &regBank;  
  
  if(prnt) Serial.begin(9600);

// Initialize the serial port for coms at 9600 baud  
  master.setBaud(9600);   
  master.setSerial(0);

  while(1){
	  if(master.run()) {master.printMemory(2); break;
  }
  
  }
}

void loop()
{
}

byte READEEPROM(int index){
//if(prnt) {Serial.print(index);Serial.print(" ");Serial.println(EEPROM.read(index));}
return EEPROM.read(index);
}






