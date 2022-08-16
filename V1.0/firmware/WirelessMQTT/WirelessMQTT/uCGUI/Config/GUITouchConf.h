/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUITouch.Conf.h
Purpose     : Configures touch screen module
----------------------------------------------------------------------
*/


#ifndef GUITOUCH_CONF_H
#define GUITOUCH_CONF_H
#include "Touch_s.h"

#define GUI_TOUCH_AD_LEFT    0//370   
#define GUI_TOUCH_AD_RIGHT   4095//3830
#define GUI_TOUCH_AD_TOP     0//250
#define GUI_TOUCH_AD_BOTTOM  4095//3900

#define GUI_TOUCH_SWAP_XY    0
#define GUI_TOUCH_MIRROR_X   0
#define GUI_TOUCH_MIRROR_Y   0

void GUI_TOUCH_GetxyMinMax(void);

#endif /* GUITOUCH_CONF_H */
