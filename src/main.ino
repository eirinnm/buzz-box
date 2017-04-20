#include <arduino.h>

const byte volPin[2] = {2,4};
const byte soundPin = 3;
const byte lightPin = 5;
const byte jumperPin = 11;
const byte buttonPin = 12;
const byte solenoidPin = 6;
bool isPlaying=false;
bool buttonPlaying=false;
unsigned long t;
unsigned long playUntil=0;
byte volNext;
byte background_brightness = 0;
unsigned long startNext=0; //for scheduling another tap
unsigned int lenNext;
unsigned int freq=200;
//SerialEvent variables
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup(){
  pinMode(soundPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(jumperPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  for(int i=0;i<2;i++){
    pinMode(volPin[i],OUTPUT);
    digitalWrite(volPin[i],HIGH);
  }
  Serial.begin(115200);
  Serial.print(inputString);
  Serial.println("Ready.");
}

void loop(){
  //check for commands on the serial input
  //Serial.println(digitalRead(jumperPin));
  //delay(1);
  if (stringComplete) {
    //Serial.println(inputString);
    if(inputString=="on"){
      //turn lights on
      background_brightness=255;
      setBrightness(background_brightness);
    }else if(inputString=="off"){
      background_brightness=0;
      setBrightness(background_brightness);
    }
    if(inputString.startsWith("H")){
      //set the frequency to some Hz
      inputString.remove(0,1);
      char command[inputString.length()];
      inputString.toCharArray(command, inputString.length()+1);
      char *i;
      char *h = strtok_r(command,",",&i);
      freq=atoi(h);
      Serial.print("Frequency set to ");
      Serial.print(h);
    Serial.println("Hz");
    }
    if(inputString.startsWith("B")){
      //set the lights to a certain brightness
      inputString.remove(0,1);
      char command[inputString.length()];
      inputString.toCharArray(command, inputString.length()+1);
      char *i;
      char *bright = strtok_r(command,",",&i);
      background_brightness=atoi(bright);
      setBrightness(atoi(bright));
    }
    if(inputString.startsWith("T")){
      //tap the solenoid for 50ms
      inputString.remove(0,1);
      char command[inputString.length()];
      inputString.toCharArray(command, inputString.length()+1);
      char *i;
      char *strength = strtok_r(command,",",&i);
      char *len = strtok_r(NULL,",",&i);
      tapNow(atoi(strength),atoi(len));
    }
    if(inputString.startsWith("F")){
      //set the lights to a certain brightness for x amount of time
      inputString.remove(0,1);
      char command[inputString.length()];
      inputString.toCharArray(command, inputString.length()+1);
      char *i;
      char *bright = strtok_r(command,",",&i);
      char *len = strtok_r(NULL,",",&i);
      flashNow(atoi(bright),atoi(len));
    }else if(inputString.startsWith("S")){
      inputString.remove(0,1);
      char command[inputString.length()];
      inputString.toCharArray(command, inputString.length()+1);
      char *i;
      char *vol = strtok_r(command,",",&i);
      char *len = strtok_r(NULL,",",&i);
      char *wait = strtok_r(NULL,",",&i);
      char *vol2 = strtok_r(NULL,",",&i);
      char *len2 = strtok_r(NULL,",",&i);
      if(vol && len){
        playNow(atoi(vol),atoi(len));
      }
      if(vol2 && len2){
        //schedule a tap
        playLater(atoi(wait),atoi(vol2),atoi(len2));
      }
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  //do we need to stop playback?
  if(isPlaying){
    if(millis()>playUntil){
      noTone(soundPin);
      digitalWrite(LED_BUILTIN,LOW);
      isPlaying=false;
    }
  }else{
    //check if we need to start a scheduled tap
    if((startNext>0) && (millis()>=startNext)){
      playNow(volNext, lenNext);
      startNext=0;
    }
  }
  //is the button being pressed?
  if(!digitalRead(buttonPin)){
    //is the jumper in place?
    if(digitalRead(jumperPin)){ //high if no jumper
      //send an audio pulse at minimum volume
      playNow(0,10);
      delay(8); //debouncing
    }else{
      //toggle the Lights
      background_brightness = background_brightness ? 0 : 255;
      setBrightness(background_brightness);
      delay(250); //debouncing
    }
  }
}

void setBrightness(byte bright_val){
  analogWrite(lightPin,bright_val);
  Serial.print("Lights set to ");
  Serial.println(bright_val);
}

void flashNow(byte bright_val, long len){
  digitalWrite(LED_BUILTIN,HIGH);
  setBrightness(bright_val);
  Serial.print("Waiting for ");
  Serial.print(len);
  Serial.println(" ms...");
  delay(len);
  setBrightness(background_brightness);
  digitalWrite(LED_BUILTIN,LOW);
}
void tapNow(byte strength, long len){
  digitalWrite(LED_BUILTIN,HIGH);
  analogWrite(solenoidPin,strength);
  Serial.print("Tapping for ");
  Serial.print(len);
  Serial.print(" ms at strength ");
  Serial.println(strength);
  delay(len);
  analogWrite(solenoidPin,0);
  digitalWrite(LED_BUILTIN,LOW);
}
void playNow(byte vol, long len){
  Serial.print("Playing ");
  Serial.print(freq);
  Serial.print("Hz at volume ");
  Serial.print(vol);
  Serial.print(" for ");
  Serial.print(len);
  Serial.println(" ms");
  digitalWrite(LED_BUILTIN,HIGH);

  if(vol>0){
    setVolume(vol);
    tone(soundPin,freq);
  }
  playUntil=millis()+len;
  isPlaying=true;
}
void playLater(unsigned long wait, byte vol, unsigned long len){
  startNext = millis() + wait;
  volNext = vol;
  lenNext = len;
}
void setVolume(byte vol){
  int trueVol = vol-1;
  //the real volumes (for the TPA chip) are 0,1,2,3
  for(int i=0;i<2;i++){
    digitalWrite(volPin[i],bitRead(trueVol,i));
  }
}
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = Serial.read();
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if(inChar>0 && inChar<=127){ //filter out gibberish that I get at startup
      if (inChar == '\n') {
        stringComplete = true;
        inputString.trim();
      }else{
        // add it to the inputString:
        inputString += inChar;
      }
    }
  }
}
