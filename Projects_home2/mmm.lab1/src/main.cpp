#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include "../../DPIntegrator/src/dpintegrator.h"
#define W 1280
#define H 1024

int window;

struct Gnomon
{
	Vect<double> V;
	double l;
};

class Globe
{
 public:
	Gnomon IG, G;
	Globe(double R, double _l, double _alpha, double _beta )
	{
		G.V = Vect<double>(R*sin(_beta)*sin(_alpha),R*cos(_beta),R*sin(_beta)*cos(_alpha));
		G.l = _l;
		IG = G;
	}
	void Offset( double X, double Y, double Z )
	{
		G.V[V_X] = IG.V[V_X] + X;
                G.V[V_Y] = IG.V[V_Y] + Y;
                G.V[V_Z] = IG.V[V_Z] + Z;
	}
	void Rotate( double t )
	{
		Quat<double> res,q,qm1,tmp;
                double angle;
                angle =  t * 24.0 * 60.0 * 60.0 * 7.292115*10e-6 / 2 ;
                q = Quat <double> (cos(angle),Vect<double>(0,sin(angle),0));
                qm1 = Quat <double> (cos(angle), Vect<double>(0,-sin(angle),0));
                tmp = Quat <double> (0,Vect<double>(IG.V));
                res = q * tmp * qm1;
                IG.V = res.V;
	}
};

EilerIntegrator<double> *Integr;
vector<double> daytime;
vector<double> shadowarr;

void onDraw()
{
	glBegin(GL_LINES);
		glVertex2f(100,H/2.0);
		glVertex2f(1200,H/2.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
		for(int i = 1; i < daytime.size(); i++)
		glVertex2f(100+i/(double)daytime.size()*1100, -daytime[i]*100.0+H/2.0);
		glEnd();
	glColor3f(1.0,1.0,0.0);
	glBegin(GL_LINE_STRIP);
		for(int i=0; i < shadowarr.size(); i++)
		glVertex2f( 100+i/(double)shadowarr.size()*1100, -fabs(shadowarr[i])+H-100 );
	glEnd();
	
	glutSwapBuffers();
}

void onResize(int Width, int Height)
{
	glMatrixMode(GL_PROJECTION);
	glOrtho(0,Width,Height,0,0,1);
	glMatrixMode(GL_MODELVIEW);
}

void InitGL(int Width, int Height)
{
	glClearColor(0,0,0.07,0);
	onResize(Width, Height);
}

void onIdle(void)
{
	glutPostRedisplay();
}

KeplerModel<double>* Kepl;
Vect<double> PV;
double an, shadow, timer;
Vect<double> GVect, SunVect;
int main(int argc, char** argv)
{
	Kepl = new KeplerModel<double>;
	Integr = new EilerIntegrator<double>(Kepl, 0, 365, 1/192.0);
	PV = Vect<double>(3);
	Globe Earth( 6378.137,10,M_PI/4.0,M_PI/4.0 );
	while( Integr->getT() <= Integr->getTk() )
	{
		Integr->NextStep();
                PV = Integr->PhaseVect() * 149597870.66;
                Earth.Offset(PV[V_X],PV[V_Y],PV[V_Z]);
                Earth.Rotate(Integr->getStep());
                GVect = Earth.IG.V * (1 / Earth.IG.V.Length()) * ( Earth.IG.l + Earth.IG.V.Length() );
                SunVect = PV + GVect;
                GVect = GVect * (1 / GVect.Length()) *( Earth.IG.l * (-1));
                
                if((an > 0.2) && (an < (M_PI/2.0-0.2)))
                        {	
                        shadow = Earth.IG.l*tan(acos((GVect*SunVect /(GVect.Length()*SunVect.Length()) )));
                                timer+=Integr->getStep();
				shadowarr.push_back(fabs(shadow));
                        }
                else
                        if( timer!=0 )
                        {
                                daytime.push_back(timer);
                                timer = 0;
                        }

	}
	
	glutInit(&argc,argv);
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
        glutInitWindowSize(W, H);
        glutInitWindowPosition(100, 100);

        window = glutCreateWindow("OpenGLTutor");
        glutDisplayFunc(&onDraw);
        glutIdleFunc(&onIdle);
        glutReshapeFunc(&onResize);
	
	InitGL(W,H);
	glutMainLoop();
	delete Kepl;
	delete Integr;
	return 0;
}
