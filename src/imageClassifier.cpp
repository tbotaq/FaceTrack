// ~moriyama/projects/sr3000/trunkimageClassifier.cpp
// http://svn.xp-dev.com/svn/cou929_repo/master/sr4000/trunk/imageClassifier.cpp
//
// 2008-12-02 
// Kousei MORIYAMA
//
// take binary image, classify regions (white region in image), and output a specify region
//

#include <cstdio>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include "pointing.h"

using namespace std;

namespace point
{

imageClassifier::imageClassifier() : NONE(0), LEFT(1), RIGHT(2), TOP(3), BOTTOM(4)
{
  label = 1;
}

imageClassifier::~imageClassifier()
{
  cvReleaseImage(&classImg);
}

IplImage* imageClassifier::getRegionImg(IplImage *src, int x, int y)
{
  // input: binary image, IplImage, (recommend 8 bit depth and 1 channel)
  //        a coordinate which is contained in region you want to get, int
  // return: region image, IplImage, binary image
  //         or 0 if coordinate is invalid (out of iamge size or there are no region)
  //
  // Take binary image, classify sorce image's white regions separated by black pixels,
  // and return the region which contain pixel (x, y).

  IplImage *tmp;

  // set WHITE and LABEL_MAX (need to automate lator)
  WHITE = 255;
  LABEL_MAX = 254;

  // clone image src to tmp
  tmp = cvCloneImage(src);

  // convert tmp to binary image (for safety)
  cvThreshold(tmp, tmp, WHITE/2, WHITE, CV_THRESH_BINARY);

  // classify
  classify(tmp);

  // release
  cvReleaseImage(&tmp);

  // get region
  return getRegionByCoordinate(x, y);
}

int imageClassifier::classify(IplImage *src)
{
  // input: binary image, IplImage, any bit depth, 1 channel
  // output: set classified image to 'classImg'
  // return: 0
  //
  // Classify 'src' image by putting "labels" (1 to 0xff-1 (8bit image case) value, inclusive) to region in IplImage format.

  CvScalar currentPixel;
  int i, j;

  // clone src to classImg
  classImg = cvCloneImage(src);

  // search
  for(i=0; i<src->width; i++)
    for(j=0; j<src->height; j++)
      {
	currentPixel = cvGet2D(classImg, j, i);

	// if current pixel is white (not labeled pixel)
	if(currentPixel.val[0] == WHITE)
	  {
	    labelling(i, j, NONE);
	    labelIncrement();
	  }
      }

  return 0;
}

int imageClassifier::labelling(int x, int y, int direction)
{
  // input: coordinate, int
  //        direction, const int, LEFT, RIGHT, TOP, BOTTOM or NONE
  // output: set label to 'classImg'
  // return: 0
  //
  // Recursive function.
  // Traverse white pixels and labell

  CvScalar currentPixel;

  // check 'x' and 'y' range
  if(x < 0 || y < 0 || x >= classImg->width || y >= classImg->height)
    return 0;

  // get current pixel's value
  currentPixel = cvGet2D(classImg, y, x);

  // if current pixel is black or already labelled, break
  if(currentPixel.val[0] != WHITE)
    return 0;

  // set label to current pixel
  currentPixel.val[0] = label;
  cvSet2D(classImg, y, x, currentPixel);

  // search near pixel
  if(direction != BOTTOM) labelling(x, y-1, TOP);
  if(direction != TOP) labelling(x, y+1, BOTTOM);
  if(direction != LEFT) labelling(x+1, y, RIGHT);
  if(direction != RIGHT) labelling(x-1, y, LEFT);

  return 0;
}

int imageClassifier::labelIncrement()
{
  // input: none
  // output: increment label number or report error
  // return: 0
  //
  // Increment label number.
  // If 'label' is larger than 0xff, print error message and return NULL. (this is 8bit image case).

  if(label < LABEL_MAX)
    label++;
  else
    fprintf(stderr, "ERROR: imageClassifier, label is maximum.\n");

  return 0;
}

IplImage* imageClassifier::getRegionByLabel(int labelNumber)
{
  // input: number of label, int
  // output: none
  // return: region which labelled 'labelNumber', binary image, IplImage
  //
  // return region image which labelled 'labelNumber'

  CvScalar currentPixel;
  IplImage *dst;
  int i, j;

  dst = cvCloneImage(classImg);

  for(i=0; i<classImg->width; i++)
    for(j=0; j<classImg->height; j++)
      {
	currentPixel = cvGet2D(classImg, j, i);

	if(currentPixel.val[0] != 0)
	  if(currentPixel.val[0] == labelNumber)
	    cvSet2D(dst, j, i, cvScalar(WHITE));
	  else
	    cvSet2D(dst, j, i, cvScalar(0));
      }

  return dst;
}

IplImage* imageClassifier::getRegionByCoordinate(int x, int y)
{
  // input: coordinate, int
  // output: none
  // return: region which labelled 'labelNumber', binary image, IplImage
  //
  // return region image which contain the (x, y) coordinate pixel

  CvScalar tmp;

  tmp = cvGet2D(classImg, y, x);

  if(tmp.val[0] == 0 || tmp.val[0] > label)
    {
      fprintf(stderr, "ERROR: imageClassifier, invalid coordinate.\n");
      return 0;
    }

  return getRegionByLabel((int)tmp.val[0]);
}

}
