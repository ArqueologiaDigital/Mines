#include "main.h"
#include "video-tiles.h"
#include <string.h>
#include <time.h>
#include <GL/glut.h>
#include <math.h>

void init_lights(void) 
{
/*
NOTE: I am still a newbie on OpenGL coding and I'm not sure how to properly setup lights.
      This should be reviewed and fixed, as I know that colors are now showing up correctly yet.
*/

   GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
   GLfloat LightDiffuse[]= { 0.5f, 1.0f, 0.5f, 1.0f };
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
   glClearColor (0.05, 0.2, 0.02, 0.0);
   glShadeModel (GL_FLAT);


   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
//   glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);   
   glEnable(GL_COLOR_MATERIAL);
}

void draw_brick(int w, int h, int z)
{
	w*=1.5;
	h*=1.5;

	glColor3f(0.5, 0.5, 0.5);
	glPushMatrix();
	glTranslatef(-w/2, -h/2,  0);

//bottom:
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(w, 0, 0);
	glVertex3f(w, h, 0);
	glVertex3f(0, h, 0);
	glEnd();

//top:
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glVertex3f(z, z, z);
	glVertex3f(w-z, z, z);
	glVertex3f(w-z, h-z, z);
	glVertex3f(z, h-z, z);
	glEnd();

//front:
	glBegin(GL_QUADS);
	glNormal3d(0, 0.707, 0.707);
	glVertex3f(0, 0, 0);
	glVertex3f(w, 0, 0);
	glVertex3f(w-z, z, z);
	glVertex3f(z, z, z);
	glEnd();

//back:
	glBegin(GL_QUADS);
	glNormal3d(0, -0.707, 0.707);
	glVertex3f(0, h, 0);
	glVertex3f(w, h, 0);
	glVertex3f(w-z, h-z, z);
	glVertex3f(z, h-z, z);
	glEnd();

//left:
	glBegin(GL_QUADS);
	glNormal3d(-0.707, 0, 0.707);
	glVertex3f(0, 0, 0);
	glVertex3f(0, h, 0);
	glVertex3f(z, h-z, z);
	glVertex3f(z, z, z);
	glEnd();

//right:
	glBegin(GL_QUADS);
	glNormal3d(0.707, 0, 0.707);
	glVertex3f(w, 0, 0);
	glVertex3f(w, h, 0);
	glVertex3f(w-z, h-z, z);
	glVertex3f(w-z, z, z);
	glEnd();

	glPopMatrix();
}

void draw_minefield_border(int w, int h, int stems) {
	GLUquadricObj *q = gluNewQuadric();
	float th = w/10.0;
	int n = 2;
	glColor3f(0.5, 0.8, 0.8);

/*
	if (stems != 0b1010 && stems != 0b0101){
		glPushMatrix();
		glutSolidSphere(w/5.0, 25, 25);
		glPopMatrix();
	}
*/

	if (stems & 1){ //right:
		glPushMatrix();
		glRotatef(90, 0, 0, 1);
		glRotatef(-90, 0, 1, 0);
		gluCylinder(q, th, th, w/2, n, n);
		glPopMatrix();
	}

	if (stems & 2){ //down:
		glPushMatrix();
		glTranslatef(w/2, 0,  0);
		glRotatef(-90, 0, 1, 0);
		gluCylinder(q, th, th, h/2, n, n);
		glPopMatrix();
	}

	if (stems & 4){ //left:
		glPushMatrix();
		glTranslatef(0, h/2,  0);
		glRotatef(90, 0, 0, 1);
		glRotatef(-90, 0, 1, 0);
		gluCylinder(q, th, th, w/2, n, n);
		glPopMatrix();
	}

	if (stems & 8){ //up:
		glPushMatrix();
		glRotatef(-90, 0, 1, 0);
		gluCylinder(q, th, th, h/2, n, n);
		glPopMatrix();
	}
}

void draw_bomb(int w, int h){
	w*=0.7;

	GLUquadricObj *q = gluNewQuadric();
	int n=20;

	glColor3f(0.4, 0.3, 0.3);

	glPushMatrix();
	glutSolidSphere(w*0.75, 25, 25);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, w*0.7);
	gluCylinder(q, w/5.0, w/5.0, w*0.2, n, n);
	glPopMatrix();

	for (int i=0; i<4; i++){
		glPushMatrix();
		glRotatef(90*i, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		glTranslatef(0, 0, w*0.7);
		gluCylinder(q, w/5.0, w/5.0, w*0.2, n, n);
		glPopMatrix();
	}
}

void draw_explosion(int w, int h){
// TODO: At least change color.
//       Maybe draw something completely different.
	draw_bomb(w, h);
}
	
void draw_flag(int w, int h){
	draw_brick(w, h, 2);

	glPushMatrix();
	glTranslated(-w/2 + 0.2*w, -h/2, 0.3*w);
	glScalef(0.1, 0.1, 0.1);
	glColor4f(1.0, 0, 0, 1.0);
	glLineWidth(5.0);
	glutStrokeCharacter(GLUT_STROKE_ROMAN, 'F');
	glPopMatrix();
}

void draw_question_mark(int w, int h){
	draw_brick(w, h, 2);

	glPushMatrix();
	glTranslated(-w/2 + 0.2*w, -h/2, 0.3*w);
	glScalef(0.1, 0.1, 0.1);
	glColor4f(0.6, 0.6, 0.1, 1.0);
	glLineWidth(5.0);
	glutStrokeCharacter(GLUT_STROKE_ROMAN, '?');
	glPopMatrix();
}

void draw_cursor(int w, int h){
	GLUquadricObj *q = gluNewQuadric();
	int n=20;

	for (int i=0; i<4; i++){
		glPushMatrix();
		glTranslatef(0, 0, w/2);
		glRotatef(90*i, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		glTranslatef(0, 0, w*0.7);

		gluCylinder(q, w/10.0, w/10.0, w*0.8, n, n);
		glPopMatrix();
	}
}

void draw_number(int w, int h, int n, float r, float g, float b){
	glPushMatrix();
	glTranslated(-w/2 + 0.2*w, -h/2, 0);
	glScalef(0.1, 0.1, 0.1);
	glColor4f(r, g, b, 1.0);
	glLineWidth(5.0);
	glutStrokeCharacter(GLUT_STROKE_ROMAN, '1' + n-1);
	glPopMatrix();
}

/* set_tile emulates tile behaviour, but is actually drawing 3d models */
void set_tile(uint8_t dst_x, uint8_t dst_y, uint8_t tile)
{	
	glPushMatrix();
	glTranslatef(-250 + 12 * dst_x,  -120 + 12 * (25-dst_y),  0);

	switch(tile){
		case ONE_BOMB: draw_number(12, 12, 1, 0, 0, 1); break;
		case TWO_BOMBS: draw_number(12, 12, 2, 0, 1, 0); break;
		case THREE_BOMBS: draw_number(12, 12, 3, 0, 0, 1); break;;
		case FOUR_BOMBS: draw_number(12, 12, 4, 1, 1, 0); break;
		case FIVE_BOMBS:  draw_number(12, 12, 5, 1, 0, 1); break;
		case SIX_BOMBS: draw_number(12, 12, 6, 1, 1, 0); break;
		case SEVEN_BOMBS: draw_number(12, 12, 7, 1, 1, 1); break;
		case EIGHT_BOMBS: draw_number(12, 12, 8, 0.1, 0.1, 0.1); break;
		case BLANK: break;
		case CURSOR: draw_cursor(12, 12); break;

		case BOMB: draw_bomb(12, 12); break;
		case FLAG: draw_flag(12, 12); break;
		case QUESTION_MARK: draw_question_mark(12, 12); break;
		case EXPLOSION: draw_explosion(12, 12); break;
		case GROUND: break;

		case MINEFIELD_CORNER_TOP_LEFT: draw_minefield_border(12, 12, 0b0011); break;
		case MINEFIELD_TOP_TEE: draw_minefield_border(12, 12, 0b1011); break;
		case MINEFIELD_HORIZONTAL_TOP: draw_minefield_border(12, 12, 0b1010); break;
		case MINEFIELD_HORIZONTAL_MIDDLE: draw_minefield_border(12, 12, 0b1010); break;
		case MINEFIELD_HORIZONTAL_BOTTOM: draw_minefield_border(12, 12, 0b1010); break;
		case MINEFIELD_CORNER_TOP_RIGHT: draw_minefield_border(12, 12, 0b1001); break;

		case MINEFIELD_LEFT_TEE: draw_minefield_border(12, 12, 0b0111); break;
		case MINEFIELD_CROSS: draw_minefield_border(12, 12, 0b1111); break;
		case MINEFIELD_RIGHT_TEE: draw_minefield_border(12, 12, 0b1101); break;

		case MINEFIELD_VERTICAL_LEFT: draw_minefield_border(12, 12, 0b0101); break;
		case MINEFIELD_VERTICAL_MIDDLE: draw_minefield_border(12, 12, 0b0101); break;
		case MINEFIELD_VERTICAL_RIGHT: draw_minefield_border(12, 12, 0b0101); break;
		case CLOSED_CELL: draw_brick(12, 12, 3); break;

		case MINEFIELD_CORNER_BOTTOM_LEFT: draw_minefield_border(12, 12, 0b0110); break;
		case MINEFIELD_BOTTOM_TEE: draw_minefield_border(12, 12, 0b1110); break;
		case MINEFIELD_CORNER_BOTTOM_RIGHT: draw_minefield_border(12, 12, 0b1100); break;

		case CORNER_TOP_LEFT: break;
		case CORNER_TOP_RIGHT: break;
		case CORNER_BOTTOM_LEFT: break;
		case CORNER_BOTTOM_RIGHT: break;

		case TOP_BORDER__LEFT: break;
		case TOP_BORDER__RIGHT: break;
		case BOTTOM_BORDER__LEFT: break;
		case BOTTOM_BORDER__RIGHT: break;

		case LEFT_BORDER__TOP: break;
		case LEFT_BORDER__BOTTOM: break;
		case RIGHT_BORDER__TOP: break;
		case RIGHT_BORDER__BOTTOM: break;

		default:
			break;
	}

	glPopMatrix();
}

void highlight_cell(minefield* mf, int x, int y)
{
    if (mf->state == GAME_OVER)
        set_tile(x, y, EXPLOSION);
    else
        set_tile(x, y, CURSOR);
}

double minefield_angle = 0;

extern void gl_main_loop();

// Clears the current window and draws a triangle.
void display() {
	// Set every pixel in the frame buffer to the current clear color.
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	minefield_angle += 0.001;
	glRotatef(-60 + 5*sin(minefield_angle), 1, 0, 0);
	glRotatef(5*sin(minefield_angle+90), 0, 1, 0);
	gl_main_loop();
	glPopMatrix();

	// Flush drawing command buffer to make drawing happen as soon as possible.
	glFlush();
}

void idle_loop(minefield* mf)
{
}

void idle_func()
{
	glutPostRedisplay();
}

extern void keyboard_normal_event(unsigned char key, int x, int y);
extern void keyboard_special_event(int key, int x, int y);
void init_gl()
{
	int argc = 0;
	char ** argv = NULL;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutInitWindowPosition(80, 80);
	glutInitWindowSize(1280, 800);
	glutCreateWindow("Mines!");

	init_lights();

	// Especifica as dimensões da Viewport
	glViewport(0, 0, 1280, 800);

	// EspecificaParametrosVisualizacao:
	glMatrixMode(GL_PROJECTION);

	// Inicializa sistema de coordenadas de projeção
	glLoadIdentity();

	// Especifica a projeção perspectiva
	GLfloat fAspect = (GLfloat)1280 / (GLfloat)800;
	GLfloat angle = 60;
	gluPerspective(angle, fAspect, 0.1, 500);

	// Especifica sistema de coordenadas do modelo
	glMatrixMode(GL_MODELVIEW);
	// Inicializa sistema de coordenadas do modelo
	glLoadIdentity();

	// Especifica posição do observador e do alvo
	gluLookAt(0,80,200, 0,0,0, 0,1,0);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard_normal_event);
	glutSpecialFunc(keyboard_special_event);
	glutIdleFunc(idle_func);
}

void platform_init()
{
	// Init random number generator:
	srand(time(NULL));

	init_gl();
    draw_scenario();
}

void platform_shutdown()
{
   exit(0);
}
