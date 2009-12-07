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

#define TEMP_WIDTH 80
#define TEMP_HEIGHT 70

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
  panTiltUnit *ptu = new panTiltUnit();
  tools *tool = new tools();
  
  int key;
  IplImage *interfaceImg = NULL,*templateImgH = NULL,*templateImgF = NULL;

  CvPoint tempPt1,tempPt2,tempPtCenter;
  int tempWidth,tempHeight;

  bool createdTemplateImg = false;

  tempWidth = TEMP_WIDTH;
  tempHeight = TEMP_HEIGHT;

  tempPtCenter = cvPoint( imageSize.width / 2 + 25, imageSize.height / 2 - 25 );
  tempPt1 = cvPoint( tempPtCenter.x - tempWidth / 2, tempPtCenter.y - tempHeight / 2 );
  tempPt2 = cvPoint( tempPtCenter.x + tempWidth / 2, tempPtCenter.y + tempHeight / 2 );

  cvNamedWindow("SET YOUR HAND", 0 ); 
  

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
	  templateImgH = cvCreateImage( cvSize( tempWidth, tempHeight ), IPL_DEPTH_8U, 1 );
	  tmch -> createTemplateImg( human -> getResult(), templateImgH, &tempPtCenter );
	  cvNamedWindow("TEMPLATE IMAGE", 0 );
	  cvShowImage("TEMPLATE IMAGE", templateImgH );
	  cout<<"Created temlate image is shown.OK?(y or n)"<<endl;
	  key = cvWaitKey( 0 );
	  if( key == 'y' )
	    createdTemplateImg = true;
	  if( key == 'q' )
	    {
	      cvReleaseImage(&templateImgH);

	      delete ci;
	      delete fd;
	      delete human;
	      delete tmch;
	      delete ptu;
	      delete tool;
	      return 0;
	    }
	}
    }

  cvDestroyWindow("SET YOUR HAND");

  cvNamedWindow("Match Result", 0);

  CvPoint dstCenterLoc = cvPoint(0,0);
  CvPoint dstPrevCenterLoc = cvPoint(0,0);
  int diffCenterLocX=0,diffCenterLocY=0;
  int dstSize = 0;
  bool updatedTamplateImg = false;
  double pan=0,tilt=0;
  int frames = 0;

  while(1)
    {
      ci->acquire();
      interfaceImg = ci -> getIntensityImg();

      if(human->track() == 0)
	{
	  dstPrevCenterLoc = dstCenterLoc;
	  tmch -> calcMatchResult( human -> getResult(), templateImgH, imageSize, &dstCenterLoc, &dstSize );
	  diffCenterLocX = abs(dstCenterLoc.x-dstPrevCenterLoc.x);
	  diffCenterLocY = abs(dstCenterLoc.y-dstPrevCenterLoc.y);

	  if( updatedTamplateImg && ( diffCenterLocX > 10 || diffCenterLocY > 10 ) )
	    {
	      cout<<"### Detected Abnormal Value"<<endl;
	      dstCenterLoc = dstPrevCenterLoc;
	    }
	  if( frames % 4 == 0 )
	    {
	      cout<<"#####"<<endl;
	      dstCenterLoc.x -= 2;
	      dstCenterLoc.y -= 2;
	    }

	  cout<<"CX="<<dstCenterLoc.x<<endl;
	  cout<<"CY="<<dstCenterLoc.y<<endl;
	  cvCircle( interfaceImg, dstCenterLoc, dstSize, CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	}
      pan = tool->getMoveDist(tempPtCenter.x,dstCenterLoc.x);
      tilt = tool->getMoveDist(tempPtCenter.y,dstCenterLoc.y);
      //ptu->move(pan,tilt);
      cout<<"(pan,tilt) = ("<<pan<<","<<tilt<<")"<<endl;
      //templateImgH = cvCreateImage( cvSize(
      tmch -> createTemplateImg( human -> getResult(), templateImgH, &dstCenterLoc);
      updatedTamplateImg = true;

      cvShowImage("Match Result", interfaceImg);
      cvShowImage("TEMPLATE IMAGE", templateImgH);

      key = cvWaitKey(10);
      if(key == 'q')
	break;
      frames++;
    }
  
  cvDestroyWindow("TEMPLATE IMAGE");
  cvDestroyWindow("Match Result");


  cvReleaseImage(&templateImgH);

  delete ci;
  delete fd;
  delete human;
  delete tmch;
  delete ptu;
  delete tool;

  return 0;
}
