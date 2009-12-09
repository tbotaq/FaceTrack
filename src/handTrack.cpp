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
#define TEMP_WIDTH 50
#define TEMP_HEIGHT 45
#define DST_THRESHOLD 70
#define FACE_THRESHOLD 40
#define A 0.1
#define B 0.1

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
  IplImage *interfaceImg = NULL,*dstTemplateImg = NULL,*faceTemplateImg = NULL,*dstDiffMapImg = NULL,*faceDiffMapImg = NULL;

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

  CvPoint windowOrigin = {10, 10};
  int align_offset = 270, vartical_offset = 350;
  cvNamedWindow( "SET YOUR HAND", 0);
  cvNamedWindow( "Binary Image", 0 );
  cvNamedWindow( "Destination Template Image", 0 );
  cvNamedWindow( "Face Template Image", 0 );
  cvNamedWindow( "Dst Diff Map Image", 0);
  cvNamedWindow( "Face Diff Map Image", 0);
  cvNamedWindow( "Match Result", 0);

  cvMoveWindow( "SET YOUR HAND", windowOrigin.x, windowOrigin.y);
  cvMoveWindow( "Binary Image", windowOrigin.x + vartical_offset, windowOrigin.y);
  cvMoveWindow( "Destination Template Image", windowOrigin.x, windowOrigin.y + align_offset);
  cvMoveWindow( "Face Template Image", windowOrigin.x + vartical_offset, windowOrigin.y + align_offset);
  cvMoveWindow( "Dst Diff Map Image", windowOrigin.x, windowOrigin.y + align_offset * 2);
  cvMoveWindow( "Face Diff Map Image", windowOrigin.x + vartical_offset, windowOrigin.y + align_offset * 2);
  cvMoveWindow( "Match Result", windowOrigin.x + vartical_offset * 2, windowOrigin.y);

  ///////////////////////////////////// 
  //   create first template image   //
  /////////////////////////////////////

  cout<<"Press 't' key to create template image"<<endl;
  
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
	  cvShowImage( "Binary Image", human -> getResult() ); 
	  if( key == 't' )
	    {
	      //setting the size of template image
	      dstTemplateImg = cvCreateImage( cvSize( tempWidth, tempHeight ), IPL_DEPTH_8U, 1 );
	      faceTemplateImg = cvCreateImage( cvSize( radius*2, radius*2 ), IPL_DEPTH_8U, 1 );
	  
	      //create template images
	      tmch -> createTemplateImg( human -> getResult(), faceTemplateImg, &faceCenterLoc );
	      tmch -> createTemplateImg( human -> getResult(), dstTemplateImg, &tempPtCenter );
  
	      cvShowImage( "Destination Template Image", dstTemplateImg );
	      cvShowImage( "Face Template Image", faceTemplateImg );
	    
	      cout<<"Created temlate image is shown.OK?(y or n)"<<endl;

	      key = cvWaitKey( 0 );

	      if( key == 'y' )
		createdTemplateImg = true;
	    }
	}
    }

  ///////////////////////////////////// 
  //         tracking loop           //
  ///////////////////////////////////// 

  double currentDstErrorValue,currentFaceErrorValue;
  double dstSimilarity,faceSimilarity;
  int ohandx = 0, ohandy = 0, ofacex = 0, ofacey = 0;
  int mx = 0, my = 0;

  while(createdTemplateImg)
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
	  currentDstErrorValue = tmch -> getErrorValue();
	  dstSimilarity = tmch -> getSimilarity();
	  cout<<"Similarity[%] of dst \t= "<<faceSimilarity<<"[%]"<<endl;
	  //if(currentDstErrorValue>A)
	  //cerr<<"Dst EVal \t= "<<currentDstErrorValue<<endl;

	  tmch -> calcMatchResult( human -> getResult(), faceTemplateImg, imageSize, &faceCenterLoc, &radius );
	  currentFaceErrorValue = tmch -> getErrorValue();
	  faceSimilarity = tmch -> getSimilarity();
	  cout<<"Similarity[%] of face\t= "<<dstSimilarity<<"[%]"<<endl;
	  //if(currentFaceErrorValue>B)
	  //cerr<<"Face EVal \t= "<<currentFaceErrorValue<<endl;

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
	    }

	  // set -1 as an invalid value if the similarity is smaller than threshold
	  if( dstSimilarity < DST_THRESHOLD )
	    dstCenterLoc = cvPoint( -1, -1 );
	  if( faceSimilarity < FACE_THRESHOLD )
	    faceCenterLoc = cvPoint( -1, -1 );

	  // move pan/tilt unit
	  	  if(( faceCenterLoc.x != -1 )&&( dstCenterLoc.x != -1 )) 
	    { 
	      ofacex = faceCenterLoc.x, ofacey = faceCenterLoc.y, ohandx = dstCenterLoc.x, ohandy = dstCenterLoc.y;
	    }
	  else if(( faceCenterLoc.x != -1 )&&( dstCenterLoc.x == -1 ))
	    {
	      mx = ohandx;
	      my = ohandy;
	      fprintf( stderr, "move to %d, %d\n", mx, my );
	    }
	  else if(( faceCenterLoc.x == -1 )&&( dstCenterLoc.x != -1 ))
	    {
	      mx = ofacex;
	      my = ofacey;
	      fprintf( stderr, "move to %d, %d\n", mx, my );
	    }
	  else if (( faceCenterLoc.x == -1 )&&( dstCenterLoc.x == -1 ))
	    {
	      fprintf( stderr, "lost two points\n" );
	    }
	  else if (!(( faceCenterLoc.x != -1 )&&( dstCenterLoc.x != -1 )) && !(( ofacex != -1 )&&( ohandx != -1 )))
	    {
	      fprintf( stderr, "lost points completely\n" );
	    }
	}

      //unknown phenomenon handling
      if( frames % 4 == 0 )
	{
	  dstCenterLoc.x -= 2;
	  dstCenterLoc.y -= 2;
	  faceCenterLoc.x -= 2;
	  faceCenterLoc.y -= 2;
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
      cvShowImage( "Dst Diff Map Image", tmch -> getDiffMapImg( human -> getResult(), dstTemplateImg, dstDiffMapImg ) );
      cvShowImage( "Face Diff Map Image", tmch -> getDiffMapImg( human -> getResult(), faceTemplateImg, faceDiffMapImg ) );

      key = cvWaitKey(10);
      if( key == 'q' )
	break;
      frames++;
    }

  //release memory
  cvDestroyWindow( "SET YOUR HAND" );
  cvDestroyWindow( "Destination Template Image" );
  cvDestroyWindow( "Face Template Image" );
  cvDestroyWindow( "Match Result" );
  cvDestroyWindow( "Binary Image" );
  cvDestroyWindow( "Dst Diff Map Image" );
  cvDestroyWindow( "Face Diff Map Image" );

  cvReleaseImage( &dstTemplateImg );
  cvReleaseImage( &faceTemplateImg );

  delete ci;
  delete fd;
  delete human;
  delete tmch;
  delete tool;

  return 0;
}
