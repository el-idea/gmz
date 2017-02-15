/*********************************************************
**                                                      **
** PIC GMZ-2015                                         **
** MENUE-Routinen                                         **
**                                                      **
** Autor: Holger Becht                                  **
** Compiler: mikroC PRO 6.5                             **
** Date: March 2015                                     **
** Clock: 8 MHz internal                                **
** PIC 18F46K22                                         **
**                                                      **
** Version 1                                            **
**                                                      **
*********************************************************/

#include "MENU.h"
#include "GLOBAL.h"
#include "LCD.h"
#include "ADC.h"


// *****************************************************************************
// Hauptmenü
// *****************************************************************************

void _hauptmenu(void)
{
  const signed short max_pos = 6; //Anzahl der menüpunkte * 2 Zeilen
  signed short count, count_last; // indikator

  _lcd_clear();
  taster_count = 0;
  enc_count_2 = 0;                   //setze Encoder_Position auf 0
  count_last = 2;                    // vergleichsindikator für Encoder Pos.

  while (1){

    count = enc_count_2;          // übergebe counter Wert

    if (count >= max_pos) count = enc_count_2 = max_pos - 2;
    else if (count.B7) count = enc_count_2 = 0;

    if (count != count_last)
    {
     _lcd(CURSOR_BL_OFF,'I');
     _lcd(ROW1,'I');
     _romtolcd(main_menu_text[count]);
     _lcd(ROW2,'I');
     _romtolcd(main_menu_text[count+1]);

     // Spannungsanzeige (nur TEXT)
     if (count == 2)
      {
       _lcd(ROW2,'I');
       if (bat_flag == 1) _romtolcd(ADC_text[3]); // Batterie oder Netz
       else _romtolcd(ADC_text[4]);
      }

     // Auswahlpfeil einblenden
     if ((count == 2) || (count == 4))
     {
      taster_count = 0;  // taster zähler rücksetzen
      if (count == 2) _lcd(ROW1+19,'I');
      else _lcd(ROW2+19,'I');
      _lcd(ARROW_RIGHT,'D');
      if (count == 2) _lcd(ROW1+19,'I');
      else _lcd(ROW2+19,'I');
      _lcd(CURSOR_BL_ON,'I');
     }
     count_last = count;
    }

   // Spannungsanzeige (nur WERT)
   if ((count == 2) && (sek_flag_ADC_refresh))
   {
     GO_DONE_bit = 1;                // ADC starten
     while (GO_DONE_bit == 1) {};    // Warten bis ADC fertig
     _ADCToInt(&adc_read);            // Umrechnung in Int
     _ADCintToVolt(&adc_read, adc_string); // Umw. in String
     if (bat_flag == 1) _lcd(ROW2+9,'I'); // Nach Text "Batterie:"
     else _lcd(ROW2+14,'I');               // Nach Text "Netzspannung:"
     _vartolcd(adc_string);          // Ausgabe
     _lcd(ROW1+19,'I');
     sek_flag_ADC_refresh = 0;   //aktualisere im Sekundentakt
   }

   // Messung starten
   if ((count == 2) && (taster_count)) 
   {
    _start_GMZ();
    _lcd_clear();
    taster_count = 0;
    count = 0;
    enc_count_2 = 0;                   //setze Encoder_Position auf 0
    count_last = 2;                    // vergleichsindikator für Encoder Pos.
   }

   // Settings auswählen
   if ((count == 4) && (taster_count)) 
   {
    _settings();
    _lcd_clear();
    taster_count = 0;
    count = 0;
    enc_count_2 = 0;                   //setze Encoder_Position auf 0
    count_last = 4;                    // vergleichsindikator für Encoder Pos.
   }
  };
}


// *****************************************************************************
// GMZ-counter starten
// *****************************************************************************
void _start_GMZ(void)
{
  const signed short max_pos = 4; //Anzahl der menüpunkte * 2 Zeilen
  signed short count, count_last; // indikator
  
  char tick_text[11], byte_text[4];

  _lcd_clear();
  _lcd(CURSOR_BL_OFF,'I');    // cursor aus
  taster_count = 0;
  enc_count_2 = 0;                   //setze Encoder_Position auf 0
  count_last = 2;                    // vergleichsindikator für Encoder Pos.

  ticks = 0;
  TMR3ON_bit = 0;
  day = hour = min = sek = sek_sum = xtel_sek = TMR3L = TMR3H = 0;
  tks_min_0eff[0] = tks_min_0eff[1] = d_tks_min_0eff = tks_sum_0eff = min_sum_0eff = 0;  // ticks pro minute für Nulleffekt
  TMR3ON_bit = 1;

  PWM_ON;
  TICKS_ON;

  while (1)
  {

    count = enc_count_2;          // übergebe counter Wert

    if (count >= max_pos) count = enc_count_2 = max_pos - 2;
    else if (count.B7) count = enc_count_2 = 0;

    if (count != count_last)
    {
     tick_flag = day_flag = hour_flag = min_flag = sek_flag = 1;   // zeige bei neuem Durchlauf die Werte an
     _lcd(CURSOR_BL_OFF,'I');
     _lcd(ROW1,'I');
     _romtolcd(run_text[count]);
     _lcd(ROW2,'I');
     _romtolcd(run_text[count+1]);
     count_last = count;
    }

    if (count == 0)
    {
      if (tick_flag)
      {
        _lcd(ROW1+7,'I');
        LongWordToSTR(ticks, tick_text);   // convert to string
        _vartolcd(tick_text);          // Ausgabe(ohne Vorzeichen)
        tick_flag = 0;
      }

      if (day_flag)
      {
        _lcd(ROW2+5,'I');
        ByteToStr(day, byte_text);   // convert to string
        _vartolcd(byte_text);          // Ausgabe(ohne Vorzeichen)
        day_flag = 0;
      }

      if (hour_flag)
      {
        _lcd(ROW2+9,'I');
        ByteToStr(hour, byte_text);   // convert to string
        if(byte_text[1] == ' ') byte_text[1] = '0';
        _vartolcd(byte_text+1);          // Ausgabe(ohne Vorzeichen)
        hour_flag = 0;
      }

      if (min_flag)
      {
        _lcd(ROW2+12,'I');
        ByteToStr(min, byte_text);   // convert to string
         if(byte_text[1] == ' ') byte_text[1] = '0';
        _vartolcd(byte_text+1);          // Ausgabe(ohne Vorzeichen)
        min_flag = 0;
      }

       if (sek_flag)
      {
        _lcd(ROW2+15,'I');
        ByteToStr(sek, byte_text);   // convert to string
        if(byte_text[1] == ' ') byte_text[1] = '0';
        _vartolcd(byte_text+1);          // Ausgabe(ohne Vorzeichen)
        sek_flag = 0;
      }
     }
     
     else if (count == 2)
     {
       if (min_flag)
       {
        // Nulleffekt
        _lcd(ROW2+11,'I');
        ByteToStr(tks_min_0eff[1], byte_text);   // convert to string
        _vartolcd(byte_text+1);          // Ausgabe(ohne Vorzeichen)
       
        // Nulleffekt-Durchschnitt aller bisherigen  Nulleffekt-Messungen
        _lcd(ROW2+15,'I');
        _lcd(AVERAGE,'D');
        _lcd(ROW2+18,'I');
        if (!min_sum_0eff) d_tks_min_0eff = 0; //erster durchlauf
        else d_tks_min_0eff = tks_sum_0eff/min_sum_0eff;
        ByteToStr(d_tks_min_0eff, byte_text);   // convert to string
        _vartolcd(byte_text+1);          // Ausgabe(ohne Vorzeichen)
        
        min_flag = 0;
       }
     }

    // zurück in das Hauptmenü
    if (taster_count)
    {
     PWM_OFF                            // PWM ausschalten
     TICKS_OFF                          // Ticks ausschalten
     taster_count = 0;                  // Taster zähler rücksetzen
     return;                            // Rücksprung
    }
  };
}



// *****************************************************************************
// Settings auswählen
// *****************************************************************************

void _settings(void)
{
 const signed short max_pos = 10; //Anzahl der menüpunkte * 2 Zeilen
 signed short count, count_last, flag_last; // indikator

  taster_count = 0;
  _lcd_clear();
  enc_count_2 = 0;                   //setze Encoder_Position auf 0
  count_last = 2;                    // vergleichsindikator für Encoder Pos.
  flag_last = 0;

  while (1){

    count = enc_count_2;          // übergebe counter Wert

    if (count >= max_pos) count = enc_count_2 = max_pos - 2;
    else if (count.B7) count = enc_count_2 = 0;

    if ((count != count_last) || (flag_last))
    {
     flag_last = 0;  // Auswahl aktualisieren
    
     _lcd(CURSOR_BL_OFF,'I');
     _lcd(ROW1,'I');
     _romtolcd(set_menu_text[count]);
     _lcd(ROW2,'I');
     _romtolcd(set_menu_text[count+1]);

     // Checkbox oder Auswahlhaken einblenden
     if (count == 0)
     {
      taster_count = 0;  // taster zähler rücksetzen
      _lcd(ROW1+19,'I');
      if (sound_flag) _lcd(CHECKMARK,'D');
      else _lcd(CHECKBOX,'D');
      _lcd(ROW1+19,'I');
      _lcd(CURSOR_BL_ON,'I');
     }
     
     if (count == 2)
     {
      taster_count = 0;  // taster zähler rücksetzen
      _lcd(ROW1+19,'I');
      if (LED_flag) _lcd(CHECKMARK,'D');
      else _lcd(CHECKBOX,'D');
      _lcd(ROW1+19,'I');
      _lcd(CURSOR_BL_ON,'I');
     }
     
     // Auswahlpfeil in erster oder zweiter Reihe einblenden
     if (count >= 4)
     {
      taster_count = 0;  // taster zähler rücksetzen
      if (count == 8)
      {
       _lcd(ROW1,'I');
       _lcd(ARROW_LEFT,'D');
       _lcd(ROW1,'I');
       _lcd(CURSOR_BL_ON,'I');
      }
      else
      {
       _lcd(ROW2+19,'I');
       _lcd(ARROW_RIGHT,'D');
       _lcd(ROW2+19,'I');
       _lcd(CURSOR_BL_ON,'I');
      }
     }
     count_last = count;
    }
  
   // Wenn Taster gedrückt
   if (taster_count)
   {
     // Beeper aktivieren oder deaktivieren
     if (count == 0)
     {
       if (sound_flag) sound_flag = 0;
       else sound_flag = 1;
       EEPROM_write(BEEP_EEPROM, sound_flag);
       flag_last = 1; //ausgabe aktivieren
     }
     
     // Beeper aktivieren oder deaktivieren
     if (count == 2)
     {
       if (LED_flag) LED_flag = 0;
       else LED_flag = 1;
       EEPROM_write(LED_EEPROM, LED_flag);
       flag_last = 1; // ausgabe aktivieren
     }

     // Hochspannung einstellen auswahl
     if (count == 4)
     {
      _set_HV();
      _lcd_clear();
      count = 0;
      enc_count_2 = 0;                   //setze Encoder_Position auf 0
      count_last = 2;                    // vergleichsindikator für Encoder Pos.
     }

     // Informationen anzeigen
     if (count == 6)
     {
      _info_box();
      _lcd_clear();
      count = 0;
      enc_count_2 = 0;                   //setze Encoder_Position auf 0
      count_last = 2;                    // vergleichsindikator für Encoder Pos.
     }

     if (count == 8) return;
     
     taster_count = 0;  // taster zähler rücksetzen
   }
   
  };
}
  
  
// *****************************************************************************
// Hochspannung einstellen
// *****************************************************************************
void _set_HV(void)
{
  signed short count, count_last, count_hold; // indikator

  char percent_text[5]; // String für Prozentwert auf LCD

  PWM_PW = 5*249/100; // auf 5% voreinstellen
  TICKS_ON;

  led_flag = 1;    // aktiviere LED ein
  sound_flag = 1;  // aktiviere Beeper

  _lcd(ROW1,'I');
  _romtolcd(HV_menu_text[0]);
  _lcd(ROW2,'I');
  _romtolcd(HV_menu_text[1]);


  // Lese HV-Wert und zeige auf LCD als "ALTER WERT" an
  ShortToStr(EEPROM_read(HV_EEPROM), percent_text);
  _lcd(ROW2+4,'I');
  _vartolcd(percent_text+1);          // Ausgabe(ohne Vorzeichen)

  // Neuen Wert einstellen

  PWM_ON                            // PWM einschalten
  taster_count = 0;
  enc_count =  5;                   //setze Encoder_Position auf startwert 5%
  count_last =  enc_count+1;        // vergleichsindikator für Encoder Pos.

  while (!taster_count)
  {
    count = enc_count;          // übergebe counter Wert

    if (count >= HV_MAX) count = enc_count = HV_MAX;
    else if (count.B7) count = enc_count = 0;

    if (count != count_last)
    {
      ShortToStr(count, percent_text);
      _lcd(ROW2+16,'I');
      _vartolcd(percent_text+1);          // Ausgabe(ohne Vorzeichen)
      _lcd(ROW2+15,'I');
      count_last = count;
      PWM_PW = count*249/100;             //Hochspannung einstellen
    }
  }


  // Wert durch User bestätigen
  _lcd(ROW1,'I');
  _romtolcd(HV_menu_text[2]);
  _lcd(ROW2,'I');
  _romtolcd(HV_menu_text[3]);
  _lcd(ROW1+12,'I');
  _vartolcd(percent_text+1);          // Ausgabe(ohne Vorzeichen)
  _lcd(ROW2+12,'I');

  count_hold = count;                 // behalte den eingstellten Prozentwert
  taster_count = 0;
  enc_count =  0;                      //setze Encoder_Position auf startwert

  while (!taster_count)
  {
    count = enc_count;          // übergebe counter Wert

    if (count >= 1)
    {
     count = enc_count = 1;
     _lcd(ROW2+17,'I');
    }
    else if (count <= 0)
    {
     count = enc_count = 0;
     _lcd(ROW2+12,'I');
    }
  }

  // Speichern/Abbruch durch PIC bestätigen


  PWM_OFF                            // PWM ausschalten
  TICKS_OFF                          // Ticks ausschalten

  _lcd(ROW2,'I');

  if (count <= 0) _romtolcd(HV_menu_text[5]);     // Abbruch
  else                                            // Speichern
  {
    _romtolcd(HV_menu_text[4]);
    EEPROM_write(HV_EEPROM, count_hold);         // speichere HV-Wert in EEPROM
  }
  _delay_10ms(200);                          //2 Sekunden warten

  led_flag = EEPROM_read(LED_EEPROM);    // lese Einstelleng der LED
  sound_flag = EEPROM_read(BEEP_EEPROM);    // lese Einstelleng des Beepers
  return;
}


// *****************************************************************************
// Informationen zum Gerät anzeigen
// *****************************************************************************

void _info_box(void)
{
 const signed short max_pos = 12; //Anzahl der menüpunkte * 2 Zeilen
 signed short count, count_last; // indikator

  taster_count = 0;
  _lcd_clear();
  enc_count_2 = 0;                   //setze Encoder_Position auf 0
  count_last = 2;                    // vergleichsindikator für Encoder Pos.

  while (1){

    count = enc_count_2;          // übergebe counter Wert

    if (count >= max_pos) count = enc_count_2 = max_pos - 2;
    else if (count.B7) count = enc_count_2 = 0;

    if (count != count_last)
    {
     _lcd(CURSOR_BL_OFF,'I');
     _lcd(ROW1,'I');
     _romtolcd(info_menu_text[count]);
     _lcd(ROW2,'I');
     _romtolcd(info_menu_text[count+1]);

     // Auswahlpfeil einblenden
    if (count == 10)
     {
      taster_count = 0;  // taster zähler rücksetzen
      _lcd(ROW1,'I');
      _lcd(ARROW_LEFT,'D');
      _lcd(ROW1,'I');
      _lcd(CURSOR_BL_ON,'I');
     }
     count_last = count;
    }

   if ((count == 10) && (taster_count)) return;

  };
}