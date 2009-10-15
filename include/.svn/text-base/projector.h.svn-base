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
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

using namespace std;

namespace prj
{

void on_mouse(int event, int x, int y, int flags, void *param);

class projector
{
private:
  IplImage *img;              // display of projector
  int width;                  // display width
  int height;                 // display height
  int pointRadius;            // radius of point which projected
  int backgroundColor;        // background color
  CvScalar pointColor;        // point color
  char *windowName;           // display window name
  CvPoint *pointArray;        // pointer to point coordinates (array of CvPoint)
  int pointIndex;             // index of point array
  const int POINT_ARRAY_MAX;  // maximum number of point array index
  const char NO_POINT;        // label for hidden point

public:
  projector(int w, int h);
  ~projector();
  enum COORDINATE_ORIGIN      // origin of coordinate
    {
      LEFT_TOP,
      CENTER
    };
  int drawPoint(int x, int y);
  int hidePoint(int s, int y);
  int hidePoint(int index);
  int show();
  int isPoint(int x, int y);
  int printPointCoordinate(enum COORDINATE_ORIGIN origin = LEFT_TOP);
  int saveFile(char *fileName);
  CvPoint convertOriginCenter(CvPoint src);
  CvPoint convertOriginLeftTop(CvPoint src);
};

}

#endif
