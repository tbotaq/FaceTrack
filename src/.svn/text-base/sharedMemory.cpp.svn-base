// ~moriyama/projects/sr4000/trunk/src/sharedMemory.cpp
// http://svn.xp-dev.com/svn/cou929_sakanelab/sr4000/trunk/src/sharedMemory.cpp
//
// 2008-12-07
// Kousei MORIYAMA
//
// shared memory class
//

#include "sharedMemory.h"

using namespace std;

namespace shm
{

sharedMemory::sharedMemory(char *fileName, int fileSize)
{
  // input: map file size, byte, int (default 64 byte)
  //        map file name, char*     (default "MapFile")
  // output: open map file and maps to memroy
  // return: none
  //
  // Open map file and map it to memory

  // contain arguments
  mapFileSize = fileSize;
  mapFileName = fileName;

  // open map file
  fp = open(mapFileName, O_RDWR|O_CREAT, 0666);
  if(fp == -1)
    {
      perror("open");
      return;
    }

  // file mapping
  sharedString = (char *)mmap(0, mapFileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fp, 0);
  if(sharedString == MAP_FAILED)
    {
      perror("mmap");
      return;
    }
}

sharedMemory::~sharedMemory()
{
  // synchronize memory and file
  msync(sharedString, mapFileSize, 0);

  // unmap
  if(munmap(sharedString, mapFileSize) == -1)
    perror("munmap");

  // file close
  close(fp);
}


int sharedMemory::write(char *src)
{
  // input: charactor or string, char*
  // output: write input to the mapped file
  // return: 0
  //
  // Write input charactors into mapped file.

  int length;

  // check input charactor size
  length = strlen(src);
  if(length >= mapFileSize)
    {
      fprintf(stdout, "ERROR: string length is too long.\n");
      return -1;
    }

  // write input
  strncpy(sharedString, src, mapFileSize);
  msync(sharedString, mapFileSize, 0);

  return 0;
}

char* sharedMemory::read()
{
  // input: none
  // output: none
  // return: charactor or string read from map file, char*
  //
  // Read string from map file and return it

  return sharedString;
}

int sharedMemory::initialize()
{
  // input: none
  // output: set \0 to map file
  // return: 0
  //
  // Set \0 to all byte in the map file.

  int i;

  for(i=0; i<mapFileSize; i++)
    sharedString[i] = '\0';

  msync(sharedString, mapFileSize, 0);

  return 0;
}

}
