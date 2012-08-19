#ifndef GVSTATE_H
#define GVSTATE_H

#define GL_GLEXT_PROTOTYPES
#include <GL/freeglut.h>

struct ViewRect
{
    int x, y, w, h;
    ViewRect( int W=200, int H=300, int X=0, int Y=0 ):
        x(X), y(Y), w(W), h(H)
    {}
};

struct ViewParams: public ViewRect
{
    ViewParams( int W=200, int H=300, int X=0, int Y=0,
                int pwmin=0, int pwmax=100000,
                int phmin=0, int phmax=100000,
                double pws=1, double phs=1 ):
        ViewRect( W, H, X, Y )
    {
        /*
        pw.min = pwmin;
        pw.max = pwmax;
        ph.min = phmin;
        ph.max = phmax;
        */
        pw.s = pws;
        ph.s = phs;
    }
    struct 
    { 
        //int min, max; 
        double s; 
    } 
    pw, ph;
};

class GLViewState: public ViewParams
{
protected:
    virtual void reshapeHook(){}
public:

    GLViewState(): ViewParams() {}

    virtual void init() = 0;
    virtual void draw() = 0;
    virtual void destroy() = 0;

    void reshape( int _w, int _h )
    {
        this->w = _w;//this->pw.s>0?_w:this->w;
        this->h = _h;//this->ph.s>0?_h:this->h;
        reshapeHook();
    }
    void reshape( int _x, int _y, int _w, int _h )
    {
        this->x = _x;
        this->y = _y;
        reshape( _w, _h );
    }
    void reshape( const ViewRect &v )
    {
        this->x = v.x;
        this->y = v.y;
        reshape( v.w, v.h );
    }
    virtual void keyboard( unsigned char k, int mx, int my ) {}
    virtual void idle() {}
    virtual void mouse( int btn, int st, int mx, int my ) {}
    virtual void motion( int mx, int my ) {}
    virtual void wheel( int wh, int dir, int mx, int my ){}
    virtual void passive( int mx, int my ) {}
    virtual void timer( int tNo ) {}

    virtual float getTimerDelay( int tNo ) { return 1.0f/60.0f; }

    virtual void setAsActive() {}
    virtual void releaseFromActive() {}
    virtual void update() {}
};


class GLViewHandler: public GLViewState
{
protected:
    virtual void reshapeHook() { cur->reshape( *this );}
    virtual void drawPreHook() {}
    virtual void drawPostHook() {}
    virtual bool keyboardHookCheck( unsigned char k, int mx, int my ) { return 1; }
    virtual void idleHook() {}
    virtual void mouseHook( int btn, int st, int mx, int my ) {}
    virtual void motionHook( int mx, int my ) {}
    virtual void passiveHook( int mx, int my ) {}
    virtual void timerHook( int tNo ) {}

    void setCurrent( GLViewState *st )
    {
        if( cur )
            cur->releaseFromActive();
        cur = st;
        cur->reshape( *this );
        cur->setAsActive();
    }

    GLViewState *cur;
public:
    GLViewHandler(): cur(0) {}

    /*
     * In void init() cur must be initialize
    void init()
    {

    }

    void destroy()
    {

    }
     */

    void draw()
    {
        drawPreHook();
        cur->draw();
        drawPostHook();
    }

    void keyboard( unsigned char k, int mx, int my )
    {
        if( keyboardHookCheck( k, mx, my ) )
            cur->keyboard( k, mx, my );
    }

    void idle()
    {
        idleHook();
        cur->idle();
    }

    void mouse( int btn, int st, int mx, int my )
    {
        mouseHook( btn, st, mx, my );
        cur->mouse( btn, st, mx, my );
    }

    void motion( int mx, int my )
    {
        motionHook( mx, my );
        cur->motion( mx, my );
    }

    void passive( int mx, int my )
    {
        passiveHook( mx, my );
        cur->passive( mx, my );
    }

    void timer( int tNo )
    {
        timerHook( tNo );
        cur->timer( tNo );
    }

    void setAsActive() { cur->setAsActive(); }
    void releaseFromActive() { cur->setAsActive(); }
};

#endif//GVSTATE_H
