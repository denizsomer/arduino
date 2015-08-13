


#ifndef HS
#define HS

class HardwareSerial
{
	public:
		HardwareSerial(void){};
		~HardwareSerial(void){};
		int available(){return 1;}

		void begin(int){};
		byte read(){return 0;};

		void print(char*){};
		void print(int){};
		void print(word){};
		void print(long){};
		void print(double){};
		void write(byte){};
		void println(){};
		void println(char*){};
		void println(int){};
		void println(double){};

 

};


#endif
