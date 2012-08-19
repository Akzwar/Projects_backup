#include <stdio.h>
#include "libfreenect.h"
#include <GL/glut.h>
#include <GL/glu.h>
//#include <iostream>
#include <pthread.h>
#include <cassert>
#define W 640
#define H 480
#define maxdepth 2048.0
uint16_t* depth;
uint8_t *rgb, *rgb_back;
int depth2d[640][480];
int rgb2d[W*3][H];
freenect_context *f_context;
freenect_device *f_device;
pthread_t f_thread;
int f_angle = -30;
bool shutdown = 0;
bool started = false;
bool rgbstarted = false;
GLuint gl_depth_texture;
int offset = 0;
//uint8_t* depth_gl;
pthread_mutex_t gl_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gl_cond = PTHREAD_COND_INITIALIZER;

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

void onResize(int Width, int Height)
{
	glViewport( 0, 0, Width, Height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, W, H, 0, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void InitGL( int Width, int Height )
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepth( 1.0 );
	//glDepthFunc( GL_LESS );
	//glDepthMask( GL_FALSE );
	//glDisable( GL_DEPTH_TEST );
	//glDisable( GL_BLEND );
	//glDisable(GL_ALPHA_TEST);
	//glShadeModel( GL_FLAT );
	//glEnable(GL_TEXTURE_2D);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glGenTextures( 1, &gl_depth_texture );
	//glBindTexture( GL_TEXTURE_2D, gl_depth_texture );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	onResize(Width, Height);
}

void DrawGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Depth
	//glGenTextures(1, &gl_depth_texture);
	//glBindTexture(GL_TEXTURE_2D, gl_depth_texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, depth_gl);
	//glBegin(GL_TRIANGLE_FAN);
	//	glColor4f(1.0f,1.0f,1.0f,1.0f);
	//	glTexCoord2f(0.0,0.0); glVertex3f(0.0,0.0,0.0);
	//	glTexCoord2f(1.0,0.0); glVertex3f(W,0.0,0.0);
	//	glTexCoord2f(1.0,1.0); glVertex3f(W,H,0.0);
	//	glTexCoord2f(0.0,1.0); glVertex3f(0.0,H,0.0);
	//glEnd();
	//Arm center
	pthread_mutex_lock( &gl_mutex );
	glBegin(GL_POINTS);
	for(int i=0; i<W*3; i++)
		for(int j=0; j<H; j++)
		{
			/*if((depth2d[i][j]>(maxdepth-maxdepth/3.0)) && (depth2d[i][j]<maxdepth-1))
			{	
				glColor3f( depth2d[i][j]/maxdepth, 1.0f - depth2d[i][j]/maxdepth, 0.0f );
			}
			else
			if(depth2d[i][j]<(maxdepth-maxdepth/3.0)&&depth2d[i][j]>(maxdepth/3.0))
			{
				glColor3f( 0.0f, depth2d[i][j] / maxdepth, 1.0f - depth2d[i][j]/maxdepth );
			}
			else
			if(depth2d[i][j]<(maxdepth/3.0 + offset ))
			{	
				glColor3f( 0.0f, 0.0f, depth2d[i][j] / maxdepth );
			}
			else
			if((depth2d[i][j]>(maxdepth-maxdepth/3.0)) && (depth2d[i][j]<maxdepth))
				glColor3f(0.0f,0.0f,0.0f);*/
			glColor3f(rgb2d[i][j]/255.0,rgb2d[i+1][j]/255.0,rgb2d[i+2][j]/255.0);
			glVertex2f( i/3, j );
		}
	glEnd();
	pthread_mutex_unlock( &gl_mutex );
	glColor3f(0.0f,1.0f,0.0f);
	glBegin(GL_QUADS);
		glVertex2f(arm.center.x,arm.center.y);
		glVertex2f(arm.center.x+10,arm.center.y);
		glVertex2f(arm.center.x+10,arm.center.y+10);
		glVertex2f(arm.center.x,arm.center.y+10);
	glEnd();
	//Arm quad
	glBegin(GL_LINE_STRIP);
		glVertex2f(arm.left.x,arm.top.y);
		glVertex2f(arm.right.x,arm.top.y);
		glVertex2f(arm.right.x,arm.bottom.y);
		glVertex2f(arm.left.x,arm.bottom.y);
		glVertex2f(arm.left.x,arm.top.y);
	glEnd();

	glutSwapBuffers();
}

void CloseAll()
{
	shutdown = 1;
	pthread_join( f_thread, NULL );
	freenect_stop_depth( f_device );
	freenect_close_device( f_device );
	//free(depth_gl);
	exit( 0 );
}

void onIdle(void)
{
	pthread_mutex_lock(&gl_mutex);
	if(!shutdown)
	{
		while(!started)
			pthread_cond_wait(&gl_cond, &gl_mutex);

		started=false;
 		/*for(int i=0; i<W; i+=1)
			for(int j=0; j<H; j+=1)
			{
				if(depth2d[i][j]>(k-k/3.0))
				{	
					depth_gl[i*H+j*3+0]=depth2d[i][j]/k*255;
					depth_gl[i*H+j*3+1]=0.0;
					depth_gl[i*H+j*3+2]=0.0;
				}
				if(depth2d[i][j]<(k-k/3.0)&&depth2d[i][j]>(k/3.0))
				{
					depth_gl[i*H+j*3+1]=depth2d[i][j]/k*255;
					depth_gl[i*H+j*3+0]=0.0;
					depth_gl[i*H+j*3+2]=0.0;
				}
				if(depth2d[i][j]<(k/3.0))
				{	
					depth_gl[i*H+j*3+2]=depth2d[i][j]/k*255;
					depth_gl[i*H+j*3+1]=0.0;
					depth_gl[i*H+j*3+0]=0.0;
				}
			}*/
		for(int i = 0;i<W;i++)
			for(int j = 1; j < H; j++ )
			{
				if(depth2d[i][j]<(maxdepth/3.0+offset))
				{
					arm.left.x = i;		
					arm.left.y = j;
					break;
				}
			}
		for(int j=0; j<=H;j++)
			for(int i=1; i < W; i++)
			{
				if(depth2d[i][j]<(maxdepth/3.0+offset))
				{
					arm.top.x = i;		
					arm.top.y = j;
					break;
				}
			}
		for(int i = W-1; i >= 0; i--)
			for(int j = H-1; j > 0; j--)
			{
				if( depth2d[i][j]<(maxdepth/3.0+offset))
				{
					arm.right.x = i;
					arm.right.y = j;
					break;
				}
			}
		for(int j = H-1; j >= 0; j--)
			for(int i = W-1; i > 0; i--)
			{
				if( depth2d[i][j]<(maxdepth/3.0+offset))
				{
					arm.bottom.x = i;
					arm.bottom.y = j;
					break;
				}
			}
	}
	pthread_mutex_unlock(&gl_mutex);
	arm.center.x = arm.left.x + (arm.right.x - arm.left.x)/2;
	arm.center.y = arm.top.y + (arm.bottom.y - arm.top.y)/2;
	glutPostRedisplay();
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
	if( key == 'q'  )
	{
		offset++;
		if( offset > maxdepth/3 )
			offset = maxdepth/3;
	}
	if( key == 'e' )
	{
		offset--;
		if( offset < 0 )
			offset = 0;
	}
	freenect_set_tilt_degs( f_device, f_angle );
}

void mousePressed( int button, int state, int x, int y )
{
printf("%d\n",depth2d[x][y]);
fflush(stdout);
} 

void *gl_threadfunc( void* arg )
{
	printf("Starting GL thread...\n");

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH );

	glutInitWindowSize( W, H );

	glutInitWindowPosition( 0, 0 );	

	glutCreateWindow( "Arm detecting" );
	
	glutDisplayFunc( &DrawGL );
	glutIdleFunc( &onIdle );
	glutReshapeFunc( &onResize );
	glutKeyboardFunc( &keyPressed );
	glutMouseFunc( &mousePressed );
	InitGL( W, H );

		
	glutMainLoop();
	return NULL;
}


void depth_cb( freenect_device *dev, void *v_depth, uint32_t timestamp )
{
	depth = (uint16_t*)v_depth;

	pthread_mutex_lock(&gl_mutex);
	for(int i=0; i<W; i++)
		for(int j=0; j<H; j++)
			depth2d[i][j] = depth[j*W+i]; 
	started = true;
	pthread_cond_signal(&gl_cond);
	pthread_mutex_unlock(&gl_mutex);
}

void rgb_cb( freenect_device *dev, void *v_rgb, uint32_t timestamp )
{
	pthread_mutex_lock(&gl_mutex);
	//assert( rgb_back == rgb);
	rgb_back = rgb;
	freenect_set_video_buffer(dev, rgb_back);
	rgb = (uint8_t*)v_rgb;
	for(int i=0; i<W*3; i++)
		for(int j=0; j<H; j++)
			rgb2d[i][j] = rgb[j*W+i];
	printf("asfafsdgsdfgasd");
	rgbstarted = true;
	pthread_cond_signal(&gl_cond);
	pthread_mutex_unlock(&gl_mutex);
}	

void*  f_threadfunc( void *arg )
{
	freenect_set_tilt_degs( f_device, f_angle );
	freenect_set_led( f_device, LED_RED );
	freenect_set_depth_callback( f_device, depth_cb );
	freenect_set_video_callback(f_device, rgb_cb);
	if( (W == 640) && H ==( 480 ))
	{
		freenect_set_video_mode( f_device, freenect_find_video_mode( FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB ) );
		printf("Resolution:%d:%d\n",W,H);
		fflush(stdout);
	}
	else
	if(( W == 1280) &&( H == 1024) )
	{
		freenect_set_video_mode( f_device, freenect_find_video_mode( FREENECT_RESOLUTION_HIGH, FREENECT_VIDEO_RGB ) );
		printf("Resolution:%d:%d\n",W,H);
		fflush(stdout);
	}
	else
	{
		printf("Unknown resolution. Shutting down...\n");
		CloseAll();
	}
	freenect_set_depth_mode( f_device, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT) );
	freenect_start_video( f_device );
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
	//depth_gl = (uint8_t*)malloc(640*480*3);
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
