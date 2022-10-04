#ifndef UTIL_INCLUDE
#define UTIL_INCLUDE
#include "types.h"

extern void drawRect(float x, float y, float w, float h);
extern void drawLine(float x1, float y1, float x2, float y2);
extern void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
extern void setOffset(float x_off, float y_off);
extern void drawCurrent(void);

#endif

