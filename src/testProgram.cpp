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
 
  IplImage *intensityImage,*humanImage;
  templateMatching *tmch =new templateMatching();
  cameraImages *ci = new cameraImages();
  ci->initialize();
  ci->acquire();
  faceDetector *fd = new faceDetector(ci->getImageSize());

  int faces=0;
  CvPoint center;
  int radius;

  CvSize imageSize = ci->getImageSize();
  CvScalar faceCenterDepth = cvScalar(0);
  CvScalar currentValue = cvScalar(0);

  int faceThreshold;
  cout<<"Input face threshold"<<endl;
  cin>>faceThreshold;
  cout<<"center="<<center.x<<","<<center.y<<endl;

  int WHITE=0,BLACK=0;

  cvNamedWindow("Result",0);
  cvNamedWindow("Depth",0);

  //get face information (center location and radius)
  while(faces<1)
    {
      ci->acquire();
      faces = fd->faceDetect(ci->getIntensityImg(),&center,&radius);
    }
  ci->acquire();
  faceCenterDepth=cvGet2D(ci->getDepthImg(),center.y,center.x);
  faceCenterDepth.val[0] /= 50;

  while(1)
    {
      ci->acquire();
      int WHITE=0,BLACK=0;
      
      for(int y=0;y<imageSize.height;y++)
	{
	  for(int x=0;x<imageSize.width;x++)
	    {
	      currentValue = cvGet2D(ci->getDepthImg(),y,x);
	      currentValue.val[0] /= 50;
	      if(!(currentValue.val[0] > (faceCenterDepth.val[0] + faceThreshold) || currentValue.val[0] < (faceCenterDepth.val[0] - faceThreshold)))
		{
		  cvSet2D(ci->getIntensityImg(),y,x,CV_RGB(255,255,255));
		  WHITE++;
		}
	      else
		{
		  cvSet2D(ci->getIntensityImg(),y,x,CV_RGB(0,0,0));
		  BLACK++;
		  
		}
	    }
	}
      int totalPixels = imageSize.height*imageSize.width;
      cout<<"WHITE,BLACK="<<WHITE*100/totalPixels<<","<<BLACK*100/totalPixels<<endl;
      cout<<"Choosed value on depth image="<<currentValue.val[0]<<endl;

      cvCircle(ci->getIntensityImg(),center,1,CV_RGB(255,255,255),1,8,0);

      cvShowImage("Result",ci->getIntensityImg());
      cvShowImage("Depth",ci->getDepthImg());
      char key = cvWaitKey(10)j;
      if(key=='q')
	break;
    }

  delete ci,fd,tmch;
  cvDestroyWindow("Result");
  cvDestroyWindow("Depth");

  return 0;
}
  

