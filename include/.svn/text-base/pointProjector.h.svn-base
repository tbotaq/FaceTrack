// ~moriyama/projects/sr4000/turnk/include/pointProjector.h
// http://svn.xp-dev.com/svn/cou929_sakanelab/sr4000/trunk/include/pointProjector.h
//
// 2009-01-16 change name from 'projector' to 'pointProjector'
// 2008-12-17 add function 'convertOriginLeftTop'
// 2008-12-15 add parameters, backgroundColor and pointColor
// 2008-12-06
// Kousei MORIYAMA
//
// header file for pointProjector class
//

#ifndef __POINTPROJECTOR_H_
#define __POINTPROJECTOR_H_

#include <cstdio>
#include <algorithm>
#include <list>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

using namespace std;

namespace prj
{

void on_mouse_prj(int event, int x, int y, int flags, void *param);

class pointProjector
{
private:
  IplImage *img;               // display of projector
  int width;                   // display width
  int height;                  // display height
  int pointRadius;             // radius of point which projected
  int backgroundColor;         // background color
  int pointColor;              // point color
  list <CvPoint> pointList;    // list of points which is in the image

protected:
  char *windowName;           // display window name

public:
  pointProjector(int w, int h);
  ~pointProjector();
  enum COORDINATE_ORIGIN      // origin of coordinate
    {
      LEFT_TOP,
      CENTER
    };
  int drawPoint(CvPoint point);
  int drawPoint(int x, int y);
  int hidePoint(CvPoint point);
  int hidePoint(int x, int y);
  int show();
  list <CvPoint>::iterator getPosition(CvPoint point);
  list <CvPoint>::iterator getPosition(int x, int y);
  int hasPoint(CvPoint point);
  int hasPoint(int x, int y);
  int printPointCoordinate(enum COORDINATE_ORIGIN origin = LEFT_TOP);
  int saveFile(char *fileName);
  CvPoint convertOriginCenter(CvPoint src);
  CvPoint convertOriginLeftTop(CvPoint src);
};


class onePointProjector
{
private:
  IplImage *img;              // display of projector
  int width;                  // display width
  int height;                 // display height
  int pointRadius;            // radius of point which projected
  int backgroundColor;        // background color
  CvScalar pointColor;        // point color
  char *windowName;           // display window name
  CvPoint previous;           // previous point
  int show(void);
public:
  onePointProjector(int w, int h);
  enum COORDINATE_ORIGIN      // origin of coordinate
    {
      LEFT_TOP,
      CENTER
    };
  int drawPoint(CvPoint point);
  int hidePoint(CvPoint point);
  int showPoint(CvPoint point, enum COORDINATE_ORIGIN origin = CENTER);
  CvPoint convertOriginCenter(CvPoint src);
  CvPoint convertOriginLeftTop(CvPoint src);
};

}

#endif
