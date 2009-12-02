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
  center.x = 0;
  center.y = 0;
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
  faceCenterDepth=cvGet2D(ci->getDepthImg(),center.y,center.x);
  faceCenterDepth.val[0] /= 50;

  int startX = center.x - radius;
  int startY = center.y -radius;
  int endX = center.x + radius;
  int endY = center.y + radius;

  IplImage *faceBinaryImage;
  faceBinaryImage = cvCreateImage(cvSize(imageSize.width,imageSize.height),IPL_DEPTH_8U,1);

  while(1)
    {
      ci->acquire();
      int WHITE=0,BLACK=0;
      
      for(int y=startY;y<endY;y++)
	{
	  for(int x=startX;x<endX;x++)
	    {cout<<"2"<<endl;
	      currentValue = cvGet2D(ci->getDepthImg(),y,x);
	      cout<<"3"<<endl;
	      currentValue.val[0] /= 50;
	      //int diff = currentValue.val[0] - faceCenterDepth.val[0];
	      if(!(currentValue.val[0] > (faceCenterDepth.val[0] + faceThreshold) || currentValue.val[0] < (faceCenterDepth.val[0] - faceThreshold)))
		{cout<<"A"<<endl;
		  cvSet2D(faceBinaryImage,y,x,CV_RGB(255,255,255));
		  WHITE++;
		}
	      else
		{cout<<"B"<<endl;
		  cout<<"x,y="<<x<<","<<y<<endl;
		  cvSet2D(faceBinaryImage,y,x,CV_RGB(0,0,0));
		  BLACK++;
		}
	    }
	}
      int totalPixels = imageSize.height*imageSize.width;
      cout<<"WHITE,BLACK="<<WHITE*100/totalPixels<<","<<BLACK*100/totalPixels<<"[%]"<<endl;
      cout<<"Choosed value on depth image="<<currentValue.val[0]<<endl;

      cvCircle(ci->getIntensityImg(),center,1,CV_RGB(255,255,255),1,8,0);

      cvShowImage("Result",faceBinaryImage);
      cvShowImage("Depth",ci->getDepthImg());
      
      char key = cvWaitKey(10);
      if(key=='q')
	break;
    }

  delete ci,fd,tmch;
  cvReleaseImage(&faceBinaryImage);
  cvDestroyWindow("Result");
  cvDestroyWindow("Depth");

  return 0;
}

