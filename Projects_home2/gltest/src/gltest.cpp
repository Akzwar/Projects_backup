#include <stdio.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
GLuint gl_texture;
uint8_t* tex_gl;

void InitGL( int Width, int Height )
{
	//glClearColor( 0,0,0,0 );
	glEnable( GL_TEXTURE_2D );
	glGenTextures( 1, &gl_texture );
	glBindTexture( GL_TEXTURE_2D, gl_texture );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glViewport( 0, 0, Width, Height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, 640,480, 0, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity(); 
       for(int i = 0; i < 640*480; i++)
                {
                        tex_gl[3*i] = 255;
                        tex_gl[3*i+1] = 0;
                        tex_gl[3*i+2] = 0;
                }
	glTexImage2D( GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_gl );

}

void onIdle(void)
{
	glutPostRedisplay();
} 

void DrawGL()
{
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glBindTexture( GL_TEXTURE_2D, gl_texture );
	glBegin(GL_TRIANGLE_FAN);
		glColor3f( 0.0,0.0,0.0);
                glTexCoord2f(0.0,0.0); glVertex3f(0.0,0.0,0.0);
                glTexCoord2f(1.0,0.0); glVertex3f(640,0.0,0.0);
                glTexCoord2f(1.0,1.0); glVertex3f(640,480,0.0);
                glTexCoord2f(0.0,1.0); glVertex3f(0.0,480,0.0);
        glEnd(); 
	//glBindTexture( GL_TEXTURE_2D, 0 );     
	glutSwapBuffers(); 
}

int main( int argc, char** argv )
{
	tex_gl = (uint8_t*)malloc(640*480*3);		
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH );
	glutInitWindowSize( 640, 480 );
	glutInitWindowPosition( 0,0 );
	glutCreateWindow( "GLtest" );
	
	glutDisplayFunc( &DrawGL );
	glutIdleFunc( &DrawGL );
	InitGL( 640, 480 );
	glutMainLoop();
	free(tex_gl);
	return 1;	
}
