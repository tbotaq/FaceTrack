//Takuya Otsubo
//using SR4000 as a camera unit.
//using Biclops as a Pan-Tilt unit.
//Thanks to Kosei Moriyama,who helps me coding.

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
#define K 0.4 //the constant used for PID
//library for templateMatching
#include<templateMatching.h>

using namespace std;
using namespace point;

  void PPP(string string)
  {
  cout<<string<<endl;
  }

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

  //the structure used for threads
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
  bool detectedAbnormalNum;
  bool updatedCenterLoc;
  };

  //the thread for move method
  void *thread_move(void *_arg_m)
  {
  pthread_mutex_lock(&mutex);
  struct thread_arg *arg_m;
  arg_m = (struct thread_arg *)_arg_m;
  ptu->move(arg_m->pan, arg_m->tilt);
  pthread_mutex_unlock(&mutex); 
  }

  //the thread for image processing
void *thread_facedetect(void *_arg_f)
{
  pthread_mutex_lock(&mutex);
  struct thread_arg *arg_f;

  //previous center points
  double prevX=0,prevY=0;

  //difference between present points and those of previous
  int diffX,diffY;      
 
  arg_f=(struct thread_arg *)_arg_f;

  //setting center of window as destination
  arg_f -> dst_x = arg_f -> Lx /2;
  arg_f -> dst_y = arg_f -> Ly /2;

  //acquire current image 
  ci->acquire();

  if(arg_f->updatedCenterLoc)
    {
      prevX = arg_f->center.x;
      prevY = arg_f->center.y;
    }
  else
    {
      prevX=0;
      prevX=0;
    }


  //calculate the center location and radius of matched face
  tmch -> calcMatchResult(ci -> getIntensityImg(),arg_f->templateImage,ci->getImageSize(),&arg_f->center,&arg_f->radius);
  arg_f->updatedCenterLoc = true;

  //calculate the distance between face's center location and destination
  arg_f -> dX = dist(arg_f -> center.x, arg_f -> dst_x);
  arg_f -> dY = dist(arg_f -> center.y, arg_f -> dst_y);
    
  //drow a circle on the detected face,line from face's center location to destination
  cvCircle(ci->getIntensityImg(),cvPoint(arg_f -> center.x,arg_f -> center.y),arg_f -> radius,CV_RGB(255,255,255),3,8,0);
  cvLine(ci->getIntensityImg(),cvPoint(arg_f -> dst_x,arg_f -> dst_y),cvPoint(arg_f -> center.x,arg_f -> center.y),CV_RGB(255,255,255),3,8,0);  

  diffX = abs(arg_f->center.x - prevX);  
  diffY = abs(arg_f->center.y - prevY);

  //printf("Input diff>");
  //scanf("%d",&diffX);
  //diffY = diffX;

  cout<<"diff="<<diffX<<","<<diffY<<endl;
  arg_f->detectedAbnormalNum = false;
  
  if(diffX>100 || diffY>100)
    arg_f->detectedAbnormalNum = true;
  
  if(!(arg_f->detectedAbnormalNum))
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
 
  pthread_mutex_unlock(&mutex);
}


int main(void)
{
  //PPP("0");
  //reference to each function
  ptu = new panTiltUnit();
  //PPP("1");
  ci = new cameraImages();
  tmch = new templateMatching();
  //PPP("2");
  //for time measurement
  double t1=0,t2=0;
  double totalTime=0;
  int times=0;
  //for key handling
  int key;

  //interface to structure
  struct thread_arg arg;
  arg.updatedCenterLoc = false;

  // initialize camera image class
  ci->initialize();
  //PPP("5");
  //estimate the size of image
  ci->acquire();
  CvSize size = ci -> getImageSize();
  //PPP("6");
  //pass the image size to class faceDetector's constructor
  fd = new faceDetector(size);
  //PPP("7");
  //define the size of recognitive region using "size"
  arg.Lx = size.width;
  arg.Ly = size.height;
  //PPP("10");
  //initialize mutex
  pthread_mutex_init(&mutex,NULL);
   
  // define the  window
  cvNamedWindow("Face Detection", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Temp", CV_WINDOW_AUTOSIZE);


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
      tmch->setTempImage(ci->getIntensityImg(),&arg.center,arg.templateImage);
      hasBeenInitialized = true;
      cout<<"SYSTEM:\tFound your face !!"<<endl;
    }
  cvShowImage("Template Image",arg.templateImage);
  
  cout<<"\n\tOK.Now created first template image."<<endl;
    
  cout<<"Biclops:"<<endl;


  //-----main procces-----//
  
  while(1)
    {	 

      ////PPP("15");
      //ID for move thread and face detection thread
      pthread_t thread_m,thread_f;

      //starting time measurement
      t1 = getrusageSec();
      // create threads
      pthread_create(&thread_f, NULL, thread_facedetect, (void *)&arg);
      pthread_create(&thread_m, NULL, thread_move, (void *)&arg);
      ////PPP("19");
      //say goodbye to created threads	
      pthread_join(thread_m, NULL);
      pthread_join(thread_f,NULL);

      //stopping time measurement
      t2 = getrusageSec(); 
      totalTime += t2-t1;
      times ++;
      ////PPP("20");
      if(arg.detectedAbnormalNum)
	{
	  cout<<"SYSTEM:\tDetected abnormal data.Goto Initialization phase."<<endl;
	  goto INITIALIZATION;
	}

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
  ////PPP("100");
  return 0;
}
