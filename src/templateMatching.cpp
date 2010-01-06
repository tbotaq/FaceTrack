#include<templateMatching.h>
#define HAND_WIDTH 80
#define HAND_HEIGHT 75
#define DST_THRESHOLD 70
#define FACE_THRESHOLD 40

//---------------------------------------------
templateMatching::templateMatching(cameraImages *ci)
{
  imageSize = ci->getImageSize();

  imageCenterLoc = cvPoint( imageSize.width / 2, imageSize.height / 2 );
  handWidth = HAND_WIDTH;
  handHeight = HAND_HEIGHT;
  tempPtCenter = cvPoint( imageCenterLoc.x + 25, imageCenterLoc.y - 25 );
  tempPt1 = cvPoint( tempPtCenter.x - handWidth / 2, tempPtCenter.y - handHeight / 2 );
  tempPt2 = cvPoint( tempPtCenter.x + handWidth / 2, tempPtCenter.y + handHeight / 2 );

  differenceMapImg = NULL;
  minLocation = cvPoint( 0, 0 );
  scalar = cvScalar( 0 );
  key = 0;
  frames = 0;
  errorValue = 0;

  errorIsDetectedByDiff = false;
  errorIsDetectedBySimi = false;

}

//---------------------------------------------
templateMatching::~templateMatching()
{
  cvReleaseImage( &handTemplateImg );
  cvReleaseImage( &faceTemplateImg );
  cvReleaseImage( &differenceMapImg );
  cvReleaseImage( &diffMapImg );
}

//---------------------------------------------
bool templateMatching::init(cameraImages *ci, faceDetector *fd, regionTracker *human )
{

  //create template image to use in the first
  //return true when done 

  hasBeenInitialized = false;

  //create template images
  cout<<"Press 't' key to create template image"<<endl;
  
  while( !hasBeenInitialized )
    {
      //acquire current frame
      ci -> acquire();
      interfaceImg = ci -> getIntensityImg();

      //detect human's face using ada-boost
      fd -> faceDetect( interfaceImg, &faceCenterLoc, &radius );

      //draw a rectangle representing the region to be template image
      cvRectangle( interfaceImg, tempPt1, tempPt2, CV_RGB( 255, 255, 255 ), 2, 8, 0 );
      cvCircle( interfaceImg, faceCenterLoc, radius+10, CV_RGB( 255, 255, 255 ), 2, 8, 0 );
      cvShowImage("SET YOUR HAND", interfaceImg );

      key = cvWaitKey( 10 );

      if( human -> track() == 0)
	{
	  cvShowImage( "Binary Image", human -> getResult() ); 
	  if( key == 't' )
	    {
	      //setting the size of template image
	      handTemplateImg = cvCreateImage( cvSize( handWidth, handHeight ), IPL_DEPTH_8U, 1 );
	      faceTemplateImg = cvCreateImage( cvSize( radius*2+10, radius*2+10 ), IPL_DEPTH_8U, 1 );
	  
	      //create template images
	      this -> createTemplateImg( human -> getResult(), handTemplateImg, &tempPtCenter );
	      this -> createTemplateImg( human -> getResult(), faceTemplateImg, &faceCenterLoc );

	      cvShowImage( "Destination Template Image", handTemplateImg );
	      cvShowImage( "Face Template Image", faceTemplateImg );
	    
	      cout<<"Created temlate image is shown.OK?(y or n)"<<endl;

	      key = cvWaitKey( 0 );

	      if( key == 'y' )
		hasBeenInitialized = true;
	    }
	}
    }
  return true;
}

//-----------------------------------------
/*
string templateMatching::getErrorIncident()
{
  //returns error incident in string  

  string d,s,n,ret;
  ret = 'Error incident is';
  d = 'DIFFERENCE TOO LARGE';
  s = 'SIMILALITY TOO LOW';
  n = 'NO ERROR DETECTED';
  
  if(errorIsDetectedByDiff)
    ret.push_back(d);
  if(errorIsDetectedBySimi)
    ret.push_back(s);
  if( !errorIsDetectedByDiff && !errorIsDetectedBySimi )
    ret.push_back(n);
  return ret;
}
*/

 //--------------------------------------------
void templateMatching::checkErrorByDiff(int diff_threshold)
{
  //error handlling by 'difference'
  //id error is detected,call previous location as current locaion.
  //set flags with checking result(true or false)

  this -> calcLocDifference();

  if( updatedTamplateImg )
    {
      if( diffHandCenterLocX > diff_threshold || diffHandCenterLocY > diff_threshold )
	{
	  cout<<"### Detected Abnormal Value in Hand Tracking."<<endl;
	  handCenterLoc = handPrevCenterLoc;
	  errorIsDetectedByDiff = true;
	}
      else errorIsDetectedByDiff = false;

      if( diffFaceCenterLocX > diff_threshold || diffFaceCenterLocY > diff_threshold )
	{
	  cout<<"### Detected Abnormal Value in Face Tracking."<<endl;
	  faceCenterLoc = facePrevCenterLoc;
	  errorIsDetectedByDiff = true;
	}
      else errorIsDetectedByDiff = false;
    }
}

//------------------------------------------------
void templateMatching::checkErrorBySimi(int simi_threshold)
{

  //error checking by 'similality'
  if(updatedTamplateImg)//if it is not first loop
    {
      if( faceSimilarity < simi_threshold )
	{
	  cout<<"### Face's similality is too low."<<endl;
	  cout<<"Similality:"<<faceSimilarity<<endl;
	  errorIsDetectedBySimi = true;
	}

      else 
	{
	  cout<<"Similality(face) :"<<faceSimilarity<<endl;
	  errorIsDetectedBySimi = false;
	}

      if( handSimilarity < simi_threshold )
	{
	  cout<<"### Hand's similality is too low"<<endl;
	  cout<<"Similality:"<<handSimilarity<<endl;
	  errorIsDetectedBySimi = true;
	}
      else 
	{
	  cout<<"Similality(hand) :"<<faceSimilarity<<","<<handSimilarity<<endl;
	  errorIsDetectedBySimi = false;
	}
    }
}

//---------------------------------------------
void templateMatching::createTemplateImg( IplImage *sourceImg, IplImage *templateImg, CvPoint *templateCenterLoc )
{
  cvGetRectSubPix( sourceImg, templateImg, cvPointTo32f( *templateCenterLoc ) );
}

//---------------------------------------------
void templateMatching::calcMatchResult( IplImage *sourceImg, IplImage *templateImg, CvSize srcSize, CvPoint *center, int *radius )
{
  differenceMapImg  = cvCreateImage( cvSize( srcSize.width - templateImg -> width + 1, srcSize.height - templateImg -> height + 1 ),IPL_DEPTH_32F,1 );
  
  //calculate the similarity by "SSD",which returns minimum value as most matching-value   
  cvMatchTemplate( sourceImg, templateImg, differenceMapImg, CV_TM_SQDIFF_NORMED );
  
  //find the minimum-resembled point of differenceMapImage and write it to minLocation    
  cvMinMaxLoc( differenceMapImg, &errorValue, NULL, &minLocation, NULL, NULL );
  
  //calculate the center location and radius of detected face
  center->x = minLocation.x + templateImg -> width / 2;
  center->y = minLocation.y + templateImg -> height / 2;
  *radius = max(templateImg->width/2,templateImg->height/2);
}

//---------------------------------------------
IplImage *templateMatching::getDiffMapImg()
{
  return differenceMapImg;
}

//---------------------------------------------
IplImage *templateMatching::getDiffMapImg( IplImage *sourceImg, IplImage *templateImg, IplImage *diffMapImg )
{
  diffMapImg  = cvCreateImage( cvSize( sourceImg -> width - templateImg -> width + 1, sourceImg -> height - templateImg -> height + 1 ), IPL_DEPTH_32F, 1 );
  cvMatchTemplate( sourceImg, templateImg, diffMapImg, CV_TM_SQDIFF_NORMED );
  return diffMapImg;
}

//---------------------------------------------
double templateMatching::getErrorValue()
{
  return errorValue;
}

//---------------------------------------------
int templateMatching::getAvgDepth( IplImage *humanImage, IplImage *depthImage )
{
  int sum = 0;
  CvScalar humanValue = cvScalar( 0 );
  CvScalar depthValue = cvScalar( 0 );
  int points = 0;
  int ret = 0;
  
  for( int i=0; i < 144; i++ )
    for( int j = 0; j < 176; j++ )
      {
	humanValue = cvGet2D( humanImage, i, j ); 
	depthValue = cvGet2D( depthImage, i, j );
	
	if( humanValue.val[0] != 0 )
	  {
	    sum += depthValue.val[0];
	    points++;
	  }
      }

  if( sum != 0 )
    {
      ret = sum / points;
      return ret;
    }
  else
    return -1;
}

//---------------------------------------------
int templateMatching::resizeBinarizedImg( IplImage *binarizedImg )
{
  CvScalar currentValue = cvScalar( 0 ), prevCurrentValue = cvScalar( 0 );
  bool findFirstWhite = false;
  int minX=200,maxX,minY;
  
  for( int i=0; i < binarizedImg -> height; i++ )
    for( int j = 0; j < binarizedImg -> width; j++ )
      {
	if(j!=0)
	  prevCurrentValue = currentValue;
	currentValue = cvGet2D( binarizedImg, i, j ); 
	if( currentValue.val[0] == 1 )
	  {
	    if( !findFirstWhite )
	      {
		minY = i;
		findFirstWhite = true;
	      }
	    if( minX > j )
	      minX = j;
	    if( maxX < j )
	      maxX = j;
	  }
      }

  cout<<"minX,maxX,minY ="<<minX<<","<<maxX<<","<<minY<<endl;

  return 0;
}

//---------------------------------------------
double templateMatching::getSimilarity()
{
  return ( (1 - errorValue ) * 100 );
}

//----------------------------------------------
void templateMatching::calcLocDifference()
{
  //calculate difference between current and previous for hand and face center location
  diffHandCenterLocX = abs( handCenterLoc.x - handPrevCenterLoc.x );
  diffHandCenterLocY = abs( handCenterLoc.y - handPrevCenterLoc.y );

  diffFaceCenterLocX = abs( faceCenterLoc.x - facePrevCenterLoc.x );
  diffFaceCenterLocY = abs( faceCenterLoc.y - facePrevCenterLoc.y );
}

//-----------------------------------------------
void templateMatching::slideCentLoc(int slideVal,int perThisFrame)
{
  //force handle with ???
  //unknown phenomenon handling
  if( !outOfRegion && frames % perThisFrame == 0 )
    {
      handCenterLoc.x += slideVal;
      handCenterLoc.y += slideVal;
      faceCenterLoc.x += slideVal;
      faceCenterLoc.y += slideVal;
    }
}

//-----------------------------------------------
void templateMatching::savePrevLoc()
{
  //save previous values
  handPrevCenterLoc = handCenterLoc;
  facePrevCenterLoc = faceCenterLoc;
}

//------------------------------------------------
void templateMatching::calcMoveDist(double *pan,double *tilt,tools *tool)
{
  if( outOfRegion )
    {
      //if the hand or face is out of region,set pan/tilt 0 in order not to move
      cout<<"#### Objects are expected to be out of region."<<endl;
      *pan = 0;
      *tilt = 0;
    }
  else
    {
      //if both objects are in the region,calculate middle location between hand and face and set pan/tilt value
      midLocOfFaceAndHand = cvPoint( ( handCenterLoc.x + faceCenterLoc.x ) / 2, ( handCenterLoc.y + faceCenterLoc.y ) / 2 );
      *pan = tool -> getMoveDist( midLocOfFaceAndHand.x, imageCenterLoc.x );
      *tilt = tool -> getMoveDist( midLocOfFaceAndHand.y, imageCenterLoc.y );
    }
}

