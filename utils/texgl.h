#include <vector>
#include <GL/gl.h>

struct tex_info
{
    int Depth,Width,Height;
};

class pic_tex
{
 protected:
    std::vector<GLubyte> data;
 public:
    tex_info info;
    pic_tex(){}
    virtual int LoadfromFile( char* fname ){}
    void* ptr(){ return &data[0]; }    
};

class tga_tex : public pic_tex
{
 public:
    tga_tex() : pic_tex(){}
    virtual int LoadfromFile( char* fname )
    {
        FILE *fp = NULL;
        data.clear();
        data.resize(0);
        fp = fopen( fname, "rb" );
        if( !fp )
            return 1;
        unsigned char inf;
        unsigned int size;
        fread( &inf, 1, 1, fp );
        char IdentLength = inf;
        fread( &inf, 1, 4, fp );
        fread( &size, 2, 1, fp );
        int CMLength = size;
        fread( &inf, 1, 5, fp );
        fread( &size, 2, 1, fp );
        info.Width = size;
        fread( &size, 2, 1, fp );
        info.Height = size;
        fread( &inf, 1, 1, fp );
        info.Depth = inf;
        fread( &inf, 1, IdentLength + CMLength, fp );
        printf("CM=%d\n",info.Width);
        char R,G,B,A;
        for( int i = 0; i < info.Width * info.Height * 4 ; i+=4 )
        {
            fread( &inf, 1, 1, fp );
            R = inf;
            fread( &inf, 1, 1, fp );
            G = inf;
            fread( &inf, 1, 1, fp );
            B = inf;
            A = 255;
            if( R == B && B == G )
                A = 0;
            data.push_back(B);
            data.push_back(G);
            data.push_back(R);
            data.push_back(A);
        }
        fclose(fp);
        return 0;
    }
};
