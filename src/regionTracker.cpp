// ~moriyama/projects/sr4000/trunk/src/regionTracker.cpp
//
// 2009-01-06 add class 'new'ing process into constructor, because of accepting change of these class's constructor
// 2008-12-22 change centroid calcuration. erode human region before calcuration centroid, because of reducing influence of arm,
//            add memory release processed into some functions
// 2008-12-04
// Kousei MORIYAMA
//
// tracking human in the image sequence
//

#include <cstdio>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"

using namespace std;

namespace point
{

regionTracker::regionTracker(cameraImages *cam)
{
  cm = cam;
  initializeFlag = 0;
  result = cvCreateImage(cm->getImageSize(), IPL_DEPTH_8U, 1);
  contractedResult = cvCreateImage(cm->getImageSize(), IPL_DEPTH_8U, 1);
  element = cvCreateStructuringElementEx (3, 3, 1, 1, CV_SHAPE_CROSS, NULL);
  intensity = cm->getIntensityImg();
  depth = cm->getDepthImg();
  human = new regionDetector(cm->getImageSize());
  fd = new faceDetector(cm->getImageSize());
}

regionTracker::~regionTracker()
{
  cvReleaseImage(&result);
  cvReleaseImage(&contractedResult);
  delete human;
  delete fd;
}

int regionTracker::track()
{
  // input: none
  // return: 0
  //
  // Controler of regionTracker.

  if(initializeFlag == 0)
    return initialize();
  else
    return trackRegion();
}

int regionTracker::initialize()
{
  // input: intensity image, 8bit, 1channel, grayscale
  //        depth image, 16bit, 1channel, grayscale
  // output: human region, IplImage, 8bit, 1channlel, binary image
  //         ('initializeFlag' is set to 1 if initialize is successfully processed)
  // return: 0
  //
  // take normal intensity image and generate silhouette binary image
  // silhouette is human region in the input image

  CvPoint center;                               // coordinate of center of detected face
  int radius;                                   // radius of detected face
  CvScalar tmp;

  // clear result image
  cvSetZero(result);

  // detect face and get human region
  if(fd->faceDetect(intensity, &center, &radius) > 0)
    {
      // get human region
      human->getRegion(depth, center.x, center.y, result);

      // calcurate centroid and area of human region
      if(calcCentroidAndArea() == -1)
	{
	  initializeFlag = 0;
	  return -1;
	}

      // get depth value of centroid
      tmp = cvGet2D(depth, center.y, center.x);
      centroidDepth = (int)tmp.val[0];

      // set flag
      initializeFlag = 1;

      return 0;
    }

  return -1;
}

int regionTracker::trackRegion()
{
  // input: current frame's depth image
  // output: set centroid coordinate, depth of centroid and area, CvPoint, int and int
  // return: 0 if success, -1 if failed
  //
  // Track the region.
  // Get region by starting centroid point.
  // If depth value of centroid or area value is obviously difference from previous frame's one,
  // set flag 1 (re-initialize) and return -1.

  int depthThreshold;                  // threshold of depth value difference between current and previous frame
  int areaThreshold;                   // threshold of area value difference between current and previous frame
  int prevArea;                        // area of previous frame's region
  int bitDepth;                        // contain 'depth''s bit depth
  CvScalar pix;

  // for debug /////////////////////////////
  IplImage *tmp = cvCreateImage(cvSize(depth->width, depth->height), IPL_DEPTH_8U, 1);
  int i, j;
  CvScalar a;
  for(i=0; i<tmp->width; i++)
    for(j=0; j<tmp->height; j++)
      {
	a = cvGet2D(depth, j, i);
	a.val[0] = a.val[0] * 255 / 65535;
	cvSet2D(tmp, j, i, a);
      }
  cvSet2D(tmp, centroid.y, centroid.x, cvScalar(255));
  cvCircle(tmp, centroid, 20, CV_RGB(255, 255, 255));
  cvShowImage("centroid", tmp);
  cvReleaseImage(&tmp);
  ///////////////////////////////////////

  // clear result image
  cvSetZero(result);

  // set detph threshold by bit depth
  bitDepth = depth->depth;
  if(bitDepth == IPL_DEPTH_8U)
    depthThreshold = 100;
  else if(bitDepth == IPL_DEPTH_16U)
    depthThreshold = 15000;
  else
    return 0;

  // set area threshold
  //  areaThreshold = 4000;
  areaThreshold = 14000;

  // get depth value of centroid coordinate
  pix = cvGet2D(depth, centroid.y, centroid.x);
  if( abs((int)pix.val[0] - centroidDepth) > depthThreshold)
    {
      fprintf(stderr, "Cannot track by depth threshold: %d, %d\n", (int)pix.val[0], (int)centroidDepth);
      initializeFlag = 0;
      return -1;
    }

  // get region
  human->getRegion(depth, centroid.x, centroid.y, result);

  // evacuate area value
  prevArea = area;

  // calcurate centroid and area of human region
  if(calcCentroidAndArea() == -1)
    {
      fprintf(stderr, "Cannot track by calcuraion of centroid and area\n");
      initializeFlag = 0;
      return -1;
    }

  // check area value
  if( abs(prevArea - area) > areaThreshold)
    {
      fprintf(stderr, "Cannot track by area threshold: %d, %d\n", prevArea, area);
      initializeFlag = 0;
      return -1;
    }

  return 0;
}

int regionTracker::calcCentroidAndArea()
{
  // input: none
  // output: set centroid and area, CvPoint and int
  // return: 0
  //
  // Calculate controid and area of region

  int areaCount = 0;         // count total pixel of region
  int xSum = 0, ySum = 0;    // sum of x (or y) coordinate of each pixel in the region
  CvScalar current;
  int i, j;
  int iteration = 5;

  // contract 'result' for reduce influence by arm when calcurate centroid
  cvErode (result, contractedResult, element, iteration);

  for(i=0; i<result->width; i++)
    for(j=0; j<result->height; j++)
      {
	current = cvGet2D(contractedResult, j, i);
	if(current.val[0] != 0)
	  {
	    areaCount++;
	    xSum += i;
	    ySum += j;
	  }
      }

  // set result
  if(areaCount == 0) return -1;
  area = areaCount;
  centroid.x = xSum / areaCount;
  centroid.y = ySum / areaCount;

  return 0;
}

IplImage* regionTracker::getResult()
{
  // input: none
  // output: none
  // return: result, IplImage
  //
  // Return tracking result, the binary image.

  return result;
}

}
