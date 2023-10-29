# Komunikace přes UART, seznámení se s laboratorním kitem, vývojovým prostředím a základy programování a ladění vestavných aplikací; detekce události (příchozích dat) pomocí dotazování (polling)

(14denní blok **zahajuje 16. 10. 2023**)

## Detaily k zadání

Při řešení využijete např. [Referenční manuál](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/KL05P48M48SF1RM.pdf) od s. 635, popř. také [Schéma laboratorního kitu](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/arm_kit.pdf) (např. k identifikaci vývodů přijímače a vysílače UART z rozhraní MCU).

Toto zadání, mj., vyžaduje nastavit a spustit na straně PC terminál (např. pomocí aplikace [Putty](https://putty.org/)) pro komunikaci mezi PC (USB) a laboratorním kitem (UART) s těmito parametry: přenosová rychlost **115200 Bd** (b/s), **8 datových bitů** v komunikačním rámci, **1 stop bit**, **žádná parita**, připojení přes **COMx** v PC (konkrétní hodnota "x " se může lišit pro jednotlivá PC - zjistěte s pomocí vyučujícího a např. Správce zařízení), řízení toku: žádné.

**Po naprogramování a spuštění** [**šablony** vestavné aplikace](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/lab1_main.c) by měla na laboratorním kitu probliknout LED a poté zaznít krátký tón; zbylou část vestavné aplikace je nutno zprovoznit doplněním chybějících úseků programu (viz dále).

**Doplňte šablonu** vestavné aplikace tak, aby se výsledná aplikace chovala dle níže uvedeného očekávání:

- Uživatel aplikace bude, prostřednictvím terminálu na PC, **zadávat** na PC klávesnici **znaky**. Každý z techto znaků bude na straně MCU **přijat modulem přijímače UART** a následně modulem vysílače UART **zaslán zpět do PC**, aby se zobrazil v terminálu.
- Aplikace bude příchozí znaky **ukládat** do pole znaků (textového řetězce) `login`, a to v pořadí jejich příchodu. Po zaplnění pole (tj. po přijetí 8 znaků) aplikace **porovná obsah pole** `login` s obsahem referenčního textového řetězce (viz pole znaků `corrl`, které inicializujte textovým řetězcem odpovídajícím **Vašeho loginu** na FIT VUT).
- Při shodě řetězců `login` a `corrl` aplikace **vydá zvuk** ("pípne"), např. pro signalizaci povolení přístupu, voláním funkce `beep()`. Jinak, tj. v případě neshody rětězců aplikace **vypíše na terminál v PC vhodný text**, např. "STOP", informující o zamítnutí přístupu a **umožní opakované zadání osmice znaků**.
- Postupujte podle komentářů v zdrojovém souboru na příslušných místech; funkci pro příjem znaků implementujte pomocí **dotazovací smyčky** (angl. *polling loop*).
