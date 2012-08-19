#include "testApp.h"
using namespace std;
//--------------------------------------------------------------
long double minld(long double a, long double b)
{
    if(a<b)
        return a;
    else
        return b;
}

long double maxld(long double a, long double b)
{
    if(a>b)
        return a;
    else
        return b;
}

testApp::testApp()
{

    Model M;

    Rung = Integrator( M, 0.0,5*10e-6, 10e-9, 10e-9);
                                                         
}


DrawPoint prev;
void testApp::setup(){
    ofBackground(0, 0, 20);
    prev.x = 0;
    prev.y = 0;

}

//--------------------------------------------------------------
#define COUNT 10000
double delta = 0.01;
DrawPoint Va[COUNT];
void testApp::update(){

    if(Rung.getT() < Rung.tk)
    	Rung.SmallStep();
  /*  {
        for( int i=0; i < COUNT; i++ )
        {
            Rung.NextStep();
            Va[i].x = Rung.CurrVect.Y1;
            Va[i].y = Rung.CurrVect.Y2;
        }
    }
    //prev.x = Va[0].x;
    //prev.y = Va[0].y;
    for( int i = 0; i < COUNT; i++ )
    {
        if ((abs(prev.x - Va[i].x) > delta) || (abs(prev.y - Va[i].y) > delta))
        {
            prev.x = Va[i].x;
            prev.y = Va[i].y;
            Result.push_back(Va[i]);
        }
    }
    
   if(Rung.getT() < Rung.tk){
        prev.x = Va[0].x;
        prev.y = Va[0].y;
    for(int i=0;i<COUNT;i++)
    {
        Rung.NextStep();
        Va[i].x = Rung.CurrVect.Y1;
        Va[i].y = Rung.CurrVect.Y2;
        if ((abs(prev.x - Va[i].x) > delta) || (abs(prev.y - Va[i].y) > delta)){
            prev.x = Va[i].x;
            prev.y = Va[i].y;
            Result.push_back(Va[i]);
        }
    }
   }
   */
}

//--------------------------------------------------------------

void testApp::draw(){
   // ofClear(0,0,20);
	glColor3f(0.0,0.0,1.0);
	glBegin(GL_LINE_STRIP);
		glVertex2f(100,525);
		glVertex2f(1000,525);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex2f(100,100);
		glVertex2f(100,1000);
	glEnd();
        glColor3f(1.0,1.0,1.0);
        glBegin(GL_POINTS);
            glVertex2f(100+(Rung.getT() + Rung.Tout)*1000000000,525-(double)Rung.OutVect.Vce/5);
        glEnd();
	
//	if(Rung.getT() < Rung.getTk() )
	printf("%2.10f,%f\n",(double)(100+(Rung.getT() + Rung.Tout)*100000000),
				(double)(525-Rung.OutVect.Vce/10));
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}

