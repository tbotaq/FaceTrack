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

using namespace std;
using namespace point;

int main( void )
{
  //initializations of classes
  cameraImages *ci = new cameraImages();
  //cameraImages *face = new cameraImages();
  ci -> initialize();
  //face -> initialize();
  CvSize imageSize = ci -> getImageSize();
  faceDetector *fd = new faceDetector( imageSize );
  regionTracker *human = new regionTracker( ci );
  templateMatching *tmch = new templateMatching(ci);
  tools *tool = new tools();

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //-------------------------------------------------variables declaration-------------------------------------------------//
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  //numbers of frames
  int frames = 0;

  int key;
 
  double pan=0,tilt=0;

  //define and set GUI windows 
  CvPoint windowOrigin = {10, 10};
  int align_offset = 270, vartical_offset = 350;

  cvNamedWindow( "Interface Image", 0);
  cvNamedWindow( "Binary Image", 0 );
  cvNamedWindow( "Destination Template Image", 0 );
  cvNamedWindow( "Face Template Image", 0 );
  cvNamedWindow( "Hand Diff Map Image", 0);
  cvNamedWindow( "Face Diff Map Image", 0);
  cvNamedWindow( "Match Result", 0);

  cvMoveWindow( "Interface Image", windowOrigin.x, windowOrigin.y);
  cvMoveWindow( "Binary Image", windowOrigin.x + vartical_offset, windowOrigin.y);
  cvMoveWindow( "Destination Template Image", windowOrigin.x, windowOrigin.y + align_offset);
  cvMoveWindow( "Face Template Image", windowOrigin.x + vartical_offset, windowOrigin.y + align_offset);
  cvMoveWindow( "Hand Diff Map Image", windowOrigin.x, windowOrigin.y + align_offset * 2);
  cvMoveWindow( "Face Diff Map Image", windowOrigin.x + vartical_offset, windowOrigin.y + align_offset * 2);
  cvMoveWindow( "Match Result", windowOrigin.x + vartical_offset * 2, windowOrigin.y);

  //initialize (create first template image)
  tmch -> init(ci,fd,human);

  panTiltUnit *ptu = new panTiltUnit();

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //-------------------------------------------------tracking loop---------------------------------------------------------//
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  while( 1 )
    {
      tmch -> outOfRegion = false;

      //acquire current frame
      ci->acquire();
      
      if( !tmch -> updatedTamplateImg )
	fd -> faceDetect( tmch -> interfaceImg, &(tmch -> faceCenterLoc), &(tmch -> radius ) );

      if( human -> track() == 0 )
	{
	  cout<<"!!handCentLoc  ="<<tmch -> handCenterLoc.x<<","<<tmch -> handCenterLoc.y<<endl;
	  cout<<"!!faceCenterLoc="<<tmch -> faceCenterLoc.x<<","<<tmch -> faceCenterLoc.y<<endl;
	  cout<<"!!PH="<<tmch -> handPrevCenterLoc.x<<","<<tmch->handPrevCenterLoc.y<<endl;
	  cout<<"!!PF="<<tmch -> facePrevCenterLoc.x<<","<<tmch->facePrevCenterLoc.y<<endl; 
	  //calculate matching results for each destination(hand and face)
	  tmch -> calcMatchResult( human -> getResult(), tmch -> handTemplateImg, tmch -> imageSize, &(tmch -> handCenterLoc), &(tmch -> handSize ));
	  tmch -> handSimilarity = tmch -> getSimilarity();
	  cout<<"Similarity[%] of hand \t= "<<tmch -> handSimilarity<<"[%]"<<endl;
	 
	  tmch -> calcMatchResult( ci -> getIntensityImg(), tmch -> faceTemplateImg, tmch -> imageSize, &(tmch -> faceCenterLoc), &(tmch -> radius ));
	  tmch -> faceSimilarity = tmch -> getSimilarity();
	  cout<<"Similarity[%] of face\t= "<<tmch -> faceSimilarity<<"[%]"<<endl;

	  cout<<"handCentLoc  ="<<tmch -> handCenterLoc.x<<","<<tmch -> handCenterLoc.y<<endl;
	  cout<<"faceCenterLoc="<<tmch -> faceCenterLoc.x<<","<<tmch -> faceCenterLoc.y<<endl;
	  
	  tmch -> checkErrorByDiff(100);
	  tmch -> checkErrorBySimi(50);
	  	  
	  if( tmch -> errorIsDetectedBySimi || tmch -> errorIsDetectedByDiff )
	    {
	      tmch -> init(ci,fd,human);
	      //tmch -> savePrevLoc();
	    }
	  else
	    {
	      //define how pan/tilt unit behaves
	      tmch -> calcMoveDist(&pan,&tilt,tool);	
	      cout<<"Pan  ="<<pan<<endl;
	      cout<<"Tilt ="<<tilt<<endl;

	      //move pan/tilt unit
	      ptu -> move( pan, tilt );
	      
	      //draw two circles(for hand and face)
	      cvCopy( ci-> getIntensityImg(), tmch -> interfaceImg, NULL);
	      cvCircle( tmch -> interfaceImg, tmch -> handCenterLoc, tmch -> handSize, CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	      cvCircle( tmch -> interfaceImg, tmch -> faceCenterLoc, tmch -> radius, CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	      //draw a line from region's center location to middle location between hand and face
	      cvLine( tmch -> interfaceImg, tmch -> imageCenterLoc, tmch -> midLocOfFaceAndHand, CV_RGB(255,255,255), 1, 8, 0 );

	      //update template image and set flag
	      tmch -> createTemplateImg( human -> getResult(), tmch -> handTemplateImg, &(tmch -> handCenterLoc ));
	      tmch -> createTemplateImg( ci -> getIntensityImg(), tmch -> faceTemplateImg, &(tmch -> faceCenterLoc ));
	      tmch -> updatedTamplateImg = true;	    
	    }

	  //show images
	  cvShowImage( "Match Result", tmch -> interfaceImg );
	  cvShowImage( "Destination Template Image", tmch -> handTemplateImg );
	  cvShowImage( "Face Template Image", tmch -> faceTemplateImg );
	  cvShowImage( "Binary Image", human -> getResult() );
	  cvShowImage( "Hand Diff Map Image", tmch -> getDiffMapImg( human -> getResult(), tmch -> handTemplateImg, tmch -> handDiffMapImg ) );
	  cvShowImage( "Face Diff Map Image", tmch -> getDiffMapImg( ci -> getIntensityImg(), tmch -> faceTemplateImg, tmch -> faceDiffMapImg ) );

	  //key handlling
	  key = cvWaitKey(10);
	  if( key == 'q' )
	    break;

	  //couut the number of frames 
	  frames++;
	  
	  //slide center location -4 pixels per four frames
	  tmch -> slideCentLoc( -4, 4 );

	  //save previous center locations
	  tmch -> savePrevLoc();
	  cout<<"PH="<<tmch -> handPrevCenterLoc.x<<","<<tmch->handPrevCenterLoc.y<<endl;
	  cout<<"PF="<<tmch -> facePrevCenterLoc.x<<","<<tmch->facePrevCenterLoc.y<<endl;

	  //save appropriate values for each center locaion(face and hand)
	  if( tmch -> handSimilarity > 90 )
	    tmch -> appropriateHandCenterLoc = tmch -> handCenterLoc;
	  if( tmch -> faceSimilarity > 90 )
	    tmch -> appropriateFaceCenterLoc = tmch -> faceCenterLoc;
  	
	}
    }
  
  //release memory and terminate app
  cvDestroyWindow( "Interface Image" );
  cvDestroyWindow( "Destination Template Image" );
  cvDestroyWindow( "Face Template Image" );
  cvDestroyWindow( "Match Result" );
  cvDestroyWindow( "Binary Image" );
  cvDestroyWindow( "Hand Diff Map Image" );
  cvDestroyWindow( "Face Diff Map Image" );

  delete ci;
  delete fd;
  delete human;
  delete tmch;
  delete ptu;
  delete tool;

  return 0;
}
