#ifndef __GLOBAL_def                                                //gegen mehrfaches Einbinden schützen
#define __GLOBAL_def

// *****************************************************************************
// Konstantenzuordnung
// *****************************************************************************

 #define LED           RB4_bit            // LED
 #define BEEP          RC1_bit           // Signalgeber
 #define TICK          RB0_bit           // Ticks vom Zählrohr
 #define BEEP_EEPROM   0x01              //Adresse im EEPROM
 #define LED_EEPROM    0x02              //Adresse im EEPROM
 
 //PWM
 #define HV_MAX        15                // Maximaler Prozentwert für HV
 #define PWM_ON        DC1B0_bit = DC1B1_bit = 1; PWM_PW = ((EEPROM_read(HV_EEPROM))*249/100);        // PWM einschalten
 #define PWM_OFF       CCPR1L = DC1B0_bit = DC1B1_bit = 0;      // PWM auf 0%
 #define PWM_PW        CCPR1L            // Pulsweite des PWM-Signals
 
 //Encoder
 #define ENC_P         !RB1_bit           // Drehencoder Pushbutton
 #define ENC_A         !RB3_bit           // Drehencoder A
 #define ENC_B         !RB2_bit           // Drehencoder B

 // LCD
 #define DISP_BL       !LATC0_bit        // Output Latch Display Backlight
 #define DISP_BL_ON    LATC0_bit = 0;    // Output Latch Display Backlight
 #define DISP_BL_OFF   LATC0_bit = 1;    // Output Latch Display Backlight
 #define LCD_OUT       LATD
 #define EN            RE0_bit
 #define RS            RE1_bit
 
 // 20x4
 #define ROW1                 0x80   // Zuordnung Display
 #define ROW2                 0xC0   //Adresse zu Zeile
 #define CHECKMARK            0x00   //Adresse für Symbol
 #define CHECKBOX             0x03   //Adresse für Symbol
 #define ARROW_RIGHT          0x01   //Adresse für Symbol
 #define ARROW_LEFT           0x02   //Adresse für Symbol
 #define AVERAGE              0x04   //Adresse für Symbol
 #define RADIOACTIVITY        0x05   //Adresse für Symbol
 #define CURSOR_BL_ON         0x0D   //CURSOR Blinken ein
 #define CURSOR_BL_OFF        0x0C   //CURSOR Blinken aus

 
 //EEPROM
 #define HV_EEPROM            0x00   //Adresse im EEPROM

 // Zählrohr
 #define TICKS_HV_ON          INT0IE_bit = 1;  // Interrupt für Ticks aktivieren
 #define TICKS_HV_OFF         INT0IE_bit = 0;  // Interrupt für Ticks deaktivieren
 
// *****************************************************************************
// Funktionsprototypen
// *****************************************************************************
void _delay_10ms(unsigned char);
 
extern const code char *ADC_text[];
extern const code char *main_menu_text[];
extern const code char *set_menu_text[];
extern const code char *info_menu_text[];
extern const code char *HV_menu_text[];
extern const code char *run_text[];
extern const code char *run_text_ext[];
extern const code char *einheit_text[];
 
// *****************************************************************************
// Externe Variablen
// *****************************************************************************

// Encoder
extern signed short        enc_old,
                           enc_count,    // -120...120
                           enc_count_2,  // -60...60
                           enc_delta,    // -127...127
                           enc_p_hold,
                           enc_p_count;

// Zählrohr
extern unsigned long       ticks,         // counter for ticks (11 Stellen)
                           tks_sum_0eff,   // sum of all ticks per minute <=20
                           min_sum_0eff,   // sum of all minutes with valid ticks per minute <=20
                           sek_sum;

extern unsigned int      tks_min_0eff[2],          // Ticks/min
                                                   // [0]-temp counter
                                                   // [1]-disp counter
                           d_tks_min_0eff,         // Durschnitt Ticks/min
                           
                           
                           tks_per_sek,       // Ticks pro Sekunde
                           tks_per_sek_old,   // Ticks pro Sekunde alter wert
                           tks_per_min[2];

extern float                sv;                // Sievert

// Tick-Zähler
extern unsigned short      TMR3L_buff,        // Kopie des Zählers
                           TMR3H_buff;        // Kopie des Zählers

// Zählervars
extern unsigned short      counter,
                           taster_count,  // taster
                           bat_count;

// Flags
extern unsigned short      bat_flag,      // batterie
                           tick_flag,     // flag zur textaktualisierung bei neuen ticks
                           led_flag,
                           sound_flag,
                           sek_flag_ADC_refresh; // Sekundentakt für ADC-Wert Aktualisierung

// Zeit
extern unsigned short      xtel_sek,   // Teil-Sekundenzähler
                           sek,        // Sekundezähler (0-59)
                           min,        // Minutenzähler (0-59)
                           hour,       // Stundenzähler (0-59)
                           day;        // Tageszähler (0-255)

extern unsigned short      sek_flag,
                           min_flag,
                           hour_flag,
                           day_flag;

// ADC
extern unsigned int i, adc_read;
extern char bin8_text[8], adc_string[7];


#endif