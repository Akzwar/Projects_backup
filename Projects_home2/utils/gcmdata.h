#ifndef GCMDATA_H
#define GCMDATA_H

#include <cmath>

/**
 * @file gcmdata.h
 * Definition of compact data structs and functions
 */

#if defined(CMDATA_FLOAT)
typedef float DT;
#elif defined(CMDATA_LONG_DOUBLE)
typedef long double DT;
#else
typedef double DT;
#endif

struct point2d 
{ 
    DT x, y; 
    point2d( DT _x=0, DT _y=0 ): x(_x), y(_y) {}
};

struct point3d: public point2d
{  
    DT z; 
    point3d( DT _x=0, DT _y=0, DT _z=0 ): point2d( _x, _y ), z(_z) {}
    point3d( const point2d &p ): point2d(p), z(0) {}
    static point3d fromSpherical( DT dst, DT yaw, DT tng )
    {
        point3d ret;
        ret.x = cos(tng) * cos(yaw) * dst;
        ret.y = cos(tng) * sin(yaw) * dst;
        ret.z = sin(tng) * dst;
        return ret;
    }
    static point3d fromSpherical( const point3d &sph )
    {
        return point3d::fromSpherical( sph.x, sph.y, sph.z );
    }
    static point3d toSpherical( const point3d &pnt )
    {
        point3d ret;
        ret.x = pnt.x * pnt.x + pnt.y * pnt.y + pnt.z * pnt.z;
        ret.y = atan2( pnt.y, pnt.x );
        ret.z = asin( pnt.z / ret.x );
        return ret;
    }
};

struct point3dtex1d: public point3d 
{ 
    DT s; 
    point3dtex1d( const point3d &p ): point3d( p ), s( 0 ) {}
    point3dtex1d( DT _x=0, DT _y=0, DT _z=0, DT _s=0 ): point3d( _x, _y, _z ), s(_s) {}
};

struct point3dtex2d: public point3dtex1d { 
    DT t; 
    point3dtex2d( const point3d &p ):
        point3dtex1d( p ), t(0)
    {}
    point3dtex2d( DT _x=0, DT _y=0, DT _z=0, DT _s=0, DT _t=0 ):
        point3dtex1d( _x, _y, _z, _s), t(_t)
    {}
};
//struct point3dtex3d: public point3dtex2d { DT r; };
// struct point3dtex4d: public point3dtex3d { DT q; }; // animate texture o_O

struct point4d: public point3d 
{ 
    DT w; 
    point4d( DT _x=0, DT _y=0, DT _z=0, DT _w=0 ):
        point3d( _x, _y, _z ), w(_w)
    {}
    point4d( const point3d &p ):
        point3d(p), w(0)
    {}
};

struct color3d 
{
    DT r, g, b; 
    color3d( DT _r=0, DT _g=0, DT _b=0 ):
        r(_r), g(_g), b(_b)
    {}
};

struct color4d: public color3d 
{ 
    DT a; 
    color4d( DT _r=0, DT _g=0, DT _b=0, DT _a=0 ):
        color3d( _r, _g, _b ), a(_a)
    {}
    color4d( const color3d &c ):
        color3d(c), a(0)
    {}
};

/* point2d<TP> *********************************************/
inline point2d operator + ( const point2d &a, const point2d &b )
{
    point2d ret;
    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    return ret;
}

inline point2d operator - ( const point2d &a, const point2d &b )
{
    point2d ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    return ret;
}

inline point2d operator * ( const point2d &a, const DT &b )
{
    point2d ret;
    ret.x = a.x * b;
    ret.y = a.y * b;
    return ret;
}

inline point2d operator / ( const point2d &a, const DT &b )
{
    point2d ret;
    ret.x = a.x / b;
    ret.y = a.y / b;
    return ret;
}

/* point3d *********************************************/
inline point3d operator + ( const point3d &a, const point3d &b )
{
    point3d ret;
    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    ret.z = a.z + b.z;
    return ret;
}

inline point3d operator - ( const point3d &a, const point3d &b )
{
    point3d ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    ret.z = a.z - b.z;
    return ret;
}

inline point3d operator * ( const point3d &a, const DT &b )
{
    point3d ret;
    ret.x = a.x * b;
    ret.y = a.y * b;
    ret.z = a.z * b;
    return ret;
}

inline point3d operator / ( const point3d &a, const DT &b )
{
    point3d ret;
    ret.x = a.x / b;
    ret.y = a.y / b;
    ret.z = a.z / b;
    return ret;
}

inline point3d operator += ( point3d &a, const point3d &b )
{
    point3d ret;
    ret.x = a.x += b.x;
    ret.y = a.y += b.y;
    ret.z = a.z += b.z;
    return ret;
}

inline point3d operator -= ( point3d &a, const point3d &b )
{
    point3d ret;
    ret.x = a.x -= b.x;
    ret.y = a.y -= b.y;
    ret.z = a.z -= b.z;
    return ret;
}

inline point3d operator *= ( point3d &a, const DT &b )
{
    point3d ret;
    ret.x = a.x *= b;
    ret.y = a.y *= b;
    ret.z = a.z *= b;
    return ret;
}

inline point3d operator /= ( point3d &a, const DT &b )
{
    point3d ret;
    ret.x = a.x /= b;
    ret.y = a.y /= b;
    ret.z = a.z /= b;
    return ret;
}

inline DT operator ^ ( const point3d &a, const point3d &b )
{ return a.x * b.x + a.y * b.y + a.z * b.z; }

inline DT sqrMagPoint3d( const point3d &a )
{ return a ^ a; }

inline DT magPoint3d( const point3d &a ) 
{ return sqrt( sqrMagPoint3d( a ) ); }

inline DT distPoint3d( const point3d &a, const point3d &b )
{ return magPoint3d( b - a ); }

/* point4d *********************************************/
inline point4d operator + ( const point4d &a, const point4d &b )
{
    point4d ret;
    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    ret.z = a.z + b.z;
    ret.w = a.w + b.w;
    return ret;
}

inline point4d operator - ( const point4d &a, const point4d &b )
{
    point4d ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    ret.z = a.z - b.z;
    ret.w = a.w - b.w;
    return ret;
}

inline point4d operator * ( const point4d &a, const DT &b )
{
    point4d ret;
    ret.x = a.x * b;
    ret.y = a.y * b;
    ret.z = a.z * b;
    ret.w = a.w * b;
    return ret;
}

inline point4d operator / ( const point4d &a, const DT &b )
{ 
    point4d ret;
    ret.x = a.x / b;
    ret.y = a.y / b;
    ret.z = a.z / b;
    ret.w = a.w / b;
    return ret;
}

inline DT operator ^ ( const point4d &a, const point4d &b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

/* color3d *********************************************/
inline color3d operator + ( const color3d &a, const color3d &b )
{
    color3d ret;
    ret.r = a.r + b.r;
    ret.g = a.g + b.g;
    ret.b = a.b + b.b;
    return ret;
}

inline color3d operator - ( const color3d &a, const color3d &b )
{
    color3d ret;
    ret.r = a.r - b.r;
    ret.g = a.g - b.g;
    ret.b = a.b - b.b;
    return ret;
}

inline color3d operator * ( const color3d &a, const DT &b )
{
    color3d ret;
    ret.r = a.r * b;
    ret.g = a.g * b;
    ret.b = a.b * b;
    return ret;
}

inline color3d operator / ( const color3d &a, const DT &b )
{
    color3d ret;
    ret.r = a.r / b;
    ret.g = a.g / b;
    ret.b = a.b / b;
    return ret;
}

/* color4d *********************************************/
inline color4d operator + ( const color4d &a, const color4d &b )
{
    color4d ret;
    ret.r = a.r + b.r;
    ret.g = a.g + b.g;
    ret.b = a.b + b.b;
    ret.a = a.a + b.a;
    return ret;
}

inline color4d operator - ( const color4d &a, const color4d &b )
{
    color4d ret;
    ret.r = a.r - b.r;
    ret.g = a.g - b.g;
    ret.b = a.b - b.b;
    ret.a = a.a - b.a;
    return ret;
}

inline color4d operator * ( const color4d &a, const DT &b )
{
    color4d ret;
    ret.r = a.r * b;
    ret.g = a.g * b;
    ret.b = a.b * b;
    ret.a = a.a * b;
    return ret;
}

inline color4d operator / ( const color4d &a, const DT &b )
{
    color4d ret;
    ret.r = a.r / b;
    ret.g = a.g / b;
    ret.b = a.b / b;
    ret.a = a.a / b;
    return ret;
}

struct matrix4d
{
    point4d lx, ly, lz, lw;
    matrix4d( point4d _lx, point4d _ly, point4d _lz, point4d _lw ):
        lx(_lx), ly(_ly), lz(_lz), lw(_lw)
    {}
    matrix4d( DT lxx=1, DT lxy=0, DT lxz=0, DT lxw=0,
              DT lyx=0, DT lyy=1, DT lyz=0, DT lyw=0,
              DT lzx=0, DT lzy=0, DT lzz=1, DT lzw=0,
              DT lwx=0, DT lwy=0, DT lwz=0, DT lww=1 ):
        lx( lxx, lxy, lxz, lxw ),
        ly( lyx, lyy, lyz, lyw ),
        lz( lzx, lzy, lzz, lzw ),
        lw( lwx, lwy, lwz, lww )
    {}  
    matrix4d( DT *v ):
        lx( v[ 0], v[ 1], v[ 2], v[ 3] ),
        ly( v[ 4], v[ 5], v[ 6], v[ 7] ),
        lz( v[ 8], v[ 9], v[10], v[11] ),
        lw( v[12], v[13], v[14], v[15] )
    {}
    static matrix4d fromGLMatrix( double *v )
    {
        matrix4d ret( v[ 0], v[ 4], v[ 8], v[12],
                      v[ 1], v[ 5], v[ 9], v[13],
                      v[ 2], v[ 6], v[10], v[14],
                      v[ 3], v[ 7], v[11], v[15] );
        return ret;
    }

    static matrix4d fromQuaterni( const point4d &quat )
    {
        DT wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
        x2 = quat.x + quat.x;
        y2 = quat.y + quat.y;
        z2 = quat.z + quat.z;
        xx = quat.x * x2;   xy = quat.x * y2;   xz = quat.x * z2;
        yy = quat.y * y2;   yz = quat.y * z2;   zz = quat.z * z2;
        wx = quat.w * x2;   wy = quat.w * y2;   wz = quat.w * z2;

        matrix4d m;
        /*
        m[0][0]=1.0f-(yy+zz); m[0][1]=xy-wz;        m[0][2]=xz+wy;
        m[1][0]=xy+wz;        m[1][1]=1.0f-(xx+zz); m[1][2]=yz-wx;
        m[2][0]=xz-wy;        m[2][1]=yz+wx;        m[2][2]=1.0f-(xx+yy);

        m[0][3] = m[1][3] = m[2][3] = 0;
        m[3][0] = m[3][1] = m[3][2] = 0;
        m[3][3] = 1;
        */
        m.lx.x = 1.0 - ( yy + zz );
        m.lx.y = xy - wz;
        m.lx.z = xz + wy;

        m.ly.x = xy + wz;
        m.ly.y = 1.0 - ( xx + zz );
        m.ly.z = yz - wx;

        m.lz.x = xz - wy;
        m.lz.x = yz + wx;
        m.lz.x = 1.0 - ( xx + yy );

        return m;
    }
};

inline point3d operator * ( const matrix4d &m, const point3d &p )
{
    point4d v(p);
    v.w = 1;
    return point3d( m.lx ^ v, m.ly ^ v, m.lz ^ v );
}

inline void matrixToGL( double *glmtr, const matrix4d &mtr )
{
    /*
    glmtr[ 0] = mtr.lx.x; glmtr[ 4] = mtr.lx.y; glmtr[ 8] = mtr.lx.z; glmtr[12] = mtr.lx.w;
    glmtr[ 1] = mtr.ly.x; glmtr[ 5] = mtr.ly.y; glmtr[ 9] = mtr.ly.z; glmtr[13] = mtr.ly.w;
    glmtr[ 2] = mtr.lz.x; glmtr[ 6] = mtr.lz.y; glmtr[10] = mtr.lz.z; glmtr[14] = mtr.lz.w;
    glmtr[ 3] = mtr.lw.x; glmtr[ 7] = mtr.lw.y; glmtr[11] = mtr.lw.z; glmtr[15] = mtr.lw.w;
    */
    glmtr[ 0] = mtr.lx.x; glmtr[ 1] = mtr.lx.y; glmtr[ 2] = mtr.lx.z; glmtr[ 3] = mtr.lx.w;
    glmtr[ 4] = mtr.ly.x; glmtr[ 5] = mtr.ly.y; glmtr[ 6] = mtr.ly.z; glmtr[ 7] = mtr.ly.w;
    glmtr[ 8] = mtr.lz.x; glmtr[ 9] = mtr.lz.y; glmtr[10] = mtr.lz.z; glmtr[11] = mtr.lz.w;
    glmtr[12] = mtr.lw.x; glmtr[13] = mtr.lw.y; glmtr[14] = mtr.lw.z; glmtr[15] = mtr.lw.w;
}

inline matrix4d matrixToGL( const matrix4d &p )
{
    return matrix4d::fromGLMatrix( (double*)( (void*)&p ) );
}

template <class Type>
inline void swap( Type &a, Type &b )
{
    Type buf = a;
    a = b;
    b = buf;
}

inline matrix4d matrixTrans( const matrix4d &p )
{
    matrix4d ret( p.lx.x, p.ly.x, p.lz.x, p.lw.x,
                  p.lx.y, p.ly.y, p.lz.y, p.lw.y,
                  p.lx.z, p.ly.z, p.lz.z, p.lw.z,
                  p.lx.w, p.ly.w, p.lz.w, p.lw.w );
    return ret;
}

inline matrix4d operator * ( const matrix4d &m, const matrix4d &p )
{
    matrix4d t = matrixTrans( p );
    matrix4d ret;
    ret.lx = point4d( m.lx ^ t.lx, m.ly ^ t.lx, m.lz ^ t.lx, m.lw ^ t.lx );
    ret.ly = point4d( m.lx ^ t.ly, m.ly ^ t.ly, m.lz ^ t.ly, m.lw ^ t.ly );
    ret.lz = point4d( m.lx ^ t.lz, m.ly ^ t.lz, m.lz ^ t.lz, m.lw ^ t.lz );
    ret.lw = point4d( m.lx ^ t.lw, m.ly ^ t.lw, m.lz ^ t.lw, m.lw ^ t.lw );
    return matrixTrans(ret);
}

inline matrix4d matrixInv( const matrix4d &m )
{
    matrix4d mbuf(m);
    DT *buf = (DT*)( (void*)&mbuf );

    swap<DT>( buf[0*4+1], buf[1*4+0] ); // 3 eq op
    swap<DT>( buf[0*4+2], buf[2*4+0] );
    swap<DT>( buf[1*4+2], buf[2*4+1] );

    DT v0 = buf[3*4+0] * buf[0*4+0] +\
            buf[3*4+1] * buf[1*4+0] +\
            buf[3*4+2] * buf[2*4+0];
    DT v1 = buf[3*4+0] * buf[0*4+1] +\
            buf[3*4+1] * buf[1*4+1] +\
            buf[3*4+2] * buf[2*4+1];
    DT v2 = buf[3*4+0] * buf[0*4+2] +\
            buf[3*4+1] * buf[1*4+2] +\
            buf[3*4+2] * buf[2*4+2];


    DT k = -1.0 / buf[15];

    buf[3*4+0] = v0 * k; 
    buf[3*4+1] = v1 * k;
    buf[3*4+2] = v2 * k;

        v0 = buf[0*4+0] * buf[0*4+3] +\
             buf[0*4+1] * buf[1*4+3] +\
             buf[0*4+2] * buf[2*4+3];
        v1 = buf[1*4+0] * buf[0*4+3] +\
             buf[1*4+1] * buf[1*4+3] +\
             buf[1*4+2] * buf[2*4+3];
        v2 = buf[2*4+0] * buf[0*4+3] +\
             buf[2*4+1] * buf[1*4+3] +\
             buf[2*4+2] * buf[2*4+3];

    buf[0*4+3] = v0 * k;
    buf[1*4+3] = v1 * k;
    buf[2*4+3] = v2 * k;

    buf[15] = -k * ( 1 - ( buf[0*4+3] * buf[3*4+0] + 
                           buf[1*4+3] * buf[3*4+1] + 
                           buf[2*4+3] * buf[3*4+2] ) );

    return mbuf;
}



#endif//GCMDATA_H
