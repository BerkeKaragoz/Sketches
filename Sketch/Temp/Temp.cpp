/*********
CTIS164 - Template Source Program
----------
STUDENT : E. Berke KARAGÖZ / 21704206
SECTION : 2
HOMEWORK: Racing Game
----------
PROBLEMS: ¿Problemo?
----------
Features:
- Works at 60 fps. TIMER_PERIOD = 166.6 ( divided by multiplier which is 10) -> 1 seconds = 1000 ms and 1000(ms)/60(hertz)= 16.6
- 4 States
- Adjustable racer amount, with adjusting lane spacing.
- All racers have diffrent look.
- Using acceleration to be smooth
- Flashing lead icon.
- Pause / Continue
- Slow and fast motion with arrow keys
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

#define TIMER_PERIOD  166.6 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

#define LOAD 0
#define MENU 1
#define PLAY 2
#define END 3
#define RACER_AMOUNT 17 // <---- ADJUSTABLE
#define STARTING_TIME 180

/*
if (flag == true) {
timecount++;
printf("Time: %d\n", timecount);
pos.x = initpos.x + vel*timecount / 10.;
pos.y = initpos.y + -5 * powf(timecount / 10., 2);
}
*/



typedef struct {
	int x, y;
}point_t;

typedef struct {
	float trix, triy;
}tri_t;

typedef struct {
	point_t pos;
	int lenght;
	float angle;
}player_t;

typedef struct {
	point_t pos;
	int speed;
	tri_t tri;
};
						  //Glubuls
int posx[RACER_AMOUNT], posy[RACER_AMOUNT], winner, counter[RACER_AMOUNT] = { 0 }, multiplier = 10;
int gamestate = LOAD, timecount = 0, initial, i, ms, sec,
max = -5000; //
player_t player = { {-WINDOW_WIDTH / 2 + 50, 0 }, 30, 0, };
point_t onmove;

double accel[RACER_AMOUNT], speed[RACER_AMOUNT];
bool touch[RACER_AMOUNT],//if touched to the limit
flag = false,
timer = true;

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void print(int x, int y, char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void stickMan(int mposx, int mposy, float r, float g, float b, int i) {
	glColor3f(r, g, b);//color

	circle(mposx + 5 - ((mposx + 5000) % 50) / 6, mposy, 15); //head

	glLineWidth(10);
	glBegin(GL_LINES);

	//body
	glVertex2f(mposx, mposy);
	glVertex2f(mposx - 5, mposy - 75);

	//not-forearm 1
	glVertex2f(mposx, mposy - 10);
	glVertex2f(mposx + 15 - ((mposx + 5000) % 50), mposy - 50 + ((mposx + 5000) % 100) / 5);
	//forearm 1
	glVertex2f(mposx + 18 - ((mposx + 5000) % 50), mposy - 50 + ((mposx + 5000) % 100) / 5);
	glVertex2f(mposx + 25 - ((mposx + 5000) % 50), mposy - 10 - ((mposx + 5000) % 50) * 1.1);

	//not-forearm 2
	glVertex2f(mposx, mposy - 10);
	glVertex2f(mposx - 15 + ((mposx + 5000) % 50), mposy - 50 + ((mposx + 5000) % 50) / 10);
	//forearm 2
	glVertex2f(mposx - 18 + ((mposx + 5000) % 50), mposy - 45 + ((mposx + 5000) % 50) / 10);
	glVertex2f(mposx - 18 + ((mposx + 5000) % 50), mposy - 65 + ((mposx + 5000) % 50) * 1.1);

	//upperleg 1
	glVertex2f(mposx - 5, mposy - 75);
	glVertex2f(mposx - 15 - ((mposx - 5000) % 50), mposy - 112);
	//lowerleg 1
	glVertex2f(mposx - 15 - ((mposx - 5000) % 50), mposy - 112);
	glVertex2f(mposx - 40 - ((mposx - 5000) % 50), mposy - 145);

	//upperleg 2
	glVertex2f(mposx - 5, mposy - 75);
	glVertex2f(mposx + 30 + ((mposx - 5000) % 50), mposy - 100);
	//lowerleg 2
	glVertex2f(mposx + 30 + ((mposx - 5000) % 50), mposy - 100);
	glVertex2f(mposx + 5 + ((mposx - 5000) % 50), mposy - 130);

	glEnd();

	circle(mposx + 17 - ((mposx + 5000) % 50), mposy - 49 + ((mposx + 5000) % 100) / 5, 5); //arm joint1
	circle(mposx - 16 + ((mposx + 5000) % 50), mposy - 48 + ((mposx + 5000) % 50) / 10, 5); //arm joint2
	circle(mposx - 15 - ((mposx - 5000) % 50), mposy - 112, 5); //leg joint1
	circle(mposx + 30 + ((mposx - 5000) % 50), mposy - 100, 5); //leg joint2
	circle(mposx + 25 - ((mposx + 5000) % 50) / 1, mposy - 10 - ((mposx + 5000) % 50) * 1.1, 5); //hand 1
	circle(mposx - 18 + ((mposx + 5000) % 50) / 1, mposy - 65 + ((mposx + 5000) % 50) * 1.1, 5); //hand 2

	glColor3f(0.5, 0.5, 0.5);
	vprint(mposx - 5, mposy - 40, GLUT_BITMAP_HELVETICA_12, "%d", i + 1);
}
void stickmanReverse(int mposx, int mposy, float r, float g, float b, int i) {

	glColor3f(r, g, b);//color

	circle(mposx + 4 + ((mposx - 5000) % 50) / 6, mposy, 15); //head

	glLineWidth(10);
	glBegin(GL_LINES);

	//body
	glVertex2f(mposx, mposy);
	glVertex2f(mposx - 5, mposy - 75);

	//not-forearm 1
	glVertex2f(mposx, mposy - 10);
	glVertex2f(mposx - 15 + ((mposx + 5000) % 50), mposy - 50 + ((mposx + 5000) % 100) / 5);
	//forearm 1
	glVertex2f(mposx - 18 + ((mposx + 5000) % 50), mposy - 50 + ((mposx + 5000) % 100) / 5);
	glVertex2f(mposx - 25 + ((mposx + 5000) % 50), mposy - 10 - ((mposx + 5000) % 50) * 1.1);

	//not-forearm 2
	glVertex2f(mposx, mposy - 10);
	glVertex2f(mposx + 15 - ((mposx + 5000) % 50), mposy - 50 + ((mposx + 5000) % 50) / 10);
	//forearm 2
	glVertex2f(mposx + 18 - ((mposx + 5000) % 50), mposy - 45 + ((mposx + 5000) % 50) / 10);
	glVertex2f(mposx + 18 - ((mposx + 5000) % 50), mposy - 65 + ((mposx + 5000) % 50) * 1.1);

	//upperleg 1
	glVertex2f(mposx - 5, mposy - 75);
	glVertex2f(mposx + 15 - ((mposx + 5000) % 50), mposy - 112);
	//lowerleg 1
	glVertex2f(mposx + 15 - ((mposx + 5000) % 50), mposy - 112);
	glVertex2f(mposx + 40 - ((mposx + 5000) % 50), mposy - 145);

	//upperleg 2
	glVertex2f(mposx - 5, mposy - 75);
	glVertex2f(mposx - 30 + ((mposx + 5000) % 50), mposy - 100);
	//lowerleg 2
	glVertex2f(mposx - 30 + ((mposx + 5000) % 50), mposy - 100);
	glVertex2f(mposx - 5 + ((mposx + 5000) % 50), mposy - 130);

	glEnd();

	circle(mposx - 17 + ((mposx + 5000) % 50), mposy - 49 + ((mposx + 5000) % 100) / 5, 5); //arm joint1
	circle(mposx + 16 - ((mposx + 5000) % 50), mposy - 48 + ((mposx + 5000) % 50) / 10, 5); //arm joint2
	circle(mposx + 15 - ((mposx + 5000) % 50), mposy - 112, 5); //leg joint1
	circle(mposx - 30 + ((mposx + 5000) % 50), mposy - 100, 5); //leg joint2
	circle(mposx - 25 + ((mposx + 5000) % 50) / 1, mposy - 10 - ((mposx + 5000) % 50) * 1.1, 5); //hand 1
	circle(mposx + 18 - ((mposx + 5000) % 50) / 1, mposy - 65 + ((mposx + 5000) % 50) * 1.1, 5); //hand 2

	glColor3f(0.5, 0.5, 0.5);
	vprint(mposx - 5, mposy - 40, GLUT_BITMAP_HELVETICA_12, "%d", i + 1);
}
void dispPlayer() {

	glColor3f(0.2, 0.2, 0.2);
	glLineWidth(5);
	glBegin(GL_LINES);
	glVertex2f(player.pos.x, player.pos.y);
	glVertex2f(0, 0);
	glEnd();

	vprint(player.pos.x - 15, player.pos.y - 30, GLUT_BITMAP_9_BY_15, "%d", player.angle);
	//player.angle = atan(fabs(y - player.posy) / fabs(x - player.posx));
}
void dispBullet(){

}
void displayLoad() {
	glColor3f(0.2, 0.5, 0.2);
	vprint2(-125, 50, 0.6, "LOADING");

	glColor3f(0.1, 0.4, 0.1);
	glLineWidth(10);
	glBegin(GL_LINES);
	glVertex2f(-250, -25);
	glVertex2f((timecount / 13) * 50 - 200, -25);
	glEnd();

	//circle((timecount / 16) * 50 - 200, -50, 15);

	//printf("%c", 13);
	if (timecount > 150)
		gamestate = MENU;

}
void displayMenu() {
	stickMan(-250, 170, 0, 1, 0, 0);
	glColor3f(0.2, 0.5, 0.2);
	vprint2(-180, 120, 0.6, "STICKMAN");
	vprint2(-150, 40, 0.6, "SHOOTER");
	glColor3f(0, 0, 0);
	vprint(-123, -20, GLUT_BITMAP_HELVETICA_18, "Press \'Enter\' to start the game.");
	vprint(-100, -40, GLUT_BITMAP_HELVETICA_12, "Press Arrow Keys to change speed.");
	vprint(-90, -60, GLUT_BITMAP_HELVETICA_12, "Press \'Spacebar\' to pause.");
}
void displayPlay() {
	/*
	0: 0 0 0
	1: 1 0 0
	2: 0 1 0
	3: 0 0 1
	4: 1 1 0
	5: 1 0 1
	6: 0 1 1
	7: 1 1 1

	r: 2 5 6 8 +3 +1 +2
	g: 3 5 7 8 +2 +2 +1
	b: 4 6 7 8 +2 +1 +1
	*/

	//stickMan(*(posx+1), *(posy+1), 0, 1, 0);
	static bool r = 0, g = 0, b = 0;
	for (initial = RACER_AMOUNT - 1; initial >= 0; initial--) {
		switch (initial % 8) {
		case 0:break;
		case 4: g = 1;
		case 1: r = 1; break;
		case 5: r = 1;
		case 2: b = 1; break;
		case 7: r = 1;
		case 6: b = 1;
		case 3: g = 1; break;
		}
		if (*(touch + initial) == false)
			stickmanReverse(*(posx + initial), *(posy + initial), r, g, b, initial);
		r = g = b = 0;
	}

	dispPlayer();

	//DEBUG
	vprint(onmove.x - 15, onmove.y - 30, GLUT_BITMAP_9_BY_15, "Angle:%d X: %d Y: %d", player.angle, onmove.x, onmove.y);
	vprint(onmove.x - 15, onmove.y - 50, GLUT_BITMAP_9_BY_15, "pX: %d pY: %d", player.pos.x, player.pos.y);

	glColor3f(0.3, 0.3, 0.3);
	vprint(-80, 330, GLUT_BITMAP_HELVETICA_18, "%d Pixels Sprint Race", WINDOW_WIDTH * 2);
	vprint(0, 315, GLUT_BITMAP_HELVETICA_12, "by E. Berke KARAGOZ");
	ms = timecount - STARTING_TIME;

	if (timer == true)
		vprint(310, 340, GLUT_BITMAP_HELVETICA_10, "Press \'Spacebar\' to pause.");
	if (multiplier != 10)
		vprint(340, 305, GLUT_BITMAP_HELVETICA_10, "%0.1f Speed", multiplier / 10.);

	if (ms < 0) {
		sec = ms / 60;
		ms -= sec * 60;
		vprint(350, 320, GLUT_BITMAP_HELVETICA_18, "%0.2d", sec*-1);
		vprint(370, 320, GLUT_BITMAP_HELVETICA_12, ":%d", ms*-1);
	}
	else {
		sec = ms / 60;
		ms -= sec * 60;
		vprint(350, 320, GLUT_BITMAP_HELVETICA_18, "%0.2d", sec);
		vprint(370, 320, GLUT_BITMAP_HELVETICA_12, ":%d", ms);
		vprint(-350, 335, GLUT_BITMAP_HELVETICA_12, "Closest enemy HP:");
		vprint(-350, 315, GLUT_BITMAP_HELVETICA_18, "%0.3d", winner + 1);
	}

	if (timecount > STARTING_TIME - 25 && timecount < STARTING_TIME + 25) {
		glColor3f(0, 0, 0);
		vprint2(-60, 150, 0.5, "START!");
	}

	if (timer == false) {
		glColor3f(0.2, 0.2, 0.2);
		vprint2(-10, 150, 1, "ll");
		vprint(-65, 125, GLUT_BITMAP_HELVETICA_12, "Press Spacebar to continue.");
	}
}
void displayEnd() {
	glColor3f(0, 0, 0);
	vprint(-30, 0, GLUT_BITMAP_HELVETICA_18, "%3d Wins!", winner + 1);
	vprint(-70, -40, GLUT_BITMAP_HELVETICA_12, "Press \'Enter\' or \'F1\' to start again!");
}


//
// To display onto window using OpenGL commands
//
void display() {

	//
	// clear window to black
	//
	glClearColor(0.8, 0.8, 0.8, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	switch (gamestate) {
	case LOAD:displayLoad(); break;
	case MENU:displayMenu(); break;
	case PLAY:displayPlay(); break;
	case END:displayEnd(); break;
	}

	glutSwapBuffers();

}

void startGame() {
	//initializing speed, acceleration, position, states
	for (initial = 0; initial < RACER_AMOUNT; initial++) {
		*(speed + initial) = 0.2;
		*(accel + initial) = (rand() % 30 + 30) / 100.0;
		printf("No %d: Accel: %0.4f Speed: %0.4f\n", initial, *(accel + i), *(speed + i));
		*(posx + initial) = 600;
		*(posy + initial) = -200 + initial * 500 / RACER_AMOUNT;
		*(touch + initial) = false;
	}
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == 13 && gamestate == MENU) {
		gamestate = PLAY;
		timecount = 0;
	}
	if (key == 13 && gamestate == END) {
		startGame();
		gamestate = PLAY;
	}
	if (key == 32 && gamestate == PLAY) {
		timer = !timer;
		printf("Timer = %d\n", timer);
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP:
	case GLUT_KEY_RIGHT:
		if (gamestate == PLAY)
			multiplier++; break;
	case GLUT_KEY_DOWN:
	case GLUT_KEY_LEFT:
		if (gamestate == PLAY && multiplier > 1)
			multiplier--; break;


	}

	if (key == GLUT_KEY_F1 && gamestate == END) {
		startGame();
		gamestate = PLAY;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.
	if (stat == GLUT_DOWN)
		printf("X = %d  Y = %d\n", x - winWidth / 2, y - winHeight / 2);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.
	onmove.x = x - winWidth / 2;
	onmove.y = winHeight / 2 - y;
	player.angle = atan(fabs(onmove.y - player.pos.y) / fabs(onmove.x - player.pos.x));


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD / multiplier, onTimer, 0);
	if (timer == true) {

		timecount++;
		// Write your codes here.

		switch (gamestate) {
		case LOAD: break;
		case MENU: break;
		case PLAY:
			if (timecount >= STARTING_TIME)
				for (i = RACER_AMOUNT - 1; i >= 0; i--) {

					if (*(touch + i) == false) {
						if (winWidth % winWidth / 20.0 == 0.0 || 0.2 + +*(accel + i));
						*(speed + i) += (*(accel + i) / 40.0);
						*(posx + i) -= *(speed + i);
						if (max <= *(posx + i) && flag == false) {
							max = *(posx + i);
							winner = i;
						}
					}
					else {
						if (*(speed + i) <= +0.2 + *(accel + i))
							*(speed + i) += *(accel + i) / 40.0;
						*(posx + i) -= *(speed + i);
					}
					if (*(posx + i) <= -WINDOW_WIDTH / 2 - 50) {
						*(touch + i) = true;
						flag = true;
						//if (max < *(speed + i)) {
						//	max = *(speed + i);
						//	winner = i;
						//}
						*(accel + i) = (rand() % 30 + 30) / 100.0;
						printf("No %d: Accel: %0.4f aceel/20: %0.4f Speed: %0.4f\n", i, *(accel + i), *(accel + i) / 40.0, *(speed + i));

					}
					if (*(posx + i) <= -WINDOW_WIDTH / 2 - 50) {
						gamestate = END;
						timecount = 0;
						max = -5000;
						flag = false;
					}
				}
			break;
		case END: break;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void callTimer() {
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
}

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Stickman Shooter");

	srand(time(NULL));

	startGame();

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}