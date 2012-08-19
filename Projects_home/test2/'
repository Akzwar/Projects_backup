#include <stdio.h>
#include "libfreenect.h"
#include <GL/glut.h>
#include <GL/glu.h>
//#include <iostream>
#include <pthread.h>
#define W 640
#define H 480
uint16_t *depth;
freenect_context *f_context;
freenect_device *f_device;
pthread_t f_thread;
int f_angle = 0;
bool shutdown = 0;
bool started = false;

struct point
{
	int x,y;
};

struct marm
{	
	point right, left, top, bottom;
	point finger[6];
	point center;	
};

marm arm;

void InitGL( int Width, int Height )
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	//glClearDepth( 1.0 );
	//glDepthFunc( 1.0 );
	//glDepthMask( GL_FALSE );
	//glDisable( GL_DEPTH_TEST );
	//glDisable( GL_BLEND );
	//glDisable(GL_ALPHA_TEST);
	//glShadeModel( GL_FLAT );
	glViewport( 0, 0, Width, Height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, Width, Height, 0, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void DrawGL()
{
float k=2048.0;
if(!shutdown && started)
{
	glBegin(GL_POINTS);
		for(int i=1; i<H; i+=1)
			for(int j=1; j<W; j+=1)
			{
				if(depth[i*W+j]>(k-k/3.0))
				glColor3f(0.0f,0.0f,0.0f);
				if(depth[i*W+j]<(k-k/3.0)&&depth[i*W+j]>(k/3.0))
				glColor3f(0.0f,depth[i*W + j]/k,0.0f);
				if(depth[i*W+j]<(k/3.0))
				{
					glColor3f(0.0f,0.0f,depth[i*W+j]/k);
				}
				glVertex2f( j, i );
			}
	glEnd();

}

	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_QUADS);
		glVertex2f(arm.left.x,arm.left.y);
		glVertex2f(arm.left.x+10,arm.left.y);
		glVertex2f(arm.left.x+10,arm.left.y+10);
		glVertex2f(arm.left.x,arm.left.y+10);
	glEnd();
	
	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_QUADS);
		glVertex2f(arm.right.x,arm.right.y);
		glVertex2f(arm.right.x+10,arm.right.y);
		glVertex2f(arm.right.x+10,arm.right.y+10);
		glVertex2f(arm.right.x,arm.right.y+10);
	glEnd();
	glutSwapBuffers();
}

void CloseAll()
{
	shutdown = 1;
	pthread_join( f_thread, NULL );
	freenect_stop_depth( f_device );
	freenect_close_device( f_device );
	exit( 0 );
}

void keyPressed( unsigned char key, int x, int y )
{
	if( key == 27 )
	{
		CloseAll();
	}
	if( key == 'w' )
	{
		f_angle++;
		if( f_angle > 30 )
			f_angle = 30;
	}
	if( key == 's' )
	{
		f_angle--;
		if( f_angle < -30 )
			f_angle = -30;
 	}

	if( key == 'x' )
	{
		f_angle = 0;
 	}
	freenect_set_tilt_degs( f_device, f_angle );
}

void mousePressed( int button, int state, int x, int y )
{
} 

void *gl_threadfunc( void* arg )
{
	printf("Starting GL thread...\n");

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH );

	glutInitWindowSize( W, H );

	glutInitWindowPosition( 0, 0 );	

	glutCreateWindow( "Kinect Test" );
	
	glutDisplayFunc( &DrawGL );
	glutIdleFunc( &DrawGL );
	glutKeyboardFunc( &keyPressed );
	glutMouseFunc( &mousePressed );
	InitGL( W, H );
	
	glutMainLoop();
	return NULL;
}


void depth_cb( freenect_device *dev, void *v_depth, uint32_t timestamp )
{
	int k=2048;
	depth = (uint16_t*)v_depth;
	started = true;
	arm.left.x = 0;
	arm.left.y = 0;
	for(int i = 1;i<W;i++)
		for(int j = 1; j < H; j++ )
			{
				if(depth[j*W+i]<(k/3.0) && arm.left.x == 0 && arm.left.y == 0)
				{
					arm.left.x = i;		
					arm.left.y = j;
					break;
				}		
			}
	arm.right.x = 0;
	arm.right.y = 0;
	for(int i = 1; i<W; i++)
		for(int j = H; j >0; j--)
			{
				if( depth[j*W+i]<(k/3.0) && arm.right.x == 0 && arm.right.y == 0 )
				{
					arm.right.x = i;
					arm.right.y = j;
					break;
				}
			}
	printf("%d,%d",arm.right.x, arm.right.y);
}

void*  f_threadfunc( void *arg )
{
	freenect_set_tilt_degs( f_device, f_angle );
	freenect_set_led( f_device, LED_RED );
	freenect_set_depth_callback( f_device, depth_cb );
	if( (W == 640) && H ==( 480 ))
	{
		freenect_set_depth_mode( f_device, freenect_find_depth_mode( FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT ) );
		printf("Resolution:%d:%d\n",W,H);
		fflush(stdout);
	}
	else
	if(( W == 1280) &&( H == 1024) )
	{
		freenect_set_depth_mode( f_device, freenect_find_depth_mode( FREENECT_RESOLUTION_HIGH, FREENECT_DEPTH_11BIT ) );
		printf("Resolution:%d:%d\n",W,H);
		fflush(stdout);
	}
	else
	{
		printf("Unknown resolution. Shutting down...\n");
		CloseAll();
	}
	freenect_start_depth( f_device );
	while ( !shutdown && freenect_process_events( f_context ) >= 0 )
	{
	
	}
	CloseAll();
	printf( "Done. Shutting down...\n" );
	fflush(stdout);
	return NULL;
}

int main( int argc, char **argv )
{
	if( freenect_init( &f_context, NULL ) < 0 )
	{
		printf( "Freenect initialization failed.\n" );
		return 1;
	}
	freenect_set_log_level( f_context, FREENECT_LOG_DEBUG );
	freenect_select_subdevices( f_context, ( freenect_device_flags )( FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA ) );
	if( freenect_num_devices ( f_context ) < 1 )
	{
		printf( "No devices found. Shutting down...\n" );
		freenect_shutdown( f_context );
		return 1;
	}
	if( freenect_open_device( f_context, &f_device, 0 ) <0 )
	{
		printf( "Couldn't open device. Shutting down...\n" );
		freenect_shutdown( f_context );
		return 1;	
	}
	if( pthread_create( &f_thread, NULL, f_threadfunc, NULL  ) )
	{
		printf( "Thread creation failed. Shutting down...\n" );
		freenect_shutdown( f_context );
		return 1;
	}
	glutInit( &argc, argv );
	gl_threadfunc( NULL );
	return 0;
}
