const int SENSOR_PIN = A0;
const long INTERVAL = 25;
const int MAX_ANALOG = 1023;

const int THRESHOLD = 500;

const String MORSE_TABLE[] = {
  "._",
  "_...",
  "_._.",
  "_..",
  ".",
  ".._.",
  "__.",
  "....",
  "..",
  ".___",
  "_._",
  "._..",
  "__",
  "_.",
  "___",
  ".__.",
  "__._",
  "._.",
  "...",
  "_",
  ".._",
  "..._",
  ".__",
  "_.._",
  "_.__",
  "__..",
  "_____",
  ".____",
  "..___",
  "...__",
  "...._",
  ".....",
  "_....",
  "__...",
  "___..",
  "____."
};
const int MORSE_TABLE_SIZE = 35;

const int LETTER_PAUSE = 10;
const int WORD_PAUSE = 60;
const int DOT_LENGTH = 8;
const int BUF_LENGTH = 6;


unsigned long previousMillis = 0;
bool isOn = false;
bool wasOn = false;
int onCounter = 0;
int offCounter = 0;
bool buf[BUF_LENGTH];
int bufLength = 0;
bool lastSpace = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  isOn = isOn || analogRead(SENSOR_PIN) < THRESHOLD;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= INTERVAL) {  
    previousMillis = currentMillis;

    if(bufLength > BUF_LENGTH) {
      bufLength = 0;
    }
    if(!isOn && wasOn && onCounter > 0) {
      buf[bufLength++] = onCounter < DOT_LENGTH;// ? '.' : '_';
    }
    if(isOn && !wasOn && offCounter > 0) {              
      if(offCounter > LETTER_PAUSE) {
        Serial.print(translateMorse());
        lastSpace = false;
        bufLength = 0;
      }
      if(offCounter > WORD_PAUSE && !lastSpace) {
        Serial.print(' ');
        lastSpace = true;
      }
    }

    if(isOn) {
      if(wasOn) {
        onCounter++;
      }
      wasOn = true;
      offCounter = 0;
    }
    else {
      offCounter++;
      onCounter = 0;
      wasOn = false;
    }     
    isOn = false;
  }
}

char translateMorse() {
  String w = "";
  for(int i = 0; i < bufLength; i++) {
    w += String(buf[i] ? '.' : '_');
  }

  
  Serial.print("(" + w + ")");

  for(int i = 0; i < MORSE_TABLE_SIZE; i++) {
    if(w == MORSE_TABLE[i]) {
      return i + (i < 27 ? 65 : 48);      
    }
  }
  return '_';
}

