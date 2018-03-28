/*********
CTIS164 - Template Source Program
----------
STUDENT :Buğrahan Durukan - 21601657
SECTION :02
HOMEWORK:02
----------
PROBLEMS: When mouse is being moved on the screen it makes the program run faster
EXTRAS: Full auto bow
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  1300
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  35// Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int thickness = 80;
bool hit;
int  winWidth, winHeight; // current Window width and height
int i, y = -WINDOW_HEIGHT / 2 + thickness - 50 + 115, speed = 35;
int timeSw;
int timer = 0, mseconds, seconds, onmovex = 0, onmovey = 0, playerx = 0;
#define TARGET_AMOUNT 5
#define PLAY 0
#define END 1
#define LOAD 2
int targets[TARGET_AMOUNT], targetsx[TARGET_AMOUNT], gamestate = LOAD;
bool active[TARGET_AMOUNT] = { false }, fire;
int tarcnt;
int lastHitPoints;
int hitcnt;
int points;

//
// to draw circle, center at (x,y)

// radius r
//

void toTime() {
	mseconds = timer;
	seconds = mseconds / 60;
	mseconds -= seconds * 60;
}

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

void drawTarget(int positionx, int positiony, int size) {
	glColor3f(0.9, 0.9, 0.9);
	circle(positionx, positiony, size * 5);
	glColor3f(0.1, 0.1, 0.1);
	circle(positionx, positiony, size * 4);
	glColor3f(0, 0, 0.85);
	circle(positionx, positiony, size * 3);
	glColor3f(0.85, 0, 0);
	circle(positionx, positiony, size * 2);
	glColor3f(0.85, 0.85, 0);
	circle(positionx, positiony, size);
	if (positionx >= 600 && positionx <= 605)
		tarcnt++;


}

void drawBow(int thickness) {
	glColor3f(0.3, 0.2, 0.1);
	glBegin(GL_POLYGON);
	glVertex2f(playerx - 50, -winHeight / 2 + thickness - 50 + 50);
	glVertex2f(playerx - 35, -winHeight / 2 + thickness - 20 + 50);
	glVertex2f(playerx, -winHeight / 2 + thickness + 50);
	glVertex2f(playerx + 35, -winHeight / 2 + thickness - 20 + 50);
	glVertex2f(playerx + 50, -winHeight / 2 + thickness - 50 + 50);
	glEnd();
	glColor3f(0.7, 0.7, 0.7);
	glBegin(GL_POLYGON);
	glVertex2f(playerx - 45, -winHeight / 2 + thickness - 45 + 45);
	glVertex2f(playerx - 30, -winHeight / 2 + thickness - 15 + 45);
	glVertex2f(playerx, -winHeight / 2 + thickness + 45);
	glVertex2f(playerx + 30, -winHeight / 2 + thickness - 15 + 45);
	glVertex2f(playerx + 45, -winHeight / 2 + thickness - 45 + 45);
	glEnd();
}

void drawAreas(int thickness, double factor) {

	//hor
	glColor3f(0.1, 0.1, 0.1);
	glBegin(GL_QUAD_STRIP);
	glVertex2f(-winWidth / 2, -winHeight / 2 + thickness);
	glVertex2f(winWidth / 2, -winHeight / 2 + thickness);
	glVertex2f(-winWidth / 2, -winHeight / 2);
	glVertex2f(winWidth / 2, -winHeight / 2);
	glEnd();

	//seperator
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_LINES);
	glVertex2f(-winWidth / 2, -winHeight / 2 + thickness / factor);
	glVertex2f(winWidth / 2, -winHeight / 2 + thickness / factor);
	glEnd();

	if (timeSw == 1)
		toTime();
	//info
	glColor3f(0.8, 0.95, 0.8);
	//producer and product
	vprint(-50, -winHeight / 2 + thickness / (factor*factor), GLUT_BITMAP_HELVETICA_18, "Archer The Game by Bugrahan DURUKAN");
	//target counter amount of targets displayed on the screen
	vprint(200, -winHeight / 2 + thickness / (factor*factor) + 50, GLUT_BITMAP_HELVETICA_18, "Target Counter");
	vprint(200, -winHeight / 2 + thickness / (factor*factor) + 28, GLUT_BITMAP_HELVETICA_18, "%d", tarcnt);
	//Shots that hit the target
	vprint(0, -winHeight / 2 + thickness / (factor*factor) + 50, GLUT_BITMAP_HELVETICA_18, "Hit Counter");
	vprint(0, -winHeight / 2 + thickness / (factor*factor) + 28, GLUT_BITMAP_HELVETICA_18, "%d", hitcnt);
	//points gained from hits depending on specified area
	vprint(-200, -winHeight / 2 + thickness / (factor*factor) + 50, GLUT_BITMAP_HELVETICA_18, "Points Gained");
	vprint(-200, -winHeight / 2 + thickness / (factor*factor) + 28, GLUT_BITMAP_HELVETICA_18, "%d", points);
	//time counter
	vprint(-400, -winHeight / 2 + thickness / 2 + 17, GLUT_BITMAP_HELVETICA_18, "Time: %0.2d:%0.2d", seconds, mseconds);
	if (seconds == 20)
	{
		timeSw = 0;
		//points = 0;
		//hitcnt = 0;
		//tarcnt = 0;
		//lastHitPoints = 0;
		timer = 0;
	}
	//last point gained
	vprint(400, -winHeight / 2 + thickness / (factor*factor) + 50, GLUT_BITMAP_HELVETICA_18, "Last Point");
	vprint(400, -winHeight / 2 + thickness / (factor*factor) + 28, GLUT_BITMAP_HELVETICA_18, "%d", lastHitPoints);
	//instruction
	glColor3f(0, 0, 0);
	vprint(400, -winHeight / 2 + thickness / (factor*factor) + 150, GLUT_BITMAP_HELVETICA_18, "F1 to Stop the game");
	vprint(400, -winHeight / 2 + thickness / (factor*factor) + 100, GLUT_BITMAP_HELVETICA_18, "Space to (Re)Start the game");

}

void callTargets(int size) {
	int i, random;
	for (i = 0; i < TARGET_AMOUNT; i++)
	{
		srand(time(NULL));
		random = rand() % 5;
		if (active[i] == true)
			targetsx[i] -= 10;
		else if (timer % 20 == 10)
			active[random] = true;
		drawTarget(targetsx[i], targets[i], size);
		if (targetsx[i] < -winWidth / 2 - 50 || active[i] == false) {
			active[i] = false;
			targetsx[i] = WINDOW_WIDTH / 2 + 50;
		}
	}

}

void arrow(int thickness, int pos, int size) {
	glColor3f(0.5, 0.5, 0.5);

	//static bool hit;
	hit = false;
	//static int i, y = -winHeight / 2 + thickness - 50 + 115, speed = 0;
	do
	{
		//speed++;
		y += speed;
		glBegin(GL_TRIANGLES);
		glVertex2f(pos, y);
		glVertex2f(pos + 7, y - 15);
		glVertex2f(pos - 7, y - 15);
		glEnd();
		glBegin(GL_TRIANGLES);
		glVertex2f(pos, y - 75);
		glVertex2f(pos + 7, y - 90);
		glVertex2f(pos - 7, y - 90);
		glEnd();
		glColor3b(0.5, 0.5, 0.5);
		glLineWidth(4);
		glBegin(GL_LINES);
		glVertex2f(pos, y - 14);
		glVertex2f(pos, y - 80);
		glEnd();

		if (y >= WINDOW_HEIGHT / 2)
		{
			y = -winHeight / 2 + thickness - 50 + 115;
			//speed = 0;
			//fire = false;
		}

		for (i = 0; i < TARGET_AMOUNT; i++) {

			//if hits 10 bulls-eye
			if (targets[i] < y + size * 1 && targets[i] > size*-1 && targetsx[i] < pos + size * 1 && targetsx[i] > pos + size*-1 && hit == false)
			{
				hit = true;
				active[i] = false;
				//if (hit == true)
				hitcnt++;
				points = points + 10;
				lastHitPoints = 10;
			//	fire = false;

			}
			//if hits 6
			if (targets[i] < y + size * 1 && targets[i] > size*-1 && targetsx[i] < pos + size * 2 && targetsx[i] > pos + size*-2 && hit == false)
			{
				hit = true;
				active[i] = false;
				hitcnt++;
				points = points + 6;
				lastHitPoints = 6;
				fire = false;

			}
			//if hits 1
			if (targets[i] < y + size * 1 && targets[i] > size*-1 && targetsx[i] < pos + size * 4 && targetsx[i] > pos + size*-4 && hit == false)
			{
				hit = true;
				active[i] = false;
				hitcnt++;
				points++;
				lastHitPoints = 1;
				fire = false;
			}
			//glBegin(GL_POLYGON);
			//glVertex2f(, y + size * 1)
			hit = false;
		}

	} while (hit == true || fire == true);
}

//
// To display onto window using OpenGL commands
//
void display() {

	//
	// clear window to black
	//
	glClearColor(0.7, 0.7, 0.7, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	drawAreas(80, 3.5);
	drawBow(80);
	//drawTarget(onmovex, onmovey + 50, 8);

	callTargets(8);

	if (fire == true) {
		fire = false;

		arrow(80, playerx, 8);
	}

	glutSwapBuffers();

}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.

	if (key == 27)
		exit(0);
	if (key == 32)
	{
		fire = true;
		timeSw = 1;
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
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: playerx -= 8; left = true;
		if (playerx >= 600)
			playerx = 600;
		break;
	case GLUT_KEY_RIGHT:playerx += 8; right = true;
		if (playerx <= -600)
			playerx = -600;
		break;
	case GLUT_KEY_F1:
		timeSw = 0;
		points = 0;
		hitcnt = 0;
		tarcnt = 0;
		lastHitPoints = 0;
		timer = 0;
		break;
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
		printf("X = %d  Y = %d\n", x - winWidth / 2, winHeight / 2 - y);


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

	//x = x - winWidth / 2;
	//y = winHeight / 2 - y;

	//onmovex = x;
	//onmovey = y;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
	if (timeSw == 1)
		timer++;


	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Archer The Game By Bugrahan Durukan"); //Buğrahan DURUKAN
	int i;
	for (i = 0; i < TARGET_AMOUNT; i++) {
		targets[i] = WINDOW_HEIGHT / 8 + 80 * i - 100;
		targetsx[i] = WINDOW_WIDTH / 2 + 50;
		printf("T%d - X:%0.2d Y:%0.2d\n", i, targetsx[i], targets[i]);
	}

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