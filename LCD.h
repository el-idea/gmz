#ifndef __LCD_def                                                //gegen mehrfaches Einbinden schützen
#define __LCD_def

// *****************************************************************************
// Funktionsprototypen
// *****************************************************************************

void _lcd_customchar(const char *, char);
void _lcd_clear(void);
void _lcd(unsigned char, unsigned char);
void _romtolcd(const char *);
void _vartolcd(char *);

#endif