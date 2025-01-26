#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define BUZZER_PIN 0      // PA6, socket pin 2
#define TRIGGER_PIN 2     // PA1, socket pin 4

volatile bool shouldBeep = false;   // Príznak či má pípať
volatile uint8_t beepState = 0;     // 0 = ticho, 1 = pípanie
volatile uint8_t secondsCounter = 0; // Počítadlo sekúnd

void setupRTC() {
  RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;  
  RTC.PITINTCTRL = RTC_PI_bm;         
  RTC.PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC32768_gc;  
  
  TCA0.SPLIT.CTRLA = 0; 
  ADC0.CTRLA &= ~ADC_ENABLE_bm;
  
  // Vypnutie nepoužívaných periférií
  USART0.CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);
  TWI0.CTRLA &= ~TWI_ENABLE_bm;
  SPI0.CTRLB &= ~SPI_ENABLE_bm;
}

void setupPins() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  pinMode(TRIGGER_PIN, INPUT);
  PORTA.PIN1CTRL = PORT_ISC_FALLING_gc;  // Prerušenie na zostupnu hranu
//  PORTA.PIN1CTRL = PORT_ISC_RISING_gc;  // Prerušenie na nábežnú hranu
  
  // Nastavenie nepoužívaných pinov
  pinMode(1, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
}

// Prerušenie pri zmene stavu na pine
ISR(PORTA_PORT_vect) {
  if(PORTA.INTFLAGS & PIN1_bm) {
    PORTA.INTFLAGS = PIN1_bm;      // Vyčistiť príznak prerušenia
    shouldBeep = true;             // Povoliť pípanie
    beepState = 1;                 // Začať pípaním
    secondsCounter = 0;            // Vynulovať počítadlo
    digitalWrite(BUZZER_PIN, HIGH); // Zapnúť bzučiak
  }
}

// RTC prerušenie - volá sa každú sekundu
ISR(RTC_PIT_vect) {
  RTC.PITINTFLAGS = RTC_PI_bm;    // Vyčistiť príznak prerušenia
  
  if (!shouldBeep) return;         // Ak nemá pípať, nič nerobiť

  secondsCounter++;                // Zvýšiť počítadlo sekúnd
  
  if (beepState == 1) {           // Ak pípa
    if (secondsCounter >= 1) {     // Po 1 sekunde
      digitalWrite(BUZZER_PIN, LOW); // Vypnúť bzučiak
      beepState = 0;                // Prejsť do stavu ticha
      secondsCounter = 0;           // Vynulovať počítadlo
    }
  } else {                        // Ak je ticho
    if (secondsCounter >= 10) {    // Po 10 sekundách
      digitalWrite(BUZZER_PIN, HIGH);// Zapnúť bzučiak
      beepState = 1;                // Prejsť do stavu pípania
      secondsCounter = 0;           // Vynulovať počítadlo
    }
  }
}

void setup() {
  setupPins();
  setupRTC();
  sei();
}

void loop() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
  sleep_disable();
}
