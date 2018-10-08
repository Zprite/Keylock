#include <Keypad.h>

const byte rows=2;
const byte cols=6;
void clearInput(char array[], byte arraylen);
// Declare the layout of the keypad
char keys [rows][cols]={
  {'1','2','3','4','5','*'},
  {'6','7','8','9','0','#'}
  };
//Declare input pins for the Arduino
byte colPins[cols]={13,12,11,10,9,8};
byte rowPins[rows]={7,6};
byte ledPin=2;
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

void setup() {
  Serial.begin(9600);
  pinMode(ledPin,OUTPUT);
}

const byte keylen=6;
char code[keylen+1]={'1','2','3','4','5','6','\0'};
char input_code[keylen+1];
byte currentPos=0;
void loop() {
  char key = keypad.getKey();
  if(key != NO_KEY){
    input_code[currentPos]=key;
    Serial.println(input_code);
    currentPos++;
    }
  //Checks the inputed code after key
  if (currentPos == keylen){
    for(byte i=0; i < keylen;i++){
      if(code[i]!=input_code[i]){
        Serial.println("WRONG CODE!!!");
        currentPos=0;
        clearInput(input_code,keylen);
        break;
      } 
      if(i==keylen-1){
        Serial.println("UNLOCKED!");
        digitalWrite(ledPin,HIGH);
        currentPos=0;
        clearInput(input_code,keylen);
        delay(1000);
        digitalWrite(ledPin,LOW);
      }
   }
 }
}

void clearInput(char array[], byte arraylen){
  for(byte i=0; i<arraylen;i++){
    array[i]='\0';
    }
}
  
