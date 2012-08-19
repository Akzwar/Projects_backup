#ifndef HELPERS_H
#define HELPERS_H

#include "randn.h"
#include "gcmdata.h"

/**
 * @file helpers.h
 * Collection of functions for many operations
 */ 

/**  
 * Function calculate grid step
 * @param limVal limit value, used as ~max value of axis
 * @param divCoef ratio of view to limVal (count of occurrences), dy default 5
 * @param subStepDiv count of substep finding, dy default 0
 * @param findStep ratio of serial find range values, dy default 10
 * @param subStepCoef substeping size, dy default binary 2
 * @return size of step for separation axis 
 */
inline double getGridStep( 
        double limVal, 
        double divCoef=5,
        unsigned int subStepDiv=0,
        double findStep=10,
        double subStepCoef=2 )
{
    double gs = fabs(limVal) * divCoef;
    double i;
    double fs = fabs(findStep);
    fs = fs>1?fs:2;
    double bdiv = fabs(subStepCoef);
    bdiv = bdiv>1?bdiv:2; 
    for( i = 1e-20; i < 1e20; i*=fs )
        if( gs >=i && gs < i*fs ) 
        {
            double k = fs/bdiv;
            while( subStepDiv > 0 && gs < i * k )
            {
                i /= bdiv;
                k /= bdiv;
                subStepDiv--;
             }
            break;
         }
    return i;
} 

/**
 * Function liner interpolation calc
 * class Type must have overloaded operators:
 *      Type operator * ( double b )
 *      Type operator + ( const Type &b )
 *
 * @param dx position
 * @param x0 position of value 0
 * @param x1 position of value 1
 * @param v0 value 0
 * @param v1 value 1
 * @return interpolation value for dx
 */
template <class Type>
inline Type linearInterp( double dx, double x0, double x1,
        Type v0, Type v1 )
{
    double k = ( dx - x0 ) / ( x1 - x0 );
    return v0 * ( 1.0 - k ) + v1 * k;
}

/**
 * Function fill buffer linear interpolation data
 * class Type must have overloaded operators:
 *      Type operator * ( double b )
 *      Type operator + ( const Type &b )
 * source data ( positions and values ) must be
 * arranged in order of increasing positions
 *
 * @param sxp[in] pointer to src data of positions weight
 * @param src[in] pointer to src data of values
 * @param scnt size of src data buffer
 * @param dst[out] pointer to dst data
 * @param dcnt size of dst data
 */
template <class Type>
inline void fillLinearInterp( double *sxp, Type *src, int scnt,
        Type *dst, int dcnt )
{ 
    double maxVal = sxp[scnt-1];
    double minVal = sxp[0];
    double range = maxVal - minVal;
    double step = range / (double)dcnt;

    int j = 0;
    for( int i = 0; i < dcnt; i++ )
    {
        double val = step*i + minVal;
        for( j = 0; j < scnt; j++ )
            if( val >= sxp[j] && val < sxp[j+1] ) break;
        dst[i] = linearInterp<Type>( val, sxp[j], sxp[j+1],
                src[j], src[j+1] );
    }
}

/**
 *
 */
template < class T >
class DataBuffer
{ 
public:
    virtual int vsize () const = 0;
    virtual T operator [] ( int i ) const = 0;
    virtual T& operator [] ( int i ) = 0;
};

/**
 * Function fill buffer with normal realisations
 *
 * @param v[out] pointer to data
 * @param cnt data size (in elems)
 * @param mu 
 * @param sigma
 */
inline void getNormalSamples( double *v, int cnt, double mu=0.0, double sigma=1.0 )
{
    for( int i = 0; i < cnt; i++ )
        v[i] = randn_trig( mu, sigma );
}

/** 
 * Function fill DataBuffer object with normal realisations
 * class T must have initialization with double
 *
 * @param obj
 * @param mu
 * @param sigma
 * @see getNormalSamples
 */
template < class T >
inline void getNormalSDataBuffer( DataBuffer<T> &obj, double mu=0.0, double sigma=1.0 )
{
    for( int i = 0; i < obj.vsize(); i++ )
        obj[i] = (T)randn_trig( mu, sigma );
}

inline double whiteNoiseGen( double dt ) { return randn_trig( 0, sqrt( 1.0 / dt ) ); }

// TODO: write uniform fillLinearInterp

template < class T >
inline void norm( T &x, T min, T max )
{
    if( x < min ) x = min;
    if( x > max ) x = max;
}

template < class T >
inline void fmod( T &x, T val, bool circle=0 )
{
    int count = x / val;
    x = x - val * (T)count;
    // FIXME: circle not work well
    if( circle ) x = -val + x;
}

inline void keplerToDecart( point3d &pos, point3d &vel, 
        long double OM, long double i, long double om, long double a, long double e, long double th, long double mu )
{
    long double rk = 1.0 / 180.0 * M_PI;
    OM *= rk;
    i  *= rk;
    om *= rk;
    th *= rk;
    long double sinth = sin(th);
    long double costh = cos(th);

    //long double p = a * ( 1.0 - e * e );
    long double p = a;
    long double r = p / ( 1.0 + e * costh );
    long double u = th + om;

    long double cosu = cos(u);
    long double sinu = sin(u);
    long double cosOM = cos(OM);
    long double sinOM = sin(OM);
    long double sini = sin(i);
    long double cosi = cos(i);

    long double px, py, pz, vx, vy, vz;

    px = r * ( cosu * cosOM - sinu * sinOM * cosi );
    py = r * ( cosu * sinOM - sinu * cosOM * cosi );
    pz = r * sinu * sini;

    long double mudp = sqrt( mu / p );
    long double Vr = mudp * e * sinth / r;
    long double Vn = mudp * ( 1.0 + e * costh );

    vx = px * Vr + Vn * ( -sinu * cosOM - cosu * sinOM * cosi );
    vy = py * Vr + Vn * ( -sinu * sinOM + cosu * cosOM * cosi );
    vz = pz * Vr + Vn * cosu * sini;
    pos = point3d( px, py, pz );
    vel = point3d( vx, vy, vz );
}

#endif//HELPERS_H
