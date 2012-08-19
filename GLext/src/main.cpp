#include "GLext.h"

OGLWindow<1200,800,M_3D> Window;

int main( int argc, char** argv )
{
    Window = OGLWindow<1200,800,M_3D>(argc,argv,"GLext");
    Window.Show();
}
