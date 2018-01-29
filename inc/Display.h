/*
 * FB.h
 *
 *  Created on: 27 дек. 2017 г.
 *      Author: bekir
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

struct TMenu
{
struct TMenu* UP;
struct TMenu* DOWN;
struct TMenu* ENTER;
void (*menudisplay) (void);
void (*menuaction) (void);
};

typedef struct TMenu Menu;

void PreAsmDisp (void);
void PostAsmDisp (void);
void GIDisp (void);
void ShipModeDisp (void);
void ExitDisp (void);
//void PreAsmTestDisp(void);
void DownloadDisp (void);

void PreAsmAct (void);
void PostAsmAct (void);
void GIAct (void);
void ShipModeAct (void);
void ExitAct (void);
//void PreAsmTestAct(void);
void DownloadAct (void);

void MenuInit (void);

void* preasm_thread_func(void* thread_data);

#endif /* INC_DISPLAY_H_ */
