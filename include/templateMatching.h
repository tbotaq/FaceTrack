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
  IplImage *sourceBinaryImage;
  IplImage *templateBinaryImage;
  IplImage *differenceMapImage;
  CvPoint minLocation;
  CvScalar scalar;
  int key;
  double errorValue;
 public:
  templateMatching();
  ~templateMatching();
  void setTempImage(IplImage *sourceImage,CvPoint *center,IplImage *templateImage);
  void calcMatchResult(IplImage *sourceImage,IplImage *templateImage,CvSize srcSize,CvPoint *center,int *radius);
  double getErrorValue();
};
#endif
