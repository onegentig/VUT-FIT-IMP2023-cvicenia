# Pulsní šířková modulace (PWM) pomocí časovače a na ní založená aplikace (řízení intenzity svitu RGB LED) #

(14denní blok **zahajuje 13. 11. 2023**)

## Detaily k zadání ##

Při řešení využijete např. [Referenční manuál](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/KL05P48M48SF1RM.pdf), kap. 31 (od str. 507) a [**Schéma** laboratorního kitu](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/arm_kit.pdf) (např. pro identifikaci vývodů (kanálů) časovače využitelných k ovládání LED pomocí PWM signálu z rozhraní MCU).

**Po naprogramování a spuštění** [**šablony** vestavné aplikace](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/lab3_main.c) se aplikace nebude navenek nijak projevovat, nicméně vstoupí do nekonečné smyčky v `main()`, v jejímž těle bude nejprve inkrementovat proměnnou `compare` (až do dosažení konečné hodnoty *`OVERFLOW`*) a poté ji dekrementovat k hodnotě 0 (pozn.: přepínání mezi režimy de/inkrementace řídí proměnná increment).

**Doplňte šablonu** vestavné aplikace, tj. těla funkcí `Timer0Init()` a `main()` tak, aby se výsledná aplikace **chovala dle** níže uvedeného **očekávání** (viz také komentáře v `*.c`):

- Aplikace bude cyklicky vykonávat následující činnost: nejprve plynule zesilovat, a po dosažení maxima, plynule zeslabovat, až k dosažení minima, intenzitu svitu tříbarevné (RGB) LED diody. Pro zajištění konkrétní **intenzity svitu LED** na konkrétním z R, G, B kanálů využijte příslušný **PWM signál** generovaný časovačem dostupným na MCU.
