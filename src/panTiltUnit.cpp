#include "panTiltUnit.h"

panTiltUnit::panTiltUnit()
{
  //The interface to Biclops.
  biclops = new Biclops;

  //Define which axes we want to use.
  axisMask = Biclops::PanMask + Biclops::TiltMask;
  
  //Pointers to each axis.
  panAxis = NULL;
  tiltAxis = NULL;
  
  
  hasBeenInitialized = false;
  
  //Initialize the Biclops unit.
  coutDbg << "Initiallize > START\n";
  biclops->SetDebugLevel(0);
  if(biclops->Initialize("/home/2009/ootsubo/work/SR4000/trunk/data/BiclopsRevI.cfg")) //loading the cfg file
    {
      //Set shortcut to each axis.
      panAxis = biclops->GetAxis(Biclops::Pan);
      tiltAxis = biclops->GetAxis(Biclops::Tilt);
      
      panAxis -> GetProfile(panProfile);
      tiltAxis -> GetProfile(tiltProfile);
      
      cout << "Initialize > FINISHED\n";
      hasBeenInitialized = true;
      
      if(hasBeenInitialized){
	biclops->SetDebugLevel(0);
	coutDbg << "Homing sequence > START\n";
	if(biclops->HomeAxes(axisMask,true)) //finding the home position
	  {
	    coutDbg << "Homing sequence > FINISHED\n";
	  }
      }
    }
}

panTiltUnit::~panTiltUnit()
{
  panAxis -> DisableAmp();
  tiltAxis -> DisableAmp();
  coutDbg << "DisableAmp >FINISHED\n";
}

int panTiltUnit::homing(void)
{
  if (!hasBeenInitialized)
    {
      return -1;
    }
  coutDbg << "Homing sequence > START\n";
  biclops->SetDebugLevel(0);
  if(biclops->HomeAxes(axisMask,true))
    {
      coutDbg << "Homing sequence > FINISHED\n";
    }
  
  return 0;
}

int panTiltUnit::move(double pan, double tilt)
{
  biclops->SetDebugLevel(0);
  if (!hasBeenInitialized)
    {
      return -1;
    }

  //Change the profile to move to a new location
  panProfile.pos += PMDUtils::DegsToRevs((int)pan);
  tiltProfile.pos += PMDUtils::DegsToRevs((int)tilt);
  
  //Update
  panAxis -> SetProfile(panProfile);
  tiltAxis -> SetProfile(tiltProfile);

  //Start the move
  printf("Pan(%2.2f)[degrees], Tilt(%2.2f)[degrees]\n",pan,tilt);
 
  biclops->Move(axisMask);
       
  return 0;
}

