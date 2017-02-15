/*********************************************************
**                                                      **
** PIC GMZ-2015                                         **
** LCD-Routinen                                         **
**                                                      **
** Autor: Holger Becht                                  **
** Compiler: mikroC PRO 6.5                             **
** Date: March 2015                                     **
** Clock: 8 MHz internal                            **
** PIC 18F46K22                                         **
**                                                      **
** Version 1                                            **
**                                                      **
*********************************************************/

#include "GLOBAL.h"
#include "LCD.h"



// *****************************************************************************
// New character for LCD
// *****************************************************************************

void _lcd_customchar(const char *symbol, char _pos)
{
    char _start, _end;
    _start = _pos*8;
    _end = _start+8;

    _lcd(0x40,'I');
    for (_start = 0; _start <= _end; _start++) _lcd(symbol[_start],'D');   //pos
}

// *****************************************************************************
// LCD löschen
// *****************************************************************************
void _lcd_clear(void)
{
    _lcd(0x01,'I');   // clear display
    Delay_ms(2);
}


// *****************************************************************************
// LCD: ein Instruction- oder Datenbyte im 8 Bit Modus schreiben
// *****************************************************************************
void _lcd(unsigned char d_byte, unsigned char mode)
{
    if (mode == 'I') RS = 0;
    else if (mode == 'D') RS = 1;
    else return;
    LCD_OUT = d_byte;  // Lege Daten auf LCD-Port
    EN = 1;
    Delay_us(50);
    EN = 0;
}



// *****************************************************************************
// Copy Text-Literals from ROM to LCD
// *****************************************************************************

void _romtolcd(const char *source) {

    unsigned short  i = 0;              // def variable

    while (source[i] != 0) {            // Gib alle Zeichen bis zur 0 aus
      _lcd(source[i],'D');
      i = i++;
    }
}

// *****************************************************************************
// Copy Text-Strings from Var to LCD
// *****************************************************************************

    void _vartolcd(char *source) {

    unsigned short  i = 0;              // def variable

    while (source[i] != 0) {            // Gib alle Zeichen bis zur 0 aus
      _lcd(source[i],'D');
      i = i++;
    }
}