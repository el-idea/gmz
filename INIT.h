#ifndef __INIT_def                                                //gegen mehrfaches Einbinden schützen
#define __INIT_def


// *****************************************************************************
// Funktionsprototypen
// *****************************************************************************

void _pic_init(void);
void _lcd_init(void);
void _pwm_init(void);
void _enc_init(void);
void _tick_init(void);
void _time_init(void);


#endif

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                        PORT A
                       --------
  * RA0       AN -> Eingangsspannung (Bat/Netz)
  * RA1
  * RA2       Vref-
  * RA3       Vref+
  * RA4
  * RA5
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                        PORT B
                       --------
  * RB0       ZÄHLROHR -> Ticks
  * RB1       ENCODER ->  Taster (Pull up)
  * RB2       ENCODER ->  B (Pull up)
  * RB3       ENCODER ->  A (Pull up)
  * RB4       LED
  * RB5
  * RB6       PICkit -> ICSPCLK
  * RB7       PICkit -> ICSPDAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                        PORT C
                       --------
  * RC0        DISPLAY -> Backlight
  * RC1        SIGNALGEBER
  * RC2        PWM -> HV-Kaskade
  * RC3
  * RC4
  * RC5
  * RC6
  * RC7
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                        PORT D
                       --------
  * RD0-RD7   LCD -> D0-D7
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                        PORT E
                       --------
  * RE0       LCD -> E
  * RE1       LCD -> R/W
  * RE2
  * RE3       PIC -> Reset
*********************************************************/