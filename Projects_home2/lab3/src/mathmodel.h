#ifndef MATHMODEL_H
#define MATHMODEL_H

#include <stdexcept>
#include <cstring>
#include <cstdio>

#include "utils/integ.h"
#include "utils/helpers.h"

using namespace dvinteg;

#define VARCNT 7

struct MTmpData { LD v[5]; };

class MState: public State< VARCNT, LD >
{ 
public:
    MState(): State< VARCNT, LD >() {}
    MState( const State< VARCNT, LD > &b ):
        State< VARCNT, LD > ( b )
    {}

    static const int w1 = 0;
    static const int w2 = 1;
    static const int w3 = 4;

    static const int w2q = 2;
    static const int w2r = 3;

    static const int w3q = 5;
    static const int w3r = 6;

     /*      state        tmp 
     * out  0 - w1       0
     *
     * out  1 - w2       1
     *      2 - w2q      
     *      3 - w2r      2
     *
     * out  4 - w3       3: K * ( T1 * w3r + w3q )
     *      5 - w3q
     *      6 - w3r      4
     *
     */

    MState getRightPart( const MTmpData &tmp, const LD &dt ) const
    {
        MState ret;

        ret[MState::w1]  = tmp.v[0];

        ret[MState::w2]  = 0;//tmp.v[1];
        ret[MState::w2q] = (*this)[MState::w2r];
        ret[MState::w2r] = tmp.v[2];

        ret[MState::w3]  = 0;//tmp.v[3];
        ret[MState::w3q] = (*this)[MState::w3r];
        ret[MState::w3r] = tmp.v[4];
        
        return ret;
    }
};

struct MFixData 
{  
    LD d, beta, lambda; 
    struct { LD K, T; } w1;
    struct { LD K, T1, T2, Nu, dtnu, TT2; } w2;
    struct { LD K, T1, T2, TT2; } w3;

    void setVars( LD D, LD Beta, LD Lambda )
    { 
        d = D; beta = Beta; lambda = Lambda;
        LD sigma = sqrt(d);

        w1.K = sigma * sqrt( 2.0 / lambda );
        w1.T = 1.0 / lambda;

        LD l2b2 = lambda * lambda + beta * beta;
        //w2.K = sigma * sqrt( lambda / sqrt(l2b2) );
        w2.K = sigma * sqrt( 2.0 * lambda / l2b2 );
        w2.T1 = sqrt( 1.0 / l2b2 );
        w2.T2 = w2.T1;
        w2.TT2 = w2.T2 * w2.T2;
        //w2.Nu = sqrt( fabs( lambda * lambda - beta * beta ) / ( 2.0 * l2b2 ) + 0.5 );
        w2.Nu = sqrt( ( lambda * lambda - beta * beta ) / ( 2.0 * l2b2 * l2b2 ) + 0.5 );
        w2.dtnu = 2.0 * w2.T2 * w2.Nu;

        /*
        w3.K = sqrt( 4.0 * d / lambda );
        w3.T1 = 0.5 / lambda;
        w3.T2 = 1.0 / lambda;
        */
        w3.K = sigma * sqrt( 2.0 / lambda );
        w3.T1 = sqrt( fabs( 1.0 - lambda ) / pow( lambda, 3 ) );
        w3.T2 = 1.0 / lambda;
        w3.TT2 = w3.T2 * w3.T2;
    }

    inline LD K( int i, LD t ) const { return Kn(i,t) * d; }

    inline LD Kn( int i, LD t ) const
    { 
        LD base = exp( -lambda * t );
        if( i == 0 ) return base; 
        else if( i == 1 ) return base * cos( beta * t );
        else if( i == 2 ) return base * ( 1.0 + lambda * t );
        else return base;
    }

    // расчёт временных данных ( правые части и значения для 2х шумов )
    MTmpData calcBufRP( const MState &s, const LD &dt ) const
    { 
        MTmpData tmp;

        LD wn[3];
        for( int i = 0; i < 3; i++ )
            wn[i] = whiteNoiseGen( dt );
        
        tmp.v[0] = ( (*this).w1.K * wn[0] - s[MState::w1] ) / (*this).w1.T;

        // первая линейная комбинация
        tmp.v[1] = (*this).w2.K * ( (*this).w2.T1 * s[MState::w2r] + s[MState::w2q] );
        tmp.v[2] = ( wn[1] - s[MState::w2r] * (*this).w2.dtnu - s[MState::w2q] ) /\
                   (*this).w2.TT2;
        
        // вторая линейная комбинация
        tmp.v[3] = (*this).w3.K * ( (*this).w3.T1 * s[MState::w3r] + s[MState::w3q] );
        tmp.v[4] = ( wn[2] - s[MState::w3r] * (*this).w3.T2 * 2 - s[MState::w3q] ) /\
                   (*this).w3.TT2;

        return tmp;
    }
};

class MFunction: public Function< MState, MTmpData, MFixData >
{
public:
    MState call( const MState &s, const MTmpData &tmp, const LD &dt ) const
    {
        return s.getRightPart( tmp, dt );
    }
    void post( MState &st, MTmpData &tmp, const MFixData &fix, const LD &dt ) const
    {
        tmp = fix.calcBufRP( st, dt );
        // присвоение шумам вычисленных значений ( линейных комбинаций )
        st[MState::w2] = tmp.v[1];
        st[MState::w3] = tmp.v[3];
    }
} mfunc;

typedef Trace< MState, MTmpData, MFixData > MTrace;

#define RESULTCNT 4

class MResult: public State< RESULTCNT, LD >
{ 
public:
    MResult(): State< RESULTCNT, LD >() {}
    MResult( const State< RESULTCNT, LD> &b ): 
        State< RESULTCNT, LD >( b ) {}
};


class MRTrace: public std::vector< MResult >
{ 
protected:
public:
    LD MO, MOs;
    LD D, Ds;
    LD rp;
};

//#define USEDPINTEG

class MathModel
{
    #ifdef USEDPINTEG
    DPInteg integ;
    #else
    EUInteg integ;
    #endif
public:
    MTrace tr;
    MRTrace res[3];

    MathModel( const MFixData &mfix, LD integStep = 1 ):
        #ifdef USEDPINTEG
        integ( 1.0e-18, integStep ),
        #endif
        tr( MState(), MTmpData(), mfix, &mfunc, integStep )
    { srand( time(NULL) ); }

    inline void step( LD dt ) { integ.run<MState,LD>( tr, dt ); }

    inline void calc()
    {
        int wId[3] = { MState::w1, MState::w2, MState::w3 };
        int count = tr.size();
        if( count < 2 ) return;
        MFixData fix = tr.getFix();
        for( int i = 0; i < 3; i++ )
        {
            LD summ = 0;
            for( int k = 0; k < count; k++ )
                summ += tr[k][wId[i]];
            res[i].MO = summ / (LD)count;

            summ = 0;
            for( int k = 0; k < count; k++ )
                summ += pow( res[i].MO - tr[k][wId[i]], 2 );
            res[i].D = summ / (LD)(count - 1);

            res[i].MOs = sqrt( res[i].D / (LD)count ); 
            res[i].Ds = sqrt( 2.0 / (LD)(count-1) ) * res[i].D;

            int zps = 120;
            //int zps = 10;
            if( count < zps ) return;
            LD dt = 0;
            int rp = 0;
            int kk = count - zps;
            res[i].resize( kk );
            for( int k = 0; k < kk; k++ )
            {
                summ = 0;
                for( int s = 0; s < count - k; s++ )
                    summ += ( tr[s][wId[i]] - res[i].MO ) * ( tr[s+k][wId[i]] - res[i].MO );
                MResult buf;
                buf.time = tr[k].time;
                LD r;
                buf[0] = summ / (LD)( count - k - 1 ) / res[i].D;
                buf[1] = r = fix.Kn( i, buf.time );

                LD sigma = sqrt( 1.0 / (LD)( count - k - 3 ) ) * 3;
                LD z = 0.5 * log( ( 1 + r ) / ( 1 - r ) );
                LD exp2z1 = exp( 2.0 * ( z - sigma ) );
                LD exp2z2 = exp( 2.0 * ( z + sigma ) );
                LD r1 = ( exp2z1 - 1 ) / ( exp2z1 + 1 );
                LD r2 = ( exp2z2 - 1 ) / ( exp2z2 + 1 );

                buf[2] = r1;
                buf[3] = r2;

                if( buf[0] > r1 && buf[0] < r2 ) rp++;

                res[i][k] = buf;
            }
            res[i].rp = (LD)rp / (LD)(count - zps);
            fprintf( stderr, "calc trace[%d] (count=%d) finish\n", i, count );
        }
    }

    inline void reset() 
    { 
        tr.reset(); 
        for( int i = 0; i < 3; i++ )
        {
            res[i].clear();
            res[i].resize(0);
        }
    }
};

#endif//MATHMODEL_H
