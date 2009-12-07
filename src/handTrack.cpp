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

//width and height of template image created in the first
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
  tools *tool = new tools();
  
  int key;
  IplImage *interfaceImg = NULL,*dstTemplateImg = NULL,*faceTemplateImg = NULL;

  CvPoint tempPt1,tempPt2,tempPtCenter;
  int tempWidth,tempHeight;

  //flags
  bool createdTemplateImg = false;
  bool updatedTamplateImg = false;

  CvPoint dstCenterLoc = cvPoint(0,0),faceCenterLoc = cvPoint(0,0);
  CvPoint dstPrevCenterLoc = cvPoint(0,0),facePrevCenterLoc = cvPoint(0,0);
  int diffDstCenterLocX = 0,diffDstCenterLocY = 0,diffFaceCenterLocX = 0,diffFaceCenterLocY = 0;
  int dstSize = 0,radius = 0;
  
  double pan = 0,tilt = 0;
  int frames = 0;

  tempWidth = TEMP_WIDTH;
  tempHeight = TEMP_HEIGHT;

  tempPtCenter = cvPoint( imageSize.width / 2 + 25, imageSize.height / 2 - 25 );
  tempPt1 = cvPoint( tempPtCenter.x - tempWidth / 2, tempPtCenter.y - tempHeight / 2 );
  tempPt2 = cvPoint( tempPtCenter.x + tempWidth / 2, tempPtCenter.y + tempHeight / 2 );

  cvNamedWindow( "SET YOUR HAND", 0 );  
  

  cout<<"Press 't' key to create template image"<<endl;


  ///////////////////////////////////// 
  //   create first template image   //
  /////////////////////////////////////
  
  while( !createdTemplateImg )
    {
      //acquire current frame
      ci -> acquire();
      interfaceImg = ci -> getIntensityImg();

      //detect human's face using ada-boost
      fd -> faceDetect( interfaceImg, &faceCenterLoc, &radius );

      //draw a rectangle representing the region to be template image
      cvRectangle( interfaceImg, tempPt1, tempPt2, CV_RGB( 255, 255, 255 ), 1, 8, 0 );

      cvShowImage("SET YOUR HAND", ci -> getIntensityImg() );

      key = cvWaitKey( 10 );

      if( human -> track() == 0)
	{
	  cvNamedWindow( "Binary Image", 0 );
	  cvShowImage( "Binary Image", human -> getResult() ); 
	  if( key == 't' )
	    {
	      //setting the size of template image
	      dstTemplateImg = cvCreateImage( cvSize( tempWidth, tempHeight ), IPL_DEPTH_8U, 1 );
	      faceTemplateImg = cvCreateImage( cvSize( radius*2, radius*2 ), IPL_DEPTH_8U, 1 );
	  
	      //create template images
	      tmch -> createTemplateImg( human -> getResult(), faceTemplateImg, &faceCenterLoc );
	      tmch -> createTemplateImg( human -> getResult(), dstTemplateImg, &tempPtCenter );

	      cvNamedWindow( "Destination Template Image", 0 );
	      cvNamedWindow( "Face Template Image", 0 );
	  

	      cvShowImage( "Destination Template Image", dstTemplateImg );
	      cvShowImage( "Face Template Image", faceTemplateImg );
	      

	      cout<<"Created temlate image is shown.OK?(y or n)"<<endl;

	      key = cvWaitKey( 0 );

	      if( key == 'y' )
		createdTemplateImg = true;
	      if( key == 'n' )
		continue;
	      if( key == 'q' )
		{
		  //release memory and terminate this program
		  cvReleaseImage( &dstTemplateImg );
		  delete ci;
		  delete fd;
		  delete human;
		  delete tmch;
		  delete tool;
		  return 0;
		}
	    }
	}
    }

  cvDestroyWindow( "SET YOUR HAND" );

  cvNamedWindow( "Match Result", 0);

  ///////////////////////////////////// 
  //         tracking loop           //
  ///////////////////////////////////// 

  while(1)
    {

      //acquire current frame
      ci->acquire();
      interfaceImg = ci -> getIntensityImg();

      if( !updatedTamplateImg )
	fd -> faceDetect( interfaceImg, &faceCenterLoc, &radius );

      if( human -> track() == 0 )
	{
	  dstPrevCenterLoc = dstCenterLoc;
	  facePrevCenterLoc = faceCenterLoc;

	  tmch -> calcMatchResult( human -> getResult(), dstTemplateImg, imageSize, &dstCenterLoc, &dstSize );
	  cout<<"Similarity[%] of dst \t= "<<(int)tmch->getSimilarity()<<"[%]"<<endl;
	  tmch -> calcMatchResult( human -> getResult(), faceTemplateImg, imageSize, &faceCenterLoc, &radius );
	  cout<<"Similarity[%] of face\t= "<<(int)tmch->getSimilarity()<<"[%]"<<endl;

	  diffDstCenterLocX = abs( dstCenterLoc.x - dstPrevCenterLoc.x );
	  diffDstCenterLocY = abs( dstCenterLoc.y - dstPrevCenterLoc.y );

	  diffFaceCenterLocX = abs( faceCenterLoc.x - facePrevCenterLoc.x );
	  diffFaceCenterLocY = abs( faceCenterLoc.y - facePrevCenterLoc.y );

	  if( updatedTamplateImg )
	    {
	      if( diffDstCenterLocX > 10 || diffDstCenterLocY > 10 )
		{
		  cout<<"### Detected Abnormal Value in Destination Tracking."<<endl;
		  dstCenterLoc = dstPrevCenterLoc;
		}
	      if( diffFaceCenterLocX > 10 || diffFaceCenterLocY > 10 )
		{
		  cout<<"### Detected Abnormal Value in Face Tracking."<<endl;
		  faceCenterLoc = facePrevCenterLoc;
		}

	      //unknown phenomenon handling
	      if( frames % 4 == 0 )
		{
		  dstCenterLoc.x -= 2;
		  dstCenterLoc.y -= 2;
		  faceCenterLoc.x -= 2;
		  faceCenterLoc.y -= 2;
		}
	    }
	}

      //draw two circles to destinarion and face
      cvCircle( interfaceImg, dstCenterLoc, dstSize, CV_RGB( 255, 255, 255 ), 1, 8, 0 );
      cvCircle( interfaceImg, faceCenterLoc, radius, CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	  
      //update template image        
      tmch -> createTemplateImg( human -> getResult(), dstTemplateImg, &dstCenterLoc );
      tmch -> createTemplateImg( human -> getResult(), faceTemplateImg, &faceCenterLoc );
      updatedTamplateImg = true;

      cvShowImage( "Match Result", interfaceImg );
      cvShowImage( "Destination Template Image", dstTemplateImg );
      cvShowImage( "Face Template Image", faceTemplateImg );
      cvShowImage( "Binary Image", human -> getResult() );

      key = cvWaitKey(10);
      if( key == 'q' )
	break;

      frames++;
    }

  //release memory
  cvDestroyWindow( "Destination Template Image" );
  cvDestroyWindow( "Face Template Image" );
  cvDestroyWindow( "Match Result" );
  cvDestroyWindow( "Binary Image" );

  cvReleaseImage( &dstTemplateImg );
  cvReleaseImage( &faceTemplateImg );

  delete ci;
  delete fd;
  delete human;
  delete tmch;
  delete tool;

  return 0;
}
