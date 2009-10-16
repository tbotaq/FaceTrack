#ifndef _TEMPLATE_MATCHING_H_
#define _TEMPLATE_MATCHING_H_

#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include <stdio.h>
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"
#include "mathfuncs.h"

using namespace std;
using namespace point;

class templateMatching
{
 private:
  //char windowNameTemplate[] = "Template";
  //char windowNameDestination[] = "Destination";
  IplImage *sourceImage;
  IplImage *templateImage;
  IplImage *sourceGrayImage;
  IplImage *templateGrayImage;
  IplImage *sourceBinaryImage;
  IplImage *templateBinaryImage;
  IplImage *differenceMapImage;
  CvPoint minLocation;
  CvScalar scalar;
  
  int key;
 public:
  templateMatching(CvSize srcSize);
  ~templateMatching();
  int calcMatchResult(IplImage *sourceImage,CvPoint *center,int *radius);
};
#endif
