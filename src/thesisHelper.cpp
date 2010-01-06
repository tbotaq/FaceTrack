#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
//for SR4000
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"
//for Biclops 
#include <panTiltUnit.h>
//for templateMatching
#include <templateMatching.h>
//for multi thread coding
#include <pthread.h>
//libraries for time count
#include <sys/time.h>
#include <sys/resource.h>
//etc...
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <tools.h>

using namespace std;
using namespace point;

int main(void)
{

  IplImage *depthImage,*binarizedImage;
  char key = 0;
  cameraImages *ci = new cameraImages();
  ci->initialize();
  regionTracker *human = new regionTracker(ci);


  while(1)
    {
     
      ci->acquire();
      depthImage = ci->getDepthImg();
      human->track();
      binarizedImage = human->getResult();

      cvNamedWindow("Depth Image",CV_WINDOW_AUTOSIZE);
      cvNamedWindow("Binarized Image",CV_WINDOW_AUTOSIZE);

      cvShowImage("Depth Image",depthImage);
      cvShowImage("Binarized Image",binarizedImage);

      key = cvWaitKey(10);
      if(key == 'q')
	break;
    }

  cvDestroyWindow("Depth Image");
  cvDestroyWindow("Binarized Image");

  delete ci;
  delete human;

  return 0;
}
