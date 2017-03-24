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

  while (1)
  {
    count = enc_count_2;          // übergebe counter Wert

    if (count >= max_pos) count = enc_count_2 = max_pos - 2;
    else if (count.B7) count = enc_count_2 = 0;

    // Drehencoder aktiviert
    if (count != count_last)
    {
     _lcd(CURSOR_BL_OFF,'I');
     _lcd(ROW1,'I');
     _romtolcd(main_menu_text[count]);
     _lcd(ROW2,'I');
     _romtolcd(main_menu_text[count+1]);

     // Spannungs (Auswahlpfeile)
     if (count == 2)
      {
       taster_count = 0;  // taster zähler rücksetzen
       _show_arrow(TOP_RIGHT);
       _show_arrow(TOP_LEFT);
       _lcd(ROW2,'I');
       if (bat_flag == 1) _romtolcd(ADC_text[3]); // Batterie oder Netz
       else _romtolcd(ADC_text[4]);
      }

     // Einstellungen (Auswahlpfeile)
     if (count == 4)
     {
      taster_count = 0;  // taster zähler rücksetzen
      _show_arrow(BOT_LEFT);
     }
     count_last = count;
    }  // End



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
  const signed short max_pos = 6; //Anzahl der menüpunkte * 2 Zeilen
  signed short count, count_last; // indikator
  unsigned short zf = 0; // Zeilenflag für Anzeige
  unsigned short chg_counter = 0; // Flag für Wechsel (ISR <-> Counter)

   
  char tick_text[11], byte_text[4], sievert_text[10];

  _lcd_clear();
  _lcd(CURSOR_BL_OFF,'I');    // cursor aus
  taster_count = 0;
  enc_count_2 = 0;                   // setze Encoder_Position auf 0
  count_last = 2;                    // vergleichsindikator für Encoder Pos.

  ticks = 0;
  TMR5ON_bit = 0;
  day = hour = min = sek = sek_sum = xtel_sek = TMR3L = TMR3H = 0;
  tks_min_0eff[0] = tks_min_0eff[1] = d_tks_min_0eff = tks_sum_0eff = min_sum_0eff = 0;  // ticks pro minute für Nulleffekt
  tks_per_sek = 0;
  tks_per_min[0] = tks_per_min[1] = 0;
  TMR3L_buff = TMR3H_buff = 0;
  sv = 0;

  TMR5ON_bit = 1; // starte Zeit-Timer

  PWM_ON;
  TMR3ON_bit = 1; // starte Tick-Zaehler

  delay_ms(1000);

  while (1)
  {
    count = enc_count_2;          // übergebe counter Wert

    if (count >= max_pos) count = enc_count_2 = max_pos - 2;
    else if (count.B7) count = enc_count_2 = 0;

     // aktualisiere alle Werte und Display, wenn neue Seite gewählt wurde
    if (count != count_last)
    {
     tick_flag = day_flag = hour_flag = min_flag = sek_flag = 1;
     _lcd(CURSOR_BL_OFF,'I');
     _lcd(ROW1,'I');
     _romtolcd(run_text[count]);
     _lcd(ROW2,'I');
     _romtolcd(run_text[count+1]);
     count_last = count;
     zf = 0;
    }

     // Bei > 100 Ticks/sec -> Beeper Dauersignal -> Int. aus
     if ( tks_per_sek > 100 )
     {
       TICKS_HV_OFF;
       if (sound_flag) BEEP = 1;  // Beeper nur einschalten, wenn aktiviert
       if (LED_flag) LED = 1;  // LED nur einschalten, wenn aktiviert
       chg_counter = 0;
     }
     else if ( !chg_counter )
     {
       TICKS_HV_ON;
       BEEP = 0;
       LED = 0;
       chg_counter = 1;
     }

      // Zeige Sievers
      // *************
     if (count == 0)
     {
       if (sek_flag)
       {
         // Berechne Sievers
         if(!tks_per_min[1] && tks_per_sek < 2) _show_message(1, count, &zf);
         else
         {
           _show_message(0, count, &zf);

           if (tks_per_sek > 1) sv = (tks_per_sek / 1.5);
           else if (tks_per_min[1]) sv = (tks_per_min[1] / 90.00); // /60 /1.5
           else sv = 0;

           _lcd(ROW2+7,'I');  // Cursor Position
           
           if( sv < 1.0 )
           {
            _romtolcd(einheit_text[0]);
            if (sv) sv = sv * 1000;
           }

           else if( sv < 1000.0 ) _romtolcd(einheit_text[1]);
           
           else
           {
             _romtolcd(einheit_text[2]);
             sv = sv / 1000;
           }
           
           _romtolcd(einheit_text[3]);

           _lcd(ROW2,'I');  // Cursor Position
           FloatToStr_FixLen(sv, sievert_text,7);   // convert to string
           _vartolcd(sievert_text);          // Ausgabe
         }

        sek_flag = 0;
       }
     }
    
    
    // Zeige Ticks ueber Zeit
    //***********************
    else if (count == 2)
    {

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
       // Wenn ISR für Ticks aus
       if (INT0IE_bit == 0)
       { 
         ticks = ticks + tks_per_sek;
         tick_flag = 1;
       }
        _lcd(ROW2+15,'I');
        ByteToStr(sek, byte_text);   // convert to string
        if(byte_text[1] == ' ') byte_text[1] = '0';
        _vartolcd(byte_text+1);          // Ausgabe(ohne Vorzeichen)
        sek_flag = 0;
      }
     
      if (tick_flag)
      {
        _lcd(ROW1+7,'I');
        LongWordToSTR(ticks, tick_text);   // convert to string
        _vartolcd(tick_text);          // Ausgabe(ohne Vorzeichen)
        tick_flag = 0;
      }
     }

     // Zeige Hintergrundstrahlung
     //***************************
     else if (count == 4)
     {
       if ( tks_per_min[1] > 20 || tks_per_sek > 1 ) _show_message(2, count, &zf);
       else if ( !tks_per_min[1] ) _show_message(1, count, &zf);
       else  if ( tks_per_min[1] <= 20 )
       {
         _show_message(0, count, &zf);
         // Zeige Werte an
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
     }

    // zurück in das Hauptmenü
    if (taster_count)
    {
     TICKS_HV_OFF;                       // Ticks aus
     BEEP = 0;                           // Beeper aus
     LED = 0;                            // LED aus
     PWM_OFF                            // PWM ausschalten
     TMR3ON_bit = 0;                      // stoppe Zaehler
     taster_count = 0;                  // Taster zähler rücksetzen
     return;                            // Rücksprung
    }
  };
}


// *****************************************************************************
//  Wartezeit anzeigen bei (noch) nicht auswertbaren  Messungen
// *****************************************************************************

void _show_message(unsigned short show_line, signed short count, unsigned short *zf)
{
  unsigned short min_rest = 0; // rest einer minute
  char byte_text[4];
  
  switch (show_line)
  {
    case 1:  // Zeige "Bitte Warten"
      if (*zf != 1)
      {
         *zf = 1;
         _lcd(ROW2,'I');
         _romtolcd(run_text_ext[0]);
      }
       min_rest = 60-sek;
       _lcd(ROW2+13,'I');
       ByteToStr(min_rest, byte_text);   // convert to string
      _vartolcd(byte_text);          // Ausgabe(ohne Vorzeichen)
      break;

    case 2:  // Zeige "Spannung zu hoch"
      if (*zf != 2)
      {
         *zf = 2;
         _lcd(ROW2,'I');
         _romtolcd(run_text_ext[1]);
      }
      break;


    default:  // Zeige Werte (Grundeinstellung)
      if (*zf != 3)
      {
        *zf = 3;
        _lcd(ROW2,'I');
        _romtolcd(run_text[count+1]);
      }
  }
}


// *****************************************************************************
// Settings auswählen
// *****************************************************************************

void _settings(void)
{
 const signed short max_pos = 8; //Anzahl der menüpunkte * 2 Zeilen
 signed short count, count_last, flag_last; // indikator

  taster_count = 0;
  _lcd_clear();
  enc_count_2 = 0;                   //setze Encoder_Position auf 0
  count_last = 2;                    // vergleichsindikator für Encoder Pos.
  flag_last = 0;

  while (1){

    count = enc_count_2;          // übergebe counter Wert

    if (count >= max_pos) count = enc_count_2 = max_pos;
    else if (count.B7) count = enc_count_2 = 0;

    // Drehencoder aktiviert
    if ((count != count_last) || (flag_last))
    {
     flag_last = 0;  // Auswahl aktualisieren
    
     _lcd(CURSOR_BL_OFF,'I');
     _lcd(ROW1,'I');
     _romtolcd(set_menu_text[count]);
     _lcd(ROW2,'I');
     _romtolcd(set_menu_text[count+1]);


     switch( count )
     {
       case 0: // Pfeil rechts + Checkbox
         taster_count = 0;  // taster zähler rücksetzen
         _show_arrow(BOT_RIGHT);
         _show_checkbox(TOP_RIGHT, sound_flag);
         break;


       case 2: // Pfeil rechts & links + Checkbox
         taster_count = 0;  // taster zähler rücksetzen
         _show_arrow(BOT_RIGHT);
         _show_arrow(BOT_LEFT);
         _show_checkbox(TOP_RIGHT, led_flag);
         break;


       case 4: // Pfeil rechts & links
         taster_count = 0;  // taster zähler rücksetzen
         _show_arrow(BOT_RIGHT);
         _show_arrow(BOT_LEFT);
         break;
         
         
       case 6: // Pfeil rechts & links
         taster_count = 0;  // taster zähler rücksetzen
         _show_arrow(BOT_RIGHT);
         _show_arrow(BOT_LEFT);
         break;
         
       /*
       case 8: // Pfeil rechts & links
         taster_count = 0;  // taster zähler rücksetzen
         _show_arrow(BOT_RIGHT);
         _show_arrow(BOT_LEFT);
         break;
       */
         
       case 8: // Pfeil links
         taster_count = 0;  // taster zähler rücksetzen
         _show_arrow(BOT_LEFT);
         break;
       
       default: break;
     }
     count_last = count;
   }
  

   // Wenn Taster gedrückt
   if (taster_count)
   {
     switch( count )
     {
     case 0:  // Beeper aktivieren oder deaktivieren
       if (sound_flag) sound_flag = 0;
       else sound_flag = 1;
       EEPROM_write(BEEP_EEPROM, sound_flag);
       flag_last = 1; //ausgabe aktivieren
       break;
     

     case 2:  // LED aktivieren oder deaktivieren
       if (LED_flag) LED_flag = 0;
       else LED_flag = 1;
       EEPROM_write(LED_EEPROM, LED_flag);
       flag_last = 1; // ausgabe aktivieren
       break;


     case 4:  // Hochspannung einstellen auswahl
      _info_HV();
      _lcd_clear();
      count = 4;
      enc_count_2 = 4;                   //setze Encoder_Position
      count_last = 2;                    // vergleichsindikator für Encoder Pos.
      break;
     
     /*
     case 6:  // Zeit-Basis einstellen auswahl
      _info_time_base();
      _lcd_clear();
      count = 0;
      enc_count_2 = 0;                   //setze Encoder_Position
      count_last = 2;                    // vergleichsindikator für Encoder Pos.
      break;
     */

     case 6:  // Informationen anzeigen
      _info_box();
      _lcd_clear();
      count = 6;
      enc_count_2 = 6;                   //setze Encoder_Position
      count_last = 2;                    // vergleichsindikator für Encoder Pos.
      break;

     case 8: return;
    
     default: break;
    }
     taster_count = 0;  // taster zähler rücksetzen
   }
  };
}
 
 
// *****************************************************************************
// - VORABINFO - Hochspannung einstellen
// *****************************************************************************
void _info_HV(void)
{
   const signed short max_pos = 6; //Anzahl der menüpunkte - 2 Zeilen
   signed short count, count_last; // indikator

  taster_count = 0;
  _lcd_clear();
  enc_count_2 = 0;                   //setze Encoder_Position auf 0
  count_last = 2;                    // vergleichsindikator für Encoder Pos.

  while (1){

    count = enc_count_2;          // übergebe counter Wert

    if (count > max_pos) count = enc_count_2 = max_pos;
    else if (count.B7) count = enc_count_2 = 0;

    if (count != count_last)
    {
     _lcd(CURSOR_BL_OFF,'I');
     _lcd(ROW1,'I');
     _romtolcd(HV_menu_info[count]);
     _lcd(ROW2,'I');
     _romtolcd(HV_menu_info[count+1]);

     // Auswahlpfeil einblenden

    if (!count) _show_arrow(BOT_RIGHT);
    else if (count > 1  &&  count < max_pos)
    {
     _show_arrow(BOT_RIGHT);
     _show_arrow(BOT_LEFT);
    }
    else
     {
      taster_count = 0;  // taster zähler rücksetzen
      _show_arrow(BOT_LEFT);
     }
     count_last = count;
    }

   if (count == max_pos)
   {
     
     taster_count = 0;
     enc_count =  0;                      //setze Encoder_Position auf startwert

     while (!taster_count)
     {
       count = enc_count;          // übergebe counter Wert
      
      // Auswahl "Abbrechen"
       if (count <= 0)
       {
          _show_checkbox(TOP_LEFT, OFF);
          _show_checkbox(BOT_LEFT, ON);

          while ( count <= 0 )
          {
            if (taster_count) return;
            count = enc_count;          // übergebe counter Wert
            if (count < 0 ) count = enc_count = 0;
          };
       }
       
       // Auswahl "Starten"
       else if (count >= 1)
       {
         _show_checkbox(BOT_LEFT, OFF);
         _show_checkbox(TOP_LEFT, ON);

         while ( count >= 1 )
          {
            if (taster_count) 
            {
             _set_HV();
             return;
            }
            count = enc_count;          // übergebe counter Wert
            if (count > 1 ) count = enc_count = 1;
          };
       }
     
     };
    }
 }
   _set_HV();
}
  
  
// *****************************************************************************
// Hochspannung einstellen
// *****************************************************************************
void _set_HV(void)
{
  signed short count, count_last, count_hold; // indikator

  char percent_text[5]; // String für Prozentwert auf LCD

  PWM_PW = 5*249/100; // auf 5% voreinstellen
  TICKS_HV_ON;

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
  enc_count =  5;                   // setze Encoder_Position auf startwert 5%
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
  TICKS_HV_OFF                          // Ticks ausschalten

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
 const signed short max_pos = 8; //Anzahl der menüpunkte - 2 Zeilen
 signed short count, count_last; // indikator

  taster_count = 0;
  _lcd_clear();
  enc_count_2 = 0;                   //setze Encoder_Position auf 0
  count_last = 2;                    // vergleichsindikator für Encoder Pos.

  while (1){

    count = enc_count_2;          // übergebe counter Wert

    if (count > max_pos) count = enc_count_2 = max_pos;
    else if (count.B7) count = enc_count_2 = 0;

    if (count != count_last)
    {
     _lcd(CURSOR_BL_OFF,'I');
     _lcd(ROW1,'I');
     _romtolcd(info_menu_text[count]);
     _lcd(ROW2,'I');
     _romtolcd(info_menu_text[count+1]);

     // Auswahlpfeil einblenden
     
    if (!count) _show_arrow(BOT_RIGHT);
    else if (count > 1  &&  count < max_pos)
    {
     _show_arrow(BOT_RIGHT);
     _show_arrow(BOT_LEFT);
    }
    else
     {
      taster_count = 0;  // taster zähler rücksetzen
      _show_arrow(BOT_LEFT);
     }
     count_last = count;
    }

   if ((count == max_pos) && (taster_count)) return;

  };
}


// *****************************************************************************
// Navigations-Pfeile anzeigen
// *****************************************************************************
void _show_arrow(unsigned short pos)
{
  switch (pos)
  {
   case TOP_LEFT:
     _lcd(ROW1,'I');
     _lcd(ARROW_LEFT,'D');
     break;
   
   case TOP_RIGHT:
     _lcd(ROW1+19,'I');
     _lcd(ARROW_RIGHT,'D');
     break;
     
    case BOT_LEFT:
     _lcd(ROW2,'I');
     _lcd(ARROW_LEFT,'D');
     break;

   case BOT_RIGHT:
     _lcd(ROW2+19,'I');
     _lcd(ARROW_RIGHT,'D');
     break;
   
   default: break;
  }
}



// *****************************************************************************
// CheckBoxe anzeigen
// *****************************************************************************
void _show_checkbox(unsigned short pos, unsigned short state)
{
  switch (pos)
  {
   case TOP_LEFT:
     _lcd(ROW1,'I');
     if (state) _lcd(CHECKMARK,'D');
     else _lcd(CHECKBOX,'D');
     _lcd(ROW1,'I');
     _lcd(CURSOR_BL_ON,'I');
     break;

   case TOP_RIGHT:
     _lcd(ROW1+19,'I');
     if (state) _lcd(CHECKMARK,'D');
     else _lcd(CHECKBOX,'D');
     _lcd(ROW1+19,'I');
     _lcd(CURSOR_BL_ON,'I');
     break;

    case BOT_LEFT:
      _lcd(ROW2,'I');
      if (state) _lcd(CHECKMARK,'D');
      else _lcd(CHECKBOX,'D');
      _lcd(ROW2,'I');
      _lcd(CURSOR_BL_ON,'I');
      break;

   case BOT_RIGHT:
     _lcd(ROW2+19,'I');
     if (state) _lcd(CHECKMARK,'D');
     else _lcd(CHECKBOX,'D');
     _lcd(ROW2+19,'I');
     _lcd(CURSOR_BL_ON,'I');
     break;

   default: break;
  }
}



