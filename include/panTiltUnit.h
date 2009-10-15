#ifndef _PAN_TILT_UNIT_H_
#define _PAN_TILT_UNIT_H_

#include<stdio.h>
#include<cstdio> 
#include<iostream>
#include"PMDUtils.h"
#include"Biclops.h"
static char dbgClass[] = "PT Unit:";
const char dbgMethod[] = "main:";
#define coutDbg cout << dbgClass <<dbgMethod

using namespace std;

class panTiltUnit
{
 private:
  Biclops *biclops;
  char dbgHdr[32];
  int axisMask;
  PMDAxisControl *panAxis;
  PMDAxisControl *tiltAxis;
  PMDAxisControl::Profile panProfile,tiltProfile;
  bool hasBeenInitialized;

 public:
  panTiltUnit();
  ~panTiltUnit();
  int homing();
  int move(int pan, int tilt);
};

#endif
