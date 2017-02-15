/*********************************************************
**                                                      **
** PIC GMZ-2015                                         **
**-------------                                         **
**                                                      **
** Autor: Holger Becht                                  **
** Compiler: mikroC PRO 6.5                             **
** Date: March 2015                                     **
** Clock: 8 MHz internal                                **
** PIC 18F46K22                                         **
**                                                      **
** Version 1                                            **
** Library C_String_Special benötigt                    **
**                                                      **
*********************************************************/

/* TODO:
- Einstellen der Beleuchtungszeit bei Batteriebetrieb
- microsievers berechnen etc 
(siehe Bedienungsanleitung SMB-20 Geiger-Counter)
*/



#include "GLOBAL.h"
#include "GMZ_2015.h"
#include "INIT.h"
#include "LCD.h"
#include "ADC.h"
#include "MENU.h"



// *****************************************************************************
// ISR
// *****************************************************************************
void interrupt(void)
{
  signed short enc_new, enc_diff; // vars for encoder
  
  //*************************************
  // Tick-Ereignis mit Beeperstart (TMR6)
  //*************************************
  
  if(INT0IF_bit && INT0IE_bit)
  {

    tick_flag = 1;
    ticks++;
    tks_min_0eff[0]++;

    if (sound_flag) BEEP = 1;  // Beeper nur einschalten, wenn aktiviert
    if (LED_flag) LED = 1;  // LED nur einschalten, wenn aktiviert

    // Timer 6 starten
    TMR6 = 0x00;    // reset TMR6
    TMR6IF_bit = 0; // ISR Flag löschen
    TMR6ON_bit = 1; // start Timer6 -> 32 ms

    INT0IF_bit = 0;   // lösche isr-flag
  }
  
  //Beepzeit
  if(TMR6IF_bit)
  {
   BEEP = 0;   // Beeper aus
   LED = 0;    // LED aus
   TMR6ON_bit = 0; // stop Timer6
   TMR6 = 0;       // reset TMR6
   TMR6IF_bit = 0;   // lösche flag
  }

  
  //*************************************
  // Zeitzähler
  //*************************************

  if(CCP3IF_bit)
  {
    //Displaybelauchtung bei Batteriebetrieb
    if ((bat_flag) && (bat_count >= 10) && (DISP_BL))
    {
      DISP_BL_OFF            // Backlight aus
      bat_count = 0;
    }
    
    // Zeitzähler
    xtel_sek++;
    if (xtel_sek == 5)     // 0,2s pro isr
    {
     xtel_sek = 0;
     sek++;
     sek_sum++;
     if (DISP_BL) bat_count++; // wenn Backlight an
     sek_flag = 1;
     sek_flag_ADC_refresh = 1;
    }
    if (sek == 60)
    {
     sek = 0;
     min++;
     min_flag = 1;
     // ticks für Nulleffekt
     if (tks_min_0eff[0] <= 20)
     {
      tks_min_0eff[1] = tks_min_0eff[0]; // übergebe Ticks pro Minute zur Anzeige
      tks_sum_0eff += tks_min_0eff[0]; // bilde Summe aller gültigen Ticks pro Minute
      min_sum_0eff++; //addiere alle gültigen Minuten für Ticks pro Minute
     }
     else tks_min_0eff[1] = 0;  // Hintergrundstrahlung zulässig, dann lösche
     tks_min_0eff[0] = 0;
    }
    if (min == 60)
    {
     min = 0;
     hour++;
     hour_flag = 1;
    }
    if (hour == 24)
    {
     hour = 0;
     day++;
     day_flag = 1;
    }
    
    CCP3IF_bit = 0;   // lösche isr-flag
  }
  
  
  
  //*******************************
  // Encoderabfrage alle 2ms (TMR4)
  //*******************************
  if(TMR4IF_bit)
  {
    enc_new = 0;
    if( ENC_A ) enc_new = 3;
    if( ENC_B ) enc_new ^= 1;        // convert gray to binary
    enc_diff = enc_old - enc_new;                // difference last - new
    if( enc_diff & 1 )               // bit 0 = value (1)
    {
      enc_old = enc_new;                 // store new as next last
      enc_delta += (enc_diff & 2) - 1;        // bit 1 = direction (+/-)
     if (!(enc_delta % 4))   // nur jeden 4. Schritt zählen
     {
      if (bat_flag) 
      {
       DISP_BL_ON    // Backlight einschalten
       bat_count = 0;
      }
      // prüfen ob auf oder ab
      if (enc_diff & 2)
      {
       if (enc_count < 120)  enc_count++;    // 0-120
       if (enc_count_2 < 120) enc_count_2 = enc_count_2 + 2;   //doppelschritte

      }
      else 
      {
       if (enc_count < 0x88) enc_count--;    // 120-0
       if (enc_count_2 < 0x88)  enc_count_2 = enc_count_2 - 2;         //doppelschritte

      }
     }
    }

    // Encoder- Taster ENC_P
    // ----------------------

    if ((!ENC_P) && (!enc_p_hold)) enc_p_count = 0;        // taster wurde noch nicht
    else if ((ENC_P) && (enc_p_hold)) enc_p_count = 5;     // taster ist noch gedrückt
    else if ((!ENC_P) && (enc_p_hold))                     // taster wurde gerade losgelassen (entprell 5x2 ms)
    {
     enc_p_count--;
     if (enc_p_count == 0) enc_p_hold = 0;

    }
    else if ((ENC_P) && (!enc_p_hold))                     // taster wurde gerade gedrückt (entprell 5x2 ms)
    {
      enc_p_count++;
      if (enc_p_count >= 5)
      {
       if (bat_flag)
       {
        DISP_BL_ON    // Backlight einschalten
        bat_count = 0;
       }
       taster_count += 1; // Taster ist gedrückt
       enc_p_hold = 1;
      }
    }

    TMR4IF_bit = 0;   // lösche flag
  }
  
  

 }
 
// *****************************************************************************
// Hauptprogramm
// *****************************************************************************

void main()
{
    _pic_init();                                        // initialisation
    _lcd_init();
    _enc_init();
    _pwm_init();
    _tick_init();
    _time_init();
    GIEL_bit = 1;                                        // Enable Global_ISR
    GIEH_bit = 1;
    GIE_bit = 1;                                        // Enable Global_ISR
    _function_test();
    _hauptmenu();                                       // start programm

  while (1){};


}

// *****************************************************************************
// Funktionstest
// *****************************************************************************

void _function_test(void) {

    _lcd_clear();
    DISP_BL_ON            // Backlight an
    TMR3ON_bit = 1;         // Zeitzähler an

    // Spannungsmessung
    GO_DONE_bit = 1;                // ADC starten
    while (GO_DONE_bit == 1) {};    // Warten bis ADC fertig
    _ADCToInt(&adc_read);            // Umrechnung in Int
    
    if ((adc_read < 380) ||  (adc_read > 1300))   // außerhalb des Bereichs
    {
      _lcd(ROW1,'I');
     // Batteriespannung zu niedrig
     if (adc_read < 380) _romtolcd(ADC_text[1]);

     // Netzspannung zu hoch
     else _romtolcd(ADC_text[2]);

     // Spannungswert anzeigen
     _lcd(ROW2,'I');
     _romtolcd(ADC_text[0]);
     _ADCintToVolt(&adc_read, adc_string); // Umw. in String
     _vartolcd(adc_string);          // Ausgabe

     //Ende
     while (1);
    }

   // Batteriebetrieb
   if (adc_read < 480)  bat_flag = 1;       // setze Flag

   // Netzbetrieb
   else bat_flag = 0;       // setze Flag
   
   TMR4ON_bit = 1;     // Encoderabfrage einschalten
}