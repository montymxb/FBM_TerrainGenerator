//
// Final Project for CS 557
// by Benjamin Friedman
// based on the template OpenGL code provided by Prof. Bailey in CS 550
//
// final_project.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32

// Detect Windows
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"


#elif __APPLE__

// Detect Mac
#include <time.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#elif

// Detect Linux
//#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

#endif

#include "glslprogram.h"


// title of these windows:

const char *WINDOWTITLE = { "CS 557 Final Project" };

// the escape key:
#define ESCAPE_KEY		0x1b


// initial window size:
const int INIT_WINDOW_SIZE = 600;

// multiplication factors for input interaction:
//  (these are known from previous experience)
const float ANGFACT = 1.0;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:
const float MINSCALE = 0.05f;

// active mouse buttons (or them together):
const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

// which projection:
enum Projections
{
	ORTHO,
	PERSP
};

// which button:
enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):
// full black
const GLfloat BACKCOLOR[] = {0.0, 0.0, 0.0, 1.0};

// line width for the axes:
const GLfloat AXES_WIDTH = 3.0;


// the color numbers:
// this order must match the radio button order
enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};


// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:
const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };


// non-constant global variables:
int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn = 1;	// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to use the z-buffer


/* Final Project Components */
GLSLProgram *LandscapePatt;
GLSLProgram *WaterPatt;
GLSLProgram *SkyPatt;
GLSLProgram *SkyVolPatt;

float programSeed;
GLuint testSheetId;
GLuint denseSheetId;
GLuint boxList;

float LightX;
float LightY;
float LightZ;

//
// Control Variables
//
int CloudOctaves = 8;
int LandOctaves = 8;
int WaterOctaves = 8;
float WaterHeight = 0.0f;
bool EnableClouds = false;
bool EnableSunRotation = false;
bool EnableCloudVolume = true;
bool EnableLand = false;
bool EnableWater = false;
bool EnableShading = false;
bool EnableFluidLand = false;
//
//
//


float *Array3(float r, float g, float b);
float *MulArray3(float factor, float array0[3]);
void setup_point_light(int lightId, float x, float y, float z, float r, float g, float b);
void setup_spot_light(int lightId, float x, float y, float z, float xDir, float yDir, float zDir, float r, float g, float b);
void put_light_sphere_at(float x, float y, float z, float r, float g, float b);
void set_material(float r, float g, float b, float shininess);

int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu(int);
float	ElapsedSeconds();
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );

// main program:

int main( int argc, char *argv[ ] ) {
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)
	srand(time(NULL));
	glutInit(&argc, argv);
	//glutInitContextVersion(3, 2);
	//glutInitContextProfile(GLUT_CORE_PROFILE);

	// set program seed
	programSeed = (rand() % 1000000 + 1) / 100000;

	LightX = 1.0f;
	LightY = 20.0f;
	LightZ = 20.0f;

	// setup all the graphics stuff:
	InitGraphics( );

	const GLubyte *str = glGetString(GL_VERSION);
	printf("OpenGL Version: %s\n", str);

	printf("::Commands::\n");
	printf("q/esc:\tquit\n");
	printf("f:\ttoggle freeze\n");
	printf("1-9:\tset octaves to #\n");
	printf("c:\ttoggle cloud 2D\n");
	printf("s:\ttoggle sun rotation\n");
	printf("w:\ttoggle water\n");
	printf("e:\ttoggle land\n");
	printf("m:\ttoggle cloud volume\n");
	printf("d:\ttoggle cloud shadows\n");
	printf("t:\ttoggle flowing land\n");
	printf("=:\tincrease water height\n");
	printf("-:\tdecrease water height\n");
	printf("option + click + move:\tzoom\n");

	// create the display structures that will not change:
	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay
	Reset( );

	// setup all the user interface stuff:
	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)
	glutSetWindow( MainWindow );
	glutMainLoop( );
	return 0;

}


// global time code
float ActualTime;
float Time;
float LongTime;
float LongLongTime;
#define MS_IN_THE_ANIMATION_CYCLE    3300 // every 3.3 seconds
#define MS_IN_THE_LONG_ANIMATION_CYCLE 10160 // every 10.16 seconds (double beat
#define MS_IN_THE_LONG_LONG_ANIMATION_CYCLE 40640

// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it
void Animate() {
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	// full time
	ActualTime = glutGet(GLUT_ELAPSED_TIME) * 0.005;

  // Time
  int ms = glutGet( GLUT_ELAPSED_TIME );    // milliseconds
  ms  %=  MS_IN_THE_ANIMATION_CYCLE;
  Time = (float)ms  /  (float)MS_IN_THE_ANIMATION_CYCLE;        // [ 0., 1. )

  // LongTime
  ms = glutGet( GLUT_ELAPSED_TIME );
  ms  %=  MS_IN_THE_LONG_ANIMATION_CYCLE;
  LongTime = (float)ms  /  (float)MS_IN_THE_LONG_ANIMATION_CYCLE;

	// LongLongTime
	ms = glutGet( GLUT_ELAPSED_TIME );
  ms  %=  MS_IN_THE_LONG_LONG_ANIMATION_CYCLE;
  LongLongTime = (float)ms  /  (float)MS_IN_THE_LONG_LONG_ANIMATION_CYCLE;

	// force a call to Display( ) next time it is convenient:

	// check to enable sun rotation
	if(EnableSunRotation) {
		LightY = cos(Time * 2.0 * 3.14159) * 20.0f;
		LightZ = sin(Time * 2.0 * 3.14159) * 20.0f;

	} else {
		// TODO kind of an odd setup
		// lighting for cloud effects
		//printf("Adjusting...\n");
		LightY = cos(370 * 2.0 * 3.14159) * 20.0f;
		LightZ = sin(370 * 2.0 * 3.14159) * 20.0f;

	}

	glutSetWindow(MainWindow);
	glutPostRedisplay( );
}

// generates a basic box
void generateBox() {
	boxList = glGenLists(1);
	glNewList(boxList, GL_COMPILE);

	// set flat shading
	glShadeModel(GL_SMOOTH);

	// going to do object (and thus normal) scaling
	glEnable(GL_NORMALIZE);

	set_material(0.5, 0.5, 0.5, 10.0);

	// bottom
	glBegin(GL_POLYGON);
	glNormal3f(0.0,0.0,-1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(  0.5, -0.5, -0.5 );
	glTexCoord2f(1.0, 0.0);
	glVertex3f(  0.5,  0.5, -0.5 );
	glTexCoord2f(1.0, 1.0);
	glVertex3f( -0.5,  0.5, -0.5 );
	glTexCoord2f(0.0, 1.0);
	glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();

	// top
	glBegin(GL_POLYGON);
	glNormal3f(0.0,0.0,1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(  0.5, -0.5, 0.5 );
	glTexCoord2f(1.0, 0.0);
	glVertex3f(  0.5,  0.5, 0.5 );
	glTexCoord2f(1.0, 1.0);
	glVertex3f( -0.5,  0.5, 0.5 );
	glTexCoord2f(0.0, 1.0);
	glVertex3f( -0.5, -0.5, 0.5 );
	glEnd();

	// right
	glBegin(GL_POLYGON);
	glNormal3f(1.0,0.0,0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f( 0.5, -0.5, -0.5 );
	glTexCoord2f(1.0, 0.0);
	glVertex3f( 0.5,  0.5, -0.5 );
	glTexCoord2f(1.0, 1.0);
	glVertex3f( 0.5,  0.5,  0.5 );
	glTexCoord2f(0.0, 1.0);
	glVertex3f( 0.5, -0.5,  0.5 );
	glEnd();

	// left
	glBegin(GL_POLYGON);
	glNormal3f(-1.0,0.0,1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f( -0.5, -0.5,  0.5 );
	glTexCoord2f(1.0, 0.0);
	glVertex3f( -0.5,  0.5,  0.5 );
	glTexCoord2f(1.0, 1.0);
	glVertex3f( -0.5,  0.5, -0.5 );
	glTexCoord2f(0.0, 1.0);
	glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();

	// front
	glBegin(GL_POLYGON);
	glNormal3f(0.0,1.0,0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(  0.5,  0.5,  0.5 );
	glTexCoord2f(1.0, 0.0);
	glVertex3f(  0.5,  0.5, -0.5 );
	glTexCoord2f(1.0, 1.0);
	glVertex3f( -0.5,  0.5, -0.5 );
	glTexCoord2f(0.0, 1.0);
	glVertex3f( -0.5,  0.5,  0.5 );
	glEnd();

	// back
	glBegin(GL_POLYGON);
	glNormal3f(0.0,-1.0,0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(  0.5, -0.5, -0.5 );
	glTexCoord2f(1.0, 0.0);
	glVertex3f(  0.5, -0.5,  0.5 );
	glTexCoord2f(1.0, 1.0);
	glVertex3f( -0.5, -0.5,  0.5 );
	glTexCoord2f(0.0, 1.0);
	glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();

	glEndList();
}


// simple test sheet
void generateSimpleTestSheet() {
	testSheetId = glGenLists(1);
	glNewList(testSheetId, GL_COMPILE);

	glBegin(GL_TRIANGLE_STRIP);

	// set flat shading
	glShadeModel(GL_SMOOTH);

	// setup normal for reflections
	glNormal3f(0.0,0.0,1.0);

	// set material characteristics, shiny
	set_material(1.0, 1.0, 1.0, 10.0);

	// going to do object (and thus normal) scaling
	glEnable(GL_NORMALIZE);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.0, 1.0, 0.0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0, 1.0, 0.0);

	glEnd();

	glEndList();

}


// 'heavy' test sheet
// used for high density quad stuff
void generateDenseSheet() {
	denseSheetId = glGenLists(1);
	glNewList(denseSheetId, GL_COMPILE);

	// set flat shading
	glShadeModel(GL_SMOOTH);

	// setup normal for reflections
	glNormal3f(0.0,1.0,0.0);

	// set material characteristics, shiny
	set_material(1.0, 1.0, 1.0, 10.0);

	// going to do object (and thus normal) scaling
	glEnable(GL_NORMALIZE);

	// draw plate made of pieces
	float xPos = 0.0;
	float zPos = 0.0;
	float definition = 50;
	float increment = 1.00 / definition;

	for(int x = 0; x < definition; x++) {
		glBegin(GL_TRIANGLE_STRIP);

		glTexCoord2f(xPos, zPos);
		glVertex3f(xPos, 0.0, zPos);

		glTexCoord2f(xPos, zPos+increment);
		glVertex3f(xPos, 0.0, zPos+increment);
		xPos+=increment;

		for(int y = 0; y < definition; y++) {
		//for(int y = 0; y < definition; y++) {

			glTexCoord2f(xPos+increment, zPos);
			glVertex3f(xPos+=(increment), 0.0, zPos);

			glTexCoord2f(xPos, zPos+(increment));
			glVertex3f(xPos, 0.0, zPos+(increment));
			xPos+=(increment);
		}

		glEnd();

		xPos = 0.0;
		zPos+=(increment);
	}

	glEndList();

}

// draws a landscape sheet
void drawLandscape() {

	// use the landscape patt
	LandscapePatt->Use();
	// used to seed fbm distribution
	LandscapePatt->SetUniformVariable("uSeed", programSeed);
	// used as extremely slow time tick for flowing effects
	LandscapePatt->SetUniformVariable("uSlowTime", ActualTime * 0.1f);
	// standard repeating time from 0 - 1
	LandscapePatt->SetUniformVariable("uTime", Time);
	LandscapePatt->SetUniformVariable("uAmbient", 0.3f);
	LandscapePatt->SetUniformVariable("uDiffuse", 0.5f);
	LandscapePatt->SetUniformVariable("uSpecular", 0.05f);
	LandscapePatt->SetUniformVariable("Shininess", 1.0f);
	LandscapePatt->SetUniformVariable("LightX", LightX);
	LandscapePatt->SetUniformVariable("LightY", LightY);
	LandscapePatt->SetUniformVariable("LightZ", LightZ);
	LandscapePatt->SetUniformVariable("SpecularColor", 1.0f, 1.0f, 1.0f);
	LandscapePatt->SetUniformVariable("uOctaves", LandOctaves);
	LandscapePatt->SetUniformVariable("uCloudOctaves", CloudOctaves);
	LandscapePatt->SetUniformVariable("uCloudsEnabled", EnableClouds);
	LandscapePatt->SetUniformVariable("uShadingEnabled", EnableShading);
	LandscapePatt->SetUniformVariable("uFluidLandEnabled", EnableFluidLand);

	glPushMatrix();
	//glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(1.0/2.0, 1.0, 1.0);
	//glTranslatef(0,0.05,0.0);
	glCallList(denseSheetId);
	glPopMatrix();

	// no patt
	LandscapePatt->Use(0);

}


// draws a cloud volume
void drawCloudVolume() {
	// use the landscape patt
	SkyVolPatt->Use();
	// used to seed fbm distribution
	SkyVolPatt->SetUniformVariable("uSeed", programSeed * 2.0f);
	// used as extremely slow time tick for flowing effects
	SkyVolPatt->SetUniformVariable("uSlowTime", ActualTime * 0.1f);
	// standard repeating time from 0 - 1
	SkyVolPatt->SetUniformVariable("uTime", Time);
	SkyVolPatt->SetUniformVariable("uAmbient", 0.1f);
	SkyVolPatt->SetUniformVariable("uDiffuse", 0.6f);
	SkyVolPatt->SetUniformVariable("uSpecular", 1.0f);
	SkyVolPatt->SetUniformVariable("Shininess", 50.0f);
	SkyVolPatt->SetUniformVariable("LightX", LightX);
	SkyVolPatt->SetUniformVariable("LightY", LightY);
	SkyVolPatt->SetUniformVariable("LightZ", LightZ);
	SkyVolPatt->SetUniformVariable("SpecularColor", 1.0f, 1.0f, 1.0f);
	SkyVolPatt->SetUniformVariable("uOctaves", CloudOctaves);
	SkyVolPatt->SetUniformVariable("uVolumeStart", -0.5f, -0.5f, -0.5f);
	SkyVolPatt->SetUniformVariable("uVolumeDimens", 1.0f, 1.0f, 1.0f);
	SkyVolPatt->SetUniformVariable("uEyeAt", 1.0, 1.0, 1.0);

	glPushMatrix();
	//glRotatef(360.0 * LongTime, 1.0, 0.0, 0.0);
	//glScalef(10.0, 10.0, 1.0);
	glTranslatef(0.0,0.5,0.0);
	glCallList(boxList);
	glPopMatrix();

	// no patt
	SkyVolPatt->Use(0);

}


// draws the seascape component
void drawWater() {
	// use the landscape patt
	WaterPatt->Use();
	// used to seed fbm distribution
	WaterPatt->SetUniformVariable("uSeed", programSeed * 2.0f);
	// used as extremely slow time tick for flowing effects
	WaterPatt->SetUniformVariable("uSlowTime", ActualTime * 0.01f);
	// standard repeating time from 0 - 1
	WaterPatt->SetUniformVariable("uTime", Time);
	WaterPatt->SetUniformVariable("uAmbient", 0.1f);
	WaterPatt->SetUniformVariable("uDiffuse", 0.6f);
	WaterPatt->SetUniformVariable("uSpecular", 1.0f);
	WaterPatt->SetUniformVariable("Shininess", 50.0f);
	WaterPatt->SetUniformVariable("LightX", LightX);
	WaterPatt->SetUniformVariable("LightY", LightY);
	WaterPatt->SetUniformVariable("LightZ", LightZ);
	WaterPatt->SetUniformVariable("SpecularColor", 1.0f, 1.0f, 1.0f);
	WaterPatt->SetUniformVariable("uOctaves", LandOctaves);
	WaterPatt->SetUniformVariable("uCloudOctaves", CloudOctaves);
	WaterPatt->SetUniformVariable("uCloudsEnabled", EnableClouds);
	WaterPatt->SetUniformVariable("uShadingEnabled", EnableShading);
	WaterPatt->SetUniformVariable("uWaterHeight", WaterHeight);

	glPushMatrix();
	//glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(1.0/2.0, 1.0, 1.0);
	//glTranslatef(0,0.05,0.0);
	glCallList(denseSheetId);
	glPopMatrix();

	// no patt
	WaterPatt->Use(0);

}

void drawClouds() {
	// use the landscape patt
	SkyPatt->Use();
	// used to seed fbm distribution
	SkyPatt->SetUniformVariable("uSeed", programSeed * 2.0f);
	// used as extremely slow time tick for flowing effects
	SkyPatt->SetUniformVariable("uSlowTime", ActualTime * 0.01f);
	// standard repeating time from 0 - 1
	SkyPatt->SetUniformVariable("uTime", Time);
	SkyPatt->SetUniformVariable("uAmbient", 0.1f);
	SkyPatt->SetUniformVariable("uDiffuse", 0.6f);
	SkyPatt->SetUniformVariable("uSpecular", 1.0f);
	SkyPatt->SetUniformVariable("Shininess", 50.0f);
	SkyPatt->SetUniformVariable("LightX", LightX);
	SkyPatt->SetUniformVariable("LightY", LightY);
	SkyPatt->SetUniformVariable("LightZ", LightZ);
	SkyPatt->SetUniformVariable("SpecularColor", 1.0f, 1.0f, 1.0f);
	SkyPatt->SetUniformVariable("uOctaves", CloudOctaves);

	glPushMatrix();
	//glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(1.0/2.0, 1.0, 1.0);
	glTranslatef(0.0,0.7,0.0);
	glCallList(denseSheetId);
	glPopMatrix();

	// no patt
	SkyPatt->Use(0);

}


void drawSun() {
	glPushMatrix();
	glTranslatef(LightX, LightY, LightZ);
	glCallList(testSheetId);
	glPopMatrix();
}


// draw the complete scene:
void Display() {
	if(DebugOn != 0) {
		fprintf( stderr, "Display\n" );
	}

	// set which window we want to do the graphics into:
	glutSetWindow( MainWindow );

	// erase the background:
	glDrawBuffer( GL_BACK );
	// GL_ONE_MINUS_DST_ALPHA
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if( DepthBufferOn != 0 ) {
		glEnable( GL_DEPTH_TEST );
	} else {
		glDisable( GL_DEPTH_TEST );
	}

	// can do smooth shading as well???
	// specify shading to be SMOOTH:
	glShadeModel(GL_FLAT); // GL_FLAT

	// set the viewport to a square centered in the window:
	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	// viewport fix?
	glViewport(xl, yb, v * 2, v * 2);

	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO ) {
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	} else {
		gluPerspective( 90., 1.,	0.1, 1000. );
	}

	// place the objects into the scene:
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// set the eye position, look-at position, and up-vector:
	gluLookAt(
		1.5, 2.2, 1.5,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0
	);

	// rotate the scene
	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );

	// uniformly scale the scene:
	if( Scale < MINSCALE ) {
			Scale = MINSCALE;
	}
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

	// set the fog parameters:
	if(DepthCueOn != 0) {
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	} else {
		glDisable( GL_FOG );
	}

	// possibly draw the axes:
	if(AxesOn != 0) {
		set_material(1.0,1.0,1.0,5.0);
		glColor3fv( &Colors[WhichColor][0] );
		glPushMatrix();
		glTranslatef(0.0,0.0,0.0);
		glCallList( AxesList );
		glPopMatrix();

	}

	//
	// Final Project Drawing Code
	//

	glEnable(GL_LIGHTING);

	glPushMatrix();
	//glScalef(1.0, 1.0, 1.0);

	glPushMatrix();
	glTranslatef(-0.5, -0.5, -0.5);

	/**/
	if(EnableLand) {
		drawLandscape();
	}
	if(EnableWater) {
		drawWater();
	}
	if(EnableClouds) {
		drawClouds();
	}
	/**/

	// draws the sun
	drawSun();

	glPopMatrix();

	// box test
	if(EnableCloudVolume) {
		drawCloudVolume();
	}

	glPopMatrix();

	glDisable(GL_LIGHTING);

	// swap the double-buffered framebuffers:
	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !
	glFlush( );
}


// toggling view mode for the helicopter
void DoViewMenu(int id) {
		#ifdef ASSIGNMENT_2
    view_mode = id;
		#endif

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


void DoAxesMenu( int id ) {
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void DoColorMenu( int id ) {
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void DoDebugMenu( int id ) {
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void DoDepthBufferMenu( int id ) {
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void DoDepthFightingMenu( int id ) {
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void DoDepthMenu( int id ) {
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:
void DoMainMenu(int id) {
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay();

}


void DoProjectMenu(int id) {
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// return the number of seconds since the start of the program:
float ElapsedSeconds() {
	// get # of milliseconds since the start of the program:
	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:
	return (float)ms / 1000.f;

}


// initialize the glui window:
void InitMenus() {

	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );

	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

	// attach the pop-up menu to the right mouse button:
	glutAttachMenu(GLUT_RIGHT_BUTTON);

}


// Loads a shader and returns on success
// otherwise indicates errors and stops
GLSLProgram* loadShader(char *vertShader, char *fragShader) {
	// Load fragment/vertex shader
	bool valid;
	GLSLProgram *Pattern = new GLSLProgram();
	valid = Pattern->Create(vertShader,fragShader);
	if(!valid) {
		// bad shader
		fprintf(stderr, "Shader cannot be created!\n");
		DoMainMenu(QUIT);

	} else {
		// no issues
		fprintf( stderr, "Shader created.\n" );

	}
	Pattern->SetVerbose(false);
	return Pattern;
}


// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions
void InitGraphics() {
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:
	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// Load fragment/vertex shaders
	LandscapePatt = loadShader("landscape.vert", "landscape.frag");
	WaterPatt 		= loadShader("water.vert", "water.frag");
	SkyPatt 			= loadShader("sky.vert","sky.frag");
	SkyVolPatt 		= loadShader("skyvol.vert", "skyvol.frag");

}

float *White = Array3(1.,1.,1.);

// generates 4 elm array from 3 elm array
float *MulArray3(float factor, float array0[3]) {
	static float ra[4];
	ra[0] = factor * array0[0];
	ra[1] = factor * array0[1];
	ra[2] = factor * array0[2];
	ra[3] = 1.;
	return ra;
}

// sets material characteristics for lighting purposes
void set_material(float r, float g, float b, float shininess) {
	// set back material (quite dull)
	glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0.2, 0.2, 0.2));
	glMaterialf (GL_BACK, GL_SHININESS, 2.f);

	// set front material, normal colors, and shininess
	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, White));
	glMaterialf (GL_FRONT, GL_SHININESS, shininess);

}

// sets up a point light
void setup_point_light(int lightId, float x, float y, float z, float r, float g, float b) {
	glLightfv(lightId, GL_POSITION, Array3(x, y, z));
	glLightfv(lightId, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(lightId, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(lightId, GL_SPECULAR, Array3(r, g, b));
	glLightf(lightId, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(lightId, GL_LINEAR_ATTENUATION, 0.);
	glLightf(lightId, GL_QUADRATIC_ATTENUATION, 0.);
}

// sets up a spot light facing a given direction
void setup_spot_light(int lightId, float x, float y, float z, float xDir, float yDir, float zDir, float r, float g, float b) {
	glLightfv(lightId, GL_POSITION, Array3(x, y, z));
	glLightfv(lightId, GL_SPOT_DIRECTION, Array3(xDir, yDir, zDir));
	glLightf(lightId, GL_SPOT_EXPONENT, 1.);
	glLightf(lightId, GL_SPOT_CUTOFF, 45.);
	glLightfv(lightId, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(lightId, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(lightId, GL_SPECULAR, Array3(r, g, b));
	glLightf(lightId, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(lightId, GL_LINEAR_ATTENUATION, 0.);
	glLightf(lightId, GL_QUADRATIC_ATTENUATION, 0.);

	#ifdef DEBUG
	// draw debug line from GREEN -> RED
	glBegin(GL_LINES);
	set_material(0.0, 1.0, 0.0, 1.0);
	glVertex3f(x,y,z);
	set_material(1.0, 0.0, 0.0, 1.0);
	glVertex3f(x+xDir,y+yDir,z+zDir);
	glEnd();
	#endif
}

float *Array3(float r, float g, float b) {
	static float array[4];
	array[0] = r;
	array[1] = g;
	array[2] = b;
	array[3] = 1.0;
	return array;
}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )
void InitLists() {
	glutSetWindow( MainWindow );

  // create the axes
  AxesList = glGenLists( 1 );
  glNewList( AxesList, GL_COMPILE );
  glLineWidth( AXES_WIDTH );
  Axes( 1.5 );
  glLineWidth( 1. );
  glEndList( );

	// generate the test sheet lists
	generateBox();
	generateDenseSheet();
	generateSimpleTestSheet();

}


// used to track when animation should be locked
bool Frozen;


void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE_KEY:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

    case 'f':
        Frozen = ! Frozen;
        if( Frozen )
            glutIdleFunc( NULL );
        else
            glutIdleFunc( Animate );
        break;

		case '1':
			LandOctaves = 1;
			CloudOctaves = 1;
			WaterOctaves = 1;
			break;

		case '2':
			LandOctaves = 2;
			CloudOctaves = 2;
			WaterOctaves = 2;
			break;

		case '3':
			LandOctaves = 3;
			CloudOctaves = 3;
			WaterOctaves = 3;
			break;

		case '4':
			LandOctaves = 4;
			CloudOctaves = 4;
			WaterOctaves = 4;
			break;

		case '5':
			LandOctaves = 5;
			CloudOctaves = 5;
			WaterOctaves = 5;
			break;

		case '6':
			LandOctaves = 6;
			CloudOctaves = 6;
			WaterOctaves = 6;
			break;

		case '7':
			LandOctaves = 7;
			CloudOctaves = 7;
			WaterOctaves = 7;
			break;

		case '8':
			LandOctaves = 8;
			CloudOctaves = 8;
			WaterOctaves = 8;
			break;

		case '9':
			LandOctaves = 16;
			CloudOctaves = 16;
			WaterOctaves = 16;
			break;

		case 'c':
			EnableClouds = EnableClouds ? false : true;
			break;

		case 's':
			EnableSunRotation = EnableSunRotation ? false : true;
			printf("Time: %d\n", LongLongTime);
			break;

		case 'w':
			EnableWater = EnableWater ? false : true;
			break;

		case 'e':
			EnableLand = EnableLand ? false : true;
			break;

		case 'm':
			EnableCloudVolume = EnableCloudVolume ? false : true;

		case 'd':
			EnableShading = EnableShading ? false : true;
			break;

		case 't':
			EnableFluidLand = EnableFluidLand ? false : true;
			break;

		case '=':
			WaterHeight+=0.01f;
			break;

		case '-':
			WaterHeight-=0.01f;
			break;

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):
	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse button transitions down or up:

void MouseButton(int button, int state, int x, int y ) {
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 ) {
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );
	}


	// get the proper button bit mask:

	switch(button) {
		case GLUT_LEFT_BUTTON:
			b = LEFT;
			break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;
			break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button);
	}


	// button down sets the bit, up clears the bit:
	if(state == GLUT_DOWN) {
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit

	} else {
		ActiveButton &= ~b;		// clear the proper bit

	}

}


// called when the mouse moves while a button is down:
void MouseMotion( int x, int y ) {
	if( DebugOn != 0 ) {
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );
	}

	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 ) {
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 ) {
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:
		if( Scale < MINSCALE ) {
			Scale = MINSCALE;
		}
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );

}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene
void Reset() {
	ActiveButton = 0;
	AxesOn = 0;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void Resize( int width, int height ) {
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void Visibility ( int state ) {
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE ) {
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	} else {
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{

				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{

				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{

				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );
void HsvRgb( float hsv[3], float rgb[3] ) {
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;

		case 1:
			r = q;	g = v;	b = p;
			break;

		case 2:
			r = p;	g = v;	b = t;
			break;

		case 3:
			r = p;	g = q;	b = v;
			break;

		case 4:
			r = t;	g = p;	b = v;
			break;

		case 5:
			r = v;	g = p;	b = q;
			break;
	}

	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}
