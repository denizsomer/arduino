
#ifndef incl_MyString_h
#define incl_MyString_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
typedef unsigned char byte;
typedef unsigned int word;
#define _CRT_SECURE_NO_DEPRECATE
#include <stddef.h>
#include <iostream>
#include "HardwareSerial.h"
#endif

class MyString
{
  public:

    MyString(void);
    MyString(char*);
    virtual ~MyString();
 
    MyString &append(MyString &);
    MyString &append(char*);
    MyString &append(char);
	MyString &append(int);

	void print(void);

	    char &operator[](int);

    MyString &free(void);
    
    char* asCharArray(void);

    int length(void);

	    bool toInt(int *, bool errMsg = true);

		void appendDouble(double, int);
		void appendInt(int);
		void appendLong(long);
		void appendTime(unsigned long);

		int charToInt(int);
		int hexcharToInt(int);
		int hexToInt(int);

    
  private:

    char* ch;
};


inline bool charInCharArray(char c, char *s)
{
  int i = 0, l = strlen(s); 
  
  while(i < l && c != s[i]) i++;

  if (i < l) return true;

  return false;
}





inline bool scanInt(char *s, int *i, bool errMsg = true)
{
  int j = 0;
  if (s == NULL) goto error;
  
  if (strlen(s) == 0) goto error;
  
  while (charInCharArray(s[j],"1234567890+-")) j++;  
  if (s[j]!='\0') goto error;
  
  if (!sscanf(s,"%d",i)) goto error;
  
  return true;
  
  error:
  //  if (errMsg) std::cout << " ERROR in 'scanInt'!";   
    return false;
}  







#endif



