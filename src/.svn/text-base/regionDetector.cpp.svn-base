// ~moriyama/projects/sr4000/trunk/regionDetector.cpp
// http://svn.xp-dev.com/svn/cou929_repo/master/sr4000/trunk/regionDetector.cpp
//
// 2009-01-06 remove memory allocation which did in function 'getRegion' to constructor, for memory leak.
// 2008-12-04
// Kousei MORIYAMA
//
// region detector
// Detect ragion from depth, intensity, binary or etc images.
//

#include <cstdio>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include "pointing.h"

using namespace std;

namespace point
{

regionDetector::regionDetector(CvSize size) : NONE(0), LEFT(1), RIGHT(2), TOP(3), BOTTOM(4), WHITE(255)
{
  original = 0;
  threshold = 20;
  element = cvCreateStructuringElementEx (3, 3, 1, 1, CV_SHAPE_CROSS, NULL);
  result = cvCreateImage(size, IPL_DEPTH_8U, 1);
}

regionDetector::~regionDetector()
{
  cvReleaseImage(&result);
}


int regionDetector::getRegion(IplImage *src, int x, int y, IplImage *dst)
{
  // input: image (depth, intensity and so on), IplImage, 1 channel, (recommend 8 bit depth)
  //        a coordinate which is contained in region you want to get, int
  // return: region image, IplImage, binary image
  //         or 0 if coordinate is invalid (out of iamge size or there are no region)
  //
  // Take (depth, intensity, binary and so on) image, classify the image by the region.
  // The region is pixels which has almost same value between that pixel and around pixels.
  // And return the region which contain pixel (x, y).

  int bitDepth;
  int iteration;

  // prepare images
  original = src;
  result->imageData = dst->imageData;

  // set threshold value.
  bitDepth = src->depth;
  if(bitDepth == IPL_DEPTH_8U)
    threshold = 20;
  else if(bitDepth == IPL_DEPTH_16U)
    threshold = 4000;
  else
    return 0;

  // get region
  traverse(x, y, NONE);

  // noise reduction
  iteration = 2;
  cvErode (dst, dst, element, iteration);
  cvDilate (dst, dst, element, iteration);

  return 0;
}

int regionDetector::traverse(int x, int y, int direction)
{
  // input: coordinate, int
  //        direction, const int, LEFT, RIGHT, TOP, BOTTOM or NONE
  // output: make binary image in 'result', white pixels represents the same region
  // return: 0
  //
  // Recursive function.
  // Traverse pixels and then label.
  // When search process reaches the pixel, calcurate gradient value 
  // between current pixel and around of current pixel, 
  // total these gradient values, and then if total of gradient is 
  // smaller than threshold (default 20 or 2000), current pixel is setted as inner region
  // and next searches around pixels,
  // if larger than threshold, current pixel is setted as inner region
  // but not traverse around pixels.

  int sumGradient = 0;
  int i, j;
  CvScalar tmp;

  // check 'x' and 'y' range
  if(x < 0 || y < 0 || x >= original->width || y >= original->height)
    return 0;

  // if current pixel is already labeled, exit
  tmp = cvGet2D(result, y, x);
  if(tmp.val[0] == WHITE)
    return 0;

  // set label to current pixel
  cvSet2D(result, y, x, cvScalar(WHITE));

  // check gradient value between current pixel and around pixels
  for(i=x-1; i<=x+1; i+=2)
    for(j=y-1; j<=y+1; j+=2)
      if(i > 0 && j > 0 && i < original->width && j < original->height)
	sumGradient += calcGradient(cvPoint(x, y), cvPoint(i, j));

  // diverge by 'sumGradient'
  if(sumGradient > threshold)
    return 0;

  // search near pixel
  if(direction != BOTTOM) traverse(x, y-1, TOP);
  if(direction != TOP) traverse(x, y+1, BOTTOM);
  if(direction != LEFT) traverse(x+1, y, RIGHT);
  if(direction != RIGHT) traverse(x-1, y, LEFT);

  return 0;
}

int regionDetector::calcGradient(CvPoint arg1, CvPoint arg2)
{
  // input: two coordinates, CvPoint
  // output: none
  // return: gradient value
  //
  // Calcurate gradient value, the absolute value of 
  // difference between 'arg1' and 'arg2'

  CvScalar s1, s2;
  double tmp;

  s1 = cvGet2D(original, arg1.y, arg1.x);
  s2 = cvGet2D(original, arg2.y, arg2.x);

  if(s1.val[0] < s2.val[0])
    {
      tmp = s1.val[0];
      s1.val[0] = s2.val[0];
      s2.val[0] = tmp;
    }

  return (int)(s1.val[0] - s2.val[0]);
}

int regionDetector::setThreshold(int th)
{
  // input: threshold value, int
  // output: none
  // return: 0
  //
  // Set threshold.

  threshold = th;

  return 0;
}

}
