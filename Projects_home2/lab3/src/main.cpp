#define GL_GLEXT_PROTOTYPES
#define STATICFPS 60.0f

#include "viewsys.h"

#define FULLSCREEN 1

#ifndef ESCKEY
#define ESCKEY 27
#endif//ESCKEY

#ifndef STARTWINX
#define STARTWINX 1920
#endif//STARTWINX

#ifndef STARTWINY
#define STARTWINY 1200
#endif//STARTWINY

int main( int arg, char *argv[] )
{ 
    GLViewSystem::init( arg, argv, STARTWINX, STARTWINY, FULLSCREEN );
    GLMainView *v = new GLViewLab3;
    GLViewSystem::setMainView( v );
    GLViewSystem::start( GLViewSystem::WITH_INPUT_FILE );
    delete v;
    return GLViewSystem::exitCode();
}
