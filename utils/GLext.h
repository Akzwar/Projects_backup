#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glut.h>
#include<GL/freeglut_ext.h>
#include<stdio.h>
#include<stdexcept>

#define M_2D 0
#define M_3D 1
using namespace std;

void TextOut(int X, int Y, const char* Text)
{
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(X,Y,0.0);
    glScalef(0.1,0.1,1.0);
    glutStrokeString(GLUT_STROKE_ROMAN,Text);
    glPopMatrix();
}

template<int Width, int Height, int Mode>
class OGLWindow
{
 protected:
    static bool isExist;
    const char* Caption;
 public:
    static void onIdle(){glutPostRedisplay();}
    static void onMouseClick(int Button, int State, int X, int Y){}
    static void onMouseMove(int X, int Y){}
    static void onKey(unsigned char Key, int X, int Y)
    {
        if( Key == 27 )
            glutLeaveMainLoop();
    }
    static void onDraw(void)
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glLoadIdentity();
        glTranslatef(-Width/5.0,0.0,0.0);
        glScalef(0.5,0.5,0.5);
        glColor3f(1.0,0.0,0.0);
        const unsigned char* Text = "Default";
        glutStrokeString(GLUT_STROKE_ROMAN, Text);
        glutSwapBuffers();
    }
    static void onResize(int _Width, int _Height)
    {
        glViewport(0,0,_Width,_Height);    
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        switch(Mode)
        {
            case M_2D:
                glOrtho( -Width/2.0, Width/2.0, -Height/2.0, Height/2.0, -1.0, 1.0 );
                break;
            case M_3D:
                gluPerspective( 70.0, Width/(double)Height, 0.01, 1000 );
                break;
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    OGLWindow(int argc, char** argv, const char* _Caption)
        :Caption(_Caption)
    {
        //if(OGLWindow::isExist)
        //    throw runtime_error("Only one window enabled yet.");
        //OGLWindow::isExist = true;
        glutInit(&argc, argv);
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
        glutInitWindowSize( Width, Height );
        if( !glutCreateWindow( Caption ) )
            printf("GL window creation failed.");
        glClearColor( 0.0,0.0,0.1,0.0 );

        glutDisplayFunc(&OGLWindow::onDraw);
        glutIdleFunc(&OGLWindow::onIdle);
        glutReshapeFunc(&OGLWindow::onResize);
        glutKeyboardFunc(&OGLWindow::onKey);
        glutMouseFunc(&OGLWindow::onMouseClick);
        glutMotionFunc(&OGLWindow::onMouseMove);
        
        onResize(Width, Height);
    }
    void Show()
    {
        glutMainLoop();
    }
    void setDisplayFunc( void(*Callback)(void) ){ glutDisplayFunc(Callback); }
    void setIdleFunc( void(*Callback)(void) ){ glutIdleFunc(Callback); }
    void setReshapeFunc( void(*Callback)(int, int) ){ glutReshapeFunc(Callback); }
    void setKeyboardFunc( void(*Callback)(unsigned char, int, int) ){ glutKeyboardFunc(Callback); }
    void setMouseFunc( void(*Callback)(int, int, int, int) ){ glutMouseFunc(Callback); }
    void setMotionFunc( void(*Callback)(int, int) ){ glutMotionFunc(Callback); }
};

