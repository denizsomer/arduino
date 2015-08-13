#include "Tristar.h"

#define readfromEEPROM

#if defined(readfromEEPROM)

extern byte READEEPROM(int);

//EEPROM Offsets
const int Offset_auchCRCHi=0, Offset_auchCRCLo=256, Offset_COMMAND=512, Offset_State=520
, Offset_Alarms=628, Offset_Faults=1036;

//These live in EEPROM, use writetristarEEPROM.ino to upload them


#else

 //Table of CRC values for high–order byte 
const byte _auchCRCHi[] = {
				0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
				0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
				0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
				0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
				0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
				0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
				0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
				0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
				0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
				0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
				0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
				0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
				0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
				0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
				0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
				0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
				0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
				0x40};

// Table of CRC values for low–order byte 
const byte _auchCRCLo[] = {
				0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
				0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
				0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
				0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
				0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
				0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
				0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
				0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
				0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
				0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
				0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
				0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
				0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
				0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
				0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
				0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
				0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
				0x40};
				
const byte _COMMAND[] = {
0xF0, 0x00, 0x00, 0x04, //1 61440 4
0x00, 0x08, 0x00, 0x16, //2 8 22 
};


const char _State[9][12] = {
"Start",	
"Night Check",
"Disconnect",
"Night",
"Fault",
"Bulk",
"PWM",
"Float",
"Equalize"
};

const char _Alarms[24][17] = {
"RTS open        ",
"RTS shorted     ",
"RTS disconnected",
"Ths disconnected",
"Ths shorted     ",
"TriStar hot     ",
"Current limit   ",
"Current offset  ",
"Battery Sense   ",
"Batt Sense disc ",
"Uncalibrated    ",
"RTS miswire     ",
"HVD             ",
"high d          ",
"miswire         ",
"FET open        ",
"P12*            ",
"Load Disc.*     ",
"Alarm 19*       ",
"Alarm 20*       ",
"Alarm 21*       ",
"Alarm 22*       ",
"Alarm 23*       ",
"Alarm 24*       "
};

const char _Faults[15][17] = {
"External Short  ",
"Overcurrent     ",
"FET short       ",
"Software        ",
"HVD             ",
"TriStar hot     ",
"DIP sw changed  ",
"Setting edit    ",
"reset?          ",
"Miswire         ",
"RTS shorted     ",
"RTS disconnected",
"Fault 12        ",
"Fault 13        ",
"Fault 14        "
};
#endif


Tristar::Tristar()
{
		
	_device=NULL;
	tristarSerial=NULL;
	serialTimeout=NULL;
	convTimeout=NULL;

_funcType=0;
	_baud=(word) 0;
	_crc=(word) 0;
	_frameDelay= (word) 0;

	return;
}

Tristar::~Tristar()
{
	if(_device!=NULL) delete _device; _device=NULL;

if(_msg!=NULL) delete _msg; _msg=NULL;
	return;
}

void Tristar::reset()
{
	 setSerial(ARD);

	return;
}
//*
//Set the Serial Baud rate.
//Reconfigure the UART for 8 data bits, no parity, and 1 stop bit.
//*/
void Tristar::setBaud(word baud)
{
	_baud = baud;
	//calculate the time perdiod for 3 characters for the given bps in ms.
	_frameDelay = 24000/_baud;

}

word Tristar::getBaud(void)
{
	return(_baud);
}






void Tristar::setSerial(byte id){

	resetSerial();

	_sid=id;


	if(prnt)  Serial.print("setSerial: ");
if( _sid == TRISTAR14) {
	if(prnt)  Serial.println("TRISTAR14");

	tristarSerial=&Serial2;
}

else if(_sid == TRISTAR12) {
	if(prnt)  Serial.println("TRISTAR12");

	tristarSerial=&Serial3;

}
else {
	if(prnt)  Serial.println("ARD");
	tristarSerial=&Serial;}

enable();

return;

}

void Tristar::calcCrc(void)
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






void Tristar::run(){

if(tristarSerial==NULL) return;


int i, j, jmyPDU0=0, jmyPDU=0, unpackedBytesLength, first;
byte inbyte;

//if(prin) Serial.print(tristarSerial->available());

	while(tristarSerial->available()&&!_ended)
{

inbyte = tristarSerial->read();

if(prin)  {Serial.print((int )inbyte); Serial.print(":");}


if( !_started && inbyte == _device->getId() )  {resetSerial();  _started=true; startSerialTimeout();}

		if(_started)
		{
			_len++;

			//Serial.print((byte) character); Serial.print("(");
			//Serial.print(_len); Serial.print(") ,");

			if(_len==2) _funcType=	inbyte;

			//if(_len==10) _ended=true;
		
	//Serial.print(inbyte); Serial.print("-"); Serial.print(_len);Serial.print(":");


	preprocessMessage(inbyte);

	//Serial.println(_ended);

	//for(int i=0; i<_len; i++) {Serial.print(_msg[i]); Serial.print("#");}


		}

} //SERIAL AVAILABLE



	if(_ended)
	{
	
	calcCrc();

			if(dbg)  {
				for(int i=0; i<5; i++) {Serial.print(_msg[i]); Serial.print("#");}
					 Serial.println();
			}

	//if the checksum does not match, ignore the message
			if ( _crc != ((_msg[_len-2]	 << 8) + _msg[_len-1]	)){

		if(dbg) 
		{
					Serial.println(_len);
			Serial.println(_msg[_len-1]);
			Serial.println(_msg[_len-2]);
			Serial.println((_msg[_len-2] << 8) + _msg[_len-1]);
			Serial.println(_crc);
			if(dbg){Serial.println("crc not ok");}
			
		}

		exitSerialTimeout();
	return;
	}

	if(dbg) Serial.print("crc ok");

		processMessage();

	exitSerialTimeout();
	return;
	}		

	}



void Tristar::resetSerial(void){

_ended=false; _started=false;

_ndata=0;
_len=0;
if(_msg!=NULL) free(_msg); _msg=NULL;

Timeout::resetSerial();

}

void Tristar::setStatus(word reg, word val) {_device->set(reg, val);}
void Tristar::getStatus(word reg, word val)
{
	//Set the query response message length
	//Device ID byte, Function byte, Register byte, Value byte, CRC word
	_len = 8;
	//allocate memory for the message buffer.
	if(_msg!=NULL) free(_msg);
	_msg = (byte *) malloc(_len);

	//write the device ID
	_msg[0] = _device->getId();

	_msg[1] = _funcType;
	
	//write the register number high byte value
	_msg[2] = reg >> 8;
	//write the register number low byte value
	_msg[3] = reg & 0xFF;
	//write the control value's high byte
	_msg[4] = val >> 8;
	//write the control value's low byte
	_msg[5] = val & 0xFF;

	//calculate the crc for the query reply and append it.
	this->calcCrc();
	_msg[_len - 2]= _crc >> 8;
	_msg[_len - 1]= _crc & 0xFF;

	sendMsg();
}



bool Tristar::writeMemorytoString(byte id, MyString *_SMS)
{

word num;
        byte nalm=0, nflt=0, mask=1;
		int i;

_SMS->free();

	 num = _device->get(byteOffset[id]+30030);
	 _SMS->append((char) (num & 0xFF));
	 _SMS->append((char) (num >> 8));
	 
	 num = _device->get(byteOffset[id]+30031);
	 _SMS->append((char) (num & 0xFF));
	 _SMS->append((char) (num >> 8));
	 
	 num = _device->get(byteOffset[id]+30032);
	 _SMS->append((char) (num & 0xFF));
	 _SMS->append((char) (num >> 8));
	 
	 num = _device->get(byteOffset[id]+30033);
	 _SMS->append((char) (num & 0xFF));
	 _SMS->append((char) (num >> 8)); _SMS->append(": ");

num=((long)_device->get(byteOffset[id]+0x7545)) <<16;
num|=_device->get(byteOffset[id]+0x7546);
_SMS->appendLong(num);  _SMS->append("h, ");

if(_device->get(byteOffset[id]+0x7537)==1) _SMS->append("IGNON, ");
else _SMS->append("IGNOFF, ");

_SMS->append("Vb:"); _SMS->appendDouble( 2.950043e-3*_device->get(byteOffset[id]+0x7538), 2);   _SMS->append(", ");

_SMS->append("Vsp:"); _SMS->appendDouble( 4.246521e-3*_device->get(byteOffset[id]+0x753a), 2);  _SMS->append(", ");

_SMS->append("Vref:"); _SMS->appendDouble( 2.950043e-3*_device->get(byteOffset[id]+0x7540), 2); _SMS->append(", ");

_SMS->append("Achrg:"); _SMS->appendDouble( 2.034515e-3*_device->get(byteOffset[id]+0x753b), 2);  _SMS->append(", ");

num=((long)_device->get(byteOffset[id]+0x7535)) <<16;
num|=_device->get(byteOffset[id]+0x7536);
_SMS->append("Ahd:"); _SMS->appendDouble( 2.034515e-3*num, 1);  _SMS->append(", ");


//num=((long)_device->get(byteOffset[id]+0x7541)) <<16;
//num|=_device->get(byteOffset[id]+0x7542);
//_SMS->append("Ahr:"); _SMS->appendDouble( (int) 2.950043e-3*num,0);   _SMS->append(", ");
//
//num=((long)_device->get(byteOffset[id]+0x7543)) <<16;
//num|=_device->get(byteOffset[id]+0x7544);
//_SMS->append("Aht:"); _SMS->appendDouble( (int) 2.950043e-3*num,0);   _SMS->append(", "); 

_SMS->append("Ti:"); _SMS->appendDouble( _device->get(byteOffset[id]+0x753e),0);   _SMS->append(", ");

 _SMS->append("Tb:");_SMS->appendDouble( _device->get(byteOffset[id]+0x753f),0);   _SMS->append(", ");

State(_device->get(byteOffset[id]+0x7532),_SMS); _SMS->append(", ");

num=_device->get(byteOffset[id]+0x7531);
_SMS->appendLong(num);   _SMS->append("m, ");

num=_device->get(byteOffset[id]+0x754C);

//_SMS->append(", PWM:");
if(num>=230) _SMS->append("100");
else _SMS->appendDouble( num/2.3, 1); 

_SMS->append("%, ");

_SMS->append("Al:");
num=_device->get(byteOffset[id]+0x7547);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					if(nalm>0) _SMS->append(",");
					_SMS->appendInt(i);  nalm++;}
				mask<<=1;
			}
num=_device->get(byteOffset[id]+0x754D);
			mask=00000001;
			for (i=0; i<8; i++)
			{
				if(num & mask) {
					if(nalm>0) _SMS->append(",");
					_SMS->appendInt(16+i);  nalm++;}
				mask<<=1;
			}
			if(nalm==0) {_SMS->append("-"); _SMS->append(", ");}



_SMS->append(" Fl:");
num=_device->get(byteOffset[id]+0x7548);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					if(nflt>0) _SMS->append(",");
					_SMS->appendInt(i); nflt++;}
				mask<<=1;
			}
			if(nflt==0) {_SMS->append("-"); _SMS->append(" ");}


return (nalm>0||nflt>0);
}








void Tristar::writeAlarmstoString(byte id, MyString *_SMS)
{

word num;
        byte nalm=0, nflt=0, mask=1;
		int i;

		_SMS->free();

			 num = _device->get(byteOffset[id]+30030);
	 _SMS->appendInt(num & 0xFF);
	 _SMS->appendInt(num >> 8);
	 
	 num = _device->get(byteOffset[id]+30031);
	 _SMS->appendInt(num & 0xFF);
	 _SMS->appendInt(num >> 8);
	 
	 num = _device->get(byteOffset[id]+30032);
	 _SMS->appendInt(num & 0xFF);
	 _SMS->appendInt(num >> 8);
	 
	 num = _device->get(byteOffset[id]+30033);
	 _SMS->appendInt(num & 0xFF);
	 _SMS->appendInt(num >> 8); _SMS->append(": ");

_SMS->append("Alarms: ");

num=_device->get(byteOffset[id]+0x7547);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					Alarms(i,_SMS);
					if(nalm>0) _SMS->append(",");
					 nalm++;}
				mask<<=1;
			}
num=_device->get(byteOffset[id]+0x754D);
			mask=00000001;
			for (i=0; i<8; i++)
			{
				if(num & mask) {
					Alarms(16+i,_SMS);
					if(nalm>0) _SMS->append(",");
					  nalm++;}
				mask<<=1;
			}
			if(nalm==0) {_SMS->append("No alarms"); _SMS->append(" ");}

_SMS->append(". Faults: ");
num=_device->get(byteOffset[id]+0x7548);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					Faults(i,_SMS);
					if(nflt>0) _SMS->append(",");
					  nflt++;}
				mask<<=1;
			}
			if(nalm==0) {_SMS->append(" No faults"); _SMS->append(" ");}


_SMS->append(". Dip Switch: ");
num=_device->get(byteOffset[id]+0x7549);
			mask=00000001;
			for (i=0; i<8; i++)
			{
				if(num & mask) {_SMS->append("1");}
				else {_SMS->append("0");}
				mask<<=1;
			}

return;
}









void Tristar::sendMsg(){

		int i;
		//send the reply to the serial UART
		//Senguino doesn't support a bulk serial write command....

				if(prout)
				{
					 //Serial.print("len:"); Serial.println(_len);
					for (i=0 ; i < _len ; i++){ 
		 Serial.print((int)_msg[i]); Serial.print(":");}
		 Serial.println("out");Serial.println();}
		
		for(i = 0 ; i < _len ; i++) {tristarSerial->write(_msg[i]);}

resetSerial();

}

void Tristar::printMemory(byte id)
{
word num;
        byte nalm=0, nflt=0, mask=1;
		int i;
		MyString _tmpSMS;


	 num = _device->get(byteOffset[id]+30030);
	 Serial.write(num & 0xFF);
	 Serial.write(num >> 8);
	 
	 num = _device->get(byteOffset[id]+30031);
	 Serial.write(num & 0xFF);
	 Serial.write(num >> 8);
	 
	 num = _device->get(byteOffset[id]+30032);
	 Serial.write(num & 0xFF);
	 Serial.write(num >> 8);
	 
	 num = _device->get(byteOffset[id]+30033);
	 Serial.write(num & 0xFF);
	 Serial.write(num >> 8);

	 Serial.print(": ");





num=((long)_device->get(byteOffset[id]+0x7545)) <<16;
num|=_device->get(byteOffset[id]+0x7546);
Serial.print(num); Serial.print("h ");

if(_device->get(byteOffset[id]+0x7537)==1) Serial.print("IGNON ");
else Serial.print("IGNOFF ");

Serial.print("V_b: ");Serial.print(2.950043e-3*_device->get(byteOffset[id]+0x7538));Serial.print("V ");

Serial.print("V_sp: "); Serial.print(4.246521e-3*_device->get(byteOffset[id]+0x753a)); Serial.print("V ");

Serial.print("V_ref: "); Serial.print(2.950043e-3*_device->get(byteOffset[id]+0x7540));Serial.print("V ");

Serial.print("A_chrg: ");Serial.print(2.034515e-3*_device->get(byteOffset[id]+0x753b)); Serial.print("A ");

num=((long)_device->get(byteOffset[id]+0x7535)) <<16;
num|=_device->get(byteOffset[id]+0x7536);
Serial.print(" Ah_d: ");Serial.print(2.950043e-3*num); 

num=((long)_device->get(byteOffset[id]+0x7541)) <<16;
num|=_device->get(byteOffset[id]+0x7542);
Serial.print(" Ah_r: "); Serial.print(2.950043e-3*num); 

num=((long)_device->get(byteOffset[id]+0x7543)) <<16;
num|=_device->get(byteOffset[id]+0x7544);
Serial.print(" Ah_t: "); Serial.print(2.950043e-3*num); 

Serial.print(" T_in: "); Serial.print(_device->get(byteOffset[id]+0x753e)); Serial.print("C ");

Serial.print("T_out: "); Serial.print(_device->get(byteOffset[id]+0x753f)); Serial.print("C ");

State(_device->get(byteOffset[id]+0x7532),&_tmpSMS);
_tmpSMS.print();  _tmpSMS.free(); Serial.print(" ");

num=_device->get(byteOffset[id]+0x7531);
Serial.print(num); Serial.print("m ");

num=_device->get(byteOffset[id]+0x754C);

Serial.print("PWM: ");
if(num>=230) Serial.print("100");
else Serial.print((double)num/2.3); 

Serial.println("% ");

num=_device->get(byteOffset[id]+0x7547);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					Alarms(i,&_tmpSMS);
					_tmpSMS.print();  _tmpSMS.free(); Serial.print(", "); nalm++;}
				mask<<=1;
			}
num=_device->get(byteOffset[id]+0x754D);
			mask=00000001;
			for (i=0; i<8; i++)
			{
				if(num & mask) {
					Alarms(i,&_tmpSMS);
					_tmpSMS.print();  _tmpSMS.free(); Serial.print(", "); nalm++;}
				mask<<=1;
			}
			if(nalm==0) {Serial.print("No alarms "); Serial.print(" ");}


num=_device->get(byteOffset[id]+0x7548);
			mask=0000000000000001;
			for (i=0; i<16; i++)
			{
				if(num & mask) {
					Faults(i,&_tmpSMS);
					_tmpSMS.print();  _tmpSMS.free(); Serial.print(", "); nalm++;}
				mask<<=1;
			}
			if(nalm==0) {Serial.print("No faults "); Serial.print(" ");}


num=_device->get(byteOffset[id]+0x7549);
			mask=00000001;
			for (i=0; i<8; i++)
			{
				if(num & mask) {Serial.print("1");}
				else {Serial.print("0");}
				mask<<=1;
			}
			Serial.println();

  return;
}






void Tristar::sendStatus(word startreg, word numregs)
{
	//Serial.println(_sid);
	//Serial.println(_funcType);

	if(_sid!=ARD&&(_funcType==WRITE_AO||_funcType==WRITE_DO)) return;

	word val;
	byte i = 0;

	//if the function is to read analog inputs then add 30001 to the start register
	//else add 40001 to the start register
	if(startreg<30000)
	{
	if(_funcType == READ_AO) 		startreg += 30000;
	else startreg += 40000;
	}

	//Serial.println(startreg);
	

	//calculate the query reply message length
	//for each register queried add 2 bytes
	_len = numregs * 2;
	//allow room for the Device ID byte, Function type byte, data byte count byte, and crc word
	_len += 5;

	//allocate memory for the query response
	if(_msg!=NULL) free(_msg);
	_msg = (byte *) malloc(_len);

	//write the device ID
	_msg[0] = _device->getId();
	//write the function type
	_msg[1] = _funcType;
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
	
	calcCrc();

	_msg[_len - 2] = _crc >> 8;
	_msg[_len - 1] = _crc & 0xFF;

	//if(dbg){
	//	Serial.print("_len:"); Serial.println(_len);
	//Serial.print("_crc:"); Serial.println(_crc); 
	//Serial.print("_msg[_len - 1]:"); Serial.println(_msg[_len - 1]); 
	//Serial.print("_msg[_len - 2]:"); Serial.println(_msg[_len - 2]); 
	//}


	sendMsg();
}














#if defined(readfromEEPROM)
byte Tristar::auchCRCHi(byte index){
	//if(prnt) {Serial.print(index);Serial.print(" e "); Serial.println(READEEPROM(Offset_auchCRCHi+index));}
	return READEEPROM(Offset_auchCRCHi+index);}

byte Tristar::auchCRCLo(byte index){
	//if(prnt) {Serial.print(index);Serial.print(" e "); Serial.println(READEEPROM(Offset_auchCRCLo+index));}
	return READEEPROM(Offset_auchCRCLo+index);}

byte Tristar::COMMAND(byte index){
	//if(prnt) {Serial.print(index);Serial.print(" e "); Serial.println(READEEPROM(Offset_COMMAND+index));}
return READEEPROM(Offset_COMMAND+index);}

//void Tristar::State(byte index, char* state){
//	if(index>8){state=" "; return;}
//	for (int j=0 ; j < 12 ; j++) state[j]=(char)READEEPROM(Offset_State+index*12+j);
//	return ;}
//
//void Tristar::Alarms(byte index, char* state){
//	if(index>23){state=" "; return;}
//	for (int j=0 ; j < 17 ; j++) state[j]=(char)READEEPROM(Offset_Alarms+index*17+j);
//	return ;}
//
//void Tristar::Faults(byte index, char* state){
//	if(index>14){state=" "; return;}
//	for (int j=0 ; j < 17 ; j++) state[j]=(char)READEEPROM(Offset_Faults+index*17+j);
//	return ;}

void Tristar::State(byte index, MyString* state){
	if(index>8){return;}
	char c;
	for (int j=0 ; j < 12 ; j++) {
		c = (char)READEEPROM(Offset_State+index*12+j);
		if(c!=' ') state->append(c);}
	return ;}

void Tristar::Alarms(byte index, MyString* state){
	if(index>23){return;}
	char c;
	for (int j=0 ; j < 17 ; j++) {
		c = (char)READEEPROM(Offset_Alarms+index*17+j);
		if(c!=' ') state->append(c);}
	return ;}

void Tristar::Faults(byte index, MyString* state){
	if(index>14){return;}
	char c; 
	for (int j=0 ; j < 17 ; j++) {
		c = (char)READEEPROM(Offset_Faults+index*17+j);
		if(c!=' ') state->append(c);
	}
	return ;}

#else
byte Tristar::auchCRCHi(byte index){
	//if(prnt) {Serial.print(index);Serial.print(" "); Serial.println(_auchCRCHi[index]);}
	return _auchCRCHi[index];}

byte Tristar::auchCRCLo(byte index){
		//if(prnt) {Serial.print(index);Serial.print(" "); Serial.println(_auchCRCLo[index]);}
return _auchCRCLo[index];}

byte Tristar::COMMAND(byte index){
		//if(prnt) {Serial.print(index);Serial.print(" "); Serial.println(_COMMAND[index]);}
return _COMMAND[index];}

void Tristar::State(byte index, MyString* state){
	if(index>8){return;}
	char c;
	for (int j=0 ; j < 12 ; j++) {
		c = (char) _State[index][j];
		if(c!=' ') state->append(c);}
	return ;}

void Tristar::Alarms(byte index, MyString* state){
	if(index>23){return;}
	char c;
	for (int j=0 ; j < 17 ; j++) {
		c = (char) _Alarms[index][j];
		if(c!=' ') state->append(c);}
	return ;}

void Tristar::Faults(byte index, MyString* state){
	if(index>14){return;}
	char c; 
	for (int j=0 ; j < 17 ; j++) {
		c = (char) _Faults[index][j];
		if(c!=' ') state->append(c);
	}
	return ;}
#endif



