/******************************************************************************/
/*                                                                            */
/* Laboratorní úloha č. 2 z předmětu IMP                                      */
/*                                                                            */
/* Mereni hodnoty napeti na potenciometru pomoci AD prevodniku                */
/*                                                                            */
/* Řešení vytvořil(a) a odevzdáva: Onegen Something, xkrame00                     */
/*                                                                            */
/******************************************************************************/

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "MKL05Z4.h"

/* Aktivační konstanty jednotlivých pozic LED displeje */
// svítí v log. 1
#define D1 0x0800  // PTA11
#define D2 0x1000  // PTA12
#define D3 0x2000  // PTA13
#define D4 0x1000  // PTB12

/* Aktivační konstanty hodnot číslic (7 seg. a-g na PTA3-9) */
// svítí v log. 0
#define N0 0x0607           // "0"
#define N1 0x07CF           // "1"
#define N2 0x0527           // "2"
#define N3 0x0587           // "3"
#define N4 0x04C8           // "4"
#define N5 0x0497           // "5"
#define N6 0x0417           // "6"
#define N7 0x07C7           // "7"
#define N8 0x0407           // "8"
#define N9 0x0487           // "9"
#define DOT_ON_MASK 0x03FF  // "."

/* Pomocné konštanty */
#define ERR 0x5FF  // "-"
#define NIC 0x7FF  // nic

#define DELAY_LEDD 2000

const unsigned int digit[] = {N0, N1, N2, N3, N4, N5, N6, N7, N8, N9};

unsigned int index;

/* Funkce inicializace MCU */
void MCU_Init(void) {
     SIM->COPC = SIM_COPC_COPT(0x00);  // Deaktivace modulu WatchDog
     // Aktivace hodin řídících modulů portů A B
     SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK;
     SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;  // Aktivace hodin pro ADC0

     // Nastavení vývodů LED displeje jako univerzální GPIO pro SW řízení
     PORTA->PCR[3] = PORT_PCR_MUX(0x01);   // seg A
     PORTA->PCR[4] = PORT_PCR_MUX(0x01);   // seg B
     PORTA->PCR[5] = PORT_PCR_MUX(0x01);   // seg C
     PORTA->PCR[6] = PORT_PCR_MUX(0x01);   // seg D
     PORTA->PCR[7] = PORT_PCR_MUX(0x01);   // seg E
     PORTA->PCR[8] = PORT_PCR_MUX(0x01);   // seg F
     PORTA->PCR[9] = PORT_PCR_MUX(0x01);   // seg G
     PORTA->PCR[10] = PORT_PCR_MUX(0x01);  // seg DP

     PORTA->PCR[11] = PORT_PCR_MUX(0x01);  // DIG1
     PORTA->PCR[12] = PORT_PCR_MUX(0x01);  // DIG2
     PORTA->PCR[13] = PORT_PCR_MUX(0x01);  // DIG3

     PTA->PDDR = GPIO_PDDR_PDD(0x3FF8);  // Konfigurace jako výstupní piny

     PORTB->PCR[0] = PORT_PCR_MUX(0x00);   // Potenciometr jako vstup ADC0
     PORTB->PCR[12] = PORT_PCR_MUX(0x01);  // DIG4 na portu B jako GPIO
     PTB->PDDR = GPIO_PDDR_PDD(0x1000);    // Konfigurace DIG4 jako výstupu
}

/* Funkce realizující zpoždění formou aktivního čekání */
void delay(long long bound) {
     long long i;
     for (i = 0; i < bound; i++)
          ;
}

/* Funkce display_val nastavuje vývody LED pro právě aktivní pozici (index)  *
 * řetezce val_str a číslici 7-seg. LED displeje. Řetezec val_str obsahuje   *
 * hodnotu pro zobrazení na displeji ve formátu 1.234 (des. tečka nepovinná) */
void display_val(char *val_str) {
     // Jsme-li s ukazovátkem mimo řetezec nebo mimo displej,
     // nic nezobrazujeme a skončíme
     if (index >= strlen(val_str) || index > 4) {
          PTA->PDOR = GPIO_PDOR_PDO(NIC);
          return;
     }

     // Má smysl zobrazovat pouze číslice
     if (isdigit(val_str[index])) {
          PTA->PDOR = GPIO_PDOR_PDO(digit[val_str[index] - '0']);
          index++;

          // Je-li za číslicí desetinná tečka, aktivujeme ji
          if (val_str[index] == '.' || val_str[index] == ',') {
               PTA->PDOR &= GPIO_PDOR_PDO(DOT_ON_MASK);
               index++;
          }
     } else {
          // Nečíselný symbol => zobrazíme "-" (ERR)
          PTA->PDOR = GPIO_PDOR_PDO(ERR);
          index++;
     }
}

/*****************************************************************************
 *                            ÚKOLY K DOPLNĚNÍ:                              *
 *                                                                           *
 * 1. Doplňte konfiguraci AD převodníku ADC0 podle následující specifikace:  *
 *    - zpracování naměřené hodnoty v rámci obsluhy přerušení,               *
 *    - zdroj hodinového signálu: (Bus clock)/2, clock divider = 8,          *
 *    - režim převodu: kontinuálni konverze, 8-bit,                          *
 *    - výsledek bude hardwarově průměřován z 32 vzorků.                     *
 * Nezapomeňte povolit preřušení v systému NVIC (inspirace viz 2. cvičení).  *
 * Registry ADC0, které se netýkaji výše uvedené konfigurace, nemodifikujte. *
 * DŮLEŽITÉ: REGISTR ADC0_SC1A JE NUTNO NASTAVIT JAKO POSLEDNÍ, JELIKOŽ TEN  *
 * AKTIVUJE ČINNOST PŘEVODNIKU A POTÉ JIŽ JEHO KONFIGURACI NELZE MĚNIT.      */
void ADC0_Init(void) {
     NVIC_EnableIRQ(ADC0_IRQn);

     // ADC0->CFG1 = 0110 0001
     // ADICLK     :        01 (bus clock/2)
     // MODE       :      00   (8-bit)
     // ADIV       :  11       (input clock/8)
     // ADC0->CFG1 = 0x61;
     ADC0->CFG1
         = ADC_CFG1_ADICLK(0x01) | ADC_CFG1_MODE(0x00) | ADC_CFG1_ADIV(0x03);

     // ADC0->SC3 = 0000 1111
     // AVGS      :        11 (32 vzorkov na priemerovanie)
     // AVGE      :       1   (HW priemerovanie)
     // ADCO      :      1    (kontinualny mod)
     // ADC0->SC3 = 0x0F
     ADC0->SC3 = ADC_SC3_AVGS(0x03) | ADC_SC3_AVGE_MASK | ADC_SC3_ADCO_MASK;

     // ADC0_SC1A = 0100 0110
     // ADCH      :    0 0110 (kanal 6)
     // AIEN      :  1        (povolit prerusenia)
     // ADC0_SC1A = 0x46;
     ADC0->SC1[0] = ADC_SC1_ADCH(6) | ADC_SC1_AIEN_MASK;
}

/* 2. Výpočet a vložení výsledku do níže uvedeného řetezce result proveďte    *
 * doplněním obslužného podprogramu preřušení. Pro tyto potřeby se velmi hodí *
 * funkce sprintf známá ze std. C, která však ne na všech platformách bývá    *
 * použitelná v plném rozsahu (na velmi jednoduchých MCU dokonce ani nemáme   *
 * podporu pro floating-point výpočty!). KDO BUDE CHTÍT FUNKCI SPRINTF VYUŽÍT *
 * PRO VÝPIS FP VÝSLEDKU, MUSÍ SI V NASTAVENÍ PROJEKTU V KDS AKTIVOVAT PRO TO *
 * PODPORU, NÁVOD VIZ ZDE: https://community.nxp.com/thread/442798            *
 * případně je možné využít vlastní pomocné implementace nebo se inspirovat   *
 * něčím existujícím např. zde: https://stackoverflow.com/questions/12703307  *
 *
 * Do tohoto řetezce vložte výslednou hodnotu přepočtenou z ADC ve formátu
 * s přesností na 2 desetinná místa v rozsahu napětí potenciometru. Pozor:
 * jedná se o standardní Céčkovský řetezec, který musí být ukončen znakem \0! */
char result[10] = "xxxx";
void ADC0_IRQHandler(void) {
     // Dokončenie konverzie signalizuje COCO (conversion complete) flag.
     // Kontrolovať ho netreba, pretože len COCO=1 vyvolá prerušenie, pokiaľ
     //      je flag AIEN=1.
     // if (!((ADC0->SC1[0] & ADC_SC1_COCO_MASK))) return;

     // Napätie = (hodnota ADC × referenčné näpatie) / rozlíšenie ADC
     unsigned int res = (ADC0->R[0] * 330) / 255; // 330 = 3.3V × 100

     // Konverzia na reťazec bez floating logiky cez insert charov od konca.
     result[4] = '\0';  // NULL terminator
     for (int res_i = 3; res_i > -1; res_i--) {
          if (res_i == 1) {
               result[res_i] = '.';
               continue;
          }

          result[res_i] = (res % 10) + '0';
          res /= 10;
     }

     // COCO flag sa automaticky vynuluje po čítaní z ADC0->R[0], žiadne
     //     "čistenie prerušenia" netreba riešiť.
     // ADC0->SC1[0] &= ~ADC_SC1_COCO_MASK;
}

/******************************************************************************/

int main(void) {
     MCU_Init();
     ADC0_Init();

     while (true) {
          // Index do řetezce result, odkud bude zpracovávaná hodnota, která
          // se má zobrazit na 7-segm. LED displeji. V každém průchodu hlavní
          // smyčkou postupně aktivujeme jednotlivé pozice displeje a pomocí
          // funkce display_val zobrazujeme příslušné číslice z řetezce result.
          index = 0;

          display_val(result);             // číslice k zobrazení
          PTA->PDOR |= GPIO_PDOR_PDO(D1);  // aktivace první cifry
          delay(DELAY_LEDD);
          PTA->PDOR &= GPIO_PDOR_PDO(~D1);  // deaktivace první cifry

          display_val(result);             // číslice k zobrazení
          PTA->PDOR |= GPIO_PDOR_PDO(D2);  // aktivace druhé cifry
          delay(DELAY_LEDD);
          PTA->PDOR &= GPIO_PDOR_PDO(~D2);

          display_val(result);             // číslice k zobrazení
          PTA->PDOR |= GPIO_PDOR_PDO(D3);  // aktivace třetí cifry
          delay(DELAY_LEDD);
          PTA->PDOR &= GPIO_PDOR_PDO(~D3);

          display_val(result);             // číslice k zobrazení
          PTB->PDOR |= GPIO_PDOR_PDO(D4);  // aktivace čtvrté cifry
          delay(DELAY_LEDD);
          PTB->PDOR &= GPIO_PDOR_PDO(~D4);
     }
}
