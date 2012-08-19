#ifndef VIEWHELPERS_H
#define VIEWHELPERS_H

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/freeglut.h>
#include "quaterni.h"
#include "helpers.h"

/* for pnglib */
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <png.h>
/**/

/*
 * TODO: befor test gcamera.h remove this code
 *

class GLCamera3d
{    
public:
    float fov;
    float near;
    float far;
    float ar;
    Quaterni pos;
    Quaterni trg;
    Quaterni up;
    bool persp;

    GLCamera3d():
        fov( 50 ),
        near( 0.1 ),
        far( 100 ),
        ar( 1 ),
        pos( 1, 0, 0 ),
        up ( 0, 0, 1 ),
        persp( 1 )
    {}

    void set( float Fov,
              float Near,
              float Far,
              float AR,
              Quaterni Pos,
              Quaterni Trg,
              Quaterni Up
            )
    { 
        fov = Fov;
        near = Near;
        far = Far;
        ar = AR;
        pos = Pos;
        trg = Trg;
        up = Up;
    }
    
    void view()
     {  
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();

        if( persp ) gluPerspective( fov, ar, near, far );
        gluLookAt( pos.x, pos.y, pos.z,
                   trg.x, trg.y, trg.z,
                   up.x,  up.y,  up.z );
    }
    
};

class OrbitCamera: public GLCamera3d
{    
protected:
    Quaterni oPos;
    float dst; // distance
    float tng; // tangage
    float yaw; // yaw =)

    int oldx, oldy;
    float rcf; // rotation coef
    float mvs; // move step

public:

    OrbitCamera():
        oPos( 1, 0, 0 ),
        dst( 2 ),
        tng( 15/180.0 * M_PI ),
        yaw( 75/180.0 * M_PI ),
        rcf( 1/300.0 ),
        mvs( 0.2 )
    {}

    void setDst( float Dst ) { dst = Dst; recalcPos(); }
    void setTng( float Tng ) { tng = Tng; recalcPos(); }
    void setYaw( float Yaw ) { yaw = Yaw; recalcPos(); }

    void setRCf( float RCf ) { rcf = RCf; }
    void setMvS( float MvS ) { mvs = MvS; }

    void setOrbit( float Dst, float Tng, float Yaw )
    { 
        dst = Dst;
        tng = Tng;
        yaw = Yaw;
        recalcPos();
    }

    bool active; // for rotation
    void mRotation( int x, int y )
    { 
        int dx = x - oldx;
        int dy = y - oldy;

        tng += dy * rcf;
        yaw += dx * rcf;

        float maxTng = M_PI_4 * 1.9;
        if( tng > maxTng ) tng = maxTng;
        if( tng < -maxTng ) tng = -maxTng;

        oldx = x;
        oldy = y;
        recalcPos();
    }

    void moveDst( bool front )
    {
        float nmvs = dst * mvs;
        if( front ) dst -= mvs;
        else  dst += mvs;

        if( dst < ( near * 2 ) ) dst = near * 2;
        if( dst > ( far / 2.0 ) ) dst = far / 2.0;
        recalcPos();
    }

    void zoomFov( bool front )
    {
        if( front ) fov -= 1;
        else fov += 1;

        if( fov < 2 ) fov = 2;
        if( fov > 170 ) fov = 170;
        recalcPos();
    }

    void setOld( int x, int y )
    {
        oldx = x;
        oldy = y;
    }

    void recalcPos()
    {
        oPos = Quaterni( oPos.x, oPos.y, 0 ).e();
        Quaterni p = Quaterni::rotAngle( tng, oPos * Quaterni(0,0,1) ) *\
                     Quaterni::rotAngle( yaw, 0, 0, -1);
        oPos = p.rot(oPos).e() * dst;
        yaw = 0;
        pos = oPos + trg;
    }
};
*/

/** class GLGraph2d
 * TODO: WRITE THIS
 *

class GLGraph2d
{
protected:
    int w, h;
    int ileft, iright;
    float panX, panY;
    float st, en;
public:
    GLGraph2d(){}

    void setViewportSize( int _w, int _h )
    {
        w = _w;
        h = _h;
    }

    void setIndents( int _left, int _right )
    {
        ileft = _left;
        iright = _right;
    }

    void setYView( float start, float end )
    {
        st = start;
        en = end;
    }

    void pan( float nPanX, float nPanY )
    {
        panX = nPanX;
        panY = nPanY;
    }

    void appendPan( float aPanX, float aPanY )
    {
        panX += aPanX;
        panY += aPanY;
    }

    void view( int sctCnt, int sctNo, float maxVal, bool positive = 0 )
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        
        
        
    }

    virtual ~GLGraph2d(){}
};

*
* TODO: WRITE THIS
*/


/* lim = max( fabs( valGraph[i] ) ) [1]
 * t - time value to draw grid line [c]
 * yshift - for text [1]
 */
inline void drawXGrid( float min, float max, float t, float yshift )
{
    glBegin( GL_LINES );

    glVertex2f( t, min );
    glVertex2f( t, max );

    glEnd();

    char buf[20];
    if( t < 1e-5 ) sprintf( buf, " 0" );
    else sprintf( buf, " %g", t );
    glRasterPos2f( t, min + yshift );
    glutBitmapString( GLUT_BITMAP_HELVETICA_12, (unsigned char*) buf );
}

/* st - start draw [c]
 * en - end draw [c]
 * v - value [1]
 * xs, ys - x and y shifts for text [1]
 */
inline void drawYGrid( float st, float en, float v, float xs, float ys )
{
    glBegin( GL_LINES );

    glVertex2f( st, v );
    glVertex2f( en, v );

    glEnd();

    char buf[20];
    if( v == 0 ) sprintf( buf, "   0" );
    else if( v < 0 ) sprintf( buf, " % 5.3g", v );
    else if( v > 0 ) sprintf( buf, "  % 5.3g", v );
    glRasterPos2f( xs, v+ys );
    glutBitmapString( GLUT_BITMAP_HELVETICA_12, (unsigned char*) buf );
}

inline void drawSeparateLines( float st, float en, float up, float down, bool upOnly=0 )
{  
    float lh = fabs(up - down) * 0.005;

    glBegin( GL_TRIANGLE_FAN );

    glVertex2f( st, up );
    glVertex2f( en, up );
    glVertex2f( en, up-lh );
    glVertex2f( st, up-lh );

    glEnd();

    if( upOnly ) return;

    glBegin( GL_TRIANGLE_FAN );

    glVertex2f( st, down );
    glVertex2f( en, down );
    glVertex2f( en, down+lh );
    glVertex2f( st, down+lh );

    glEnd();

}

inline void drawCoordinateLines( float st, float en, float el, float lim )
{
    glBegin( GL_LINES );

    glVertex2f( st, 0 );
    glVertex2f( en, 0 );

    glVertex2f( el,  lim );
    glVertex2f( el, -lim );

    glEnd();
}

/* floor float x to r digits 
 */
inline float dfloor( float x, int r )
{
    if( !r ) return floor( x );
    float b = 1;
    for( int i = 0; i < r; i++ ) b*=(r>0)?10:0.1;
    x = 1/b * floor( b * x );
    return x;
}

inline point3d map( const point3d &p )
{
    GLdouble modelview[16];               
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview); 

    GLdouble projection[16];               
    glGetDoublev(GL_PROJECTION_MATRIX, projection); 

    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    double wx, wy, wz;
    gluProject( p.x, p.y, p.z,
                modelview,
                projection,
                viewport,            
                &wx, &wy, &wz
            );
    return point3d( wx, wy, wz );
}

inline point3d unmap( const point3d &p )
{
    GLdouble modelview[16];               
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview); 

    GLdouble projection[16];               
    glGetDoublev(GL_PROJECTION_MATRIX, projection); 

    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    double wx, wy, wz;
    gluUnProject( p.x, p.y, p.z,
                  modelview,
                  projection,
                  viewport,            
                  &wx, &wy, &wz
            );
    return point3d( wx, wy, wz );
}

inline void printPointLabel( const point3d &p, const point2d &offset,
        char *str, void *FONT = GLUT_BITMAP_HELVETICA_18, 
        bool disable_depht=1 )
{
    if( disable_depht )
    {
        glPushAttrib( GL_DEPTH_TEST );
        glDisable( GL_DEPTH_TEST );
    }
    glBegin( GL_POINTS );
    glVertex3f( p.x, p.y, p.z );
    glEnd();

    point3d w = map( p );

    //glWindowPos3f( w.x + offset.x, w.y + offset.y, w.z );
    glWindowPos2d( w.x + offset.x, w.y + offset.y );
    glutBitmapString( FONT, (unsigned char*)str );
    if( disable_depht ) 
        glPopAttrib();
}

inline void glCheckErr( const char *where = 0, bool printAll = 0 )
{
    int err = glGetError();
    if( err || printAll )
    {
        printf( "%c[%d;%d;%dm", 0x1B, 1, 31, 40 );
        printf( "error #%d: %s\n",
                err, gluErrorString( err ) );
        if( where )
            printf( "%c[%d;%d;%dmtag: %s\n",
                    0x1B, 0, 33, 40, where );
        printf( "%c[%dm\n", 0x1B, 0 );

    }
}

inline void glScreenShot( std::string fname )
{
    GLint vp[4];
    glGetIntegerv( GL_VIEWPORT, vp );
    int dx = vp[2] - vp[0];
    int dy = vp[3] - vp[1];
    GLuint *p = new GLuint[ dx*dy*4 ];
    glPixelStorei( GL_PACK_ALIGNMENT, 4 );
    glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
    glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
    glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );

    glReadPixels( vp[0], vp[1], vp[2], vp[3], GL_RGBA, GL_UNSIGNED_INT, p );

    /* libpng code  
     */

    png_byte color_type = PNG_COLOR_TYPE_RGB;
    png_byte bit_depth = 8;

    png_structp png_ptr;
    png_infop info_ptr;

    fname+=".png";
    FILE *fp = fopen( fname.c_str(), "wb" );
    if (!fp) 
    { 
        fprintf( stderr, "[glScreenShot] file %s could not be opened for writing", fname.c_str() );
        return;
    }

    /* initialize stuff */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );

    if (!png_ptr)
    {
        fprintf( stderr, "[glScreenShot] png_create_write_struct failed" );
        return;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf( stderr, "[glScreenShot] png_create_info_struct failed" );
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fprintf( stderr, "[glScreenShot] Error during init_io" );
        return;
    }

    png_init_io(png_ptr, fp);

    /* write header */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fprintf( stderr, "[glScreenShot] Error during writing header" );
        return;
    }

    png_set_IHDR( png_ptr, info_ptr, vp[2], vp[3],
                  bit_depth, color_type, PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

    png_write_info( png_ptr, info_ptr );


    /* write bytes */
    if( setjmp( png_jmpbuf( png_ptr ) ) )
    {
        fprintf( stderr, "[glScreenShot] Error during writing bytes");
        return;
    }

    png_byte *row = new png_byte[ vp[2] * 3 ];

    for( int y = vp[3]-1; y >= 0; y-- )
    {
        for( int x = 0; x < vp[2]; x++ )
        {
            int k = y * vp[2] + x;
            row[x*3+0] = p[k*4+0];
            row[x*3+1] = p[k*4+1];
            row[x*3+2] = p[k*4+2];
        }
        png_write_row( png_ptr, row );
    }

    delete [] row;
    delete [] p;

    /* end write */
    if( setjmp( png_jmpbuf( png_ptr ) ) )
    {
        fprintf( stderr, "[glScreenShot] Error during end of write");
        return;
    }

    png_write_end( png_ptr, NULL );

    fclose( fp );
    fprintf( stderr, "file \"%s\" writen\n", fname.c_str() );
}

#endif//VIEWHELPERS_H
