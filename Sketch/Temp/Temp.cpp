/*********
CTIS164 - Template Source Program
----------
STUDENT : E. Berke KARAGOZ	
SECTION : 2
HOMEWORK: 3
----------
PROBLEMS: If your program does not function correctly,
explain here which parts are not running.

Features:
-3D animation
	Eliptic orbit
	Targets becomes brighter when they get closer to the lightsource
	3D shading
	Beams get closer or further
-Colors are diffrent from each other everytime
-Trail effect
-Beam shooting
-Health (20 initially)
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

#define TARGET_AMOUNT 3
#define MAX 20 // Bullet array
#define TARGET_HP 20

typedef struct {
	float x, y;
}trigo_t;

typedef struct {
	int x, y;
}point_t;

typedef struct {
	point_t pos;
	int lenght, width;
	double angle;
}player_t;

typedef struct {
	point_t pos;
	int hp;
	float angle, r, g, b, radius, speed;
	bool hit;
} target_t;

typedef struct {
	point_t pos;
	float angle, dist;
	bool flag;
}projectile_t;

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false,
shoot, timeflag, flag = 0;
int winWidth, winHeight; // current Window width and height
int timecount = 0, i, bulletcount = MAX, reltime = 0;
double colorsum = 0;
player_t player;
target_t targets[3];
trigo_t tri;
projectile_t blt[MAX]; // bullets

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

void refCircle(int x, int y, int r, int tangle, double red, double g, double b)
{
#define PI 3.1415
	float angle;
	double clr;

	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		clr = (i / -100.0 + 1 - 0.6) / 1.5;
		glColor4f(clr + red / 4., clr + g / 4., clr + b / 4., 1);

		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();

	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		clr = (i / -40.0 + 1 - 0.6) / 2.;
		glColor4f(fabs(clr) + red / 2., fabs(clr) + g / 2., fabs(clr) + b / 2., (fabs(1 - fabs(180 - fabs(tangle - 45)) / 180.0)) / 1.25);

		angle = 2 * PI*i / -100;
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

int findAvailableFire() {
	for (int i = 0; i < MAX; i++) {
		if ((*(blt + i)).flag == false)
			return i;
	}
	return -1;
}

void dispProj()
{
	for (int i = 0; i < MAX; i++) {
		if ((*(blt + i)).flag) {
			glColor3f(0, 1, 0);
			circle((*(blt + i)).pos.x, (*(blt + i)).pos.y, (*(blt + i)).dist);
		}
	}
}

void initialize() {
	timeflag = 0;
	flag = 0;
	player.lenght = 20;
	player.pos.x = 0;
	player.pos.y = 0;
	srand(time(NULL));
	int r = 0, g = 0, b = 0;
	for (i = 0; i < TARGET_AMOUNT; i++) {
		(*(targets + i)).angle = rand() % 360 - 180;
		(*(targets + i)).speed = rand() % 20 / 10.0 - 1;
		while (colorsum == r + g + b || r + g + b == 0 || r + g + b == 3) {
			r = rand() % 2;
			g = rand() % 2;
			b = rand() % 2;
		}
		colorsum = r + g + b;
		(*(targets + i)).r = r;
		(*(targets + i)).g = g;
		(*(targets + i)).b = b;
		(*(targets + i)).hp = TARGET_HP;
		(*(targets + i)).hit = false;
		colorsum = r + g + b;
		while (fabs((*(targets + i)).speed) < 0.5)
			(*(targets + i)).speed = rand() % 20 / 10.0 - 1;
	}
}

void dispPlayer() {

	tri.x = cos(player.angle * D2R);
	tri.y = sin(player.angle * D2R);

	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(player.pos.x + player.lenght*tri.x - 10, player.pos.y + player.lenght*tri.y + 15);

	glVertex2f(player.pos.x + 15 - player.lenght*tri.x - 10, player.pos.y + 15);

	glVertex2f(player.pos.x - 10 - player.lenght*tri.x - 10, player.pos.y - 10);

	glVertex2f(player.pos.x + player.lenght*tri.x / 2 + 15, player.pos.y + player.lenght*tri.y / 2 - 15);
	glEnd();

	glColor3f(0.5, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(player.pos.x + player.lenght*tri.x - 10, player.pos.y + player.lenght*tri.y + 15);

	glVertex2f(player.pos.x + 15 - player.lenght*tri.x - 10, player.pos.y + 15);


	glVertex2f(player.pos.x + player.lenght*tri.x / 2 + 15, player.pos.y + player.lenght*tri.y / 2 - 15);
	glEnd();


	//glBegin(GL_LINES);
	//glVertex2f(player.pos.x, player.pos.y);
	//glVertex2f(player.pos.x + tri.x*player.lenght, player.pos.y + tri.y*player.lenght);
	//glEnd();
}

void elipseWire(int x, int y, int xfac, int yfac, int tang, double speed)
{
#define PI 3.1415
	float angle;
	double clr;
	glBegin(GL_LINE_LOOP);
	int i;
	if (speed > 0) {
		for (i = 100; i >= 0; i--)
		{
			angle = 2 * PI*i / 100;
			clr = i / -100.0 + 1 - 0.6;
			if (clr > 0) {
				clr = i / -40.0 + 1;
				glVertex2f(x - 1 + xfac*cos(angle + tang / 57.5 + 0.15), y - 1 + yfac*sin(angle + tang / 57.5 + 0.17));
			}
			glColor3f(clr, clr, clr);
			glVertex2f(x + xfac*cos(angle + tang / 57.5), y + yfac*sin(angle + tang / 57.5));
		}
	}
	else {
		for (i = 100; i >= 0; i--)
		{
			angle = 2 * PI*i / 100;
			clr = i / 100.0 - 0.6;
			if (clr > 0) {
				clr = i / 160.0;
				glVertex2f(x + 1 + xfac*cos(angle + tang / 57.5 - 0.15), y + 1 + yfac*sin(angle + tang / 57.5 - 0.17));
			}
			glColor3f(clr, clr, clr);
			glVertex2f(x + xfac*cos(angle + tang / 57.5), y + yfac*sin(angle + tang / 57.5));

		}
	}

	glEnd();
}

void dispOrbit() {
	glColor3f(0.3, 0.3, 0.3);

	for (i = 0; i < TARGET_AMOUNT; i++) {
		if (targets[i].hit == false)
			elipseWire(0, 0, WINDOW_WIDTH / 8 + 50 + i * WINDOW_WIDTH / 8, WINDOW_HEIGHT / 8 + i * WINDOW_HEIGHT / 8, (int)(*(targets + i)).angle, (*(targets + i)).speed);
	}
}

void dispTargets() {
	flag = 0;
	for (i = 0; i < TARGET_AMOUNT; i++) {
		if ((*(targets + i)).hit != true) {
			flag = 1;
			//(*(targets + i)).angle = rand() % 360 - 180;
			//(*(targets + i)).angle = 0;
			(*(targets + i)).radius = ((int)((WINDOW_WIDTH / 32) * sin(((*(targets + i)).angle) *D2R)) % (WINDOW_WIDTH / 8 + 100) - WINDOW_WIDTH / 7.5) + WINDOW_WIDTH / 12;

			(*(targets + i)).pos.x = (WINDOW_WIDTH / 8 + 50 + i * WINDOW_WIDTH / 8) * cos(((*(targets + i)).angle) * D2R);
			(*(targets + i)).pos.y = (WINDOW_HEIGHT / 8 + i * WINDOW_HEIGHT / 8) * sin(((*(targets + i)).angle) *D2R);
			
			glColor3f(0.3, 0.3, 1);
			refCircle((*(targets + i)).pos.x, (*(targets + i)).pos.y, (*(targets + i)).radius, (int)(*(targets + i)).angle, (*(targets + i)).r, (*(targets + i)).g, (*(targets + i)).b);
			
			glColor3f(0.8, 0.8, 0.8);
			//vprint((WINDOW_WIDTH / 4 + i * 50) * cos(((*(targets + i)).angle) * D2R), 100, GLUT_BITMAP_8_BY_13, "%d", (int)(*(targets + i)).angle);
		}
	}
	if (flag == 0)
		initialize();

}

//
// To display onto window using OpenGL commands
//
void display() {

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor4f(1, 1, 1, 0.1);

	glBegin(GL_LINES);
	glVertex2f(-WINDOW_WIDTH / 2, 0);
	glVertex2f(WINDOW_WIDTH / 2, 0);
	glVertex2f(0, -WINDOW_HEIGHT / 2);
	glVertex2f(0, WINDOW_HEIGHT / 2);
	glEnd();

	glColor3f(1, 1, 1);
	//vprint(190, 30, GLUT_BITMAP_8_BY_13, "%0.f", (*(blt + 1)).dist); //white angle
	vprint(player.pos.x - 5, player.pos.y - 50, GLUT_BITMAP_HELVETICA_18, "%2d", bulletcount);

	//vprint(-190, 50, GLUT_BITMAP_8_BY_13, "%d", hitcnt);
	glColor3f(0.7, 0.7, 0.7);
	vprint((WINDOW_WIDTH / 16) * cos((player.angle) * D2R), (WINDOW_HEIGHT / 16) * sin((player.angle) * D2R), GLUT_BITMAP_8_BY_13, "%3.0f", player.angle);

	for (i = 0; i < bulletcount; i++) {
		vprint(player.pos.x + 3 + i * 2 - bulletcount, player.pos.y - 30, GLUT_BITMAP_HELVETICA_12, "l");
	}

	

	dispProj();

	glColor4f(1, 1, 0.9, 1);
	circle(-WINDOW_WIDTH / 1.5, -WINDOW_HEIGHT / 1.5, 255);
	refCircle(-WINDOW_WIDTH / 1.5, -WINDOW_HEIGHT / 1.5, 250, -135, 1, 1, 0);
	glColor4f(1, 1, 0.9, 0.5);
	circle(-WINDOW_WIDTH / 1.5, -WINDOW_HEIGHT / 1.5, 250);
	
	dispPlayer();
	glColor3f(0.3, 0.3, 1);
	dispOrbit();
	dispTargets();

	glColor3f(1, 1, 1);
	vprint(WINDOW_WIDTH / 2 - 175, WINDOW_HEIGHT/ 2 - 25, GLUT_BITMAP_HELVETICA_18, "3D Orbiting Targets");
	vprint(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT/ 2 - 50, GLUT_BITMAP_HELVETICA_12, "by E. Berke KARAGOZ");


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
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
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
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
		shoot = true;
	}
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_UP) {
		shoot = false;
	}

	x = x - winWidth / 2;
	y = winHeight / 2 - y;
	printf("X = %d\tY = %d\n", x, y);

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
	x = x - winWidth / 2;
	y = winHeight / 2 - y;
	double val = 180.0 / PI;
	player.angle = atan2(y - player.pos.y, x - player.pos.x) * val;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	timecount++;
	if (shoot && bulletcount > 0)
	{
		int k = findAvailableFire();
		if (k != -1)
		{
			(*(blt + k)).pos = player.pos;
			(*(blt + k)).dist = 5;
			(*(blt + k)).angle = player.angle;
			(*(blt + k)).flag = true;
			bulletcount--;
		}
	}
	int k;
	// Move all fires that are active.
	for (int i = 0; i < MAX; i++)
	{
		if ((*(blt + i)).flag)
		{
			(*(blt + i)).dist -= sin((*(blt + i)).angle * D2R) / 9;
			(*(blt + i)).pos.x += 10 * cos((*(blt + i)).angle * D2R);
			(*(blt + i)).pos.y += 3 + (*(blt + i)).dist * 2 * sin((*(blt + i)).angle * D2R);
			for (k = 0; k < TARGET_AMOUNT; k++) {
				//if ((*(blt + i)).pos.x > (*(targets + k)).pos.x - 40 && (*(blt + i)).pos.y > (*(targets + k)).pos.y - 40 && (*(blt + i)).pos.x < (*(targets + k)).pos.y + 40 && (*(blt + i)).pos.y < (*(targets + k)).pos.x + 40)
				//	hitcnt++;
				float distance = sqrtf((blt[i].pos.x - targets[k].pos.x) * (blt[i].pos.x - targets[k].pos.x) + (blt[i].pos.y - targets[k].pos.y) * (blt[i].pos.y - targets[k].pos.y));
				if (distance <= fabs(targets[k].radius) + fabs((*(blt + i)).dist)) {
					targets[k].hp -= 1;
					(*(blt + i)).flag = false;
					//bulletcount++;
					reltime++;
					if (targets[k].hp == 0) {
						targets[k].hit = true;
						targets[k].radius = 0;


					}
				}
			}

			if ((*(blt + i)).pos.x > WINDOW_WIDTH / 2 || (*(blt + i)).pos.x < -WINDOW_WIDTH / 2 || (*(blt + i)).pos.y > WINDOW_HEIGHT / 2 || (*(blt + i)).pos.y < -WINDOW_HEIGHT / 2)
			{
				bulletcount++;
				(*(blt + i)).flag = false;
				(*(blt + i)).dist = 0;
			}
		}
	}
	if (reltime > 0 && timecount % 100 == 0)
		while (reltime != 0) {
			bulletcount++;
			reltime--;
		}

	for (i = 0; i < TARGET_AMOUNT; i++) {
		(*(targets + i)).angle -= (*(targets + i)).speed;
		if (fabs((*(targets + i)).angle) >= 360)
			(*(targets + i)).angle = 0;
	}
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
	glutCreateWindow("E. Berke KARAGOZ 'HW3' 3D Orbiting Targets");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	initialize();

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