// ~moriyama/projects/sr4000/trunk/src/faceDetector.cpp
// 
// 2008-12-01
// Kosuei MORIYAMA
//
// class for face detect
//

#include <cstdio>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include "pointing.h"

namespace point
{

faceDetector::faceDetector(CvSize size)
{
  scale = 1.3;
  cascadeName = "/home/sakane/pds/opencv-1.0.0-centos-x86/data/haarcascades/haarcascade_frontalface_alt2.xml";
  //cascadeName = "/mercury/home/sakane/pds/opencv-1.0.0.fc5/data/haarcascades/haarcascade_frontalface_alt2.xml";
  cascade = (CvHaarClassifierCascade*)cvLoad( cascadeName, 0, 0, 0 );
  storage = cvCreateMemStorage(0);
  smallImg = cvCreateImage( cvSize( cvRound (size.width/scale), cvRound (size.height/scale)), 8, 1 );
}

faceDetector::~faceDetector()
{
  cvReleaseImage(&smallImg);
  cvReleaseMemStorage(&storage);
}

int faceDetector::setCascadeName(char *name)
{
  if( !(cvLoad( name, 0, 0, 0 )) )
    return -1;

  cascadeName = name;
  cascade = (CvHaarClassifierCascade*)cvLoad( cascadeName, 0, 0, 0 );

  return 0;
}

int faceDetector::faceDetect( IplImage* img, CvPoint *center, int *radius )
{
  int i, ret;

  cvResize( img, smallImg, CV_INTER_LINEAR );
  cvEqualizeHist( smallImg, smallImg );
  cvClearMemStorage( storage );

  if( cascade )
    {
      CvSeq* faces = cvHaarDetectObjects( smallImg, cascade, storage, 1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(30, 30) );
      ret = (faces ? faces->total : 0);   

      for( i = 0; i < ret; i++ )
	{
	  CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
	  center->x = cvRound((r->x + r->width*0.5)*scale);
	  center->y = cvRound((r->y + r->height*0.5)*scale);
	  *radius = cvRound((r->width + r->height)*0.25*scale);
	}
    }

  return ret;
}

}
