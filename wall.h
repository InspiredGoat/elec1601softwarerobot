#ifndef WALL_H_INCLUDED
#define WALL_H_INCLUDED

#include "stdio.h"
#include "stdlib.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "types.h"

void wallSetPosition(struct Wall * wall, int x, int y, int width, int height);
void wallUpdate(struct Wall * wall);

void insertFirstWall(struct Wall_collection ** head, int key, struct Wall * wall);
void insertAndSetFirstWall(struct Wall_collection ** head, int key, int x, int y, int width, int height);
void updateAllWalls(struct Wall_collection * head);
#endif // WALL_H_INCLUDED

