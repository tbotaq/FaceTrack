// ~moriyama/projects/sr3000/trunk/include/sharedMemory.h
// http://svn.xp-dev.com/svn/cou929_repo/master/sr4000/trunk/include/sharedMemory.h
//
// 2008-12-07
// Kousei MORIYAMA
//
// shared memory class header
//

#ifndef __SHARED_MEMORY_H_
#define __SHARED_MEMORY_H__

#include <cstdio>
#include <cstring>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace std;

namespace shm
{

// shared memory
class sharedMemory
{
private:
  char *sharedString;
  int mapFileSize;
  char *mapFileName;
  int fp;

public:
  sharedMemory(char *fileName = "MapFile", int fileSize = 64);
  ~sharedMemory();
  int write(char *src);
  char* read();
  int initialize();
};


// connection
class connection : protected sharedMemory
{
private:
  char **CODE_NAME;
public:
  enum CODE_SET
    {
      SHM_CODE_POINT_TO_PRJ,
      SHM_CODE_JULIUS_TO_POINT,
      SHM_CODE_END
    };
  connection(char *mapName);
  ~connection();
  int sendCommand(int code, char *message);
  int getCommand(int code, char *ret);
  int codeCheck(int code);
  int printCode();
};


// projector connection
class projectorConnection : private connection
{
private:
public:
  projectorConnection(char *mapName);
  int sendCoordinate(CvPoint src);
  CvPoint getCoordinate();
};

}

#endif
