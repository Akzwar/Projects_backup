#include <stdio.h>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <unistd.h>
#include <math.h>
#include "../../DPIntegrator/src/dpintegrator.h"
#include <vector>
#include "../../../Projects/utils/graphgl.h"

#define M_3D 0
#define M_2D 1

int W=1280;
int H=1024;

GraphGL2D<double> G1, G2, G3;

struct { double MO,D,MOs,Ds; } info[3];
struct dot4d
{
    double X[3];
    double time;
};
int sign( double Val )
{
    if( Val > 0 )
        return 1;
    if( Val < 0 )
        return -1;
    return 0;
}

using namespace std;

int window;
Model<double>* model;
EilerIntegrator<double>* Integr;
Quat<double> rotVect,Q,Qm1;
vector<dot4d> PV[4];
int mode;
int  l1,l2,l3,l4;

void ReinitCamera()
{	
	gluLookAt(rotVect.V[V_X], rotVect.V[V_Y], rotVect.V[V_Z],
 		0,0,0, 
		0,1,0);
}

void Draw3D()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0,W/(double)H,0.01,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    ReinitCamera();	

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


void Draw2D()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,W,H,0,0,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glColor3f(1.0,1.0,1.0);
    double Ky1 = 50;
    double Ky2 = 7;
    double Ky3 = 10;
    glBegin(GL_LINE_STRIP);
        for( int i = 0; i < PV[0].size() ; i ++ )
            glVertex2f( 100 + i/(double)PV[0].size()*(W-200), 
                        H/2.0 - 200 - PV[0][i].X[0]*Ky1);
    glEnd();
    glColor3f(0.0,1.0,1.0);
    glBegin(GL_LINE_STRIP);
        for( int i = 0; i < PV[0].size() ; i ++ )
            glVertex2f( 100 + i/(double)PV[0].size()*(W-200), 
                        H/2.0 - PV[0][i].X[1]*Ky2);
    glEnd();
    glColor3f(0.0,0.0,1.0);
    glBegin(GL_LINE_STRIP);
        for( int i = 0; i < PV[0].size() ; i ++ )
            glVertex2f( 100 + i/(double)PV[0].size()*(W-200), 
                        H/2.0 + 200 - PV[0][i].X[2]*Ky3);
    glEnd();

}

void DrawGLScene()
{
    if(mode == M_2D)
        Draw2D();
    if(mode == M_3D)
        Draw3D();
 
    glutSwapBuffers(); 	
}

bool IsDrag_Left;

int prevX, prevY;
void onMouseClick(int button, int state, int x, int y)
{
	if( button == GLUT_LEFT_BUTTON )
	{
		if( state == GLUT_DOWN )
		{
			IsDrag_Left = true;
			prevX = x;
			prevY = y;
		}
		if( state == GLUT_UP )
			IsDrag_Left = false;
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

Vect<double> rotAxis;
void onMouseMove( int X, int Y )
{
    if( IsDrag_Left )
    {
    	int kx = 500;
    	int ky = 500;
    	int dX = X - prevX;
    	int dY = Y - prevY;
    		double Angle = -(double)dX / kx;
    		Q = Quat<double>(cos(Angle / 2.0 ),Vect<double>(0,sin(Angle / 2.0 ),0));
    		Qm1 = Quat<double>(cos(Angle / 2.0),Vect<double>(0,-sin(Angle / 2.0 ),0));
    		rotVect = Q * rotVect * Qm1;
    		Angle = -(double)dY / ky;
            rotAxis = Vect<double>(0,1,0) % rotVect.V;
    		Q = Quat<double>(cos(Angle / 2.0 ), rotAxis * (1 / rotAxis.Length()) * sin(Angle / 2.0) );
    		Qm1 = Quat<double>(cos(Angle / 2.0), rotAxis * (1 / rotAxis.Length()) * ( - sin(Angle / 2.0 ) ) );
    		rotVect = Q * rotVect * Qm1;
            if( fabs(rotVect.V[V_X]) <= 0.01 && fabs(rotVect.V[V_Z]) <= 0.01 )
            {
                rotVect.V[V_X] = sign(rotVect.V[V_X]) * 0.01;
                rotVect.V[V_Z] = sign(rotVect.V[V_Z]) * 0.01;
            }
    	prevX = X;
    	prevY = Y;
}

}

void keyPressed(unsigned char key, int x, int y)
{
	if( key == 's' )
		rotVect.V = rotVect.V * 1.1;
	if( key == 'w' )
		rotVect.V = rotVect.V * 0.9;	
    if( key == 'g' )
        if(mode == M_3D)
            mode = M_2D;
        else
            mode = M_3D;
    if( key == 27 )
        exit(0);
}

void ReSizeGLScene(int Width, int Height)
{
	glViewport(0,0,Width,Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, Width/(double)Height, 0.01, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ReinitCamera();
    W = Width;
    H = Height;
}

void InitGL(int Width, int Height)
{
	glClearColor( 0,0,0.07,0 );
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	ReSizeGLScene(Width, Height);
	//glEnableClientState(GL_VERTEX_ARRAY);
	glPointSize(3.0);

}

char PVi;
dot4d d;
void onIdle(void)
{
    if( l1 < PV[0].size() ){l1+=1;}
    if( l2 < PV[1].size()-30 ){l2+=30;}
    if( l3 < PV[2].size()-30 ){l3+=30;}
    if( l4 < PV[3].size()-30 ){l4+=30;}
    /*Integr->NextStep();
    d.X[0] = Integr->PhaseVect()[0];
    d.X[1] = Integr->PhaseVect()[1];
    d.X[2] = Integr->PhaseVect()[4];
    d.time = Integr->getStep();
    PV[0].push_back(d);
    printf("%f\n",Integr->getT());
    */
	glutPostRedisplay();
} 

/*
double Ks( double MO, double D )
{
    int zps = 120;
    if( PV[C].size() < zps )
        return 0;
    double dt = 0;
    int rp = 0;
    int kk = count - zps;
    double res;
    double summ;
    dot4d buff;
    for( int k =0; k < kk; k++ )
    {
        summ = 0;
        for( int s = 0; s < count - k; s++ )
        {
            summX += ( PV[s].X[C] - res[0].MO ) * (PV[s+k].X[C] - res[0].MO);    
        }
        buff.time = PV[k].time;
        double r;
        buff.X[0] = 

    }
}
*/
void IntegrateModel()
{
    double MO, D, MOs, Ds;
    dot4d d;
    PV[PVi].clear();
    while(Integr->getT()<=Integr->getTk())
	    {
		    Integr->NextStep();
            //Integr->PhaseVect().print();
            d.X[0] = Integr->PhaseVect()[0];
            d.X[1] = Integr->PhaseVect()[1];
            d.X[2] = Integr->PhaseVect()[4];
            d.time = Integr->getStep();
            PV[PVi].push_back(d);
	    }
  
    for( int j = 0; j < 3; j++ )
    {
        double summ = 0;
        for( int  i = 0; i < PV[0].size(); i++ )
            summ += PV[0][i].X[j];
        MO = summ/(double)PV[0].size();
        printf("MO[%d] : %f\n",j,MO);
        info[j].MO = MO;
        summ = 0;
        for( int i = 0; i < PV[0].size(); i++ )
            summ += pow( PV[0][i].X[j] - MO, 2 );
        D = summ / (double)(PV[0].size() - 1);
        info[j].D = D;
        printf("D[%d] : %f\n",j,D);
        MOs = sqrt( D / (double)PV[0].size() );
        info[j].MOs = MOs;
        printf("MOs[%d] : %f\n",j,MOs);
        Ds = sqrt( 2.0 / (double)PV[0].size() ) * D; 
        info[j].Ds = Ds;
        printf("Ds[%d] : %f\n",j,Ds);

        printf("\n");
    }
}

int main(int argc, char **argv)
{
    mode = M_2D;
    l1 = l2 = l3 = l4 = 0;
	model = new ShapingFilter<double>(0.05,100,M_PI);
	Integr = new EilerIntegrator<double>(model,0,200,5.0e-3);
    IntegrateModel();
	rotVect = Quat<double>(0,Vect<double>(0,0,0.3));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(100, 100);

	window = glutCreateWindow("OpenGLTutor");
	glutDisplayFunc(&DrawGLScene);
	glutIdleFunc(&onIdle);
	glutReshapeFunc(&ReSizeGLScene);
	glutKeyboardFunc(&keyPressed);
	glutMouseFunc(onMouseClick);
	glutMotionFunc(onMouseMove);
	InitGL(W,H);
	glutMainLoop();
	delete Integr;
	delete model;
	return 0;
}
