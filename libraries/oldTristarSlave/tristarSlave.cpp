#include <tristarSlave.h>
#include <modbus.h>
#include <modbusDevice.h>
#include <Arduino.h>





tristarSlave::tristarSlave()
{
_prnt=prnt;
enable();
}
/*
Set the Serial Baud rate.
Reconfigure the UART for 8 data bits, no parity, and 1 stop bit.
and flush the serial port.
*/
void tristarSlave::setBaud(word baud)
{
	_baud = baud;
	//calculate the time perdiod for 3 characters for the given bps in ms.
	_frameDelay = 0;// 24000/_baud;

}

void tristarSlave::reset()
{
	enable();

}

void tristarSlave::setSerial(byte id){

if(tristarSerial) tristarSerial->end();

if(id==3) tristarSerial=&Serial3;
else if(id==2) tristarSerial=&Serial2;
else {
	_prnt=false;
	tristarSerial=&Serial;
}

tristarSerial->begin(getBaud());

/*deviceconnected=false;
deviceidentified=false;
jobDone=false;
icmd=-1,icd=0,iid=0,isr=0,ilg=0,ian=0;*/

}

/*
Retrieve the serial baud rate
*/
word tristarSlave::getBaud(void)
{
	return(_baud);
}

/*
Generates the crc for the current message in the buffer.
*/

#if defined(readfromEEPROM)
byte tristarSlave::auchCRCHi(byte index){return READEEPROM(Offset_auchCRCHi+index);}
byte tristarSlave::auchCRCLo(byte index){return READEEPROM(Offset_auchCRCLo+index);}
byte tristarSlave::COMMAND(byte index){return READEEPROM(Offset_COMMAND+index);}
void tristarSlave::State(byte index, char* state){
	for (int j=0 ; j < 12 ; j++) state[j]=(char)READEEPROM(Offset_State+index*12+j);

	return ;
}

void tristarSlave::Alarms(byte index, char* state){
	for (int j=0 ; j < 17 ; j++) state[j]=(char)READEEPROM(Offset_Alarms+index*17+j);
	return ;}

void tristarSlave::Faults(byte index, char* state){
	for (int j=0 ; j < 17 ; j++) state[j]=(char)READEEPROM(Offset_Faults+index*17+j);
	return ;}


#else
byte tristarSlave::auchCRCHi(byte index){return _auchCRCHi[index];}
byte tristarSlave::auchCRCLo(byte index){return _auchCRCLo[index];}
byte tristarSlave::COMMAND(byte index){return _COMMAND[index];}
void tristarSlave::State(byte index, char* state){
	for (int j=0 ; j < 12 ; j++) state[j]=(char) _State[index][j];

	return ;
}

void tristarSlave::Alarms(byte index, char* state){
	for (int j=0 ; j < 17 ; j++) state[j]=(char) _Alarms[index][j];
	return ;}

void tristarSlave::Faults(byte index, char* state){
	for (int j=0 ; j < 17 ; j++) state[j]=(char) _Faults[index][j];
	return ;}

#endif
void tristarSlave::calcCrc(void)
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
void tristarSlave::checkSerial(void)
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
void tristarSlave::serialRx(void)
{

	byte i;

	//allocate memory for the incoming query message
	_msg = (byte*) malloc(_len);

		//copy the query byte for byte to the new buffer
		
		for (i=0 ; i < _len ; i++) _msg[i] = tristarSerial->read();
		
		if(_prnt)
		{for (i=0 ; i < _len ; i++) 
		 {Serial.print(_msg[i]); Serial.print(":");}
		 Serial.println("slave in");Serial.println();}
}

/*
Generates a query reply message for Digital In/Out status update queries.
*/
void tristarSlave::writeCommand(int nexe)
{
	word val= (word) nexe;

	if(_msg!=NULL) free(_msg); _msg=NULL;
	_len = 2;
	//allow room for the Device ID byte, Function type byte, data byte count byte, and crc word
	_len += 5;

	//allocate memory for the query response
	//_msg = (byte *) malloc(_len);
	_msg = new (byte)(_len);

	//write the device ID
	_msg[0] = _device->getId();
	//write the function type
	_msg[1] = WRITE_AO;
	//set the data byte count
	_msg[2] = _len - 5;

		//write the high byte of the register value
		_msg[3]  = val >> 8;
		//write the low byte of the register value
		_msg[4] = val & 0xFF;
		//increment the register


	//generate the crc for the query reply and append it
	this->calcCrc();
	_msg[_len - 2] = _crc >> 8;
	_msg[_len - 1] = _crc & 0xFF;


	sendMsg();

}


void tristarSlave::sendAnalogStatus(byte funcType, word startreg, word numregs)
{

	word val;
	byte i = 0;
if(_msg!=NULL) free(_msg); _msg=NULL;

	//if the function is to read analog inputs then add 30001 to the start register
	//else add 40001 to the start register
	if(startreg<30000)
	{
	if(funcType == READ_AO) 		startreg += 30000;
	else startreg += 40000;
	}
	

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
	_msg[1] = funcType;
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
}

void tristarSlave::sendMsg(){

		int i;
		//send the reply to the serial UART
		//Senguino doesn't support a bulk serial write command....


		for(i = 0 ; i < _len ; i++) tristarSerial->write(_msg[i]);


					
		if(_prnt)
		{for (i=0 ; i < _len ; i++){ 
		 Serial.print(_msg[i]); Serial.print(":");}
		 Serial.println("slave out");Serial.println();}
		//free the allocated memory for the reply message
		
if(_msg!=NULL) free(_msg); _msg=NULL;
//if (_msg!=NULL) delete [] _msg; _msg=NULL;
		//reset the message length
		_len = 0;

		delay(100);
}
void tristarSlave::run(void)
{

	byte deviceId;
	byte funcType;
	word field1;
	word field2;
	word startreg;
	
	int i, ndata;
	
	//initialize mesasge length
	_len = 0;

	//check for data in the recieve buffer
	this->checkSerial();

	//if there is nothing in the recieve buffer, bail.
	if(_len == 0)
	{
		return;
	}

	//retrieve the query message from the serial uart
	this->serialRx();


	//if the message id is not 255, and
	// and device id does not match bail
	if( (_msg[0] != 0xFF) && 
		(_msg[0] != _device->getId()) )
	{
		//if(prnt) Serial.println("id error");
		if(_msg!=NULL) free(_msg); _msg=NULL;
		return;}
	//calculate the checksum of the query message minus the checksum it came with.
	this->calcCrc();


	//if the checksum does not match, ignore the message
	if ( _crc != ((_msg[_len - 2] << 8) + _msg[_len - 1]))
	{
		if(prnt) Serial.println("crc error");
if(_msg!=NULL) free(_msg); _msg=NULL;
return;}
	
	//copy the function type from the incoming query
	funcType = _msg[1];


	

		//return;

	//generate query response based on function type
	switch(funcType)
	{
	case READ_AI:
			case READ_AO:
	
	//copy fiield 1 from the incoming query
	field1	= (_msg[2] << 8) | _msg[3];

	//copy field 2 from the incoming query
	field2  = (_msg[4] << 8) | _msg[5];


	//free the allocated memory for the query message

if(_msg!=NULL) free(_msg); _msg=NULL;
//reset the message length;
	_len = 0;


		this->sendAnalogStatus(funcType, field1, field2);
		break;

	case WRITE_AO:
			ndata = _msg[2];

			startreg	= 0;

			for(i = 0 ; i < ndata/2 ; i++) 
			{
			//copy field 1 from the incoming query
			field1  = (_msg[3+2*i] << 8) | _msg[4+2*i];

			if(i==0&&field1==3) startreg	= 10000;

			//if(prnt){ Serial.print(30001+startreg+i); Serial.print("<-"); Serial.println(field1);}

			this->setStatus(30001+startreg+i, field1);

			}

			//printMemory(startreg);
			

if(_msg!=NULL) free(_msg); _msg=NULL;
//reset the message length;
	_len = 0;

		break;


	default:
		return;
		break;
	}

	//if a reply was generated
	if(_len) sendMsg();

}

void tristarSlave::setStatus(word reg, word val)
{
			_device->set(reg, val);
		}

void tristarSlave::printMemory(int byteOffset)
{

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

Serial.print(", Vb: ");Serial.print(2.950043e-3*_device->get(byteOffset+0x7538)); 

Serial.print(", Vsp: ");Serial.print(4.246521e-3*_device->get(byteOffset+0x753a));

Serial.print(", Vref: "); Serial.print(2.950043e-3*_device->get(byteOffset+0x7540)); 

Serial.print(", Achg: "); Serial.print(2.034515e-3*_device->get(byteOffset+0x753b)); 

num=((long)_device->get(byteOffset+0x7535)) <<16;
num|=_device->get(byteOffset+0x7536);
Serial.print(", Ah_d: ");Serial.print(2.950043e-3*num); 

num=((long)_device->get(byteOffset+0x7541)) <<16;
num|=_device->get(byteOffset+0x7542);
Serial.print(", Ah_r: "); Serial.print(2.950043e-3*num); 

num=((long)_device->get(byteOffset+0x7543)) <<16;
num|=_device->get(byteOffset+0x7544);
Serial.print(", Ah_t: "); Serial.print(2.950043e-3*num); 

Serial.print(", Ti "); Serial.print(_device->get(byteOffset+0x753e)); 

Serial.print(", Tb "); Serial.print(_device->get(byteOffset+0x753f)); 

Serial.print(stateChar); Serial.print(", ");

num=((long)_device->get(byteOffset+0x7533)) <<16;
num|=_device->get(byteOffset+0x7534);
num=(millis()-num);
Serial.print(num/60000.); Serial.print(" m ");

num=_device->get(byteOffset+0x754C);

Serial.print(", PWM:");
if(num>=230) Serial.print("100");
else Serial.print(num/2.3); 

Serial.print("% ");

Serial.print(", Alarms:");
num=_device->get(byteOffset+0x7547);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					Alarms(i,alarmChar);
					if(nalm>0) Serial.print(",");
					Serial.print(alarmChar);  nalm++;}
				mask<<=1;
			}
num=_device->get(byteOffset+0x754D);
			mask=00000001;
			for (i=0; i<8; i++)
			{
				if(num & mask) {
					Alarms(i,alarmChar);
					if(nalm>0) Serial.print(",");
					Serial.print(alarmChar);  nalm++;}
				mask<<=1;
			}
			if(nalm==0) {Serial.print("No alarms"); Serial.print(" ");}

Serial.print(", Faults:");
num=_device->get(byteOffset+0x7548);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					Faults(i,alarmChar);
					if(nflt>0) Serial.print(",");
					Serial.print(alarmChar); nflt++;}
				mask<<=1;
			}
			if(nalm==0) {Serial.print("No faults"); Serial.print(" ");}

Serial.print(", Dip  Switch:");
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
void tristarSlave::writeMemorytoString(int byteOffset,MyString *_SMS)
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








void tristarSlave::writeAlarmstoString(int byteOffset,MyString *_SMS)
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







