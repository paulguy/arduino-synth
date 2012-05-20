#define SILENT 0
#define SQUARE 1

//#define DEBUG

#define DOCHANNEL(X) \
  if(mode[X] == SQUARE) { \
    phase[X] += 1; \
    phase[X] %= period[X]; \
    outbyte += phase[X] > duty[X] ? highval[X] : lowval[X]; \
  }

#define TIMER_CLOCK_FREQ 2000000.0 //2MHz for /8 prescale from 16MHz

//Interrupt code from http://www.uchobby.com/index.php/2007/11/24/arduino-interrupts/
//Setup Timer2.
//Configures the ATMega168 8-Bit Timer2 to generate an interrupt
//at the specified frequency.
//Returns the timer load value which must be loaded into TCNT2
//inside your ISR routine.
//See the example usage below.
unsigned char SetupTimer2(float timeoutFrequency){
  unsigned char result; //The timer load value.

  //Calculate the timer load value
  result=(int)((257.0-(TIMER_CLOCK_FREQ/timeoutFrequency))+0.5);
  //The 257 really should be 256 but I get better results with 257.

  //Timer2 Settings: Timer Prescaler /8, mode 0
  //Timer clock = 16MHz/8 = 2Mhz or 0.5us
  //The /8 prescale gives us a good range to work with
  //so we just hard code this for now.
  TCCR2A = 0;
  TCCR2B = 0<<CS22 | 1<<CS21 | 0<<CS20;

  //Timer2 Overflow Interrupt Enable
  TIMSK2 = 1<<TOIE2;

  //load the timer for its first cycle
  TCNT2=result;

  return(result);
}

int timerLoadValue;
volatile int counter;
// 4
unsigned char mode[4]; //waveform
unsigned int period[4]; //set period
unsigned int phase[4]; //phase
unsigned int duty[4]; //duty cycle
char lowval[4]; //amplitude
char highval[4];
// 36

ISR(TIMER2_OVF_vect) {
  int latency;
  counter++;
  
  latency=TCNT2;
  TCNT2=latency+timerLoadValue;
}

inline void audioStep() {
  char outbyte;
  // 1
  outbyte = 0;
  
  DOCHANNEL(0)
  DOCHANNEL(1)
  DOCHANNEL(2)
  DOCHANNEL(3)

  if(outbyte & 0x80) {
    PORTB = (unsigned char)outbyte >> 3 & B00001111;
/*    digitalWrite(12, LOW);
    digitalWrite(11, ~(outbyte & 0x40));
    digitalWrite(10, ~(outbyte & 0x20));
    digitalWrite(9, ~(outbyte & 0x10));
    digitalWrite(8, ~(outbyte & 0x08)); */
  } else {
    PORTB = (unsigned char)outbyte >> 3 | B00010000;
/*    digitalWrite(12, HIGH);
    digitalWrite(11, outbyte & 0x40);
    digitalWrite(10, outbyte & 0x20);
    digitalWrite(9, outbyte & 0x10);
    digitalWrite(8, outbyte & 0x08); */
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  DDRB = B11111000;
  PORTB = 0;
  phase[0] = 0;
  phase[1] = 0;
  phase[2] = 0;
  phase[3] = 0;
  mode[0] = SILENT;
  mode[1] = SILENT;
  mode[2] = SILENT;
  mode[3] = SILENT;
  timerLoadValue=SetupTimer2(16000);
  while(Serial.available())
    Serial.read();
  Serial.print("Timer2 Load:");
  Serial.println(timerLoadValue,HEX);
}

void loop() {
  // put your main code here, to run repeatedly: 
  int lastCounter;
  char ch;
  while(Serial.available() >= 8) {
    ch = char(Serial.read());
    mode[ch] = char(Serial.read());
    period[ch] = Serial.read() << 8;
    period[ch] |= Serial.read();
    duty[ch] = Serial.read() << 8;
    duty[ch] |= Serial.read();
    lowval[ch] = char(Serial.read());
    highval[ch] = char(Serial.read());
 /*   Serial.print(lowval[ch], DEC);
    Serial.print('=');
    Serial.print((unsigned char)(lowval[ch] >> 3 & 0x0F), DEC);
    Serial.print(' ');
    Serial.print(highval[ch], DEC);
    Serial.print('=');
    Serial.print((unsigned char)(highval[ch] >> 3 | 0x10), DEC);
    Serial.print('\n');*/
  }
  
  if(counter != lastCounter) {
    audioStep();
    lastCounter = counter;
  }
}
