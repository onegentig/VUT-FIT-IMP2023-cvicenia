# Aplikace portů ve funkci obecně využitelných číslicových/digitálních vstupů/výstupů (GPIO), detekce události (stisku tlačítka) pomocí přerušení, reakce na událost pomocí obsluhy přerušení #

(14denní blok **zahajuje 30. 10. 2023**)

## Detaily k zadání ##

Při řešení využijete např. [Referenční manuál](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/KL05P48M48SF1RM.pdf), část 3.3.2 (od str. 48) a část 11.5 (od str. 161) a také dokument [Cortex-M0+ Devices Generic User Guide](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/dui0662b_cortex_m0p_r0p1_dgug.pdf), část 4.2 (od str. 87) a [**Schéma** laboratorního kitu](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/arm_kit.pdf) (např. pro identifikaci vývodů MCU využitelných k detekci stavu tlačítek a způsobu zapojení tlačítek).

**Po naprogramování a spuštění** [**šablony** vestavné aplikace](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/lab2_main.c?time=1665480816797) by měla na laboratorním kitu probliknout LED a poté zaznít krátký tón, poté vstoupí aplikace do nekonečné smyčky, viz `while(1);` v `main()` po případném stisku některého z tlačítek bude aplikace cyklicky spouštět funkci `PORTB_IRQHandler()`, což se projeví opakovaným problikáváním LED - viz volání `flash()`.

**Doplňte šablonu** vestavné aplikace, tj. tělo funkce `PORTB_IRQHandler()` tak, aby se výsledná aplikace chovala dle níže uvedeného očekávání:

- Uživatel může na laboratorním kitu stisknout libovolné z 5 tlačítek (nahoru/up, dolů/down, vlevo/left, vpravo/right, prostřední/center), přičemž **událost stisku** jednotlivého tlačítka je zachycena přerušovacím systémem a vede k jednomu vyvolání obsluhy `PORTB_IRQHandler()`. Jiné události, tj. např. událost současného stisku několika tlačítek či událost uvolnění tlačítka, není nutno řešit.
- Doplňte tělo obsluhy tak, aby **jednotlivý stisk** každého z tlačítek byl korektně detekován a následně **jednou obsloužen**, např. s využitím funkcí `flash()` či `beep()`.
