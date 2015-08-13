#include <tristarMaster.h>



tristarMaster::tristarMaster()
{
		timerlogTristar=NULL;
}

void tristarMaster::reset(){
whoisPOOR=-1;
	resetConversation();

	if(timerlogTristar!=NULL) timerlogTristar->deleteTimer(); timerlogTristar=NULL;
    timerlogTristar= t.setInterval(logTristarInterval, LOGTRISTAR);

Tristar::reset();

}

void tristarMaster::resetConversation(void){

deviceconnected=false;
tristarLogStarted=false;
tristarLogReceived=false;

Tristar::resetConversation();

}




void tristarMaster::stopTimers(void){
	 timerlogTristar->disable();
	 Tristar::stopTimers();
 }
void tristarMaster::startTimers(void){
	 timerlogTristar->enable();
	 Tristar::startTimers();
 }



void tristarMaster::preprocessMessage(byte n)
{
	switch(_funcType)
	{
	case READ_DI:
	case READ_DO:
	case READ_AI:
	case READ_AO:
	case WRITE_DO:
	case WRITE_AO:

		if(_len==3) {
		_ndata=n; 
		if(_msg!=NULL) free(_msg);
		_msg = (byte*) malloc(_ndata+5);
		_msg[0]= _device->getId();
		_msg[1]=	 _funcType;
		_msg[2]=	 _ndata;
		}	
		else if(_len>3) {
			_msg[_len-1]=n;
		}

		if(_ndata>0&&_len==_ndata+5) _ended=true; 


		break;
	case IDENTIFY:
		break;
	
	default:
		break;
	}
}

void tristarMaster::connectDevice(void){
startConvTimeout();
tristarLogStarted=false;

if(prnt) Serial.println("connecting Device");

_funcType=READ_AO;
getStatus(61440, 4);

}



void tristarMaster::getAnalogStatus(void){

startConvTimeout();
tristarLogStarted=false;

	_funcType=READ_AO;
	getStatus(8, 22);

	
	if(prnt) Serial.println("reading Analog Status");
	return;
}








void tristarMaster::execCommand(int nexe)		
{

	char state[5];
	if(prnt)  Serial.print("Executing ");if(prnt) Serial.println(nexe);

// toggle digital pins
if(nexe>-1&&nexe<54) { 
//arp.setPin(nexe,!arp.getPin(nexe)); 
if(prnt){ Serial.print("Pin "); Serial.print(nexe); Serial.print(" is ");  
Serial.println(digitalRead(nexe));}

} 
else switch(nexe){



case 60: {sendmemorytoARC(TRISTAR14); break;}
 case 61: {sendmemorytoARC(TRISTAR12); break;}

		  case 90: RESETALL(); break;
//case 70: arp.resetSolar(); break;
//	case 71: arp.AllSolarto1(); break;
//		case 72: arp.AllSolarto2(); break;
	

default: break;	 
//
}


}	


void tristarMaster::sendmemorytoARC(byte id){
	setSerial(ARD);

	delay(1000);

	if (id==TRISTAR14) _funcType=WRITE_AO;
	else if(id==TRISTAR12) _funcType=WRITE_DO;

	sendStatus(30001+byteOffset[id],33);

	SUCCESS();


return;
}

void tristarMaster::update(byte id){

	unsigned long val;  
		  //record when state changed last 
	if(_device->get(byteOffset[id]+0x7532)!=_device->get(byteOffset[id]+0x754B)){
		_device->set(byteOffset[id]+0x7532,_device->get(byteOffset[id]+0x754B));
                
		val=millis();

		//write new value for since change
		_device->set(byteOffset[id]+0x7533,val >> 16);
		_device->set(byteOffset[id]+0x7534,val & byteOffset[id]+0xFFFF);
		}
	
		//read last state change
		val=((long)_device->get(byteOffset[id]+0x7533)) <<16;
		val|=_device->get(byteOffset[id]+0x7534);

	
		//write new value for since change (in mins)
		_device->set(byteOffset[id]+0x7531,(long) (millis()-val)/60000);

		//update Ah counter 
		if(_device->get(byteOffset[id]+0x7532)==3) 	{//if night
			_device->set(byteOffset[id]+0x7535,0 >> 16);
			_device->set(byteOffset[id]+0x7536,0 & byteOffset[id]+0xFFFF);
		}	
		else {
		//read existing value
			val=((long) _device->get(byteOffset[id]+0x7535)) <<16;
			val|=_device->get(byteOffset[id]+0x7536);

			//update
			val|=_device->get(byteOffset[id]+0x753B);

			//write new value
			_device->set(byteOffset[id]+0x7535,val >> 16);
			_device->set(byteOffset[id]+0x7536,val & byteOffset[id]+0xFFFF);
		}

		batteryCondition[id]=NORMAL;

		if(_device->get(byteOffset[id]+0x7532)==5) batteryCondition[id]=POOR; //BULK
		else if(_device->get(byteOffset[id]+0x7532)==7) batteryCondition[id]=RICH; //FLOAT
		else if(_device->get(byteOffset[id]+0x7532)==6) { //PWM
		
			val=_device->get(byteOffset[id]+0x754C);

			if(val>180) batteryCondition[id]=POOR;
			else if(val<60) batteryCondition[id]=RICH;

		}

		if(prnt){
		if (batteryCondition[id]==POOR) Serial.println("POOR");
		else if (batteryCondition[id]==RICH) Serial.println("RICH");
		else Serial.println("NORMAL");
		}

		
		if(id==TRISTAR12){

			if(whoisPOOR!=-1) //regulating
			{
				if(batteryCondition[whoisPOOR]==RICH) //stop regulating
				{
					whoisPOOR=-1;
					  RESETSOLAR();
				}			
			}
			else //not regulating
			{
				if(batteryCondition[TRISTAR12]==POOR&&batteryCondition[TRISTAR14]==RICH) {ALLSOLARTO2(); whoisPOOR=TRISTAR12;}
				else  if(batteryCondition[TRISTAR14]==POOR&&batteryCondition[TRISTAR12]==RICH) {ALLSOLARTO1(); whoisPOOR=TRISTAR14;}
				else whoisPOOR=-1;
			}
		}
return;
}

void tristarMaster::logTristar(byte id){

//if(prnt) {Serial.print("logTristar"); Serial.println(id);}

	setSerial(id);

	resetConversation();

	tristarLogStarted=true;


}


void tristarMaster::run()
{
	//Serial.print(tristarLogStarted); Serial.print(deviceconnected);  Serial.println(tristarLogReceived);
	
	if(tristarLogStarted)
	{
		if(!deviceconnected) connectDevice();
		else getAnalogStatus();
	}
	else if(tristarLogReceived)
	{
			if(prnt) {Serial.println("updating"); }
			
			update((byte) _sid);

			if(prnt) Serial.println("writing to SD");

			WRITEMEMORYTOSD((byte) _sid);

			//sendmemorytoARC((byte) _sid);

			setSerial(ARD);

			exitConvTimeout();

	}


	Tristar::run();
	return ;
	}



void tristarMaster::processMessage(void)
{
	byte deviceId;
	word field1;
	word field2;
	word startreg;
	
	int i, ndata,j, j0, nobj;

 long num;
	byte mask=1;

	if(dbg) Serial.println("MASTER RECEIVED..");

	delay(100);


	if(_funcType == WRITE_AO) //request from slave
	{
	
	ndata = _msg[2];
	
				if(ndata==2) 
				{
				//copy field 1 from the incoming query
				field1  = (_msg[3] << 8) | _msg[4];

				exitSerialTimeout();

				SUCCESS();

				execCommand((int) field1);

				return;

				}

	}



		if(_funcType == READ_AI || _funcType == READ_AO)
		{
			ndata = _msg[2];

			if(!deviceconnected) {
				tristarLogStarted=true;
				deviceconnected=true; 
				if(prnt) Serial.println("connected..");
				startreg=30030+byteOffset[_sid];
				}
			else 
			{
				startreg=30008+byteOffset[_sid]; 

				resetConversation();

				tristarLogReceived=true;
			}

				for(i = 0 ; i < ndata/2 ; i++) 
				{
				//copy field 1 from the incoming query
				field1  = (_msg[3+2*i] << 8) | _msg[4+2*i];

				//if(dbg) {Serial.print(startreg+i); Serial.print("<-"); Serial.println((int) field1); }

				setStatus(startreg+i, field1);

				}
	 
		}


		
	Tristar::processMessage();
	return;
	}

