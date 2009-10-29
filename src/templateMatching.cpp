//Takuya Otsubo
#include<templateMatching.h>
#define THRESHOLD 50//threshold value for binarization
#define THRESHOLD_MAX_VALUE 255//max value for binarization
#define LINE_THICKNESS 1
#define LINE_TYPE 8
#define SHIFT 0

templateMatching::templateMatching()
{
  sourceBinaryImage = NULL;
  templateBinaryImage = NULL;
  differenceMapImage = NULL;
  minLocation = cvPoint(0, 0);
  scalar = cvScalar(0);
  key = 0;
  errorValue = 0;
}

templateMatching::~templateMatching()
{
  //memory allocation
  cvReleaseImage( &sourceBinaryImage );
  cvReleaseImage( &templateBinaryImage );
  cvReleaseImage( &differenceMapImage );
}

void templateMatching::setTempImage(IplImage *sourceImage,CvPoint *center,IplImage *templateImage)
{

  cvGetRectSubPix(sourceImage,templateImage,cvPointTo32f(*center));
  if(templateImage == NULL)
    cout<<"Failed in creating new template image."<<endl;
}

double templateMatching::getErrorValue()
{
  return errorValue;
}

void templateMatching::calcMatchResult(IplImage *sourceImage,IplImage *templateImage,CvSize srcSize,CvPoint *center,int *radius)//given sourceImage to process and returns center location and radius of detected face
{
  sourceBinaryImage   = cvCreateImage(srcSize,IPL_DEPTH_8U,1);
  templateBinaryImage = cvCreateImage(cvGetSize(templateImage),IPL_DEPTH_8U,1);
  differenceMapImage  = cvCreateImage(cvSize(srcSize.width - templateImage->width + 1,srcSize.height - templateImage->height + 1),IPL_DEPTH_32F,1);
 
  //Binarization
  //cvThreshold( sourceImage, sourceBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );
  //cvThreshold( templateImage, templateBinaryImage, THRESHOLD, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );
      
  //calculate the similarity by "SSD",which returns minimum value as most resembled value   
  cvMatchTemplate( sourceImage, templateImage, differenceMapImage, CV_TM_SQDIFF_NORMED );
  
  double a;
  //find the minimum-resembled point of differenceMapImage and write it to minLocation    
  cvMinMaxLoc( differenceMapImage, &errorValue, &a, &minLocation, NULL, NULL );

  cerr << "min, max: " << errorValue << ", " << a << ", " << abs((int)errorValue - (int)a) << endl;

  //calculate the center location and radius of detected face
  center->x = minLocation.x + templateImage->width/2;
  center->y = minLocation.y + templateImage->height/2;
  *radius   = max(templateImage->width/2,templateImage->height/2);
}


