// ~moriyama/projects/sr4000/trunk/src/line.cpp
// http://svn.xp-dev.com/svn/cou929_sakanelab/sr4000/trunk/src/line.cpp
//
// 2009-01-13
// Kousei MORIYAMA
//
// Difinition of class 'line'.
//

#include "pointing.h"

namespace point {

// class 'line3DCv' extends 'line3D'. 'line3D' extends 'line'
line3DCv::line3DCv(void)
{
  flg = 0;
}

int line3DCv::setLine(CvPoint3D32f point1, CvPoint3D32f point2)
{
  // input: two points, CvPoint3D32f
  // output: set 'point' and 'directionVecror'
  // return: 0 if succeed, -1 if failure the process
  //
  // Attributes 'point1' and 'point2' is the point which is on a straight line on 3D space.
  // Calcurate direction vector from two point and set it to member variable.

  CvPoint3D32f tmp;
  double z;
  int ret = -1;

  this->init();

  tmp.x = point2.x - point1.x;
  tmp.y = point2.y - point1.y;
  tmp.z = point2.z - point1.z;

  z = sqrt(tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z);
  if (z != 0)
    {
      directionVecror.x = tmp.x / z;
      directionVecror.y = tmp.y / z;
      directionVecror.z = tmp.z / z;
      point = point2;
      flg = 1;
      ret = 0;
    }

  return ret;
}

CvPoint3D32f line3DCv::getLinePointByZ(double v)
{
  // input: Z (depth) value, double
  // output: none
  // return: a coordinate, CvPoint3D32f
  //
  // Return a coordinate which is on the straight line and it's Z value is equal to attribute 'v'.

  CvPoint3D32f ret;

  ret.x = -1;
  ret.y = -1;
  ret.z = -1;

  if (flg)
    {
      ret.x = directionVecror.x * ( v - point.z) / directionVecror.z + point.x;
      ret.y = directionVecror.y * ( v - point.z) / directionVecror.z + point.y;
      ret.z = v;
    }

  return ret;
}

int line3DCv::init(void)
{
  // input: none
  // output: none
  // return: 0 if succeed, -1 if failure the process
  //
  // Initialize member variables. All values are setted to -1.

  point.x = -1;
  point.y = -1;
  point.z = -1;
  directionVecror.x = -1;
  directionVecror.y = -1;
  directionVecror.z = -1;
  flg = 0;

  return 0;
}

bool line3DCv::isValid(void)
{
  // input: none
  // output: none
  // return: 1 if direction vector is successfully calcurated, -1 if failure the process
  //
  // Validate the line.

  bool ret = false;

  if (flg == 1 && directionVecror.x != -1)
    ret = true;

  return ret;
}

} // ! namepacs point
