#include<panTiltUnit.h>

using namespace std;

int main(void)
{
  panTiltUnit *ptu = new panTiltUnit();
  int pan=0,tilt=0;
  bool go = true;
  char key='n';
  while(go)
    {
      cout<<"Input pan and tilt."<<endl;
      cin>>pan;
      cin>>tilt;
      ptu -> move(pan,tilt);
      cout<<"quit? (y or n)"<<endl;
      cin >> key;
      if(key == 'y')
	go = false;  
    }

  delete ptu;

  cout<<"Amp is turned off"<<endl;

  return 0;
}
