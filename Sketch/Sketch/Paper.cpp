#define SKETCH 10

//String.h funcs
#if SKETCH == 0
#include<stdio.h>
#include<string.h>

//strlen, strcpy, strcat, strcmp

void main() {
	int lenght, diff;
	char input[50] = {"Lol braz."}, a[50] = " ", b[50] = { "Lol dudez." };

	//printf("Put this:");
	//scanf("%[^\n]", input);


	lenght = strlen(input);

	strncpy(a, input, 5);
	
	diff = strncmp(input, b, 6);

	printf("Input: %s\nLenght: %d\na: %s\n", input, lenght, a);
	printf("Strcat: %s\nb: %s\nStrcomp: %d\n", strcat(a, input), b, diff);
}
#endif // 0

//String Test (Useless)
#if SKETCH == 1
#include<stdio.h>
#include<string.h>
#include<ctype.h>

//strlen, strcpy, strcat, strcmp

void main() {
	char input[50];

	printf("Put this:");
	scanf("%s", input);

	printf("Input: %s", input);
}
#endif // SKETCH == 1

//isUpper/isLower
#if SKETCH == 2
#include<stdio.h>
#include<ctype.h>

void main() {

	char ch1 = 'b', ch2 = 'B';
	ch1 = toupper(ch1);
	printf("%c %d\n", ch1, !isupper(ch1));
	printf("%c %d\n", tolower(ch2), !isupper(ch2));
}
#endif // SKETCH == 2

//State Machine Game aka Flickshot Trainer
#if SKETCH == 3
//05-StateMachine-Game

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

/* TEMPLATE CONSTANTS AND VARIABLES */
#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 500

#define  TIMER_PERIOD  300 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532

bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height


						  /* USER GLOBAL VARIABLES and CONSTANTS */
#define  BLOCK_NUMBERS  30
#define  MIN_TIME_PERIOD    400
#define  TIME_DECREMENT    50

#define LOAD 0
#define MENU 1
#define RUN 2
#define GAMEOVER 3

int cx, cy;  // randomly selected cells coordinate.
int mx, my;  // current cell of the mouse position
int score;   // how many times the user clicked the popped cell.
int counter; // how many cells are popped out randomly.
bool active; // prevent more than one click on the same cell.
int timer;   // set the speed of game.

int gameState = LOAD;
int dotCount = 0;
int elapsedTime = 0;

//
// Intialize global variables.
//
void Init_Globals() {
	score = 0;
	counter = 0;
	timer = 300;
	// Initially, make the current selection outside of the window.
	cx = -100;
	cy = -100;
	mx = -100;
	my = -100;
	active = true;

}


//
// to draw circle, center at (x,y)
//  radius r
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
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}


// display text with variables.
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
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

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
	for (i = 0; i<len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void drawGrid() {
	glColor3f(0.6, 0.6, 0.6);
	for (int i = -4; i <= 4; i++) {
		glBegin(GL_LINES);
		glVertex2f(50 * i, -200);
		glVertex2f(50 * i, 200);

		glVertex2f(-200, 50 * i);
		glVertex2f(200, 50 * i);
		glEnd();
	}
}

void drawCell() {
	int sx = cx * 50;
	int sy = cy * 50;
	glColor3f(0.9, 0.9, 0.9);
	glRectf(sx + 3, sy + 3, sx + 46, sy + 46);
}

void borderCell() {
	if (mx <= 3 && mx >= -4 && my <= 3 && my >= -4) {
		int posx = mx * 50;
		int posy = my * 50;
		glColor3f(0.7, 0.7, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(posx, posy);
		glVertex2f(posx, posy + 50);
		glVertex2f(posx + 50, posy + 50);
		glVertex2f(posx + 50, posy);
		glEnd();
	}
}

void display_load() {
	vprint2(-50, -50, 0.2, "LOADING");
	for (int i = 0; i < dotCount; i++) {
		vprint2(80 + i * 20, -50, 0.2, ".");
	}
}

void display_menu() {
	glColor3f(1, 1, 0);
	vprint(-30, 100, GLUT_BITMAP_TIMES_ROMAN_24, "MENU");
	glColor3f(1, 1, 1);

	vprint(-50, 50, GLUT_BITMAP_HELVETICA_18, "START GAME");
	vprint(-50, 20, GLUT_BITMAP_HELVETICA_18, "OPTION");
	vprint(-50, -10, GLUT_BITMAP_HELVETICA_18, "EXIT");

	glColor3f(0, 1, 0);
	vprint(-100, -230, GLUT_BITMAP_9_BY_15, "SpaceBar to Start Game");
}

void display_run() {
	drawGrid();
	borderCell();
	drawCell();
	vprint(-240, -240, GLUT_BITMAP_9_BY_15, "Score: %d/%d", score, counter);
}

void display_gameover() {
	glColor3f(1, 1, 0);
	vprint(-50, 0, GLUT_BITMAP_9_BY_15, "Score: %d/%d", score, counter);
	glColor3f(1, 0.5, 0.5);
	vprint(-70, 30, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");

	glColor3f(1, 1, 1);
	vprint(-70, -230, GLUT_BITMAP_8_BY_13, "- F5 to go to Menu -");
}

//
// To display onto window using OpenGL commands
//
void display()
{

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	switch (gameState) {
	case LOAD: display_load(); break;
	case MENU: display_menu(); break;
	case RUN: display_run(); break;
	case GAMEOVER: display_gameover(); break;
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

	// Start Game when the user presses spacebar in Menu.
	if (key == ' ' && gameState == MENU) {
		gameState = RUN;
		timer = 2000;
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
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;

	}
	// Go back to Menu.
	if (key == GLUT_KEY_F5 && gameState == GAMEOVER) {
		Init_Globals();  // restart game.
		gameState = MENU;
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
void onClick(int button, int state, int x, int y)
{
	// Write your codes here.

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// if the user clicks on the random block, increase score by 1.
		if (mx == cx && my == cy && active == true && gameState == RUN) {
			score++;
			active = false; // prevent more than one click for the same block.
		}
	}



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

void onMove(int x, int y) {
	// Write your codes here.

	// from GLUT to OpenGL coordinate
	x = x - winWidth / 2;
	y = winHeight / 2 - y;

	// from OpenGL to Block coordinate
	mx = floor(x / 50.0f);
	my = floor(y / 50.0f);


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(timer, onTimer, 0);
	// Write your codes here.

	if (gameState == LOAD) {
		dotCount = (dotCount + 1) % 4;
		elapsedTime++;
		if (elapsedTime == 8) {
			gameState = MENU;
		}
	}
	else if (gameState == RUN) {
		if (counter == BLOCK_NUMBERS) {
			gameState = GAMEOVER;
		}
		else {

			cx = rand() % 8 - 4;
			cy = rand() % 8 - 4;
			active = true;

			if (timer > MIN_TIME_PERIOD) {
				timer -= TIME_DECREMENT;
			}

			counter++;
		}
	}



	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {
	Init_Globals();
}


void main(int argc, char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Template File");

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
#endif // SKETCH == 3

//Stickman Runner
#if SKETCH == 4
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

						  //Glubuls
int posx[RACER_AMOUNT], posy[RACER_AMOUNT], winner, counter[RACER_AMOUNT] = { 0 }, multiplier = 10;
int gamestate = LOAD, timecount = 0, initial, i, ms, sec,
max = -5000; // for leading player

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
	vprint2(-150, 40, 0.6, "RUNNER");
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
			stickMan(*(posx + initial), *(posy + initial), r, g, b, initial);
		else stickmanReverse(*(posx + initial), *(posy + initial), r, g, b, initial);
		r = g = b = 0;
	}
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
		vprint(-350, 335, GLUT_BITMAP_HELVETICA_12, "Lead:");
		vprint(-350, 315, GLUT_BITMAP_HELVETICA_18, "%0.3d", winner + 1);
	}

	if (timecount > STARTING_TIME - 25 && timecount < STARTING_TIME + 25) {
		glColor3f(0, 0, 0);
		vprint2(-60, 150, 0.5, "START!");
	}
	if (timecount > STARTING_TIME + 40 && timecount % 50 >= 15) {
		glColor3f(0.8, 0.2, 0.3);
		glBegin(GL_TRIANGLES);
		glVertex2f(*(posx + winner), *(posy + winner) + 20);
		glVertex2f(*(posx + winner) - 10, *(posy + winner) + 40);
		glVertex2f(*(posx + winner) + 10, *(posy + winner) + 40);
		glEnd();
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
		*(posx + initial) = -500;
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
						*(posx + i) += *(speed + i);
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
					if (*(posx + i) >= WINDOW_WIDTH / 2 - 50) {
						*(touch + i) = true;
						flag = true;
						//if (max < *(speed + i)) {
						//	max = *(speed + i);
						//	winner = i;
						//}
						*(accel + i) = (rand() % 30 + 30) / 100.0;
						printf("No %d: Accel: %0.4f aceel/20: %0.4f Speed: %0.4f\n", i, *(accel + i), *(accel + i) / 40.0, *(speed + i));

					}
					if (*(posx + i) <= -WINDOW_WIDTH / 2 + 50) {
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
	glutCreateWindow("Stickman Runner");

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
#endif // SKETCH == 4

//HASHING
#if SKETCH == 5
/*LG8_Q3 -> Hash Table*/

#include<stdio.h>
#include<stdlib.h>

#define SIZE 15

enum record_status { EMPTY, DELETED, OCCUPIED };

typedef struct {
	int ct_id;
	char ct_name[20];
	char ct_department[20];
}candidate_t;

typedef struct {
	candidate_t info;
	enum record_status status;
}hash_t;

int hashFunction(int key)
{
	return (key % SIZE);
}

int searchFromHash(int key, hash_t *hash_table)
{
	int count, temp, position;
	temp = hashFunction(key);
	position = temp;
	for (count = 1; count != SIZE - 1; count++)
	{
		if (hash_table[position].status == EMPTY)
			return -1;
		if (hash_table[position].info.ct_id == key)
			return position;
		position = (temp + count) % SIZE;
	}
	return -1;
}

void insertIntoHash(candidate_t candidate, hash_t hash_table[])
{
	int count, position, temp, flag = 1;
	int key = candidate.ct_id;
	temp = hashFunction(key);
	position = temp;
	for (count = 1; count != SIZE - 1 && flag; count++)
	{
		if (hash_table[position].status == EMPTY || hash_table[position].status == DELETED)
		{
			hash_table[position].info = candidate;
			hash_table[position].status = OCCUPIED;
			printf("\nRecord Inserted into the Hash Table\n");
			flag = 0;
		}
		else if (hash_table[position].info.ct_id == key)
		{
			printf("\nDuplicate Record cannot be Inserted\n");
			flag = 0;
		}
		else position = (temp + count) % SIZE; //linear probing
	}
	if (flag) printf("\nHash Table Limit Exceeded\n");

}

void displayHashContents(hash_t hash_table[])
{
	int count;
	printf("\nHash Table\n");
	for (count = 0; count < SIZE; count++)
	{
		printf("[%d]:\t", count);
		if (hash_table[count].status == OCCUPIED)
			printf("Occupied - ID: %d    Name: %s   Department: %s\n", hash_table[count].info.ct_id, hash_table[count].info.ct_name, hash_table[count].info.ct_department);
		else if (hash_table[count].status == DELETED)
			printf("Deleted\n");
		else
			printf("Empty\n");
	}
}

void deleteFromHash(int key, hash_t hash_table[])
{
	int position = searchFromHash(key, hash_table);
	if (position == -1)
		printf("\nId Not Found\n");
	else
	{
		hash_table[position].status = DELETED;
		printf("\nId deleted from the Hash Table\n");
	}
}

int menu()
{
	int option;
	printf("\n     MENU\n");
	printf("*********************\n");
	printf("1. Insert a Candidate\n");
	printf("2. Delete a Candidate\n");
	printf("3. Search a Candidate\n");
	printf("4. Display All Candidates\n");
	printf("5. Exit\n");
	do {
		printf("Enter Your Option:\t");
		scanf("%d", &option);
	} while (option <1 || option >5);
	return option;
}

int main()
{
	int count, key, option;
	hash_t hash_table[SIZE];
	candidate_t candidate;
	for (count = 0; count <= SIZE - 1; count++)
		hash_table[count].status = EMPTY;
	do
	{
		option = menu();

		switch (option)
		{
		case 1: printf("\nEnter Candidate ID:\t");
			scanf("%d", &candidate.ct_id);

			printf("Enter Candidate Name:\t");
			scanf("%s", candidate.ct_name);

			printf("Enter Department of Candidate:\t");
			scanf("\n%[^\n]", candidate.ct_department);

			insertIntoHash(candidate, hash_table);
			break;

		case 2: printf("\nEnter the ID to Delete Candidate:\t");
			scanf("%d", &key);

			deleteFromHash(key, hash_table);
			break;

		case 3: printf("\nEnter the Id to Search Candidate:\t");
			scanf("%d", &key);

			count = searchFromHash(key, hash_table);

			if (count == -1)
				printf("\nId Not Found\n");
			else
			{
				printf("\nId Found at Index Position:\t%d\n", count);
				printf("ID: %d     Name : %s     Department : %s\n", hash_table[count].info.ct_id, hash_table[count].info.ct_name, hash_table[count].info.ct_department);
			}
			break;

		case 4: displayHashContents(hash_table);
			break;
		}
	} while (option != 5);
	return 0;
}


#endif // SKETCH == 5

//LAB Guide 8 - 1
#if SKETCH == 6
#include<stdio.h>
#include<stdlib.h>
#define SIZE 40

int binSearch(int *arr, int top, int bot, int src, int *cnt)
{
	if (top <= bot)
	{
		int mid = (top + bot) / 2;

		if (*(arr+mid) == src)
			return (mid);
		else if (src > *(arr+mid))
			top = mid + 1;
		else
			bot = mid - 1;
		(*cnt)++;
		binSearch(arr, top, bot, src, cnt);
	}
	return -1;
}

void readFile(FILE *inptr, int *arr, int *i) {
	//WOT?
	if (fscanf(inptr, "%d", arr) != EOF) {
		(*i)++;
		readFile(inptr, arr + 1, i);
	}
}

void main() {

	FILE *inptr = fopen("scores.txt", "r");

	if (inptr == NULL)
		printf("Something happened...\n");
	else {
		int *arr, src, cnt = 0, i = -1, res;

		arr = (int *)malloc(sizeof(int)*SIZE);
		readFile(inptr, arr, &i);

		free(arr);
		arr = (int *)malloc(sizeof(int)*i);

		printf("Enter a value to find: ");
		scanf("%d", &src);

		res = binSearch(arr, 0, i, src, &cnt);

		if (res != -1)
			printf("%d is found at %d. line.\n%d comparisons.\n", res, cnt);
		else
			printf("The number is NOT found in the scores.txt\n%d comparisons.\n", cnt);


		free(arr);
		fclose(inptr);
	}
}
#endif // SKETCH == 6

//Archery Practice Uncompleted Edition
#if SKETCH == 7
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

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

#define TIMER_PERIOD  50 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
int timer = 0, mseconds, seconds, onmovex = 0, onmovey = 0, playerx = 0;
#define TARGET_AMOUNT 5
#define PLAY 0
#define END 1
int targets[TARGET_AMOUNT], targetsx[TARGET_AMOUNT], gamestate = PLAY;
bool active[TARGET_AMOUNT] = { false }, fire;


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
}

void drawAreas(int thickness, double factor) {

	//hor
	glColor3f(0.05, 0.1, 0.05);
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


	toTime();

	glColor3f(0.8, 0.95, 0.8);
	vprint(-50, -winHeight / 2 + thickness / (factor*factor), GLUT_BITMAP_9_BY_15, "OVERLOK MACHINE CAME TO YOUR FEET");
	vprint(350, -winHeight / 2 + thickness / 2, GLUT_BITMAP_HELVETICA_18, "Time: %0.2d:%0.2d", seconds, mseconds);

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
		if (targetsx[i] < -winWidth / 2 - 50) {
			active[i] = false;
			targetsx[i] = WINDOW_WIDTH / 2 + 50;
		}
	}

}

void arrow(int thickness, int pos, int size) {
	glColor3f(0.5, 0.5, 0.5);

	bool hit = false;
	int i, y = -winHeight / 2 + thickness - 50 + 115, speed = 0;
	do
	{
		speed++;
		y += speed;
		glBegin(GL_TRIANGLES);
		glVertex2f(pos, y);
		glVertex2f(pos + 7, y - 15);
		glVertex2f(pos - 7, y - 15);
		glEnd();
		for (i = 0; i < TARGET_AMOUNT; i++) {

			/*

			IT WONT AFFECT WHETER IT IS A 10 HIT OR 1 HIT


			*/
			if (targets[i] == y && pos > targetsx[i] - size * 2.5 && pos > targetsx[i] + size * 2.5)
				hit = true;
		}
	} while (hit == false);//?

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
		fire == false;

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
		fire = true;

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
	case GLUT_KEY_LEFT: playerx -= 8; left = true; break;
	case GLUT_KEY_RIGHT:playerx += 8; right = true; break;
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
typedef struct {
	int posx, posy, angle, lenght;
}player_t;
player_t player = { -WINDOW_WIDTH / 2 + 50, 0, 0, 30, };
void onMove(int x, int y) {
	// Write your codes here.

	x = x - winWidth / 2;
	y = winHeight / 2 - y;
	player.angle = atan(fabs(y - player.posy) / fabs(x - player.posx));
	vprint(x - 15, y - 30, GLUT_BITMAP_9_BY_15, "%d", player.angle);
	//onmovex = x;
	//onmovey = y;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
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
	glutCreateWindow("Temp - HW2");
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

#endif // SKETCH == 7

//Free Throwing
#if SKETCH == 8
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

#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 500

#define TIMER_PERIOD  10 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

						  //Glubuls
typedef struct {
	float x = 0, y = 0;
}point_t;

point_t initpos, pos;
int timecount = 0, vel = 0;
bool flag = false;

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

//
// To display onto window using OpenGL commands
//
void display() {

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	circle(pos.x, pos.y, 5);
	vprint(pos.x - 15, pos.y - 30, GLUT_BITMAP_HELVETICA_12, "Velocity: %d", vel);

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
	if (key == 32) {
		flag = !flag;
		printf("Flag: %d\n", flag);
		if (flag == false)
			timecount = 0;
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
		vel += 10;
		printf("Vel+10: %d\n", vel);
		up = true; break;
	case GLUT_KEY_DOWN:
		vel -= 10;
		if (vel < 1)
			vel = 1;
		printf("Vel-10: %d\n", vel);
		down = true; break;
	case GLUT_KEY_LEFT:
		if (vel > 1) {
			vel--;
			printf("Vel--: %d\n", vel);
		}
		left = true; break;
	case GLUT_KEY_RIGHT: vel++;
		printf("Vel++: %d\n", vel); right = true; break;
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

	if (stat == GLUT_DOWN) {
		initpos.x = x - winWidth / 2;
		initpos.y = winWidth / 2 - y;
		printf("X = %.1f Y = %.1f\n", initpos.x, initpos.y);
		pos.x = initpos.x;
		pos.y = initpos.y;
	}

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



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
	if (flag == true) {
		timecount++;
		printf("Time: %d\n", timecount);
		pos.x = initpos.x + vel*timecount / 10.;
		pos.y = initpos.y + -5 * powf(timecount / 10., 2);
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
	glutCreateWindow("Template File");

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
#endif // SKETCH == 8

//Archery Practice Completed Golden Edition
#if SKETCH == 9
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

#define TIMER_PERIOD  15// Period for the timer.
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
			speed = 0;
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
			}
			//if hits 6
			if (targets[i] < y + size * 1 && targets[i] > size*-1 && targetsx[i] < pos + size * 2 && targetsx[i] > pos + size*-2 && hit == false)
			{
				hit = true;
				active[i] = false;
				hitcnt++;
				points = points + 6;
				lastHitPoints = 6;
			}
			//if hits 1
			if (targets[i] < y + size * 1 && targets[i] > size*-1 && targetsx[i] < pos + size * 4 && targetsx[i] > pos + size*-4 && hit == false)
			{
				hit = true;
				active[i] = false;
				hitcnt++;
				points++;
				lastHitPoints = 1;
			}
			//glBegin(GL_POLYGON);
			//glVertex2f(, y + size * 1)
			hit = false;
		}

	} while (hit == true || fire == false);
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
		fire == false;

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
		speed = 35;
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


#endif // SKETCH == 9

//Stickman Shooter
#if SKETCH == 10
/*********
CTIS164 - Template Source Program
----------
STUDENT : E. Berke KARAG?Z / 21704206
SECTION : 2
HOMEWORK: Racing Game
----------
PROBLEMS: ?Problemo?
----------
Features:
- Used trigonometry to aim and shoot with the mouse.
- Targets have health and they are adjustable
- Target health bar
- Player move animation
- Timer becomes red when 3 seconds left
- Starting countdown
- Total hits, score, total shots fired, last hit points and location
- Shows closest target hp (new technique to track)
- Changeable game duration.
- Changeable target amount
Could add more but...
From last homework:
- Works at 60 fps. TIMER_PERIOD = 166.6 ( divided by multiplier which is 10) -> 1 seconds = 1000 ms and 1000(ms)/60(hertz)= 16.6
- 4 States
- Adjustable target amount, with adjusting lane spacing.
- All racers have diffrent look.
- Using acceleration to be smooth
- Pause / Continue
- Slow and fast motion with arrow keys
Note:
Homework stated that we have to set the score points 1-5 depending on how far we shoot from the middle, instead i made headshot (5 pts), bodyshot (2 pts) and legshot hitboxes (1 pts).
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
#define TARGET_MAX 50 // <---- ADJUSTABLE
#define STARTING_TIME 80

typedef struct {
	int x, y;
}point_t;

typedef struct {//trigonometry
	double x, y;
}tri_t;

typedef struct {
	point_t pos;
	int lenght, width;
	double angle;
}player_t;

typedef struct {
	point_t pos;
	tri_t tri;
}projectile_t;

typedef struct {
	point_t pos;
	int speed;
	tri_t tri;
};
//Glubuls
int posx[TARGET_MAX], posy[TARGET_MAX], counter[TARGET_MAX] = { 0 }, multiplier = 10, gamestate = LOAD,
timecount = 0, initial, i, ms, sec, bulletcount = -1, bulletspeed = 10, score = 0, health[TARGET_MAX], healthpoints = 4,
gametime = 20, accurate = 0, lasthit = 0, closest = 0, targetamount = 5;
player_t player = { { -WINDOW_WIDTH / 2 + 65, 0 }, 40, 10, 0, };
point_t onmove;
tri_t tri;
projectile_t *bullets;

double accel[TARGET_MAX], speed[TARGET_MAX];
bool touch[TARGET_MAX],//if touched to the limit
flag = false,
timer = true,
*shoot;

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
	glVertex2f(mposx - 15 - ((mposy - 5000) % 50), mposy - 112);
	//lowerleg 1
	glVertex2f(mposx - 15 - ((mposy - 5000) % 50), mposy - 112);
	glVertex2f(mposx - 40 - ((mposy - 5000) % 50), mposy - 145);

	//upperleg 2
	glVertex2f(mposx - 5, mposy - 75);
	glVertex2f(mposx + 30 + ((mposy - 5000) % 50), mposy - 100);
	//lowerleg 2
	glVertex2f(mposx + 30 + ((mposy - 5000) % 50), mposy - 100);
	glVertex2f(mposx + 5 + ((mposy - 5000) % 50), mposy - 130);

	glEnd();

	circle(mposx + 17 - ((mposx + 5000) % 50), mposy - 49 + ((mposx + 5000) % 100) / 5, 5); //arm joint1
	circle(mposx - 16 + ((mposx + 5000) % 50), mposy - 48 + ((mposx + 5000) % 50) / 10, 5); //arm joint2
	circle(mposx - 15 - ((mposy - 5000) % 50), mposy - 112, 5); //leg joint1
	circle(mposx + 30 + ((mposy - 5000) % 50), mposy - 100, 5); //leg joint2
	circle(mposx + 25 - ((mposx + 5000) % 50) / 1, mposy - 10 - ((mposx + 5000) % 50) * 1.1, 5); //hand 1
	circle(mposx - 18 + ((mposx + 5000) % 50) / 1, mposy - 65 + ((mposx + 5000) % 50) * 1.1, 5); //hand 2
}
void stickmanReverse(int mposx, int mposy, float r, float g, float b, int hp) {

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

	for (i = 0; i < healthpoints; i++) {
		if (hp > i)
			glColor3f(0, 1, 0);
		else glColor3f(1, 0, 0);
		vprint(mposx - 3 + i * 2 - healthpoints, mposy + 20, GLUT_BITMAP_HELVETICA_12, "l");
	}
}
void dispPlayer() {

	stickMan(player.pos.x - 6, player.pos.y + 30, 1, 0, 0, -1);

	glColor3f(0.2, 0.2, 0.2);
	glLineWidth(player.width);
	glBegin(GL_LINES);
	glVertex2f(player.pos.x, player.pos.y);
	glVertex2f(player.pos.x + tri.x*player.lenght, player.pos.y + tri.y*player.lenght);
	glVertex2f(player.pos.x + 5, player.pos.y);
	glVertex2f(player.pos.x + 5 + tri.y*player.lenght / 2, player.pos.y + -tri.x*player.lenght / 2);
	glEnd();

	//vprint(player.pos.x - 15, player.pos.y - 30, GLUT_BITMAP_9_BY_15, "%0.2f", player.angle);
	//player.angle = atan(fabs(y - player.posy) / fabs(x - player.posx));
}


void hitBoxes() {
	for (initial = 0; initial < targetamount; initial++) {
		if (*(posx + initial) - 9 < (*(bullets + i)).pos.x && *(posx + initial) + 9 > (*(bullets + i)).pos.x && *(posy + initial) - 9 < (*(bullets + i)).pos.y && *(posy + initial) + 9 > (*(bullets + i)).pos.y) {
			printf("Headshot! %d\n", initial + 1);
			*(shoot + i) = false;
			score += 4;
			*(health + initial) -= 5;
			lasthit = 5;
			accurate++;
			if (*(health + initial) <= 0) {
				*(speed + initial) = 0.1;
				printf("No %d: Accel: %0.4f Speed: %0.4f\n", initial, *(accel + initial), *(speed + initial));
				*(posx + initial) = WINDOW_WIDTH / 2 + 50 + rand() % 500;
				*(health + initial) = healthpoints;
			}
		}
		else if (*(posx + initial) - 6 < (*(bullets + i)).pos.x && *(posx + initial) + 6 > (*(bullets + i)).pos.x && *(posy + initial) - 75 < (*(bullets + i)).pos.y && *(posy + initial) - 9 > (*(bullets + i)).pos.y) {
			printf("Bodyshot! %d\n", initial + 1);
			*(shoot + i) = false;
			score += 2;
			*(health + initial) -= 2;
			lasthit = 2;
			accurate++;
			if (*(health + initial) <= 0) {
				*(speed + initial) = 0.1;
				printf("No %d: Accel: %0.4f Speed: %0.4f\n", initial, *(accel + initial), *(speed + initial));
				*(posx + initial) = WINDOW_WIDTH / 2 + 50 + rand() % 500;
				*(health + initial) = healthpoints;
			}
		}
		else if (*(posx + initial) - 6 < (*(bullets + i)).pos.x && *(posx + initial) + 6 > (*(bullets + i)).pos.x && *(posy + initial) - 135 < (*(bullets + i)).pos.y && *(posy + initial) - 75 > (*(bullets + i)).pos.y) {
			printf("Legshot %d\n", initial + 1);
			*(shoot + i) = false;
			score += 1;
			*(health + initial) -= 1;
			lasthit = 1;
			accurate++;
			if (*(health + initial) <= 0) {
				*(speed + initial) = 0.1;
				printf("No %d: Accel: %0.4f Speed: %0.4f\n", initial, *(accel + initial), *(speed + initial));
				*(posx + initial) = WINDOW_WIDTH / 2 + 50 + rand() % 500;
				*(health + initial) = healthpoints;
			}
		}
	}
}

void dispBullet(int index) {

	glColor3f(0, 0, 0);
	//printf("%d ", index);
	for (i = 0; i <= index; i++) {
		circle(((*(bullets + i)).pos.x), ((*(bullets + i)).pos.y), 3);

		if ((*(bullets + i)).pos.y > WINDOW_HEIGHT / 2 || (*(bullets + i)).pos.x > WINDOW_WIDTH / 2 || (*(bullets + i)).pos.y < -WINDOW_HEIGHT || (*(bullets + i)).pos.x < -WINDOW_WIDTH)
			*(shoot + i) = false;
		hitBoxes();
	}

}
void displayLoad() {
	glColor3f(0.5, 0.2, 0.2);
	vprint2(-125, 50, 0.6, "LOADING");

	glColor3f(0.4, 0.1, 0.1);
	glLineWidth(10);
	glBegin(GL_LINES);
	glVertex2f(-250, -25);
	glVertex2f((timecount / 2) * 40 - 200, -25);
	glEnd();

	//circle((timecount / 16) * 50 - 200, -50, 15);

	//printf("%c", 13);
	if (timecount > 25)//150
		gamestate = MENU;

}
void displayMenu() {
	stickMan(-250, 170, 1, 0, 0, healthpoints);
	glColor3f(0.5, 0.2, 0.2);
	vprint2(-180, 120, 0.6, "STICKMAN");
	vprint2(-150, 40, 0.6, "SHOOTER");
	glColor3f(0, 0, 0);
	vprint(-123, -10, GLUT_BITMAP_HELVETICA_18, "Press \'Enter\' to start the game.");
	vprint(-75, -35, GLUT_BITMAP_9_BY_15, "Use Mouse to aim.");
	vprint(-125, -60, GLUT_BITMAP_9_BY_15, "Press Left Mouse Click to shoot.");
	vprint(-115, -85, GLUT_BITMAP_9_BY_15, "Press \'W\' and \'S\' to move.");
	vprint(-135, -110, GLUT_BITMAP_9_BY_15, "Press Arrow Keys to change speed.");
	vprint(-115, -135, GLUT_BITMAP_9_BY_15, "Press \'Spacebar\' to pause.");
	vprint(-135, -160, GLUT_BITMAP_9_BY_15, "Press \'F1\' to switch to menu.");
	vprint(-225, -185, GLUT_BITMAP_9_BY_15, "Press \'F2\' to decrease, \'F3\' to increase target health.");
	vprint(WINDOW_WIDTH / 4 + 25, 20, GLUT_BITMAP_HELVETICA_12, "Game Duration:");
	vprint(WINDOW_WIDTH / 4 + 20, -5, GLUT_BITMAP_HELVETICA_18, "%0.2d seconds", gametime);
	vprint(WINDOW_WIDTH / 4 - 85, -30, GLUT_BITMAP_HELVETICA_12, "Press \'F4\' to decrease, \'F5\' to increase game duration.");
	vprint(-WINDOW_WIDTH / 3 - 20, 20, GLUT_BITMAP_HELVETICA_12, "Target Amount:");
	vprint(-WINDOW_WIDTH / 3 - 20, -5, GLUT_BITMAP_HELVETICA_18, "%0.2d targets", targetamount);
	vprint(-WINDOW_WIDTH / 3 - 120, -30, GLUT_BITMAP_HELVETICA_12, "Press \'F6\' to decrease, \'F7\' to increase target amount.");

	glColor3f(0, 1, 0);
	for (i = 0; i < healthpoints; i++) {
		vprint2(5 + i * 20 - healthpoints * 10, -260, 0.5, "l");
	}
	glColor3f(0, 0, 0);
	vprint(-55, -290, GLUT_BITMAP_HELVETICA_12, "Target Healthpoints: ");
	vprint(-5, -315, GLUT_BITMAP_HELVETICA_18, "%-2d", healthpoints);

	glLineWidth(10);
	glBegin(GL_LINES);
	glVertex2f(-228, 160);
	glVertex2f(-210, 150);
	glVertex2f(-225, 160);
	glVertex2f(-205, 190);
	glEnd();
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
	for (initial = targetamount - 1; initial >= 0; initial--) {
		switch ((initial + 2) % 8) {
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
			stickmanReverse(*(posx + initial), *(posy + initial), r, g, b, *(health + initial));
		r = g = b = 0;
	}

	dispPlayer();

	for (i = 0; i <= bulletcount; i++) {
		if (*(shoot + i) == true)
			dispBullet(i);
		//printf("%d ", i);
	}

	//DEBUG
	//vprint(onmove.x - 15, onmove.y - 30, GLUT_BITMAP_9_BY_15, "Angle:%0.2f X: %d Y: %d", player.angle, onmove.x, onmove.y);
	//vprint(onmove.x - 15, onmove.y - 50, GLUT_BITMAP_9_BY_15, "pX: %d pY: %d", player.pos.x, player.pos.y);

	glColor3f(0.3, 0.3, 0.3);
	vprint(-80, 330, GLUT_BITMAP_HELVETICA_18, "%d Pixels Line Defense", WINDOW_WIDTH);
	vprint(0, 315, GLUT_BITMAP_HELVETICA_12, "by E. Berke KARAGOZ");
	ms = timecount - STARTING_TIME;

	if (timer == true)
		vprint(325, 335, GLUT_BITMAP_HELVETICA_12, "Press \'Spacebar\' to pause.");
	if (multiplier != 10)
		vprint(360, 300, GLUT_BITMAP_HELVETICA_10, "%0.1f Speed", multiplier / 10.);

	if (ms < 0) {
		sec = ms / 60;
		ms -= sec * 60;
		vprint(375, 315, GLUT_BITMAP_HELVETICA_18, "%0.2d", sec*-1);
		vprint(395, 315, GLUT_BITMAP_HELVETICA_12, ":%d", ms*-1);
	}
	else {
		sec = ms / 60;
		ms -= sec * 60;
		if (gametime - 1 - sec <= 3)
			glColor3f(1, 0, 0);
		vprint(375, 315, GLUT_BITMAP_HELVETICA_18, "%0.2d", gametime - 1 - sec);
		vprint(395, 315, GLUT_BITMAP_HELVETICA_12, ":%d", 60 - ms);
		glColor3f(0.3, 0.3, 0.3);
		vprint(-600, 335, GLUT_BITMAP_HELVETICA_12, "Score:");
		vprint(-590, 315, GLUT_BITMAP_HELVETICA_18, "%0.3d", score);

		vprint(-490, 335, GLUT_BITMAP_HELVETICA_12, "Total Hits:");
		vprint(-480, 315, GLUT_BITMAP_HELVETICA_18, "%0.3d", accurate);

		vprint(-370, 335, GLUT_BITMAP_HELVETICA_12, "Shots fired:");
		vprint(-340, 315, GLUT_BITMAP_HELVETICA_18, "%0.3d", bulletcount + 1);

		vprint(-260, 335, GLUT_BITMAP_HELVETICA_12, "Last hit was: (%d pts)", lasthit);
		if (lasthit == 5)
			vprint(-250, 315, GLUT_BITMAP_HELVETICA_18, "Headshot!");
		else if (lasthit == 2)
			vprint(-250, 315, GLUT_BITMAP_HELVETICA_18, "Bodyshot");
		else if (lasthit == 1)
			vprint(-250, 315, GLUT_BITMAP_HELVETICA_18, "Legshot");

		vprint(200, 335, GLUT_BITMAP_HELVETICA_12, "Closest target HP:");
		vprint(230, 315, GLUT_BITMAP_HELVETICA_18, "%0.2d", *(health + closest));

		if (gametime <= sec)
			gamestate = END;
	}

	if (timecount < STARTING_TIME + 45) {
		vprint(-30, 100, GLUT_BITMAP_HELVETICA_18, "Aim: Mouse");
		vprint(-50, 70, GLUT_BITMAP_HELVETICA_18, "Shoot: Left Click");
		vprint(-30, 40, GLUT_BITMAP_HELVETICA_18, "Move: W / S");
		if (timecount > STARTING_TIME - 25) {
			glColor3f(0, 0, 0);
			vprint2(-80, 150, 0.5, "START!");
		}
	}
	if (timer == false) {
		glColor3f(0.2, 0.2, 0.2);
		vprint2(-10, 150, 1, "ll");
		vprint(-65, 125, GLUT_BITMAP_HELVETICA_12, "Press Spacebar to continue.");
	}
}
void displayEnd() {
	glColor3f(0.6, 0.2, 0.2);
	vprint(-45, 75, GLUT_BITMAP_HELVETICA_18, "Your score is:");
	glColor3f(0.0, 1, 0.0);
	vprint2(-25, 0, 0.5, "%-3d", score);
	glColor3f(0.0, 0.0, 0.0);
	vprint(-70, -50, GLUT_BITMAP_HELVETICA_12, "Press \'Enter\' or \'F1\' to start again!");
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
	//initializing speed, acceleration, position, states, health, score, time
	for (initial = 0; initial < targetamount; initial++) {
		*(speed + initial) = 0.2;
		*(accel + initial) = (rand() % 30 + 30) / 175.0;
		printf("No %d: Accel: %0.4f Speed: %0.4f\n", initial, *(accel + i), *(speed + i));
		*(posx + initial) = WINDOW_WIDTH / 2 + 50 + rand() % 400;
		*(posy + initial) = -200 + initial * 550 / targetamount;
		*(touch + initial) = false;
		*(health + initial) = healthpoints;
	}
	score = timecount = accurate = lasthit = closest = 0;
	bulletcount = -1;
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
		startGame();
		gamestate = PLAY;
	}
	if (key == 13 && gamestate == END) {
		startGame();
		gamestate = PLAY;
	}
	if (key == 32 && gamestate == PLAY) {
		timer = !timer;
		printf("Timer = %d\n", timer);
	}
	if (key == 'w' && gamestate == PLAY && player.pos.y < WINDOW_HEIGHT / 2 - 50) {
		player.pos.y += 7;
	}
	if (key == 's' && gamestate == PLAY && player.pos.y > -WINDOW_HEIGHT / 2 + 115) {
		player.pos.y -= 7;
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

	if (key == GLUT_KEY_F1) {
		startGame();
		gamestate = MENU;
	}

	if (key == GLUT_KEY_F2 && gamestate == MENU && healthpoints > 1) {
		healthpoints--;
	}

	if (key == GLUT_KEY_F3 && gamestate == MENU) {
		healthpoints++;
	}
	if (key == GLUT_KEY_F4 && gamestate == MENU && gametime > 10) {
		gametime -= 5;
	}
	if (key == GLUT_KEY_F5 && gamestate == MENU) {
		gametime += 5;
	}
	if (key == GLUT_KEY_F6 && gamestate == MENU && targetamount > 1) {
		targetamount--;
	}
	if (key == GLUT_KEY_F7 && gamestate == MENU && targetamount < 50) {
		targetamount++;
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
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && gamestate == PLAY) {
		//if (bulletcount == 10)
		// bulletcount = 0;
		bulletcount++;
		bullets = (projectile_t *)malloc(sizeof(projectile_t) * (bulletcount + 1));
		shoot = (bool *)malloc(sizeof(bool) * (bulletcount + 1));
		*(bullets + bulletcount) = { { player.pos.x, player.pos.y },{ tri.x, tri.y } };
		printf("Shoot %d: %d -> ", bulletcount, *(shoot + bulletcount));
		*(shoot + bulletcount) = true;
		printf("%d\n", *(shoot + bulletcount));
	}

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

	glutTimerFunc(TIMER_PERIOD / multiplier, onTimer, 0);
	if (timer == true) {

		timecount++;
		for (i = 0; i <= bulletcount; i++) {
			if (*(shoot + i) == true) {
				(*(bullets + i)).pos.x += bulletspeed * ((*(bullets + i)).tri.x);
				(*(bullets + i)).pos.y += bulletspeed * ((*(bullets + i)).tri.y);
				hitBoxes();
				(*(bullets + i)).pos.x += bulletspeed * ((*(bullets + i)).tri.x);
				(*(bullets + i)).pos.y += bulletspeed * ((*(bullets + i)).tri.y);
				hitBoxes();
				(*(bullets + i)).pos.x += bulletspeed * ((*(bullets + i)).tri.x);
				(*(bullets + i)).pos.y += bulletspeed * ((*(bullets + i)).tri.y);
				//to make the bullets faster and make the hitboxes more consistent
			}
		}
		// Write your codes here.

		switch (gamestate) {
		case LOAD: break;
		case MENU: break;
		case PLAY:
			if (timecount >= STARTING_TIME)
				for (i = targetamount - 1; i >= 0; i--) {

					if (*(touch + i) == false) {
						// if (winWidth % winWidth / 20.0 == 0.0 || 0.2 + +*(accel + i));
						*(speed + i) += (*(accel + i) / 40.0);
						*(posx + i) -= *(speed + i);

					}
					else {
						if (*(speed + i) <= +0.2 + *(accel + i))
							*(speed + i) += *(accel + i) / 40.0;
						*(posx + i) -= *(speed + i);
					}
					if (*(posx + i) - 5000 < *(posx + closest) - 5000) {
						closest = i;
					}
					if (*(posx + i) <= -WINDOW_WIDTH / 2 - 50) {
						*(touch + i) = true;
						flag = true;
						//if (max < *(speed + i)) {
						// max = *(speed + i);
						// closest = i;
						//}
						*(accel + i) = (rand() % 30 + 30) / 100.0;
						printf("No %d: Accel: %0.4f aceel/20: %0.4f Speed: %0.4f\n", i, *(accel + i), *(accel + i) / 40.0, *(speed + i));

					}
					if (*(posx + i) <= -WINDOW_WIDTH / 2 - 50) {
						gamestate = END;
						timecount = 0;
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
#endif // SKETCH == 10