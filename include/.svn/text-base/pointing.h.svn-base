// ~moriyama/projects/sr4000/trunk/include/pointing.h
// http://svn.xp-dev.com/svn/cou929_sakanelab/sr4000/trunk/include/pointing.h
// 
// 2009-01-16 add functon 'farther' and 'getIntersectionObjAndLine' for calcrurate intersection of pointing line and object.
// 2009-01-13 add line classes, 'line', 'line3D' and 'line3DCv'.
// 2009-01-06 fix constructor of 'regionDetector' and 'regionTracker'
// 2008-12-17 add functon 'checkCoordinateRange'
// 2008-12-15 add function 'on_mouse_getDepth'
// 2008-12-13 add macro 'WsumCal2' and funtion 'calth' for enhounced 'myOresen', by watabe
// 2008-12-09 clear functions and add 'calcDirectionVector', 'calcCoordinateOnPanel' and 'clipImage'
// 2008-12-04 add class 'regionDetector' and class 'regionTracker'
// 2008-12-02 add class 'imageClassifier'
// 2008-12-01 add namespace 'point', add class 'faceDetector'
// 2008-11-30
// Kosuei MORIYAMA
//
// header file for pointing system

#ifndef __POINTING_H_
#define __POINTING_H_

#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include "libusbSR.h"
#include "definesSR.h"

using namespace std;


namespace point
{

#define ptMax 10000     //リストに格納する点の数の上限
#define LINE3D_X 1
#define LINE3D_Y 2
#define LINE3D_Z 3
#define WINDOW_PX 40
#define WINDOW_PY 120
#define WINDOW_DX 50
#define WINDOW_DY 260

#define WsumCal(dx,dy,n) \
 pm[n] = dst->imageData[dst->widthStep * (dy) + (dx)]; \
 if (pm[n] == 255) { \
   Wsum++; \
   b1 = n; \
}

#define WsumCal2(dx,dy,n) \
 pm[n] = dst->imageData[dst->widthStep * (dy) + (dx)]; \
 if (pm[n] == 255) { \
   Wsum ++; \
   b1 = calth(n, preth, b1, &min_thdel, &yosouth);	\
}

#define ch_dxdy(delx,dely) \
  dx+=delx; \
  dy+=dely;




class cameraImages
{
private:
  SRCAM srCam;                // camera handler
  IplImage* cvDepth;          // contain depth image
  IplImage* cvInt;            // contain intensity image
  IplImage* cvConf;           // contain confidence map
  ImgEntry* imgEntryArr;      // image list of sr4000
  short* X;                   // X coordinates of each pixel
  short* Y;                   // X coordinates of each pixel
  unsigned short int* Z;      // X coordinates of each pixel
  int ampImgThreshold;        // threshold of amplitude image. cut values which is above this threshold. nice value is 3000
  int width;
  int height;
  int pich;
  int checkCoordinateRange(int x, int y)
    {
      if(x >= 0 && y >= 0 && x < width && y < height)
        return 0;
      else 
        return -1;
    }

public:
  cameraImages();
  ~cameraImages();
  int initialize();
  int acquire();
  IplImage* getDepthImg();
  IplImage* getIntensityImg();
  IplImage* getConfidenceMap();
  int getIntensityVal(int x, int y);
  CvPoint3D32f getCoordinate(int x, int y);
  CvPoint3D32f getCoordinate(CvPoint point);
  int getConfidenceVal(int x, int y);
  int setAmpImgThreshold(int th);
  CvSize getImageSize();
};

class faceDetector
{
private:
  char* cascadeName;
  CvHaarClassifierCascade* cascade;
  CvMemStorage* storage;
  IplImage *smallImg;
  double scale;

public:
  faceDetector(CvSize size);
  ~faceDetector();
  int setCascadeName(char *name);
  int faceDetect( IplImage* img, CvPoint *center, int *radius );
};

class imageClassifier
{
private:
  const char NONE;
  const char LEFT;
  const char RIGHT;
  const char TOP;
  const char BOTTOM;
  int WHITE;
  int LABEL_MAX;
  IplImage *classImg;
  int label;
  int classify(IplImage *src);
  int labelling(int x, int y, int direction);
  int labelIncrement();
  IplImage* getRegionByLabel(int labelNumber);
  IplImage* getRegionByCoordinate(int x, int y);

public:
  imageClassifier();
  ~imageClassifier();
  IplImage* getRegionImg(IplImage *src, int x, int y);
};

class regionDetector
{
private:
  const char NONE;        // represent traverse direction
  const char LEFT;        // represent traverse direction
  const char RIGHT;       // represent traverse direction
  const char TOP;         // represent traverse direction
  const char BOTTOM;      // represent traverse direction
  const int WHITE;        // value of white pixel (for binary image)
  IplImage *original;     // original image
  IplImage *result;       // result of process
  int threshold;          // threshold of gradient
  IplConvKernel* element; // for morphology calcuration
  int traverse(int x, int y, int direction);
  int calcGradient(CvPoint arg1, CvPoint arg2);

public:
  regionDetector(CvSize size);
  ~regionDetector();
  int getRegion(IplImage *src, int x, int y, IplImage *dst);
  int setThreshold(int th);
};

class regionTracker
{
private:
  cameraImages *cm;           // camera image handling
  char initializeFlag;        // normrly 1, if detector lost the subject, set to 0 and re-initialize
  CvPoint centroid;           // centroid of subject region (previous frame)
  int centroidDepth;          // depth value of centroid pixel (previous frame)
  int area;                   // total pixel in the region
  IplImage *result;           // tracking result
  IplImage *contractedResult; // this is used in centroid calcuration process
  IplConvKernel* element;     // for morphology calcuration
  regionDetector *human;      // class 'regionDetector'
  IplImage *intensity;        // intensity iamge
  IplImage *depth;            // depth image
  faceDetector *fd;           // class 'faceDetector'
  int initialize();
  int trackRegion();
  int calcCentroidAndArea();
public:
  regionTracker(cameraImages *cam);
  ~regionTracker();
  int track();
  IplImage* getResult();
};

template <typename T> class line
{
protected:
  T point;

public:
  T directionVecror;
  virtual ~line(void){};
  T getPoint(void) {return point;}
  T getDirectionVector(void) {return directionVecror;}
  virtual int setLine(T point1, T point2) = 0;
};

template <typename T3D, typename Tscolar> class line3D : public line<T3D>
{
public:
  virtual T3D getLinePointByZ(Tscolar v) = 0;
};

class line3DCv : public line3D <CvPoint3D32f, double>
{
private:
  int flg;
public:
  line3DCv(void);
  int init(void);
  int setLine(CvPoint3D32f point1, CvPoint3D32f point2);
  CvPoint3D32f getLinePointByZ(double v);
  bool isValid(void);
};


int getArmImage(const IplImage *src, IplImage *dst);
int getArmPoints(IplImage *src, CvPoint *dstFinger, CvPoint *dstElbow);
CvPoint detectFingertip(IplImage *body, CvPoint candidate1, CvPoint candidate2);
int countAround(IplImage *img, CvPoint point, int range);
void myThinningInit(CvMat** kpw, CvMat** kpb);
IplImage* myTinning(IplImage* src);
IplImage* myOresen(IplImage* src , CvPoint* Ap);
int calth(int _n, int _preth,int _b1, int* min_thdel_ad, int* yosouth_ad);
CvPoint DecideKakupt(CvPoint _pt[] , int _ptNow);
void on_mouse_pointing(int event, int x, int y, int flags, void *param);
void on_mouse_getDepth(int event, int x, int y, int flags, void *param);
int calcDirectionVector( CvPoint3D32f  _P , CvPoint3D32f _Q , CvPoint3D32f* _d);
CvPoint3D32f calcCoordinateOnPanel(int panelDistance, CvPoint3D32f startPoint, CvPoint3D32f directionVec);
int clipImage(IplImage *img, CvPoint origin, int width, int height, char* fileName);
bool farther(CvPoint3D32f p1, CvPoint3D32f p2);
CvPoint3D32f getIntersectionObjAndLine(line3DCv *line, cameraImages *ci);
CvPoint3D32f calcCoordFromZ(int z, CvPoint3D32f startPoint, CvPoint3D32f directionVec);
double getError(CvPoint3D32f v1, CvPoint3D32f v2);
CvPoint3D32f getMarkCoord(line3DCv *line, cameraImages *ci, double threshold = 100);

}

#endif
