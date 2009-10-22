//Takuya Otsubo
//using SR4000 as a camera unit.
//using Biclops as a Pan-Tilt unit.
//thanks to Kosei Moriyama,my teacher.

//libraries for image proccess
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"
//#include<mathfuncs.h>
#include<math.h>
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

//declaration of interfaces for each class
cameraImages *ci;
faceDetector *fd;
panTiltUnit *ptu;
templateMatching *tmch;

//for mutex lock
pthread_mutex_t mutex;

//function to measure time
double getrusageSec(){
  struct rusage t;
  struct timeval s;
  getrusage(RUSAGE_SELF, &t);
  s = t.ru_utime;
  return s.tv_sec + (double)s.tv_usec*1e-6;
}

//fuction to calculate the distance between face and destination
double dist(int center, int dst)
{
  double ret = K * (center - dst);
  return ret;
}

//structure for thread control
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
};


//the thread for move method
void *thread_move(void *_arg_m)//thread for move control
{
  pthread_mutex_lock(&mutex);
  struct thread_arg *arg_m;
  arg_m = (struct thread_arg *)_arg_m;
  ptu->move(arg_m->pan, arg_m->tilt);
  pthread_mutex_unlock(&mutex); 
}

//the thread for face detection method
void *thread_facedetect(void *_arg_f)//thread for image proccessing
{
  pthread_mutex_lock(&mutex);
  struct thread_arg *arg_f;
  arg_f=(struct thread_arg *)_arg_f;

  //setting center of window as destination
  arg_f -> dst_x = arg_f -> Lx /2;
  arg_f -> dst_y = arg_f -> Ly /2;

  //acquire current image 
  ci->acquire();

  //calculate the center location and radius of matched face
  tmch -> calcMatchResult(ci -> getIntensityImg(),arg_f->templateImage,ci->getImageSize(),&arg_f->center,&arg_f->radius);

  //calculate the distance between face's center location and destination
  arg_f -> dX = dist(arg_f -> center.x, arg_f -> dst_x);
  arg_f -> dY = dist(arg_f -> center.y, arg_f -> dst_y);
    
  //drow a circle on the detected face and  line from face's center location to destination
  cvCircle(ci->getIntensityImg(),cvPoint(arg_f -> center.x,arg_f -> center.y),arg_f -> radius,CV_RGB(255,255,255),3,8,0);
  cvLine(ci->getIntensityImg(),cvPoint(arg_f -> dst_x,arg_f -> dst_y),cvPoint(arg_f -> center.x,arg_f -> center.y),CV_RGB(255,255,255),3,8,0);  

  double DISTANCE = sqrt(pow(arg_f->dX,2)+pow(arg_f->dY,2));

  if(DISTANCE<10)
    {
      //define how long PT unit make movement
      arg_f -> pan = arg_f -> dX;
      arg_f -> tilt = arg_f -> dY;
      tmch -> setTempImage(ci->getIntensityImg(),&arg_f->center,arg_f->templateImage);
    }
  else 
    {     
      arg_f -> pan = 0;
      arg_f -> tilt = 0;
    }

  //create a new template image using the acquired data
  
  
  cvWaitKey(1);
  pthread_mutex_unlock(&mutex);
}


int main(void)
{
  //reference to each function
  ptu = new panTiltUnit();
  ci = new cameraImages();
  tmch = new templateMatching();

  //for time measurement
  double t1=0,t2=0;
  double totalTime=0;
  int times=0;
  //for key handling
  int key;

  //interface to structure
  struct thread_arg arg;

  // initialize camera image class
  ci->initialize();

  //estimate the size of image
  ci->acquire();
  CvSize size = ci -> getImageSize();
 
  //pass the image size to class faceDetector's constructor
  fd = new faceDetector(size);

  //define the size of recognitive region using "size"
  arg.Lx = size.width;
  arg.Ly = size.height;

  //initialize mutex
  pthread_mutex_init(&mutex,NULL);
   
  // define the  window
  cvNamedWindow("Face Detection", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Temp", CV_WINDOW_AUTOSIZE);

  int faces = 0;
  bool hasCreatedTemp = false;
      
  //-----acquire the template image to use in the first-----//  
  while(!hasCreatedTemp)
    {
      cout<<"SYSTEM:\tPlease show your face to the camera."<<endl;
      
      while(faces<1)
	{
	  ci->acquire();
	  faces = fd->faceDetect(ci->getIntensityImg(),&arg.center,&arg.radius);
	  cout<<"\tSearching sequence in progress..."<<endl;
	  sleep(1);
	}
      arg.templateImage = cvCreateImage(cvSize(arg.radius*2,arg.radius*2),IPL_DEPTH_8U,1);
      tmch->setTempImage(ci->getIntensityImg(),&arg.center,arg.templateImage);
      hasCreatedTemp = true;
      cout<<"SYSTEM:\tFound your face !!"<<endl;
    }
  cvShowImage("Template Image",arg.templateImage);
  
  cout<<"\n\tOK.Now created first template image."<<endl;
  cout<<"\tPress any key to destroy window."<<endl;
  //cvShowImage("Temp",arg.templateImage);
  //cvWaitKey(0);
  //cvDestroyWindow("Temp");

  cout<<"Biclops:"<<endl;
  //-----main procces-----//
  
  while(1)
    {	 
      //ID for move thread and face detection thread
      pthread_t thread_m,thread_f;

      //starting time measurement
      t1 = getrusageSec();
      // create threads
      pthread_create(&thread_f, NULL, thread_facedetect, (void *)&arg);
      pthread_create(&thread_m, NULL, thread_move, (void *)&arg);
     
      //say goodbye to created threads	
      pthread_join(thread_m, NULL);
      pthread_join(thread_f,NULL);

      //stopping time measurement
      t2 = getrusageSec(); 
      totalTime += t2-t1;
      times ++;

      // show images
      cvShowImage("Face Detection",ci->getIntensityImg());
      cvShowImage("Temp",arg.templateImage);

      // key handling
      key = cvWaitKey(100);
      if(key == 'q')
	{
	  break;
	}
    }
  printf("\nAverage time is %f[sec/process]\n(calculated by %d processes)\n\n",totalTime/times,times);
 

  // release memory
  pthread_mutex_destroy(&mutex);
  cvDestroyWindow("Face Detection");
  cvDestroyWindow("Temp");
  delete ci;
  delete ptu;
  delete tmch;
 
  return 0;
}
