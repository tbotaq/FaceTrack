#if !defined BICLOPS_H
#define BICLOPS_H

#include <time.h>           // for clock_t and clock
#include <list>
using namespace std;

//#include <stdio.h>          // for FILE defn
#include <cstdio>          // for FILE defn

#include "PMDAxisControl.h"

class Biclops {
public:
    enum Axes { Pan,Tilt,Roll,Verge};
    enum {  PanMask     = 1, 
            TiltMask    = 1<<1, 
            RollMask    = 1<<2, 
            VergeMask   = 1<<3,
            AllAxesMask = PanMask+TiltMask+RollMask+VergeMask};

    Biclops();

    // Load configuration information from the supplied file, connect to 
    // the controller, and populate axes with configuration parameters.
    // This method provides a single call to prepare the controller and
    // interface for axis control.
    bool Initialize(const char *fileName);
    bool IsInitialized() {return hasBeenInitialized;};

    // Get access to the specified axis.
    PMDAxisControl *GetAxis(int axis);
    PMDAxisControl *GetAxis(const char *axis);

    // Axis motion control
    bool HomeAxes(int axes, bool forceHoming = false);
    bool Move(int axes, int32_t waitPeriod = 100);

    // Mounting base attitude measurement (angles are in revolutions). 
    // With pan at home position, base tilt is consistent with actuated tilt 
    // and base roll is consistent with actuated roll.
    // NOTE: This routine will return meaningless values when used with a 
    // Biclops that does not have a tilt sensor.
    void SetLevelBaseAttitude();
    void GetBaseAttitude(double &pitch, double &roll);
    double AnalogToRevolutions(PMDuint16 analogValue, PMDuint16 centerValue);

    // Returns true if any axis of Biclops is powered.
    bool AnAxisIsPowered();

    int GetDebugLevel();
    void SetDebugLevel(int level);

private:

    PMDAxisControl *axes[4];

    bool allAxesOnOneController;
    bool hasBeenInitialized;

    // Analog readings for tilt sensor when device is level.
    // Values must be loaded before readings from base attitude method are valid.
    PMDuint16 rollCenter;
    PMDuint16 pitchCenter;

};
#endif

