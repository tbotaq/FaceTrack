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
  cameraImages *ci = new cameraImages();
  ci->initialize();
  //ci->acquire();
  regionTracker *human = new regionTracker(ci);
  faceDetector *fd = new faceDetector(ci->getImageSize());
  templateMatching *tmch = new templateMatching();
  IplImage *sourceImage,*templateImage,*humanImage,*intensityImage,*depthImage,*confidenceMap;

  cvNamedWindow("Intensity Image", 0);
  cvNamedWindow("Depth Image", 0);
  cvNamedWindow("Confidence Map", 0);
  cvNamedWindow("Human Image", 0);
  cvNamedWindow("Current Template Image", 0);

  int key;
  CvPoint center;
  int radius;

  while(1)
    {
      ci->acquire();
      intensityImage = ci->getIntensityImg();
      depthImage = ci->getDepthImg();
      confidenceMap = ci->getConfidenceMap();
     
      human->track();      
      humanImage = human -> getResult();
      
      fd->faceDetect(ci->getIntensityImg(),&center,&radius);
      templateImage = cvCreateImage(cvSize(radius*2,radius*2),IPL_DEPTH_8U,1);
      tmch->setTempImage(humanImage,&center,&radius,templateImage);
      tmch->calcMatchResult(intensityImage,templateImage,ci->getImageSize(),&center,&radius);

      cvShowImage("Intensity Image",intensityImage);
      cvShowImage("Depth Image",depthImage);
      cvShowImage("Confidence Map",confidenceMap);
      cvShowImage("Human Image",humanImage);
      cvShowImage("Current Template Image",templateImage);
      
      key = cvWaitKey(10);
      if(key =='q')
	break;
    }

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
