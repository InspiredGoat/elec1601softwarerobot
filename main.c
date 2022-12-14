#include <stdio.h>
#include <stdlib.h>

#include "renderer_internal.h"

#include "settings.h"

#include "maze.h"
#include "time.h"
#include "formulas.h"
#include "wall.h"
#include "robot.h"

int done = 0;

int main(int argc, char *argv[]) {
	SDL_Window *window;
	SDL_Renderer *renderer;


	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		return 1;
	}

	window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, -1, 0);
	initRenderer(renderer);

	struct Robot robot;
	struct Wall_collection *head = NULL;
	int front_left_sensor, front_right_sensor=0;
	clock_t start_time, end_time;
	int msec;

	// SETUP MAZE
	// You can create your own maze here. line of code is adding a wall.
	// You describe position of top left corner of wall (x, y), then width and height going down/to right
	// Relative positions are used (OVERALL_WINDOW_WIDTH and OVERALL_WINDOW_HEIGHT)
	// But you can use absolute positions. 10 is used as the width, but you can change this.

#ifdef OLD_MAZE
    insertAndSetFirstWall(&head, 1,  OVERALL_WINDOW_WIDTH/2, OVERALL_WINDOW_HEIGHT/2, 10, OVERALL_WINDOW_HEIGHT/2);
    insertAndSetFirstWall(&head, 2,  OVERALL_WINDOW_WIDTH/2-100, OVERALL_WINDOW_HEIGHT/2+100, 10, OVERALL_WINDOW_HEIGHT/2-100);
    insertAndSetFirstWall(&head, 3,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2+100, 150, 10);
    insertAndSetFirstWall(&head, 4,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2, 150, 10);
    insertAndSetFirstWall(&head, 5,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2-200, 10, 300);
    insertAndSetFirstWall(&head, 6,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2-100, 10, 100);
    insertAndSetFirstWall(&head, 7,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2-200, 450, 10);
    insertAndSetFirstWall(&head, 8,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2-100, 250, 10);
    insertAndSetFirstWall(&head, 9,  OVERALL_WINDOW_WIDTH/2+200, OVERALL_WINDOW_HEIGHT/2-200, 10, 300);
    insertAndSetFirstWall(&head, 10,  OVERALL_WINDOW_WIDTH/2+100, OVERALL_WINDOW_HEIGHT/2-100, 10, 300);
    insertAndSetFirstWall(&head, 11,  OVERALL_WINDOW_WIDTH/2+100, OVERALL_WINDOW_HEIGHT/2+200, OVERALL_WINDOW_WIDTH/2-100, 10);
    insertAndSetFirstWall(&head, 12,  OVERALL_WINDOW_WIDTH/2+200, OVERALL_WINDOW_HEIGHT/2+100, OVERALL_WINDOW_WIDTH/2-100, 10);
#else
	int key = 0;
	for (int x = 0; x < MAZE_WIDTH; x++)
		for (int y = 0; y < MAZE_HEIGHT; y++) {
			// each pixel contains 4 bytes (rgba) we're only interested in one of these bytes to determine if the maze is a wall
			int img_index = (y * MAZE_WIDTH + x) * 4;
			if (MAZE_DATA[img_index] == 0) {
				insertAndSetFirstWall(&head,
						key,
						x * WALL_WIDTH,
						y * WALL_WIDTH,
						WALL_WIDTH,
						WALL_WIDTH);
				key++;
			}
		}
#endif

	setup_robot(&robot);
	updateAllWalls(head);

	float smooth_x = robot.true_x;
	float smooth_y = robot.true_y;

	SDL_Event event;
	while(!done){
		SDL_SetRenderDrawColor(renderer, 250, 250, 250, 255);
		SDL_RenderClear(renderer);

		//Move robot based on user input commands/auto commands
		if (robot.auto_mode == 1)
			robotAutoMotorMove(&robot, front_left_sensor, front_right_sensor);
		robotMotorMove(&robot);

		//Check if robot reaches endpoint. and check sensor values
#ifdef OLD_MAZE
		if (checkRobotReachedEnd(&robot, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT/2+100, 10, 100)){
#else
		if (checkRobotReachedEnd(&robot, MAZE_END_X * WALL_WIDTH, MAZE_END_X * WALL_WIDTH, WALL_WIDTH, WALL_WIDTH)){
#endif
			end_time = clock();
			msec = (end_time-start_time) * 1000 / CLOCKS_PER_SEC;
			robotSuccess(&robot, msec);
		}
		else if(checkRobotHitWalls(&robot, head))
			robotCrash(&robot);
		//Otherwise compute sensor information
		else {
			front_left_sensor 	= checkRobotSensorFrontLeftAllWalls(&robot, head);
			front_right_sensor 	= checkRobotSensorFrontRightAllWalls(&robot, head);
		}
		robotUpdate(&robot);
		updateAllWalls(head);
#ifdef OLD_MAZE
#else
		smooth_x += (robot.true_x - smooth_x) * .4f;
		smooth_y += (robot.true_y - smooth_y) * .4f;
		setOffset(OVERALL_WINDOW_WIDTH / 2.f - robot.true_x, OVERALL_WINDOW_HEIGHT / 2.f - robot.true_y);
#endif

		// Check for user input
		SDL_RenderPresent(renderer);
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT){
				done = 1;
			}
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			if(state[SDL_SCANCODE_ESCAPE]){
				done = 1;
			}
			if(state[SDL_SCANCODE_UP] && robot.direction != DOWN){
				robot.direction = UP;
			}
			if(state[SDL_SCANCODE_DOWN] && robot.direction != UP){
				robot.direction = DOWN;
			}
			if(state[SDL_SCANCODE_LEFT] && robot.direction != RIGHT){
				robot.direction = LEFT;
			}
			if(state[SDL_SCANCODE_RIGHT] && robot.direction != LEFT){
				robot.direction = RIGHT;
			}
			if(state[SDL_SCANCODE_SPACE]){
				setup_robot(&robot);
			}
			if(state[SDL_SCANCODE_RETURN]){
				robot.auto_mode = 1;
				start_time = clock();
			}
		}

		SDL_Delay(SPEED);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	printf("DEAD\n");
}

