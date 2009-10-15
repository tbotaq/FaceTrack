// ~moriyama/projects/sr4000/trunk/src/calibDataCollector.cpp
// http://svn.xp-dev.com/svn/cou929_sakanelab/sr4000/trunk/src/calibDataCollector.cpp
//
// 2009-02-07
// Kousei MORIYAMA
//
// Collect data for calibration.
//

#include <cstdio>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"
#include "pointProjector.h"

using namespace std;
using namespace point;
using namespace prj;

void mouse_cam(int event, int x, int y, int flags, void *param);
void mouse_prj(int event, int x, int y, int flags, void *param);

class calibProjector : public pointProjector
{
private:
public:
  calibProjector(int w, int h);
};

class calibData
{
private:
  vector <string> data;
  int prjFlg, camFlg;
  CvPoint prj;
  CvPoint3D32f cam;
  char *fn;
public:
  calibData(char *filename = "calib.txt");
  int set(CvPoint point);
  int set(CvPoint3D32f point);
  int setData();
  int save();
};

calibData *calib = new calibData();

int main(void)
{
  int width = 1024;
  int height = 768;
  char key;
  calibProjector *prj = new calibProjector(width, height);
  cameraImages *ci = new cameraImages();

  // initialize camera image class
  ci->initialize();

  // make windows
  cvNamedWindow("Depth", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Intensity", CV_WINDOW_AUTOSIZE);

  // set callback funtions
  cvSetMouseCallback ("Depth", mouse_cam, ci);
  cvSetMouseCallback ("Intensity", on_mouse_pointing, ci->getDepthImg());

  while(1)
    {
      // acquire current frame
      ci->acquire();

      // show images
      cvShowImage("Depth", ci->getDepthImg());
      cvShowImage("Intensity", ci->getIntensityImg());
      prj->show();

      // key handling
      key = cvWaitKey(10);
      if(key == 'q')
	break;
    }

  calib->save();

  cvDestroyWindow("Depth");
  cvDestroyWindow("Intensity");
  delete ci;
  delete prj;
  delete calib;

  return 0;
}








calibProjector::calibProjector(int w, int h) : pointProjector(w, h)
{
  cvSetMouseCallback(windowName, mouse_prj, this);
}

calibData::calibData(char *filename)
{
  prjFlg = 0;
  camFlg = 0;
  prj = cvPoint(0, 0);
  cam = cvPoint3D32f(0, 0, 0);
  fn = filename;
}

int calibData::set(CvPoint point)
{
  prj = point;
  prjFlg = 1;

  if (prjFlg == 1 && camFlg == 1)
    setData();

  return 0;
}

int calibData::set(CvPoint3D32f point)
{
  cam = point;
  camFlg = 1;

  if (prjFlg == 1 && camFlg == 1)
    setData();

  return 0;
}

int calibData::setData()
{
  char tmp[100];

  sprintf(tmp, "%f %f %f %d %d", (-1)*cam.z, (-1)*cam.x, cam.y, prj.x, prj.y);

  data.push_back(tmp);

  prjFlg = 0;
  camFlg = 0;

  return 0;
}

int calibData::save()
{
  FILE *fp;
  vector <string>::iterator it;

  fp = fopen(fn, "w");
  if(fp == NULL)
    {
      fprintf(stderr, "ERROR: file \"%s\"cannot open\n", fn);
      return -1;
    }

  fprintf(fp, "(\n");
  for (it = data.begin(); it<data.end(); it++)
    fprintf(fp, "(%s)\n", (*it).c_str());
  fprintf(fp, ")\n");

  fclose(fp);

  return 0;
}

void mouse_cam(int event, int x, int y, int flags, void *param)
{
  cameraImages *cam = (cameraImages *)param;
  CvPoint3D32f coordinate;

  if(event == CV_EVENT_LBUTTONDOWN)
    {
      coordinate = cam->getCoordinate(x, y);
      printf("%f, %f, %f\n", coordinate.x, coordinate.y, coordinate.z);
      calib->set(coordinate);
    }
}

void mouse_prj(int event, int x, int y, int flags, void *param)
{
  // input: void *param is pointer to class 'pointProjector'
  //        left button mouse click
  //        right button mouse click
  // output: draw or hide circle on display when left button clicked
  //         show clicked coordinate when right button clicked
  // return: none
  //
  // Callback function.
  // Handle left button mouse click and draw red circle on display.
  // If there are already cricle, hide it.
  // And if right button mouse click is handeled,
  // show coordinate to standard output.
 
  calibProjector *prj = (calibProjector *)param;

  if (event == CV_EVENT_LBUTTONDOWN)
    {
      // draw or hide
      if(prj->hasPoint(x, y))
	prj->hidePoint(x, y);
      else
	{
	  prj->drawPoint(x, y);
	  printf("%d %d\n", x, y);
	  calib->set(prj->convertOriginCenter(cvPoint(x, y)));
	}

      prj->show();
    }
  else if (event == CV_EVENT_RBUTTONDOWN)
    {
      fprintf(stdout, "%d, %d\n", x, y);
      return;
    }
}
