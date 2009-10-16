// ~moriyama/projects/sr3000/trunk/src/connection.cpp
// http://svn.xp-dev.com/svn/cou929_repo/master/sr4000/trunk/src/connection.cpp
//
// 2008-12-22 add memory release process to 'sendCommand'
// 2008-12-09 fix around code process
// 2008-12-07
// Kousei MORIYAMA
//
// class 'connection', extends 'sharedMemory'
// define message code
//

#include <cstdio>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "sharedMemory.h"

using namespace std;

namespace shm
{

connection::connection(char *mapName) : sharedMemory(mapName, 64)
{
  CODE_NAME = (char**)calloc(SHM_CODE_END, sizeof(char));
  CODE_NAME[0] = "SHM_CODE_POINT_TO_PRJ";
  CODE_NAME[1] = "SHM_CODE_JULIUS_TO_POINT";
  CODE_NAME[2] = "SHM_CODE_END";
}

connection::~connection()
{
  free(CODE_NAME);
}

int connection::sendCommand(int code, char *message)
{
  // input: code, int
  //        message, char*
  // output: write code and 'message' to shared memory
  // return: 0
  //
  // Take 'code' and 'message', write these data to shared memory.

  char *string;
  int res;

  // check code existing
  if(codeCheck(code) == -1)
    return -1;

  // make string to write
  sprintf(string, "%d,%s", code, message);

  // write to shared memory
  res = write(string);

  if(res == -1)
    return -1;

  return 0;
}

int connection::getCommand(int code, char *ret)
{
  // input: code, int
  //        pointer to char
  // output: message (part of command), char*
  // return: 0 if succeed
  //
  // If argument 'code' is valid and there are command which has same 'code', return 0,
  // and set argument 'char* ret' the message that is part of command (command format is "code,message").
  // else return -1.

  char *string;
  int tmp;

  // check code existing
  if(codeCheck(code) == -1)
    return -1;

  // read
  string = read();

  // if read string is not contain 'code', exit
  if(atoi(&(string[0])) != code)
    return -1;

  // retrieve message from command, and set message to 'ret'
  sscanf(string, "%d,%s", &tmp, ret); 

  // initilize
  initialize();

  return 0;
}

int connection::codeCheck(int code)
{
  // input: none
  // output: none
  // return: 0 if argument 'code' is exist.
  //
  // Check existing of code.

  if(code >= SHM_CODE_END && code >= 0)
    return -1;

  return 0;
}

int connection::printCode()
{
  // input: none
  // output: print all code
  // return: 0
  //
  // Print all code.

  int i;

  for(i=0; i<SHM_CODE_END; i++)
    printf("%s\n", CODE_NAME[i]);

  return 0;
}

}
