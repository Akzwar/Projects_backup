#ifndef GVCAMERA_H
#define GVCAMERA_H

#include "gvobj.h"
#include "../utils/helpers.h"

class GCamera: public GPos
{
public:
    point3d posPnt;
    point3d trgPnt;
    point3d upPnt;

    double fov;
    double near;
    double far;
    double arat;
    double persp;

    GPos *posObj;
    GPos *trgObj;
    GPos *upObj;

    GCamera( point3d Pos=point3d(2,0,0),
             point3d Trg=point3d(0,0,0),
             point3d Up =point3d(0,0,1),
             double Fov=50.0,
             double Near=0.1,
             double Far=1.0e4,
             double ARat=1.0,
             bool Persp=true ):
        posPnt( Pos ),
        trgPnt( Trg ),
        upPnt( Up ),
        fov( Fov ),
        near( Near ),
        far( Far ),
        arat( ARat ),
        persp( Persp ),
        posObj(0),
        trgObj(0),
        upObj(0)
    {}

    void view() const
    {
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        point3d p = this->transform( posObj ? posObj->pos() : posPnt );
        point3d t = this->transform( trgObj ? trgObj->pos() : trgPnt );
        point3d u = this->transform( upObj ? upObj->pos() : upPnt );

        gluLookAt( p.x, p.y, p.z,
                   t.x, t.y, t.z,
                   u.x, u.y, u.z );

        if( persp )
        {
            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();
            gluPerspective( fov, arat, near, far );
            glMatrixMode( GL_MODELVIEW );
        }
    }

    point3d pos() const { return posObj?posObj->pos():posPnt; }

};

class GOrbitCam: protected GCamera
{
protected:
    double dst;
    double resetdst;
    double yaw;
    double tng;
public:
    GOrbitCam( point3d Trg=point3d(),
               double Dst=2.0,
               double Yaw=0.0,
               double Tng=0.0,
               double Fov=50.0,
               double Near=0.1,
               double Far=1.0e4,
               double ARat=1.0,
               bool Persp=true ):
        GCamera( Trg+point3d::fromSpherical( Dst, Yaw, Tng ),
                 Trg, point3d(0,0,1),
                 Fov, Near, Far, ARat, Persp ),
        dst( Dst ),
        tng( Tng ),
        yaw( Yaw )
    {}

    void setTrg( const point3d &trg, bool reset_obj=1 )
    {
        if( reset_obj ) this->trgObj = 0;
        this->trgPnt = trg;
        recalcPos();
    }

    void setTrg( GPos *obj ) { this->trgObj = obj; }

    void setOrbit( double Dst, double Yaw, double Tng )
    {
        dst = fabs( Dst );
        yaw = Yaw;
        tng = Tng;
        //fmod<double>( yaw, M_PI, 1 );
        fmod<double>( yaw, M_PI * 2.0f );
        double maxtng = M_PI_4 * 1.999;
        norm<double>( tng, -maxtng, maxtng );
        recalcPos();
    }

    void setDst( double Dst, bool asreset=0 )
    {
        dst = fabs( Dst );
        if( asreset ) resetdst = dst;
        recalcPos();
    }

    void resetDst() { dst = resetdst; recalcPos(); }

    point3d getOrbit() const { return point3d( dst, yaw, tng ); }

    void setCam( double Fov, double Near, double Far, double ARat, bool Persp )
    {
        this->fov = Fov;
        this->near = Near;
        this->far = Far;
        this->arat = ARat;
        this->persp = Persp;
    }

    void setFov( double Fov ) { this->fov = Fov; }
    double getFov() const { return this->fov; }

    void setNear( double Near ) { this->near = Near; }
    double getNear() const { return this->near; }

    void setFar( double Far ) { this->far = Far; }
    double getFar() const { return this->far; }

    void setARatio( double ARat ) { this->arat = ARat; }
    double getARatio() const { return this->arat; }

    void setPerspective( bool Persp ) { this->persp = Persp; }
    bool getPerspective() const { return this->persp; }

    point3d pos() const { return posPnt; }

    void view() const { ((GCamera*)(this))->view(); }

    void recalcPos()
    {
        point3d t = trgObj?trgObj->pos():trgPnt;
        posPnt = t + point3d::fromSpherical( dst, yaw, tng );
    }
};

class GOrbitCamM: public GOrbitCam
{
protected:
    int oldx, oldy;
public:
    GOrbitCamM( point3d Trg=point3d(),
                double Dst=5.0,
                double Yaw=0.0,
                double Tng=0.0,
                double Fov=50.0,
                double Near=0.1,
                double Far=1.0e4,
                double ARat=1.0,
                bool Persp=true ):
        GOrbitCam( Trg, Dst, Yaw, Tng, Fov, Near, Far, ARat, Persp )
    {}

    void setOldMPos( int x, int y ) { oldx = x; oldy = y; }

    void rotation( int x, int y, double sx, double sy )
    {
        double dx = x - oldx;
        double dy = y - oldy;

        double nyaw = this->yaw - dx * sx;
        double ntng = this->tng + dy * sy;
        this->setOrbit( this->dst, nyaw, ntng );

        setOldMPos( x, y );
        this->recalcPos();
    }

    void move( double delta )
    {
        this->dst += delta * this->dst;
        norm<double>( this->dst, this->near * 2.0, this->far / 2.0 );
        this->recalcPos();
    }

    void zoom( double delta )
    {
        this->fov += delta;
        norm<double>( this->fov, 1, 179 );
    }

};

#endif//GVCAMERA_H
