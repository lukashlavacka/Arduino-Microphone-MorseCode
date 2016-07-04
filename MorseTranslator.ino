#define DEBUG true // displays aditional information during morse parsing
#define PAUSE_DOT_DEBUG false // used to determine pause lengths, stops morse parsing
#define PAUSE_GAP_DEBUG false // if PAUSE_DEBUG is true also debugs the lengths of pauses
#define SENSOR_PIN A0
#define INTERVAL 15 // polling interval for . _ parsing, does not affect sensor reading frequency
#define MAX_ANALOG 1023 // not really needed, always the same
#define THRESHOLD 500 // volume threshold, most sounds are close to 0

const String MORSE_TABLE[55][2] = {
  {"A", ".-"},  
  {"B", "-..."},  
  {"C", "-.-."},  
  {"D", "-.."}, 
  {"E", "."}, 
  {"F", "..-."},  
  {"G", "--."}, 
  {"H", "...."},  
  {"I", ".."},  
  {"J", ".---"},  
  {"K", "-.-"}, 
  {"L", ".-.."},  
  {"M", "--"},    
  {"N", "-."},  
  {"O", "---"}, 
  {"P", ".--."},  
  {"Q", "--.-"},  
  {"R", ".-."}, 
  {"S", "..."}, 
  {"T", "-"}, 
  {"U", "..-"}, 
  {"V", "...-"},  
  {"W", ".--"}, 
  {"X", "-..-"},  
  {"Y", "-.--"},  
  {"Z", "--.."},  
  {"0", "-----"}, 
  {"1", ".----"}, 
  {"2", "..---"}, 
  {"3", "...--"}, 
  {"4", "....-"}, 
  {"5", "....."}, 
  {"6", "-...."}, 
  {"7", "--..."}, 
  {"8", "---.."}, 
  {"9", "----."}, 
  {"Ä", ".-.-"},  
  {"Á", ".--.-"}, 
  {"Å", ".--.-"}, 
  {"Ch",  "----"},  
  {"É", "..-.."}, 
  {"Ñ", "--.--"}, 
  {"Ö", "---."},  
  {"Ü", "..--"},  
  {".", ".-.-.-"},  
  {",", "--..--"},  
  {":", "---..."},  
  {"?", "..--.."},  
  {"'", ".----."},  
  {"-", "-....-"},  
  {"\\", "-..-."}, 
  {"(", "-.--.-"},  
  {"\"", ".-..-."},  
  {"@", ".--.-."},  
  {"=", "-...-"}, 
};
const int MORSE_TABLE_SIZE = 55;

// all lengths are in miliseconds
const int LETTER_PAUSE = 250 / INTERVAL; // gap after letter
const int WORD_PAUSE = 1500 / INTERVAL; // gap after work
const int SENTENCE_PAUSE = 7500 / INTERVAL; // gap after pause
const int DOT_LENGTH = 175 / INTERVAL; // length in between . and _
const int DOT_THRESHOLD = 25 / INTERVAL; // minimal length
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
    if(PAUSE_DOT_DEBUG || PAUSE_GAP_DEBUG) {
      pauseDebug();
    }
    else {
      writeMorse();         
    }

    storeOnOff();
  }
}

String translateMorse() {
  if(DEBUG) Serial.print('*');
  String w = "";
  for(int i = 0; i < bufLength; i++) {
    w += buf[i] ? "." : "-";
  }

  if(DEBUG) Serial.print("(" + w + ")");    


  for(int i = 0; i < MORSE_TABLE_SIZE; i++) {
    if(w == MORSE_TABLE[i][1]) {
      return MORSE_TABLE[i][0];      
    }
  }
  return DEBUG ? "?" : "_";
}

void pauseDebug() {
  if(PAUSE_DOT_DEBUG && !isOn && wasOn && onCounter > 0) {     
    Serial.println(onCounter * INTERVAL);
  }
  if(PAUSE_GAP_DEBUG && isOn && !wasOn && offCounter > 0) {
    Serial.println(-1 * offCounter  * INTERVAL);
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



