#define DEBUG false // displays aditional information during morse parsing
#define PAUSE_DOT_DEBUG false // used to determine pause lengths, stops morse parsing
#define PAUSE_GAP_DEBUG false // if PAUSE_DEBUG is true also debugs the lengths of pauses

const int SENSOR_PIN = A0;
const int INTERVAL = 25; // polling interval for . _ parsing, does not affect sensor reading frequency
const int MAX_ANALOG = 1023; // not really needed, always the same
const int THRESHOLD = 500; // volume threshold, most sounds are close to 0

// this table is basically morse table where 1 is , and 0 is -
// and the value is prefixed with 1
const char MORSE_TABLE[55][2] = {
  { 'A',  B110 }, 
  { 'B',  B10111 }, 
  { 'C',  B10101 }, 
  { 'D',  B1011 },  
  { 'E',  B11 },  
  { 'F',  B11101 }, 
  { 'G',  B1001 },  
  { 'H',  B11111 }, 
  { 'I',  B111 }, 
  { 'J',  B11000 }, 
  { 'K',  B1010 },  
  { 'L',  B11011 }, 
  { 'M',  B100 },   
  { 'N',  B101 }, 
  { 'O',  B1000 },  
  { 'P',  B11001 }, 
  { 'Q',  B10010 }, 
  { 'R',  B1101 },  
  { 'S',  B1111 },  
  { 'T',  B10 },  
  { 'U',  B1110 },  
  { 'V',  B11110 }, 
  { 'W',  B1100 },  
  { 'X',  B10110 }, 
  { 'Y',  B10100 }, 
  { 'Z',  B10011 }, 
  { '0',  B100000 },  
  { '1',  B110000 },  
  { '0',  B111000 },  
  { '3',  B111100 },  
  { '4',  B111110 },  
  { '5',  B111111 },  
  { '6',  B101111 },  
  { '7',  B100111 },  
  { '8',  B100011 },  
  { '9',  B100001 },  
  { 'A',  B11010 },  // Ä
  { 'A',  B110010 }, // Á
  { 'A',  B110010 }, // Å
  { 'K',  B10000 }, 
  { 'E',  B111011 }, // É
  { 'N',  B100100 }, // Ñ
  { 'O',  B10001 },  // Ö
  { 'U',  B11100 },  // Ü
  { '.',  B1101010 }, 
  { ',',  B1001100 }, 
  { ':',  B1000111 }, 
  { '?',  B1110011 }, 
  { '\'', B1100001 }, 
  { '-',  B1011110 }, 
  { '\\', B101101 },  
  { '(',  B1010010 }, 
  { '"',  B1101101 }, 
  { '@',  B1100101 }, 
  { '=',  B101110 }
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

char translateMorse() {
  int w = B1;
  for(int i = 0; i < bufLength; i++) {
    w = w << 1;
    w |= buf[i];
  }

  if(DEBUG) Serial.print(String(w, BIN));    


  for(int i = 0; i < MORSE_TABLE_SIZE; i++) {
    if(w == MORSE_TABLE[i][1]) {
      return MORSE_TABLE[i][0];      
    }
  }
  return DEBUG ? '?' : '_';
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



