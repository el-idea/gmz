/*********************************************************
**                                                      **
** PIC GMZ-2015                                         **
** GLOBALE-Deklarationen                                **
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

#include "GLOBAL.h"


//ß                  "\xE2"   // Umlaut LCD Adresse
//ä                  "\xE1"   // Umlaut LCD Adresse
//ü                  "\x7E"   // Umlaut LCD Adresse
//ö                  "\xF5"   // Umlaut LCD Adresse


const code char *ADC_text[5] = {    "SPANNUNG: ",
                                    "BATTERIE LEER",
                                    "NETZSPANNUNG ZU HOCH",
                                    "BATTERIE:",
                                    "NETZSPANNUNG:"
                                    };



const code char *main_menu_text[6] = {   "      GMZ 2017      ",
                                         "    GSI DARMSTADT   ",

                                         "  MESSUNG STARTEN   ",
                                         "                    ",
                                         
                                         "    EINSTELLUNGEN   ",
                                         "       AENDERN      "
                                    };


const code char *set_menu_text[14] =    {"SIGNALGEBER AKTIV   ",
                                         "                    ",
                                         
                                         "LED AKTIV           ",
                                         "                    ",

                                         "    HOCHSPANNUNG    ",
                                         "     EINSTELLEN     ",

                                         "     ZEIT-BASIS     ",
                                         "    FEINABGLEICH    ",

                                         "INFORMATIONEN UEBER ",
                                         "    DIESES GERAET   ",
                                         
                                         "                    ",
                                         "      ZURUECK       "
                                    };




const code char *info_menu_text[10] =   {"AUSBILDUNGSABTEILUNG",
                                         "    GSI DARMSTADT   ",
                                         
                                         "PRAKTIKANTEN-BAUSATZ",
                                         "     PROTOTYP V3    ",
                                         
                                         "ZAEHLROHR: UNGEEICHT",
                                         " ENERGIEKOMPENSIERT ",
                                         
                                         "      ZEITBASIS     ",
                                         "        +/-2%       ",

                                         "                    ",
                                         "      ZURUECK       "
                                    };


const code char *HV_menu_info[8] = {     "  ACHTUNG! HINWEIS! ",
                                         "                    ",
                                         
                                         "WIRD DIE HV ZU HOCH ",
                                         "    EINGESTELLT,    ",

                                         " KANN DAS ZAEHLROHR ",
                                         " BESCHAEDIGT WERDEN ",
                                         
                                         "    STARTEN         ",
                                         "    ABBRECHEN       "
                                    };



const code char *timebase_info[12] = {   "  FUER DEN ABGLEICH ",
                                         "      WIRD EIN      ",

                                         " FUNKTIONSGENERATOR ",
                                         "  60 KHZ BENOETIGT  ",
                                         
                                         "      ZAEHLROHR     ",
                                         "     ENTFERNEN!!    ",

                                         "  TTL-SIGNAL AN X6  ",
                                         " (KATHODE) ANLEGEN  ",
                                         
                                         "MIT DREHKNOPF AUF CA",
                                         " 60 KHZ ABGLEICHEN  ",

                                         "    STARTEN         ",
                                         "    ABBRECHEN       "
                                    };




const code char *HV_menu_text[6] = {     "MESSEN SIE AN HV-MP1",
                                         "ALT:   %    NEU:   %",
                                         
                                         "ACHTUNG! HV:   %    ",
                                         "SPEICHERN?  NEIN  JA",
                                         
                                         "   GESPEICHERT!!    ",
                                         "NICHT GESPEICHERT!! "
                                    };
                                    
                                    

const code char *osctune_menu_text[6] = {"MESSUNG LAEUFT      ",
                                         "FREQUENZ:        HZ ",

                                         "ABGLEICH SPEICHERN? ",
                                         "    NEIN  /  JA     ",

                                         "   GESPEICHERT!!    ",
                                         "NICHT GESPEICHERT!! "
                                    };




const code char *run_text[6] =      {    "UMG.AEQUIVALENTDOSIS",
                                         "                    ",
                                         
                                         "TICKS:              ",
                                         "ZEIT:   :  :  :     ",
                                         
                                         "HINTERGRUNDSTRAHLUNG",
                                         "TICKS/MIN:      :   "
                                    };


const code char *run_text_ext[2] =  {
                                         "BITTE WARTEN:    SEK",
                                         "STRAHLUNG ZU HOCH!!!"
                                    };


const code char *einheit_text[4] = {     " nSv/h",   // nano
                                         " äSv/h",   // ä = µ in display chart
                                         " mSv/h",   // milli
                                         " H*(10)"
                                    };


// *****************************************************************************
// Deklaration der Globalen Variablen
// *****************************************************************************

// Encoder
signed short          enc_old = 0,
                      enc_count = 0,    //-120...120
                      enc_count_2 = 0,  //-60...60
                      enc_delta = 0,    //-127...127
                      enc_p_hold = 0,
                      enc_p_count = 0;

// Zählrohr
unsigned long         ticks = 0,         // counter for ticks (11 Stellen)
                      tks_sum_0eff = 0,  // sum of all ticks per minute <=20
                      min_sum_0eff = 0,  // sum of all minutes with valid ticks per minute <=20
                      sek_sum = 0;

unsigned int          tks_min_0eff[2] = {0}, // Ticks/min
                                             // [0]-temp counter
                                             // [1]-disp counter
                      d_tks_min_0eff = 0,    // Durchschnitt Ticks/min
                      
                      tks_per_sek = 0,       // Ticks pro Sekunde
                      tks_per_sek_old = 0,   // Ticks pro Sekunde alter wert
                      tks_per_min[2] = {0};  // Ticks pro Minute alter wert
                      
float                 sv = 0;                // Sievert

// Tick-Zähler
unsigned short        TMR3L_buff = 0,        // Kopie des Zählers
                      TMR3H_buff = 0;        // Kopie des Zählers

// Zählervars
unsigned short        counter = 0,
                      taster_count = 0, // taster
                      bat_count = 0;

// Flags
unsigned short        bat_flag = 0,  // batterie
                      tick_flag = 0, // flag zur textaktualisierung bei neuen ticks
                      led_flag = 0,
                      sound_flag = 0,
                      sek_flag_ADC_refresh = 0, //Sekundentakt für ADC-Wert Aktualisierung
                      tmts = 0, // Flag für ticks/min auf ticks/sek
                      new_flag = 0; // flag um Anzeige nach Änderung zu erneuern
                      
// Oscillator-Tune
unsigned short        osc_tune = 0;  // Einstellung für OSCTUNE-Register (PLL off)


// Zeit
unsigned short        xtel_sek = 0,   // Teil-Sekundenzähler
                      sek = 0,        // Sekundezähler (0-59)
                      min = 0,        // Minutenzähler (0-59)
                      hour = 0,       // Stundenzähler (0-59)
                      day = 0;        // Tageszähler (0-255)



unsigned short        sek_flag = 0,
                      min_flag = 0,
                      hour_flag = 0,
                      day_flag = 0;
                      
// ADC
unsigned int i = 0, adc_read = 0;
char bin8_text[8] = {0}, adc_string[7] = {0};


// *****************************************************************************
// Delay 10 ms loop
// *****************************************************************************

void _delay_10ms(unsigned char loop) {

    unsigned short  i = 0;              // def variable

    for (i = 0; i < loop; i++ )
    {
      Delay_ms(10);
    }
}