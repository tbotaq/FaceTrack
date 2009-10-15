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

projector::projector(int w, int h) : POINT_ARRAY_MAX(1000), NO_POINT(-1)
{
  img = cvCreateImage(cvSize(w, h),IPL_DEPTH_8U,3);
  width = w;
  height = h;
  pointIndex = -1;
  pointRadius = 15;
  backgroundColor = 0; // black
  pointColor = CV_RGB(255, 0, 0);
  windowName = "Projector";

  // Make a window
  cvNamedWindow(windowName, CV_WINDOW_AUTOSIZE);

  // make background image
  cvSet(img, cvScalarAll(backgroundColor));

  // set mouse callback
  cvSetMouseCallback (windowName, on_mouse, this);

  // allocate point array memory
  pointArray = (CvPoint*)calloc(POINT_ARRAY_MAX, sizeof(CvPoint));
}

projector::~projector()
{
  cvReleaseImage(&img);
  cvDestroyWindow(windowName);
  free(pointArray);
}

int projector::drawPoint(int x, int y)
{
  // input: x and y coordinate, int
  // output: draw a red circle
  // return: 0
  //
  // Draw a red circle at point (x, y).
  // And contain point coordinate into 'pointArray'.

  // escape if x and y is out of image, or pointIndex is full
  if(x < 0 || x >= width || y < 0 || y >= height)
    return -1;

  // if pointIndex is larger than POINT_ARRAY_MAX, set pointIndex to 0 (the first element of pointArray) 
  if(pointIndex >= POINT_ARRAY_MAX)
    pointIndex = 0;

  // contain into 'pointArray'
  pointIndex++;
  pointArray[pointIndex].x = x;
  pointArray[pointIndex].y = y;

  // draw circle
  cvCircle( img, pointArray[pointIndex], pointRadius, pointColor, -1, 8, 0);

  return 0;
}

int projector::hidePoint(int x, int y)
{
  // input: x and y coordinate, int
  // output: hide drawn point
  // return: 0
  //
  // Hide drawn point which coordinate if (x, y).

  int index;

  // check x and y range
  if(x < 0 || x > width || y < 0 || y > height)
    return -1;

  // check there are point at (x, y)
  index = isPoint(x, y);
  if(index == NO_POINT)
    return -1;

  // hide point
  hidePoint(index);

  return 0;
}

int projector::hidePoint(int index)
{
  // input: index number, int
  // output: hide drawn point
  // return: 0
  //
  // Hide drawn point which pointArray's index is argument 'index'
  // by drawing same size white circle on the red cricle.
  // And then set label 'NO_POINT' to the current index of 'pointArray'.

  // check 'index' range
  if(index > pointIndex)
    return -1;

  // get index's x and y coordinate
  int x = pointArray[index].x;
  int y = pointArray[index].y;

  // draw a white circle
  cvCircle( img, cvPoint(x, y), pointRadius, cvScalarAll(backgroundColor), -1, 8, 0);

  // label 'NO_POINT'
  pointArray[index].x = NO_POINT;

  return 0;
}

int projector::show()
{
  // input: none
  // output: display image
  // return: 0
  //
  // display image

  cvShowImage(windowName, img);

  return 0;
}

int projector::isPoint(int x, int y)
{
  // input: x and y coordinate, int
  // output: none
  // return: if there are point which coordinate is (x, y) in the 'pointArray', return 'index',
  //         or not, return 'NO_POINT'
  //
  // Check there are (x, y) point in the 'pointArray' or not.
  // If that point found, return index, not found, return 'NO_POINT'.

  int i;
  int index = NO_POINT;

  // search
  for(i = 0; i <= pointIndex; i++)
    {
      // if coordinate is inside the circle
      if( abs(pointArray[i].x - x) <= pointRadius && abs(pointArray[i].y - y) <= pointRadius)
	{
	  index = i;
	  break;
	}
    }

  return index;
}

int projector::printPointCoordinate(enum COORDINATE_ORIGIN origin)
{
  // input: none
  // output: print coordinates that is contained in the 'pointArray'
  // return: 0
  //
  // Print coordinates that is contained in the 'pointArray'.

  int i;
  CvPoint result;

  for(i=0; i<=pointIndex; i++)
    if(pointArray[i].x != NO_POINT)
      {
	result = pointArray[i];
	if(origin == CENTER)
	  result = convertOriginCenter(result);
	fprintf(stdout, "%d, %d\n", result.x, result.y);
      }

  return 0;
}

int projector::saveFile(char *fileName)
{
  // input: filename, char*
  // output: save 'pointArray' coordinates to the file
  // return: 0
  //
  // Save 'pointArray' coordinates to the file

  FILE *fp;
  int i;

  // open file
  fp = fopen(fileName, "w");
  if(fp == NULL)
    {
      fprintf(stderr, "ERROR: file \"%s\"cannot open\n", fileName);
      return -1;
    }

  // write to file
  for(i=0; i<=pointIndex; i++)
    if(pointArray[i].x != NO_POINT)
      fprintf(fp, "%d,%d\n", pointArray[i].x, pointArray[i].y);

  // close
  fclose(fp);

  return 0;
}

CvPoint projector::convertOriginCenter(CvPoint src)
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

CvPoint projector::convertOriginLeftTop(CvPoint src)
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

void on_mouse(int event, int x, int y, int flags, void *param)
{
  // input: void *param is pointer to class 'projector'
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
 
  int index;

  projector *tmp = (projector *)param;

  if(event == CV_EVENT_LBUTTONDOWN)
    {
      index = tmp->isPoint(x, y);

      // draw or hide
      if(index == -1)
	tmp->drawPoint(x, y);
      else
	tmp->hidePoint(index);

      // show
      tmp->show();
    }
  else if(event == CV_EVENT_RBUTTONDOWN)
    {
      fprintf(stdout, "%d, %d\n", x, y);
      return;
    }
}

}
