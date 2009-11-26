//using SR4000 as a camera unit.
//using Biclops as a Pan-Tilt unit.

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

cameraImages *ci;
faceDetector *fd;
panTiltUnit *ptu;
templateMatching *tmch;
regionTracker *human;
tools *tool;

//mutex lock
pthread_mutex_t mutex;

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                      theread definition                                     //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

//the structure used by threads
struct thread_arg
{
  double pan;
  double tilt;
  CvPoint center;
  int radius;
  double dX;
  double dY; //distance between center.x/y and dst_x/y
  int Lx;    //width of window
  int Ly;    //height of window
  int dst_x; //destination point
  int dst_y; //destination point
  IplImage *templateImage;
  bool detectedAbnormalValue;
  bool updatedCenterLoc;
};

//the thread for image processing
void *thread_facedetect(void *_arg_f)
{
  //lock
  pthread_mutex_lock(&mutex);
  
  struct thread_arg *arg_f;
  
  //previous center points
  double prevX=0,prevY=0;
  
  //difference between present points and those of previous
  int diffX,diffY;      
  
  arg_f=(struct thread_arg *)_arg_f;

  //setting center of window as destination
  arg_f->dst_x = arg_f->Lx /2;
  arg_f->dst_y = arg_f->Ly /2;
    
  if(arg_f->updatedCenterLoc)
    {
      prevX = arg_f->center.x;
      prevY = arg_f->center.y;
    }
  else
    {
      prevX = 0;
      prevX = 0;
    }
  
  //acquire current image 
  ci->acquire();  

  //calculate the center location and radius of matched face
  tmch->calcMatchResult(human->getResult(),arg_f->templateImage,ci->getImageSize(),&arg_f->center,&arg_f->radius);
  arg_f->updatedCenterLoc = true;
  
  //calculate the distance between face's center location and destination
  arg_f->dX = tool->dist(arg_f->center.x, arg_f->dst_x);
  arg_f->dY = tool->dist(arg_f->center.y, arg_f->dst_y);
  
  diffX = abs(arg_f->center.x - prevX);  
  diffY = abs(arg_f->center.y - prevY);
  
 
  arg_f->detectedAbnormalValue = false;
  
  if(diffX>100 || diffY>100 || tmch->getErrorValue()>0.2)
    {
      cerr<<"++++Detected Abnormal Value++++"<<endl;
      cerr<<"++++diffX,diffY="<<diffX<<","<<diffY<<endl;
      cerr<<"++++ErrorValue="<<tmch->getErrorValue()<<endl;
      arg_f->detectedAbnormalValue = true;
    }

  if(!(arg_f->detectedAbnormalValue))
    {
      //define how long PT unit make movement
      arg_f->pan = arg_f->dX;
      arg_f->tilt = arg_f->dY;
      tmch->setTempImage(human->getResult(),&arg_f->center,&arg_f->radius,arg_f->templateImage);
    }
  else 
    {     
      arg_f->pan = 0;
      arg_f->tilt = 0;
    }
  
  //drow a circle on the detected face and the line from face's center location to destination
  cvCircle(ci->getIntensityImg(),cvPoint(arg_f->center.x,arg_f->center.y),arg_f->radius,CV_RGB(255,255,255),3,8,0);
  cvLine(ci->getIntensityImg(),cvPoint(arg_f->dst_x,arg_f->dst_y),cvPoint(arg_f->center.x,arg_f->center.y),CV_RGB(255,255,255),3,8,0);    
 
  pthread_mutex_unlock(&mutex);
}

//the thread for move method
void *thread_move(void *_arg_m)
{
  pthread_mutex_lock(&mutex);
  struct thread_arg *arg_m;
  arg_m = (struct thread_arg *)_arg_m;
  ptu->move(arg_m->pan, arg_m->tilt);
  pthread_mutex_unlock(&mutex); 
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                      main program                                           //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
  ptu = new panTiltUnit();
  ci = new cameraImages();
  tmch = new templateMatching();
  tool = new tools();

  //for time measurement
  double t1=0,t2=0;
  double totalTime=0;
  int times=0;
  
  //for key handling
  int key;

  //interface to structure
  struct thread_arg arg;

  arg.pan=0;
  arg.tilt=0;

  // initialize camera image class
  ci->initialize();

  //estimate the size of image
  ci->acquire();
  human = new regionTracker(ci);
  CvSize size = ci->getImageSize();
  fd = new faceDetector(size);

  //initialize mutex
  pthread_mutex_init(&mutex,NULL);
   
  //define the size of recognitive region using "size"
  arg.Lx = size.width;
  arg.Ly = size.height;

  // define the  window
  cvNamedWindow("Result", 0);
  cvNamedWindow("Current Template Image", 0);
  cvNamedWindow("Human Image(Source Image)", 0);
  cvNamedWindow("Depth Image", 0);
 
  //flag initialization
  arg.updatedCenterLoc = false;

 INITIALIZATION://LABEL

  cout<<"SYSTEM:INITIALIZATION"<<endl;
  
  int faces = 0;
  bool hasBeenInitialized = false;
      
  //-----acquire the template image to use in the first-----//  
  while(!hasBeenInitialized)
    {
     
      cout<<"\tPlease show your face to the camera."<<endl;
      
      while(faces<1)
	{
	  ci->acquire();
	  faces = fd->faceDetect(ci->getIntensityImg(),&arg.center,&arg.radius);
	  //cout<<"\tSearching sequence in progress..."<<endl;
	  //sleep(1);
	}
      arg.templateImage = cvCreateImage(cvSize(arg.radius*2,arg.radius*2),IPL_DEPTH_8U,1);
      tmch->setTempImage(human->getResult(),&arg.center,&arg.radius,arg.templateImage);
      hasBeenInitialized = true;
      cout<<"SYSTEM:\tFound your face !!"<<endl;
    }
  
  cout<<"\n\tOK.Now created first template image."<<endl;
    
  cout<<"Biclops:"<<endl;

  int abnormTimes = 0;

  //-----main procces-----//
  while(1)
    {	 
      //ID for move thread and face detection thread
      pthread_t thread_m,thread_f;
    
      human->track();
 
      //starting time measurement
      t1 = tool->getrusageSec();
      
      // create threads
      pthread_create(&thread_f, NULL, thread_facedetect, (void *)&arg);
      pthread_create(&thread_m, NULL, thread_move, (void *)&arg);

      //say goodbye to created threads	
      pthread_join(thread_m, NULL);
      pthread_join(thread_f, NULL);

      //stopping time measurement
      t2 = tool->getrusageSec(); 
      totalTime += t2-t1;
      times ++;
            
      //if(arg.detectedAbnormalValue)
      //{
      //abnormTimes++;
      //cout<<"###detected abnormal number ("<<abnormTimes<<")"<<endl;
      //if(abnormTimes==5)
      //  {
      //  cout<<"SYSTEM:\tDetected abnormal data.Go to Initialization phase."<<endl;
      //goto INITIALIZATION;
      // }
      //	}
      //else 
      //abnormTimes = 0;

      // show images
      cvShowImage("Result",ci->getIntensityImg());
      cvShowImage("Current Template Image",arg.templateImage);
      cvShowImage("Human Image(Source Image)",human->getResult());
      cvShowImage("Depth Image",ci->getDepthImg());
 
      cout<<"Average Depth ="<<tmch->getAvgDepth(human->getResult(),ci->getDepthImg())<<endl;

      // key handling
      key = cvWaitKey(100);
      if(key == 'q')
	{
	  break;
	}
      if(key == 'i')
	{
	  cout<<"SYSTEM:\tforce initialize"<<endl;
	  goto INITIALIZATION;
	}
      //goto INITIALIZATION;
    }

  printf("\nAverage time is %f[sec/process]\n(calculated by %d processes)\n\n",totalTime/times,times);
 
  // release memory
  pthread_mutex_destroy(&mutex);
  cvDestroyWindow("Result");
  cvDestroyWindow("Current Template Image");
  cvDestroyWindow("Human Image(Source Image)");
  cvDestroyWindow("Depth Image");

  delete ci;
  delete ptu;
  delete tmch;
  delete human;
  delete tool;

  return 0;
}
