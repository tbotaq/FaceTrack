// ~moriyama/projects/sr4000/trunk/src/oldPointer.cpp
// http://svn.xp-dev.com/svn/cou929_sakanelab/sr4000/trunk/src/oldPpointer.cpp
//
// 2009-01-16 name change from 'pointer' to 'oldPointer'. 'pointer' makes to be main program of pointing detection.
// 2008-12-17 add pointer origin convert process
// 2008-12-07 add shared memory connection class, 'sharedMemory.h' and 'projectorConnection'
// 2008-12-06
// Kousei MORIYAMA
//
// main program to show pointer using projector
//

#include <cstdio>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include "pointProjector.h"
#include "sharedMemory.h"

// for time
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;
using namespace prj;
using namespace shm;

double getrusageSec()
{
  struct rusage t;
  struct timeval s;
  getrusage(RUSAGE_SELF, &t);
  s = t.ru_utime;
  return s.tv_sec + (double)s.tv_usec*1e-6;
}

int main(void)
{
  int width = 1280;
  int height = 1024;
  char key;
  onePointProjector *prj = new onePointProjector(width, height);
  projectorConnection *prjCon = new projectorConnection("MapFilePrj");
  CvPoint point = {-1, -1};
  CvPoint prev = point;

  double t1, t2;

  t1 = getrusageSec();
  while(1)
    {
      // get coordinate from pointing system
      point = prjCon->getCoordinate();
      //      printf("%d, %d\n", point.x, point.y);
    
      // draw point
      prj->showPoint(point);

      // key handlig
      key = cvWaitKey(10);
      if(key == 'q')
	break;
      else if(key == 'p')
	prj->printPointCoordinate(prj->LEFT_TOP);

      t2 = getrusageSec();
      printf("%f\n", t2 - t1);
      t1 = t2;
    }

  return 0;
}


