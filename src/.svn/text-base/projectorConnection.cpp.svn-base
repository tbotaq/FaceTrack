// ~moriyama/projects/sr3000/trunk/src/projectorConnection.cpp
// http://svn.xp-dev.com/svn/cou929_repo/master/sr4000/trunk/src/projectorConnection.cpp
//
// 2008-12-09 fix connection to class 'connection'
// 2008-12-07
// Kousei MORIYAMA
//
// class 'projectorConnection', extends 'connection'
//

#include <cstdio>
#include <cstring>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "sharedMemory.h"

using namespace std;

namespace shm
{

projectorConnection::projectorConnection(char *mapName) : connection(mapName)
{
}

int projectorConnection::sendCoordinate(CvPoint src)
{
  // input: coordinate, CvPoint
  // output: write coordinate to shared memory
  // return: 0
  //
  // Write coordinate to shared memory.

  char string[30];

  sprintf(string, "%d,%d", src.x, src.y);

  sendCommand(SHM_CODE_POINT_TO_PRJ, string);

  return 0;
}

CvPoint projectorConnection::getCoordinate()
{
  // input: binary image, IplImage, (recommend 8 bit depth and 1 channel)
  // output: none
  // return: coordinate, CvPoint
  //
  // Read shared memory and return coordinate.

  CvPoint ret;
  char string[30];

  // set default value
  ret.x = -1;
  ret.y = -1;

  // read
  if(getCommand(SHM_CODE_POINT_TO_PRJ, string) == -1)
    return ret;

  // extract coordinate
  sscanf(string, "%d,%d", &ret.x, &ret.y);

  return ret;
}

}
