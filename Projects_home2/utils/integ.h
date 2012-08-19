#ifndef DVINTEG_H
#define DVINTEG_H

#include <vector>
#include <cmath>
#include <limits>
#include <malloc.h>

namespace dvinteg
{

#define templatedef template< class VState, class TmpData, class FixData, class DD=LD >

typedef long double LD;

template< int N, class DD=LD >
class State
{
protected:
    DD data[N];
    friend class State;
public:
    DD time;
    State(): time(0) { memset( data, 0, sizeof(DD)*N ); }

    State( const State<N,DD> &b ):
        time(0)
    {
        memset( data, 0, sizeof(DD)*N );
        for( int i = 0; i < N; i++ )
            data[i] = b.data[i];
        this->time = b.time;
    }

    inline int size() const { return N; }

    inline DD operator [] ( int i ) const { return data[i]; }

    inline DD& operator [] ( int i ) { return data[i]; }

    State<N,DD> operator = ( const State<N,DD> &b )
    {
        for( int i = 0; i < N; i++ )
            data[i] = b.data[i];
        this->time = b.time;
        return (*this);
    }

    State<N,DD> operator += ( const State<N,DD> &b )
    {
        for( int i = 0; i < N; i++ )
            data[i] += b.data[i];
        return (*this);
    }

    State<N,DD> operator -= ( const State<N,DD> &b )
    {
        for( int i = 0; i < N; i++ )
            data[i] -= b.data[i];
        return (*this);
    }

    State<N,DD> operator *= ( const DD &b )
    {
        for( int i = 0; i < N; i++ )
            data[i] *= b;
        return (*this);
    }

    State<N,DD> operator /= ( const DD &b )
    {
        for( int i = 0; i < N; i++ )
            data[i] /= b;
        return (*this);
    }

    State<N,DD> operator + ( const State<N,DD> &b ) const
    {
        State<N,DD> ret;
        for( int i = 0; i < N; i++ )
            ret.data[i] = data[i] + b.data[i];
        return ret;
    }

    State<N,DD> operator - ( const State<N,DD> &b ) const
    {
        State<N,DD> ret;
        for( int i = 0; i < N; i++ )
            ret.data[i] = data[i] - b.data[i];
        return ret;
    }

    State<N,DD> operator * ( const DD &b ) const
    {
        State<N,DD> ret;
        for( int i = 0; i < N; i++ )
            ret.data[i] = data[i] * b;
        return ret;
    }

    State<N,DD> operator / ( const DD &b ) const
    {
        State<N,DD> ret;
        for( int i = 0; i < N; i++ )
            ret.data[i] = data[i] / b;
        return ret;
    }

    DD operator ^ ( const State<N,DD> &b ) const
    {
        DD ret = 0;
        for( int i = 0; i < N; i++ )
            ret += data[i] * b.data[i];
        return ret;
    }

};

templatedef
class Function
{
public:
    virtual VState call( const VState &s,
            const TmpData &data, const DD &dt ) const = 0;
    virtual void post( VState &x, TmpData &tmp,
            const FixData &fix, const DD &dt ) const = 0;
};

template< class VState, class DD=LD >
class SimpleTrace
{
public:
    DD h;
    DD dpDenseSdh;
    virtual void pushback( const VState &st, const DD &dt ) = 0;
    virtual const VState& last() const = 0;
    virtual void pushData() = 0;
    virtual void popData() = 0;
    virtual VState f( const VState &st, const DD &dt ) const = 0;
    SimpleTrace( DD H ): h(H), dpDenseSdh(0) {}
};

templatedef
class Trace: public SimpleTrace< VState, DD >, public std::vector<VState>
{
protected:
    typedef Function< VState, TmpData, FixData, DD > TraceFunc;
    const TraceFunc *fnc;

    TmpData tmpData;
    TmpData tmpDataInit;
    TmpData tmpDataPush;

    FixData fixData;
    VState statePush;
    VState stateInit;
    VState stateLast;

    inline void pushLast( const DD &dt )
    {
        fnc->post( stateLast, tmpData, fixData, dt );
        this->push_back( stateLast );
    }

public:
    Trace( const VState  &init,
           const TmpData &tmpdata,
           const FixData &fixdata,
           const TraceFunc *fn,
           DD step = 0.0001
         ):
        SimpleTrace<VState, DD>(step),
        tmpData(tmpdata),
        tmpDataInit(tmpdata),
        tmpDataPush(tmpdata),
        fixData(fixdata)
    {
        stateInit = init;
        stateInit.time = 0;
        stateLast = stateInit;
        fnc = fn;
        pushLast( this->h );
    }

    void pushback( const VState &st, const DD &dt )
    {
        DD t = stateLast.time;
        stateLast = st;
        stateLast.time = t + dt;
        pushLast( dt );
    }

    void pushData()
    {
        statePush = stateLast;
        tmpDataPush = tmpData;
    }

    void popData()
    {
        stateLast = statePush;
        tmpData = tmpDataPush;
    }

    const VState& last() const { return stateLast; }
    const FixData& getFix() const { return fixData; }

    VState f( const VState &s, const DD &dt ) const
    { return fnc->call( s, tmpData, dt ); }

    void reset()
    {
        this->clear();
        this->resize(0);
        stateLast = stateInit;
        tmpData = tmpDataInit;
        pushLast( this->h );
    }

    virtual ~Trace(){}
};

//template< class VS, class DD=LD >
class Integrator
{
public:
    //template< class VS, class DD=LD >
    //virtual void run( SimpleTrace< VS, DD > &x, DD time ) const = 0;
};


class EUInteg: public Integrator
{
public:
    template< class VS, class DD=LD >
    void run( SimpleTrace< VS, DD > &x, DD time ) const
    {
        VS x0, x1;
        for( DD t = 0; t < time; t+=x.h )
        {
            x0 = x.last();
            x1 = x0 + x.f( x0, x.h ) * x.h;
            x.pushback( x1, x.h );
        }
    }
};

class RKInteg: public Integrator
{
public:
    template< class VS, class DD=LD >
    void run( SimpleTrace< VS, DD > &x, DD time ) const
    {
        VS k[4], x1, x0;
        for( DD t = 0; t < time; t+=x.h )
        {
            x0 = x.last();
            x1 = x0;
            k[0] = x.f( x0, 0 ) * x.h;
            k[1] = x.f( x0 + k[0] * 0.5, 0.5 * x.h ) * x.h;
            k[2] = x.f( x0 + k[1] * 0.5, 0.5 * x.h ) * x.h;
            k[3] = x.f( x0 + k[2], x.h ) * x.h;
            x1 += ( k[0] + k[1] * 2.0 + k[2] * 2.0 + k[3] ) / 6.0;
            x.pushback( x1, x.h );
        }
    }
};

template< class DD=LD >
inline DD max3( DD a1, DD a2, DD a3 )
{
    DD mx = -std::numeric_limits<DD>::max();
    if (a1>mx) mx = a1;
    if (a2>mx) mx = a2;
    if (a3>mx) mx = a3;
    return mx;
}

template< class DD=LD > inline DD min( DD a1, DD a2 ) { return (a1<a2)?a1:a2; }
template< class DD=LD > inline DD max( DD a1, DD a2 ) { return (a1>a2)?a1:a2; }

class DPInteg: public Integrator
{
protected:
    const static LD a[];
    const static LD b[];
    const static LD c[];
    const static LD b0k[];
    const static LD b2k[];
    const static LD b3k[];
    const static LD b4k[];
    const static LD b5k[];

    LD max_err;
    LD denseStep;

    template< class VS, class DD=LD >
    void calcBuf( const SimpleTrace<VS,DD> &tr, VS &x1, VS *k, DD &h, DD &hn, DD min_h = -1 ) const
    {
        hn = h;
        VS temp;
        VS x0 = tr.last();
        VS x5;
        while( 1 )
        {
            if( min_h > 0 )
                h = min<DD>( hn, min_h );
            else
                h = hn;
            x1 = x0;
            x5 = x0;
            for( int i = 0, z = 0; i < 7; i++)
            {
                temp = VS();
                for( int j = 0; j < i; j++)
                    temp += k[j] * a[z++];
                k[i] = tr.f( x0 + temp * h, c[i] * h );
                x1 += k[i] * ( a[15+i] * h );
                x5 += k[i] * ( b[ 0+i] * h );
            }

            LD err = 0;
            DD buf = 0;
            DD x0i, x1i, x5i;

            for( int i = 0; i < x0.size(); i++ )
            {
                x0i = x0[i];
                x1i = x1[i];
                x5i = x5[i];
                buf = h * (x1i - x5i) /
                    max3<DD>( 1.0e-5, fabs(x1i), fabs(x0i) );
                err += buf * buf;
            }
            err = sqrt( err / x0.size() );
            hn = h / max<DD>( 0.1, min<DD>( 5.0, pow( err/max_err, 0.2 ) / 0.9 ) );

            if( err <= max_err ) break;
        }
    }

    template< class VS, class DD=LD >
    void calcDense( SimpleTrace<VS,DD> &tr, const VS *k, DD h, DD &sdh ) const
    {
        tr.pushData();
        DD t, th, thS;
        VS x0 = tr.last(), out;
        LD bn[6];
        for( t = sdh + denseStep; t < h; t += denseStep )
        {
            th = t / h;
            thS = th * th;

            bn[0] = th * ( 1 + th * ( b0k[0] + th * ( b0k[1] + th * b0k[2] ) ) );
            bn[1] = 0;
            bn[2] = b2k[0] * thS * ( b2k[1] + th * ( b2k[2] + th * b2k[3] ) );
            bn[3] = b3k[0] * thS * ( b3k[1] + th * ( b3k[2] + th * b3k[3] ) );
            bn[4] = b4k[0] * thS * ( b4k[1] + th * ( b4k[2] + th * b4k[3] ) );
            bn[5] = b5k[0] * thS * ( b5k[1] + th * ( b5k[2] + th * b5k[3] ) );

            out = x0;
            for( int i = 0; i < 6; i++ )
                out += k[i] * bn[i] * denseStep;

            tr.pushback( out, denseStep );
        }
        sdh = -h + t;
        tr.popData();
    }

public:
    DPInteg( LD mErr=1.0e-18, LD step=0 ):
        max_err( mErr ),
        denseStep( fabs(step) )
    {}

    template< class VS, class DD=LD >
    void run( SimpleTrace<VS,DD> &tr, DD time ) const
    {
        VS k[7], res;
        DD h = tr.h, hn = tr.h, sdh = tr.dpDenseSdh;
        for( DD t = 0; t < time; t += h )
        {
            calcBuf<VS,DD>( tr, res, k, h, hn, denseStep );
            if( denseStep > 0 )
                calcDense<VS,DD>( tr, k, h, sdh );
            tr.h = hn;
            tr.dpDenseSdh = sdh;
            tr.pushback( res, h );
        }
    }
};

};

#endif//DPINTEG_H
