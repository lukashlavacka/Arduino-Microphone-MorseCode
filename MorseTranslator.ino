#define DEBUG true
#define PAUSE_DEBUG true
#define SENSOR_PIN A0
#define INTERVAL 25
#define MAX_ANALOG 1023
#define THRESHOLD 500

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

const int LETTER_PAUSE = 250 / INTERVAL;
const int WORD_PAUSE = 1500 / INTERVAL;
const int SENTENCE_PAUSE = 7500 / INTERVAL;
const int DOT_LENGTH = 175 / INTERVAL;
const int DOT_THRESHOLD = 25 / INTERVAL;
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
    if(PAUSE_DEBUG) {
      pauseDebug();
    }
    else {
      writeMorse();         
    }

    storeOnOff();
  }
}

char translateMorse() {
  String w = "";
  for(int i = 0; i < bufLength; i++) {
    w += String(buf[i] ? '.' : '_');
  }

  if(DEBUG) Serial.print("(" + w + ")");

  for(int i = 0; i < MORSE_TABLE_SIZE; i++) {
    if(w == MORSE_TABLE[i]) {
      return i + (i < 27 ? 65 : 48);      
    }
  }
  return DEBUG ? '?' : '_';
}

void pauseDebug() {
  if(!isOn && wasOn && onCounter > 0) {     
    Serial.println(onCounter);
  }
  if(isOn && !wasOn && offCounter > 0) {
    Serial.println(-1 * offCounter);
  }
}

void storeOnOff() {
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

void writeMorse() {  
  if(!isOn && wasOn && onCounter > DOT_THRESHOLD) {
    buf[bufLength++] = onCounter < DOT_LENGTH;// ? '.' : '_';
  }
  if(offCounter > LETTER_PAUSE && bufLength > 0) {
    Serial.print(translateMorse());
    lastSpace = false;
    bufLength = 0;
  }
  if(offCounter > SENTENCE_PAUSE && !lastSpace) {
    Serial.println();
    lastSpace = true;
  }
  if(offCounter > WORD_PAUSE && !lastSpace) {
    Serial.print(DEBUG ? '_' : ' ');
    lastSpace = true;
  }
}

