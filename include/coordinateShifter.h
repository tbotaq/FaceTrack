// ~moriyama/projects/sr3000/trunk/include/coordinateShifter.h
// http://svn.xp-dev.com/svn/cou929_repo/master/sr4000/trunk/include/coordinateShifter.h
// 
// 2008-12-09 based on 'projection.h'
// Kosuei MORIYAMA
//
// shift camera coordinate to projector coordinate
//

#ifndef __COORDINATESHIFTER_H_
#define __COORDINATESHIFTER_H_

#include <cstdio>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
using namespace std;

namespace cor
{

class rotationMatrix
{
private:
  CvMat *matrix;
public:
  rotationMatrix();
  int generateFromRPY(double x, double y, double z);   // roll/pitch/yaw angle
  int generateFromEuler(double x, double y, double z); // euler angle, not inmplement yet
  int set(int row, int col, double val);
  double get(int row, int col);
  friend class coordinateShifter;
};

class coordinateShifter
{
private:
  class rotationMatrix *rotation;
  CvMat *trans;
  double focal;
  const int NO_VALUE;
public:
  coordinateShifter();
  ~coordinateShifter();
  int setParameter(double Tx, double Ty, double Tz, double Rx, double Ry, double Rz, double f);
  int setParameter(CvPoint3D32f *srcExp, CvPoint3D32f *srcRes, int size);
  CvPoint world2img(double x, double y, double z);
  CvPoint3D32f model2world(double x, double y, double z);
};

}

#endif
