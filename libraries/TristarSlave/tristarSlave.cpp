#include "tristarSlave.h"
#include "Tristar.h"
#include "HardwareSerial.h"

tristarSlave::tristarSlave()
{
}

void tristarSlave::reset()
{
	 Tristar::reset();
}









void tristarSlave::writeCommand(int nexe)
{
	word val= (word) nexe;

	_len = 2;
	//allow room for the Device ID byte, Function type byte, data byte count byte, and crc word
	_len += 5;

	//allocate memory for the query response
	if(_msg!=NULL) free(_msg);
	_msg = (byte *) malloc(_len);

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






void tristarSlave::preprocessMessage(byte n)
{
	switch(_funcType)
	{
	case READ_DI:
	case READ_DO:
	case READ_AI:
	case READ_AO:
		if(_len==2){
		 if(_msg!=NULL) free(_msg);
		_msg = (byte*) malloc(8);
		_msg[0]= _device->getId();
		_msg[1]=	 _funcType;
		}
		else if(_len>2) _msg[_len-1]=n;

				if(_len==8) {_ended=true; break;}
		
		//Serial.print(PDU.asCharArray()[PDU.length()-1]); Serial.print(":");
		break;
	case WRITE_DO:
	case WRITE_AO:

		if(_len==3) {_ndata=n; 
		if(_msg!=NULL) free(_msg);
		_msg = (byte*) malloc(_ndata+5);
		_msg[0]= _device->getId();
		_msg[1]=	 _funcType;
		_msg[2]=	 _ndata;

	//Serial.print(_msg[0]); Serial.print(","); 
	//Serial.print(_msg[1]); Serial.print(",");
	//Serial.println(_msg[2]); 
		}	
		else if(_len>3) _msg[_len-1]=n;



		if(_ndata>0&&_len==_ndata+5) {_ended=true; break;}


		break;
	case IDENTIFY:
		//this->readDeviceIdentification(field1, field2);
		break;
	
	default:
		break;
	}
}
void tristarSlave::processMessage(){


	byte deviceId;
	word field1;
	word field2;
	word startreg;

	int i, ndata;
	byte rid;

	if(dbg) Serial.println("SLAVE RECEIVED");


	//generate query response based on function type
	switch(_funcType)
	{
	case READ_AI:
			case READ_AO:
	
	//copy fiield 1 from the incoming query
	field1	= (_msg[2] << 8) | _msg[3];

	//copy field 2 from the incoming query
	field2  = (_msg[4] << 8) | _msg[5];


		sendStatus(field1, field2);

		break;


	case WRITE_AO:
		if(dbg) Serial.print("WRITE_AO");
			case WRITE_DO:
				if(dbg) Serial.print("WRITE_DO");

			ndata = _msg[2];

	if (_funcType==WRITE_AO) rid=TRISTAR14;
	else if(_funcType==WRITE_DO) rid=TRISTAR12;

			startreg	= 30001+byteOffset[rid];

			for(i = 0 ; i < ndata/2 ; i++) 
			{
			//copy field 1 from the incoming query
			field1  = (_msg[3+2*i] << 8) | _msg[4+2*i];


			if(dbg)
			{ Serial.print(startreg+i); Serial.print("<-"); Serial.println(field1);}

			setStatus(startreg+i, field1);

			}

			delay(500);


			
				resetSerial();
				exitConvTimeout();

						
			//if(_funcType==WRITE_DO) REPORTMEMORY(TRISTAR12);
			//else if(_funcType==WRITE_AO) REPORTMEMORY(TRISTAR14);

		break;


	default:
		break;
	}

	t.setTimeout(1000,RESETSLAVE);

	Tristar::processMessage();
return;
}







void tristarSlave::run()
{

	Tristar::run();
	return ;
	}