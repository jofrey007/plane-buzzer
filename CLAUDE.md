# CLAUDE.md

Tento súbor poskytuje pokyny pre Claude Code (claude.ai/code) pri práci s kódom v tomto repozitári.

## Prehľad projektu

Determalizátor a bzučiak pre voľné modely lietadiel. Mechanizmus uvoľnenia používa rozpálený odporový drôt na prepálenie gumičky. Systém sa skladá z:
- 433 MHz prijímač s výkonovým MOSFET tranzistorom
- Modul bzučiaka s aktívnym piezzo bzučiakom
- Napájanie LiPo akumulátorom 1S (3.7V, 35mAh)

## Hardvér

- **MCU**: ATtiny402
- **PCB**: Vlastná doska navrhnutá v Eagle (súbory v `hardware/`)
- **Komponenty**: Zoznam súčiastok s odkazmi na dodávateľov v `BOM/bom.txt`

## Kompilácia a programovanie

### Požiadavky
- megaTinyCore toolchain (arduino-cli)
- Arduino Uno s jtag2updi firmvérom ako programátor

### Kompilácia
```bash
make          # skompilovať plane-buzzer.c
make clean    # vymazať binárne súbory
make size     # zobraziť využitie pamäte
```

### Programovanie ATtiny402
```bash
./flash.sh    # naprogramovať cez jtag2updi (Arduino Uno na /dev/ttyUSB0)
```

Alternatívne použite Arduino IDE s UPDI programovaním. Referenčné video: https://www.youtube.com/watch?v=YOGeoW_QySs

## Architektúra kódu

Firmvér (`plane-buzzer.c`) je riadený prerušeniami s hlbokým spánkom pre úsporu energie:

- **Konfigurácia pinov**: PA6 (pin 0) pre výstup bzučiaka, PA1 (pin 2) pre vstup triggera
- **RTC**: Používa interný 32kHz oscilátor s PIT (Periodic Interrupt Timer) pre 1-sekundové intervaly
- **Správa napájania**: Vypína nepoužívané periférie (TCA0, ADC, USART, TWI, SPI, AC0) a používa režim spánku POWER_DOWN
- **Optimalizácie**: BOD v sampled móde, vypnuté vstupné buffery, pull-up na trigger pine
- **Vzor pípania**: Pri spustení (zostupná hrana na PA1) pípne 1 sekundu, potom 10 sekúnd ticho, opakuje sa

## Úprava nabíjačky batérie

Nabíjačka MCP73831 vyžaduje výmenu odporu na 68kΩ pre obmedzenie nabíjacieho prúdu na 5-10mA pre malý LiPo článok.
