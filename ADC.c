/*********************************************************
**                                                      **
** PIC GMZ-2015                                         **
** ADC-Routinen                                         **
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


// *****************************************************************************
// ADC - eingelesener Wert in Integer umwandeln
// *****************************************************************************
void _ADCToInt(int *adc_val)
{
 //Erzeuge 16bit-Variable
 *adc_val = 0;
 *adc_val = ADRESH;
 *adc_val = *adc_val << 8;
 *adc_val = *adc_val | ADRESL;

 //adc_read = ((adc_read << 2) + adc_read);     // *5
 *adc_val = ((*adc_val << 1));     // *2
}

// *****************************************************************************
// ADC - eingelesener Wert in String mit Trennpunkt und V (Volt) umwandeln
// *****************************************************************************
void _ADCintToVolt(int *adc_val, char *adc_str)
{
 U_IntToStr(*adc_val, adc_str, 4);          // Convertiere in Char

 if(*adc_str == '0') *adc_str =' ';  // Entferne führende 0
 *(adc_str+6) = '\0';
 *(adc_str+5) = 'V';
 *(adc_str+4) = *(adc_str+3);
 *(adc_str+3) = *(adc_str+2);
 *(adc_str+2) = '.';

}