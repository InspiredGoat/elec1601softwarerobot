#include <SDL2/SDL.h>

float x_off = 0;
float y_off = 0;
SDL_Renderer* r = NULL;

void initRenderer(SDL_Renderer* renderer) {
	r = renderer;
}
void setOffset(float x, float y) {
	x_off = x;
	y_off = y;
}
void drawRect(float x, float y, float w, float h) {
    SDL_Rect rect = {x + x_off, y + y_off, w, h};
    SDL_RenderFillRect(r, &rect);
    SDL_RenderDrawRect(r, &rect);
}
void drawLine(float x1, float y1, float x2, float y2) {
    SDL_RenderDrawLine(r, x1 + x_off, y1 + y_off, x2 + x_off, y2 + y_off);
}
void setColor(unsigned char red, unsigned char g, unsigned char b, unsigned char a) {
    SDL_SetRenderDrawColor(r, red, g, b, a);
}

void drawCurrent(void) {
        SDL_RenderPresent(r);
}

