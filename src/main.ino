const byte volPin[3] = {2,4,7};
const byte soundPin = 5;
const byte lightPin = 3;
bool isPlaying=false;
bool buttonPlaying=false;
unsigned long t;
unsigned long playUntil=0;
byte volNext;
unsigned long startNext=0; //for scheduling another tap
unsigned int lenNext;
unsigned int freq=1000;
//SerialEvent variables
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup(){
  pinMode(soundPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  for(int i=0;i<3;i++){
    pinMode(volPin[i],OUTPUT);
    digitalWrite(volPin[i],HIGH);
  }
  Serial.begin(9600);
  Serial.print(inputString);
  Serial.println("Ready.");
}

void loop(){
  //check for commands on the serial input
  if (stringComplete) {
    Serial.println(inputString);
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
  tone(soundPin,freq);
  playUntil=millis()+len;
  isPlaying=true;
}
void playLater(unsigned long wait, byte vol, unsigned long len){
  startNext = millis() + wait;
  volNext = vol;
  lenNext = len;
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
      }else{
        // add it to the inputString:
        inputString += inChar;
      }
    }
  }
}
