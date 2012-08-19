#ifndef VIEWSYS_H
#define VIEWSYS_H

#define GL_GLEXT_PROTOTYPES
#define CMDATA_LONG_DOUBLE
#define HASINPUTFILE

#include "glview/gvsystem.h"
#include "mathmodel.h"

class GScreen1: public GTimeTraceViewer
{
protected:
    GTimeTraceDrawSmpl< MTrace, MState > w[3];
public:
    void setTrace( MTrace *tr )
    {
        int indexs[3] = { MState::w1, MState::w2, MState::w3 };
        color3d colors[1] = { color3d( 1, 0, 0 ) };
        for( int i = 0; i < 3; i++ )
        {
            w[i].setTrace( tr );
            w[i].setIndexs( 1, indexs+i, colors );
        }
    }
    void init()
    {
        const char *tip[3] = { "first", "second", "third" };
        for( int i = 0; i < 3; i++ )
            this->pushPlot( w+i, tip[i] );
    }
};

class GSimpleText: public GLViewState
{
protected:
    char str[512];
public:
    std::string text;
    void init() {}
    void destroy() {}
    void draw()
    {

        glOrtho(-1, 1, -1, 1, -1, 1 );
        glColor3f( 1, 1, 1 );
        glLineWidth( 4 );
        glBegin( GL_LINES );

        glVertex2f( -1, -1 );
        glVertex2f(  1, -1 );
        glVertex2f( -1,  1 );
        glVertex2f(  1,  1 );

        glEnd();

        glLineWidth( 1 );
        glBegin( GL_LINES );

        glVertex2f( -1, -1 );
        glVertex2f( -1,  1 );
        glVertex2f(  1, -1 );
        glVertex2f(  1,  1 );

        glEnd();

        sprintf( str, "%s", text.c_str() );
        // TODO: TODO: TODO: pos
        glWindowPos2d( this->x + 10, this->y + this->h - 15 );
        glutBitmapString( GLUT_BITMAP_HELVETICA_12,
                (unsigned char*) str );
    }
};

template< class ViewClass, int N >
class GLFixLineLayout: public GLViewHandler
{
protected:
    ViewClass view[N];
    GLStackHandler stack;
    GLTileLineLayout layout;
public:
    GLFixLineLayout( bool hor=1 ):
        layout( hor )
    {
        for ( int i = 0; i < N; i++ )
            stack.push_back( view+i );
    }

    void init()
    {
        stack.initAll();
        layout.setStackHandler( &stack );
        setCurrent( &layout );
    }

    const ViewClass & operator[] ( int i ) const { return *(view+i); }
    ViewClass & operator[] ( int i )  { return *(view+i); }

    void destroy() { stack.destroyAll(); }
};

class GScreen2: public GLViewHandler
{ 
protected:
    GLStackHandler stack;
    GLTileLineLayout layout;

    GTimeTraceViewer trv;
    GLFixLineLayout<GSimpleText,3> text;
    GLFixLineLayout<GSimpleText,3> text2;
    GLFixLineLayout<GLFixLineLayout<GSimpleText,3>,3> text3;
    GSimpleText stext;

    GTimeTraceDrawSmpl< MRTrace, MResult > k[3];
    GTimeTraceDrawSmpl< MRTrace, MResult > kf[3];
    GTimeTraceDrawSmpl< MRTrace, MResult > kr[3];

    GTimeTraceDrawAnti< MRTrace, MResult > akf[3];
    GTimeTraceDrawAnti< MRTrace, MResult > akr[3];
    MRTrace *res;
public:
    void setResult( MRTrace *tr )
    {
        trv.clearPlots();
        res = tr;
        int ik[1] = { 0 };
        color3d ck[1] = { color3d( 1, 0, 0 ) };
        int ikf[1] = { 1 };
        color3d ckf[1] = { color3d( 0, 0, 1 ) };
        int ikr[2] = { 2, 3 };
        color3d ckr[1] = { color3d( 0, 1, 0 ) };

        std::vector<GObject *> ks;
        const char *tip[3] = { "first", "second", "third" };
        for( int i = 0; i < 3; i++ )
        {

            k[i].setTrace( &(res[i]) );
            k[i].setIndexs( 1, ik, ck, 1 );
            kf[i].setTrace( &(res[i]) );
            kf[i].setIndexs( 1, ikf, ckf, 1 );
            kr[i].setTrace( &(res[i]) );
            kr[i].setIndexs( 2, ikr, ckr, 1 );

            akf[i].setTrace( &(res[i]) );
            akf[i].setIndexs( 1, ikf, ckf, 1 );
            akr[i].setTrace( &(res[i]) );
            akr[i].setIndexs( 2, ikr, ckr, 1 );

            ks.clear();
            ks.push_back( (GObject*)&(k[i]) );
            ks.push_back( (GObject*)&(kf[i]) );
            ks.push_back( (GObject*)&(kr[i]) );

            //ks.push_back( (GObject*)&(akf[i]) );
            //ks.push_back( (GObject*)&(akr[i]) );

            trv.pushPlot( ks, tip[i] );
        }
    }

    void init()
    { 
        text.pw.s = 0;
        text.w = 200;

        stack.push_back( &trv );
        stack.push_back( &text );
        //stack.push_back( &stext );
        //stack.push_back( &text2 );
        //stack.push_back( &text3 );
        stack.initAll();

        layout.hor = 0;
        layout.setStackHandler( &stack );
        setCurrent( &layout );
    }

    void destroy() {}

    void setAsActive()
    {
        layout.setAsActive();
        char buf[256];
        for( int i = 0; i < 3; i++ )
        {
            sprintf( buf, "mo: % 8.4Lf  sigma: % 8.4Lf\nD:  % 8.4Lf  sigma: % 8.4Lf\nP( r in [r1, r2] ) = % 8.5Lf",
                res[i].MO, res[i].MOs, res[i].D, res[i].Ds, res[i].rp );
            text[i].text = buf;
        }
        trv.update();
    }

    void releaseFromActive()
    {

    }
};

class GLViewLab3: public GLMainView
{
protected:
    MathModel *mm;
    GScreen1 screen1;
    GScreen2 screen2;

    MFixData mfix;
    LD calcStep;

    bool run;
    void idleHook()
    {
        if( run ) 
            mm->step( needTime );
        run = 0;
    }

    void drawPostHook()
    {
        if( run ) return;
        char str[90];
        sprintf( str, "BINGO!" );
        glWindowPos2d( this->w - 100, 10 );
        glutBitmapString( GLUT_BITMAP_HELVETICA_12,
                (unsigned char*) str );
    }

    void saveScreen()
    {
        char c_time[90];
        sprintf( c_time, "_%09.1Lf", mm->tr.last().time );
        glScreenShot( outfolder + '/' + input + c_time );
    }

    void calculate()
    {
        run = 0;
        mm->calc();
        setCurrent( &screen2 );
        samplemode = 1;
    }

    void resample()
    {
        mm->reset();
        setCurrent( &screen1 );
        samplemode = 0;
        run = 1;
    }

    bool samplemode;
    void pause() { if( !samplemode ) run = !run; }

    bool keyboardHookCheck( unsigned char k, int x, int y )
    {
        if( k == ' ' ) pause();
        else if( k == 's' ) saveScreen();
        else if( k == 'c' ) calculate();
        else if( k == 'r' ) resample();
        else return 1;
        return 0;
    }
    std::string input;
    std::string outfolder;
    LD needTime;
public:

    void readFile( const char *fname )
    {
        //TODO: remove whitespaces
        input = std::string( fname );
        outfolder = "screenshots";
        FILE *f = fopen( fname, "r" );
        if( f )
        {
            LD d, beta, lambda;
            fscanf( f, "%Lf %Lf %Lf",
                         &d, &beta, &lambda );
            printf( "input: D: %Lf   beta: %Lf   lambda: %Lf\n",
                          d, beta, lambda );
            mfix.setVars( d, beta, lambda );
            printf( "calc filter params\n" );
            printf( "  f1: K: %Lf   T: %Lf\n", mfix.w1.K, mfix.w1.T );
            printf( "  f2: K: %Lf   T1: %Lf   T2: %Lf   Nu: %Lf\n",
                    mfix.w2.K, mfix.w2.T1, mfix.w2.T2, mfix.w2.Nu );
            printf( "  f3: K: %Lf   T1: %Lf   T2: %Lf\n",
                    mfix.w3.K, mfix.w3.T1, mfix.w3.T2 );

            fscanf( f, "%Lf", &calcStep );
            fscanf( f, "%Lf", &needTime );
            fclose( f );
        }
    }

    void init()
    {
        run = 1;
        samplemode = 0;
        mm = new MathModel( mfix, calcStep );

        screen1.setTrace( &(mm->tr) );
        screen1.init();

        screen2.setResult( mm->res );
        screen2.init();

        setCurrent( &screen1 );
    }

    void destroy()
    {
        /*
        st.destroyAll();
        delete mm;
        */
    }

};

#endif//VIEWSYS_H
