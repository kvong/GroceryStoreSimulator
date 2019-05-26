#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
  
  FILE * inFile;
  srand(time(NULL));
  printf("Enter number of hours store will be opened: \n");
  float hour;
  scanf("%f", &hour);
  double hourInSec = (hour*3600)/5;
  inFile = fopen( "customers", "w");
  if ( inFile == NULL)
  {
    printf("File does not exist");
    return 0;
  }
  float iniTime = 0;
  int i = 0;
  for ( i = 0; i < hourInSec; i++){
    iniTime = iniTime + 5;
    float timeR = rand()%400 + 100;
    fprintf( inFile, "%f\t%f\n", iniTime, timeR);
  }

  fclose(inFile);
}
