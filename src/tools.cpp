#include<tools.h>

tools::tools()
{
}

tools::~tools()
{
}


double tools::getrusageSec()
  {
    struct rusage t;
    struct timeval s;
    getrusage(RUSAGE_SELF, &t);
    s = t.ru_utime;
    return s.tv_sec + (double)s.tv_usec*1e-6;
  }


double tools::getMoveDist(int center, int dst)
  {
    double ret = 0.4 * (center - dst);
    return ret;
  }
