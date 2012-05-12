#define SILENT 0
#define SQUARE 1

//#define DEBUG

#define DOCHANNEL(X) \
  phase[X] += 1; \
  phase[X] %= period[X]; \
  if(mode[X] == SQUARE) \
    outbyte += phase[X] > duty[X] ? highval[X] : lowval[X];

unsigned long int nextmicros;
// 4
unsigned char mode[4]; //waveform
unsigned int period[4]; //set period
unsigned int phase[4]; //phase
unsigned int duty[4]; //duty cycle
char lowval[4]; //amplitude
char highval[4];
// 36

void audioStep() {
  char outbyte;
  // 1
  outbyte = 0;
  
  DOCHANNEL(0)
  DOCHANNEL(1)
  DOCHANNEL(2)
  DOCHANNEL(3)

#ifdef DEBUG
/*  Serial.print(outbyte & 0x80);
  Serial.print(outbyte & 0x40);
  Serial.print(outbyte & 0x20);
  Serial.print(outbyte & 0x10);
  Serial.print(outbyte & 0x08);*/
  Serial.println((int)outbyte);
#else
  if(outbyte & 0x80) {
    digitalWrite(2, LOW);
    digitalWrite(3, ~(outbyte & 0x40));
    digitalWrite(4, ~(outbyte & 0x20));
    digitalWrite(5, ~(outbyte & 0x10));
    digitalWrite(6, ~(outbyte & 0x08));
  } else {
    digitalWrite(2, HIGH);
    digitalWrite(3, outbyte & 0x40);
    digitalWrite(4, outbyte & 0x20);
    digitalWrite(5, outbyte & 0x10);
    digitalWrite(6, outbyte & 0x08);
  }
#endif
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  phase[0] = 0;
  phase[1] = 0;
  phase[2] = 0;
  phase[3] = 0;
  mode[0] = SILENT;
  mode[1] = SILENT;
  mode[2] = SILENT;
  mode[3] = SILENT;
  nextmicros = 0;
}

void loop() {
  // put your main code here, to run repeatedly: 
  char ch;
  if(Serial.available() >= 8) {
    ch = Serial.read();
    mode[ch] = (char)Serial.read();
    period[ch] = (int)((char)Serial.read() << 8 | (char)Serial.read());
    duty[ch] = (int)((char)Serial.read() << 8 | (char)Serial.read());
    lowval[ch] = (char)Serial.read();
    highval[ch] = (char)Serial.read();
  }

  audioStep();
  while (micros() < nextmicros);
  nextmicros = (micros() >> 7 + 1) << 7;
}
