const  int v_solarPanel = 0; //A0
const  int v_lbattery   = 1; //A2
const  int v_vbattery   = 2; //A2
const  int v_solarCont  = 3; //A3
const  int v_ignition   = 4; //A4
   
    //tested, accurate
const double f0=0.02600;
const double f1=0.01532;
const double f2=0.01580;
const double f3=0.01555;
const double f4=0.01445;

    void setup()
    {
    Serial.begin(9600); //initialize serial communication
    }

    void loop()
    {
      double Voltage[5];

      
readVoltages(&Voltage[0]);
writeVoltages(&Voltage[0]);





    delay(500); 
    
    }
    
        void writeVoltages(double* Voltage)
    {
     String msg = "SolarPanel : ";
msg+=printDouble(Voltage[0], 100);
msg += "V, Leis.Bat.: ";
msg+=printDouble(Voltage[1], 100);
msg += "V, Veh.Bat.: ";
msg+=printDouble(Voltage[2], 100);
msg+= "V, Solar Controller: ";
msg+=printDouble(Voltage[3], 100);
msg += "V, Ignition: ";
msg+=printDouble(Voltage[4], 100);
msg += "V";
Serial.println(msg); 
    }
    
    
    void readVoltages(double* Voltage)
    {

    double sensorValue = double(analogRead(v_solarPanel));
    Voltage[0]=sensorValue * f0;
    
    sensorValue = analogRead(v_lbattery);
    Voltage[1]=sensorValue * f1;

    sensorValue = analogRead(v_vbattery);
    Voltage[2]=sensorValue * f2;
    
    sensorValue = analogRead(v_solarCont);
    Voltage[3]=sensorValue * f3;

    sensorValue = analogRead(v_ignition);
    Voltage[4]=sensorValue * f4;

    
    }
    
    String printDouble( double val, unsigned int precision){
// prints val with number of decimal places determine by precision
// NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
// example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

String number;

    number= String (int(val));  //prints the int part
    number.concat("."); // print the decimal point
    
    unsigned int frac;
    if(val >= 0)
        frac = (val - int(val)) * precision;
    else
        frac = (int(val)- val ) * precision;
    number.concat(frac) ;
    
    return number;
} 

