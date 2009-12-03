#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
//for SR4000
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"
//for Biclops 
#include <panTiltUnit.h>
//for templateMatching
#include <templateMatching.h>
//for multi thread coding
#include <pthread.h>
//libraries for time count
#include <sys/time.h>
#include <sys/resource.h>
//etc...
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <tools.h>

#define TEMP_WIDTH 100
#define TEMP_HEIGHT 120

using namespace std;
using namespace point;

int main( void )
{
  cameraImages *ci = new cameraImages();
  ci -> initialize();
  CvSize imageSize = ci -> getImageSize();
  faceDetector *fd = new faceDetector( imageSize );
  regionTracker *human = new regionTracker( ci );
  templateMatching *tmch = new templateMatching();
  
  int key;
  IplImage *interfaceImg = NULL,*templateImg=NULL;

  CvPoint tempPt1,tempPt2,tempPtCenter;
  int tempWidth,tempHeight;

  bool createdTemplateImg = false;

  tempWidth = TEMP_WIDTH;
  tempHeight = TEMP_HEIGHT;

  tempPtCenter = cvPoint( imageSize.width / 2,imageSize.height / 2 );
  tempPt1 = cvPoint( tempPtCenter.x - tempWidth / 2, tempPtCenter.y - tempHeight / 2 );
  tempPt2 = cvPoint( tempPtCenter.x + tempWidth / 2, tempPtCenter.y + tempHeight / 2 );

  cvNamedWindow("SET YOUR HAND", 0 ); 
  cvNamedWindow("TEMPLATE IMAGE", 0 );

  cout<<"Press 't' key to create template image"<<endl;

  //create template image to use in the first
  while( !createdTemplateImg )
    {
      ci -> acquire();
      interfaceImg = ci -> getIntensityImg();
      cvRectangle( interfaceImg, tempPt1, tempPt2, CV_RGB( 255, 255, 255 ), 1, 8, 0 );
      cvShowImage("SET YOUR HAND", ci -> getIntensityImg() );
      key = cvWaitKey( 10 );
      if( human -> track()==0 && key == 't' )
	{
	  templateImg = cvCreateImage( cvSize( tempWidth, tempHeight ), IPL_DEPTH_32F, 1 );
	  tmch -> createTemplateImg( human -> getResult(), templateImg, tempPtCenter );
	  cvShowImage("TEMPLATE IMAGE", templateImg );
	  cout<<"Created temlate image is shown.OK?(y or n)"<<endl;
	  key = cvWaitKey( 0 );
	  if( key == 'y' )
	    createdTemplateImg = true;
	}
    }

  cvNamedWindow("Match Result", 0);

  CvPoint dstCenterLoc = cvPoint(0,0);
  int dstSize = 0;
  bool matched = false;

  while(1)
    {
      ci->acquire();
      interfaceImg = ci -> getIntensityImg();

      if(human->track() == 0)
	{
	  tmch -> calcMatchResult( human -> getResult(), templateImg, imageSize, dstCenterLoc, dstSize );
	  cvCircle( interfaceImg, dstCenterLoc, dstSize, CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	}
      
      cvShowImage("Match Result", interfaceImg);

      key = cvWaitKey(10);
      if(key == 'q')
	break;
    }

  cvDestroyWindow("SET YOUR HAND");
  cvDestroyWindow("TEMPLATE IMAGE");
  cvDestroyWindow("Match Result");


  cvReleaseImage(&templateImg);

  delete ci;
  delete fd;
  delete human;
  delete tmch;

  return 0;
}
