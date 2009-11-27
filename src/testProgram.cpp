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

faceDetector *fd;
cameraImages *ci;


void getFaceInfo(IplImage *intensityImage,CvPoint *center,int radius)
{
  //input intensity image
  //get face's center location
  //get face"s radius

  fd->faceDetect(intensityImage,center,&radius);
}

IplImage *createHumanImage(IplImage *depthImage,CvPoint *center)
{
  cout<<"A"<<endl;
  //input depth image and face's center location
  //return human image (human region is colored white)

  IplImage *retImage;
  cout<<"B"<<endl;
  //create "human image" from input image
  CvSize imageSize = cvGetSize(depthImage);
  cout<<"C"<<endl;
  CvScalar faceCenterDepth = cvScalar(0);
  CvScalar currentValue = cvScalar(0);
  faceCenterDepth=cvGet2D(depthImage,center->y,center->x);
  cout<<"D"<<endl;
  int faceThreshold = 50;
  cout<<"E"<<endl;

  
  for(int y=0;y<imageSize.height;y++)
    for(int x=0;y<imageSize.width;x++)
      {
	currentValue = cvGet2D(depthImage,y,x);
	if(currentValue.val[0] < (faceCenterDepth.val[0] + faceThreshold) || currentValue.val[0] > (faceCenterDepth.val[0] - faceThreshold))
	  cvSet2D(retImage,y,x,CV_RGB(255,255,255));
	else
	  cvSet2D(retImage,y,x,CV_RGB(0,0,0));
      }
  
  return retImage;
}

IplImage *createTempImg(IplImage *humanImage,CvPoint *center,int radius)
{
  //input pointer to source/template image
  //input face info(center location and radius)
  //cut souceImage to create templateImage
  //return template image 

  IplImage *retImage;
  int tempWidth = radius * 2;
  int tempHeight = tempWidth;//this formula might need changed

  retImage = cvCreateImage(cvSize(tempWidth,tempHeight),IPL_DEPTH_8U,1);
  cvGetRectSubPix(humanImage,retImage,cvPointTo32f(*center));

  return retImage;
}

void calcMatchResult(IplImage *sourceImage,IplImage *templateImage)
  {

    //ask to hagiwara

  }


int main(void)
{
  cout<<"1"<<endl;
  ci = new cameraImages();
  ci->initialize();
  ci->acquire();
  fd = new faceDetector(ci->getImageSize());

  CvPoint *center;
  int radius;
  cout<<"25"<<endl;
  //get face location and radius
  getFaceInfo(ci->getIntensityImg(),center,radius);
  cout<<"30"<<endl;
  //convert depth image into human image(human region is colored white)
  IplImage *humanImage = createHumanImage(ci->getDepthImg(),center);
  cout<<"50"<<endl;
  //create template image using faceDetector::faceDetect()
  IplImage *templateImage = createTempImg(humanImage,center,radius);

  cvNamedWindow("Template Image",0);
  cvShowImage("Template Image",templateImage);
  int key = cvWaitKey(10);
  if(key=='q')
    cvDestroyWindow("Template Image");

  cvReleaseImage(&templateImage);
  cout<<"100"<<endl;

  delete ci;
  delete fd;

  return 0;
}
