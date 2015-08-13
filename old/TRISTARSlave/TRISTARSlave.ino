#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <tristarSlave.h>
#include <EEPROM.h>



const bool prnt=true;

modbusDevice regBank;
//Create the modbus slave protocol handler
tristarSlave slave;


void setup()
{   
int i;
//Assign the modbus device ID.  
  regBank.setId(1);


//Add Analog Input registers 30001-10010 to the register bank
for(i = 0 ; i < 28 ; i++) regBank.add(30001+i);
for( i = 0 ; i < 28 ; i++) regBank.add(40001+i);
for( i = 0 ; i < 0x2F ; i++) regBank.add(8000+i);
  

  regBank.set(0x8000, 0x10FF);
  regBank.set(0x8001, 0x33FE);
  regBank.set(0x8002, 0x87FD);
  regBank.set(0x8003, 0x23FC);
  regBank.set(0x8004, 0xF45B);
  regBank.set(0x8005, 0x65FA);
  regBank.set(0x8006, 0x76F9);
  regBank.set(0x8007, 0x87F8);
  regBank.set(0x8008, 0x97F7);
  regBank.set(0x8009, 0x65F6);
  regBank.set(0x800A, 0x78F5);
  regBank.set(0x800B, 0x90F4);
  regBank.set(0x800C, 0x32F3);
  regBank.set(0x800D, 0x45F2);
  regBank.set(0x800E, 0x67F1);
  regBank.set(0x800F, 0x90F0);
  
  regBank.set(0x8010, 0x20EF);
  regBank.set(0x8011, 0x12EE);
  regBank.set(0x8012, 0x44ED);
  regBank.set(0x8013, 0x87EC);
  regBank.set(0x8014, 0x7EB);
  regBank.set(0x8015, 0x65EA);
  regBank.set(0x8016, 0x55E9);
  regBank.set(0x8017, 0x56E8);
  regBank.set(0x8018, 0x78E7);
  regBank.set(0x8019, 0x89E6);
  regBank.set(0x801A, 0x90E5);
  regBank.set(0x801B, 0x76E4);
  regBank.set(0x801C, 0x64E3);
  regBank.set(0x801D, 0x55E2);
  regBank.set(0x801E, 0x45E1);
  regBank.set(0x801F, 0x67E0);
  
  regBank.set(0x8020, 0x30DF);
  regBank.set(0x8021, 0x20DE);
  regBank.set(0x8022, 0x18DD);
  regBank.set(0x8023, 0x09DC);
  regBank.set(0x8024, 0x87DB);
  regBank.set(0x8025, 0x76DA);
  regBank.set(0x8026, 0x56D9);
  regBank.set(0x8027, 0x47D8);
  regBank.set(0x8028, 0x87D7);
  regBank.set(0x8029, 0x44D6);
  regBank.set(0x802A, 0x55D5);
  regBank.set(0x802B, 0x66D4);
  regBank.set(0x802C, 0x77D3);
  regBank.set(0x802D, 0x88D2);
  regBank.set(0x802E, 0x99D1);
  regBank.set(0x802F, 0x00D0);
    
  regBank.add(57344);
  regBank.add(57345);
  regBank.add(57346);
  regBank.add(57347);
  regBank.add(57348);
  regBank.add(57349);
  
  
  regBank.add(0xF000); 
  regBank.add(0xF001);
  regBank.add(0xF002);
  regBank.add(0xF003);


  regBank.set(57344,0x1007);
  regBank.set(57345,0x1008);
  regBank.set(57346,0x1009);
  regBank.set(57347,0x1000);
  regBank.set(57348,0x1001);
  regBank.set(57349,0x1002);

  regBank.set(30001,0x0002);
  regBank.set(30002,0x0027);
  regBank.set(30003,0x0030);
  regBank.set(30004,0x0047);
  regBank.set(30005,0x0053);
  regBank.set(30006,0x0067);
  regBank.set(30007,0x0001);
  regBank.set(30008,0x1007);
  regBank.set(30009,0x1007);
  regBank.set(30010,0x1017);
  regBank.set(30011,0x0007);
  regBank.set(30012,0x0007);
  regBank.set(30013,0x0007);
  regBank.set(30014,0x0007);
  regBank.set(30015,0x0007);
  regBank.set(30016,0x0007);
  regBank.set(30017,0x0007);
  regBank.set(30018,0x0007);
  regBank.set(30019,0x0007);
  regBank.set(30020,0x0007);
  regBank.set(30021,0x002c);
  regBank.set(30022,0x0011);
  regBank.set(30023,0x0000);
  regBank.set(30024,0x0007);
  regBank.set(30025,0x0007);
  regBank.set(30026,0x0007);
  regBank.set(30027,0x0007);
  regBank.set(30028,0x0007);
  regBank.set(30029,0x0007);
  
  regBank.set(40001,0x0003);
  regBank.set(40002,0x0007);
  regBank.set(40003,0x0007);
  regBank.set(40004,0x0007);
  regBank.set(40005,0x0007);
  regBank.set(40006,0x0007);
  regBank.set(40007,0x0000);
  regBank.set(40008,0x1017);
  regBank.set(40009,0x1027);
  regBank.set(40010,0x1037);
  regBank.set(40011,0x0017);
  regBank.set(40012,0x0017);
  regBank.set(40013,0x0002);
  regBank.set(40014,0x0004);
  regBank.set(40015,0x0003);
  regBank.set(40016,0x0007);
  regBank.set(40017,0x0007);
  regBank.set(40018,0x0007);
  regBank.set(40019,0x0007);
  regBank.set(40020,0x0007);
  regBank.set(40021,0x002c);
  regBank.set(40022,0x0011);
  regBank.set(40023,0x0000);
  regBank.set(40024,0x0007);
  regBank.set(40025,0x0007);
  regBank.set(40026,0x0007);
  regBank.set(40027,0x0007);
  regBank.set(40028,0x0007);
  regBank.set(40029,0x0007);
  
  regBank.set(0xF000,(0x34 << 8) | 0x31);
  regBank.set(0xF001,(0x38 << 8) | 0x33);
  regBank.set(0xF002,(0x39 << 8) | 0x30);
  regBank.set(0xF003,(0x33 << 8) | 0x31);
  
   slave._device = &regBank;  

if(prnt) Serial.begin(9600);
// Initialize the serial port for coms at 9600 baud  
slave.setBaud(9600); 
slave.setSerial(0);    
  
  //put some data into the registers

  
  if(prnt)  Serial.println("slave on");
}

void loop()
{
 slave.run();  
}

byte READEEPROM(int index){return EEPROM.read(index);}



