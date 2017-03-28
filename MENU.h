#ifndef __MENU_def                                                //gegen mehrfaches Einbinden schützen
#define __MENU_def

// *****************************************************************************
// Funktionsprototypen
// *****************************************************************************

void _hauptmenu(void);
void _start_GMZ(void);
void _show_message(unsigned short, signed short, unsigned short *);
void _settings(void);
signed short _vorabinfo( const char *text[], const signed short max_pos);
void _set_HV(void);
void _info_box(void);
void _show_arrow(unsigned short);
void _show_checkbox(unsigned short, unsigned short);
#endif