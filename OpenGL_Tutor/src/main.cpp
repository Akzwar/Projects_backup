#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glut.h>
#include<stdio.h>

void onIdle()
{}

void Draw()
{
	glColor3f( 0.0, 0.0, 1.0 );
	glBegin( GL_QUADS );
		glVertex2f( 0.0, 0.0 );
		glVertex2f( 1.0, 0.0 );
		glVertex2f( 1.0, 1.0 );
		glVertex2f( 0.0, 1.0 );
	glEnd();
	glutSwapBuffers();
}

void InitGL()
{
	glViewport(0,0,640,480);
	glClearColor( 0.0, 0.0, 0.1, 0.0 );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 0.0, 1.0, 0.0, 1.0, -1.0, 100.0 );
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0,0,10, 0,0,0, 0,1,0);
}

int main( int argc, char** argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize( 640, 480 );
	glutCreateWindow( "OpenGL_Tutor" );
	glutDisplayFunc(Draw);
	glutIdleFunc(onIdle);
	InitGL();
	glutMainLoop();
	return 0;
}
