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
#include "tools.h"
using namespace std;
using namespace point;

class templateMatching
{
 private:

 public:


  //flags  
  bool errorIsDetectedByDiff;
  bool errorIsDetectedBySimi;
  bool hasBeenInitialized;
  bool updatedTamplateImg;
  bool outOfRegion;

  //size of destination and face(radius)
  int handSize,radius;

  //int key;
  int frames;
  CvSize imageSize;

  //images
  IplImage *interfaceImg,*handTemplateImg,*faceTemplateImg,*handDiffMapImg,*faceDiffMapImg,*faceSourceImg;

  //locations of face and hand
  CvPoint handCenterLoc,faceCenterLoc;
  CvPoint handPrevCenterLoc,facePrevCenterLoc;
  CvPoint appropriateHandCenterLoc,appropriateFaceCenterLoc;
  CvPoint midLocOfFaceAndHand;

  //for key handlling
  int key;

  //differences
  int diffHandCenterLocX,diffHandCenterLocY,diffFaceCenterLocX ,diffFaceCenterLocY;

  //used in creating template image.
  CvPoint imageCenterLoc;
  CvPoint tempPt1,tempPt2,tempPtCenter;
  int handWidth,handHeight;

  //error value and similality in template matching
  double currentHandErrorValue,currentFaceErrorValue;
  double handSimilarity,faceSimilarity;

  CvPoint minLocation;
  CvScalar scalar;
  
  double errorValue;
  IplImage *templateImg;
  IplImage *differenceMapImg,*diffMapImg;

  templateMatching(cameraImages *ci);
  ~templateMatching();
  void setTempImg(IplImage *sourceImg,IplImage *templaeImg,CvPoint center);
  void createTemplateImg(IplImage *sourceImg,IplImage *templateImg,CvPoint *templateCenterLoc);
  void calcMatchResult(IplImage *sourceImg,IplImage *templateImg,CvSize srcSize,CvPoint *center,int *radius);
  double getErrorValue();
  int getAvgDepth(IplImage *humanImg,IplImage *depthImg);
  double getSimilarity();
  IplImage *getDiffMapImg();
  IplImage *getDiffMapImg(IplImage *sourceImg,IplImage *templateImg,IplImage *diffMapImg);
  int resizeBinarizedImg(IplImage *binarizedImg);
  void calcMoveDist(double *pan,double *tilt,tools *tool);
  void savePrevLoc();
  void slideCentLoc(int handSlideVal,int faceSlideVal,int perThisFrame);
  void calcLocDifference();
  void checkErrorByDiff(int diff_threshold);
  void checkErrorBySimi(int simi_threshold);
  bool init(cameraImages *ci,faceDetector *fd, regionTracker *human);
};
#endif
