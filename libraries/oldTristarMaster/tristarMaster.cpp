#include <tristarMaster.h>
#include <modbus.h>
#include <modbusDevice.h>
#include <Arduino.h>

#include "AmbulanceP.h"

extern AmbulanceP arp;
extern void WRITEMEMORYTOSD();


using namespace std;

tristarMaster::tristarMaster()
{

	tristarSerial=NULL;
	_prnt=prnt;
	disable();

	reset();
	
}

void tristarMaster::reset(){
deviceconnected=false;
deviceidentified=false;
jobDone=false;
icmd=-1,icd=0,iid=0,isr=0,ilg=0,ian=0;

}
void tristarMaster::setBaud(word baud)
{
	_baud = baud;
	//calculate the time perdiod for 3 characters for the given bps in ms.
	_frameDelay = 24000/_baud;

	Serial.begin(baud);
	Serial2.begin(baud);
	Serial3.begin(baud);

}

/*
Retrieve the serial baud rate
*/
word tristarMaster::getBaud(void)
{
	return(_baud);
}

/*
Generates the crc for the current message in the buffer.
*/

#if defined(readfromEEPROM)
byte tristarMaster::auchCRCHi(byte index){
	//if(_prnt) {Serial.print(index);Serial.print(" e "); Serial.println(READEEPROM(Offset_auchCRCHi+index));}
	return READEEPROM(Offset_auchCRCHi+index);}

byte tristarMaster::auchCRCLo(byte index){
	//if(_prnt) {Serial.print(index);Serial.print(" e "); Serial.println(READEEPROM(Offset_auchCRCLo+index));}
	return READEEPROM(Offset_auchCRCLo+index);}

byte tristarMaster::COMMAND(byte index){
	//if(_prnt) {Serial.print(index);Serial.print(" e "); Serial.println(READEEPROM(Offset_COMMAND+index));}
return READEEPROM(Offset_COMMAND+index);}

void tristarMaster::State(byte index, char* state){
	for (int j=0 ; j < 12 ; j++) state[j]=(char)READEEPROM(Offset_State+index*12+j);
	return ;}

void tristarMaster::Alarms(byte index, char* state){
	for (int j=0 ; j < 17 ; j++) state[j]=(char)READEEPROM(Offset_State+index*17+j);
	return ;}

void tristarMaster::Faults(byte index, char* state){
	for (int j=0 ; j < 17 ; j++) state[j]=(char)READEEPROM(Offset_State+index*17+j);
	return ;}
#else
byte tristarMaster::auchCRCHi(byte index){
	//if(_prnt) {Serial.print(index);Serial.print(" "); Serial.println(_auchCRCHi[index]);}
	return _auchCRCHi[index];}

byte tristarMaster::auchCRCLo(byte index){
		//if(_prnt) {Serial.print(index);Serial.print(" "); Serial.println(_auchCRCLo[index]);}
return _auchCRCLo[index];}

byte tristarMaster::COMMAND(byte index){
		//if(_prnt) {Serial.print(index);Serial.print(" "); Serial.println(_COMMAND[index]);}
return _COMMAND[index];}

void tristarMaster::State(byte index, char* state){
	for (int j=0 ; j < 12 ; j++) state[j]=(char) _State[index][j];
	return ;}

void tristarMaster::Alarms(byte index, char* state){
	for (int j=0 ; j < 17 ; j++) state[j]=(char) _State[index][j];
	return ;}

void tristarMaster::Faults(byte index, char* state){
	for (int j=0 ; j < 17 ; j++) state[j]=(char) _State[index][j];
	return ;}
#endif




void tristarMaster::calcCrc(void)
{
	byte	CRCHi = 0xFF,
			CRCLo = 0x0FF,
			Index,
			msgLen,
			*msgPtr;

	msgLen = _len-2;
	msgPtr = _msg;

	while(msgLen--)
	{
		Index = CRCHi ^ *msgPtr++;
		CRCHi = CRCLo ^ auchCRCHi(Index);
		CRCLo = auchCRCLo(Index);
	}
	_crc = (CRCHi << 8) | CRCLo;
}

/*
  Checks the UART for query data
*/
void tristarMaster::checkSerial(void)
{
	//while there is more data in the UART than when last checked
	while(tristarSerial->available()> _len)
	{
		//update the incoming query message length
		_len = tristarSerial->available();
		//Wait for 3 bytewidths of data (SOM/EOM)
//		delayMicroseconds(RTUFRAMETIME);
		delay(_frameDelay);
		//Check the UART again
	}
}

/*
Copies the contents of the UART to a buffer
*/
void tristarMaster::serialRx(void)
{
	byte i;

	//allocate memory for the incoming query message
	_msg = (byte*) malloc(_len);

		//copy the query byte for byte to the new buffer
		
		for (i=0 ; i < _len ; i++) _msg[i] = tristarSerial->read();

		if(_prnt)
		{
        for (i=0 ; i < _len ; i++)
		{Serial.print(_msg[i]); Serial.print(" ");}
		Serial.println("master in"); Serial.println();}
}

/*
Generates a query reply message for Digital In/Out status update queries.
*/
void tristarMaster::getStatus(byte funcType)
{

	// message length
	//for each register queried add 2 bytes
	_len =  4;
	//allow room for the Device ID byte, Function type byte, and crc word
	_len += 4;

	//allocate memory for the query response
	_msg = (byte *) malloc(_len);

	//write the device ID
	_msg[0] = _device->getId();
	//write the function type
	_msg[1] = funcType;

		//write the high byte of the register value
		_msg[2]  =COMMAND(4*icmd);
		//write the low byte of the register value
		_msg[3] = COMMAND(4*icmd+1);

		//write the high byte of the register value
		_msg[4]  =COMMAND(4*icmd+2);
		//write the low byte of the register value
		_msg[5] = COMMAND(4*icmd+3);


	//generate the crc for the query reply and append it
	this->calcCrc();
	_msg[_len - 2] = _crc >> 8;
	_msg[_len - 1] = _crc & 0xFF;

	sendMsg();
}

void tristarMaster::setStatus(word reg, word val)
{

			if(deviceconnected==false&&reg==8) {
				deviceconnected=true; 
				if(_prnt) Serial.println("connected..");
			return;
			}


			if(reg<30000) {reg+=30000; }

			_device->set(reg, val);


		}

void tristarMaster::setSerial(byte id){

	idSlave=id;

	//if(tristarSerial!=NULL) {tristarSerial->end(); tristarSerial=NULL;}

	delay(500);

if(id==3) {_prnt=prnt; tristarSerial=&Serial3;}
else if(id==2) {_prnt=prnt;tristarSerial=&Serial2;}
else {
	_prnt=false;	
	tristarSerial=&Serial;}

enable();

delay(500);

reset();


}
void tristarMaster::run(void)
{
	byte deviceId;
	byte funcType;
	word field1;
	word field2;
	long num;
	byte mask=1;
	
	int i,j,j0, ndata, nobj;
	//
	//if(!deviceconnected) connectDevice();
	//	else
	//	{
	//		if(idSlave==2||idSlave==3){
	//		//readDeviceIdentification();
	//		//if(deviceidentified)
	//		//	{
	//		//readDeviceSerial();
	//		//readLog();
	//		//	}
	//			getAnalogStatus();
	//	}
	//		else sendAnalogStatus(30001,29);
	//	}

		
	//check for data in the recieve buffer
	this->checkSerial();

	//if there is nothing in the recieve buffer, bail.
	if(_len == 0) return;


	//retrieve the query message from the serial uart
	this->serialRx();

	
	//if the message id is not 255, and
	// and device id does not match bail
	if( (_msg[0] != 0xFF) && 
		(_msg[0] != _device->getId()) )
	{
			free(_msg);
	//reset the message length;
	_len = 0;
		return;
	}
	//calculate the checksum of the query message minus the checksum it came with.
	this->calcCrc();
	
	//if the checksum does not match, ignore the message
	if ( _crc != ((_msg[_len - 2] << 8) + _msg[_len - 1])){
			free(_msg);
	//reset the message length;
	_len = 0;
		return;
	}

	
	//copy the function type from the incoming query
	funcType = _msg[1];


	if(funcType == WRITE_AO) //request from slave
	{
	
	ndata = _msg[2];
	
				if(ndata==2) 
				{
				//copy field 1 from the incoming query
				field1  = (_msg[3] << 8) | _msg[4];

				execCommand((int) field1);

				}


	}

	if(icmd!=-1) //command issued, awaiting response
	{
		if(funcType == READ_AI || funcType == READ_AO)
		{
			ndata = _msg[2];

			word startreg	= (COMMAND(4*icmd) << 8) | COMMAND(4*icmd+1);

				for(i = 0 ; i < ndata/2 ; i++) 
				{
				//copy field 1 from the incoming query
				field1  = (_msg[3+2*i] << 8) | _msg[4+2*i];

				this->setStatus(startreg+i, field1);

				}
			}
		else if(funcType == IDENTIFY)
		{
			deviceidentified=true;

			nobj = _msg[7];
			j0=7;

			for(j = 0 ; j < nobj ; j++){
				j0++;
				j0++;

			ndata = _msg[j0];

			if(_prnt){
			for(i = 0 ; i < ndata ; i++) Serial.write(_msg[++j0]);
			
			Serial.println();
			}
			}
		}
	}


	//free the allocated memory for the query message

	free(_msg);
	//reset the message length;
	_len = 0;
	icmd=-1;
	return;
	}


void tristarMaster::connectDevice(void){
if(icmd!=-1) return;
if(icd==1) {icmd=-1; return;}
icmd=icd++;
if(_prnt) Serial.println("connecting Device");
getStatus(0x03);
}

//void tristarMaster::readDeviceIdentification(void){
//	if(icmd!=-1) return;
//if(iid==1) {icmd=-1; return;}
//
//icmd=1+iid++;
//if(_prnt&&iid==1) Serial.println("reading Device Identification");
//
//_len =  3;
////allow room for the Device ID byte, Function type byte, and crc word
//_len += 4;
//
////allocate memory for the query response
//_msg = (byte *) malloc(_len);
//
////write the device ID
//_msg[0]=_device->getId();
////write the function type
//_msg[1]=0x2B;
//_msg[2]=0x0E;
//_msg[3]=0x01;
//_msg[4]=0x00;
//
////generate the crc for the query reply and append it
//this->calcCrc();
//_msg[_len - 2] = _crc >> 8;
//_msg[_len - 1] = _crc & 0xFF;
//}
//
//void tristarMaster::readDeviceSerial(void){
//if(icmd!=-1) return;
//if(isr==4)  {icmd=-1; return;}
//
//word val;
// if(isr>3) {
//	Serial.print("Device Serial: ");
//
//	 val = _device->get(0xF000);
//	 Serial.write(val & 0xFF);
//	 Serial.write(val >> 8);
//	 
//	 val = _device->get(0xF001);
//	 Serial.write(val & 0xFF);
//	 Serial.write(val >> 8);
//	 
//	 val = _device->get(0xF002);
//	 Serial.write(val & 0xFF);
//	 Serial.write(val >> 8);
//	 
//	 val = _device->get(0xF003);
//	 Serial.write(val & 0xFF);
//	 Serial.write(val >> 8);
//
//	 Serial.println();	
//	 isr++;
//		 return;
// }
//icmd=2+isr++; 
//if(_prnt&&isr==1) Serial.println("reading Device Serial");
//getStatus(0x03);}
//
//void tristarMaster::readLog(void){
//if(icmd!=-1||ilg>14) return;
//icmd=18+ilg++;
//if(_prnt&&ilg==1)Serial.println("reading Logs");
//getStatus(0x03); }

void tristarMaster::getAnalogStatus(void){
		if(icmd!=-1) return;
if(ian==10)  {
	jobDone=true; 
	deviceconnected=false;
deviceidentified=false;
return;}

icmd=6+ian++;
if(_prnt&&ian==1) Serial.println("reading Analog Status");
getStatus(0x03);}



/*
Generates a query reply message for Digital In/Out status update queries.
*/
void tristarMaster::sendAnalogStatus(void)
{

	word startreg=30001;
	word numregs=29;

	word val;
	byte i = 0;


	//calculate the query reply message length
	//for each register queried add 2 bytes
	_len = numregs * 2;
	//allow room for the Device ID byte, Function type byte, data byte count byte, and crc word
	_len += 5;

	//allocate memory for the query response
	_msg = (byte *) malloc(_len);

	//write the device ID
	_msg[0] = _device->getId();
	//write the function type

	_msg[1] = WRITE_AO;

	//set the data byte count
	_msg[2] = _len - 5;

	//for each register queried
	while(numregs--)
	{
		//retrieve the value from the register bank for the current register
		val = _device->get(startreg+i);
		//write the high byte of the register value
		_msg[3 + i * 2]  = val >> 8;
		//write the low byte of the register value
		_msg[4 + i * 2] = val & 0xFF;
		//increment the register
		i++;
	}

	//generate the crc for the query reply and append it
	this->calcCrc();
	_msg[_len - 2] = _crc >> 8;
	_msg[_len - 1] = _crc & 0xFF;

	sendMsg();

	return;
}

void tristarMaster::printMemory(int byteOffset)
{
_prnt=prnt;
word num;
char* stateChar=new char[12];
char* alarmChar=new char[17];
        byte nalm=0, nflt=0, mask=1;
		int i;

State(_device->get(byteOffset+0x7532),stateChar);

num=((long)_device->get(byteOffset+0x7545)) <<16;
num|=_device->get(byteOffset+0x7546);
Serial.print(num); Serial.print("h ");

if(_device->get(byteOffset+0x7537)==1) Serial.print("IGNON ");
else Serial.print("IGNOFF ");

Serial.print("V_b: ");Serial.print(2.950043e-3*_device->get(byteOffset+0x7538));Serial.print("V ");

Serial.print("V_sp: "); Serial.print(4.246521e-3*_device->get(byteOffset+0x753a)); Serial.print("V ");

Serial.print("V_ref: "); Serial.print(2.950043e-3*_device->get(byteOffset+0x7540));Serial.print("V ");

Serial.print("A_chrg: ");Serial.print(2.034515e-3*_device->get(byteOffset+0x753b)); Serial.print("A ");

num=((long)_device->get(byteOffset+0x7535)) <<16;
num|=_device->get(byteOffset+0x7536);
Serial.print(" Ah_d: ");Serial.print(2.950043e-3*num); 

num=((long)_device->get(byteOffset+0x7541)) <<16;
num|=_device->get(byteOffset+0x7542);
Serial.print("Ah_r: "); Serial.print(2.950043e-3*num); Serial.print("Ah ");

num=((long)_device->get(byteOffset+0x7543)) <<16;
num|=_device->get(byteOffset+0x7544);
Serial.print("Ah_t: "); Serial.print(2.950043e-3*num); Serial.print("Ah ");

Serial.print("T_in: "); Serial.print(_device->get(byteOffset+0x753e)); Serial.print("C ");

Serial.print("T_out: "); Serial.print(_device->get(byteOffset+0x753f)); Serial.print("C ");

Serial.print(stateChar); Serial.print(" ");

num=((long)_device->get(byteOffset+0x7533)) <<16;
num|=_device->get(byteOffset+0x7534);
num=(millis()-num);
Serial.print(num/60000.); Serial.print("m ");

num=_device->get(byteOffset+0x754C);

Serial.print("PWM: ");
if(num>=230) Serial.print("100");
else Serial.print(num/2.3); 

Serial.println("% ");

num=_device->get(byteOffset+0x7547);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					Alarms(i,alarmChar);
					Serial.print(alarmChar); Serial.print(", "); nalm++;}
				mask<<=1;
			}
num=_device->get(byteOffset+0x754D);
			mask=00000001;
			for (i=0; i<8; i++)
			{
				if(num & mask) {
					Alarms(i,alarmChar);
					Serial.print(alarmChar); Serial.print(", "); nalm++;}
				mask<<=1;
			}
			if(nalm==0) {Serial.print("No alarms "); Serial.print(" ");}


num=_device->get(byteOffset+0x7548);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					Faults(i,alarmChar);
					Serial.print(alarmChar); Serial.print(", "); nalm++;}
				mask<<=1;
			}
			if(nalm==0) {Serial.print("No faults "); Serial.print(" ");}


num=_device->get(byteOffset+0x7549);
			mask=00000001;
			for (i=0; i<8; i++)
			{
				if(num & mask) {Serial.print("1");}
				else {Serial.print("0");}
				mask<<=1;
			}
			Serial.println();


delete []stateChar; stateChar!=NULL;
delete []alarmChar; alarmChar!=NULL;

  return;
}


void tristarMaster::sendMsg(){

		int i;
		//send the reply to the serial UART
		//Senguino doesn't support a bulk serial write command....
		
		for(i = 0 ; i < _len ; i++) tristarSerial->write(_msg[i]);

					
		if(_prnt)
		{for (i=0 ; i < _len ; i++){ 
		 Serial.print(_msg[i]); Serial.print(":");}
		 Serial.println("master out");Serial.println();}
		//free the allocated memory for the reply message
		
		free(_msg);
		//reset the message length
		_len = 0;

		delay(100);
}


void tristarMaster::execCommand(int nexe)		
{

	char state[5];
	if(_prnt)  Serial.print("Executing ");if(prnt) Serial.println(nexe);

// toggle digital pins
if(nexe>-1&&nexe<54) { 
arp.setPin(nexe,!arp.getPin(nexe)); 
if(_prnt){ Serial.print("Pin "); Serial.print(nexe); Serial.print(" is ");  
Serial.println(digitalRead(nexe));}

} 
else switch(nexe){



case 60: {sendmemorytoARC(); break;}

case 70: arp.resetSolar(); break;
	case 71: arp.AllSolarto1(); break;
		case 72: arp.AllSolarto2(); break;
	

default: break;	 
//
}


}	


void tristarMaster::sendmemorytoARC(){
	setSerial(0);
sendAnalogStatus();
return;
}

void tristarMaster::update(){

	unsigned long val;
		  //reset timer if state changed 
		if(_device->get(0x7532)!=_device->get(0x754B)){
		_device->set(0x7532,_device->get(0x754B));
                
			val=millis();

			//write new value for since change
			_device->set(0x7533,val >> 16);
			_device->set(0x7534,val & 0xFFFF);
		}

		//update Ah counter 
		if(_device->get(0x7532)==3) 	{//if night
			_device->set(0x7535,0 >> 16);
			_device->set(0x7536,0 & 0xFFFF);
		}	
		else {
		//read existing value
			val=((long) _device->get(0x7535)) <<16;
			val|=_device->get(0x7536);

			//update
			val|=_device->get(0x753B);

			//write new value
			_device->set(0x7535,val >> 16);
			_device->set(0x7536,val & 0xFFFF);
		}



}

void tristarMaster::logTristar(){


unsigned long  timer1;

for(int id=0; id<2; id++){

	if(_prnt) {Serial.print("logTristar"); Serial.println(id+2);}

	bool updated=false;
	
	timer1=millis();

	setSerial(id+2);

	_device->set(0x7531,id+2);

	while(1)
	{
	if(millis()-timer1>5000) break;

	if(logtristar()){

			updated=true;

			update();

			break;
		} //if master.logTristar
	} //while

	if(updated)
	{
				WRITEMEMORYTOSD();
				sendmemorytoARC();
	}
}

return;
};



bool tristarMaster::logtristar(void)
{
	if(!deviceconnected) connectDevice();
		else getAnalogStatus();
//{
//readDeviceIdentification();
//if(deviceidentified)
//	{
//readDeviceSerial();
//readLog();
//	}
//getAnalogStatus();
//}
	

	icmd=-1;
	return jobDone;
	}


void tristarMaster::writeMemorytoString(int byteOffset,MyString *_SMS)
{

word num;
char* stateChar=new char[12];
        byte nalm=0, nflt=0, mask=1;
		int i;

		_SMS->free();
State(_device->get(byteOffset+0x7532),stateChar);

num=((long)_device->get(byteOffset+0x7545)) <<16;
num|=_device->get(byteOffset+0x7546);
_SMS->appendTime(num); 

if(_device->get(byteOffset+0x7537)==1) _SMS->append(", IGNON, ");
else _SMS->append(", IGNOFF, ");

_SMS->append("Vb:"); _SMS->appendDouble( 2.950043e-3*_device->get(byteOffset+0x7538), 2);   _SMS->append(", ");

_SMS->append("Vsp:"); _SMS->appendDouble( 4.246521e-3*_device->get(byteOffset+0x753a), 2);  _SMS->append(", ");

_SMS->append("Vref:"); _SMS->appendDouble( 2.950043e-3*_device->get(byteOffset+0x7540), 2); _SMS->append(", ");

_SMS->append("Achrg:"); _SMS->appendDouble( 2.034515e-3*_device->get(byteOffset+0x753b), 2);  _SMS->append(", ");



num=((long)_device->get(byteOffset+0x7535)) <<16;
num|=_device->get(byteOffset+0x7536);
_SMS->append("Ahd:"); _SMS->appendDouble( 2.950043e-3*num, 1);  _SMS->append(", ");


num=((long)_device->get(byteOffset+0x7541)) <<16;
num|=_device->get(byteOffset+0x7542);
_SMS->append("Ahr:"); _SMS->appendDouble( (int) 2.950043e-3*num,0);   _SMS->append(", ");



num=((long)_device->get(byteOffset+0x7543)) <<16;
num|=_device->get(byteOffset+0x7544);
_SMS->append("Aht:"); _SMS->appendDouble( (int) 2.950043e-3*num,0);   _SMS->append(", "); 



_SMS->append("Ti:"); _SMS->appendDouble( _device->get(byteOffset+0x753e),0);   _SMS->append(", ");

 _SMS->append("Tb:");_SMS->appendDouble( _device->get(byteOffset+0x753f),0);   _SMS->append(", ");

_SMS->append(stateChar); _SMS->append(", ");



num=((long)_device->get(byteOffset+0x7533)) <<16;
num|=_device->get(byteOffset+0x7534);
num=(millis()-num);

_SMS->appendTime(num);   

num=_device->get(byteOffset+0x754C);

_SMS->append(", PWM:");
if(num>=230) _SMS->append("100");
else _SMS->appendDouble( num/2.3, 1); 

_SMS->append("%, ");


_SMS->append("Al:");
num=_device->get(byteOffset+0x7547);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					if(nalm>0) _SMS->append(",");
					_SMS->appendDouble( i, 0);  nalm++;}
				mask<<=1;
			}
num=_device->get(byteOffset+0x754D);
			mask=00000001;
			for (i=0; i<8; i++)
			{
				if(num & mask) {
					if(nalm>0) _SMS->append(",");
					_SMS->appendDouble( 16+i, 0);  nalm++;}
				mask<<=1;
			}
			if(nalm==0) {_SMS->append("-"); _SMS->append(", ");}

_SMS->append(" Fl:");
num=_device->get(byteOffset+0x7548);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					if(nflt>0) _SMS->append(",");
					_SMS->appendDouble( i, 0); nflt++;}
				mask<<=1;
			}
			if(nalm==0) {_SMS->append("-"); _SMS->append(" ");}



delete []stateChar; stateChar!=NULL;

return;
}








void tristarMaster::writeAlarmstoString(int byteOffset,MyString *_SMS)
{

word num;
char* alarmChar=new char[17];
        byte nalm=0, nflt=0, mask=1;
		int i;

		_SMS->free();
_SMS->append("Alarms: ");

num=_device->get(byteOffset+0x7547);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					Alarms(i,alarmChar);
					if(nalm>0) _SMS->append(",");
					_SMS->append(alarmChar);  nalm++;}
				mask<<=1;
			}
num=_device->get(byteOffset+0x754D);
			mask=00000001;
			for (i=0; i<8; i++)
			{
				if(num & mask) {
					Alarms(16+i,alarmChar);
					if(nalm>0) _SMS->append(",");
					_SMS->append(alarmChar);  nalm++;}
				mask<<=1;
			}
			if(nalm==0) {_SMS->append("No alarms"); _SMS->append(" ");}

_SMS->append(". Faults: ");
num=_device->get(byteOffset+0x7548);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					Faults(i,alarmChar);
					if(nflt>0) _SMS->append(",");
					_SMS->append(alarmChar);  nflt++;}
				mask<<=1;
			}
			if(nalm==0) {_SMS->append(" No faults"); _SMS->append(" ");}


_SMS->append(". Dip Switch: ");
num=_device->get(byteOffset+0x7549);
			mask=00000001;
			for (i=0; i<8; i++)
			{
				if(num & mask) {_SMS->append("1");}
				else {_SMS->append("0");}
				mask<<=1;
			}

delete []alarmChar; alarmChar!=NULL;

return;
}







