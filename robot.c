#include "settings.h"

#include "robot.h"
#include "renderer.h"
#include "maze.h"
/* #include "curvemaze.h" */

void setup_robot(struct Robot *robot){
#ifdef OLD_MAZE
	robot->x = OVERALL_WINDOW_WIDTH/2-50;
	robot->y = OVERALL_WINDOW_HEIGHT-50;
	robot->true_x = OVERALL_WINDOW_WIDTH/2-50;
	robot->true_y = OVERALL_WINDOW_HEIGHT-50;
#else
	robot->x = MAZE_START_X * WALL_WIDTH + WALL_WIDTH / 2 - ROBOT_WIDTH / 2;
	robot->y = MAZE_START_Y * WALL_WIDTH;
	robot->true_x = robot->x;
	robot->true_y = robot->y;
#endif

	robot->width = ROBOT_WIDTH;
	robot->height = ROBOT_HEIGHT;
	robot->direction = 0;
#ifdef OLD_MAZE
	robot->angle = 0;
#else
	robot->angle = 180;
#endif
	robot->currentSpeed = 0;
	robot->crashed = 0;
	robot->auto_mode = 0;
	robot->frames_alive = 0;

	printf("Press arrow keys to move manually, or enter to move automatically\n\n");
}
int robot_off_screen(struct Robot * robot) {
	if(robot->x < 0 || robot-> y < 0){
		return 0;
	}
	if(robot->x > OVERALL_WINDOW_WIDTH || robot->y > OVERALL_WINDOW_HEIGHT){
		return 0;
	}
	return 1;
}

int checkRobotHitWall(struct Robot * robot, struct Wall * wall) {

    int overlap = checkOverlap(robot->x,robot->width,robot->y,robot->height,
                 wall->x,wall->width,wall->y, wall->height);

    return overlap;
}

int checkRobotHitWalls(struct Robot * robot, struct Wall_collection * head) {
   struct Wall_collection *ptr = head;
   int hit = 0;

   while(ptr != NULL) {
      hit = (hit || checkRobotHitWall(robot, &ptr->wall));
      ptr = ptr->next;
   }
   return hit;

}

int checkRobotReachedEnd(struct Robot * robot, int x, int y, int width, int height){

    int overlap = checkOverlap(robot->x,robot->width,robot->y,robot->height,
                 x,width,y,height);

    return overlap;
}

void robotCrash(struct Robot * robot) {
    robot->currentSpeed = 0;
    if (!robot->crashed)
        printf("Ouchies!!!!!\n\nPress space to start again\n");
    robot->crashed = 1;
}

void robotSuccess(struct Robot * robot, int msec) {
    robot->currentSpeed = 0;
    if (!robot->crashed) {
        printf("Success!!!!!\n\n");
        printf("Time taken %d seconds %d milliseconds \n", msec/1000, msec%1000);
        printf("Press space to start again\n");
    }
    robot->crashed = 1;
}

int checkRobotSensor(int x, int y, int sensorSensitivityLength, struct Wall * wall)  {
    //viewing_region of sensor is a square of 2 pixels * chosen length of sensitivity
    int overlap = checkOverlap(x,2,y,sensorSensitivityLength,
                 wall->x,wall->width,wall->y, wall->height);

    return overlap;
}

int checkRobotSensorFrontRightAllWalls(struct Robot * robot, struct Wall_collection * head) {
    struct Wall_collection *ptr, *head_store;
    int i;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;

    int sensorSensitivityLength =  floor(SENSOR_VISION/5);

    head_store = head;
    robotCentreX = robot->x+ROBOT_WIDTH/2;
    robotCentreY = robot->y+ROBOT_HEIGHT/2;
    score = 0;
    int sensor_sensitivity =  floor(SENSOR_VISION/5);

    for (i = 0; i < 5; i++)
    {
        ptr = head_store;
        xDir = round(robotCentreX+5*sin((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*sin((robot->angle + 90)*PI/180));
        yDir = round(robotCentreY-5*cos((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*cos((robot->angle + 90)*PI/180));
        xTL = (int) xDir;
        yTL = (int) yDir;
        hit = 0;

        while(ptr != NULL) {
            hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
            ptr = ptr->next;
        }
        if (hit)
            score = i;
    }
    return score;
}

int checkRobotSensorFrontLeftAllWalls(struct Robot * robot, struct Wall_collection * head) {
    struct Wall_collection *ptr, *head_store;
    int i;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;
    int sensorSensitivityLength;

    head_store = head;
    robotCentreX = robot->x+ROBOT_WIDTH/2;
    robotCentreY = robot->y+ROBOT_HEIGHT/2;
    score = 0;
    sensorSensitivityLength =  floor(SENSOR_VISION/5);
    int sensor_sensitivity =  floor(SENSOR_VISION/5);

    for (i = 0; i < 5; i++)
    {
        ptr = head_store;
        xDir = round(robotCentreX-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*sin((robot->angle)*PI/180));
        yDir = round(robotCentreY+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*cos((robot->angle)*PI/180));
        xTL = (int) xDir;
        yTL = (int) yDir;
        hit = 0;

        while(ptr != NULL) {
            hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
            ptr = ptr->next;
        }
        if (hit)
            score = i;
    }
    return score;
}

void robotUpdate(struct Robot * robot){
    double xDir, yDir;

    int robotCentreX, robotCentreY, xTR, yTR, xTL, yTL, xBR, yBR, xBL, yBL;
    setColor(100, 100, 100, 255);

    robotCentreX = robot->x+ROBOT_WIDTH/2;
    robotCentreY = robot->y+ROBOT_HEIGHT/2;

    xDir = round(robotCentreX+(ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yDir = round(robotCentreY+(ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));
    xTR = (int) xDir;
    yTR = (int) yDir;

    xDir = round(robotCentreX+(ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yDir = round(robotCentreY+(ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));
    xBR = (int) xDir;
    yBR = (int) yDir;

    xDir = round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yDir = round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));
    xBL = (int) xDir;
    yBL = (int) yDir;

    xDir = round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yDir = round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));
    xTL = (int) xDir;
    yTL = (int) yDir;

    drawLine(xTR, yTR, xBR, yBR);
    drawLine(xBR, yBR, xBL, yBL);
    drawLine(xBL, yBL, xTL, yTL);
    drawLine(xTL, yTL, xTR, yTR);

    //Front Right Sensor
    int sensor_sensitivity =  floor(SENSOR_VISION/5);
    int i;
    for (i = 0; i < 5; i++)
    {
        xDir = round(robotCentreX+5*sin((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*sin((robot->angle + 90)*PI/180));
        yDir = round(robotCentreY-5*cos((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*cos((robot->angle + 90)*PI/180));
        xTL = (int) xDir;
        yTL = (int) yDir;

        setColor(80+(20*(5-i)), 80+(20*(5-i)), 80+(20*(5-i)), 255);
        drawRect(xTL, yTL, 2, sensor_sensitivity);
    }

    //Front Left Sensor
    for (i = 0; i < 5; i++)
    {
        xDir = round(robotCentreX-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*sin((robot->angle)*PI/180));
        yDir = round(robotCentreY+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*cos((robot->angle)*PI/180));
        xTL = (int) xDir;
        yTL = (int) yDir;

        setColor(80+(20*(5-i)), 80+(20*(5-i)), 80+(20*(5-i)), 255);
        drawRect(xTL, yTL, 2, sensor_sensitivity);
    }
}



void robotMotorMove(struct Robot * robot) {
    double x_offset, y_offset;
    switch(robot->direction){
        case UP :
            robot->currentSpeed += DEFAULT_SPEED_CHANGE;
            if (robot->currentSpeed > MAX_ROBOT_SPEED)
                robot->currentSpeed = MAX_ROBOT_SPEED;
            break;
        case DOWN :
            robot->currentSpeed -= DEFAULT_SPEED_CHANGE;
            if (robot->currentSpeed < -MAX_ROBOT_SPEED)
                robot->currentSpeed = -MAX_ROBOT_SPEED;
            break;
        case LEFT :
#ifdef NOISE_ENABLED
            robot->angle = (robot->angle+(rand()%2)-DEFAULT_ANGLE_CHANGE)%360;
#else
            robot->angle = (robot->angle-DEFAULT_ANGLE_CHANGE)%360;
#endif
            break;
        case RIGHT :
#ifdef NOISE_ENABLED
            robot->angle = (robot->angle+(rand()%2)+DEFAULT_ANGLE_CHANGE)%360;
#else
            robot->angle = (robot->angle+DEFAULT_ANGLE_CHANGE)%360;
#endif
            break;
    }
    robot->direction = 0;
    x_offset = (-robot->currentSpeed * sin(-robot->angle*PI/180));
    y_offset = (-robot->currentSpeed * cos(-robot->angle*PI/180));

    robot->true_x += x_offset;
    robot->true_y += y_offset;

    x_offset = round(robot->true_x);
    y_offset = round(robot->true_y);

    robot->x = (int) x_offset;
    robot->y = (int) y_offset;
}

void robotAutoMotorMove(struct Robot * robot, int front_left_sensor, int front_right_sensor) {
	static char dive_bombing = 0;
	static char dive_bomb_ticks = 0;
	static char turning_left = 0;
#define ROTATIONS_90DEG	6
#define ROTATIONS_180DEG 12
	static char left_turn_frames = ROTATIONS_90DEG;
	static int speed = 2;

	if (robot->frames_alive <= 2)
		robot->direction = UP;
	else {
		// if turning left, finish turning left
		if (left_turn_frames == 0 && turning_left) {
			turning_left = 0;
			left_turn_frames = ROTATIONS_90DEG;
			// speed back up
			robot->direction = UP;
			speed--;
		}
		else if (turning_left && left_turn_frames > 0) {
			robot->direction = LEFT;
			left_turn_frames --;
		}
		// if not turning left check sensors
		else if (front_left_sensor >= 1) {
			robot->direction = DOWN;
			speed++;
			turning_left = 1;
		}
		// emergency right
		else if (front_right_sensor >= 4) {
			robot->direction = LEFT;
		}
		else if (!dive_bombing && dive_bomb_ticks > 0) {
			robot->direction = LEFT;
			dive_bomb_ticks --;
		}
		else if (dive_bombing) {
			if (speed > 0) {
				speed --;
				robot->direction = UP;
			}
			if (front_right_sensor > 0) {
				dive_bombing = 0;
				robot->direction = LEFT;
				dive_bomb_ticks %= 2;
				dive_bomb_ticks --;
			}
			else {
				if (dive_bomb_ticks % 2 == 0)
					robot->direction = RIGHT;
				dive_bomb_ticks++;
			}
		}
		else if (front_right_sensor <= 1 && !dive_bombing) {
			if (speed > 0) {
				speed --;
				robot->direction = UP;
			}
			else {
				robot->direction = RIGHT;
				dive_bombing = 1;
			}
		}
		else if (speed > 0) {
			speed --;
			robot->direction = UP;
		}
	}

	robot->frames_alive++;
}

