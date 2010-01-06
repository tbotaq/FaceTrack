
int main()
{

  init();

  while( 1 )
    {
      calcMatchResult();

      checkErrorByDiff();
      checkErrorBySimi();

      if(errorIsDetected)
	init();
      else 
	move();

    }

  memRelease();

  return 0;
}
