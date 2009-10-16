// ~Moriyama/projects/sr4000/trunk/src/funcPointing.cpp
// http://svn.xp-dev.com/svn/cou929_sakanelab/sr4000/trunk/src/funcPointing.cpp
//
// 2009-01-16 add functon 'farther' and 'getIntersectionObjAndLine' for calcrurate intersection of pointing line and object.
// 2008-12-22 add memory release process into 'getArmImage', 'getArmPoints', 'myTinning', 'myOresen' and 'clipImage'.
// 2008-12-15 add function 'on_mouse_getDepth'
// 2008-12-13 add enhounced myOresen by watabe
// 2008-12-02 remove getSilhouette, make class 'regionTracker' (other file)
// 2008-12-01 add namespace 'point'
// 2008-11-30
// Kousei MORIYAMA
//
// functions for pointing direction detection


#include <cstdio>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include "libusbSR.h"
#include "definesSR.h"
#include "pointing.h"

using namespace std;

namespace point {

int getArmImage(const IplImage *src, IplImage *dst)
{
  // input: silhouette image, 8bit, 1channel, binary image
  // output: arm region image, 8bit, 1channlel, binary image
  // return: 0
  //
  // take the silhouette image which represents human region
  // and detect arm region from it

  IplConvKernel* element = cvCreateStructuringElementEx (3, 3, 1, 1, CV_SHAPE_CROSS, NULL);
  int iteration = 14;
  IplImage *tmp = cvCreateImage(cvSize(src->width, src->height), src->depth, src->nChannels);
  CvScalar org, processed, t;
  int i, j;

  // morphorogy
  cvErode (src, tmp, element, iteration);
  cvDilate (tmp, tmp, element, iteration);

  // subtract body region
  cvSetZero(dst);
  for(i=0; i<src->height; i++)
    for(j=0; j<src->width; j++)
      {
	org = cvGet2D(src, i, j);
	processed = cvGet2D(tmp, i, j);

	if(org.val[0] == 255 && processed.val[0] == 0)
	  {
	    t.val[0] = 255;
	    cvSet2D(dst, i, j, t);
	  }
      }

  // noise reduction
  iteration = 1;
  cvErode (dst, dst, element, iteration);
  cvDilate (dst, dst, element, iteration);

  // memory release
  cvReleaseImage(&tmp);

  return 0;
}

int getArmPoints(IplImage *src, CvPoint *dstFinger, CvPoint *dstElbow)
{
  // input: silhouette image, 8bit, 1channel, binary image
  //        pointer to cameraImages class
  // output: 2D coordinate of fingertip and elbow
  //         coordinate bases on camera coordinate
  // return: 0
  //
  // detect 2D coordinate of fingertip and elbow point

  IplConvKernel* element = cvCreateStructuringElementEx (3, 3, 1, 1, CV_SHAPE_CROSS, NULL);
  int iteration = 12;  // recommend to be same value to getArmImage()'s "iteration" 
  IplImage *tmp = cvCreateImage(cvSize(src->width, src->height), src->depth, src->nChannels);
  IplImage *arm = cvCreateImage(cvSize(src->width, src->height), src->depth, src->nChannels);
  IplImage *body = cvCreateImage(cvSize(src->width, src->height), src->depth, src->nChannels);
  CvPoint ansPoint[3];  // [0] and [2] are end points (fingertip or shoulder). [1] is corner point (elbow)
  CvPoint finger2D, elbow2D;

  // get arm region image
  getArmImage(src, arm);

  // get body region image
  cvErode (src, body, element, iteration);
  cvDilate (body, body, element, iteration);

  // thinning
  tmp = myTinning(arm);

  // line approximation
  myOresen(tmp, ansPoint);

  // contain elbow 2D point. this is obviously ansPoint[1]
  elbow2D = ansPoint[1];

  // detect fingertip
  finger2D = detectFingertip(body, ansPoint[0], ansPoint[2]);

  // output result
  dstFinger->x = finger2D.x;
  dstFinger->y = finger2D.y;
  dstElbow->x = elbow2D.x;
  dstElbow->y = elbow2D.y;

  // for debug //////////////////////////////
  cvCircle(tmp, finger2D, 30, CV_RGB(255, 255, 255));
  cvCircle(tmp, elbow2D, 20, CV_RGB(255, 255, 255));
  cvShowImage("thinning", tmp);
  //////////////////////////////////////////

  // memory release
  cvReleaseImage(&tmp);
  cvReleaseImage(&arm);
  cvReleaseImage(&body);

  return 0;
}

CvPoint detectFingertip(IplImage *body, CvPoint candidate1, CvPoint candidate2)
{
  // input: arm image, 8bit, 1channel, binary image
  //        candidates of fingertip point (this points are fingertip or shoulder), CvPoint
  // return: 2D coordinate of fingertip, CvPoint
  //
  // Take arm binary image and coordinate of two points, which are candidates of fingertip.
  // Count white pixel around two candidate's coordinate in arm image
  // and return point which has smaller count number (i guess it is fingertip point).
  // The reason why smaller count number point is fingertip point is that
  // i consider there are no white pixel around the fingertip point,
  // and there are body region (white pixels) around the shoulder point.
  // 
  // TODO:
  // error check, if candidate1 equel to candidate2
  // how to decide range value (now this is fixed)

  int c1Count, c2Count;
  int range;

  range = 10;

  c1Count = countAround(body, candidate1, range);
  c2Count = countAround(body, candidate2, range);

  return (c1Count < c2Count) ? candidate1 : candidate2;
}

int countAround(IplImage *img, CvPoint point, int range)
{
  // input: binary image, 8bit, 1channel
  //        2D coordinate, CvPoint
  //        search range, radius, int
  // return: number of white pixel around the argument 'point' within 'range' pixel
  //
  // count white pixel around the 'point'
  // search range is square which radius is 'range' pixel

  int i, j;
  CvScalar t;
  int ret = 0;
 
  for(i=-range; i<=range; i++)
    for(j=-range; j<=range; j++)
      {
	// skip if (i, j) coordinate if out of image
	if(point.x+i < 0 || point.y+j < 0 || point.x+i >= img->width || point.y+j >= img->height)
	  continue;

	t = cvGet2D(img, point.y + j, point.x + i);

	if(t.val[0] != 0)
	  ret++;
      }
 
  return ret;
}

void myThinningInit(CvMat** kpw, CvMat** kpb)
{
  //kernels for cvFilter2D
  //アルゴリズムでは白、黒のマッチングとなっているのをkpwカーネルと二値画像、
  //kpbカーネルと反転した二値画像の2組に分けて畳み込み、その後でANDをとる

  for (int i=0; i<8; i++){
    *(kpw+i) = cvCreateMat(3, 3, CV_8UC1);
    *(kpb+i) = cvCreateMat(3, 3, CV_8UC1);
    cvSet(*(kpw+i), cvRealScalar(0), NULL);
    cvSet(*(kpb+i), cvRealScalar(0), NULL);
  }

  //kernel1
  cvSet2D(*(kpb+0), 0, 0, cvRealScalar(1));
  cvSet2D(*(kpb+0), 0, 1, cvRealScalar(1));
  cvSet2D(*(kpb+0), 1, 0, cvRealScalar(1));
  cvSet2D(*(kpw+0), 1, 1, cvRealScalar(1));
  cvSet2D(*(kpw+0), 1, 2, cvRealScalar(1));
  cvSet2D(*(kpw+0), 2, 1, cvRealScalar(1));
  //kernel2
  cvSet2D(*(kpb+1), 0, 0, cvRealScalar(1));
  cvSet2D(*(kpb+1), 0, 1, cvRealScalar(1));
  cvSet2D(*(kpb+1), 0, 2, cvRealScalar(1));
  cvSet2D(*(kpw+1), 1, 1, cvRealScalar(1));
  cvSet2D(*(kpw+1), 2, 0, cvRealScalar(1));
  cvSet2D(*(kpw+1), 2, 1, cvRealScalar(1));
  //kernel3
  cvSet2D(*(kpb+2), 0, 1, cvRealScalar(1));
  cvSet2D(*(kpb+2), 0, 2, cvRealScalar(1));
  cvSet2D(*(kpb+2), 1, 2, cvRealScalar(1));
  cvSet2D(*(kpw+2), 1, 0, cvRealScalar(1));
  cvSet2D(*(kpw+2), 1, 1, cvRealScalar(1));
  cvSet2D(*(kpw+2), 2, 1, cvRealScalar(1));
  //kernel4
  cvSet2D(*(kpb+3), 0, 2, cvRealScalar(1));
  cvSet2D(*(kpb+3), 1, 2, cvRealScalar(1));
  cvSet2D(*(kpb+3), 2, 2, cvRealScalar(1));
  cvSet2D(*(kpw+3), 0, 0, cvRealScalar(1));
  cvSet2D(*(kpw+3), 1, 0, cvRealScalar(1));
  cvSet2D(*(kpw+3), 1, 1, cvRealScalar(1));
  //kernel5
  cvSet2D(*(kpb+4), 1, 2, cvRealScalar(1));
  cvSet2D(*(kpb+4), 2, 2, cvRealScalar(1));
  cvSet2D(*(kpb+4), 2, 1, cvRealScalar(1));
  cvSet2D(*(kpw+4), 0, 1, cvRealScalar(1));
  cvSet2D(*(kpw+4), 1, 1, cvRealScalar(1));
  cvSet2D(*(kpw+4), 1, 0, cvRealScalar(1));
  //kernel6
  cvSet2D(*(kpb+5), 2, 0, cvRealScalar(1));
  cvSet2D(*(kpb+5), 2, 1, cvRealScalar(1));
  cvSet2D(*(kpb+5), 2, 2, cvRealScalar(1));
  cvSet2D(*(kpw+5), 0, 2, cvRealScalar(1));
  cvSet2D(*(kpw+5), 0, 1, cvRealScalar(1));
  cvSet2D(*(kpw+5), 1, 1, cvRealScalar(1));
  //kernel7
  cvSet2D(*(kpb+6), 1, 0, cvRealScalar(1));
  cvSet2D(*(kpb+6), 2, 0, cvRealScalar(1));
  cvSet2D(*(kpb+6), 2, 1, cvRealScalar(1));
  cvSet2D(*(kpw+6), 0, 1, cvRealScalar(1));
  cvSet2D(*(kpw+6), 1, 1, cvRealScalar(1));
  cvSet2D(*(kpw+6), 1, 2, cvRealScalar(1));
  //kernel8
  cvSet2D(*(kpb+7), 0, 0, cvRealScalar(1));
  cvSet2D(*(kpb+7), 1, 0, cvRealScalar(1));
  cvSet2D(*(kpb+7), 2, 0, cvRealScalar(1));
  cvSet2D(*(kpw+7), 1, 1, cvRealScalar(1));
  cvSet2D(*(kpw+7), 1, 2, cvRealScalar(1));
  cvSet2D(*(kpw+7), 2, 2, cvRealScalar(1));
}

IplImage* myTinning(IplImage* src)
{
  //白黒それぞれ8個のカーネルの入れ物
  CvMat** kpb = new CvMat *[8];
  CvMat** kpw = new CvMat *[8];
  myThinningInit(kpw, kpb);

  IplImage* dst = cvCloneImage(src);
  //32Fの方が都合が良い
  IplImage* src_w = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
  IplImage* src_b = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
  IplImage* src_f = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
  cvScale(src, src_f, 1/255.0, 0);

  //原画像を2値化(しきい値はo用途に合わせて考える)
  //src_f:2値化した画像(32F)
  //src_w:作業バッファ
  //src_b:作業バッファ(反転)
  cvThreshold(src_f,src_f,0.5,1.0,CV_THRESH_BINARY);
  cvThreshold(src_f,src_w,0.5,1.0,CV_THRESH_BINARY);
  cvThreshold(src_f,src_b,0.5,1.0,CV_THRESH_BINARY_INV);

  //1ターンでマッチしてなければ終了
  double sum=1;
  while(sum>0){
    sum=0;
    for (int i=0; i<8; i++){
      cvFilter2D(src_w, src_w, *(kpw+i));
      cvFilter2D(src_b, src_b, *(kpb+i));
      //各カーネルで注目するのは3画素ずつなので、マッチした注目画素の濃度は3となる
      //カーネルの値を1/9にしておけば、しきい値は0.99で良い
      cvThreshold(src_w,src_w,2.99,1,CV_THRESH_BINARY); //2.5->2.99に修正
      cvThreshold(src_b,src_b,2.99,1,CV_THRESH_BINARY); //2.5->2.99
      cvAnd(src_w, src_b, src_w);
      //この時点でのsrc_wが消去候補点となり、全カーネルで候補点が0となった時に処理が終わる
      sum += cvSum(src_w).val[0];
      //原画像から候補点を消o去(二値画像なのでXor)
      cvXor(src_f, src_w, src_f);
      //作業バッファを更新
      cvCopyImage(src_f, src_w);
      cvThreshold(src_f,src_b,0.5,1,CV_THRESH_BINARY_INV);
    }
  }

  //8Uの画像に戻して表示thinning_test.jp
  cvConvertScaleAbs(src_f, dst, 255, 0);    

  // memory release
  cvReleaseImage(&src_w);
  cvReleaseImage(&src_b);
  cvReleaseImage(&src_f);

  return dst;
}


IplImage*  myOresen(IplImage* src , CvPoint* Ap)
{

  //CvPoint Ap[3];
  CvPoint Kaku;
  CvPoint pt[ptMax],buf_pt[ptMax];
  int s,x,y,dx,dy,b1,prep,ptNow,buf_ptNow;
  int Wsum;//白い点の集まり
  uchar p;//注目点の画素
  uchar pm[8];//注目点の回りの画素
  //     int ckw[8][7] //点をチェックする順番
  //       //チェックする順番の優先順位
  //       = {{4,3,5,2,6,1,7},
  //     	 {5,4,6,3,7,2,0},//
  // 	 {6,5,7,4,0,3,1},
  // 	 {7,6,0,5,1,4,2},
  // 	 {0,7,1,6,2,5,3},//
  // 	 {1,0,2,7,3,6,4},//
  // 	 {2,1,3,0,4,7,5},//
  // 	 {3,2,4,1,5,0,6}};

  int preth;
  int min_thdel;
  int yosouth;

  IplImage* dst;

  dst = cvCloneImage(src);

  ptNow = 0;//主線の点の数
  for (y = 0; y < dst->height; y++){
    for (x = 0; x < dst->width; x++){
      dx = x;
      dy = y;

      //注目点の画素を抽出
      p = dst->imageData[ dst->widthStep * dy + dx];
 
      //Step1 端点の抽出

      //step1-1 注目点(のR)が0なら除外
      if (p != 255) continue;

      //step1-2 注目点の回りの白の数が1以外なら除外

      b1 = -99;//端点チェックに用いる
      Wsum = 0;//白の合計
      WsumCal(dx + 1,dy    , 0)
	WsumCal(dx + 1,dy + 1, 1)
	WsumCal(dx    ,dy + 1, 2)
	WsumCal(dx - 1,dy + 1, 3)
	WsumCal(dx - 1,dy    , 4)
	WsumCal(dx - 1,dy - 1, 5)
	WsumCal(dx    ,dy - 1, 6)
	WsumCal(dx + 1,dy - 1, 7)

	if (Wsum != 1) continue;

      //ここまで来ていれば、端点である
      prep = 0;

      //Step2 端点からリストを作成する(2008/10/08)
      
      preth = -1;//θ
 
      buf_ptNow = 0;
      for(;;){

	//注目点をリストに加える
	buf_pt[buf_ptNow].x = dx;
	buf_pt[buf_ptNow].y = dy; 
	//リストにいれたら、画素を100とする
	dst->imageData[ dst->widthStep * dy + dx] = 100;
	buf_ptNow++;

	//曲線リストに入りきらなければ作業終了
	if (buf_ptNow == ptMax) break;

	//-------------------------------------------------------------
	//最適解を見つけ出す新しいやり方　081117

	b1 = -99;
	Wsum = 0;//白の合計
	min_thdel = 999;//現在の候補となるθの差
	yosouth = -1;//次のprethになる候補
    
	WsumCal2(dx + 1,dy    , 0)
	  WsumCal2(dx + 1,dy + 1, 1)
	  WsumCal2(dx    ,dy + 1, 2)
	  WsumCal2(dx - 1,dy + 1, 3)
	  WsumCal2(dx - 1,dy    , 4)
	  WsumCal2(dx - 1,dy - 1, 5)
	  WsumCal2(dx    ,dy - 1, 6)
	  WsumCal2(dx + 1,dy - 1, 7)

	  if (Wsum == 0) break;

	preth = yosouth;

	//-------------------------------------------------------------
	/*
	//注目点の回りの白の数が0ならば終了
	b1 = -99;//端点チェックに用いる
	Wsum = 0;//白の合計
       
	//チェックする順番を考える
	for (s = 0; s < 7 ; s ++){
	//優先順位の低い方から探索
	switch (ckw[prep][6 - s]){
	case 0:
	WsumCal(dx + 1,dy    , 0)
	break;
	case 1:
	WsumCal(dx + 1,dy + 1, 1)
	break;
	case 2:
	WsumCal(dx    ,dy + 1, 2)
	break;
	case 3:
	WsumCal(dx - 1,dy + 1, 3)
	break;
	case 4:
	WsumCal(dx - 1,dy    , 4)
	break;
	case 5:
	WsumCal(dx - 1,dy - 1, 5)
	break;
	case 6:
	WsumCal(dx    ,dy - 1, 6)
	break;
	case 7:
	WsumCal(dx + 1,dy - 1, 7)
	break;
	}
	}

	//最初の端点だけは、仮に決めた前の位置も候補に入れる
	if (buf_ptNow == 1) 
	WsumCal(dx + 1,dy    , 0)
      

	//注目画素が無ければ無限ループから出る
	if (Wsum == 0) break;*/

	//注目画素を変更する
	switch (b1){
	case 0:
	  ch_dxdy( 1, 0);
	  //prep = 4;
	  break;
	case 1:
	  ch_dxdy( 1, 1);
	  //prep = 5;
	  break;
	case 2:
	  ch_dxdy( 0, 1);
	  //prep = 6;
	  break;
	case 3:
	  ch_dxdy(-1, 1);
	  //prep = 7;
	  break;
	case 4:
	  ch_dxdy(-1, 0);
	  //prep = 0;
	  break;
	case 5:
	  ch_dxdy(-1,-1);
	  //prep = 1;
	  break;
	case 6:
	  ch_dxdy( 0,-1);
	  //prep = 2;
	  break;
	case 7:
	  ch_dxdy( 1,-1);
	  //prep = 3;
	  break;
	default:
	  printf("予期せぬエラーです。");
	}

      }//無限ループ

      if (buf_ptNow > ptNow)
	{
	  memcpy(pt,buf_pt,sizeof(pt));
	  ptNow = buf_ptNow;
	}
      //遅ければ修正対象とする
      for(s=0 ;s < buf_ptNow ; s++)
	{
	  //元の画素に戻す
	  //	  dst->imageData[ dst->widthStep * buf_pt[s].y + buf_pt[s].x] = 255;
	  dst->imageData[ dst->widthStep * buf_pt[s].y + buf_pt[s].x] = 127;
	}
    }//x
  }//y

  //遅ければ修正対象とする
  //本リストに入っている直線の画素を変更
  for(s=0 ;s < ptNow ; s++)
    dst->imageData[ dst->widthStep * pt[s].y + pt[s].x] = 100;	

  //角点を求める
  Kaku = DecideKakupt(pt,ptNow);

  Ap[0] = pt[0];//始点
  Ap[1] = Kaku;//角点
  Ap[2] = pt[ptNow - 1];//終点

  // release memory
  cvReleaseImage(&dst);

  return dst;

}
  
  //θ関連を処理する関数
int calth(int _n, int _preth,int _b1, int* min_thdel_ad, int* yosouth_ad)
{
  int ckth[8] = {0,45,90,135,180,225,270,315};
  int t = ckth[_n];
  int p = _preth;
  int delth = 9999;

  if (t >= p){
    delth = t - p;
    if (delth >= 180){
      delth = 360 - delth;
      p = p + 360;
    }
  }else{
    delth = p - t;
    if (delth >= 180){
      delth = 360 - delth;
      t = t + 360;
    }
  }

  if (delth < *min_thdel_ad){
    *min_thdel_ad = delth;
    *yosouth_ad = (p + t) / 2;
    if (*yosouth_ad >= 360) *yosouth_ad -= 360; 
    _b1 = _n;
  }

  return _b1;
}

CvPoint DecideKakupt(CvPoint _pt[] , int _ptNow)
{
  //角点らしい点を返す関数

  CvPoint Anspt;

  //１つの点ならそのまま返す
  if ( (_ptNow - 1) <= 0 ) return _pt[0];

  int i;
  double x0,y0,x1,y1,x2,y2,a,b,c,d,maxd,_a,_b;

  x1=(double)_pt[0].x;
  y1=(double)_pt[0].y;
  x2=(double)_pt[_ptNow - 1].x;
  y2=(double)_pt[_ptNow - 1].y;

  //printf("%f",x1);
  if (x1 == x2) {
    a = 1;
    b = 0;
    c = 0 - x1;
  }else{
    _a = (y2-y1)/(x2-x1);
    _b = (y1*x2 - y2*x1)/(x2-x1);
    //y = _ax + _b -> ax + by + c = 0 へ
    a = _a;
    b = -1;
    c = _b;
  }


  // printf("a=%f\n",a);
  // printf("b=%f\n",b);
  // printf("c=%f\n",c);

  maxd = -1;
  for (i= 0; i < _ptNow ; i++){
    x0 = (double)_pt[i].x;
    y0 = (double)_pt[i].y;
    //点と直線の関係式で距離を出す
    d = fabs(a * x0 + b * y0 + c) / sqrt ( a * a + b * b) ;

    //printf("d=%f\n",d);
    
    if (d <= maxd) continue;

    // printf("ok");
    
    Anspt = _pt[i]; 

    maxd = d;
  }
 
  if( maxd == -1) printf ("予期せぬ値：距離が-1");

  return Anspt;
}

void on_mouse_pointing(int event, int x, int y, int flags, void *param)
{
  IplImage *img = (IplImage *)param;
  CvScalar imgValue;

  if(event == CV_EVENT_LBUTTONDOWN)
    {
      imgValue = cvGet2D(img, y, x);
      printf("%d, %d, %f\n", x, y, imgValue.val[0]);
    }
}

void on_mouse_getDepth(int event, int x, int y, int flags, void *param)
{
  cameraImages *cam = (cameraImages *)param;
  CvPoint3D32f coordinate;

  if(event == CV_EVENT_LBUTTONDOWN)
    {
      coordinate = cam->getCoordinate(x, y);
      printf("%f, %f, %f\n", coordinate.x, coordinate.y, coordinate.z);
    }
}

int calcDirectionVector( CvPoint3D32f  _P , CvPoint3D32f _Q , CvPoint3D32f* _d)
{
  _d->x = (int)_Q.x - (int)_P.x;
  _d->y = (int)_Q.y - (int)_P.y;
  _d->z = (int)_Q.z - (int)_P.z;

  float z;
  z = sqrt(_d->x * _d->x + _d->y * _d->y + _d->z * _d->z) ;
  if (z == 0)
    return -1;

  _d->x = _d->x / z;
  _d->y = _d->y / z;
  _d->z = _d->z / z;

  return 0;
}

CvPoint3D32f calcCoordinateOnPanel(int panelDistance, CvPoint3D32f startPoint, CvPoint3D32f directionVec)
{
  CvPoint3D32f ret;

  ret.x = directionVec.x * ( panelDistance - startPoint.z) / directionVec.z + startPoint.x;
  ret.y = directionVec.y * ( panelDistance - startPoint.z) / directionVec.z + startPoint.y;
  ret.z = panelDistance;

  return ret;
}

int clipImage(IplImage *img, CvPoint origin, int width, int height, char* fileName)
{
  // input: image, IplImage
  //        origin (left top point of region you want to clip), CvPoint
  //        width of region, int
  //        height of region, int
  //        file name, char*
  // output: save a portion of image
  // return: 0 if succeed
  //
  // save a portion of iamge, which origin, width and height are argument, to the file which name if 'fileName'

  IplImage *dst;
  CvScalar tmp;
  int i, j;

  // check range of 'origin', 'width' and 'height'
  if(origin.x < 0 || origin.y < 0 || origin.x >= img->width || origin.y >= img->height)
    return -1;
  if(width <= 0 || origin.x+width >= img->width)
    return -1;
  if(height <= 0 || origin.y+height >= img->height)
    return -1;

  // allocate image memory which contain clipped image
  dst = cvCreateImage(cvSize(width, height), img->depth, img->nChannels);
  cvSetZero(dst);

  // copy pixel values from source image to 'dst'
  for(i=origin.x; i<origin.x+width; i++)
    for(j=origin.y; j<origin.y+height; j++)
      {
	tmp = cvGet2D(img, j, i);
	cvSet2D(dst, j-origin.y, i-origin.x, tmp);
      }

  // save image
  cvSaveImage(fileName, dst);

  // release memory
  cvReleaseImage(&dst);

  return 0;
}

#if 0
  CvPoint3D32f calcIntersection(const IplImage *depth, const CvPoint3D32f startPoint, const CvPoint3D32f directionVec)
  {
    // input: depth image, IplImage
    //        start point of straight line, CvPoint3D32f
    //        direction vector, CvPoint3D32f
    // output: none
    // return: a point of intersection of direction vector and object in depth image, CvPoint3D32f
    //
    // Calcurate the point of intersection of direction vector and object in depth image.
    //
    // TODO:
    // End condition of for loop. Now simply do 5000 iteration.

    CvPoint3D32f ret = {-1, -1, -1};
    CvPoint3D32f tmp;
    int i;

    tmp = startPoint;

    for (i=0; i<5000; i++)
      {
	tmp = calcCoordinateByDepth(tmp.z, startPoint, directionVec);
	if (searchPointAtDepthImage(depth, tmp) == 0)
	  {
	    ret = tmp;
	    break;
	  }
      }

    return ret;
  }

  CvPoint3D32f calcCoordinateByDepth(int panelDistance, CvPoint3D32f startPoint, CvPoint3D32f directionVec)
  {
    CvPoint3D32f ret;

    ret.x = directionVec.x * ( panelDistance - startPoint.z) / directionVec.z + startPoint.x;
    ret.y = directionVec.y * ( panelDistance - startPoint.z) / directionVec.z + startPoint.y;
    ret.z = panelDistance;

    return ret;
  }

  int searchPointAtDepthImage(const IplImage *depth, const CvPoint3D32f point)
  {
    // input: depth Image, IplImage
    //        point to search, CvPoint
    // output: none
    // return: 0 if point found, -1 if not found.
    //
    // Search the 'point' from 'depth'. 


  }
#endif

bool farther(CvPoint3D32f p1, CvPoint3D32f p2)
{
  return p1.z < p2.z;
}

CvPoint3D32f getIntersectionObjAndLine(line3DCv *srcLine, cameraImages *ci)
{
  // input: direction of pointing, line3D
  //        dpeth image, IplImage
  // output: 
  // return:
  //
  // Calcurate the point of intersection of direction vector and object in depth image.
  //
  // TODO:
  // End condition of for loop. Now simply do 5000 iteration.

  CvPoint3D32f tmp, ret = {-1, -1, -1};
  double v;
  int i, j, sign = 1, threshold = 5;
  vector <CvPoint3D32f> sorted;
  vector <CvPoint3D32f>::iterator it;
  CvSize size = ci->getImageSize();

  for (j=0; j<size.height; j++)
    for (i=0; i<size.width; i++)
      sorted.push_back(ci->getCoordinate(i, j));
  sort(sorted.begin(), sorted.end(), farther);

  v = srcLine->getPoint().z;
  if (srcLine->getDirectionVector().z < 0)
    sign = -1;
  else if (srcLine->getDirectionVector().z == 0)
    sign = 0;

  for (i=0; i<5000; i++)
    {
      tmp = srcLine->getLinePointByZ(v + sign*i);
      it = lower_bound(sorted.begin(), sorted.end(), cvPoint3D32f(0, 0, tmp.z-threshold), farther);
      for (; ((*it).z < tmp.z + threshold) && (it != sorted.end()); it++)
	if ((*it).x >= tmp.x - threshold && (*it).y >= tmp.y - threshold && (*it).x < tmp.x + threshold && (*it).y < tmp.y + threshold)
	  {
	    ret = tmp;
	    break;
	  }
    }

  return ret;
}

double getError(CvPoint3D32f v1, CvPoint3D32f v2)
{
  double ret = 0;

  ret += (v1.x - v2.x) * (v1.x - v2.x);
  ret += (v1.y - v2.y) * (v1.y - v2.y);
  ret += (v1.z - v2.z) * (v1.z - v2.z);

  return ret;
}

CvPoint3D32f getMarkCoord(line3DCv *line, cameraImages *ci, double threshold)
{
  CvSize size = ci->getImageSize();
  CvPoint3D32f curPoint, linePoint, minErrorPoinit = cvPoint3D32f(-1, -1, -1), ret = cvPoint3D32f(-1, -1, -1);
  double curError, minError = 10000;
  int i, j;

  for (i=0; i<size.height; i++)
    for (j=0; j<size.width; j++)
      {
	curPoint = ci->getCoordinate(cvPoint(i, j));
	linePoint = line->getLinePointByZ(curPoint.z);
	curError = getError(curPoint, linePoint);
	if (curError < minError)
	  {
	    minError = curError;
	    minErrorPoinit = curPoint;
	  }
      }

  if (minError < threshold)
    ret = minErrorPoinit;

  return ret;
}


} // ! namespace point
