
#include <Keypad.h>
#include <EEPROM.h>
#include <math.h>
#include "SevSeg.h"
SevSeg seg1;
const byte MAX_KEYLEN = 4;
const byte rows=4;
const byte cols=3;
char getInput(char output[]);
// Declare the layout of the keypad
char keys [rows][cols]={
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'},
};
//Declare input pins for the Arduino
const byte rowPins[rows]={12,8,7,5};
const byte colPins[cols]={4,3,2};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

void clearInput(char array[]);
bool compareCode(char code1[],char code2[]);
void changeCode(char currentCode[]);
byte writeEEPROM(char array[],byte adress);
char getCode(byte adressStart,char output[]);
//int convertChar(char arr[]);
//void displayIntFor(int num, int t);
void displayCharFor(char ch[], int t);
char code[MAX_KEYLEN+1]={'1','2','3','\0'};
char input_code[MAX_KEYLEN+2]={'\0'}; //+2 to store \0 and a function operator
byte currentPos=0;

void setup() {
 const byte numDigits = 4;
 const byte digitPins[] = {11, 10, 9, 6};
 const byte segmentPins[] = {13, 14, 15, 16, 17, 18, 19,20};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false; // Default. Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  seg1.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros);
  seg1.setBrightness(90);

  Serial.begin(9600);
  getCode(0,code);
  Serial.println(code);
  }

void loop() {
  if(currentPos==0)seg1.setChars("----"); 
    else {
      seg1.setChars(input_code);
    }
  char temp=getInput(input_code);
  if(temp=='#'){changeCode(code);}
  else{
  if (temp=='*'||currentPos>MAX_KEYLEN-1){
      //If set code and input code mismatch
      if(compareCode(code,input_code) == false){
        Serial.println("WRONG CODE!!!");
        displayCharFor(input_code,500);
        displayCharFor("ERR",1000);
        currentPos=0;
        clearInput(input_code);
      } 
      //If code is correct
      else{
        Serial.println("UNLOCKED!");
        displayCharFor(code,500);
        displayCharFor("OPEN",1000);
        currentPos=0;
        clearInput(input_code);
    }
  }
 }
seg1.refreshDisplay();
}
char getInput(char output[]){
   char key = keypad.getKey();
  if(key != NO_KEY){
    if(key!='*'&&key!='#'){
      output[currentPos]=key;
      Serial.println(output);
    }
    //Writes 0 byte to next position
    output[currentPos+1]='\0';
    currentPos++;
    return key;
  }
}
bool compareCode(char code1[],char code2[]){
  for(byte i=0; i <= strlen(code2);i++){
    if(code1[i]!=code2[i]){return false;}
  } return true;
}
void clearInput(char array[]){
  for(byte i=0; i<=MAX_KEYLEN;i++){
    array[i]='\0';
    }
}
void changeCode(char currentCode[]){
  clearInput(input_code);
  currentPos=0;
  Serial.println("Enter current code:");
  displayCharFor("CURR",1000);
  while(true){
    seg1.setChars(input_code);
    if(getInput(input_code)=='*'||currentPos==4){
      displayCharFor(input_code,500);
      break;
    }
    seg1.refreshDisplay();
  }
  if(compareCode(currentCode,input_code)== false){
    Serial.println("WRONG CODE, aborting...");
    displayCharFor(input_code,1000);
    displayCharFor("ERR",1000);
    clearInput(input_code);
    currentPos=0;
    return;
  }
  currentPos=0;
  clearInput(input_code);
  displayCharFor("nEU",1000);
  Serial.println("Enter new code.(MAX 16-digits)Press '*' to enter.");
  while(strlen(input_code)<MAX_KEYLEN){
    seg1.setChars(input_code);
    if(getInput(input_code)=='*'){
      displayCharFor(input_code,1000);
      break;
    }
    seg1.refreshDisplay();
  }
  currentPos=0;
  Serial.println("Enter code again. Press '*' to enter.");
  displayCharFor("nEU2",1000);
  char verifyCode[MAX_KEYLEN+1]={'\0'};
  while(strlen(verifyCode) < MAX_KEYLEN){
    seg1.setChars(verifyCode);
    if(getInput(verifyCode)=='*')break;
    seg1.refreshDisplay();
  }
  displayCharFor(verifyCode,1000);
  if (compareCode(input_code,verifyCode)==false){
    Serial.println("Codes do not match. Aborting...");
    displayCharFor("Err",1000);
    clearInput(input_code);
    clearInput(verifyCode);
    currentPos=0;
    return;
    }
  clearInput(currentCode);
  for(byte i=0; i < strlen(input_code);i++){
    currentCode[i]= input_code[i];
  }
  writeEEPROM(currentCode,0);
  Serial.println("NEW CODE:");
  Serial.println(currentCode);
  displayCharFor("CODE",1500);
  displayCharFor(code,2000);
  clearInput(input_code);clearInput(verifyCode);
  currentPos=0;
  return;
}

byte writeEEPROM(char array[],byte adress){
  // +1 because we want the \0 byte.
  byte i=adress;
  for (i; i <strlen(array);i++){
    EEPROM.write(i,array[i]);
  }
  EEPROM.write(i,NULL);
}
char getCode(byte adressStart,char output[]){
  byte i=adressStart;
  char cValue=1;
  while(cValue!='\0'){
    byte bValue=EEPROM.read(i);
    cValue = char (bValue); 
    output[i]= cValue;
    i++;
  }
  //Returns first byte so that setup function can check for \0
  return output[0];
}
/*
int convertChar(char arr[]){
  int i=0;
  sscanf(arr, "%d", &i);
  return i;
}

void displayIntFor(int num, int t){
  unsigned long start_time= millis();
  unsigned long elapsed=0;
  while(elapsed<start_time+t){
    seg1.setNumber(num);
    elapsed=millis();
    seg1.refreshDisplay();
  }
}*/
void displayCharFor(char ch[], int t){
  unsigned long start_time= millis();
  unsigned long elapsed=0;
  while(elapsed<start_time+t){
    seg1.setChars(ch);
    elapsed=millis();
    seg1.refreshDisplay();
  }
}



