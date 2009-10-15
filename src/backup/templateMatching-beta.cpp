#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include <stdio.h>
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"
#define THRESHOLD 50
#define THRESHOLD_MAX_VALUE 255
#define LINE_THICKNESS 1
#define LINE_TYPE 8
#define SHIFT 0

using namespace std;
using namespace point;

int main()
{
  int key;
  cameraImages *ci = new cameraImages();
  ci->initialize();
  IplImage *templateImage = cvLoadImage("todai_boys_small.jpg",1);
  CvSize srcimagesize = ci -> getImageSize();
  CvPoint minLocation;
  char windowNameTemplate[] = "Template";
  char windowNameDestination[] = "Destination";
  IplImage *sourceImage;
  IplImage *sourceGrayImage;
  IplImage *templateGrayImage;
  IplImage *sourceBinaryImage;
  IplImage *templateBinaryImage;
  IplImage *differenceMapImage;
  CvScalar scalar;

  while(1)
    {
      ci->acquire();
      sourceImage = ci->getIntensityImg();
      sourceGrayImage = cvCreateImage(srcimagesize,IPL_DEPTH_8U,1);
      templateGrayImage = cvCreateImage(cvGetSize(templateImage),IPL_DEPTH_8U,1);
      sourceBinaryImage = cvCreateImage(srcimagesize,IPL_DEPTH_8U,1);
      templateBinaryImage = cvCreateImage(cvGetSize(templateImage),IPL_DEPTH_8U,1);
      differenceMapImage = cvCreateImage(cvSize(srcimagesize.width - templateImage->width +1,srcimagesize.height -templateImage->height +1),IPL_DEPTH_32F,1);
      sourceGrayImage = sourceImage;
      cvCvtColor( templateImage, templateGrayImage, CV_BGR2GRAY );
	
     
      cvThreshold( sourceGrayImage, sourceBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );
      cvThreshold( templateGrayImage, templateBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );
      
      
      cvMatchTemplate( sourceBinaryImage, templateBinaryImage, differenceMapImage, CV_TM_SQDIFF );
      
      cvMinMaxLoc( differenceMapImage, NULL, NULL, &minLocation, NULL, NULL );
      
	
      cvRectangle(
		  sourceImage,
		  minLocation,
		  cvPoint( minLocation.x + templateImage->width, minLocation.y + templateImage->height ),
		  CV_RGB( 255, 255, 255 ),
		  LINE_THICKNESS,
		  LINE_TYPE,
		  SHIFT
		  );
      
	
      cvNamedWindow( windowNameTemplate, CV_WINDOW_AUTOSIZE );
      cvNamedWindow( windowNameDestination, CV_WINDOW_AUTOSIZE );
      
      
      cvShowImage( windowNameTemplate, templateImage );
      cvShowImage( windowNameDestination , sourceImage );
      printf("minLocation=%d,%d\n",minLocation.x,minLocation.y);
      scalar = cvGet2D(sourceImage,minLocation.x,minLocation.y);
      printf("H=%7.2f\n",scalar.val[0]);
	
      // key handling
      key = cvWaitKey(100);
      if(key == 'q')
	{
	  break;
	} 
    }
      
      
 
      
      
  cvReleaseImage( &sourceImage );
  cvReleaseImage( &templateImage );
  cvReleaseImage( &sourceGrayImage );
  cvReleaseImage( &templateGrayImage );
  cvReleaseImage( &sourceBinaryImage );
  cvReleaseImage( &templateBinaryImage );
  cvReleaseImage( &differenceMapImage );
      
  cvDestroyWindow(windowNameTemplate);
  cvDestroyWindow(windowNameDestination);
  delete ci;
  return 0;
}
  
