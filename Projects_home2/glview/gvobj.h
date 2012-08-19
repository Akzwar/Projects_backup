#ifndef GVOBJ_H
#define GVOBJ_H

#include <GL/freeglut.h>
#include "../utils/gcmdata.h"

/* 
class SObject
{
protected:
    const SObject *parent;
    matrix4d pivot;
public:
    explicit SObject( const SObject *Parent=0 ):
        parent(Parent)
    {}

    void setParent( const SObject *Parent )
    {
        parent = Parent;
    }

    matrix4d localToParent() const
    {
        return pivot;
    }

    matrix4d localToGlobal() const
    {
        if( parent )
            return parent->localToGlobal() * pivot;
        return pivot;
    }

    matrix4d parentToLocal() const
    {
        return matrixInv( pivot );
    }

    matrix4d globalToLocal() const
    {
        if( parent )
            return matrixInv( pivot ) * parent->globalToLocal();
        return matrixInv( pivot );
    }

    point3d selfPosInParent() const 
    { return point3d( pivot.lw.x, pivot.lw.y, pivot.lw.z ); }

    point3d selfPosInGlobal() const
    {
        if( parent )
            //return parent->localToGlobal() * selfPosInParent();
            return parent->globalToLocal() * selfPosInParent();
        return selfPosInParent();
    }

    void setPosInParent( const point3d &pos )
    {
        pivot.lw = point4d( pos );
        pivot.lw.w = 1;
    }

    void setRotInParent( const point4d &q )
    {
        matrix4d n = matrix4d::fromQuaterni( q );
        n.lw = pivot.lw;
        pivot = n;
    }
};
*/

class GPos
{
protected:
    inline virtual point3d transform( const point3d &p ) const { return p; }
public:
    virtual point3d pos() const = 0;
};

class GObject: public GPos
{
protected:
    virtual void setTransform() {}
public:
    virtual void draw() = 0;
    virtual point3d getMin() const { return this->pos(); }
    virtual point3d getMax() const { return this->pos(); }
};

#endif//GVOBJ_H
