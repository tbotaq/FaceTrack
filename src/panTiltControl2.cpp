#include <stdio.h>
#include <unistd.h>

FILE *fp;
char *filename = "coordinates.txt";
int getPoints(int *facex, int *facey, int *elbowx, int *elbowy, int *handx, int *handy);
int move(int x, int y);

int main(void) {
  if((fp = fopen(filename, "r")) == NULL) {
    printf("file open error\n");
  }

  int facex, facey, elbowx, elbowy, handx, handy;
  int ofacex,ofacey,oelbowx,oelbowy,ohandx,ohandy;
  int mx,my,ox,oy;

  while (getPoints(&facex, &facey, &elbowx, &elbowy, &handx, &handy) == 0)
    {
  
      if((facex>=0)&&(elbowx>=0)&&(handx>=0)) 
	{ 
	  mx=(facex+elbowx+handx)/3;my=(facey+elbowy+handy)/3;
	}
      else if((facex>=0)&&(elbowx>=0)&&(handx<0))
	{
	  mx=ohandx;my=ohandy;  move(mx,my);
	}
      else if((facex>=0)&&(elbowx<0)&&(handx>=0))
	{
	  mx=oelbowx;my=oelbowy;  move(mx,my);
	}
      else if((facex<0)&&(elbowx>=0)&&(handx>=0))
	{
	  mx=ofacex;my=ofacey;  move(mx,my);
	}
      else if((facex>=0)&&(elbowx<0)&&(handx<0))
	{
	  mx=(oelbowx+ohandx)/2;my=(oelbowy+ohandy)/2;  move(mx,my);
	}
      else if((facex<0)&&(elbowx>=0)&&(handx<0))
	{
	  mx=(ofacex+ohandx)/2;my=(ofacey+ohandy)/2;  move(mx,my);
	}
      else if((facex<0)&&(elbowx<0)&&(handx>=0))
	{
	  mx=(ofacex+oelbowx)/2;my=(ofacey+oelbowy)/2;  move(mx,my);
	}
      else if ((facex<0)&&(elbowx<0)&&(handx<0))
	{return 0;
	}
      else if (!((facex>=0)&&(elbowx>=0)&&(handx>=0)) && !((ofacex>=0)&&(oelbowx>=0)&&(ohandx>=0)))
	{return 0;
	}
    
     

      ofacex=facex;ofacey=facey;oelbowx=elbowx;oelbowy=elbowy;ohandx=handx;ohandy=handy;
      sleep(1);
	printf("\a");
    }
  return 0;
}

  int getPoints(int *facex, int *facey, int *elbowx, int *elbowy, int *handx, int *handy) {
    char s[256];

    if (fgets(s,256,fp) != NULL) {
      if (s[0] == '#')
	{fgets(s,256,fp);}

    
      sscanf(s,"%d %d %d %d %d %d\n", facex, facey, elbowx, elbowy, handx, handy); 

      return 0;
    }

    return -1;
  }

  int move(int x, int y) {
    printf("move to %d, %d\n", x, y);

    return 0;
  }
