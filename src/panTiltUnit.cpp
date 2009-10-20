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
  coutDbg << "Phase:Initiallize > START.\n";
  biclops->SetDebugLevel(0);
  if(biclops->Initialize("../data/BiclopsRevI.cfg")) //loading the cfg file
    {
      //Set shortcut to each axis.
      panAxis = biclops->GetAxis(Biclops::Pan);
      tiltAxis = biclops->GetAxis(Biclops::Tilt);
      
      panAxis -> GetProfile(panProfile);
      tiltAxis -> GetProfile(tiltProfile);
      
      cout << "Phase:Initialize > FINISHED.\n";
      hasBeenInitialized = true;
      
      if(hasBeenInitialized){
	biclops->SetDebugLevel(0);
      coutDbg << "Phase:Homing sequence > START.\n";
      if(biclops->HomeAxes(axisMask,true)) //finding the home position
	{
	  coutDbg << "Phase:Homing sequence > FINISHED.\n";
	}
      }
    }
}

panTiltUnit::~panTiltUnit()
{
  panAxis -> DisableAmp();
  tiltAxis -> DisableAmp();
  coutDbg << "Phase:DisableAmp >FINISHED.\n";
}

int panTiltUnit::homing(void)
{
  if (!hasBeenInitialized)
    {
      return -1;
    }
  coutDbg << "Phase:Homing sequence > START.\n";
  biclops->SetDebugLevel(0);
  if(biclops->HomeAxes(axisMask,true)) //finding the home position
    {
      coutDbg << "Phase:Homing sequence > FINISHED.\n";
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
  panProfile.pos += PMDUtils::DegsToRevs(pan);
  tiltProfile.pos += PMDUtils::DegsToRevs(tilt);
    
  
  //Update
  panAxis -> SetProfile(panProfile);
  tiltAxis -> SetProfile(tiltProfile);

  cout << "\tPhase:Move > Start.\n";

  //Start the move
  printf("\tPan(%2.2f)[degrees], Tilt(%2.2f)[degrees].\n",pan,tilt);
  biclops->Move(axisMask);

  cout << "\tPhase:Move > Finished.\n"<<endl;
  
  return 0;
}

