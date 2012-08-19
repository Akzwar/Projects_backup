#include <GL/GL.h>
#include <GL/glut.h>
#include <math.h>
#include <vector>
#pragma once
using namespace std;

struct dot2d
{
	long double X;
	long double Y;
};

struct Color
{
	unsigned char R;
	unsigned char G;	
	unsigned char B;
}

class Graph2d
{
 private:
	int Width, Height;
	vector<dot2d> data;
	Color LineColor;
	Color AxisColor;
 public:
	Graph2d( int W, int H )
	{
		Width = W;
		Height = W;
		LineColor = {200,10,10};
		AxisColor = {10,200,200};
	}
	void AddXY( int X, int Y )
	{
		data.push_back({X,Y});
	}
	
};

