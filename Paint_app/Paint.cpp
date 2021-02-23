//defining these for touch and activate 
#define NULL 0
#define LINE 1
#define RECTANGLE 2
#define TRIANGLE 3
#define POINTSS 4
#define TEXTS   5
#define CIRCLE 6
#define POLYGON 7

#define PI 3.1415926535898

#include <GL/glut.h>
#include <math.h>

float posx;
float posy;

int check;
int count;
int xp[2], yp[2];


GLsizei wh = 500, ww = 500; /* initial window size */
GLfloat size = 3.0;   /* half side length of square */
GLfloat lineWidth = 5.0;
int draw_mode = 0; /* drawing mode */
int rx, ry; /*raster position*/

GLfloat r = 1.0, g = 1.0, b = 1.0; /* drawing color */
int fill = 0; //This will help to monitor strip vs quad and stuff

bool isInTheRect(int x, int y, int left, int top, int right, int bottom);
void drawSquare(int x, int y);
float distance(float x1, float y1, float x2, float y2);
void drawCircle(void);
void myReshape(GLsizei w, GLsizei h);
void myinit();
void mouse(int btn, int state, int x, int y);
int pick(int x, int y);
void screen_box(int x, int y, int s);
void middle_menu(int id);
void color_menu(int id);
void pixel_menu(int id);
void fill_menu(int id);
void key(unsigned char k, int xx, int yy);
void display();

///////////////////////////////////////////

bool isInTheRect(int x, int y, int left, int top, int right, int bottom) {
	if (left <= x && x < right && bottom <= y && y < top)
		return true;
	else
		return false;
}

void drawSquare(int x, int y)
{
	y = wh - y;
	glColor3ub((char)rand() % 256, (char)rand() % 256, (char)rand() % 256);
	glBegin(GL_POLYGON);
	glVertex2f(x + size, y + size);
	glVertex2f(x - size, y + size);
	glVertex2f(x - size, y - size);
	glVertex2f(x + size, y - size);
	glEnd();
}


float distance(float x1, float y1, float x2, float y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

//stackoverflow !!
void drawCircle(void)
{
	const int resolution = 100;
	const float dtheta = 2.0 * PI / (float)resolution;
	float theta = 0.0;
	glPushMatrix();
	glTranslatef(posx, wh - posy, 0.0f);

	if (fill)
		glBegin(GL_POLYGON);
	else
		glBegin(GL_LINE_LOOP);
	for (int i = 0; i < resolution; i++) {
		theta += dtheta;

		const float x = r * cos(theta);
		const float y = r * sin(theta);

		glVertex2f(x, y);
	}

	glEnd();
	glPopMatrix();
}

//stackoverflow | resize 

void myReshape(GLsizei w, GLsizei h)
{

	/* adjust clipping box */

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)w, 0.0, (GLdouble)h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glViewport(0, 0, w, h);
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	display();
	glFlush();

	/* set global size for use by drawing routine */

	ww = w;
	wh = h;
}

void myinit()
{

	glViewport(0, 0, ww, wh);




	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)ww, 0.0, (GLdouble)wh, -1.0, 1.0);

	/* set clear color to black and clear window */

	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

//Most Important module 
void mouse(int btn, int state, int x, int y)
{
	static int count;
	int where;
	static int xp[2], yp[2];

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		where = pick(x, y);
		glColor3f(r, g, b);
		if (where != 0)
		{
			count = 0;
			draw_mode = where;
		}
		else switch (draw_mode)
		{
		case(LINE):
			if (count == 0)
			{
				count++;
				xp[0] = x;
				yp[0] = y;
			}
			else
			{
				glBegin(GL_LINES);
				glVertex2i(x, wh - y);
				glVertex2i(xp[0], wh - yp[0]);
				glEnd();
				draw_mode = 0;
				count = 0;
			}
			break;
		case(RECTANGLE):
			if (count == 0)
			{
				count++;
				xp[0] = x;
				yp[0] = y;
			}
			else
			{
				if (fill) glBegin(GL_POLYGON);
				else glBegin(GL_LINE_LOOP);
				glVertex2i(x, wh - y);
				glVertex2i(x, wh - yp[0]);
				glVertex2i(xp[0], wh - yp[0]);
				glVertex2i(xp[0], wh - y);
				glEnd();
				draw_mode = 0;
				count = 0;
			}
			break;
		case (TRIANGLE):
			switch (count)
			{
			case(0):
				count++;
				xp[0] = x;
				yp[0] = y;
				break;
			case(1):
				count++;
				xp[1] = x;
				yp[1] = y;
				break;
			case(2):
				if (fill) glBegin(GL_POLYGON);
				else glBegin(GL_LINE_LOOP);
				glVertex2i(xp[0], wh - yp[0]);
				glVertex2i(xp[1], wh - yp[1]);
				glVertex2i(x, wh - y);
				glEnd();
				draw_mode = 0;
				count = 0;
			}
			break;
		case(POINTSS):
		{
			drawSquare(x, y);
			count++;
		}
		break;
		case(CIRCLE):
			if (count == 0)
			{
				posx = x;
				posy = y;
				check = 1;
				count++;
			}
			else
			{
				r = distance(posx, posy, x, y);
				check = 2;

				count = 0;
				draw_mode = 0;

				drawCircle();
			}
			break;
		case(TEXTS):
		{
			rx = x;
			ry = wh - y;
			glRasterPos2i(rx, ry);
			count = 0;
		}
		case(POLYGON):
		{
			if (count == 0)
			{
				count++;
				xp[0] = x;
				yp[0] = y;
			}
			else
			{


				glBegin(GL_LINE_STRIP);

				glVertex2i(x, wh - y);
				glVertex2i(xp[0], wh - yp[0]);

				xp[0] = x;
				yp[0] = y;

				glEnd();

			}
		}
		}
		glPopAttrib();
		glFlush();
	}

	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		where = pick(x, y);
		glColor3f(r, g, b);

		switch (draw_mode)
		{
		case(POLYGON):
		{
			if (count == 0)
			{
				count++;
				xp[0] = x;
				yp[0] = y;
			}
			else
			{
				glBegin(GL_LINE_STRIP);

				glVertex2i(x, wh - y);
				glVertex2i(xp[0], wh - yp[0]);

				xp[0] = x;
				yp[0] = y;

				glEnd();

			}
		}
		glBegin(GL_LINE_STRIP);

		glVertex2i(x, wh - y);
		glVertex2i(xp[0], wh - yp[0]);


		xp[0] = x;
		yp[0] = y;

		glEnd();

		count = 0;
		draw_mode = 0;

		}
	}
}

//screenpickup! 
int pick(int x, int y)
{
	y = wh - y;
	if (x < ww - ww / 10) return 0;
	else if (y > wh - ww / 10) return LINE;
	else if (y > wh - ww / 5) return RECTANGLE;
	else if (y > wh - (3 * ww / 10)) return TRIANGLE;
	else if (y > wh - (2 * ww / 5)) return POINTSS;
	else if (y > wh - (ww / 2)) return TEXTS;
	else if (y > wh - (6 * ww / 10)) return CIRCLE;
	else if (y > wh - (7 * ww / 10)) return POLYGON;
	else return 0;
}

void screen_box(int x, int y, int s)
{
	glBegin(GL_QUADS);
	glVertex2i(x, y);
	glVertex2i(x + s, y);
	glVertex2i(x + s, y + s);
	glVertex2i(x, y + s);
	glEnd();
}

void middle_menu(int id)
{
}

//dropdown color 
void color_menu(int id)
{
	if (id == 1) { r = 1.0; g = 0.0; b = 0.0; }
	else if (id == 2) { r = 0.0f; g = 1.0f; b = 0.0f; }
	else if (id == 3) { r = 0.0; g = 0.0; b = 1.0; }
	else if (id == 4) { r = 0.0; g = 1.0; b = 1.0; }
	else if (id == 5) { r = 1.0; g = 0.0; b = 1.0; }
	else if (id == 6) { r = 1.0; g = 1.0; b = 0.0; }
	else if (id == 7) { r = 1.0; g = 1.0; b = 1.0; }
	else if (id == 8) { r = 0.0; g = 0.0; b = 0.0; }
}

void quitclear_menu(int id)
{
	if (id == 1)
		exit(0);
	else
	{
		glClear(GL_COLOR_BUFFER_BIT);
		display();
		glFlush();
	}
}
void pixel_menu(int id)
{
	if (id == 1) size = 2 * size;
	else if (size > 1) size = size / 2;
}

void fill_menu(int id)
{
	if (id == 1) fill = 1;
	else fill = 0;
}

void key(unsigned char k, int xx, int yy)
{
	if (draw_mode != TEXTS) return;
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2i(rx, ry);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, k);
	rx += glutBitmapWidth(GLUT_BITMAP_9_BY_15, k);

}

//Display function||Modified Function 
void display()
{
	int shift = 0;

	double mx = ww - ww / 20;
	double my = wh - (ww / 10 * 5.5);
	double x[360], y[360];
	double rad = 10;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glColor3f(1.0, 1.0, 1.0);
	screen_box(ww - ww / 10, wh - ww / 10, ww / 10);
	glColor3f(1.0, 0.0, 0.0);
	screen_box(ww - ww / 10, wh - (ww / 10 * 2), ww / 10);
	glColor3f(0.0, 1.0, 0.0);
	screen_box(ww - ww / 10, wh - (ww / 10 * 3), ww / 10);
	glColor3f(0.0, 0.0, 1.0);
	screen_box(ww - ww / 10, wh - (ww / 10 * 4), ww / 10);
	glColor3f(1.0, 1.0, 0.0);
	screen_box(ww - ww / 10, wh - (ww / 10 * 5), ww / 10);
	glColor3f(0.0, 1.0, 1.0);
	screen_box(ww - ww / 10, wh - (ww / 10 * 6), ww / 10);
	glColor3f(1.0, 0.0, 1.0);
	screen_box(ww - ww / 10, wh - (ww / 10 * 7), ww / 10);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2i(ww - (ww / 15), wh - ww / 20);
	glVertex2i(ww - (ww / 40), wh - ww / 20);
	glEnd();
	screen_box(ww - (ww / 40) * 3, wh - (ww / 10 * 1.8), ww / 20);
	glBegin(GL_TRIANGLES);
	glVertex2i(ww - ww / 20, wh - (ww / 10 * 2.3));
	glVertex2i(ww - (ww / 40) * 3, wh - (ww / 10 * 2.8));
	glVertex2i(ww - ww / 40, wh - (ww / 10 * 2.8));
	glEnd();
	glPointSize(3.0);
	glBegin(GL_POINTS);
	glVertex2i(ww - ww / 20, wh - (ww / 10 * 3.5));
	glEnd();
	glRasterPos2i(ww - ww / 10, wh - (ww / 10 * 4.7));
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'A');
	shift = glutBitmapWidth(GLUT_BITMAP_9_BY_15, 'A');
	glRasterPos2i(ww - ww / 10 + shift, wh - (ww / 10 * 4.7));
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'B');
	shift += glutBitmapWidth(GLUT_BITMAP_9_BY_15, 'B');
	glRasterPos2i(ww - ww / 10 + shift, wh - (ww / 10 * 4.7));
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'C');

	glBegin(GL_POLYGON);
	r = ww / 35;
	float i;
	double pi = 3.141592;
	static int xp[1], yp[1];
	for (i = 0; i < 2 * pi; i += 0.1)
	{
		xp[0] = r * cos(i);
		yp[0] = r * sin(i);
		glVertex2i(ww - ww / 10 + ww / 20 + xp[0], wh - ww / 10 * 6 + ww / 20 + yp[0]);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2i(ww - ww / 20, wh - (ww / 10 * 6.2));
	glVertex2i(ww - ww / (40 * 1.5), wh - (ww / 10 * 6.3));
	glVertex2i(ww - ww / (40 * 1.5), wh - (ww / 10 * 6.6));
	glVertex2i(ww - ww / 20, wh - (ww / 10 * 6.8));
	glVertex2i(ww - (ww / 40) * 3.5, wh - (ww / 10 * 6.6));
	glVertex2i(ww - (ww / 40) * 3.5, wh - (ww / 10 * 6.3));
	glEnd();

	glFlush();
	glPopAttrib();
}


//The maim Function
int main(int argc, char** argv)
{
	int c_menu, p_menu, f_menu, qc_menu;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("square");
	glutDisplayFunc(display);

	c_menu = glutCreateMenu(color_menu);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Cyan", 4);
	glutAddMenuEntry("Magenta", 5);
	glutAddMenuEntry("Yellow", 6);
	glutAddMenuEntry("White", 7);
	glutAddMenuEntry("Black", 8);

	p_menu = glutCreateMenu(pixel_menu);
	glutAddMenuEntry("increase pixel size", 1);
	glutAddMenuEntry("decrease pixel size", 2);

	f_menu = glutCreateMenu(fill_menu);
	glutAddMenuEntry("fill on", 1);
	glutAddMenuEntry("fill off", 2);

	qc_menu = glutCreateMenu(quitclear_menu);
	glutAddMenuEntry("quit", 1);
	glutAddMenuEntry("clear", 2);

	glutCreateMenu(middle_menu);
	glutAddSubMenu("Colors", c_menu);
	glutAddSubMenu("Pixel Size", p_menu);
	glutAddSubMenu("Fill", f_menu);
	glutAddSubMenu("Quit/Clear", qc_menu);

	//the middle button activated menubar
	glutAttachMenu(GLUT_MIDDLE_BUTTON);

	myinit();
	glutReshapeFunc(myReshape);
	glutKeyboardFunc(key);
	glutMouseFunc(mouse);
	glutMainLoop();
}
