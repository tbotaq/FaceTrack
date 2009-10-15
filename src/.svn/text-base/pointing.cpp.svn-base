// ~moriyama/projects/sr4000/trunk/src/pointing.cpp
// http://svn.xp-dev.com/svn/cou929_sakanelab/sr4000/trunk/src/pointing.cpp
//
// 2009-01-13 add intersection function (remove lator)
// 2008-12-15 change callback function from 'on_mouse' to 'on_mouse_getDepth'
// 2008-12-14 change to run without julius call (temporary change for calibration test)
// 2008-12-09 add coordinate shifter and process of panel point calcuration, add test of function 'clipImage'
// 2008-12-07 add shared memory connection class, 'sharedMemory.h' and 'projectorConnection'
// 2008-12-05 add region tracker
// 2008-12-01
// 2008-11-30
// Kousei MORIYAMA
//
// Detect pointing direction and show pointer that point by projector.
// This program assume the situation that human points the panel plane.
//

#include <cstdio>
#include <cstdlib>

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include "libusbSR.h"
#include "definesSR.h"

#include "pointing.h"
#include "sharedMemory.h"
#include "coordinateShifter.h"

using namespace std;
using namespace point;
using namespace shm;
using namespace cor;

// for time
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
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
  cameraImages *ci = new cameraImages();
  connection *con = new connection("MapFileJulius");
  projectorConnection *prjCon = new projectorConnection("MapFilePrj");
  line3DCv *pointingLine = new line3DCv();
  char key;
  CvPoint fingertip2D, elbow2D, subject2D;
  CvPoint3D32f fingertip3D, elbow3D, subject3D;
  IplImage *img;
  int res;
  coordinateShifter *cs = new coordinateShifter();
  double Tx, Ty, Tz, Rx, Ry, Rz, F;

  // initialize camera image class
  ci->initialize();

  // prepare human region tracker
  regionTracker *human = new regionTracker(ci);

  // prepare IplImage (contain result of process)
  img = cvCreateImage(ci->getImageSize(), IPL_DEPTH_8U, 1);

  // prepare coordinate shifter
  FILE *fp;
  char *filename = "param.txt";
  char line[100];

  if((fp = fopen(filename, "r")) == NULL)
    {
      fprintf(stderr, "ERROR: cannot open parameter\n");
      return -1;
    }

  if(fgets(line, 1000, fp) != NULL)
    if((res = sscanf(line, "(#f(%lf %lf %lf) #f(%lf %lf %lf) %lf)\n", &Tx, &Ty, &Tz, &Rx, &Ry, &Rz, &F)) < 1)
      {
	fprintf(stderr, "ERROR: cannot read parameter\nres: %d\n", res);
	return -1;
      }
  fclose(fp);

  // set parameter
  cs->setParameter(Tx, Ty, Tz, Rx, Ry, Rz, F);

  // make windows
  CvPoint windowOrigin = {10, 10};
  cvNamedWindow("Depth", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Intensity", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Result", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("centroid", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("thinning", CV_WINDOW_AUTOSIZE);
  cvMoveWindow ("Depth",  windowOrigin.x, windowOrigin.y);
  cvMoveWindow ("Intensity",  windowOrigin.x, windowOrigin.y + 200);
  cvMoveWindow ("Result",  windowOrigin.x + 200, windowOrigin.y);
  cvMoveWindow ("centroid",  windowOrigin.x + 200, windowOrigin.y + 200);
  cvMoveWindow ("thinning",  windowOrigin.x + 200, windowOrigin.y + 400);

  // set callback funtions
  cvSetMouseCallback ("Depth", on_mouse_getDepth, ci);
  cvSetMouseCallback ("Intensity", on_mouse, ci->getDepthImg());

  double t1, t2;
  t1 = getrusageSec();

  while(1)
    {
      // acquire current frame
      ci->acquire();

      // track human region
      res = human->track();

      // if command is sent from julius and can track human region
      //      if(con->getCommand(con->SHM_CODE_JULIUS_TO_POINT, NULL) == 0 &&  res == 0)
      if(res == 0)
	{
	  // get human region
	  img = human->getResult();
	 
	  // get arm points
	  getArmPoints(img, &fingertip2D, &elbow2D);

	  // get 3D coordinate of arm points
	  fingertip3D = ci->getCoordinate(fingertip2D);
	  elbow3D = ci->getCoordinate(elbow2D);

	  // error check
	  if(fingertip3D.x == -1 || elbow3D.x == -1)
	    continue;

	  // calculate pointing direction
	  pointingLine->setLine(elbow3D, fingertip3D);
	  if(!pointingLine->isValid())
	    continue;

	  // calculate intersection of pointing line and subject object
	  //	  subject3D = getIntersectionObjAndLine(pointingLine, ci);
	  subject3D = calcCoordinateOnPanel(3500, fingertip3D, pointingLine->directionVecror);

	  // calcurate coordinate where to projector points
	  subject2D = cs->world2img((-1)*subject3D.z, (-1)*subject3D.x, subject3D.y);
	  //	  printf("%d, %d\n", subject2D.x ,subject2D.y);

	  // send result to projector
	  prjCon->sendCoordinate(subject2D);

	  cvShowImage("Result", img);

	  t2 = getrusageSec();
	  printf("%f\n", t2 - t1);
	  t1 = t2;
	}

      // show images
      cvShowImage("Depth", ci->getDepthImg());
      cvShowImage("Intensity", ci->getIntensityImg());

      // key handling
      key = cvWaitKey(10);
      if(key == 'q')
	break;

    }

  // release memory
  cvDestroyWindow("Depth");
  cvDestroyWindow("Intensity");
  cvDestroyWindow("Result");
  cvDestroyWindow("thinning");
  cvDestroyWindow("centroid");
  delete ci;
  delete con;
  delete prjCon;
  delete pointingLine;

  return 0;
}
