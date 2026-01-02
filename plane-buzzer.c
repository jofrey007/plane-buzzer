/*
 * Determalizátor a bzučiak pre voľné modely
 * ATtiny402 - čistý C kód
 *
 * Optimalizovaná verzia pre minimálnu spotrebu energie
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define BUZZER_PIN    PIN6_bm    // PA6, socket pin 2
#define TRIGGER_PIN   PIN1_bm    // PA1, socket pin 4

volatile uint8_t shouldBeep = 0;      // Príznak či má pípať
volatile uint8_t beepState = 0;       // 0 = ticho, 1 = pípanie
volatile uint8_t secondsCounter = 0;  // Počítadlo sekúnd

void disableUnusedPeripherals(void) {
    // Vypnutie nepoužívaných periférií pre úsporu energie
    TCA0.SPLIT.CTRLA = 0;
    ADC0.CTRLA &= ~ADC_ENABLE_bm;
    USART0.CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);
    TWI0.CTRLA &= ~TWI_ENABLE_bm;
    SPI0.CTRLA &= ~SPI_ENABLE_bm;
    AC0.CTRLA &= ~AC_ENABLE_bm;        // Vypnúť komparátor
}

void setupBOD(void) {
    // BOD v sleep móde - sampled mód pre úsporu energie (~20µA)
    // Namiesto kontinuálneho monitorovania vzorkuje periodicky
    BOD.CTRLA = BOD_SLEEP_SAMPLED_gc;
}

void setupRTC(void) {
    // Počkať na synchronizáciu pred zápisom
    while (RTC.STATUS & RTC_CTRLABUSY_bm);

    // Použiť interný 32kHz oscilátor
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;

    // Počkať na synchronizáciu PIT
    while (RTC.PITSTATUS & RTC_CTRLBUSY_bm);

    // Povoliť PIT prerušenie
    RTC.PITINTCTRL = RTC_PI_bm;

    // Zapnúť PIT s periódou 32768 cyklov = 1 sekunda
    RTC.PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC32768_gc;
}

void setupPins(void) {
    // Bzučiak ako výstup, začína v LOW
    PORTA.DIRSET = BUZZER_PIN;
    PORTA.OUTCLR = BUZZER_PIN;

    // Trigger ako vstup s prerušením na zostupnú hranu + pull-up
    PORTA.DIRCLR = TRIGGER_PIN;
    PORTA.PIN1CTRL = PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;

    // Nepoužívané piny ako výstupy v LOW (úspora energie)
    PORTA.DIRSET = PIN0_bm | PIN2_bm | PIN3_bm | PIN7_bm;
    PORTA.OUTCLR = PIN0_bm | PIN2_bm | PIN3_bm | PIN7_bm;

    // Vypnúť vstupné buffery na nepoužívaných pinoch (úspora ~1-5µA)
    PORTA.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;  // BUZZER je výstup
    PORTA.PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc;
}

// Prerušenie pri zmene stavu na TRIGGER pine
ISR(PORTA_PORT_vect) {
    if (PORTA.INTFLAGS & TRIGGER_PIN) {
        PORTA.INTFLAGS = TRIGGER_PIN;     // Vyčistiť príznak prerušenia
        shouldBeep = 1;                    // Povoliť pípanie
        beepState = 1;                     // Začať pípaním
        secondsCounter = 0;                // Vynulovať počítadlo
        PORTA.OUTSET = BUZZER_PIN;         // Zapnúť bzučiak
    }
}

// RTC PIT prerušenie - volá sa každú sekundu
ISR(RTC_PIT_vect) {
    RTC.PITINTFLAGS = RTC_PI_bm;          // Vyčistiť príznak prerušenia

    if (!shouldBeep) return;               // Ak nemá pípať, nič nerobiť

    secondsCounter++;                      // Zvýšiť počítadlo sekúnd

    if (beepState == 1) {                  // Ak pípa
        if (secondsCounter >= 1) {         // Po 1 sekunde
            PORTA.OUTCLR = BUZZER_PIN;     // Vypnúť bzučiak
            beepState = 0;                 // Prejsť do stavu ticha
            secondsCounter = 0;            // Vynulovať počítadlo
        }
    } else {                               // Ak je ticho
        if (secondsCounter >= 10) {        // Po 10 sekundách
            PORTA.OUTSET = BUZZER_PIN;     // Zapnúť bzučiak
            beepState = 1;                 // Prejsť do stavu pípania
            secondsCounter = 0;            // Vynulovať počítadlo
        }
    }
}

int main(void) {
    // Inicializácia v správnom poradí
    disableUnusedPeripherals();
    setupBOD();
    setupPins();
    setupRTC();

    // Povoliť globálne prerušenia
    sei();

    // Nastaviť sleep mód na POWER_DOWN (najnižšia spotreba)
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    // Hlavná slučka - MCU spí a čaká na prerušenia
    while (1) {
        sleep_enable();
        sleep_cpu();
        sleep_disable();
    }

    return 0;
}
