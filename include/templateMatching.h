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
  CvPoint minLocation;
  CvScalar scalar;
  int key;
  double errorValue;
  IplImage *templateImg;
  IplImage *differenceMapImg;
 public:
  templateMatching();
  ~templateMatching();
  void setTempImg(IplImage *sourceImg,IplImage *templaeImg,CvPoint center);
  void createTemplateImg(IplImage *sourceImg,IplImage *templateImg,CvPoint *templateCenterLoc);
  void calcMatchResult(IplImage *sourceImg,IplImage *templateImg,CvSize srcSize,CvPoint *center,int *radius);
  double getErrorValue();
  int getAvgDepth(IplImage *humanImg,IplImage *depthImg);
 
};
#endif
