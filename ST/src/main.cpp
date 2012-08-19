#include <stdio.h>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <unistd.h>
#include <math.h>
#include <vector>
#include "texgl.h"
#include "../../utils/quat.h"
#define M_2D 0
#define M_3D 1

int W=1280;
int H=1024;

double points[1000][3];

GLuint texture1, texture2;
tga_tex texturedata;
tga_tex texturedata1;

int CurrMode;

int window;

Vect<double> CameraPos(3,0.0,0.0,255.0);
Vect<double> CameraView(3,0.0,0.0,-255.0);

double sqr( double Val )
{
    return Val * Val;
}

int sign( double Value )
{
    if( Value < 0 )
        return -1;
    if( Value > 0 )
        return 1;
    return 0;
}

struct Quad
{
    double X,Y,Width,Height;
};

class DisplayObj
{
 public:
    Quad Geometry;
    virtual void Draw();
    virtual void onClick();
};

void ReinitCamera()
{
    gluLookAt( CameraPos[V_X], CameraPos[V_Y], CameraPos[V_Z],
                CameraPos[V_X]+CameraView[V_X],CameraPos[V_Y]+CameraView[V_Y],CameraPos[V_Z]+CameraView[V_Z],
                0,-1,0);
}

void Draw3D()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 70, W/(double)H, 0.01, 10000 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    ReinitCamera();
    glMatrixMode(GL_PROJECTION);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glBindTexture(GL_TEXTURE_2D,texture1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,texturedata.ptr());
    glPointSize(3);
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_POINTS);
        for(int i = 0; i<1000; i++)
        glVertex3f(points[i][0], points[i][1], points[i][2]);
    glEnd();
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-128,-128,128);
        glTexCoord2f(1,0); glVertex3f(128,-128,128);
        glTexCoord2f(1,1); glVertex3f(128,128,128);
        glTexCoord2f(0,1); glVertex3f(-128,128,128);
    glEnd();
    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-128,-128,-128);
        glTexCoord2f(1,0); glVertex3f(128,-128,-128);
        glTexCoord2f(1,1); glVertex3f(128,128,-128);
        glTexCoord2f(0,1); glVertex3f(-128,128,-128);
    glEnd();
    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(128,-128,-128);
        glTexCoord2f(1,0); glVertex3f(128,-128,128);
        glTexCoord2f(1,1); glVertex3f(128,128,128);
        glTexCoord2f(0,1); glVertex3f(128,128,-128);
    glEnd();
     glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-128,-128,-128);
        glTexCoord2f(1,0); glVertex3f(-128,-128,128);
        glTexCoord2f(1,1); glVertex3f(-128,128,128);
        glTexCoord2f(0,1); glVertex3f(-128,128,-128);
    glEnd();
     glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-128,-128,128);
        glTexCoord2f(1,0); glVertex3f(128,-128,128);
        glTexCoord2f(1,1); glVertex3f(128,-128,-128);
        glTexCoord2f(0,1); glVertex3f(-128,-128,-128);
    glEnd();
     glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-128,128,128);
        glTexCoord2f(1,0); glVertex3f(128,128,128);
        glTexCoord2f(1,1); glVertex3f(128,128,-128);
        glTexCoord2f(0,1); glVertex3f(-128,128,-128);
    glEnd();
}

void Draw2D()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0,W,H,0,0,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //glColor4f(0.0,1.0,1.0,1.0);
    glBindTexture(GL_TEXTURE_2D,texture1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,texturedata.ptr());
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2f(0,0);
        glTexCoord2f(1,0); glVertex2f(W,0);
        glTexCoord2f(1,1); glVertex2f(W,H);
        glTexCoord2f(0,1); glVertex2f(0,H);
    glEnd();
    glBindTexture(GL_TEXTURE_2D,texture2);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,texturedata1.ptr());
    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2f(100,100);
        glTexCoord2f(1,0); glVertex2f(356,100);
        glTexCoord2f(1,1); glVertex2f(356,356);
        glTexCoord2f(0,1); glVertex2f(100,356);
    glEnd();
}

void DrawGLScene()
{
    if( CurrMode == M_2D )
        Draw2D();
    if( CurrMode == M_3D )
        Draw3D();
 
    glutSwapBuffers(); 	
}

bool IsDrag_Left;
double pVY, pVX, pVZ, pX, pY;

void onMouseClick(int button, int state, int X, int Y)
{
	if( button == GLUT_LEFT_BUTTON )
	{
		if( state == GLUT_DOWN )
		{
			//IsDrag_Left = true;
			pX = X;
			pY = Y;
		}
		//if( state == GLUT_UP )
			//IsDrag_Left = false;
	}
	/*if( button == GLUT_MIDDLE_BUTTON )
	{
		if( state == GLUT_DOWN )
		{
			IsDrag_Middle = true;
			prevX = x;
			prevY = y;
		}
		if( state == GLUT_UP )
			IsDrag_Middle = false;
	}*/
}


void onMouseMove( int X, int Y )
{
        int kX(500), kY(500);
        int dX(X-pX), dY(Y-pY);
        
        CameraView.print();
        double Angle = dX / (double)kX;
        CameraView = Rotate( CameraView, Angle, Vect<double>(3, 0.0, 1.0, 0.0 ) );
        Angle = -dY / (double)kY;
        CameraView = Rotate( CameraView, Angle, Vect<double>(3, 0.0, 1.0, 0.0 ) % CameraView );
        if( fabs( CameraView[V_X] ) <= 1 && fabs( CameraView[V_Z] ) <= 1 )
        {
            CameraView[V_X] = pVX;
            CameraView[V_Y] = pVY;
            CameraView[V_Z] = pVZ;
        }
        pX = X;
        pY = Y;
        pVX = CameraView[V_X];
        pVY = CameraView[V_Y];
        pVZ = CameraView[V_Z];
        //CameraView = CameraView / CameraView.Length() * 255;
}

void keyPressed(unsigned char key, int x, int y)
{
    if( key == 'g' )
        CurrMode = ( CurrMode == M_2D )?M_3D:M_2D;
    if( key == 27 )
        exit(0);
    if( key == 'w' )
    {
        CameraPos += CameraView * 0.02;
        CameraView += CameraView * 0.02;
        CameraView = CameraView / CameraView.Length() * 255;
    }
    if( key == 's' )
    {
        CameraPos += CameraView * -0.02;
        CameraView += CameraView * -0.02;
        CameraView = CameraView / CameraView.Length() * 255;
    }
    if( key == 'a' )
    {
        CameraPos.print();
        Vect<double> tmp = Vect<double>(3,0.0,1.0,0.0) % Vect<double>(3,CameraView[V_X],0.0,CameraView[V_Z]) ;
        tmp.print();
        CameraPos += tmp / tmp.Length() * 2;
        CameraView += tmp / tmp.Length() * 2;
        CameraView = CameraView / CameraView.Length() * 255;
    }
    if( key == 'd' )
    {
        CameraPos.print();
        Vect<double> tmp =  CameraView % Vect<double>(3,0.0,1.0,0.0) ; 
        tmp.print();
        CameraPos += tmp / tmp.Length() * -2;
        CameraView += tmp / tmp.Length() * -2;
        CameraView = CameraView / CameraView.Length() * 255;
    }
    if( key == 'o' )
        glEnable(GL_ALPHA_TEST);
    if( key == 'p' )
        glDisable(GL_ALPHA_TEST);
}

void ReSizeGLScene(int Width, int Height)
{
	glViewport(0,0,Width,Height);	
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    if( CurrMode == M_2D )
	    glOrtho(0,W,H,0,0,1);
    if( CurrMode == M_3D )
        gluPerspective( 70, W/(double)H, 0.1, 1000 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
    ReinitCamera();
    W = Width;
    H = Height;
}

void InitGL(int Width, int Height)
{
	glClearColor( 0.0,0.0,0.2,1.0 );
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.0);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	ReSizeGLScene(Width, Height);
	glPointSize(3.0);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,texturedata.info.Width,texturedata.info.Height,0,GL_RGBA,GL_UNSIGNED_BYTE,texturedata.ptr());
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,texturedata1.info.Width,texturedata1.info.Height,0,GL_RGBA,GL_UNSIGNED_BYTE,texturedata1.ptr());
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}

void onIdle(void)
{
	glutPostRedisplay();
} 

int main(int argc, char **argv)
{

    for(int i = 0; i<1000; i++)
    {
        points[i][0] = rand()/(double)RAND_MAX * 1000 - 500;
        points[i][1] = rand()/(double)RAND_MAX * 1000 - 500;
        points[i][2] = rand()/(double)RAND_MAX * 1000 - 500;
    }
    CurrMode = M_3D;
    texturedata.LoadfromFile("first.icb");
    texturedata1.LoadfromFile("first.icb");
	printf("GL inititialization start...\n");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(100, 100);
	window = glutCreateWindow("OpenGLTutor");
    glutFullScreen();
	glutDisplayFunc(&DrawGLScene);
	glutIdleFunc(&onIdle);
	glutReshapeFunc(&ReSizeGLScene);
	glutKeyboardFunc(&keyPressed);
	glutMouseFunc(onMouseClick);
	glutMotionFunc(onMouseMove);
	InitGL(W,H);
	printf("GL Init done...\n");
	glutMainLoop();
	return 0;
}
