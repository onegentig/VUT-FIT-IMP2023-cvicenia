# Vzorkování spojitého signálu s využitím AD převodníku, zpracování vzorků #

(14denní blok **zahajuje 27. 11. 2023**)

## Detaily k zadání ##

Při řešení využijete např. [Referenční manuál](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/KL05P48M48SF1RM.pdf), část 28 (od str. 423) a [**Schéma** laboratorního kitu](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/arm_kit.pdf) (např. pro studium propojení displeje s MCU a identifikaci vývodů (kanálů) AD převodníku pro vzorkování spojité veličiny z rozhraní MCU).

**Po naprogramování a spuštění** [**šablony** vestavné aplikace](https://moodle.vut.cz/pluginfile.php/707397/mod_label/intro/lab4_main.c) se aplikace nebude navenek nijak projevovat, nicméně vstoupí do nekonečné smyčky v `main()`, v jejímž těle bude zajišťovat aktualizaci obsahu 4ciferného 7segmentového LED displeje na laboratorním kitu (jednotlivé cifry k zobrazení se vykreslují postupně, zleva doprava, s využitím časového multiplexu - v jednom okamžiku je zobrazena jen jedna z cifer).

**Doplňte šablonu** vestavné aplikace, tj. těla funkcí `ADC0_Init()` a `ADC0_IRQHandler()` tak, **aby se** výsledná **aplikace chovala dle** níže uvedeného **očekávání** (viz také komentáře v `*.c`):

- AD převodník (ADC) bude vzorkovat zadanou spojitou veličinu **nepřetržitě**, s **cyklem převodu** daným BUS_CLK/2 a CLK_DIV = 8, výsledný ADC vzorek bude **8bitový**, vytvořený z **hardwarově zprůměrované série 32** po sobě následujících **vzorků**; vytvoření výsledného ADC vzorku bude **signalizováno přerušením**.
- **V obsluze přerušení** bude výsledný ADC **vzorek** **načten**/**uložen**, např. do vhodné lokální proměnné, a z jeho hodnoty **bude zjištěno**, kolik jednotek, desetin a setin měřené veličiny tento vzorek představuje – zjištěná informace bude vhodně **vložena do řetězce** `result` za účelem jejího korektního **zobrazení na displeji** laboratorního kitu.
