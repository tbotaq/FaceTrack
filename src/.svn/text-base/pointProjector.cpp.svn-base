// ~moriyama/projects/sr4000/trunk/src/pointProjector.cpp
// http://svn.xp-dev.com/svn/cou929_sakanelab/sr4000/trunk/src/pointProjector.cpp
//
// 2009-01-16 change name from 'projector' to 'pointProjector'
// 2009-01-10 fix function 'convertOriginLeftTop', a sign was inversed.
// 2008-12-17 add function 'convertOriginLeftTop'
// 2008-12-16 add pointIndex process in 'drawPoint'
// 2008-12-15 add parameters, backgroundColor and pointColor
// 2008-12-06
// Kousei MORIYAMA
//
// projector class


#include "pointProjector.h"

using namespace std;

namespace prj
{

pointProjector::pointProjector(int w, int h)
{
  img = cvCreateImage(cvSize(w, h),IPL_DEPTH_8U,1);
  width = w;
  height = h;
  pointRadius = 15;
  backgroundColor = 0; // black
  pointColor = 255;
  windowName = "Projector";

  // Make a window
  cvNamedWindow(windowName, CV_WINDOW_AUTOSIZE);

  // make background image
  cvSet(img, cvScalarAll(backgroundColor));
}

pointProjector::~pointProjector()
{
  cvReleaseImage(&img);
  cvDestroyWindow(windowName);
}

int pointProjector::drawPoint(int x, int y)
{
  // input: x and y coordinate, int
  // output: draw a mark
  // return: 0
  //
  // Draw a mark at point (x, y).

  return drawPoint(cvPoint(x, y));
}

int pointProjector::drawPoint(CvPoint point)
{
  // input: x and y coordinate, CvPoint
  // output: draw a mark
  // return: 0
  //
  // Draw a mark at argument point.

  // escape if x and y is out of image
  if (point.x < 0 || point.x >= width || point.y < 0 || point.y >= height)
    return -1;

  pointList.push_front(point);

  cvCircle(img, point, pointRadius, cvScalar(pointColor), -1, 8, 0);

  return 0;
}

int pointProjector::hidePoint(int x, int y)
{
  // input: x and y coordinate, int
  // output: hide drawn point
  // return: 0
  //
  // Hide drawn point which coordinate if (x, y).

  return hidePoint(cvPoint(x, y));
}

int pointProjector::hidePoint(CvPoint point)
{
  // input: x and y coordinate, CvPoint
  // output: hide drawn point
  // return: 0
  //
  // Hide point which coordinate is 'point'.

  list <CvPoint>::iterator it;

  it = getPosition(point);
  if (it == pointList.end())
    return -1;

  // hide point on image and erase point on list
  cvCircle(img, *it, pointRadius, cvScalarAll(backgroundColor), -1, 8, 0);
  pointList.erase(it);
  
  return 0;
}

int pointProjector::show()
{
  // input: none
  // output: display image
  // return: 0
  //
  // display image

  cvShowImage(windowName, img);

  return 0;
}

list <CvPoint>::iterator pointProjector::getPosition(CvPoint point)
{
  // input: coordinate, CvPoint
  // output: none
  // return: list <CvPoint>::iterator
  //
  // If pointList has'point', returns iterator which points that,
  // else returns itarator which points end of list.

  list <CvPoint>::iterator it;
  double xDistance, yDistance;
  double squaredRadius = pointRadius*pointRadius;

  // check x and y range
  if (point.x < 0 || point.x >= width || point.y < 0 || point.y >= height)
    return pointList.end();

  // search point
  for (it=pointList.begin(); it!=pointList.end(); it++)
    {
      xDistance = ((*it).x - point.x) * ((*it).x - point.x);
      yDistance = ((*it).y - point.y) * ((*it).y - point.y);

      if (xDistance < squaredRadius && yDistance < squaredRadius )
	break;
    }

  return it;
}

list <CvPoint>::iterator pointProjector::getPosition(int x, int y)
{
  return getPosition(cvPoint(x, y));
}

int pointProjector::hasPoint(CvPoint point)
{
  // input: coordinate, CvPoint
  // output: none
  // return: 1 or 0
  //
  // If pointList has'point', returns 1, else 0.

  list <CvPoint>::iterator it;
  int ret = 0;

  it = getPosition(point);

  if (it != pointList.end())
    ret = 1;

  return ret;
}

int pointProjector::hasPoint(int x, int y)
{
  return hasPoint(cvPoint(x, y));
}

int pointProjector::printPointCoordinate(enum COORDINATE_ORIGIN origin)
{
  // input: none
  // output: print coordinates that is contained in the 'pointList'
  // return: 0
  //
  // Print coordinates that is contained in the 'pointList'.

  list <CvPoint>::iterator it;
  CvPoint result;

  for (it=pointList.begin(); it!=pointList.end(); it++)
    {
      result = *it;
      if (origin == CENTER)
	result = convertOriginCenter(result);
      fprintf(stdout, "%d, %d\n", result.x, result.y);
    }

  return 0;
}

int pointProjector::saveFile(char *fileName)
{
  // input: filename, char*
  // output: save 'pointList' coordinates to the file
  // return: 0
  //
  // Save 'pointList' coordinates to the file

  FILE *fp;
  list <CvPoint>::iterator it;

  fp = fopen(fileName, "w");
  if(fp == NULL)
    {
      fprintf(stderr, "ERROR: file \"%s\"cannot open\n", fileName);
      return -1;
    }

  for (it=pointList.begin(); it!=pointList.end(); it++)
    fprintf(fp, "%d,%d\n", (*it).x, (*it).y);

  fclose(fp);

  return 0;
}

CvPoint pointProjector::convertOriginCenter(CvPoint src)
{
  // input: x and y coordinates, CvPoint
  // output: none
  // return: x and y coordinates, CvPoint
  //
  // Convert input coordinate which origin is left top of image
  // to the coordinate which origin is center of image.
  // Direction of enter origin coordinate is that, x axis is left to right
  // and y axis if bottom to top.

  CvPoint dst;

  dst.x = src.x - width / 2;
  dst.y = src.y*(-1) + height/2;

  return dst;
}

CvPoint pointProjector::convertOriginLeftTop(CvPoint src)
{
  // input: x and y coordinates, CvPoint
  // output: none
  // return: x and y coordinates, CvPoint
  //
  // Convert input coordinate which origin is center of image
  // to the coordinate which origin is left top of image.
  // Direction of enter origin coordinate is that, x axis is left to right
  // and y axis if bottom to top.
  // This function is inverse of convertOriginCenter().

  CvPoint dst;

  dst.x = src.x + width / 2;
  dst.y = src.y*(-1) + height/2;

  return dst;
}

void on_mouse_prj(int event, int x, int y, int flags, void *param)
{
  // input: void *param is pointer to class 'pointProjector'
  //        left button mouse click
  //        right button mouse click
  // output: draw or hide circle on display when left button clicked
  //         show clicked coordinate when right button clicked
  // return: none
  //
  // Callback function.
  // Handle left button mouse click and draw red circle on display.
  // If there are already cricle, hide it.
  // And if right button mouse click is handeled,
  // show coordinate to standard output.
 
  pointProjector *tmp = (pointProjector *)param;

  if (event == CV_EVENT_LBUTTONDOWN)
    {
      if(tmp->hasPoint(x, y))
	tmp->hidePoint(x, y);
      else
	tmp->drawPoint(x, y);

      tmp->show();
    }
  else if (event == CV_EVENT_RBUTTONDOWN)
    {
      fprintf(stdout, "%d, %d\n", x, y);
      return;
    }
}


onePointProjector::onePointProjector(int w, int h)
{
  img = cvCreateImage(cvSize(w, h),IPL_DEPTH_8U,3);
  width = w;
  height = h;
  pointRadius = 15;
  backgroundColor = 0; // black
  pointColor = CV_RGB(255, 0, 0);
  windowName = "Projector";
  previous = cvPoint(-1, -1);

  // Make a window
  cvNamedWindow(windowName, CV_WINDOW_AUTOSIZE);

  // make background image
  cvSet(img, cvScalarAll(backgroundColor));

  // set mouse callback
  cvSetMouseCallback(windowName, on_mouse_prj, this);
}

int onePointProjector::drawPoint(CvPoint point)
{
  // input: x and y coordinate, CvPoint
  // output: draw a circle
  // return: 0
  //
  // Draw a circle at point (x, y).
  // And contain point coordinate into 'pointList'.

  // escape if x and y is out of image
  if (point.x < 0 || point.x >= width || point.y < 0 || point.y >= height)
    return -1;

  // draw circle
  cvCircle(img, point, pointRadius, pointColor, -1, 8, 0);

  return 0;
}

int onePointProjector::hidePoint(CvPoint point)
{
  if (point.x >= 0 || point.x < width || point.y >= 0 || point.y < height)
    cvCircle( img, point, pointRadius, cvScalarAll(backgroundColor), -1, 8, 0);  

  return 0;
}

int onePointProjector::showPoint(const CvPoint src, enum COORDINATE_ORIGIN origin)
{
  CvPoint point = src;

  if (origin == CENTER)
    point = convertOriginLeftTop(point);

  hidePoint(previous);
  drawPoint(point);
  previous = point;

  show();

  return 0;
}

CvPoint onePointProjector::convertOriginCenter(CvPoint src)
{
  // input: x and y coordinates, CvPoint
  // output: none
  // return: x and y coordinates, CvPoint
  //
  // Convert input coordinate which origin is left top of image
  // to the coordinate which origin is center of image.
  // Direction of enter origin coordinate is that, x axis is left to right
  // and y axis if bottom to top.

  CvPoint dst;

  dst.x = src.x - width / 2;
  dst.y = src.y*(-1) + height/2;

  return dst;
}

CvPoint onePointProjector::convertOriginLeftTop(CvPoint src)
{
  // input: x and y coordinates, CvPoint
  // output: none
  // return: x and y coordinates, CvPoint
  //
  // Convert input coordinate which origin is center of image
  // to the coordinate which origin is left top of image.
  // Direction of enter origin coordinate is that, x axis is left to right
  // and y axis if bottom to top.
  // This function is inverse of convertOriginCenter().

  CvPoint dst;

  dst.x = src.x + width / 2;
  dst.y = src.y*(-1) + height/2;

  return dst;
}

int onePointProjector::show()
{
  // input: none
  // output: display image
  // return: 0
  //
  // display image

  cvShowImage(windowName, img);

  return 0;
}

}
