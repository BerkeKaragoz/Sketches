/*********
CTIS164 - Template Source Program
----------
STUDENT :
SECTION :
HOMEWORK:
----------
PROBLEMS: If your program does not function correctly,
explain here which parts are not running.
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  1600
#define WINDOW_HEIGHT 1200

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define TARGET_AMOUNT 3

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
	float angle, r, g, b, radius;
	bool hit;
} target_t;

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int winWidth, winHeight; // current Window width and height
int targetangle = 0, i;
player_t player;
target_t targets[3];
trigo_t tri;

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

void initialize() {
	player.lenght = 10;
	player.pos.x = 0;
	player.pos.y = 0;
	for (i = 0; i < TARGET_AMOUNT; i++)
		(*(targets + i)).angle = rand() % 360 - 180;
}

void dispPlayer() {


	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(player.pos.x + player.lenght*tri.x - 10, player.pos.y + player.lenght*tri.y + 15);

	glVertex2f(player.pos.x + 15, player.pos.y + 15);

	glVertex2f(player.pos.x - 10, player.pos.y - 10);

	glVertex2f(player.pos.x + player.lenght*tri.x / 4 + 15, player.pos.y + player.lenght*tri.y / 4 - 15);
	glEnd();

	glColor3f(0.5, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(player.pos.x + player.lenght*tri.x - 10, player.pos.y + player.lenght*tri.y + 15);

	glVertex2f(player.pos.x + 15, player.pos.y + 15);


	glVertex2f(player.pos.x + player.lenght*tri.x / 4 + 15, player.pos.y + player.lenght*tri.y / 4 - 15);
	glEnd();

	//glBegin(GL_LINES);
	//glVertex2f(player.pos.x, player.pos.y);
	//glVertex2f(player.pos.x + tri.x*player.lenght, player.pos.y + tri.y*player.lenght);
	//glEnd();
}

/*
bool hitToIcons(player_t *p, target_t *icons, int size) {
	double d;
	for (int i = 0; i < size; i++) {
		d = dist(p->pos, icons[i].pos);
		if (d <= (p->radius + icons[i].radius)) {
			icons[i].hit = true;
			p->hit = true;
		}
	}
	return p->hit;
}
*/
//
// To display onto window using OpenGL commands
//
void display() {

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);


	glColor3f(1, 1, 1);
	double res = rand() % 360 - 180;
	vprint(190, 30, GLUT_BITMAP_8_BY_13, "%0.f", res); //white angle

	dispPlayer();

	glColor3f(0.3, 0.3, 1);
	srand(time(NULL));

	for (i = 0; i < TARGET_AMOUNT; i++) {
		//(*(targets + i)).angle = rand() % 360 - 180;
		//(*(targets + i)).angle = 0;
	
		glColor3f(0.3, 0.3, 1);
		circle((500 + i * 50) * cos(((*(targets + i)).angle + targetangle) * D2R), (150 + i * 50) * sin(((*(targets + i)).angle + targetangle) *D2R), ((int)((50 + 0) * sin(((*(targets + i)).angle + targetangle) *D2R)) % 300 - 200)+125);
		glColor3f(0.8,0.8,0.8);
		vprint((500 + i * 50) * cos(((*(targets + i)).angle + targetangle) * D2R), 300, GLUT_BITMAP_8_BY_13, "%d", ((int)((50 + 0) * sin(((*(targets + i)).angle + targetangle) *D2R)) % 300 - 200) + 125);
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
	player.angle = atan(fabs(y - player.pos.y) / fabs(x - player.pos.x));

	if (y - player.pos.y >= 0 && x - player.pos.x >= 0) {
		tri.x = cos(player.angle);
		tri.y = sin(player.angle);
	}
	else if (y - player.pos.y < 0 && x - player.pos.x >= 0) {
		tri.x = cos(player.angle);
		tri.y = -sin(player.angle);
	}
	else if (y - player.pos.y >= 0 && x - player.pos.x < 0) {
		tri.x = -cos(player.angle);
		tri.y = sin(player.angle);
	}
	else if (y - player.pos.y < 0 && x - player.pos.x < 0) {
		tri.x = -cos(player.angle);
		tri.y = -sin(player.angle);
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	targetangle++;

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
	glutCreateWindow("Template File");

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
