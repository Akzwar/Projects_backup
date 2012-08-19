#include "simulator.h"

Simulator S;
OGLWindow<800,600,M_3D> W;

void Draw(void)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    S.Draw();
    glutSwapBuffers();
}

int main( int argc, char** argv )
{
    W = OGLWindow<800,600,M_3D>(argc,argv,"AirSimulator");
    W.Show();
}

