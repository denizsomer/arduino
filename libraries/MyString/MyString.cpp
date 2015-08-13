#include "MyString.h"

MyString::MyString(void)
{
  ch = NULL;

  
  return;
}



MyString::MyString(char *strg)
{
  ch = new char[strlen(strg)+1];

  sprintf(ch,strg);


  return;
}



MyString::~MyString()
{
  if (ch != NULL) delete [] ch; ch = NULL;


  return;
}



char &MyString::operator[](int i)
{
  if (ch == NULL || strlen(ch)-1<i) 
    { Serial.print( " ERROR! MyString operator [],  i > length\n\n"); exit(0); }
  
  return ch[i];
}
MyString &MyString::append(MyString &strg2)
{
  return append(strg2.ch);
}



MyString &MyString::append(char *strg2)
{
  if (strg2 == NULL)  return *this;

  int i, l1 = length(), l2 = strlen(strg2);
  if (ch == NULL) l1 = 0;
  
  char *tmp = new char[l1 + l2 + 1];
  
  for (i=0; i<l1; i++) tmp[i]    = ch[i]; 
  for (i=0; i<l2; i++) tmp[i+l1] = strg2[i];  
  tmp[l1+l2] = '\0';

  if (ch != NULL) delete [] ch;

  ch = tmp;

  return *this;  
}



MyString &MyString::append(char c)
{
  char tmp[2]; tmp[0] = c; tmp[1] = '\0';  return append(tmp);
}




MyString &MyString::free(void)
{
  if (ch != NULL) delete [] ch; 
  
  ch = NULL;

  return *this;
}



char *MyString::asCharArray(void)
{
   return ch;
}



int MyString::length(void)
{
   if (ch == NULL) return -1;
   
   return strlen(ch);
}




bool MyString::toInt(int *i, bool errMsg) 
{
  if (!scanInt(ch,i,errMsg)) return false;

  return true;
}



void MyString::print(void)
{
  if (ch != NULL)  Serial.print(ch);
}

void MyString::appendDouble(double val, int nfrac){

float dval=abs(val);
long ival;
int i, ndigits=1, div=1;

if(val<dval) append('-');
val=dval;

while(floor(dval) >= 10) {ndigits++; div*=10; dval=val/div;}

ival=(long) floor(val);

appendLong(ival);

//if(ival==0)return;

this->append('.');

val-=ival;

i=0;
while(i<nfrac) {val*=(10.); i++;}

appendLong((long) floor(val));

return;
}



void MyString::appendTime(unsigned long val){

byte hours, minutes, seconds;

val/=1000;

hours=(byte) floor(val/3600);
val-=3600*hours;

minutes=(byte) floor(val/60);
val-=60*minutes;

seconds=(byte) floor(val);


this->append(hours);
this->append("h "); 

this->append(minutes);
this->append("m"); 

//this->append(seconds);
//this->append("s");


return;
}




void MyString::appendInt(int val){

int ival=abs(val), div=1;
int i, j, ndigits=1;

if(val<ival) append('-');
val=ival;

while(ival>=10) {ndigits++; div*=10; ival=val/div;}

int reverse[ndigits];

ival=val;

for (i=0 ; i < ndigits-1; i++)
{
	ival=val/10; 
	j=val-10*ival;

	reverse[ndigits-1-i]=j;

	val=ival;
}

reverse[0]=val;

for (j=0 ; j < ndigits; j++) append(reverse[j]);

return;
}

void MyString::appendLong(long val){

long ival=abs(val), div=1;
int i, j, ndigits=1;

if(val<ival) append('-');
val=ival;

while(ival>=10) {ndigits++; div*=10; ival=val/div;}

int reverse[ndigits];

ival=val;

for (i=0 ; i < ndigits-1; i++)
{
	ival=val/10; 
	j=val-10*ival;

	reverse[ndigits-1-i]=j;

	val=ival;
}

reverse[0]=val;

for (j=0 ; j < ndigits; j++) append(reverse[j]);

return;
}

MyString &MyString::append(int c){

 char tmp[2];  tmp[1] = '\0';  

if( c ==0) tmp[0] = '0';
else if( c ==1) tmp[0] = '1';
else if( c ==2) tmp[0] = '2';
else if( c ==3) tmp[0] = '3';
else if( c ==4) tmp[0] = '4';
else if( c ==5) tmp[0] = '5';
else if( c ==6) tmp[0] = '6';
else if( c ==7) tmp[0] = '7';
else if( c ==8) tmp[0] = '8';
else if( c ==9) tmp[0] = '9';

return append(tmp);

}





int MyString::hexToInt(int pos){

	if(pos<0||pos>=length()-1) return -666;

		return 16*hexcharToInt(pos)+hexcharToInt(pos+1);


}

int MyString::charToInt(int pos){

	//Serial.print("chartoint:"); Serial.print(pos);Serial.print("->"); Serial.println(ch[pos]);

	if(pos<0||pos>=length()) return 0;

	char c=ch[pos];

if( c =='0') return 0;
else if( c =='1') return 1;
else if( c =='2') return 2;
else if( c =='3') return 3;
else if( c =='4') return 4;
else if( c =='5') return 5;
else if( c =='6') return 6;
else if( c =='7') return 7;
else if( c =='8') return 8;
else if( c =='9') return 9;
else		return -6666;


}



int MyString::hexcharToInt(int pos){

	//Serial.print("chartoint:"); Serial.print(pos);Serial.print("->"); Serial.println(ch[pos]);

	if(pos<0||pos>=length()) return -6666;

	char c=ch[pos];

if( c =='0') return 0;
else if( c =='1') return 1;
else if( c =='2') return 2;
else if( c =='3') return 3;
else if( c =='4') return 4;
else if( c =='5') return 5;
else if( c =='6') return 6;
else if( c =='7') return 7;
else if( c =='8') return 8;
else if( c =='9') return 9;
else if( c =='A') return 10;
else if( c =='a') return 10;
else if( c =='B') return 11;
else if( c =='b') return 11;
else if( c =='C') return 12;
else if( c =='c') return 12;
else if( c =='D') return 13;
else if( c =='d') return 13;
else if( c =='E') return 14;
else if( c =='e') return 14;
else if( c =='F') return 15;
else if( c =='f') return 15;
else		return 0;


}


