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

  int facex, facey, handx, handy,elbowx,elbowy;
  int ofacex,ofacey,ohandx,ohandy;
  int mx,my,ox,oy;

  while (getPoints(&facex, &facey, &handx, &handy) == 0)
    {
  
      if((facex!=-1)&&(handx!=-1)) 
	{ 
	  mx=(facex+handx)/2;my=(facey+handy)/2;
	}
      else if((facex!=-1)&&(handx==-1))
	{
	  mx=ohandx;my=ohandy;  move(mx,my);
	}
      else if((facex==-1)&&(handx!=-1))
	{
	  mx=ofacex;my=ofacey;  move(mx,my);
	}
      

      else if ((facex==-1)&&(handx==-1))
	{return 0;
	}
      else if (!((facex!=-1)&&(handx!=-1)) && !((ofacex!=-1)&&(ohandx!=-1)))
	{return 0;
	}
    
     

      ofacex=facex;ofacey=facey;ohandx=handx;ohandy=handy;
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
