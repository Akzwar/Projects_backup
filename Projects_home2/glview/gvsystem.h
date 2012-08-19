#ifndef GVSYSTEM_H
#define GVSYSTEM_H

#include "gvstate.h"
#include "gvarea.h"
#include "gvtempl.h"

class GLMainView: public GLViewHandler
{ public: virtual void readFile( const char * fname ) = 0; };

class GLViewSystem
{
protected:

    static void reshape( int w, int h )
    {
        if( !inited || !vh ) return;
        vh->reshape( 0, 0, w, h );
        glViewport( 0, 0, w, h );
        glutPostRedisplay();
    }

    static void draw()
    {
        if( !inited || !vh ) return;
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        vh->draw();
        glutSwapBuffers();
    }

    static void key( unsigned char k, int x, int y )
    {
        if( !inited || !vh ) return;
        if( k == esccode ) exit(0);
        else vh->keyboard( k, x, y );
        glutPostRedisplay();
    }

    static void mousePMotion( int x, int y )
    {
        if( !inited || !vh ) return;
        vh->passive( x, y );
        glutPostRedisplay();
    }

    static void mouseMotion( int x, int y )
    {
        if( !inited || !vh ) return;
        vh->motion( x, y );
        glutPostRedisplay();
    }

    static void mousePress( int btn, int st, int x, int y )
    {
        if( !inited || !vh ) return;
        vh->mouse( btn, st, x, y );
        glutPostRedisplay();
    }

    static void idle()
    {
        if( !inited || !vh ) return;
        vh->idle();
        //glutPostRedisplay();
    }

    static void redisplayTimer( int x )
    {
        if( !inited || !vh ) return;
        glutPostRedisplay();
        vh->timer( x );
        #ifdef STATICFPS
        glutTimerFunc( 1.0f/STATICFPS, redisplayTimer, x );
        #else
        glutTimerFunc( vh->getTimerDelay( x ), redisplayTimer, x );
        #endif//STATICFPS
    }
    GLViewSystem() {}

    static int arg;
    static char **argv;
    static bool inited;
    static unsigned char esccode;
    static GLMainView *vh;

public:
    typedef unsigned long ulong;

    static const ulong WITH_INPUT_FILE = 0x1;

    static void init( int Arg, char *Argv[],
                      int stwinx=1920, int stwiny=1200,
                      bool fullscreen = 1,
                      unsigned char escCode = 27
                    )
    {
        if( inited ) return;
        arg = Arg;
        argv = Argv;
        esccode = escCode;
        glutInit( &arg, argv );
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
        glutInitWindowSize( stwinx, stwiny );
        glutCreateWindow( argv[0] );
        if( fullscreen )
            glutFullScreen();
        glutDisplayFunc( GLViewSystem::draw );
        glutReshapeFunc( GLViewSystem::reshape );
        glutKeyboardFunc( GLViewSystem::key );
        glutIdleFunc( GLViewSystem::idle );
        glutMotionFunc( GLViewSystem::mouseMotion );
        glutPassiveMotionFunc( GLViewSystem::mousePMotion );
        glutMouseFunc( GLViewSystem::mousePress );
        glutTimerFunc( 0, GLViewSystem::redisplayTimer, 0 );
        glClearColor( 0.0, 0.0, 0.0, 0.0 );
        inited = 1;
    }

    static void setMainView( GLMainView *st ) { if( st ) vh = st; }
    static GLMainView *getMainView() { return vh; }

    static void start( ulong bits )
    {
        if( !inited )
        {
            fprintf( stderr, "%c[%d;%d;%dmGLViewSystem was not inited%c[%dm\n",
                    0x1B, 0, 31, 40, argv[0], 0x1B, 0 );
            exit(1);
        }

        static bool hasinput = 0;
        if( bits | WITH_INPUT_FILE )
            hasinput = 1;

        if( hasinput && arg < 2 )
        {
            fprintf( stderr, "%c[%d;%d;%dmusage: %s inputfile%c[%dm\n",
                    0x1B, 0, 31, 40, argv[0], 0x1B, 0 );
            exit(1);
        }
        printf( "%c[%d;%d;%dm[ program name % 32s ]%c[%dm\n",
                0x1B, 0, 32, 40, argv[0], 0x1B, 0 );
        if( hasinput )
        {
            printf( "%c[%d;%d;%dm[ input file % 34s ]%c[%dm\n",
                    0x1B, 0, 32, 40, argv[1], 0x1B, 0 );
            vh->readFile( argv[1] );
        }
        vh->init();
        glutMainLoop();
    }

    static int exitCode()
    {
        return 0;
    }
};

#endif//GVSYSTEM_H
