#include <OneWire.h>

OneWire sensor(2); //on pin 2

void setup(void) {
  Serial.begin(9600);
  DDRB = B00111111; //8-13 pins are output
  PORTB = B00000000; //init value on 8-13 pins is zero
  
  noInterrupts();
  //timer1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 34286;
  TCCR1B = 0x04;
  TIMSK1 = 0x01;

  interrupts();
}

void loop(void) {
  getTemp();
}

ISR(TIMER1_OVF_vect) {
  TCNT1 = 34286;
  PORTB ^= B00100000; //blink pin 13
}

void getTemp(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  
  if(!sensor.search(addr)) {
    PORTB ^= B00001000; //blink pin 11
    sensor.reset_search();
    delay(250);
    PORTB ^= B00001000; //blink pin 11
    return;
  }
  
  if(OneWire::crc8(addr,7) != addr[7]) {
    return;
  }
  
  sensor.reset();
  sensor.select(addr);
  sensor.write(0x44, 1);
  
  delay(1000); //if more then one sensor - comment this
  
  present = sensor.reset();
  sensor.select(addr);
  sensor.write(0xBE);
  
  for(i = 0; i < 9; i++) {
    data[i] = sensor.read();
  }
  
  int16_t raw = (data[1] << 8) | data[0];
  if(type_s) {
    raw = raw << 3;
    if(data[7] == 0x10) {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);// at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  }
  
  celsius = (float)raw / 16.0;
  Serial.print(celsius);
  Serial.print("\r\n");
}
