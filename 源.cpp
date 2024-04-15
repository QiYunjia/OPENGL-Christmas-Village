#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#include <stdlib.h>
#include<math.h>
#include <stdio.h>
#include "vector"


int intWinWidth = 1000; //Default window size
int intWinHeight = 600;

float fltAnimationOffset = 0.0; //Animation offsets for various objects
float fltTreeOffset = 0.0;
float fltBuildingOffset = -2500.0;
float fltPoleOffset = 750.0;

float fltAcceleration = 0.0; //Speed of animation

float fltColourVal1 = 0.5; //First colour value
float fltColourVal2 = 0.7; //Second colour value
float fltColourVal3 = 0.4; //Third colour value

float fltFOV = 70; //Field Of View
float fltZoom = 1.0; //Zoom amount
float fltX0 = 0.0; //Camera position
float fltY0 = 120.0;
float fltZ0 = -500.0;
float fltXRef = 0.0; //Look At reference point
float fltYRef = 0.0;
float fltZRef = 0.0;
float fltXUp = 0.0; //Up vector
float fltYUp = 1.0;
float fltZUp = 0.0;
float fltViewingAngle = 0; //Used for rotating camera

float cone_x = 0;
float cone_y = 0;
float cone_z = 0;
float jump = 0;
float fltRoadOffset = 0;
double cameraD = 10;



//image attributes
struct bmpImage {
	GLint imagewidth;
	GLint imageheight;
	GLint pixellength;
};
//texture number
bmpImage loadTexture[12];

//sled position
float sledingx = 0;
float sledingy = 0;
//sled state
int carstate = 0;
//get on/off the sled
int viewstage = 0;

//arm rotation and state
float arm = 0;
int armstate = 0;
//jump state
int jumpstage = 0;
BOOLEAN JUMP = false;

//scene state
int statescene = 0;

//falling snow parameters
int snowfall = 0;
int randomNum[100][3];
float time = 0;

//light
GLfloat lighting = 0.6;


//snowball
BOOLEAN snowball = false;
float ball = 0;

//time interval
int time_interval = 16;

//text printing
void printBitmapString(void* font, const char* s) {
	if (s && strlen(s)) {
		while (*s) {
			glutBitmapCharacter(font, *s);
			s++;
		}
	}
}
int TextOut(float x, float y, const char* cstr) {
	glRasterPos2f(x, y);
	printBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, cstr);
	return 1;
}


//walking animate
void animate()
{
	//Increments/decrements animation variables
	fltAnimationOffset = fltAcceleration;
	fltPoleOffset = fltAcceleration;
	fltTreeOffset = fltAcceleration;
	fltBuildingOffset = fltAcceleration;
	fltRoadOffset = fltAcceleration;

	glutPostRedisplay();
}

//image reading
using namespace std;
vector<GLubyte*>p; 
GLuint texture[12];
GLfloat angle = 0;
void ReadImage(const char path[256], GLint& imagewidth, GLint& imageheight, GLint& pixellength) {
	GLubyte* pixeldata;
	FILE* pfile;
	fopen_s(&pfile, path, "rb");
	if (pfile == 0) exit(0);
	fseek(pfile, 0x0012, SEEK_SET);
	fread(&imagewidth, sizeof(imagewidth), 1, pfile);
	fread(&imageheight, sizeof(imageheight), 1, pfile);
	pixellength = imagewidth * 3;
	while (pixellength % 4 != 0) pixellength++;
	pixellength *= imageheight;
	pixeldata = (GLubyte*)malloc(pixellength);
	if (pixeldata == 0) exit(0);
	fseek(pfile, 54, SEEK_SET);
	fread(pixeldata, pixellength, 1, pfile);
	p.push_back(pixeldata);
	fclose(pfile);
}





//timer
void when_in_mainloop() { 
	glutPostRedisplay();
}
void OnTimer(int value)
{
	//sled position
	if (carstate==1&&sledingx <1500) {
		sledingx++;
	}
	if (carstate == 1 && sledingy <500&&sledingx>=200) {
		sledingy++;
	}
	if (carstate == 1 && sledingy > 0 && sledingx <= 0) {
		sledingy--;
	}
	if (carstate == 1 && sledingx == 1500) {
		sledingx = -1500;
		sledingy = 1200;
	}
	if (snowball == true) {
		ball += 5;
	}
	//waving arm
	if (armstate==0) {
		arm++;
	}
	if (arm == 100) {
		armstate = 1;
	}
	if (armstate == 1) {
		arm--;
	}
	if (arm == -100) {
		armstate = 0;
	}

	//jump
	if (time <= 1)
		time += 0.05;
	if (JUMP ==true) {
		if(jump < 24) {
			jump+=3;
		}else
			JUMP = false;	
		}   
	if (jump >= 0) {
		jump-= 0.8;
		}


	//falling snow
	if (snowfall < 400) {
		snowfall += 1;
	}
	else snowfall = -20;


	when_in_mainloop();
	glutTimerFunc(time_interval, OnTimer, 1);
}


void myinit() {

	//texture
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_FLAT);
	glEnable(GL_TEXTURE_2D);


	//images in bmp format
	const char* bmp[11] = {  
		"land.bmp","woodwall.bmp","woodfloor.bmp","background.bmp","decorate.bmp","wallpaper.bmp","window.bmp","lights.bmp","carpet.bmp","fireplace.bmp","giftpaper.bmp"
	};
	
	//read images
	for (int i = 0; i < 10; i++) {
		ReadImage(bmp[i], loadTexture[i].imagewidth, loadTexture[i].imageheight, loadTexture[i].pixellength);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &texture[i]);     
		glBindTexture(GL_TEXTURE_2D, texture[i]);   
		glTexImage2D(GL_TEXTURE_2D, 0, 3, loadTexture[i].imagewidth, loadTexture[i].imageheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[i]);


		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);    

		//parameters
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	}
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	//random numbers for snow
	for (int i = 0; i < 25; i++) {
		randomNum[i][0] = rand() % 400 - 100;
		randomNum[i][1] = rand() % 500 - 50;
		randomNum[i][2] = rand() % 500 + 20;
	}




	GLfloat lmodel_ambient[] = { lighting, lighting, lighting, 0.8};

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}




//room///////////////////////////////////////////////////
void sofa() {
	glPushMatrix();
	glTranslatef(150.0, 0.0 - jump, fltTreeOffset);

	GLfloat b1_diffuse[] = { 0,0,0,1 };
	GLfloat b1_ambient[] = { 0,0,0,1 };
	GLfloat b1_emission[] = { 0,0,0,1 };
	GLfloat b1_shininess[] = { 0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, b1_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, b1_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, b1_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, b1_shininess);
	
	glPushMatrix();
	glTranslatef(0.0, 115.0, 0.0);
	glScalef(30.0, 15.0, 10.0);
	glColor3f(0.3, 0.3, 0.8);
	glTranslatef(-10, 0.0, -6.5);
	glutSolidCylinder(8, 3.5, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 115.0, 0.0);
	glScalef(25.0, 15.0, 10.0);
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(-12.5, -5, -12.5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCylinder(8, 4.5, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 115.0, 0.0);
	glScalef(10.0, 10.0, 10.0);
	glColor3f(1, 1, 1);
	glTranslatef(-8, -2, -6.5);
	glRotatef(-90.0, 0, 1.0, 0.0);
	glutSolidCylinder(8, 3.5, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 115.0, 0.0);
	glScalef(10.0, 10.0, 10.0);
	glColor3f(1, 1, 1);
	glTranslatef(-50, -2, -6.5);
	glRotatef(-90.0, 0, 1.0, 0.0);
	glutSolidCylinder(8, 3.5, 10, 10);
	glPopMatrix();

	glPopMatrix();
}
void table() {
	glPushMatrix();
	glTranslatef(150.0, 0.0 - jump, fltTreeOffset);
	glScalef(2, 3, 2);

	//table
	GLfloat b1_diffuse[] = { 5.7,5.7,5.4,1 };
	GLfloat b1_ambient[] = { 0.7,0.7,0.4,1 };
	GLfloat b1_emission[] = { 0,0,0,1 };
	GLfloat b1_shininess[] = { 0 };

	glMaterialfv(GL_FRONT, GL_DIFFUSE, b1_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, b1_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, b1_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, b1_shininess);

	glPushMatrix();
	glTranslatef(-105.0, 15, -235.0);
	glColor3f(0.5, 0.2, 0);
	glScalef(8.0, 1, 8);
	glRotatef(90.0, 1, 0, 0);
	glutSolidCylinder(5.0, 10.0, 8, 8);
	glPopMatrix();

	//teapot and teacups
	GLfloat b2_diffuse[] = { 0,0,0,1 };
	GLfloat b2_ambient[] = { 0.4,0.4,0.4,1 };
	GLfloat b2_emission[] = { 0.3,0.3,0.3,1 };
	GLfloat b2_shininess[] = { 50 };
	GLfloat mat_specular[] = { 0.7, 0.7,0.7, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, b2_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, b2_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, b2_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, b2_shininess);
	glPushMatrix();
	glTranslatef(-105.0, 25, -235.0);
	glColor3f(0.4, 0.4, 0.4);
	glutSolidTeapot(10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-125.0, 20, -235.0);
	glColor3f(0.4, 0.4, 0.4);
	glutSolidTeacup(12);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-85.0, 20, -235.0);
	glColor3f(0.4, 0.4, 0.4);
	glutSolidTeacup(12);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-105.0, 20, -255.0);
	glColor3f(0.4, 0.4, 0.4);
	glutSolidTeacup(12);
	glPopMatrix();

	GLfloat mat_specular1[] = { 0, 0, 0, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);


	glPopMatrix();
}
void light() {
	glPushMatrix();
	glTranslatef(150.0, 200 - jump, fltTreeOffset);

	GLfloat b2_diffuse[] = { 0,0,0,1 };
	GLfloat b2_ambient[] = { 0.4,0.4,0.4,1 };
	GLfloat b2_emission[] = { 0.3,0.3,0.3,1 };
	GLfloat b2_shininess[] = { 50 };
	GLfloat mat_specular[] = { 0.7, 0.7,0.7, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, b2_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, b2_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, b2_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, b2_shininess);

	//wire
	glPushMatrix();
	glTranslatef(-105.0, 20, -255.0);
	glRotatef(90, -1, 0, 0);
	glColor3f(0, 0, 0);
	glutSolidCylinder(3, 500, 16, 16);
	glPopMatrix();

	//bulb
	glPushMatrix();
	glTranslatef(-105.0, 20, -255.0);
	glRotatef(90, -1, 0, 0);
	glColor3f(1, 1, 0);
	glutSolidCone(50, 50, 16, 16);
	glPopMatrix();

	//Lampshade
	glPushMatrix();
	glTranslatef(-105.0, 20, -255.0);
	glRotatef(90, -1, 0, 0);
	glColor3f(1, 0.5, 0);
	glutSolidSphere(10, 50, 16);
	glPopMatrix();

	GLfloat mat_specular1[] = { 0, 0, 0, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);


	glPopMatrix();
}
void housein() {
	glPushMatrix();
	glTranslatef(-100.0, 0.0 - jump, 2000 + fltBuildingOffset);

	//floor
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 2, 500);
	glTexCoord2f(1, 0); glVertex3d(-500, 2, 500);
	glTexCoord2f(1, 1); glVertex3d(-500, 2, -500);
	glTexCoord2f(0, 1); glVertex3d(500, 2, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//floor
	GLfloat t_diffuse[] = { 5.7,5.7,5.4,0 };
	GLfloat t_ambient[] = { 0.7,0.7,0.4,0 };
	GLfloat t_emission[] = { 0.5,0.5,0.5,0 };
	GLfloat t_shininess[] = { 2 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, t_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, t_shininess);
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[8]);
	glRotatef(180, 0, 1, 0);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(300, 3, 300);
	glTexCoord2f(1, 0); glVertex3d(-300, 3, 300);
	glTexCoord2f(1, 1); glVertex3d(-300, 3, -300);
	glTexCoord2f(0, 1); glVertex3d(300, 3, -300);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	GLfloat t2_diffuse[] = { 5.7,5.7,5.4,0 };
	GLfloat t2_ambient[] = { 0.7,0.7,0.4,0 };
	GLfloat t2_emission[] = { 0.5,0.5,0.5,0 };
	GLfloat t2_shininess[] = { 2 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t2_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t2_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, t2_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, t2_shininess);

	//wall front
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(-500, 400, -500);
	glTexCoord2f(1, 1); glVertex3d(-500, 0, -500);
	glTexCoord2f(0, 1); glVertex3d(500, 0, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall front paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 400, -499);
	glTexCoord2f(1, 0); glVertex3d(-500, 400, -499);
	glTexCoord2f(1, 1); glVertex3d(-500, 150, -499);
	glTexCoord2f(0, 1); glVertex3d(500, 150, -499);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall front
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-500, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(0, 400, -500);
	glTexCoord2f(1, 1); glVertex3d(-250, 800, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall right
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-500, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(-500, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(-500, 0, 500);
	glTexCoord2f(0, 1); glVertex3d(-500, 0, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall right paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-499, 400, 0);
	glTexCoord2f(1, 0); glVertex3d(-499, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(-499, 150, 500);
	glTexCoord2f(0, 1); glVertex3d(-499, 150, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall right paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-499, 150, 0);
	glTexCoord2f(1, 0); glVertex3d(-499, 150, 500);
	glTexCoord2f(1, 1); glVertex3d(-499, 0, 500);
	glTexCoord2f(0, 1); glVertex3d(-499, 0, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall right paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-499, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(-499, 400, 0);
	glTexCoord2f(1, 1); glVertex3d(-499, 150, 0);
	glTexCoord2f(0, 1); glVertex3d(-499, 150, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall right paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-499, 150, -500);
	glTexCoord2f(1, 0); glVertex3d(-499, 150, 0);
	glTexCoord2f(1, 1); glVertex3d(-499, 0, 0);
	glTexCoord2f(0, 1); glVertex3d(-499, 0, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall left
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(500, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(500, 0, 500);
	glTexCoord2f(0, 1); glVertex3d(500, 0, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall left paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(499, 400, -0);
	glTexCoord2f(1, 0); glVertex3d(499, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(499, 150, 500);
	glTexCoord2f(0, 1); glVertex3d(499, 150, -0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall left paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(499, 150, -0);
	glTexCoord2f(1, 0); glVertex3d(499, 150, 500);
	glTexCoord2f(1, 1); glVertex3d(499, 0, 500);
	glTexCoord2f(0, 1); glVertex3d(499, 0, -0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall left paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(499, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(499, 400, 0);
	glTexCoord2f(1, 1); glVertex3d(499, 150, 0);
	glTexCoord2f(0, 1); glVertex3d(499, 150, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(499, 150, -500);
	glTexCoord2f(1, 0); glVertex3d(499, 150, 0);
	glTexCoord2f(1, 1); glVertex3d(499, 0, 0);
	glTexCoord2f(0, 1); glVertex3d(499, 0, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall left
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(500, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(500, 750, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall left paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(499, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(499, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(499, 750, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall middle
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(20, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(20, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(20, 750, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall back
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 400, 500);
	glTexCoord2f(1, 0); glVertex3d(-500, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(-500, 0, 500);
	glTexCoord2f(0, 1); glVertex3d(500, 0, 500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall back paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(0, 400, 499);
	glTexCoord2f(1, 0); glVertex3d(-500, 400, 499);
	glTexCoord2f(1, 1); glVertex3d(-500, 150, 499);
	glTexCoord2f(0, 1); glVertex3d(0, 150, 499);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall back paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(0, 150, 499);
	glTexCoord2f(1, 0); glVertex3d(-500, 150, 499);
	glTexCoord2f(1, 1); glVertex3d(-500, 0, 499);
	glTexCoord2f(0, 1); glVertex3d(0, 0, 499);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall back paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 400, 499);
	glTexCoord2f(1, 0); glVertex3d(0, 400, 499);
	glTexCoord2f(1, 1); glVertex3d(0, 150, 499);
	glTexCoord2f(0, 1); glVertex3d(500, 150, 499);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall back paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 150, 499);
	glTexCoord2f(1, 0); glVertex3d(0, 150, 499);
	glTexCoord2f(1, 1); glVertex3d(0, 0, 499);
	glTexCoord2f(0, 1); glVertex3d(500, 0, 499);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall back
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-500, 400, 500);
	glTexCoord2f(1, 0); glVertex3d(0, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(-250, 800, 500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall back paper
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-500, 400, 499);
	glTexCoord2f(1, 0); glVertex3d(0, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(-250, 800, 499);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	GLfloat t_diffuse3[] = { 5.7,5.7,5.4,0 };
	GLfloat t_ambient3[] = { 0.7,0.7,0.4,0 };
	GLfloat t_emission3[] = { 0,0,0,0 };
	GLfloat t_shininess3[] = { 2 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t_diffuse3);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t_ambient3);
	glMaterialfv(GL_FRONT, GL_EMISSION, t_emission3);
	glMaterialfv(GL_FRONT, GL_SHININESS, t_shininess3);
	GLfloat mat_specular[] = { 0.0, 0.0,0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

	//roof 
	glPushMatrix();
	glTranslatef(280, 550, -300);
	glRotatef(-35.0, 1.0, 0.0, 0.0);
	glScalef(550.0, 20.0, 700.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	//roof 
	glPushMatrix();
	glTranslatef(280, 550, 280);
	glRotatef(35.0, 1.0, 0.0, 0.0);
	glScalef(550.0, 20.0, 700.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	//roof 
	glPushMatrix();
	glTranslatef(-400, 580, 0);
	glRotatef(55.0, 0.0, 0.0, 1.0);
	glScalef(520.0, 20.0, 1100.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	//roof 
	glPushMatrix();
	glTranslatef(-100, 580, 0);
	glRotatef(-55.0, 0.0, 0.0, 1.0);
	glScalef(520.0, 20.0, 1100.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidCube(1);
	glPopMatrix();


	glPopMatrix();
}
void TreeIN() {
	glPushMatrix();
	glTranslatef(150.0, 0.0 - jump, fltTreeOffset);
	GLfloat t1_diffuse[] = { 5.7,5.7,5.4,1 };
	GLfloat t1_ambient[] = { 0.7,0.7,0.4,1 };
	GLfloat t1_emission[] = { 0,0,0,1 };
	GLfloat t1_shininess[] = { 0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t1_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t1_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, t1_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, t1_shininess);

	//leaves
	glPushMatrix();
	glTranslatef(0.0, 150.0, 0.0);
	glScalef(10.0, 15.0, 10.0);
	glColor3f(0.0, 0.5, 0.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(4.0, 9.0, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 100.0, 0.0);
	glScalef(12.0, 15.0, 10.0);
	glColor3f(0.0, 0.5, 0.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(5.0, 8.0, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 50.0, 0.0);
	glScalef(12.0, 15.0, 10.0);
	glColor3f(0.0, 0.5, 0.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(6.5, 10.0, 16, 16);
	glPopMatrix();

	//ornaments
	GLfloat t_diffuse[] = { 5.7,5.7,5.4,0 };
	GLfloat t_ambient[] = { 0.7,0.7,0.4,0 };
	GLfloat t_emission[] = { 0,0,0,0 };
	GLfloat t_shininess[] = { 2 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, t_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, t_shininess);

	glPushMatrix();
	glTranslatef(30.0, 100.0, -100.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(1, 0, 0);
	glutSolidSphere(1.0, 20.0, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10, 100.0, -110.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(1, 1, 0);
	glutSolidSphere(1.0, 20.0, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-30, 170.0, -70.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(0, 1, 1.0);
	glutSolidSphere(0.8, 20.0, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-35, 200.0, -80.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(0.5, 1, 0.5);
	glutSolidSphere(0.8, 20.0, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-20, 180.0, -90.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(0.2, 0.2, 1.0);
	glutSolidSphere(0.8, 20.0, 16);
	glPopMatrix();
	GLfloat mat_specular1[] = { 0, 0, 0, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);

	//gifts
	glPushMatrix();
	glTranslatef(-20, 70.0, -180.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(0.5, 0.2, 0.2);
	glutSolidCube(3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-60, 70.0, -150.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(0.5, 0.5, 0.2);
	glutSolidCube(3);
	glPopMatrix();

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[10]);
	GLUquadric* quadricObj = gluNewQuadric();
	gluQuadricTexture(quadricObj, GL_TRUE);

	glTranslatef(-120, 40.0, -80.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	gluCylinder(quadricObj,1,1, 3, 10, 10);
	
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-60, 40.0, -40.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(0.5, 0.5, 0.8);
	glutSolidCylinder(2, 5, 10, 10);
	glPopMatrix();


	glPopMatrix();
}
void Fireplace() {
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(-800, -80 - jump, 1000 + fltRoadOffset);
	glRotatef(90, 0, 1, 0);

	GLfloat t1_diffuse[] = { 5.7,5.7,5.4,0 };
	GLfloat t1_ambient[] = { 0.7,0.7,0.4,0 };
	GLfloat t1_emission[] = { 1,1,1,0 };
	GLfloat t1_shininess[] = { 2 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t1_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t1_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, t1_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, t1_shininess);
	
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[9]);
	glRotatef(180, 0, 0, 1);
	glTranslatef(120.0, -335, 1695);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(200, 202, -300);
	glTexCoord2f(1, 0); glVertex3d(0, 202, -300);
	glTexCoord2f(1, 1); glVertex3d(0, 0, -300);
	glTexCoord2f(0, 1); glVertex3d(200, 0, -300);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();
}
/////////////////////////////////////////////////////////

//town////////////////////////////////////////////////////

void Background() {
	glPushMatrix();
	glTranslatef(0, 0.0 - jump, 2000 + fltBuildingOffset);

	//back
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(2500, 5000, 501);
	glTexCoord2f(1, 0); glVertex3d(-2500, 5000, 501);
	glTexCoord2f(1, 1); glVertex3d(-2500, -1, 501);
	glTexCoord2f(0, 1); glVertex3d(2500, -1, 501);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//left
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(2500, 5000, -3500);
	glTexCoord2f(1, 0); glVertex3d(2500, 5000, 501);
	glTexCoord2f(1, 1); glVertex3d(2500, -1, 501);
	glTexCoord2f(0, 1); glVertex3d(2500, -1, -3500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//right
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-2500, 5000, -3500);
	glTexCoord2f(1, 0); glVertex3d(-2500, 5000, 501);
	glTexCoord2f(1, 1); glVertex3d(-2500, -1, 501);
	glTexCoord2f(0, 1); glVertex3d(-2500, -1, -3500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPopMatrix();
}
void ground() {
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(0.0, 0.0 - jump, fltRoadOffset);

	glPushMatrix();
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(2500, 0, 2500);
	glTexCoord2f(1, 0); glVertex3d(-2500, 0, 2500);
	glTexCoord2f(1, 1); glVertex3d(-2500, 0, -2500);
	glTexCoord2f(0, 1); glVertex3d(2500, 0, -2500);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0, 1000);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 1, 1000);
	glTexCoord2f(1, 0); glVertex3d(-500, 1, 1000);
	glTexCoord2f(1, 1); glVertex3d(-500, 1, -1500);
	glTexCoord2f(0, 1); glVertex3d(500, 1, -1500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();


	glPopMatrix();
}
void snowGround() {


	glColor3f(1, 1, 1);
	glTranslatef(0.0, 0.0 - jump, fltRoadOffset);


	GLfloat t2_diffuse[] = { 5.7,5.7,5.4,0 };
	GLfloat t2_ambient[] = { 0.7,0.7,0.4,0 };
	GLfloat t2_emission[] = { 0,0,0,0 };
	GLfloat t2_shininess[] = { 5 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t2_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t2_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, t2_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, t2_shininess);

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(-500.0, -920.0, -500.0);
	glutSolidCylinder(1000.0, 2800.0, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(500.0, -920.0, -500.0);
	glutSolidCylinder(1000.0, 2800.0, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(1100.0, -820.0, -500.0);
	glutSolidCylinder(1000.0, 2800.0, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(-1600.0, -680.0, -500.0);
	glutSolidCylinder(1000.0, 2800.0, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(-2500.0, 80.0, 3500.0);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glutSolidCylinder(1000.0, 5000.0, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(-1500.0, 380.0, 3280.0);
	glScalef(2, 1, 1);
	glutSolidSphere(1000.0, 16, 16);
	glPopMatrix();
}


int housestate = 0;
int snowmanstate = 0;
int treestate = 0;
void Torus() {
	glPushMatrix();
	glTranslatef(0, 0.0 - jump,  fltBuildingOffset);
	glScalef(1.6, 1.6, 1.6);

	GLfloat t1_diffuse[] = { 5.7,5.7,5.4,0};
	GLfloat t1_ambient[] = { 0.7,0.7,0.4,0 };
	GLfloat t1_emission[] = { 0,0,0,0 };
	GLfloat t1_shininess[] = { 5};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t1_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t1_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, t1_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, t1_shininess);

	glPushMatrix();
	glTranslatef(0, 50,0 );
	glColor3f(0.20, 0.38, 0.18);
	glutSolidTorus(10, 20, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10, 70, -10);
	glRotatef(90.0, 1.0, -1.0, 0.0);
	glColor3f(1, 0, 0.0);
	glutSolidCone(8,40,6, 6);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-10, 70, -10);
	glRotatef(90.0, 1.0, 1.0, 0.0);
	glColor3f(1, 0, 0.0);
	glutSolidCone(8, 40, 6, 6);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(20, 50, -10);
	glColor3f(1, 1, 0.0);
	glutSolidSphere(3,10,10);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(15, 70, -10);
	glColor3f(1, 1, 0.0);
	glutSolidSphere(3, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-15, 70, -10);
	glColor3f(1, 1, 0.0);
	glutSolidSphere(3, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-10, 30, -10);
	glColor3f(1, 1, 0.0);
	glutSolidSphere(3, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(12, 32, -10);
	glColor3f(1, 1, 0.0);
	glutSolidSphere(3, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-22, 48, -10);
	glColor3f(1, 1, 0.0);
	glutSolidSphere(3, 10, 10);
	glPopMatrix();

	glPopMatrix();

}
void house() {
	glPushMatrix();
	glTranslatef(-100.0, 0.0 - jump, 2000+fltBuildingOffset);
	GLfloat t1_diffuse[] = { 5.7,5.7,5.4,0 };
	GLfloat t1_ambient[] = { 0.7,0.7,0.4,0 };
	GLfloat t1_emission[] = {0.5,0.5,0.5,0 };
	GLfloat t1_shininess[] = { 2 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t1_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t1_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, t1_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, t1_shininess);

	//floor
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500,2, 500);
	glTexCoord2f(1, 0); glVertex3d(-500, 2, 500);
	glTexCoord2f(1, 1); glVertex3d(-500, 2, -500);
	glTexCoord2f(0, 1); glVertex3d(500, 2, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall front
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(-500, 400, -500);
	glTexCoord2f(1, 1); glVertex3d(-500, 0, -500);
	glTexCoord2f(0, 1); glVertex3d(500, 0, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall front
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-500, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(0, 400, -500);
	glTexCoord2f(1, 1); glVertex3d(-250, 800, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall right
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-500, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(-500, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(-500, 0, 500);
	glTexCoord2f(0, 1); glVertex3d(-500, 0, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall left
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(500, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(500, 0, 500);
	glTexCoord2f(0, 1); glVertex3d(500, 0, -500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall left
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(500, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(500, 750, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall middle
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(20, 400, -500);
	glTexCoord2f(1, 0); glVertex3d(20, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(20, 750, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall back
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 400, 500);
	glTexCoord2f(1, 0); glVertex3d(-500, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(-500, 0, 500);
	glTexCoord2f(0, 1); glVertex3d(500, 0, 500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall back
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-500, 400, 500);
	glTexCoord2f(1, 0); glVertex3d(0, 400, 500);
	glTexCoord2f(1, 1); glVertex3d(-250, 800, 500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	GLfloat t2_diffuse[] = { 5.7,5.7,5.4,0 };
	GLfloat t2_ambient[] = { 0.7,0.7,0.4,0 };
	GLfloat t2_emission[] = { 0,0,0,0 };
	GLfloat t2_shininess[] = { 5 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t2_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t2_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, t2_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, t2_shininess);

	//roof 
	glPushMatrix();
	glTranslatef(280, 550, -300);
	glRotatef(-35.0, 1.0, 0.0, 0.0);
	glScalef(550.0, 20.0, 700.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	//roof 
	glPushMatrix();
	glTranslatef(280, 550, 280);
	glRotatef(35.0, 1.0, 0.0, 0.0);
	glScalef(550.0, 20.0, 700.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidCube(1);
	glPopMatrix();
	
	//roof 
	glPushMatrix();
	glTranslatef(-400,580, 0);
	glRotatef(55.0, 0.0, 0.0, 1.0);
	glScalef(520.0, 20.0, 1100.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	//roof 
	glPushMatrix();
	glTranslatef(-100, 580, 0);
	glRotatef(-55.0, 0.0, 0.0, 1.0);
	glScalef(520.0, 20.0, 1100.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	//chimney
	glPushMatrix();
	glTranslatef(-150, 780, -400);
	glScalef(50.0, 80.0, 50.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidCube(1.2);
	glPopMatrix();

	//chimney
	glPushMatrix();
	glTranslatef(-150,680, -400);
	glScalef(50.0, 100.0, 50.0);
	glColor3f(0.5, 0.5, 0.5);
	glutSolidCube(1);
	glPopMatrix();

	//door
	glPushMatrix();
	glTranslatef(-270, 200, -505);
	glScalef(120.0, 300.0, 5.0);
	glColor3f(0.2, 0.2, 0.35);
	glutSolidCube(1);
	glPopMatrix();

	glPopMatrix();

}
void smallhouse() {
	glPushMatrix();
	glTranslatef(-100.0, 0.0 - jump, 2000 + fltBuildingOffset);

	if (housestate == 0) {
		
	}
	if (housestate == 1) {
		glRotatef(90.0, 0.0, 0.1, 0.0);
	}
	if (housestate == 2) {
		glRotatef(120.0, 0.0, 1.0, 0.0);
	}

	glPushMatrix();
	glTranslatef(500.0, 0.0, -1300.0);
	glColor3f(0.5, 0.4, 0.1);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(-250, 250, -250);
	glTexCoord2f(1, 0); glVertex3d(-250, 250, 250);
	glTexCoord2f(1, 1); glVertex3d(-250, 400, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(500.0, 0.0, -1300.0);
	glColor3f(0.5, 0.4, 0.1);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(250, 250, -250);
	glTexCoord2f(1, 0); glVertex3d(250, 250, 250);
	glTexCoord2f(1, 1); glVertex3d(250, 400, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(500.0, 0.0, -1300.0);
	glScalef(50, 50.0, 50);
	glColor3f(0.5, 0.4, 0.1);
	glutSolidCube(10);
	glPopMatrix();

	//roof 
	glPushMatrix();
	glTranslatef(500, 300, -1480);
	glRotatef(-35.0, 1.0, 0.0, 0.0);
	glScalef(550.0, 20.0,450.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	//roof 
	glPushMatrix();
	glTranslatef(500, 300, -1130);
	glRotatef(35.0, 1.0, 0.0, 0.0);
	glScalef(550.0, 20.0, 450.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidCube(1);
	glPopMatrix();

	glPopMatrix();
}
void window() {
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(0.0, 0.0 - jump, fltRoadOffset);
	GLfloat t1_diffuse[] = { 5.7,5.7,5.4,0 };
	GLfloat t1_ambient[] = { 0.7,0.7,0.4,0 };
	GLfloat t1_emission[] = { 1,1,1,0 };
	GLfloat t1_shininess[] = { 5 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t1_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t1_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, t1_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, t1_shininess);
	//wall front
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glRotatef(180,0,0,1);
	glTranslatef(120.0, -350,1695);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(100,202, -300);
	glTexCoord2f(1, 0); glVertex3d(0, 202, -300);
	glTexCoord2f(1, 1); glVertex3d(0, 0, -300);
	glTexCoord2f(0, 1); glVertex3d(100, 0, -300);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall front
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glRotatef(180, 0, 0, 1);
	glTranslatef(-180.0, -350, 1695);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(100, 202, -300);
	glTexCoord2f(1, 0); glVertex3d(0, 202, -300);
	glTexCoord2f(1, 1); glVertex3d(0, 0, -300);
	glTexCoord2f(0, 1); glVertex3d(100, 0, -300);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall front
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[7]);
	glRotatef(180, 0, 0, 1);
	glTranslatef(-780.0, -350, 1695);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex3d(500, 102, -300);
	glTexCoord2f(1, 0); glVertex3d(0, 102, -300);
	glTexCoord2f(1, 1); glVertex3d(0, 0, -300);
	glTexCoord2f(0, 1); glVertex3d(500, 0, -300);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPopMatrix();
}
void snow() {
	glPushMatrix();
	glTranslatef(0.0, 0.0-jump, fltTreeOffset);
	glColor4f(1, 1, 1, 1);
	glutSolidSphere(5, 36, 36);
	glPopMatrix();
}
void snowMan() {
	glPushMatrix();
	glTranslatef(0.0, 0.0 - jump, fltTreeOffset);
	if (snowmanstate == 0){
		glRotatef(30.0, 0.0, 1.0, 0.0);
	}
	if (snowmanstate == 1) {
		glRotatef(-20, 0.0, 1, 0.0);
	}
	
	glScalef(1.5, 1.5, 1.5);
	//body
	glPushMatrix();
	glTranslatef(30.0, 50.0, -100.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(6.0, 5.0, 5.0);
	glColor3f(1, 1, 1.0);
	glutSolidSphere(6.0, 20.0, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(30.0, 92.0, -100.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(5, 5.0, 5.0);
	glColor3f(1, 1, 1.0);
	glutSolidSphere(4.5, 20.0, 20);
	glPopMatrix();
	//eyes
	glPushMatrix();
	glTranslatef(20.0, 102.0, -120.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(2.0, 2, 2);
	glColor3f(0, 0, 0.0);
	glutSolidSphere(1.0, 20.0, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(40.0, 102.0, -120.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(2.0, 2, 2);
	glColor3f(0, 0, 0.0);
	glutSolidSphere(1.0, 20.0, 20);
	glPopMatrix();

	//nose
	glPushMatrix();
	glTranslatef(30.0, 92.0, -110.0);
	glScalef(2.0, 2.0, 2.0);
	glColor3f(0.95, 0.52, 0.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	glutSolidCone(2.5, 10, 10, 10);
	glPopMatrix();

	//hat
	glPushMatrix();
	glTranslatef(30.0, 130.0, -90.0);
	glScalef(2.0, 2.0, 2.0);
	glColor3f(1, 0, 0.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glutSolidCylinder(5.0, 10.0, 8, 8);
	
	glPopMatrix();


	glPopMatrix();
}
void Tree() {
	glPushMatrix();
	glTranslatef(150.0, 0.0 - jump, fltTreeOffset);

	GLfloat t2_diffuse[] = { 5.7,5.7,5.4,0 };
	GLfloat t2_ambient[] = { 0.7,0.7,0.4,0 };
	GLfloat t2_emission[] = { 0,0,0,0 };
	GLfloat t2_shininess[] = { 5 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, t2_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, t2_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, t2_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, t2_shininess);
	if (treestate == 0) {

	}
	if (treestate ==1) {
		glRotatef(30.0, 0.0, 1.0, 0.0);
	}
	if (treestate == 2) {
		glRotatef(90.0, 0.0, 1.0, 0.0);
	}
	//leaves snow
	glPushMatrix();
	glTranslatef(0.0, 215.0, 0.0);
	glScalef(10.0, 15.0, 10.0);
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(2.5, 5.5, 10, 10);
	glPopMatrix();

	//2nd
	glPushMatrix();
	glTranslatef(0.0, 150.0, 0.0);
	glScalef(10.0, 15.0, 10.0);
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(4.5, 5.0, 10, 10);
	glPopMatrix();
	//3rd
	glPushMatrix();
	glTranslatef(0.0, 100.0, 0.0);
	glScalef(12.0, 15.0, 10.0);
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(5.5, 4.0, 10, 16);
	glPopMatrix();
	//4th
	glPushMatrix();
	glTranslatef(0.0, 50.0, 0.0);
	glScalef(12.0, 15.0, 10.0);
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(6.9, 5.0, 10, 10);
	glPopMatrix();

	//leaves
	glPushMatrix();
	glTranslatef(0.0, 150.0, 0.0);
	glScalef(10.0, 15.0, 10.0);
	glColor3f(0.0, 0.5, 0.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(4.0, 9.0, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 100.0, 0.0);
	glScalef(12.0, 15.0, 10.0);
	glColor3f(0.0, 0.5, 0.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(5.0, 8.0, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 50.0, 0.0);
	glScalef(12.0, 15.0, 10.0);
	glColor3f(0.0, 0.5, 0.0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(6.5, 10.0, 16, 16);
	glPopMatrix();

	//trunk
	glPushMatrix();
	glTranslatef(0.0, -50.0, -50.0);
	glScalef(15.0, 15.0, 15.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glColor3f(0.7, 0.3, 0.0);
	glutSolidCylinder(1.0, 10.0, 16, 16);
	glPopMatrix();

	//snow
	glPushMatrix();
	glTranslatef(30.0, 100.0, -100.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(1, 1, 1.0);
	glutSolidSphere(1.0, 20.0, 16);
	glPopMatrix();


	 
	glPushMatrix();
	glTranslatef(10, 100.0, -110.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(1, 1, 1.0);
	glutSolidSphere(1.0, 20.0, 16);
	glPopMatrix();


	
	glPushMatrix();
	glTranslatef(-30, 170.0, -70.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(1, 1, 1.0);
	glutSolidSphere(0.8, 20.0, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-25, 170.0, -80.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(1, 1, 1.0);
	glutSolidSphere(1, 20.0, 16);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-20, 170.0, -90.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(1, 1, 1.0);
	glutSolidSphere(0.8, 20.0, 16);
	glPopMatrix();

	//gift
	glPushMatrix();
	glTranslatef(0, -45.0, -0);
	glPushMatrix();
	glTranslatef(-20, 70.0, -180.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(0.5, 0.2, 0.2);
	glutSolidCube(3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-60, 70.0, -150.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(0.5, 0.5, 0.2);
	glutSolidCube(3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-60, 40.0, -40.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glScalef(15.0, 15.0, 15.0);
	glColor3f(0.5, 0.5, 0.8);
	glutSolidCylinder(2, 5, 10, 10);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}
void lampPost()
{
	for (int fltPoleNumber = -5; fltPoleNumber < 5; fltPoleNumber += 4.0)
	{
		glPushMatrix();
		glTranslatef(-200.0, 0.0 - jump, ((fltPoleNumber * 100) + fltPoleOffset));
	

		glPushMatrix();
		glTranslatef(0.0, 100.0, 0.0);
		glScalef(7.0, 300.0, 7.0);
		glColor3f(0, 0, 0);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, 240.0, 0.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		glColor3f(0, 0, 0);
		glutSolidCylinder(8,10,16,16);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, 270.0, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glColor3f(1, 1, 0);
		glutSolidCone(12, 30, 16, 16);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, 272.0, 0.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		glColor3f(0, 0, 0);
		glutSolidCylinder(20, 4, 16, 16);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, 279.0, 0.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		glColor3f(0, 0, 0);
		glutSolidCone(12, 14, 4, 4);
		glPopMatrix();

		glPopMatrix();
	}
	
	////========TRACK SLEEPERS========
	//for (int intSleeper = -249; intSleeper < 250; intSleeper += 2)
	//{
	//	float fltZOffset = intSleeper * 10;
	//	glPushMatrix();
	//	glTranslatef(0.0, 0.0, fltZOffset + fltAnimationOffset);
	//	glScalef(200.0, 5.0, 10.0);
	//	glPushMatrix();
	//	glColor3f(0.5, 0.25, 0.0);
	//	glutSolidCube(1);
	//	glColor3f(0.0, 0.0, 0.0);
	//	glutWireCube(1);
	//	glPopMatrix();
	//	glPopMatrix();
	//}
}
void sled() {
	glPushMatrix();
	//static
	if (carstate == 0) {
		glTranslatef(0, 0 - jump, fltTreeOffset);
	}
	//moving
	if (carstate == 1) {

		glTranslatef(sledingx, sledingy*0.3 - jump, fltTreeOffset-sledingy * 0.3);
	}
	
	//runner1
	glPushMatrix();
	glTranslatef(-50.0, 5.0, -250.0);
	glScalef(18.0, 0.25, 1.2);
	glColor3f(0.5, 0.25, 0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, 0, 1.0, 0.0);
	glutSolidCylinder(5.0, 18.0, 8, 8);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0,25.0, -250.0);
	glScalef(1.0, 1, 1.2);
	glColor3f(0.5, 0.25, 0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, 1, 0, 0.0);
	glutSolidCylinder(3.0, 20.0, 8, 8);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(100, 25.0, -250.0);
	glScalef(1.0, 1, 1.2);
	glColor3f(0.5, 0.25, 0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, 1, 0, 0.0);
	glutSolidCylinder(3.0, 20.0, 8, 8);
	glPopMatrix();

	//runner2
	glPushMatrix();
	glTranslatef(-50.0, 5.0, -280.0);
	glScalef(18.0, 0.25, 1.2);
	glColor3f(0.5, 0.25, 0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, 0, 1.0, 0.0);
	glutSolidCylinder(5.0, 18.0, 8, 8);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 25.0, -280.0);
	glScalef(1.0, 1, 1.2);
	glColor3f(0.5, 0.25, 0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, 1, 0, 0.0);
	glutSolidCylinder(3.0, 20.0, 8, 8);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(100, 25.0, -280.0);
	glScalef(1.0, 1, 1.2);
	glColor3f(0.5, 0.25, 0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, 1, 0, 0.0);
	glutSolidCylinder(3.0, 20.0, 8, 8);
	glPopMatrix();

	//carriage1
	glPushMatrix();
	glTranslatef(0, -20, 0);

	glPushMatrix();
	glTranslatef(-15.0, 80.0, -235.0);
	glScalef(8.0, 8, 8);
	glColor3f(0.2, 0.2, 0.35);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, 0, 1.0, -0.25);
	glutSolidCone(5.0, 20.0, 8, 8);
	glPopMatrix();

	//carriage2
	glPushMatrix();
	glTranslatef(120.0, 68.0, -235.0);
	glScalef(8.0, 8, 8);
	glColor3f(0.2, 0.2, 0.35);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, 0, -1.0, 0.20);
	glutSolidCone(3.0, 15.0, 8, 8);
	glPopMatrix();

	
	//Santa
	glPushMatrix();
	glTranslatef(20, -5, -20);
	//face
	glPushMatrix();
	glTranslatef(50, 128.0, -235.0);
	glScalef(4.0, 8, 4);
	glColor3f(0.6, 0.5, 0.5);
	glTranslatef(0.0, 0.0, -6.5);
	/*glRotatef(90.0, 0, -1.0, 0.25);*/
	glutSolidSphere(2.0, 24, 24);
	glPopMatrix();

	//hair
	glPushMatrix();
	glTranslatef(45, 128.0, -235.0);
	glScalef(4.0, 8, 4);
	glColor3f(0.6, 0.6, 0.6);
	glTranslatef(0.0, 0.0, -6.5);
	/*glRotatef(90.0, 0, -1.0, 0.25);*/
	glutSolidSphere(2.0, 24, 24);
	glPopMatrix();

	//hair
	glPushMatrix();
	glTranslatef(50, 138.0, -235.0);
	glScalef(4.0,3, 4);
	glColor3f(0.6, 0.6, 0.6);
	glTranslatef(0.0, 0.0, -6.5);
	/*glRotatef(90.0, 0, -1.0, 0.25);*/
	glutSolidSphere(2.0, 24, 24);
	glPopMatrix();

	//costume
	glPushMatrix();
	glTranslatef(50, 128.0, -235.0);
	glScalef(4.0, 8, 4);
	glColor3f(0.5, 0, 0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, -1, -0.5, 0);
	glutSolidCone(1.8,5, 24, 24);
	glPopMatrix();

	//costume
	glPushMatrix();
	glTranslatef(50, 80.0, -235.0);
	glScalef(4.0, 8, 4);
	glColor3f(0.5, 0, 0);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, -1, 0, 0);
	glutSolidCone(5.0, 8, 24, 24);
	glPopMatrix();

	//costume white
	glPushMatrix();
	glTranslatef(50, 80.0, -235.0);
	glScalef(4.5, 2, 4.5);
	glColor3f(0.5, 0.5, 0.5);
	glTranslatef(0.0, 0.0, -6.5);
	/*glRotatef(90.0, -1, 0, 0);*/
	glutSolidSphere(5.0, 8, 24);
	glPopMatrix();


	//arm
	glPushMatrix();
	glTranslatef(50, 140, -245.0);
	glScalef(4.0, 8, 4);
	glColor3f(0.5, 0, 0);
	glTranslatef(0.0- arm * 0.05, 0.0 , -6.5 );
	glRotatef(90.0, 1, 0 + arm * 0.005, 0);
	glutSolidCone(2.0,8, 24, 24);
	glPopMatrix();

	

	//gift packet
	glPushMatrix();
	glTranslatef(-10, 88.0, -238.0);
	glScalef(8, 8, 5);
	glColor3f(0, 0.3, 0.2);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, -1, 1.5,0);
	glutSolidCone(4.0,8, 24, 24);
	glPopMatrix();


	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}
void barrier() {
	glPushMatrix();
	glTranslatef(0, 0.0 - jump, fltBuildingOffset);
	
	GLfloat b2_diffuse[] = { 0,0,0,1 };
	GLfloat b2_ambient[] = { 0.4,0.4,0.4,1 };
	GLfloat b2_emission[] = { 0,0,0,1 };
	GLfloat b2_shininess[] = { 50 };
	GLfloat mat_specular[] = { 0, 0,0, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, b2_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, b2_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, b2_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, b2_shininess);

	glPushMatrix();
	glTranslatef(-50, -8, -50);
	glColor3f(0.7, 0.7, 0.7);
	glScalef(5, 1.6, 1.6);
	glutSolidSphere(20, 5, 5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(50, -8,450);
	glColor3f(0.7, 0.7, 0.7);
	glScalef(5, 1.6, 1.6);
	glutSolidSphere(20, 5, 5);
	glPopMatrix();

	glPopMatrix();
}
void snowBall() {
	glPushMatrix();
	glTranslatef(0, 0, 0);


	//snowball
	glPushMatrix();
	glTranslatef(-50.0+ball * fltXRef*0.01, 100.0-ball*0.5, -350.0+ball*2);
	/*glScalef(18.0, 0.25, 1.2);*/
	glColor3f(0.8, 0.8, 0.8);
	glTranslatef(0.0, 0.0, -6.5);
	glRotatef(90.0, 0, 1.0, 0.0);
	glutSolidSphere(10.0, 18.0, 8);
	glPopMatrix();

	glPopMatrix();
}
/////////////////////////////////////////////////////////

//display
void displayObject()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fltFOV, 1, 0.1, 5000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(fltX0 * fltZoom, fltY0 * fltZoom, fltZ0 * fltZoom, fltXRef, fltYRef, fltZRef, fltXUp,
		fltYUp, fltZUp);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2, 0.2, 0.8, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Christmas town
	if (statescene == 0) {
		//get on/off the sled
		if (viewstage%2== 1){
			glTranslatef(-sledingx, -sledingy * 0.3 + jump-20, -fltTreeOffset + sledingy * 0.3-500);
		}

		//snow
		glPushMatrix();
		glTranslatef(-350, 0, -100);
		for (int i = 0; i < 100; i++) {
			glPushMatrix();
			glTranslatef(randomNum[i][0] * 2, randomNum[i][1] - snowfall, randomNum[i][2]);
			snow();
			glPopMatrix();
		}
		glPopMatrix();

		//snowball
		if (snowball == true) {
			glPushMatrix();
			snowBall();
			glPopMatrix();
		}
		

		//snow barriers
		glPushMatrix();
		barrier();
		glPopMatrix();

		//sled
		glPushMatrix();
		glTranslatef(-200, 0, 280);
		sled();
		glPopMatrix();

		//window
		glPushMatrix();
		window();
		glPopMatrix();

		//Christmas wreath
		glPushMatrix();
		glTranslatef(0, 0, -100);
		glPushMatrix();
		glTranslatef(-20.0, 210.0, 1480.0);
		Torus();
		glPopMatrix();


		//main house
		glPushMatrix();
		glTranslatef(350.0, 0.0, 0);
		house();
		glPopMatrix();

		//hint
		if (fltAcceleration == 0) {
			glPushMatrix();
			glColor3f(0.8, 0.8, 0.8);
			glTranslatef(-40.0,-20, 0);
			TextOut(150, 30, "'w','s'to control the movement");
			glPopMatrix();
		}
		if (fltAcceleration == 0) {
			glPushMatrix();
			glColor3f(0.8, 0.8, 0.8);
			glTranslatef(0.0, 20, 0);
			TextOut(150, 30, "Click the left mouse button to make the sled slide.");
			glPopMatrix();
		}
		if (fltAcceleration <= -1000) {
			glPushMatrix();
			glColor3f(0.8, 0.8, 0.8);
			glTranslatef(-80.0, -20, 0);
			TextOut(150, 30, "press 'c'to enter the room");
			glPopMatrix();
		}
		if (jumpstage == 1) {
			glPushMatrix();
			glColor3f(0.8, 0.8, 0.8);
			glTranslatef(-10.0, -20, 0);
			TextOut(150, 30, "Press the spacebar to jump over the snow barrier");
			glPopMatrix();
		}
		
	

		//trees
		glPushMatrix();
		glTranslatef(-300.0, 0.0, 0);
		treestate = 1;
		Tree();
		treestate = 0;
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-350.0, 0.0, 500);
		treestate = 2;
		Tree();
		treestate = 0;
		glPopMatrix();
		glPushMatrix();
		glTranslatef(150.0, 0.0, 350.0);
		Tree();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(350.0, 0.0, 1050.0);
		treestate = 2;
		Tree();
		treestate = 0;
		glPopMatrix();
		glPushMatrix();
		glTranslatef(550.0, 80.0, 350.0);
		Tree();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-1550.0, 220.0, 1350.0);
		Tree();
		glPopMatrix();

		//snowman
		glPushMatrix();
		glTranslatef(300.0, 0.0, 1000.0);
		snowMan();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-250.0, -20.0, 450.0);
		snowmanstate = 1;
		snowMan();
		snowmanstate = 0;
		glPopMatrix();

		//lamppost
		glPushMatrix();
		glTranslatef(0.0, 0.0, 150.0);
		lampPost();
		glPopMatrix();

		//snowy ground
		glPushMatrix();
		snowGround();
		glPopMatrix();

		//small houses
		glPushMatrix();
		glTranslatef(-2200.0, 410.0, 480.0);
		smallhouse();
		glPopMatrix();
		glPushMatrix();
		housestate = 1;
		glTranslatef(3200.0, 250.0, 480.0);
		smallhouse();
		housestate = 0;
		glPopMatrix();
		glPushMatrix();
		housestate = 2;
		glTranslatef(2800.0, 250.0, -1880.0);
		smallhouse();
		housestate = 0;
		glPopMatrix();
		glPushMatrix();
		housestate = 1;
		glTranslatef(0.0, 1250.0, 1280.0);
		smallhouse();
		housestate = 0;
		glPopMatrix();

		glPopMatrix();

		/////////////////out of the stack
		//ground
		glPushMatrix();
		ground();
		glPopMatrix();

		//background
		glPushMatrix();
		Background();
		glPopMatrix();
		//////////////////

		glPopMatrix();
		glutSwapBuffers();
	}
	//
	else{
	glPushMatrix();
	glTranslatef(350.0, 0.0, -2000);

	//Christmas wreath
	glPushMatrix();
	glTranslatef(240.0, 210.0, 2480.0);
	Torus();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-20.0, 210.0, 2480.0);
	Torus();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-280.0, 210.0, 2480.0);
	Torus();
	glPopMatrix();

	//fireplace
	glPushMatrix();
	glTranslatef(-200, -50, 500);
	Fireplace();
	glPopMatrix();

	//Christmas tree
	glPushMatrix();
	glTranslatef(0, -50, 2000);
	TreeIN();
	glPopMatrix();

	//sofa
	glPushMatrix();
	glTranslatef(0, -50, 2200);
	sofa();
	glPopMatrix();

	//table&cups
	glPushMatrix();
	glTranslatef(-100, 0, 2300);
	table();
	glPopMatrix();

	//main house with wall paper
	glPushMatrix();
	housein();
	glPopMatrix();

	//light
	glPushMatrix();
	glTranslatef(-100, 0, 2300);
	light();
	glPopMatrix();

	glPopMatrix();
	glutSwapBuffers();
	
	}
	
}

void reshapeWindow(GLint intNewWidth, GLint intNewHeight)
{
	glViewport(0, 0, intNewWidth, intNewHeight);
}

//mouse interaction
void mouse_input(int button, int state, int x, int y) // mouse interactions
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN&& statescene == 0) {
		carstate = 1;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && statescene == 0) {
		
		viewstage += 1;
	}
}

//keyboard interaction
void keyboard_input(unsigned char key, int x, int y)
{

	//move forward
	if (key == 'w' || key == 'W') {
		if (statescene == 0 && fltAcceleration >= -1750 && jumpstage == 0)
			fltAcceleration -= 10; //Speed up
		if (statescene == 0 && fltAcceleration == -400 && jumpstage == 0)
			jumpstage = 1;

		if (statescene == 0 && fltAcceleration == -900 && jumpstage == 0)
			jumpstage = 1;

		if (statescene == 1 && fltAcceleration >= -550)
			fltAcceleration -= 20; //Speed up
	}
	//move backward
	if (key == 's' || key == 'S') {
		if (statescene == 0 && fltAcceleration <= 0)
			fltAcceleration -= -20; //Speed down

		if (statescene == 1 && fltAcceleration <= -5)
			fltAcceleration -= -20; //Speed down
	}

	//look left
	if (key == 'a' || key == 'A')
		fltXRef += cameraD;
	//look right
	if (key == 'd' || key == 'D')
		fltXRef -= cameraD;

	//jump
	if (key == ' ') {
		JUMP = true;
		jumpstage = 0;

	}

	//throw snow ball
	if (key == 'j' || key == 'J') {
		snowball = true;
		ball = 0;
	}


	//Enter the room
	if (key == 'c') {
		statescene = 1;
		fltAcceleration = 0;
		fltY0 = 120.0;
		fltZ0 = -500.0;
		fltXRef = 0.0;
		fltZoom = 1.0;
	}
	//Exit the room
	if (key == 'v') {
		statescene = 0;
		fltAcceleration = -1750;

	}
	//look up
	if (key == 'u' || key == 'U')
		fltYRef += 2;/*cameraD;*/
	//look down
	if (key == 'i' || key == 'I')
		fltYRef -= 2;/*cameraD;*/


	//fly up/down
	if (key == 'g' || key == 'G')
		if (fltY0 > 35)
			fltY0 -= 5; //Camera down
	if (key == 'f' || key == 'F')
		if (fltY0 < 400)
			fltY0 += 5; //Camera up

	//zoom in/out
	if (key == 'n' || key == 'N')
		if (fltZoom > 0.8)
			fltZoom -= 0.1; //Zoom in
	if (key == 'm' || key == 'M')
		if (fltZoom < 1.2)
			fltZoom += 0.1; //Zoom out
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(intWinWidth, intWinHeight);
	glutCreateWindow("Christmas");
	myinit();
	glutTimerFunc(time_interval, OnTimer, 1);
	glutDisplayFunc(displayObject);
	glutReshapeFunc(reshapeWindow);
	glutMouseFunc(mouse_input);
	glutKeyboardFunc(keyboard_input);
	glutIdleFunc(animate); 
	glutMainLoop();
}

