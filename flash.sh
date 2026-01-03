#!/bin/bash
# Skript pre naprogramovanie ATtiny402 cez UPDI
# Arduino Uno ako jtag2updi programátor na /dev/ttyUSB0

PORT="/dev/ttyUSB0"
MCU="attiny402"
HEX_FILE="plane-buzzer.hex"
BAUD="115200"

# Cesta k avrdude z Arduino toolchain
AVRDUDE="/home/attila/.arduino15/packages/DxCore/tools/avrdude/6.3.0-arduino17or18/bin/avrdude"
AVRDUDE_CONF="/home/attila/.arduino15/packages/DxCore/tools/avrdude/6.3.0-arduino17or18/etc/avrdude.conf"

# Kontrola či existuje hex súbor
if [ ! -f "$HEX_FILE" ]; then
    echo "Chyba: $HEX_FILE neexistuje. Spustite najprv 'make'"
    exit 1
fi

# Kontrola či existuje port
if [ ! -e "$PORT" ]; then
    echo "Chyba: $PORT neexistuje. Skontrolujte pripojenie programátora."
    exit 1
fi

echo "Programujem $MCU cez $PORT (jtag2updi)..."
echo "Súbor: $HEX_FILE"
echo ""

# Programovanie cez avrdude s jtag2updi
$AVRDUDE -C $AVRDUDE_CONF -c jtag2updi -p $MCU -P $PORT -b $BAUD -U flash:w:$HEX_FILE:i

if [ $? -eq 0 ]; then
    echo ""
    echo "Programovanie úspešné!"
else
    echo ""
    echo "Programovanie zlyhalo!"
    exit 1
fi
