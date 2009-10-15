// ~moriyama/projects/sr4000/trunk/src/cameraImages.cpp
// http://svn.xp-dev.com/svn/cou929_sakanelab/sr4000/trunk/cameraImages.cpp
//
// 2008-12-17 add function 'checkCoordinateRange' and set this function to some methods for validation of argument
// 2008-12-01 add namespace 'point'
// 2008-11-30
// Kousei MORIYAMA
//
// camera image class
// contain images capture from sr4000 camera
// image is OpenCV format
// present interface to access images, depth value, coordinate of each pixel, confidence of each pixel an so on
//

#include <iostream>
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

cameraImages::cameraImages()
{
  srCam = 0;
  cvDepth = 0;
  cvInt = 0;
  cvConf = 0;
  imgEntryArr = 0;
  X = 0;
  Y = 0;
  Z = 0;
  ampImgThreshold = 3000;
  width = 0;
  height = 0;
  pich = sizeof(short);
}

cameraImages::~cameraImages()
{
  cvReleaseImage(&cvDepth);
  cvReleaseImage(&cvInt);
  cvReleaseImage(&cvConf);
}

int cameraImages::initialize()
{
  //
  // input: none
  // return: 0 if succeed
  // open camera, initialize, allocate memory for IplImages and set pointer
  //
  // TODO:
  // - error handling
  // - contain window making process, if it can be
  //

  int res;
  unsigned short version[4];
  int pich = sizeof(short);

  // print version of driver
  SR_GetVersion(version);
  printf("libusbSR version: %d.%d.%d.%d\n", version[3], version[2], version[1], version[0]);

  // set camera initial settings
  res = SR_OpenUSB(&srCam, 0);
  printf("SR_OpenUSB() called result:%d\n",res);
  res = SR_SetIntegrationTime(srCam, 20);
  printf("SetIntegrationTime result:%d\n", res);
  res = SR_SetModulationFrequency(srCam, MF_20MHz);
  printf("SetModulationFrequency result:%d\n", res);
  //  res = SR_SetAutoExposure(srCam, 5,255,10,45);
  printf("SetAutoExposure result:%d\n",res);
  res = SR_SetAmplitudeThreshold(srCam, 40);
  printf("SetSetAmplitudeThreshold result:%d\n", res);

  // set acquire mode
  res = SR_SetMode(srCam, AM_COR_FIX_PTRN|AM_DENOISE_ANF|AM_CONF_MAP|AM_CONV_GRAY);
  printf("SetMode result:%d\n", res);

  // get image list
  res = SR_GetImageList(srCam, &imgEntryArr);
  printf("Number of images:%d\n", res);

  // set width and height
  width = SR_GetCols(srCam);
  height = SR_GetRows(srCam);

  // allocate memory for iplimage
  cvDepth = cvCreateImage(cvSize(width, height), IPL_DEPTH_16U, 1);
  cvInt = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
  cvConf = cvCreateImage(cvSize(width, height), IPL_DEPTH_16U, 1);

  // allocate memory for XYZ coodinate
  X = (short *)malloc(width*height*pich);
  Y = (short *)malloc(width*height*pich);
  Z = (unsigned short int*)malloc(width*height*pich);

  // set distance and confidence image to iplimage
  cvDepth->imageData = (char*)imgEntryArr[0].data;
  cvConf->imageData = (char*)imgEntryArr[2].data;

  return 0;
}

int cameraImages::acquire()
{
  // input: none
  // return: 0 if succeed
  // acquire images from camera to PC, calcurate XYZ coordinate of each pixel and convert amplitude image to IplImage.
  // call this function at beginning of while loop.
  //
  // TODO:
  // - value check

  int i, j;
  CvScalar ampVal;
  unsigned short int *p;

  // acuire image from camera to pc
  SR_Acquire(srCam);

  // calcurate XYZ coordinate of each pixel
  SR_CoordTrfUint16(srCam, X, Y, Z, pich, pich, pich);

  // calcurate amplitude image value to 8bit, and set it to iplimage
  p = (unsigned short int*)SR_GetImage(srCam,1);

  for(i=0; i<cvInt->height; i++)
    for(j=0; j<cvInt->width; j++)
      {
	ampVal.val[0] = *p;
	ampVal.val[0] *= (double)255 / (double)ampImgThreshold;

	if(ampVal.val[0] > 255)
	  ampVal.val[0] = 255;

	cvSet2D(cvInt, i, j, ampVal);
	p++;
      }

  return 0;
}

IplImage* cameraImages::getDepthImg()
{
  return cvDepth;
}

IplImage* cameraImages::getIntensityImg()
{
  return cvInt;
}

IplImage* cameraImages::getConfidenceMap()
{
  return cvConf;
}

int cameraImages::getIntensityVal(int x, int y)
{
  CvScalar tmp;

  if(checkCoordinateRange(x, y) == -1)
    return -1;

  tmp = cvGet2D(cvInt, x, y);

  return (int)tmp.val[0];
}

CvPoint3D32f cameraImages::getCoordinate(int x, int y)
{
  CvPoint3D32f ret = {-1, -1, -1};

  if(checkCoordinateRange(x, y) == -1)
    return ret;

  ret.x = X[x + y * width];
  ret.y = Y[x + y * width];
  ret.z = Z[x + y * width];

  return ret;
}

CvPoint3D32f cameraImages::getCoordinate(CvPoint point)
{
  CvPoint3D32f ret = {-1, -1, -1};

  if(checkCoordinateRange(point.x, point.y) == -1)
    return ret;

  ret.x = X[point.x + point.y * width];
  ret.y = Y[point.x + point.y * width];
  ret.z = Z[point.x + point.y * width];

  return ret;
}

int cameraImages::getConfidenceVal(int x, int y)
{
  CvScalar tmp;

  if(checkCoordinateRange(x, y) == -1)
    return -1;

  tmp = cvGet2D(cvConf, x, y);

  return (int)tmp.val[0];
}

int cameraImages::setAmpImgThreshold(int th)
{
  ampImgThreshold = th;
  return 0;
}

CvSize cameraImages::getImageSize()
{
  return cvSize(width, height);
}

}
