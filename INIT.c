/*********************************************************
**                                                      **
** PIC GMZ-2015                                         **
** INIT-Routinen                                        **
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
#include "INIT.h"
#include "LCD.h"

// max. 8 (64bytes) custom characters  (Controller ST7066U)
const char lcd_character[] = { 0, 0, 1, 2,20, 8, 0,31,        // LCD char: "check mark"     0
                              16,24,28,30,30,28,24,16,   // LCD char: "arrow to right" 1
                               1, 3, 7,15,15, 7, 3, 1,         // LCD char: "arrow to left"  2
                               0,31,17,17,17,17,31, 0,      // LCD char: "box"  3
                               0,13,26,21,11,22, 0, 0,     // LCD char: "Durchschnitt"    4
                               0,27,27, 4,14,14, 0, 0};     // LCD char: "radioactivity"    5
// *****************************************************************************
// Initialisierung
// *****************************************************************************

void _pic_init(void) {

 // OSCCON-Register
 OSCCON = 0;
 OSCCON2 = 0;
 OSCTUNE = 0b00111110;
 
 IRCF2_bit = 1;
 IRCF1_bit = 1;
 IRCF0_bit = 0;                           // Int. Clock = 8MHZ

 SCS1_bit = 1;                             // Use Internal Clock
 SCS0_bit = 0;                             // Use Internal Clock


while (HFIOFS_bit == 0) {};                 // Wait for stable clock


// Interrupts ausschalten
  // IOCB    INTERRUPT-ON-CHANGE PORTB REGISTER
  IOCB = 0x00;                            // Int. on change Disable

  // PIR
  PIR1 = 0x00;                        //Disable all PIR
  PIR2 = 0x00;
  PIR3 = 0x00;
  PIR4 = 0x00;
  PIR5 = 0x00;

  // PIE
  PIE1 = 0x00;
  PIE2 = 0x00;
  PIE3 = 0x00;
  PIE4 = 0x00;
  PIE5 = 0x00;

  // IPR
  IPR1 = 0x00;
  IPR2 = 0x00;
  IPR3 = 0x00;
  IPR4 = 0x00;
  IPR5 = 0x00;

   // INTCON
  INTCON = 0x00;                      // Clear
  INTCON2 = 0x00;                     // Clear INTEDG -> falling edges
  NOT_RBPU_bit = 1;                   // disable Pull-Ups PORTB
  INTCON3 = 0x00;                     // Clear

// WEAK PULL UP
   WPUB = 0;                      // Disable Pull Ups

// PCON
  // PowerControl Status Bits - not relevant

// EEprom
  // EECON1

//  SSPCON - I2C, SPI-Mode
  SSPEN_bit = 0;                                // Disable Bus-Modes and use I/O

// RCSTA
  SPEN_bit = 0;                                 // Serial Port disabled


// ANALOG-TO-DIGITAL CONVERTER (ADC) MODULE

   // VREFCON0
   VREFCON0 = 0b10010000;
   while (FVRST_bit == 0) {};                 // Wait for stable Reference

   // ANSELA
   //0: Enable AN1
   //2: Enable AN2 (Vref-)
   //3: Enable AN3 (Vref+)
   ANSELA = 0b00001101;                // Configure PORTB pins as AN and Dig. I/O
   ANSELB = 0x00;                     // Configure PORTB pins as digital I/O
   ANSELC = 0x00;                     // Configure PORTB pins as digital I/O
   ANSELD = 0x00;                     // Configure PORTB pins as digital I/O
   ANSELE = 0x00;                     // Configure PORTB pins as digital I/O


  // ADC Control: ADCON2
  // 7: ADFM: A/D Conversion Result Format Select bit: Right
  // 6: not implemented
  // 5-3: ACQT: 8TAD - AD Acquisition Time
  // 2-0: ADCS: Fosc/2 = 500ns/Tad = 6us Conversion-Time
  ADCON2 = 0b10100000;

  // ADC Control: ADCON1
  // 7: TRIGSEL: Not used
  // 6-4: not implemented
  // 3-2: VCFG1: Voltage Reference bit: Vref-
  // 1-0: VCFG2: Voltage Reference bit: Vref+

   ADCON1 = 0b00001000;

  //ADCON1 = 0b00000101;


  // ADC Control: ADCON0
  // 6-2: Select Channel 0
  // 1: A/D conversion not in progress
  // 0: ADC Enable
  ADCON0 = 0b00000001;


// PORTS A-E
 //Clear Ports and Latches
 PORTA = 0x00;
 PORTB = 0x00;
 PORTC = 0x01;  // Display aus
 PORTD = 0x00;
 PORTE = 0x00;
 
 LATA = 0x00;
 LATB = 0x00;
 LATC = 0x01;  // Display aus
 LATD = 0x00;
 LATE = 0x00;

 // Set In/Out
 TRISA = 0xFF;                   // All In
 TRISB = 0b11101111;
 TRISC = 0b11111000;
 TRISD = 0x00;                   //Display Dataport
 TRISE = 0b11111100;


// PMD0: PERIPHERAL MODULE DISABLE REGISTER 0
 UART2MD_bit = 1;   // Module is disabled, Clock Source is disconnected...
 UART1MD_bit = 1;   // Module is disabled, Clock Source is disconnected...
 TMR6MD_bit = 0;    // Module is enabled, Clock Source is connected...
 TMR5MD_bit = 0;    // Module is enabled, Clock Source is connected...
 TMR4MD_bit = 0;    // Module is enabled, Clock Source is connected...
 TMR3MD_bit = 0;    // Module is enabled, Clock Source is connected...
 TMR2MD_bit = 0;    // Module is enabled, Clock Source is connected...
 TMR1MD_bit = 1;    // Module is disabled, Clock Source is disconnected...
 
// PMD1: PERIPHERAL MODULE DISABLE REGISTER 1
 MSSP2MD_bit = 1;   // Module is disabled, Clock Source is disconnected...
 MSSP1MD_bit = 1;   // Module is disabled, Clock Source is disconnected...
 CCP5MD_bit = 0;    // Module is enabled, Clock Source is connected...
 CCP4MD_bit = 1;    // Module is disabled, Clock Source is disconnected...
 CCP3MD_bit = 1;    // Module is disabled, Clock Source is disconnected...
 CCP2MD_bit = 1;    // Module is disabled, Clock Source is disconnected...
 CCP1MD_bit = 0;    // Module is enabled, Clock Source is connected...

// PMD2: PERIPHERAL MODULE DISABLE REGISTER 2
 CTMUMD_bit = 1;   // Module is disabled, Clock Source is disconnected...
 CMP2MD_bit = 1;   // Module is disabled, Clock Source is disconnected...
 CMP1MD_bit = 1;   // Module is disabled, Clock Source is disconnected...
 ADCMD_bit = 0;    // Module is enabled, Clock Source is connected...



 sound_flag = EEPROM_READ(BEEP_EEPROM);  // Lese Beeper Flag
 LED_flag = EEPROM_READ(LED_EEPROM);     // Lese LED_flag
 
 // falls EEPROM nicht oder fehlerhaft beschrieben
 if (sound_flag > 1)
 {
  sound_flag = 1;
  EEPROM_WRITE(BEEP_EEPROM, sound_flag);
 }
 
  if (LED_flag > 1)
 {
  LED_flag = 1;
  EEPROM_WRITE(LED_EEPROM, LED_flag);
 }
}



// *****************************************************************************
// LCD Init(8-Bit)
// *****************************************************************************

void _lcd_init(void)
{
    EN = 0;
    _delay_10ms(10);
    _lcd(0x30,'I');  // Init
    _delay_10ms(3);
    _lcd(0x30,'I');  // Init
    _delay_10ms(1);
    _lcd(0x30,'I');  // Init
    _delay_10ms(1);
    _lcd(0x38,'I');   // 8 Bit / 2 Zeilen
    _lcd(0x10,'I');   // Cursor setzen
    _lcd(0x0C,'I');   // Disp Ein / Cursor Ein;
    _lcd(0x06,'I');   // Entry mode set;
    _lcd_customchar(lcd_character, 0x05);          // create new characters
    _lcd_clear();

}



// *****************************************************************************
// PWM Init  -> TIMER 2
// *****************************************************************************

void _pwm_init(void)
{
  
  // lese gespeicherten HV-Wert und setze ih auf 0, falls der PIC neu ist
  if (EEPROM_read(HV_EEPROM) > HV_MAX)
  {
   EEPROM_write(HV_EEPROM, 0);           //setze default Wert
   _delay_10ms(3);                  // warte 30 ms eeprom schreiben
  }
  
  // 1. PWM ausschalten
  PWM_OFF;
  
  // 2. For PWM use Timer 2
  C1TSEL1_bit = 0;
  C1TSEL0_bit = 0;
  
  // 3. Frequenz auf 500 Hz einstellen
  PR2 = 249; // 1/((249+1) * 4 * (1/8e6) * 16) = 500 Hz
  
  // 4. ECCP Control: PWM - Modefür PORTB.B0
  CCP1M2_bit = 1;
  CCP1M3_bit = 1;
  
  // 5. Impulsbreite auf 0% einstellen
  //CCPR1L = DC1B0_bit = DC1B1_bit = 0;
  //DC1B0_bit = 0;
  //DC1B1_bit = 0;
  
   // 6.IPR2 : Erster PWM-Puls wird unterdrückt
   TMR2MD_bit = 0; // Power for Timer 2 is enabled
   TMR2IF_bit = 0; // Clear Flag
   
  // Timer2 einstellen
   //Vorteiler 16:1
   T2CKPS1_bit = 1;
   T2CKPS0_bit = 1;
   // Nachteiler auf 1:2
   T2OUTPS3_bit = 0;
   T2OUTPS2_bit = 0;
   T2OUTPS1_bit = 0;
   T2OUTPS0_bit = 1;
  
   // Timer einschalten
   TMR2 = 0;
   TMR2ON_bit = 1;
}


// *****************************************************************************
// Encoder Init -> TIMER 4
// *****************************************************************************

void _enc_init( void )
{
  
  // Drehencoder
  signed short enc_new;
  
  enc_new = 0;
  if( ENC_A ) enc_new = 3;
  if( ENC_B ) enc_new ^= 1;            // convert gray to binary
  enc_old = enc_new;                   // power on state
  enc_delta = 0;
  
  //-----------------------------------------------------
  // TIMER 4 - Encoder read 2ms @ 8 MHz
   // IPR4
   TMR4MD_bit = 0; // Power for Timer 4 is enabled
   TMR4IP_bit = 0; // Low priority
   TMR4IF_bit = 0; // Clear Flag
   TMR4IE_bit = 1; // ISR Enable

   // 6-3: POSTSCALER 1:1
   // 2: TMR4ON -> OFF
   // 1-0: Prescaler 1:16
   PR4 = 250;       // Comparator-Wert
   TMR4 = 0x00;     // set default
   T4CON = 0b00000011;
}


// *****************************************************************************
// Ticks (Zählrohr) Init -> TIMER 3
// *****************************************************************************

void _tick_init(void)
{
  // Init Interrupt RB0 for Ticks in HV Menue

  // Zählrohr Ticks
  INTEDG0_bit = 0;                            // negative Flanke
  INT0IF_bit = 0;                         // Clear Flag
  INT0IE_bit = 0;                         // INT1 ausschalten

 //-----------------------------------------------------
 
  // Init TIMER 6 - Beeper 10ms @ 8 MHz
   // IPR5
   TMR6MD_bit = 0; // Power for Timer 6 is enabled
   TMR6IP_bit = 1; // High priority
   TMR6IF_bit = 0; // Clear Flag
   TMR6IE_bit = 1; // ISR Enable

   // 6-3: POSTSCALER 1:16
   // 2: TMR6ON -> OFF
   // 1-0: Prescaler 1:16
   PR6 = 250;       // Comparator-Wert
   TMR6 = 0x00;     // set default
   T6CON = 0b00100011;  // TMR6 ist aus

 //-----------------------------------------------------

// Init Timer 3 as counter
  
  // T3CON
  //*******
  // External Clocking on T3CKI Pin (RB5)
  TMR3CS1_bit = 1;
  TMR3CS0_bit = 0;
  T3SOSCEN_bit = 0;
  
  // Input Clock Prescale Select bits
  T3CKPS1_bit = 0;
  T3CKPS0_bit = 0;

  NOT_T3SYNC_bit = 0; // Sync external clock
  
  T3RD16_bit = 1; // enable 16 bit read/write
  
  TMR3ON_bit = 0; // stop Counter
   
  // T3GCON
  //*******
  
  TMR3GE_bit = 0; // counts regardless of Timer 3 gate function
  T3GPOL_bit = 0; // counts when gate is low
  T3GTM_bit = 0; //  Gate Toggle mode is disabled + toggle flip-flop is cleared
  T3GSPM_bit = 0; // gate Single-Pulse mode is disabled
  T3GSS1_bit = 0; // gate pin
  T3GSS1_bit = 0; // gate pin
}




// *****************************************************************************
// Timebase (Sekundentakt) Init -> TIMER 5
// *****************************************************************************

void _time_init(void)
{
   // Compare Mode
  C5TSEL1_bit = 1;      // CCP5 -> select Timer 5
  C5TSEL0_bit = 0;
 
  CCP5CON = 0b00001011; // Compare Mode Special Event Trigger
                        // IF will set, Timer 5 will reset
  
  CCPR5H = 0xC3;        // Compare auf 50000 x 5(2500000 = 1 Sek)
  CCPR5L = 0x50;
 
  CCP5IP_bit = 0;                         // low priority
  CCP5IF_bit = 0;                         // Clear Flag
  CCP5IE_bit = 1;                         // Interrupt aktivieren

 //-----------------------------------------------------
  // TIMER 5 - Für Compare Modus
   // IPR5
   TMR5MD_bit = 0; // Power for Timer 5 is enabled
   TMR5IP_bit = 0; // Low priority
   TMR5IF_bit = 0; // Clear Flag
   TMR5IE_bit = 0; // ISR Disable

   // T5GCON
   TMR5GE_bit = 0; // not gate controlled
   // T5SOSCEN_bit = x -> keine Funktion in dieser Einstellung
  
   // T5CON
   TMR5CS1_bit = 0; // Fosc/4
   TMR5CS0_bit = 0;
   T5CKPS1_bit = 1; // 1:8
   T5CKPS0_bit = 1;
   T5SOSCEN_bit = 0; // Sec. osc. disabled
   NOT_T5SYNC_bit = 0;  // Sync. Clock
   T5RD16_bit = 0; //8bit operation
   TMR5ON_bit = 0;   //TMR aus

   TMR5H = 0; // TMR Register auf 0
   TMR5L = 0;
}