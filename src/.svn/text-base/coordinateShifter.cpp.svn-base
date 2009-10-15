// ~moriyama/projects/sr3000/trunk/src/coordinateShifter.cpp
// http://svn.xp-dev.com/svn/cou929_repo/master/sr4000/trunk/src/coordinateShifter.cpp
// 
// 2008-12-09 based on 'projection.cpp'
// Kosuei MORIYAMA
//
// shift camera coordinate to projector coordinate
//

#include <cstdio>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include "coordinateShifter.h"

using namespace std;

namespace cor
{

rotationMatrix::rotationMatrix()
{
  matrix = cvCreateMat(3, 3, CV_64F);
}

int rotationMatrix::generateFromRPY(double x, double y, double z)
{
  int i, j;
  CvMat *rotX = cvCreateMat(3, 3, CV_64F);
  CvMat *rotY = cvCreateMat(3, 3, CV_64F);
  CvMat *rotZ = cvCreateMat(3, 3, CV_64F);

  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      {
	cvmSet(rotX, i, j, 0.0);
	cvmSet(rotY, i, j, 0.0);
	cvmSet(rotZ, i, j, 0.0);
      }

  // rotation matrix for X axis
  cvmSet(rotX, 0, 0, 1.0);
  cvmSet(rotX, 1, 1, cos(x));
  cvmSet(rotX, 1, 2, sin(x)*(-1));
  cvmSet(rotX, 2, 1, sin(x));
  cvmSet(rotX, 2, 2, cos(x));

  // rotation matrix for Y axis
  cvmSet(rotY, 1, 1, 1.0);
  cvmSet(rotY, 0, 0, cos(y));
  cvmSet(rotY, 0, 2, sin(y));
  cvmSet(rotY, 2, 0, sin(y)*(-1));
  cvmSet(rotY, 2, 2, cos(y));

  // rotation matrix for Z axis
  cvmSet(rotZ, 2, 2, 1.0);
  cvmSet(rotZ, 0, 0, cos(z));
  cvmSet(rotZ, 0, 1, sin(z)*(-1));
  cvmSet(rotZ, 1, 0, sin(z));
  cvmSet(rotZ, 1, 1, cos(z));

  CvMat *tmp = cvCreateMat(3, 3, CV_64F);

  cvMatMul(rotY, rotX, tmp);
  cvMatMul(rotZ, tmp, matrix);

  return 0;
}

int rotationMatrix::set(int row, int col, double val)
{
  cvmSet(matrix, row, col, val);

  return 0;
}

double rotationMatrix::get(int row, int col)
{
  return cvmGet(matrix, row, col);
}


coordinateShifter::coordinateShifter() : NO_VALUE(-1)
{
  rotation = new rotationMatrix;
  trans = cvCreateMat(3, 1, CV_64F);
  focal = NO_VALUE;
}

coordinateShifter::~coordinateShifter()
{
  delete rotation;
}

int coordinateShifter::setParameter(double Tx, double Ty, double Tz, double Rx, double Ry, double Rz, double f)
{
  // transport matrix
  cvmSet(trans, 0, 0, Tx);
  cvmSet(trans, 1, 0, Ty);
  cvmSet(trans, 2, 0, Tz);
 
  // rotation matrix
  rotation->generateFromRPY(Rx, Ry, Rz);
 
  // focal length
  focal = f;

  return 0;
}

int coordinateShifter::setParameter(CvPoint3D32f *srcExp, CvPoint3D32f *srcRes, int size)
{
  // (X Y Z)^T = R(x y z)^T + T
  // (X Y Z) = A (x y z)  , where A = (R|T)
  // approximate A, using least square method
  //
  // input date: (X Y Z) <- dstCoor(ex. world coordinate), and (x y z) <- srcCoor(ex. camera coordinate)
  //
  // X = (x y z 1)(r_11 r_12 r_13 t_1)^T
  // Y = (x y z 1)(r_21 r_22 r_23 t_1)^T
  // Z = (x y z 1)(r_31 r_32 r_33 t_1)^T

  if(size < 3)
    {
      fprintf(stderr, "ERROR: number of variable is not enough. at least three\n");
      return -1;
    }

  int i;

  CvMat *responseVarX = cvCreateMat(size, 1, CV_64F);  // left side of equation
  CvMat *responseVarY = cvCreateMat(size, 1, CV_64F);
  CvMat *responseVarZ = cvCreateMat(size, 1, CV_64F);
  CvMat *expVar  = cvCreateMat(size, 4, CV_64F);  // right side of equation

  CvMat *parameterX = cvCreateMat(4, 1, CV_64F);
  CvMat *parameterY = cvCreateMat(4, 1, CV_64F);
  CvMat *parameterZ = cvCreateMat(4, 1, CV_64F);

  for(i=0; i<size; i++)
    {
      cvmSet(responseVarX, i, 0, srcRes[i].x);
      cvmSet(responseVarY, i, 0, srcRes[i].y);
      cvmSet(responseVarZ, i, 0, srcRes[i].z);
      cvmSet(expVar, i, 0, srcExp[i].x);
      cvmSet(expVar, i, 1, srcExp[i].y);
      cvmSet(expVar, i, 2, srcExp[i].z);
      cvmSet(expVar, i, 3, 1);
    }

  // least square method
  CvMat *transExpVar = cvCreateMat(4, size, CV_64F);
  CvMat *square = cvCreateMat(4, 4, CV_64F);
  CvMat *inv = cvCreateMat(4, 4, CV_64F);
  CvMat *invTrans = cvCreateMat(4, size, CV_64F);

  cvTranspose(expVar, transExpVar);
  cvMatMul(transExpVar, expVar, square);
  cvInvert(square, inv);
  cvMatMul(inv, transExpVar, invTrans);
  cvMatMul(invTrans, responseVarX, parameterX);
  cvMatMul(invTrans, responseVarY, parameterY);
  cvMatMul(invTrans, responseVarZ, parameterZ);

  // set parameters
  for(i=0; i<3; i++)
    {
      rotation->set(0, i, cvmGet(parameterX, i, 0));
      rotation->set(1, i, cvmGet(parameterY, i, 0));
      rotation->set(2, i, cvmGet(parameterZ, i, 0));
    }

  cvmSet(trans, 0, 0, cvmGet(parameterX, 3, 0));
  cvmSet(trans, 1, 0, cvmGet(parameterY, 3, 0));
  cvmSet(trans, 2, 0, cvmGet(parameterZ, 3, 0));

  return 0;
}

CvPoint coordinateShifter::world2img(double x, double y, double z)
{
  double Xs, Ys, Zs, retX, retY;
  CvPoint result;

  CvMat *RCAM = cvCreateMat(3, 3, CV_64F);
  cvInvert(rotation->matrix, RCAM);

  CvMat *PmPc = cvCreateMat(3, 1, CV_64F);
  cvmSet(PmPc, 0, 0, x - cvmGet(trans, 0, 0));
  cvmSet(PmPc, 1, 0, y - cvmGet(trans, 1, 0));
  cvmSet(PmPc, 2, 0, z - cvmGet(trans, 2, 0));

  CvMat *Ps = cvCreateMat(3, 1, CV_64F);
  cvMatMul(RCAM, PmPc, Ps);

  Xs = cvmGet(Ps, 0, 0);
  Ys = cvmGet(Ps, 1, 0);
  Zs = cvmGet(Ps, 2, 0);

  retX = (-1) * ((focal * Ys) / Xs);
  retY = (-1) * ((focal * Zs) / Xs);

  result.x = (int)retX;
  result.y = (int)retY;

  return result;

#if 0
  int i, j;

  // homogeneous transformation matrix. (R|T)
  CvMat *homogeneous = cvCreateMat(3, 4, CV_64F);
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      cvmSet(homogeneous, i, j, cvmGet(rotation->matrix, i, j));

  for(i=0; i<3; i++)
    cvmSet(homogeneous, i, 3, cvmGet(trans, i, 0)*(-1));

  //  printf("\n homogeneous \n");
  //  printMat(homogeneous);

  // the point in world coodinate. (x y z 1)^T
  CvMat *worldCoordinate = cvCreateMat(4, 1, CV_64F);
  cvmSet(worldCoordinate, 0, 0, x);
  cvmSet(worldCoordinate, 1, 0, y);
  cvmSet(worldCoordinate, 2, 0, z);
  cvmSet(worldCoordinate, 3, 0, 1);
  //  printf("\n world(input) \n");
  //  printMat(worldCoordinate);

  // the point in this model coordinate. equal to (R|T)(x y z 1)^T
  CvMat *modelCoodinate = cvCreateMat(3, 1, CV_64F);
  cvMatMul(homogeneous, worldCoordinate, modelCoodinate);
  //  printf("\n model \n");
  //  printMat(modelCoodinate);

  // get image coordinate
  CvPoint result;
  //  result.x = (int)(focal * cvmGet(modelCoodinate, 0, 0) / cvmGet(modelCoodinate, 2, 0));  // f*x/z
  //  result.y = (int)(focal * cvmGet(modelCoodinate, 1, 0) / cvmGet(modelCoodinate, 2, 0));  // f*y/z
  result.x = (int)(focal * cvmGet(modelCoodinate, 1, 0) / cvmGet(modelCoodinate, 0, 0));  // f*y/x
  result.y = (int)(focal * cvmGet(modelCoodinate, 2, 0) / cvmGet(modelCoodinate, 0, 0));  // f*z/x

  return result;
#endif
}

CvPoint3D32f coordinateShifter::model2world(double x, double y, double z)
{
  int i, j;
  CvPoint3D32f result;

  // ganerate homogeneous transport matrix
  CvMat *homogeneous = cvCreateMat(3, 4, CV_64F);
  for(i=0; i<3; i++)
    {
      cvmSet(homogeneous, i, 3, cvmGet(trans, i, 0));
      for(j=0; j<3; j++)
	cvmSet(homogeneous, i, j, rotation->get(i, j));
    }

  // generate explanatory variable matrix
  CvMat *exp = cvCreateMat(4, 1, CV_64F);
  cvmSet(exp, 0, 0, x);
  cvmSet(exp, 1, 0, y);
  cvmSet(exp, 2, 0, z);
  cvmSet(exp, 3, 0, 1);

  // response variable matrix (return value)
  CvMat *res = cvCreateMat(3, 1, CV_64F);

  // shift
  cvMatMul(homogeneous, exp, res);

  result.x = cvmGet(res, 0, 0);
  result.y = cvmGet(res, 1, 0);
  result.z = cvmGet(res, 2, 0);

  return result;
}

}
