#ifndef GVTEMPL_H
#define GVTEMPL_H

#include "gvarea.h"
#include "gvobj.h"
#include "gcamera.h"
#include "gvhelpers.h"

class GLObjectViewer: public GLViewState
{
protected:
    virtual void drawHook() {}
    virtual void setView() = 0;
public:
    std::vector<GObject*> objlist;
    void init(){}
    void destroy(){}

    void draw()
    {
        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glEnable( GL_DEPTH_TEST );
        glClear( GL_DEPTH_BUFFER_BIT ); // TESTME: clear color buf

        glMatrixMode( GL_PROJECTION);
        glPushMatrix();
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glLoadIdentity();

        setView();
        drawHook();

        for( int i = 0; i < objlist.size(); i++ )
            objlist[i]->draw();

        glPopMatrix();
        glMatrixMode( GL_PROJECTION);
        glPopMatrix();
        glPopAttrib();
    }
};

class G3DViewer: public GLObjectViewer
{
protected:
    bool camactive;
    double scale;
    void setView()
    {
        glScaled( scale, scale, scale );
        cam.view();
    }

    void reshapeHook()
    {
        cam.setARatio( (float)this->w / (float)this->h );
    }

public:
    GOrbitCamM cam;

    void setUnitSize( double sc )
    {
        scale = 1.0 / sc;
        cam.setNear( sc * 0.01 );
        cam.setDst( sc * 5.0, 1 );
        cam.setFar( sc * 1000.0 );
    }

    void mouse( int btn, int st, int mx, int my )
    {
        int mod = glutGetModifiers();
        float zoomspeed = 2;
        float movespeed = .1;
        if( ( btn == 3 || btn == 4 ) && st == GLUT_UP )
        {
            float fr = (btn == 3)?-1:1;
            if( mod == GLUT_ACTIVE_SHIFT )
                cam.zoom( fr * zoomspeed );
            else
                cam.move( fr * movespeed );

        }
        if( btn == 0 )
        {
            if( st == GLUT_DOWN )
            {
                cam.setOldMPos( mx, my );
                camactive = 1;
            }
            else
                camactive = 0;
        }
        if( btn == 2 && st == GLUT_UP )
        {
            // TODO: write init params
            cam.setFov( 50 );
            cam.setOrbit( 5, 0, 0 );
            cam.resetDst();
        }
    }

    void motion( int mx, int my )
    {
        float rotspeed = 0.01;
        if( camactive )
            cam.rotation( mx, my, rotspeed, rotspeed );
    }

    void releaseFromActive() { camactive = 0; }

};

struct trpoint { float x, y, z; };

template< class TR >
class GTraceDrawAbstract: public GObject
{
private:
    int oldbufcnt;
protected:
    GLuint *buf;
    int bufcnt;
    TR *tr;
    int trlen;

    bool changes;

    trpoint min, max;
    inline void resetMinMax()
    {
        float MAX_FLOAT = 1e37;
        min.x = MAX_FLOAT;
        min.y = MAX_FLOAT;
        min.z = MAX_FLOAT;
        max.x = -MAX_FLOAT;
        max.y = -MAX_FLOAT;
        max.z = -MAX_FLOAT;
    }

    inline void setMinMax( const trpoint &pt )
    {
        // min
        if( min.x > pt.x ) min.x = pt.x;
        if( min.y > pt.y ) min.y = pt.y;
        if( min.z > pt.z ) min.z = pt.z;
        // max
        if( max.x < pt.x ) max.x = pt.x;
        if( max.y < pt.y ) max.y = pt.y;
        if( max.z < pt.z ) max.z = pt.z;
    }

    void removeBuf()
    {
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        if( buf && oldbufcnt > 0 )
        {
            glDeleteBuffers( oldbufcnt, buf );
            delete [] buf;
            buf = 0;
        }
    }

    void copyAbs()
    {
        if( !this->tr ) return;
        this->trlen = this->tr->size();
        this->resetMinMax();

        removeBuf();
        buf = new GLuint[bufcnt];
        glGenBuffers( bufcnt, this->buf );
        oldbufcnt = bufcnt;
        trpoint *tmp = new trpoint[this->trlen];
        for( int i = 0; i < bufcnt; i++ )
        {
            copyTrace( tmp, i );

            glBindBuffer( GL_ARRAY_BUFFER, this->buf[i] );
            glBufferData( GL_ARRAY_BUFFER, this->trlen*sizeof(trpoint),
                    tmp, GL_DYNAMIC_DRAW );

        }
        delete [] tmp;
        this->changes = 0;
    }

    void drawAbs()
    {
        if( bufcnt == 0 ) return;
        glPushAttrib( GL_ALL_ATTRIB_BITS );

        glEnable( GL_VERTEX_ARRAY );
        glEnable( GL_DEPTH_TEST );

        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();

        this->setTransform();

        for( int i = 0; i < bufcnt; i++ )
            drawBuf( i );

        glPopMatrix();
        glPopAttrib();
    }

    virtual inline trpoint getTrPoint( int i ) const = 0;
    virtual void copyTrace( trpoint *tmp, int bufno ) = 0;
    virtual void drawBuf( int bufno ) = 0;

public:
    GTraceDrawAbstract():
        buf(0),
        bufcnt(0),
        tr(0),
        trlen(0),
        changes(1),
        oldbufcnt(0)
    {
        resetMinMax();
    }

    inline point3d getMin() const { return point3d( min.x, min.y, min.z ); }
    inline point3d getMax() const { return point3d( max.x, max.y, max.z ); }

    void setTrace( TR *ntr )
    {
        if( ntr )
        {
            tr = ntr;
            changes = 1;
        }
    }

    void update() { changes = 1; }

    point3d pos() const
    {
        point3d p;
        trpoint ltr = getTrPoint( -1 );
        p.x = ltr.x;
        p.y = ltr.y;
        p.z = ltr.z;
        return p;
    }

    void draw()
    {
        if( !tr ) return;
        //if( trlen != tr->size() || changes )
            copyAbs();
        drawAbs();
    }

    ~GTraceDrawAbstract()
    {
        removeBuf();
    }
};

template< class TR, class ST >
class GTraceDraw3D: public GTraceDrawAbstract< TR >
{
protected:
    int trno[3]; // trace indexs

    inline float getByNo( const ST *st, int no ) const
    {
        int k = trno[no];
        if( k < 0 ) return 0;
        else return (*st)[k];
    }

    virtual inline trpoint getTrPoint( int i ) const
    {
        trpoint ret;
        const ST *st;
        if( i >= 0 ) st = &(this->tr->at(i));
        else st = &(this->tr->last());
        ret.x = getByNo( st, 0 );
        ret.y = getByNo( st, 2 );
        ret.z = getByNo( st, 1 );
        return ret;
    }

    void copyTrace( trpoint *tmp, int bufno )
    {
        // XXX: not optimize copy
        for( int i = 0; i < this->trlen; i++ )
        {
            tmp[i] = this->getTrPoint( i );
            this->setMinMax( tmp[i] );
        }
    }

    void drawBuf( int bufno )
    {
        glColor3f( col.r, col.g, col.b );
        glBindBuffer( GL_ARRAY_BUFFER, this->buf[0] );
        glVertexPointer( 3, GL_FLOAT, 0, 0 );
        glDrawArrays( GL_LINE_STRIP, 0, this->trlen );

        glPointSize( 5 );
        glBegin( GL_POINTS );
        trpoint last = getTrPoint( -1 );
        glVertex3f( last.x, last.y, last.z );
        glEnd();
    }


public:
    color3d col;

    GTraceDraw3D(): GTraceDrawAbstract<TR> ()
    {
        this->bufcnt = 1;
        this->buf = new GLuint[1];
        for( int i = 0; i < 3; i++ )
            trno[i] = i;
    }

    void setIndexs( int *indexs )
    {
        if( indexs )
            for( int i = 0; i < 3; i++ )
                trno[i] = indexs[i];
        this->changes = 1;
    }
};

template< class TR, class ST, class IND >
class GTimeTraceDrawAbs: public GTraceDrawAbstract<TR>
{ 
protected:

    std::vector<color3d> colors;
    std::vector<IND> indexs;

    inline trpoint getTrPoint( int i ) const
    {
        trpoint ret;
        ret.x = 0;
        ret.y = 0;
        ret.z = 0;
        return ret;
    }

    virtual inline float getByNo( const ST *st, int no ) const = 0;

    trpoint getTrace( int bufno, int i )
    {
        trpoint ret;
        ST *st = &(this->tr->at(i));
        ret.x = st->time;
        ret.y = getByNo( st, bufno );
        ret.z = 0;
        return ret;
    }

    void copyTrace( trpoint *tmp, int bufno )
    {
        for( int i = 0; i < this->trlen; i++ )
        {
            tmp[i] = this->getTrace( bufno, i );
            this->setMinMax( tmp[i] );
        }
    }

    void drawBuf( int bufno )
    {
        if( !this->buf ) return;
        color3d col = colors[bufno];
        glColor3f( col.r, col.g, col.b );
        glBindBuffer( GL_ARRAY_BUFFER, this->buf[bufno] );
        glVertexPointer( 3, GL_FLOAT, 0, 0 );
        glDrawArrays( GL_LINE_STRIP, 0, this->trlen );
    }

public:
    GTimeTraceDrawAbs(): GTraceDrawAbstract<TR> () { }

    void setIndexs( int icount, IND *trno, color3d *clrs=0, bool onecolor=0 )
    { 
        colors.clear();
        indexs.clear();
        for( int i = 0; i < icount; i++ )
        {
            indexs.push_back( trno[i] );
            if( clrs )
                colors.push_back( clrs[onecolor?0:i] );
            else
                colors.push_back( color3d( 1, 0, 0 ) );
        }
        this->bufcnt = indexs.size();
        this->changes = 1;
    }
};

template< class TR, class ST >
class GTimeTraceDrawSmpl: public GTimeTraceDrawAbs< TR, ST, int >
{ 
protected:
    inline float getByNo( const ST *st, int no ) const
    {
        int k = this->indexs[no];
        if( k < 0 ) return 0;
        else return (*st)[k];
    }
};

template< class TR, class ST >
class GTimeTraceDrawAnti: public GTimeTraceDrawAbs< TR, ST, int >
{ 
protected:
    inline float getByNo( const ST *st, int no ) const
    {
        int k = this->indexs[no];
        if( k < 0 ) return 0;
        else return -(*st)[k];
    }
};

struct intpair {
    int a, b;
    intpair( int A, int B ): a(A), b(B) {}
public:
};

template< class TR, class ST >
class GTimeTraceDrawComb: public GTimeTraceDrawAbs< TR, ST, intpair >
{
protected:
    virtual inline float comb( float a, float b ) const { return a - b; }

    inline float getByNo( const ST *st, int no ) const
    {
        float a, b;
        int ai = this->indexs[no].a;
        int bi = this->indexs[no].b;
        if( ai < 0 ) a = 0;
        else a = (*st)[ai];
        if( bi < 0 ) b = 0;
        else b = (*st)[bi];
        return comb( a, b );
    }
};

class G2DViewer: public GLObjectViewer
{
protected:
    std::string tip;

    void drawHook()
    {
        glColorGray( 1 );
        char str[128];
        sprintf( str, "[ tip: %s ]", tip.c_str() );
        glWindowPos2d( this->x + 10, this->y + 10 );
        glutBitmapString( GLUT_BITMAP_HELVETICA_12,
                (unsigned char*) str );
    }

    struct viewpoint { float x, y; } min, max;

    struct rect
    {
        float up, down, right, left;
        rect(): up(0), down(0), right(0), left(0) {}
        rect( const viewpoint &mn, const viewpoint &mx )
        {
            up = mx.y;
            down = mn.y;
            right = mx.x;
            left = mn.x;
        }
        rect mlt( float kx, float ky ) const
        {
            rect ret;
            ret.up = up * ky;
            ret.down = down * ky;
            ret.right = right * kx;
            ret.left = left * kx;
            return ret;
        }
        void expand( const rect& b )
        {
            up += b.up;
            down -= b.down;
            right += b.right;
            left -= b.left;
        }
        inline float dx() const { return right - left; }
        inline float dy() const { return up - down; }
    } offsetpx;

    void resetMinMax()
    {
        float MAX = 1.0e37;
        min.x = MAX;
        min.y = MAX;
        max.x = -MAX;
        max.y = -MAX;
    }

    void setMinMax( const point3d &omin, const point3d &omax )
    {
        if( min.x > omin.x ) min.x = omin.x;
        if( min.y > omin.y ) min.y = omin.y;
        if( max.x < omax.x ) max.x = omax.x;
        if( max.y < omax.y ) max.y = omax.y;
    }

    void correctMinMax()
    {
        // offset by mouse correction
    }

    void drawBase( const rect &rc, float ft )
    {
        glBegin( GL_LINES );
        glVertex2f( rc.left, 0 );
        glVertex2f( rc.right, 0 );
        glVertex2f( 0, rc.up );
        glVertex2f( 0, rc.down );
        glVertex2f( ft, rc.up );
        glVertex2f( ft, rc.down );
        glEnd();

        glLineWidth(4);
        glBegin( GL_LINES );
        glVertex2f( rc.left, rc.up );
        glVertex2f( rc.right, rc.up );
        glVertex2f( rc.left, rc.down );
        glVertex2f( rc.right, rc.down );
        glEnd();
        glLineWidth(1);

        point3d p( 0, 0, 0 );
        point2d o( 3, -15 );
        char str[25];
        sprintf( str, "0" );
        printPointLabel( p, o, str, GLUT_BITMAP_HELVETICA_12 );
        sprintf( str, "%g", ft );
        p = point3d( ft, 0, 0 );
        printPointLabel( p, o, str, GLUT_BITMAP_HELVETICA_12 );
    }

    void drawVLine( float t, float down, float up )
    {
        glBegin( GL_LINES );
        glVertex2f( t, down );
        glVertex2f( t, up );
        glEnd();
        point3d p( t, 0, 0 );
        point2d o( 3, -15 );
        char str[25];
        sprintf( str, "%g", t );
        printPointLabel( p, o, str, GLUT_BITMAP_HELVETICA_12 );
    }

    void drawHLine( float v, float st, float en, float ft )
    {
        float endr = ft + ( en - ft ) * 0.1;
        glBegin( GL_LINES );
        glVertex2f( st, v );
        glVertex2f( endr, v );
        glEnd();

        point3d p( endr, v, 0 );
        point2d o( 2, -4 );
        char str[25];
        sprintf( str, "% +g", v );
        printPointLabel( p, o, str, GLUT_BITMAP_HELVETICA_12 );
    }

    inline static void glColorGray( float g ) { glColor3f( g, g, g ); }

    void setView()
    {
        resetMinMax();
        for( int i = 0; i < this->objlist.size(); i++ )
            setMinMax( this->objlist[i]->getMin(), this->objlist[i]->getMax() );

        float dwx = max.x - min.x;
        float dwy = max.y - min.y;

        float kx = dwx / ( this->w - offsetpx.left - offsetpx.right );
        float ky = dwy / ( this->h - offsetpx.up - offsetpx.down );

        rect add( offsetpx.mlt( kx, ky ) );
        rect vrct( min, max );
        vrct.expand( add );

        glOrtho( vrct.left, vrct.right, vrct.down, vrct.up, -1, 1 );

        float stepX = getGridStep( vrct.dx(), 200.0 / (float)this->w, 0 );
        float stepY = getGridStep( vrct.dy(), 200.0 / (float)this->h, 2 );

        glLineWidth( 1 );

        glColorGray( 0.8 );
        drawBase( vrct, max.x );

        glColorGray( 0.5 );
        for( float t = stepX; t < ( max.x - stepX/2.0 ); t+= stepX )
            drawVLine( t, vrct.down, vrct.up );


        for( float v = stepY; v < max.y; v += stepY )
            drawHLine( v, vrct.left, vrct.right, max.x );
        for( float v = -stepY; v > min.y; v -= stepY )
            drawHLine( v, vrct.left, vrct.right, max.x );

        glTranslatef(0,0,0.2);

    }
public:
    G2DViewer( std::string Tip )
    {
        tip = Tip;
        offsetpx.up = 10;
        offsetpx.down = 10;
        offsetpx.right = 60;
        offsetpx.left = 10;
    }
};

/*
 *
class G2DGrid: public GObject
{
protected:
public:
};
 *
 */

/*
 * TODO: write mouse and keyboard reaction
 */
class GTimeTraceViewer: public GLViewHandler
{
protected:
    GLStackHandler st;
    GLHorisontalLayout lv;
public:
    virtual void init() {}
    void destroy() 
    {  
        for( int i = 0; i < st.size(); i++ )
            delete st[i];
    }
    GTimeTraceViewer()
    {
        lv.setStackHandler( &st );
        cur = &lv;
        lv.reshape( *this );
    }

    void pushPlot( GObject *obj, std::string tip )
    {
        if( !obj ) return;
        GLObjectViewer *v = new G2DViewer( tip );
        v->objlist.push_back( obj );
        st.push_back( (GLViewState*)v );
    }

    void pushPlot( std::vector<GObject *> &obj, std::string tip )
    {
        if( obj.size() == 0 ) return;
        GLObjectViewer *v = new G2DViewer( tip );
        for( int i = 0; i < obj.size(); i++ )
            v->objlist.push_back( obj[i] );
        st.push_back( (GLViewState*)v );
    }

    void update()
    {
        st.updateAll();
    }

    void clearPlots() { destroy(); }

    ~GTimeTraceViewer() { destroy(); }
};

#define FAILDRAW
class GSphere: public GObject
{
protected:
    GLuint buf;
    int pcnt;
    float rad;
    bool changes;
    float stepx, stepy;

    void initSpere()
    {
        float wpi = M_PI*2.0;
        float hpi = M_PI_2;
        int kx = wpi / stepx;
        int ky = hpi / stepy;

        int pcnt = (kx+1) * (ky+1) * 2;
        printf( "kx: %d  ky: %d  pcnt: %d\n", kx, ky, pcnt );
        trpoint *tmp = new trpoint[pcnt];
        for( int i = 0; i <= ky; i++ )
        {
            for( int j = 0; j <= kx; j++ )
            {
                float q = j * stepy;
                float p = i * stepx;
                int k = i * kx + j;
                trpoint buf;
                buf.x = cos( q ) * cos( p ) * rad;
                buf.y = sin( q ) * cos( p ) * rad;
                buf.z = sin( p ) * rad;
                tmp[ k*2 ] = buf;
                buf.x = cos( q ) * cos( -p ) * rad;
                buf.y = sin( q ) * cos( -p ) * rad;
                buf.z = sin( -p ) * rad;
                tmp[ k*2+1 ] = buf;
            }
        }
        if( buf ) glDeleteBuffers( 1, &buf );
        glGenBuffers( 1, &buf );
        glBindBuffer( GL_ARRAY_BUFFER, buf );
        glBufferData( GL_ARRAY_BUFFER, pcnt * sizeof(trpoint),
                tmp, GL_STATIC_DRAW );
        delete [] tmp;
        changes = 0;
    }
public:
    GSphere( float radius = 6378.1e3 ):
        rad( radius ),
        buf(0),
        changes(1),
        stepx(0.1),
        stepy(0.1),
        col( 0, 0.5, 1 )
    { }

    float getRad() const { return rad; }
    void setRad( float nrad ) { rad = nrad; changes = 1; }

    color3d col;

    point3d pos() const { return point3d(); }

    void draw()
    {
        #ifdef FAILDRAW
        glColor3f( col.r, col.g, col.b );
        float wpi = M_PI * 2.0;
        float hpi = M_PI_2;
        int kx = wpi / stepx;
        int ky = hpi / stepy;

        glPointSize( 2 );
        glBegin( GL_POINTS );
        for( int i = 0; i <= ky; i++ )
        {
            for( int j = 0; j <= kx; j++ )
            {
                float q = j * stepy;
                float p = i * stepx;
                int k = i * kx + j;
                trpoint buf;
                buf.x = cos( q ) * cos( p ) * rad;
                buf.y = sin( q ) * cos( p ) * rad;
                buf.z = sin( p ) * rad;
                glVertex3f(buf.x, buf.y, buf.z);
                buf.x = cos( q ) * cos( -p ) * rad;
                buf.y = sin( q ) * cos( -p ) * rad;
                buf.z = sin( -p ) * rad;
                glVertex3f(buf.x, buf.y, buf.z);
            }
        }
        glEnd();

        #else
        if( changes ) initSpere();
        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glEnable( GL_VERTEX_ARRAY );
        glEnable( GL_DEPTH_TEST );

        glColor3f( col.r, col.g, col.b );

        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();

        this->setTransform();

        glPointSize(2);

        glBindBuffer( GL_ARRAY_BUFFER, buf );
        glVertexPointer( 3, GL_FLOAT, 0, 0 );
        glDrawArrays( GL_POINTS, 0, pcnt );

        glPopMatrix();
        glPopAttrib();
        #endif
    }
};


#endif//GVTEMPL_H
