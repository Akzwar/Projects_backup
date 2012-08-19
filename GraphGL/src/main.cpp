#include"../../utils/GLext.h"
//#include"graphgl.h"
#include"../../utils/graphgl.h"
#include<iostream>
#include"../../utils/integrator.h"
GraphGL2D<double> *fgraph;
GraphGL2D<double> *sgraph;

ShapingFilter<double>* model;
EilerIntegrator<double>* integr;

double x(-3*M_PI);
//double x(0);
void onDraw(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    char Text[20];
    fgraph->Draw();
    sgraph->Draw();
    sprintf(Text,"%f",integr->T());
    TextOut(-500,250,Text);
    glutSwapBuffers();
}

void onKey(unsigned char Key, int X, int Y)
{
    OGLWindow<0,0,0>::onKey(Key,X,Y);
    if(Key == 'q')
    {
        //while(x<(10*M_PI))
    }
}

void onIdle(void)
{
    integr->NextStep();
    fgraph->AddDot(integr->T(),integr->PhaseVect()[0]);
    sgraph->AddDot(x+=0.1,rand()/(double)RAND_MAX*10-5);
    glutPostRedisplay();
}

int main( int argc, char** argv )
{
    model = new ShapingFilter<double>(10,10,0);
    integr = new EilerIntegrator<double>(model,0,200,1e-3);
    OGLWindow<1200,800,M_2D> mygl(argc,argv,"GraphGL");
    fgraph = new GraphGL2D<double>(-350,-250,800,300);   
    sgraph = new GraphGL2D<double>(-350,50,800,300);   
    glLineWidth(3);
    mygl.setDisplayFunc(&onDraw);
    mygl.setKeyboardFunc(&onKey);
    mygl.setIdleFunc(&onIdle);
    mygl.Show();
    printf("\tglutMainLoop stoped\n");
    delete fgraph;
	return 0;
}
