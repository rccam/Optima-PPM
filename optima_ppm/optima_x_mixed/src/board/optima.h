/*
* Copyright (c) 2012 Scott Driessens
* Licensed under the MIT License
*/

#ifndef __OPTIMA_H
#define __OPTIMA_H

#include <stdint.h>
#include <core/types.h>

/* Define the optima receiver */
#define OPTIMA          9
#define PPM             1	//OPTIMA-1
#define SELECTABLE      1
#ifdef SELECTABLE
 #define SELECTPIN      2
#endif

/* number of PPM channels */
#define PPM_CHANNELS	9

/* Always 9 channels unless Hitec decides to change their protocol */
#define NUM_CHANNELS	9

extern pin_map outputs[NUM_CHANNELS];

#endif
