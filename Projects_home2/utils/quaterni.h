#pragma once

#ifndef QUATERNI_H
#define QUATERNI_H 

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "gcmdata.h"
#include "helpers.h"

typedef long double DD;
//typedef double DD;

struct ExceptQuat : public std::exception
{ 
   std::string s;
   ExceptQuat(std::string ss) : s(ss) {}
   const char* what() const throw() { return s.c_str(); }
   virtual ~ExceptQuat() throw () {}
};

class Quaterni: public DataBuffer< DD >
{  
public:

    DD x, y, z, w;

    Quaterni( DD X = 0, DD Y = 0, DD Z = 0, DD W = 0 ):
        x(X), y(Y), z(Z), w(W)
    {}

    Quaterni( const point4d &p ):
        x(p.x), y(p.y), z(p.z), w(p.w)
    {}

    point4d toPoint4d() const 
    {
        point4d ret;
        ret.x = x;
        ret.y = y;
        ret.z = z;
        ret.w = w;
        return ret;
    }

    inline int vsize() const { return 4; }

    DD operator[] ( int i ) const
    {
        switch( i )
        {
            case 0: return x; break;
            case 1: return y; break;
            case 2: return z; break;
            case 3: return w; break;
            default:
                throw std::out_of_range( "DD Quaterni::operator[] const" );
                break;
        }
    }

    DD& operator[] ( int i )
    {
        switch( i )
        {
            case 0: return x; break;
            case 1: return y; break;
            case 2: return z; break;
            case 3: return w; break;
            default:
                throw std::out_of_range( "DD& Quaterni::operator[]" );
                break;
        }
    }

    Quaterni operator = ( const Quaterni &b )
    {
        for( int i = 0; i < vsize(); i++ )
            (*this)[i] = b[i];
        return Quaterni( *this );
    }

    bool operator == ( const Quaterni &b ) const
    {
        for( int i = 0; i < vsize(); i++ )
            if( (*this)[i] != b[i] )
                return 0;
        return 1;
    }

    bool operator != ( const Quaterni &b ) const
    { return !( (*this) == b ); }

    Quaterni operator += ( const Quaterni &b )
    {
        for( int i = 0; i < vsize(); i++ )
            (*this)[i] += b[i];
        return Quaterni( *this );
    }

    Quaterni operator -= ( const Quaterni &b )
    {
        for( int i = 0; i < vsize(); i++ )
            (*this)[i] -= b[i];
        return Quaterni( *this );
    }

    Quaterni operator *= ( const DD &b )
    {
        for( int i = 0; i < vsize(); i++ )
            (*this)[i] *= b;
        return Quaterni( *this );
    }

    Quaterni operator /= ( const DD &b )
    {
        for( int i = 0; i < vsize(); i++ )
            (*this)[i] /= b;
        return Quaterni( *this );
    }

    Quaterni operator + ( const Quaterni &b ) const
    {
        Quaterni ret;
        for( int i = 0; i < vsize(); i++ )
            ret[i] = (*this)[i] + b[i];
        return ret;
    }

    Quaterni operator - ( const Quaterni &b ) const
    {
        Quaterni ret;
        for( int i = 0; i < vsize(); i++ )
            ret[i] = (*this)[i] - b[i];
        return ret;
    }

    Quaterni operator * ( const DD &b ) const
    {
        Quaterni ret;
        for( int i = 0; i < vsize(); i++ )
            ret[i] = (*this)[i] * b;
        return ret;
    }

    Quaterni operator / ( const DD &b ) const
    {
        Quaterni ret;
        for( int i = 0; i < vsize(); i++ )
            ret[i] = (*this)[i] / b;
        return ret;
    }

    Quaterni operator - () const
    {
        Quaterni ret;
        for( int i = 0; i < vsize(); i++ )
            ret[i] = -(*this)[i];
        return ret;
    }

    Quaterni operator * ( const Quaterni &b ) const
    { 
        Quaterni a( *this );
        DD nw = 0;
        if( a.w || b.w || (a / a.norm()) == ( b / b.norm()) )
            nw = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        DD nx = a.y * b.z - a.z * b.y + a.x * b.w + b.x * a.w;
        DD ny = a.z * b.x - a.x * b.z + a.y * b.w + b.y * a.w;
        DD nz = a.x * b.y - a.y * b.x + a.z * b.w + b.z * a.w;
        return Quaterni( nx, ny, nz, nw );
    }

    Quaterni operator / ( const Quaterni &b ) const
    { return (*this) * b.inv(); }

    DD operator ^ ( const Quaterni &b ) const
    {
        Quaterni a( *this );
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    /*
     *
     */
    Quaterni scalMlt ( const Quaterni &q ) const 
    {
        Quaterni p( *this );
        return ( p.cnj() * q + q.cnj() * p ) / 2.0;
    }

    Quaterni vectMlt ( const Quaterni &q ) const 
    {
        Quaterni p( *this );
        return ( p * q - q * p ) / 2.0;
    }

    Quaterni outMlt ( const Quaterni &q ) const 
    {
        Quaterni p( *this );
        return ( p.cnj() * q - q.cnj() * p ) / 2.0;
    }

    /*
     *
     */

    Quaterni cnj () const { return Quaterni( -x, -y, -z, w ); }
    Quaterni inv () const { return Quaterni( cnj() / norm() ); }

    DD norm () const { return x*x + y*y + z*z + w*w; }
    DD mag () const { return sqrt( norm() ); }

    Quaterni e () const
    {
        DD r = mag();
        return Quaterni( x / r, y / r, z / r, w / r );
    }

    static Quaterni rotAngle( DD alpha, const Quaterni &b )
    {
        DD arg = alpha / (DD)2;
        DD sn = sin( arg );
        Quaterni u( b.x, b.y, b.z, 0 );
        u = u.e();
        return Quaterni( u.x * sn, u.y * sn, u.z * sn, cos( arg ) );
    }

    static Quaterni rotAngle( DD alpha, DD x, DD y, DD z )
    { return Quaterni::rotAngle( alpha, Quaterni( x, y, z ) ); }

    static Quaterni rot2Vect( const Quaterni &a, const Quaterni &b )
    {
        Quaterni u = a * b;
        if( u.w != 0 ) return Quaterni( 0, 0, 0, 1 );
        return Quaterni::rotAngle( acos( (DD)(a.e() ^ b.e()) ), u );
    }

    Quaterni rot( const Quaterni &b ) const
    {
        Quaterni p = this->e();
        return p * b * p.cnj();
    }

    Quaterni unrot( const Quaterni &b ) const
    {
        Quaterni p = this->e();
        return p.cnj() * b * p;
    }
};

// ------------------------------------------------------------------
inline void printQuaterni( const Quaterni &q, const char *str = 0 )
{
    char format[128];
    const char *nmb = "% 6.5le";
    sprintf( format, "[ %s %s %s %s ] [%s]\n", nmb, nmb, nmb, nmb, "%s" );
    printf( format,
            (double)q.x, (double)q.y, (double)q.z, (double)q.w,
            str?str:"***no tag***" );
}
// ------------------------------------------------------------------

class MatrixQuat: public DataBuffer< Quaterni >
{
protected:
    static void swap( DD &a, DD &b )
    {
        DD buf = a;
        a = b;
        b = buf;
    }
public:
    Quaterni lx, ly, lz, lw;
    MatrixQuat():
        lx( 1, 0, 0, 0 ),
        ly( 0, 1, 0, 0 ),
        lz( 0, 0, 1, 0 ),
        lw( 0, 0, 0, 1 )
    {}

    MatrixQuat( Quaterni x, Quaterni y, Quaterni z, Quaterni w )
    {
        lx = x;
        ly = y; 
        lz = z; 
        lw = w;
    }

    MatrixQuat( const MatrixQuat &m )
    {
        lx = m.lx;
        ly = m.ly; 
        lz = m.lz; 
        lw = m.lw;
    }

    int vsize() const { return 4; }

    void set( const Quaterni &x, const Quaterni &y, const Quaterni &z )
    {
        for( int i = 0; i < vsize(); i++ )
        {
            (*this)[i][0] = x[i];
            (*this)[i][1] = y[i];
            (*this)[i][2] = z[i];
        }
    }

    Quaterni operator[] ( int i ) const
    {
        switch( i )
        {
            case 0: return lx; break;
            case 1: return ly; break;
            case 2: return lz; break;
            case 3: return lw; break;
            default:
                throw std::out_of_range( "DD Quaterni::operator[] const" );
                break;
        }
    }

    Quaterni& operator[] ( int i )
    {
        switch( i )
        {
            case 0: return lx; break;
            case 1: return ly; break;
            case 2: return lz; break;
            case 3: return lw; break;
            default:
                throw std::out_of_range( "DD& Quaterni::operator[]" );
                break;
        }
    }

    MatrixQuat operator = ( const MatrixQuat &m )
    {
        for( int i = 0; i < vsize(); i++ )
            (*this)[i] = m[i];
        return MatrixQuat( *this );
    }

    static MatrixQuat fromArrayF( float *v )
    {
        MatrixQuat ret;
        ret.lx = Quaterni( v[ 0], v[ 1], v[ 2], v[ 3] );
        ret.ly = Quaterni( v[ 4], v[ 5], v[ 6], v[ 7] );
        ret.lz = Quaterni( v[ 8], v[ 9], v[10], v[11] );
        ret.lw = Quaterni( v[12], v[13], v[14], v[15] );
        return ret;
    }
    
    static MatrixQuat fromArrayD( double *v )        
    {
        MatrixQuat ret;
        ret.lx = Quaterni( v[ 0], v[ 1], v[ 2], v[ 3] );
        ret.ly = Quaterni( v[ 4], v[ 5], v[ 6], v[ 7] );
        ret.lz = Quaterni( v[ 8], v[ 9], v[10], v[11] );
        ret.lw = Quaterni( v[12], v[13], v[14], v[15] );
        return ret;
    }

    static MatrixQuat fromArrayLD( long double *v )  
    {
        MatrixQuat ret;
        ret.lx = Quaterni( v[ 0], v[ 1], v[ 2], v[ 3] );
        ret.ly = Quaterni( v[ 4], v[ 5], v[ 6], v[ 7] );
        ret.lz = Quaterni( v[ 8], v[ 9], v[10], v[11] );
        ret.lw = Quaterni( v[12], v[13], v[14], v[15] );
        return ret;
    }

    static MatrixQuat fromQuatUpperZ( const Quaterni &x )
    {
        MatrixQuat ret;
        ret.lx = x.e();
        Quaterni gx = ret.lx;
        gx.z = 0;
        if( gx != Quaterni(0,0,0,0) )
        {
            DD up = ret.lx.z>0?1:-1;
            ret.ly = ( ret.lx.vectMlt(gx) ).e() * up;
            if( ret.ly.w == 0 )
                ret.lz = ( ret.lx.vectMlt( ret.ly ) ).e();
            else
            {
                ret.lz = Quaterni( 0, 0, 1, 0 );
                ret.ly = ( ret.lz.vectMlt(ret.lx) ).e();
            }
        }
        else
        {
            throw ExceptQuat( "bad x" );
        }
        // TODO: write variant else
        return ret;
    }

    Quaterni mlt( const Quaterni &n ) const
    { return Quaterni( lx^n, ly^n, lz^n, lw^n ); }

    static void print( const MatrixQuat &m, const char *str = 0 )
    {
        printf( "MatrixQuat [%s]\n", str?str:"no name" );
        printQuaterni( m.lx, "lx" );
        printQuaterni( m.ly, "ly" );
        printQuaterni( m.lz, "lz" );
        printQuaterni( m.lw, "lw" );
        printf( "\n" );
    }

    MatrixQuat inv() const
    {
        MatrixQuat buf( *this );
        swap( buf[0][1], buf[1][0] ); // 3 eq op
        swap( buf[0][2], buf[2][0] );
        swap( buf[1][2], buf[2][1] );

        DD v0 = buf[3][0] * buf[0][0] +\
                buf[3][1] * buf[1][0] +\
                buf[3][2] * buf[2][0];
        DD v1 = buf[3][0] * buf[0][1] +\
                buf[3][1] * buf[1][1] +\
                buf[3][2] * buf[2][1];
        DD v2 = buf[3][0] * buf[0][2] +\
                buf[3][1] * buf[1][2] +\
                buf[3][2] * buf[2][2];


        DD k = -1.0 / buf[3][3];

        buf[3][0] = v0 * k; 
        buf[3][1] = v1 * k;
        buf[3][2] = v2 * k;

           v0 = buf[0][0] * buf[0][3] +\
                buf[0][1] * buf[1][3] +\
                buf[0][2] * buf[2][3];
           v1 = buf[1][0] * buf[0][3] +\
                buf[1][1] * buf[1][3] +\
                buf[1][2] * buf[2][3];
           v2 = buf[2][0] * buf[0][3] +\
                buf[2][1] * buf[1][3] +\
                buf[2][2] * buf[2][3];

        buf[0][3] = v0 * k;
        buf[1][3] = v1 * k;
        buf[2][3] = v2 * k;

        buf[3][3] = -k * ( 1 - ( buf[0][3] * buf[3][0] + 
                                 buf[1][3] * buf[3][1] + 
                                 buf[2][3] * buf[3][2] ) );

        return buf;
    }

    MatrixQuat T() const
    {
        return MatrixQuat( Quaterni( lx.x, ly.x, lz.x, lw.x ),
                           Quaterni( lx.y, ly.y, lz.y, lw.y ),
                           Quaterni( lx.z, ly.z, lz.z, lw.z ),
                           Quaterni( lx.w, ly.w, lz.w, lw.w ) );
    }

    bool operator == ( const MatrixQuat &b ) const
    {
        for( int i = 0; i < vsize(); i++ )
            if( (*this)[i] != b[i] )
                return 0;
        return 1;
    }

    bool operator != ( const MatrixQuat &b ) const
    { return !( (*this) == b ); }

};

inline void printMatrix( const MatrixQuat &m, const char *str = 0 )
{
    printf( "MatrixQuat [%s]\n", str?str:"no name" );
    printQuaterni( m.lx, "lx" );
    printQuaterni( m.ly, "ly" );
    printQuaterni( m.lz, "lz" );
    printQuaterni( m.lw, "lw" );
    printf( "\n" );
}

#endif//QUATERNI_H
