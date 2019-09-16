/* Convert VIC binary forcing file to ascii forcing file*/
/* Binary format:
         precip * 40, unsigned short int
         tmax * 100, short int
	 tmin * 100, short int
	 wind * 100, short int */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int main(int argc, char **argv)
{
  unsigned short int prec;
  short int array1, array2, array3;
  int x = 0;
  float prcp, tmax,tmin,wind;
  FILE *fp,*fpout;

  if(argc!=3){
    printf("Usage: %s infile outfile\n",argv[0]);
    printf("Infile is binary PTT file, outfile is ascii ptt\n");
    exit(0);
  }

  if((fp=fopen(argv[1],"r"))==NULL){
    printf("Can't open infile %s\n",argv[1]);
    exit(0);
  }
  if((fpout=fopen(argv[2],"w"))==NULL){
    printf("Can't open outfile %s\n",argv[2]);
    exit(0);
  }

  while((fread(&prec,sizeof(unsigned short int),1,fp) != 0)){
    if(fread(&array1,sizeof(short int),1,fp) != 1)
      printf("read array1 err rec %d %s\n", x,argv[1]);
    if(fread(&array2,sizeof(short int),1,fp) != 1)
      printf("read array2 err rec %d %s\n", x,argv[1]);
    x++;
    prcp = (float) (prec)/40.0;
    tmax = (float) (array1)/100.0;
    tmin = (float) (array2)/100.0;
    fprintf(fpout,"%f\t%f\t%f\n",prcp,tmax,tmin);
  }
  fclose(fp);
  fclose(fpout);
  return(0);
}
