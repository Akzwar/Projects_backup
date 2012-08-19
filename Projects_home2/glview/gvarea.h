#ifndef GVAREA_H
#define GVAREA_H

#include "gvstate.h"
#include <vector>
#include <cstdio>

class GLStackHandler: public std::vector<GLViewState*>
{
public:
    void initAll()
    {
        for( int i = 0; i < (*this).size(); i++ )
            (*this)[i]->init();
    }
    void destroyAll()
    {
        for( int i = 0; i < (*this).size(); i++ )
            (*this)[i]->destroy();
    }
    void idleAll()
    {
        for( int i = 0; i < (*this).size(); i++ )
            (*this)[i]->idle();
    }
    void timerAll( int tNo )
    {
        for( int i = 0; i < (*this).size(); i++ )
            (*this)[i]->timer( tNo );
    }
    void releaseFromActiveAll()
    {
        for( int i = 0; i < (*this).size(); i++ )
            (*this)[i]->releaseFromActive();
    }
    void updateAll()
    {
        for( int i = 0; i < (*this).size(); i++ )
            (*this)[i]->update();
    }
};

class GLLayout: public GLViewState
{
protected:
    int lastActive;
    static bool hitTest( const GLViewState *v, int mx, int my )
    {
        return (mx >= v->x) && (mx < v->w + v->x) &&
               (my >= v->y) && (my < v->h + v->y);
    }

    void choiseOne( int f, unsigned char k, int btn, int st, int mx, int my )
    {
        for( int i = gsh->size()-1; i > -1; i-- )
        {
            GLViewState *buf = gsh->at(i);
            if( hitTest( buf, mx, my ) )
            {
                if( i != lastActive && gsh->size() > lastActive && lastActive >=0 )
                    gsh->at( lastActive )->releaseFromActive();
                lastActive = i;
                buf->setAsActive();
                int lx = mx - buf->x;
                int ly = my - buf->y;
                switch( f )
                {
                    case 1: buf->keyboard( k, lx, ly ); return;
                    case 2: buf->mouse( btn, st, lx, ly ); return;
                    case 3: buf->motion( lx, ly ); return;
                    case 4: buf->passive( lx, ly ); return;
                    default: return;
                }
            }
        }
    }
    virtual void reshapeHook() { layoutSet(); }

    virtual void layoutSet() = 0;
    GLStackHandler *gsh;

public:

    void setStackHandler( GLStackHandler *Gsh )
    {
        gsh = Gsh;
        layoutSet();
    }

    void init() {}
    void destroy() {}

    virtual void draw()
    {
        for( int i = 0; i < gsh->size(); i++ )
        {
            GLViewState *buf = gsh->at(i);
            glViewport( buf->x, buf->y, buf->w, buf->h );
            buf->draw();
        }
        glViewport( this->x, this->y, this->w, this->h );
    }

    void idle() { gsh->idleAll(); }
    void timer( int tNo ) { gsh->timerAll( tNo ); }

    void keyboard( unsigned char k, int mx, int my )
    { choiseOne( 1, k, 0, 0, mx, my ); }

    void mouse( int btn, int st, int mx, int my )
    { choiseOne( 2, 0, btn, st, mx, my ); }

    void motion( int mx, int my ) { choiseOne( 3, 0, 0, 0, mx, my ); }

    void passive( int x, int y ) { choiseOne( 4, 0, 0, 0, x, y ); }

    void setAsActive()
    {
        layoutSet();
        if( lastActive < gsh->size() && lastActive >= 0 )
            gsh->at( lastActive )->setAsActive();
        else if( gsh->size() > 0 )
            gsh->at( gsh->size() - 1 )->setAsActive();
    }

    void releaseFromActive() { gsh->releaseFromActiveAll(); }
};

class GLTileLineLayout: public GLLayout
{
protected:
/*
    inline static int norm( int val, int min, int max )
    {
        return val>min ? ( val <= max ? val : max ) : min;
    }
*/

    void layoutSet( )
    {
        int count = this->gsh->size();
        if( !count ) return;
        std::vector<double> stretch;
        double summ;
        int nostretch = 0;
        for( int i = 0; i < count; i++ )
        {
            GLViewState *buf = this->gsh->at(i);
            double s = hor?buf->ph.s:buf->pw.s;
            s = s>0?s:0;
            summ += s;
            stretch.push_back( s );
            if( s == 0 )
                nostretch += hor?buf->h:buf->w;
        }
        for( int i = 0; i < count; i++ )
            stretch[i] /= summ;
        int step;
        int offset = 0;
        for( int i = 0; i < count; i++ )
        {
            int sk = (hor?this->h:this->w) - nostretch;
            step = stretch[i] * ( sk>0?sk:0 );
            GLViewState *buf = this->gsh->at(i);
            if( !step ) step = hor?buf->h:buf->w;
            ViewRect res;
            // TODO: write algo wtih min max area size
            if( hor )
            {
                //step = norm( step, buf->ph.min, buf->ph.max );
                res.x = this->x;
                res.w = this->w;
                res.y = offset + this->y;
                res.h = step;
            }
            else
            {
                //step = norm( step, buf->pw.min, buf->pw.max );
                res.x = offset + this->x;
                res.w = step;
                res.y = this->y;
                res.h = this->h;
            }
            buf->reshape( res );
            offset += step;
        }
    }
public:
    bool hor;
    GLTileLineLayout( bool Hor = 1 ): hor( Hor ) {}
};

class GLHorisontalLayout: public GLTileLineLayout
{
public:
    GLHorisontalLayout(): GLTileLineLayout( 1 ) {}
};

class GLVerticalLayout: public GLTileLineLayout
{
public:
    GLVerticalLayout(): GLTileLineLayout( 0 ) {}
};

#endif//GVAREA_H
