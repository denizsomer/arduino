#include <modbusSlave.h>
#include <modbus.h>
#include <modbusDevice.h>
#include <Arduino.h>

HardwareSerial* masterSerial=&Serial;

modbusSlave::modbusSlave()
{
}
/*
Set the Serial Baud rate.
Reconfigure the UART for 8 data bits, no parity, and 1 stop bit.
and flush the serial port.
*/
void modbusSlave::setBaud(word baud)
{
	_baud = baud;
	//calculate the time perdiod for 3 characters for the given bps in ms.
	_frameDelay = 24000/_baud;


	masterSerial->begin(baud);
	//Serial.begin(9600);

	// defaults to 8-bit, no parity, 1 stop bit
	//clear parity, stop bits, word length
//	UCSR0C = UCSR0C & B11000001;
//	UCSR0B = UCSR0B & B11111011; 

	//Set word length to 8 bits
//	UCSR0C = UCSR0C | B00000110;

	//No parity
//	UCSR0C = UCSR0C | B00000000;

	//1 Stop bit
//	UCSR0C = UCSR0C | B00000100;

	masterSerial->flush();
}

/*
Retrieve the serial baud rate
*/
word modbusSlave::getBaud(void)
{
	return(_baud);
}

/*
Generates the crc for the current message in the buffer.
*/

void modbusSlave::calcCrc(void)
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
		CRCHi = CRCLo ^ _auchCRCHi[Index];
		CRCLo = _auchCRCLo[Index];
	}
	_crc = (CRCHi << 8) | CRCLo;
}

/*
  Checks the UART for query data
*/
void modbusSlave::checkSerial(void)
{
	//while there is more data in the UART than when last checked
	while(masterSerial->available()> _len)
	{
		//update the incoming query message length
		_len = masterSerial->available();
		//Wait for 3 bytewidths of data (SOM/EOM)
//		delayMicroseconds(RTUFRAMETIME);
		delay(_frameDelay);
		//Check the UART again
	}
}

/*
Copies the contents of the UART to a buffer
*/
void modbusSlave::serialRx(void)
{
	byte i;

	//allocate memory for the incoming query message
	_msg = (byte*) malloc(_len);

		//copy the query byte for byte to the new buffer
		
		for (i=0 ; i < _len ; i++) _msg[i] = masterSerial->read();
		{Serial.print(i);Serial.print(":");Serial.println(_msg[i]);}
		Serial.println("in");
}

/*
Generates a query reply message for Digital In/Out status update queries.
*/
void modbusSlave::readDeviceIdentification(byte funcType, word startreg, word numregs)
{

	//Set the query response message length
	//Device ID byte, Function byte, Register byte, Value byte, CRC word
	_len = 47;
	//allocate memory for the message buffer.
	_msg = (byte *) malloc(_len);


	//write the device ID
	_msg[0] = _device->getId();

	_msg[1] = IDENTIFY;
	_msg[2] = SUBIDENTIFY;

	_msg[3] = 0X01; //READ DEVICE ID CODE
	_msg[4] = 0X00; //COMFORMITY LEVEL
	_msg[5] = 0X00; //MORE FOLLOWS
	_msg[6] = 0X00; //NEXT OBJECT ID
	_msg[7] = 0X03; //NUMBER OF OBJECTS

	_msg[8] = 0X00; //OBJECT id
	_msg[9] = 0X11; //OBJECT leNgth
	//"Morningstar Corp."           
	_msg[10] = 0X4d; 
	_msg[11] = 0X6f;
	_msg[12] = 0X72;
	_msg[13] = 0X6e;
	_msg[14] = 0X69;
	_msg[15] = 0X6e;
	_msg[16] = 0X67;
	_msg[17] = 0X73;
	_msg[18] = 0X74;
	_msg[19] = 0X61;
	_msg[20] = 0X72;
	_msg[21] = 0X20;
	_msg[22] = 0X43;
	_msg[23] = 0X6f;
	_msg[24] = 0X72;
	_msg[25] = 0X70;
	_msg[26] = 0X2e;


	_msg[27] = 0X01; //OBJECT id
	_msg[28] = 0X05; //OBJECT leNgth
	//"TS-45"
	_msg[29] = 0X54;
	_msg[30] = 0X53;
	_msg[31] = 0X2d;
	_msg[32] = 0X34;
	_msg[33] = 0X35;


	_msg[34] = 0X02; //OBJECT id
	_msg[35] = 0X09; //OBJECT leNgth
	//"v01.01.01"
	_msg[36] = 0X76;
	_msg[37] = 0X30;
	_msg[38] = 0X31;
	_msg[39] = 0X2e;
	_msg[40] = 0X30;
	_msg[41] = 0X31;
	_msg[42] = 0X2e;
	_msg[43] = 0X30;
	_msg[44] = 0X31;


	//calculate the crc for the query reply and append it.
	this->calcCrc();
	_msg[_len - 2]= _crc >> 8;
	_msg[_len - 1]= _crc & 0xFF;
}

void modbusSlave::getDigitalStatus(byte funcType, word startreg, word numregs)
{
	//initialize the bit counter to 0
	byte bitn =0;
	
	//if the function is to read digital inputs then add 10001 to the start register
	//else add 1 to the start register
	if(funcType == READ_DI)
		startreg += 10001;
	else
		startreg += 1;

	//determine the message length
	//for each group of 8 registers the message length increases by 1
	_len = numregs/8;
	//if there is at least one incomplete byte's worth of data
	//then add 1 to the message length for the partial byte.
	if(numregs%8)
		_len++;
	//allow room for the Device ID byte, Function type byte, data byte count byte, and crc word
	_len +=5;

	//allocate memory of the appropriate size for the message
	_msg = (byte *) malloc(_len);

	//write the slave device ID
	_msg[0] = _device->getId();
	//write the function type
	_msg[1] = funcType;
	//set the data byte count
	_msg[2] = _len-5;

	//For the quantity of registers queried
	while(numregs--)
	{
		//if a value is found for the current register, set bit number bitn of msg[3]
		//else clear it
		if(_device->get(startreg))
			bitSet(_msg[3], bitn);
		else
			bitClear(_msg[3], bitn);
		//increment the bit index
		bitn++;
		//increment the register
		startreg++;
	}
	
	//generate the crc for the query reply and append it
	this->calcCrc();
	_msg[_len - 2] = _crc >> 8;
	_msg[_len - 1] = _crc & 0xFF;
}

void modbusSlave::getAnalogStatus(byte funcType, word startreg, word numregs)
{
	word val;
	word i = 0;

	//if the function is to read analog inputs then add 30001 to the start register
	//else add 40001 to the start register
	if(funcType == READ_AO)
		startreg += 30001;
	else
		startreg += 40001;

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
}

void modbusSlave::setStatus(byte funcType, word reg, word val)
{
	//Set the query response message length
	//Device ID byte, Function byte, Register byte, Value byte, CRC word
	_len = 8;
	//allocate memory for the message buffer.
	_msg = (byte *) malloc(_len);


	//write the device ID
	_msg[0] = _device->getId();
	//if the function type is a digital write
	if(funcType == WRITE_DO)
	{
		//Add 1 to the register value and set it's value to val
		_device->set(reg + 1, val);
		//write the function type to the response message
		_msg[1] = WRITE_DO;
	}
	else
	{
		//else add 40001 to the register and set it's value to val
		_device->set(reg + 40001, val);

		//write the function type of the response message
		_msg[1] = WRITE_AO;
	}
	
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
}

void modbusSlave::run(void)
{

	byte deviceId;
	byte funcType;
	word field1;
	word field2;
	
	int i;
	
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
		return;
	}
	//calculate the checksum of the query message minus the checksum it came with.
	this->calcCrc();
	


	//if the checksum does not match, ignore the message
	if ( _crc != ((_msg[_len - 2] << 8) + _msg[_len - 1]))
		return;
	
	//copy the function type from the incoming query
	funcType = _msg[1];

		word field1;
	word field2;

	//copy fiield 1 from the incoming query
	field1	= (_msg[2] << 8) | _msg[3];

	//copy field 2 from the incoming query
	field2  = (_msg[4] << 8) | _msg[5];

		Serial.print(_msg[1]);
		Serial.print(" ");
		Serial.print(field1);
		Serial.print(" ");
		Serial.println(field2);
		Serial.println();

	//free the allocated memory for the query message

	free(_msg);
	//reset the message length;
	_len = 0;

	

		//return;

	//generate query response based on function type
	switch(funcType)
	{
	case READ_DI:
		this->getDigitalStatus(funcType, field1, field2);
		break;
	case READ_DO:
		this->getDigitalStatus(funcType, field1, field2);
		break;
	case READ_AI:
		this->getAnalogStatus(funcType, field1, field2);
		break;
	case READ_AO:
		this->getAnalogStatus(funcType, field1, field2);
		break;
	case WRITE_DO:
		this->setStatus(funcType, field1, field2);
		break;
	case WRITE_AO:
		this->setStatus(funcType, field1, field2);
		break;
	case IDENTIFY:
		this->readDeviceIdentification(funcType, field1, field2);
		break;
	default:
		return;
		break;
	}

	//if a reply was generated
	if(_len)
	{
		int i;
		//send the reply to the serial UART
		//Senguino doesn't support a bulk serial write command....
		
		for(i = 0 ; i < _len ; i++) masterSerial->write(_msg[i]);
		//{masterSerial->write(_msg[i]); Serial.print(i);Serial.print(":");Serial.println(_msg[i]);}
		//Serial.println("out");
		//free the allocated memory for the reply message
		
		free(_msg);
		//reset the message length
		_len = 0;
	}
}
