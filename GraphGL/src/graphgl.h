#include"../../utils/glinclude.h"
#include <math.h>
using namespace std;

template<class DD>
class GraphGL
{
 protected:
    DD** data;
    int X, Y, Width, Height;
    int DotCount;
 public:
    GraphGL() : X(0), Y(0), Width(100), Height(100), DotCount(0)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
    }
    GraphGL(int _X, int _Y, int _Width = 100, int _Height = 100)
        : X(_X), Y(_Y), Width(_Width), Height(_Height), DotCount(0){}
    virtual ~GraphGL()
    {
        for( int i = 0; i < DotCount; i++ )
            delete [] data[i];
        delete [] data;
    }
    virtual void Draw() = 0;
};

template<class DD>
class GraphGL2D : public GraphGL<DD>
{
 private:
    bool hasUnderZeroX,hasUnderZeroY;
    double maxX, maxY, minX, minY;
    DD fmaxX()
    {
        if( this->DotCount == 0 )
            return 0;
        DD retVal = fabs(this->data[0][0]);
        for( int i = 1; i < this->DotCount; i++ )
            if( fabs(this->data[i][0])> retVal )
                retVal = fabs(this->data[i][0]);
        return retVal - minX;
    }
    DD fmaxY()
    {
        if( this->DotCount == 0 )
            return 0;
        DD retVal = fabs(this->data[0][1]);
        for( int i = 1; i < this->DotCount; i++ )
            if( fabs(this->data[i][1]) > retVal )
                retVal = fabs(this->data[i][1]);
        return retVal - minY;
    }
    DD fminX()
    {
        if( this->DotCount == 0 )
            return 0;
        DD retVal = this->data[0][0];
        for( int i = 1; i < this->DotCount; i++ )
            if( this->data[i][0] < retVal )
                retVal = this->data[i][0];
        return retVal;
    }
    DD fminY()
    {
        if(this->DotCount == 0 )
            return 0;
        DD retVal = this->data[0][1];
        for( int i = 1; i < this->DotCount; i++ )
            if( this->data[i][1] < retVal )
                retVal = this->data[i][1];
        return retVal;
    }
 
 public:
    GraphGL2D() : GraphGL<DD>(){}
    GraphGL2D(int _X, int _Y, int _Width, int _Height) 
        : GraphGL<DD>(_X, _Y, _Width, _Height), hasUnderZeroX(false), hasUnderZeroY(false){}
    GraphGL2D(int _X, int _Y) : GraphGL<DD>(_X, _Y), hasUnderZeroX(false), hasUnderZeroY(false){}
    ~GraphGL2D(){}
    virtual void AddDot( DD _X, DD _Y )
    {
        DD realX, realY;
        realX = _X;
        realY = _Y;
        if( _X < 0 )
            hasUnderZeroX = true;
        if( _Y < 0 )
            hasUnderZeroY = true;
        if(this->DotCount)
        {
            DD** old_data;
            old_data = new DD*[this->DotCount];
            for( int i = 0; i < this->DotCount; i++ )
            {
                old_data[i] = new DD[2];
                for( int j = 0; j < 2; j++ )
                    old_data[i][j] = this->data[i][j];
                delete [] this->data[i];
            }
            delete [] this->data;
            this->DotCount++;
            this->data = new DD*[this->DotCount];
            for( int i = 0; i < this->DotCount - 1; i++ )
            {
                this->data[i] = new DD[2];
                for( int j = 0; j < 2; j++ )
                    this->data[i][j] = old_data[i][j];
                delete [] old_data[i];
            }
            delete [] old_data;
            this->data[this->DotCount-1] = new DD[2];
            this->data[this->DotCount-1][0] = realX ;
            this->data[this->DotCount-1][1] = realY ;
        }
        else
        {
            this->DotCount++;
            this->data = new DD*[this->DotCount];
            this->data[0] = new DD[2];
            this->data[0][0] = realX ;
            this->data[0][1] = realY ;
        }
        minX = fminX();
        minY = fminY();
        maxX = fmaxX();
        maxY = fmaxY();
    }
    virtual void Draw()
    {
        char Text[20];
        glLineWidth(1);
        glColor3f(0.9,0.9,0.9);

        if( this->DotCount )
        {
            sprintf(Text,"%f,%f",this->data[this->DotCount-1][0], this->data[this->DotCount-1][1]);
            TextOut( this->X + (this->data[this->DotCount-1][0]-minX)/maxX * this->Width,
                     this->Y + (this->data[this->DotCount-1][1]-minY)/maxY * this->Height,Text);
        }
        glColor3d(1.0,0.0,0.0);
        glBegin(GL_LINE_STRIP);
            for( int i = 0; i < this->DotCount; i++ )
            {
                glVertex2f(this->X + (this->data[i][0]-minX)/maxX * this->Width,
                           this->Y + (this->data[i][1]-minY)/maxY * this->Height);
            }
        glEnd();
        glLineWidth(1);
        glColor3f(0.9,0.9,0.9);
        glBegin(GL_LINE_STRIP);
            glVertex2f(this->X, this->Y - minY/maxY*this->Height);
            glVertex2f(this->X + this->Width, this->Y - minY/maxY*this->Height);
        glEnd();
        glBegin(GL_LINE_STRIP);
            glVertex2f(this->X - minX/maxX*this->Width, this->Y);
            glVertex2f(this->X - minX/maxX*this->Width, this->Y + this->Height);
        glEnd();

    }
};

template<class DD>
class GraphGL3D : public GraphGL<DD>
{
 public: 
    GraphGL3D() : GraphGL<DD>(){}
    GraphGL3D(int _X, int _Y, int _Width, int _Height) : GraphGL<DD>(_X, _Y, _Width, _Height){}
    GraphGL3D(int _X, int _Y) : GraphGL<DD>(_X, _Y){}
    virtual void AddDot( DD _X, DD _Y, DD _Z )
    {
        DD** old_data;

        old_data = new DD*[this->DotCount];
        for( int i = 0; i <this->DotCount; i++ )
        {
            old_data[i] = new DD[3];
            for( int j = 0; j < 3; j++ )
                old_data[i][j] = this->data[i][j];
            delete [] this->data[i];
        }
        delete [] this->data;
        this-> DotCount++;
        this->data = new DD*[this->DotCount];
        for( int i = 0; i < this->DotCount - 1; i++ )
        {
            this->data[i] = new DD[3];
            for( int j = 0; j < 3; j++ )
                this->data[i][j] = old_data[i][j];
            delete [] old_data[i];
        }
        delete [] old_data;
        this->data[this->DotCount-1][0] = _X;
        this->data[this->DotCount-1][1] = _Y;
        this->data[this->DotCount-1][2] = _Z;
    }
};
