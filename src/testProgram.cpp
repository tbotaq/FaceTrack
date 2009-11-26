#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"
#include <templateMatching.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <tools.h>

using namespace std;
using namespace point;

int main(void)
{
  cout<<"1"<<endl;
  cameraImages *ci = new cameraImages();
  ci->initialize();
  //ci->acquire();
  regionTracker *human = new regionTracker(ci);
  faceDetector *fd = new faceDetector(ci->getImageSize());
  templateMatching *tmch = new templateMatching();
  IplImage *sourceImage,*templateImage,*humanImage,*intensityImage,*depthImage,*confidenceMap;
  cout<<"2"<<endl;
  cvNamedWindow("Intensity Image", 0);
  cvNamedWindow("Depth Image", 0);
  cvNamedWindow("Confidence Map", 0);
  cvNamedWindow("Human Image", 0);
  cvNamedWindow("Current Template Image", 0);

  cvNamedWindow("Initialize", 0);

  cout<<"3"<<endl;
  int key;
  CvPoint center;
  int radius;


  bool initialized = false;
  char answer;

  while(!initialized)
    {
      cout<<"A"<<endl;
      fd->faceDetect(ci->getIntensityImg(),&center,&radius);
      cout<<"B"<<endl;
      human -> track();
      cout<<"CC"<<endl;
      humanImage = human -> getResult();
      cout<<"CCC"<<endl;
      //templateImage = cvCreateImage(cvSize(radius*2,radius*2),IPL_DEPTH_8U,1);
      //tmch->setTempImage(humanImage,&center,&radius,templateImage);
      cout<<"C"<<endl;
      cout<<"\tOK?(y or n)"<<endl;
      cvShowImage("Initialize",templateImage);
      cin>>answer;
      if(answer=='y')
	initialized = true;
    }

  cvDestroyWindow("Initialize");

  while(1)
    {
      cout<<"4"<<endl;
      ci->acquire();
      intensityImage = ci->getIntensityImg();
      depthImage = ci->getDepthImg();
      confidenceMap = ci->getConfidenceMap();
     
      human->track();      
      humanImage = human -> getResult();
      cout<<"5"<<endl;
      fd->faceDetect(ci->getIntensityImg(),&center,&radius);
      templateImage = cvCreateImage(cvSize(radius*2,radius*2),IPL_DEPTH_8U,1);
      cout<<"6"<<endl;
      tmch->setTempImage(humanImage,&center,&radius,templateImage);
      tmch->calcMatchResult(intensityImage,templateImage,ci->getImageSize(),&center,&radius);
      cout<<"10"<<endl;
      //cvCircle(intensityImage,cvPoint(center.x,center.y),radius,CV_RGB(255,255,255),3,8,0);
     
      cvShowImage("Intensity Image",intensityImage);
      cvShowImage("Depth Image",depthImage);
      cvShowImage("Confidence Map",confidenceMap);
      cvShowImage("Human Image",humanImage);
      cvShowImage("Current Template Image",templateImage);
      
      key = cvWaitKey(10);
      if(key =='q')
	break;
    }
  cout<<"100"<<endl;
  cvDestroyWindow("Intensity Image");
  cvDestroyWindow("Depth Image");
  cvDestroyWindow("Confidence Map");  
  cvDestroyWindow("Human Image");
  cvDestroyWindow("Current Template Image");
  
  cvReleaseImage(&templateImage);

  delete ci;
  delete human;
  delete fd;
  delete tmch;

  return 0;

}
