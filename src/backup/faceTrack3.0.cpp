//libraries for image proccess
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"
#include<mathfuncs.h>
//libraies for Biclops 
#include<panTiltUnit.h>
//for multi thread coding
#include<pthread.h>
#include <iostream>
//libraries for time count
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include<stdio.h> 
#define K 0.4 //const used for PID
//library for templateMatching
#include<templateMatching.h>
using namespace std;
using namespace point;

cameraImages *ci;
faceDetector *fd;
panTiltUnit *ptu;
templateMatching *tmch;
pthread_mutex_t mutex;

double getrusageSec(){
  struct rusage t;
  struct timeval s;
  getrusage(RUSAGE_SELF, &t);
  s = t.ru_utime;
  return s.tv_sec + (double)s.tv_usec*1e-6;
}

int dist(int center, int dst)
{
  int ret = K * (center - dst);
  return ret;
}

struct thread_arg
{
  int pan;
  int tilt;
  CvPoint center_t,center_f;
  int radius_t,radius_f;
  double dX;
  double dY;//distance between center.x/y and dst_x/y
  int Lx;//width of window
  int Ly;//height of window
  int dst_x;//destination point
  int dst_y;//destination point
};

void *thread_move(void *_arg_m)//thread for move control
{
  pthread_mutex_lock(&mutex);
  struct thread_arg *arg_m;
  arg_m = (struct thread_arg *)_arg_m;
  ptu->move(arg_m->pan, arg_m->tilt);
  pthread_mutex_unlock(&mutex); 
}


void *thread_facedetect(void *_arg_f)//thread for image proccessing
{ 
  int faces = 0;
  pthread_mutex_lock(&mutex);
  struct thread_arg * arg_f;
  arg_f=(struct thread_arg *)_arg_f;
  arg_f -> dst_x = arg_f -> Lx /2;
  arg_f -> dst_y = arg_f -> Ly /2;

  //acquire current image 
  ci->acquire();
  
  //returns the center location and radius of detected face
  faces = fd -> faceDetect(ci -> getIntensityImg(), &arg_f->center_f, &arg_f->radius_f);
  tmch -> calcMatchResult(ci -> getIntensityImg(),&arg_f->center_t,&arg_f->radius_t);

  //calculate the distance between face's center location and destination
  arg_f -> dX = dist(arg_f -> center_t.x,arg_f -> dst_x);//choose center_f or center_t
  arg_f -> dY = dist(arg_f -> center_t.y,arg_f -> dst_y);
    
  //drow a line from arg_f -> center to destination
  cvLine(ci->getIntensityImg(),cvPoint(arg_f -> dst_x,arg_f -> dst_y),cvPoint(arg_f -> center_f.x,arg_f -> center_f.y),CV_RGB(255,255,255),3,8,0);
  cvLine(ci->getIntensityImg(),cvPoint(arg_f -> dst_x,arg_f -> dst_y),cvPoint(arg_f -> center_t.x,arg_f -> center_t.y),CV_RGB(120,120,120),3,8,0); 
  
  cvCircle(ci->getIntensityImg(),cvPoint(arg_f -> center_f.x,arg_f -> center_f.y),arg_f -> radius_f,CV_RGB(255,255,255),3,8,0);
  cvCircle(ci->getIntensityImg(),cvPoint(arg_f -> center_t.x,arg_f -> center_t.y),arg_f -> radius_t,CV_RGB(120,120,120),3,8,0);
  
  if(1)//faces >= 1)
    {
      //drow a circle the detected face 
      cvCircle(ci->getIntensityImg(),cvPoint(arg_f -> center_f.x,arg_f -> center_f.y),arg_f -> radius_f,CV_RGB(255,255,255),3,8,0);
      cvCircle(ci->getIntensityImg(),cvPoint(arg_f -> center_t.x,arg_f -> center_t.y),arg_f -> radius_t,CV_RGB(120,120,120),3,8,0);
      //define how long PT unit make movement
      arg_f -> pan = arg_f -> dX;
      arg_f -> tilt = arg_f -> dY;
    }
  else 
    {     
      arg_f -> pan = 0;
      arg_f -> tilt = 0;
    }
  // show images
  cvShowImage("Face Detection",ci->getIntensityImg());
  pthread_mutex_unlock(&mutex);
}



int main(void)
{
  ptu = new panTiltUnit();
  ci = new cameraImages();
  tmch = new templateMatching();

  pthread_mutex_init(&mutex,NULL);
  double t1=0,t2=0;
  double totalTime=0;
  int times=0;
  int key;
  struct thread_arg arg;

  // initialize camera image class
  ci->initialize();
  CvSize size = ci -> getImageSize();

  //define the size of recognitive region
  arg.Lx = size.width;
  arg.Ly = size.height;
  fd = new faceDetector(ci->getImageSize());
  
  // make windows
  cvNamedWindow("Face Detection", CV_WINDOW_AUTOSIZE);

  //-----main procces-----//
  while(1)
    {	 
      //ID for move thread and face detection thread
      pthread_t thread_m,thread_f;
      
      t1 = getrusageSec();
      // create threads	
      pthread_create(&thread_f, NULL, thread_facedetect, (void *)&arg);
      pthread_create(&thread_m, NULL, thread_move, (void *)&arg);    
     
      //say goodbye to created threads	
      pthread_join(thread_m, NULL);
      pthread_join(thread_f,NULL);
      t2 = getrusageSec(); 
      printf("time=%f[sec]\n\n",t2-t1);
      totalTime += t2-t1;
      times ++;
   
      // key handling
      key = cvWaitKey(100);
      if(key == 'q')
	{
	  break;
	}
    }
  // release memory
  pthread_mutex_destroy(&mutex);
  cvDestroyWindow("Face Detection");
  printf("\nAverage time is %f[sec/process]\n(calculated by %d processes)\n\n",totalTime/times,times);
  delete ci;
  delete fd;
  delete ptu;
  delete tmch;
 
  return 0;
}
