// faceTrack ver1.0
// Grid-separeted tracking
// Pan/Tilt unit moves to the grid where the face is detected.
// Unsupport multi thread process
// Unsupport PID control as move control.

//libraries for image proccess
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"
//libraies for Biclops 
#include<panTiltUnit.h>
#define P 10
#define T 5
//libraries for time measurment
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include<stdio.h> 

using namespace std;
using namespace point;

double getrusageSec(){
  struct rusage t;
  struct timeval s;
  getrusage(RUSAGE_SELF, &t);
  s = t.ru_utime;
  return s.tv_sec + (double)s.tv_usec*1e-6;
}

int main(void)
{
  panTiltUnit *ptu;  ptu = new panTiltUnit();
  cameraImages *ci;  ci = new cameraImages();
  faceDetector *fd;  fd = new faceDetector(ci->getImageSize());
  double t1=0,t2=0;
  double totalTime=0;
  int times=0;
  int key;

  // initialize camera image class
  ci->initialize();
  CvSize size = ci -> getImageSize();
  int Lx = size.width;
  int Ly = size.height;

  // make windows
  cvNamedWindow("Face Detection", CV_WINDOW_AUTOSIZE);
 
  //-----main process-----//
  while(1)
    {
      //start measuring time	
      t1 = getrusageSec();

      //aqcuire current flame
      ci -> acquire();

      //drow a circle on the detected face
      cvCircle(ci->getIntensityImg(),cvPoint(center.x,center.y),radius,CV_RGB(255,255,255),1,8,0);

      //get the number of detected face
      int faces = fd -> faceDetect(ci -> getIntensityImg(), &center, &radius);
      int x = center.c;
      int y = center.y;
      int pan =P,tilt=T;

      if(faces >= 1)
	{
	  if(x<=Lx/3)
	    {
	      if(y<=Ly/3)
		{
		  cout<<"1";
		  tilt *= -1;
		}
	      else if(y>Ly/3 && y>Ly*2/3)
		{
		  cout<<"2";
		  tilt = 0;
		}
	      else
		cout<<"3";
	    }
	  else if(x>Lx/3 && x<Ly*2/3)
	    {
	      pan = 0;
	      if(y<=Ly/3)
		{
		  cout<<"4";
		  tilt *= -1;
		}
	      else if(y>Ly/3 && y>Ly*2/3)
		{
		  cout<<"5";
		  tilt = 0;
		}
	      else
		cout<<"6";
	    }
	  else if(x>=Lx*2/3 && x<=Lx)
	    {
	      pan *= -1;
	      if(y<=Ly/3)
		{
		  cout<<"7";
		  tilt *= -1;
		}
	      else if(y>Ly/3 && y>Ly*2/3)
		{
		  cout<<"8";
		  tilt = 0;
		}
	      else
		cout<<"9";
	    }
	}
      else
	{
	  pan = 0;
	  tilt = 0;
	}
      cout<<endl;

      //move to coordinated location
      ptu -> move(pan,tilt);

      //stop measuring time
      t2 = getrusageSec();
      times++;
      totalTime += t2-t1;

      // show images
      cvShowImage("Face Detection",ci->getIntensityImg());
   
      // key handling
      key = cvWaitKey(100);
      if(key == 'q')
	{
	  break;
	}
    }
  // release memory
  cvDestroyWindow("Face Detection");
  delete ci;
  delete fd;
  delete ptu;
  printf("\nAverage time is %f[sec/process]\n(calculated by %d processes)\n\n",totalTime/times,times);
  return 0;
}
