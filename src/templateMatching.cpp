//Takuya Otsubo
#include<templateMatching.h>
#define THRESHOLD 50//threshold value for binarization
#define THRESHOLD_MAX_VALUE 255//max value for binarization
#define LINE_THICKNESS 1
#define LINE_TYPE 8
#define SHIFT 0

templateMatching::templateMatching()
{
  differenceMapImg = NULL;
  minLocation = cvPoint( 0, 0 );
  scalar = cvScalar( 0 );
  key = 0;
  errorValue = 0;
}

templateMatching::~templateMatching()
{
  cvReleaseImage( &differenceMapImg );
}



void templateMatching::createTemplateImg( IplImage *sourceImg, IplImage *templateImg, CvPoint templateCenterLoc )
{
  cvGetRectSubPix( sourceImg, templateImg, cvPointTo32f( templateCenterLoc ) );
}

void templateMatching::calcMatchResult( IplImage *sourceImg, IplImage *templateImg, CvSize srcSize, CvPoint center, int radius )
{
  differenceMapImg  = cvCreateImage( cvSize( srcSize.width - templateImg -> width + 1, srcSize.height - templateImg -> height + 1 ),IPL_DEPTH_32F,1 );

  //calculate the similarity by "SSD",which returns minimum value as most matching-value   
  cvMatchTemplate( sourceImg, templateImg, differenceMapImg, CV_TM_SQDIFF_NORMED );
  
  //find the minimum-resembled point of differenceMapImage and write it to minLocation    
  cvMinMaxLoc( differenceMapImg, &errorValue, NULL, &minLocation, NULL, NULL );
 
  cout << "\tSimilarity[%]= "<< ( 1 - errorValue ) * 100 <<endl;
 
  //calculate the center location and radius of detected face
  center.x = minLocation.x + templateImg -> width / 2;
  center.y = minLocation.y + templateImg -> height / 2;

  radius = max(templateImg->width/2,templateImg->height/2);
}

double templateMatching::getErrorValue()
{
  return errorValue;
}

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



