#ifndef _TOOLS_H_
#define _TOOLS_H_

#include<sys/time.h>
#include<sys/resource.h>

class tools
{
 private:
  int center;
  int dst;

 public:
  tools();
  ~tools();
  double getrusageSec();
  double getMoveDist(int center, int dst);
};

#endif
