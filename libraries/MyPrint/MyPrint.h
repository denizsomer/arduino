#ifndef MyPrint_h
#define MyPrint_h

class MyPrint
{
	

  private:
	  bool phone;
  public:
	  MyPrint();
    
	void setphone(){phone=true;}
    void dprint(const char[]);
    /*void dprint(char);
    void dprint(unsigned char);
    void dprint(int);
    void dprint(unsigned int);
    void dprint(long);
    void dprint(unsigned long);
    void dprint(double);*/

    void dprintln(const char[]);
    //void dprintln(char);
    //void dprintln(unsigned char);
    //void dprintln(int);
    /*void dprintln(unsigned int);
    void dprintln(long);
    void dprintln(unsigned long);
    void dprintln(double);
    void dprintln(void);*/
};

#endif
