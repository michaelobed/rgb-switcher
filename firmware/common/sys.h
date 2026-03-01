//
//  sys.h
//  rgb-switcher
//
//  Created by Michael Obed on 03/01/2025.
//  
//  Copyright © 2025 Michael Obed.

#ifndef sys_h
#define sys_h

#include <avr/boot.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>

#define FALSE               (0 == 1)
#define TRUE                (1 == 1)
#define bool                _Bool

void SysReset(void);

#endif