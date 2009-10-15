#include<templateMatching.h>
#define THRESHOLD 50
#define THRESHOLD_MAX_VALUE 255
#define LINE_THICKNESS 1
#define LINE_TYPE 8
#define SHIFT 0

templateMatching::templateMatching()
{
  templateImage = cvLoadImage("todai_boys_small.jpg",1);
}

templateMatching::~templateMatching()
{
  cvReleaseImage( &sourceImage );
  cvReleaseImage( &templateImage );
  cvReleaseImage( &sourceGrayImage );
  cvReleaseImage( &templateGrayImage );
  cvReleaseImage( &sourceBinaryImage );
  cvReleaseImage( &templateBinaryImage );
  cvReleaseImage( &differenceMapImage );
} 

int templateMatching::calcMatchResult( IplImage *sourceImage, CvPoint *center, int *radius)//this function shoud be given sourceImage to process and returns center location of detected face
{
  CvSize srcimagesize = cvSize( sourceImage->width, sourceImage->height);
  //memory allocation
  sourceGrayImage = cvCreateImage(srcimagesize,IPL_DEPTH_8U,1);
  templateGrayImage = cvCreateImage(cvGetSize(templateImage),IPL_DEPTH_8U,1);
  sourceBinaryImage = cvCreateImage(srcimagesize,IPL_DEPTH_8U,1);
  templateBinaryImage = cvCreateImage(cvGetSize(templateImage),IPL_DEPTH_8U,1);
  differenceMapImage = cvCreateImage(cvSize(srcimagesize.width - templateImage->width +1,srcimagesize.height -templateImage->height +1),IPL_DEPTH_32F,1);
  sourceGrayImage = sourceImage;
  cvCvtColor( templateImage, templateGrayImage, CV_BGR2GRAY );

  //Binarization
  cvThreshold( sourceGrayImage, sourceBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );
  cvThreshold( templateGrayImage, templateBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );
      
  //here goes Template Match    
  cvMatchTemplate( sourceBinaryImage, templateBinaryImage, differenceMapImage, CV_TM_SQDIFF );
  
  //find most-matched point and returns it as minLocation    
  cvMinMaxLoc( differenceMapImage, NULL, NULL, &minLocation, NULL, NULL );

  //calculate the center location
  center->x = minLocation.x + templateImage->width/2;
  center->y = minLocation.y + templateImage->height/2;
  *radius = max(templateImage->width/2,templateImage->height/2);
  return 0;

}
  
