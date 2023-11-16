/******************************************************************************/
/*                                                                            */
/*  Laboratorní úloha č. 2 z předmětu IMP                                     */
/*                                                                            */
/*  Demonstrace rizeni jasu/barvy RGB LED pomoci PWM signalu generovaneho     */
/*  casovacem                                                                 */
/*                                                                            */
/*  Řešení vytvořil(a) a odevzdáva: Onegen Something, xkrame00                    */
/*                                                                            */
/******************************************************************************/

#include "MKL05Z4.h"

/** Funkce zpoždění - funkce skončí po nastaveném počtu cyklů */
void delay(long long bound) {
     long long i;
     for (i = 0; i < bound; i++)
          ;
}

/** Inicializace MCU – zastavení watchdogu, nastavení MCG, SIM, portů */
void MCUInit(void) {
     MCG->C4 |= (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01));
     SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
     SIM->COPC = SIM_COPC_COPT(0x00);
}

/** Inicializace hodinových zdrojů pro periférie */
void SysInit(void) {
     // Zdroj hodín do časovače TPM
     SIM->SOPT2 |= SIM_SOPT2_TPMSRC(0x01);

     // Povolení hodin pro port B (RGB LED)
     SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

     // Povolení hodin do časovače TPM0
     SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
}

/** Inicializace portů s RGB LED jednotlivé barevné složky LED budou řízeny
 * příslušnými kanálovými výstupy časovače TPM0 a to konkrétně signálem PWM,
 * který bude časovačem generován. Čísla kanálů pro LED vizte schéma
 * laboratorního přípravku. */
void PortInit(void) {
     PORTB->PCR[9] = PORT_PCR_MUX(0x02);   //  PTB9: TPM0_CH2 -> R
     PORTB->PCR[10] = PORT_PCR_MUX(0x02);  // PTB10: TPM0_CH1 -> G
     PORTB->PCR[11] = PORT_PCR_MUX(0x02);  // PTB11: TPM0_CH0 -> B
}

/** Doporučena hodnota přetečení časovače */
#define OVERFLOW 0xFF

void Timer0Init(void) {
     /* 1. Vynulujte registr čítače (Counter) časovače TPM0 */
     TPM0->CNT &= 0x00;

     /** 2. Nastavte hodnotu přetečení do Modulo registru TPM0 (doporučená
      * hodnota je definována výše uvedenou konstantou OVERFLOW, případně
      * experimentujte s jinými hodnotami v přípustném rozsahu změnou hodnoty
      * této konstanty). */
     TPM0->MOD = OVERFLOW;

     /** 3. Nastavte režim generování PWM na zvoleném kanálu (n) časovače v
      * řídicím registru TPM0_CnSC tohoto kanálu, konkrétně:
      *     Edge-aligned PWM: High-true pulses (clear Output on match,
      *         set Output on reload), přerušení ani DMA requests nebudou
      *         využívány. */
     TPM0->CONTROLS[2].CnSC = TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK;
     TPM0->CONTROLS[0].CnSC = TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK;
     // 0010 1000 (edge-aligned PWM preset) = 0x28

     /** 4. Nastavte konfiguraci časovače v jeho stavovém a řídicím registru
      * (SC):
      *     (up counting mode pro Edge-aligned PWM, Clock Mode Selection (01),
      *     Prescale Factor Selection (Divide by 8), bez využití přerušení či
      *     DMA. POZN.: !!!!! Budete-li masky v SC registru nastavovat postupně,
      *     je NUTNO toto provádět při Clock Mode Selection = 00 (tj. v režimu
      *     TPM disabled). */
     TPM0->SC = TPM_SC_CMOD(1) | TPM_SC_PS(3);
     // 0000 1011 (CMOD: increment counter, PS: divide by 8) = 0x0B
}

#define PWM_DELAY 5000

int main(void) {
     /** Příznak zvyšování (1) / snižování (0) compare */
     int increment = 1;

     /** Hodnota pro komparační registr (určující střídu PWM) */
     unsigned int compare = 0;

     MCUInit();
     SysInit();
     PortInit();
     Timer0Init();

     while (1) {
          if (increment) {
               /* Zvyšuj compare dokud není dosaženo hodnoty OVERFLOW (postupné
                * zvyšování jasu LED). Po negaci příznaku increment bude střída
                * snižována. */
               compare++;
               increment = !(compare >= OVERFLOW);
          } else {
               /* Snižuj compare dokud není dosaženo hodnoty 0 (postupné
                * snižování jasu LED). Po dosažení nulové hodnoty bude střída
                * opět zvyšována. */
               compare--;
               increment = (compare == 0x00);
          }

          /* 5. Přiřaďte aktuální hodnotu compare do komparačního registru
           * zvoleného kanálu časovače TPM0 (např. kanál č. 2 pro manipulaci s
           * červenou složkou RGB LED). */
          TPM0->CONTROLS[2].CnV = compare;
          TPM0->CONTROLS[0].CnV = compare;

          /* 6. LEDku nechte určitý čas svítit dle aktuální hodnoty střídy. Ve
           * skutečnosti LED velmi rychle bliká, přičemž vhodnou frekvenci
           * signálu PWM (danou hodnotou modulo registru časovače) zajistíme,
           * že blikání není pro lidské oko patrné a LEDka se tak jeví, že
           * svítí intenzitou odpovídající aktuální střídě PWM. ZDE VYUŽIJTE
           * PŘIPRAVENOU FUNKCI delay, EXPERIMENTÁLNĚ NASTAVTE HODNOTU ČEKÁNÍ
           * TAK, ABY BYLY PLYNULÉ ZMĚNY JASU LED DOBŘE PATRNÉ. */
          delay(PWM_DELAY);
     }
}
