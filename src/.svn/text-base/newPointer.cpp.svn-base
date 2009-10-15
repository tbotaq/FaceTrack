// ~moriyama/projects/sr4000/trunk/src/pointer.cpp
// http://svn.xp-dev.com/svn/cou929_sakanelab/sr4000/trunk/src/pointer.cpp
//
// 2009-01-16
// Kousei MORIYAMA
//
// main program of detecting pointing direction and showing pointer

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"
#include "pointProjector.h"
#include "sharedMemory.h"
#include "coordinateShifter.h"

using namespace std;
using namespace prj;
using namespace shm;
using namespace cor;


int main(void)
{
  int width = 1280;
  int height = 1024;
  char key;
  onePointProjector *projector = new onePointProjector(width, height);
  pointingDetector *detector = new pointingDetector();
  humanTracker *tracker = new humanTracker();
  
  connection *con = new connection();

  while(1)
    {
      human->track();

      con->listen();

      if (human->isTracked() && listener->receive())
	pointing->detect();

      if (pointing->detected())
	projector->showPoint();

      key = cvWaitKey(10);
      if (key == 'q')
	break;
    }

  delete prj;
  delete pointing;
  delete human;
  delete con;

  return 0;
}


