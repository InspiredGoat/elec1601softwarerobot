#include "settings.h"
#include "types.h"
#include "renderer.h"
#include "wall.h"

void wallSetPosition(struct Wall * wall, int x, int y, int width, int height) {
    wall->x = x;
    wall->y = y;
    wall->width = width;
    wall->height = height;
    wall->og_w = width;
    wall->og_h = height;
}

void wallUpdate(struct Wall * wall){
	drawRect(wall->x, wall->y, wall->width, wall->height);
}

//insert link at the first location
void insertFirstWall(struct Wall_collection ** head, int key, struct Wall * wall) {
   //create a link
   struct Wall_collection *link = (struct Wall_collection*) malloc(sizeof(struct Wall_collection));

   link->key = key;
   link->wall = *wall;

   //point it to old first node
   link->next = *head;

   //point first to new first node
   *head = link;
}

void insertAndSetFirstWall(struct Wall_collection ** head, int key, int x, int y, int width, int height){
   //create a link
   struct Wall *wall = (struct Wall*) malloc(sizeof(struct Wall));
   wallSetPosition(wall, x, y, width, height);
   insertFirstWall(head, key, wall);

}

void updateAllWalls(struct Wall_collection * head) {
	struct Wall_collection *ptr = head;

	//start from the beginning
	while(ptr != NULL) {
		setColor(0, 0, 0, 255);
#ifdef CRAZYWALLS
		if (ptr->wall.height > ptr->wall.width) {
			ptr->wall.width = ptr->wall.og_w * (sin(SDL_GetTicks() / 1100.f + ptr->wall.x * ptr->wall.x) + 2) * 2.9f;
		}
		else {
			ptr->wall.height = ptr->wall.og_h * (cos(SDL_GetTicks() / 1100.f + ptr->wall.x * ptr->wall.y) + 2) * 2.9f;
		}
#endif
		wallUpdate(&ptr->wall);
		ptr = ptr->next;
	}
}

