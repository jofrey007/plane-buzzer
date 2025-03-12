## Determalizátor a bzučiak pre voľné modely
Princíp uvoľnenia pohyblivej časti modelu je cez prepálenie gumičky rozpáleným odporovým drôtom.
Determalizátor sa skladá z 2 modulov

 - prijímač 433 MHz s výkonovým MOSFET tranzistorom
 - modul bzučiaka s aktívnym piezzo bzučiakom

Napájanie determalizátora je LiPo akumulátorom 1S (3.7V), 35mAh, ktoré sa používajú do BT slúchadiel.

### Programovanie Attiny402
Pre naprogramovanie plane-buzzer.ino do MCU použite nejaké iné Arduino (Uno) ?
Popis programovania je uvedený v youtube videu aj s popisom pod ním
[Programovanie cez UPDI ](https://www.youtube.com/watch?v=YOGeoW_QySs)

### PCB bzučiaka

### Odporová špirála
3 závity na vrtáku 3mm [A1 30G 100Ft](https://www.aliexpress.com/item/1005001496323840.html?spm=a2g0o.order_list.order_list_main.284.61ee1802CS6hZ)

### Háčik na zavesenie gumy

### Anténa
Pôvodnú anténu odpájkovať a nahradiť rovnako dlhým vodičom [30AWG](https://www.aliexpress.com/item/1005001590476043.html?spm=a2g0o.order_detail.order_detail_item.7.6c56f19cd0DTJT)

### Nabíjanie akumulátora
Akumulátor sa musí nabíjať malým prúdom okolo 5-10mA, z toho dôvodu je potrebné upraviť nabíjač s MCP73831. Úprava spočíva vo výmene SMD odporu za odpor s hodnotou 68k.
[nabíjačka](https://github.com/jofrey007/plane-buzzer/blob/main/hardware/charger.jpg)
