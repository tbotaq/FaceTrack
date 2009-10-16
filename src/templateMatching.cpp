#include<templateMatching.h>
#define THRESHOLD 50//threshold value for binarization
#define THRESHOLD_MAX_VALUE 255//max value for binarization
#define LINE_THICKNESS 1
#define LINE_TYPE 8
#define SHIFT 0

templateMatching::templateMatching(CvSize srcSize)
{
  CvSize srcImageSize = srcSize;
  templateImage       = cvLoadImage("todai_boys_small.jpg",1); 
  //memory allocation
  templateGrayImage   = cvCreateImage(cvGetSize(templateImage),IPL_DEPTH_8U,1);
  sourceBinaryImage   = cvCreateImage(srcImageSize,IPL_DEPTH_8U,1);
  templateBinaryImage = cvCreateImage(cvGetSize(templateImage),IPL_DEPTH_8U,1);
  differenceMapImage  = cvCreateImage(cvSize(srcImageSize.width - templateImage->width + 1,srcImageSize.height - templateImage->height + 1),IPL_DEPTH_32F,1);
}

templateMatching::~templateMatching()
{
  cvReleaseImage( &templateGrayImage );
  cvReleaseImage( &sourceBinaryImage );
  cvReleaseImage( &templateBinaryImage );
  cvReleaseImage( &differenceMapImage );
} 

int templateMatching::calcMatchResult( IplImage *sourceImage, CvPoint *center, int *radius)//this function shoud be given sourceImage to process and returns center location and radius of detected face
{
  //convertion from BGR to gray scale for source and template image
  cvCvtColor( templateImage, templateGrayImage, CV_BGR2GRAY );

  //Binarization
  cvThreshold( sourceImage, sourceBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );
  cvThreshold( templateGrayImage, templateBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );
      
  //calculate the similarity by "SSD",which returns minimum value as most resembled value   
  cvMatchTemplate( sourceBinaryImage, templateBinaryImage, differenceMapImage, CV_TM_SQDIFF );
  
  //find the minimum-resembled point of differenceMapImage and write it to minLocation    
  cvMinMaxLoc( differenceMapImage, NULL, NULL, &minLocation, NULL, NULL );

  //calculate the center location and radius of detected face
  center->x = minLocation.x + templateImage->width/2;
  center->y = minLocation.y + templateImage->height/2;
  *radius   = max(templateImage->width/2,templateImage->height/2);
  return 0;
}
  
