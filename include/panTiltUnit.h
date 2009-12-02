#ifndef _PAN_TILT_UNIT_H_
#define _PAN_TILT_UNIT_H_

#include<stdio.h>
#include<cstdio> 
#include<iostream>
#include"PMDUtils.h"
#include"Biclops.h"
static char dbgClass[] = "Biclops:";
const char dbgMethod[] = "";
#define coutDbg cout << dbgClass <<dbgMethod

using namespace std;

class panTiltUnit
{
 private:
  Biclops *biclops;
  int axisMask;
  PMDAxisControl *panAxis;
  PMDAxisControl *tiltAxis;
  PMDAxisControl::Profile panProfile,tiltProfile;
  bool hasBeenInitialized;

 public:
  panTiltUnit();
  ~panTiltUnit();
  int homing();
  int move( double pan, double tilt );
};

#endif
