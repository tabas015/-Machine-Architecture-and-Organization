#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "matvec.h"

int main(int argc, char *argv[]){
  if(argc < 2){
    printf("usage: %s <size>\n",argv[0]);
    exit(1);
  }

  printf("==== Matrix Square Print ====\n");
  long size = atoi(argv[1]);
  long rows=size, cols=size;
  matrix_t mat;
  matrix_t base_matsq, optm_matsq;

  matrix_init(&mat,rows,cols);
  matrix_fill_sequential(mat);

  matrix_init(&base_matsq,rows,cols);
  matrix_init(&optm_matsq,rows,cols);

  matsquare_BASE(mat,base_matsq);
  matsquare_OPTM(mat,optm_matsq);


  printf("Original Matrix:\n");
  matrix_write(stdout, mat);
  printf("\n");

  printf("BASE Matrix Squared :\n");
  matrix_write(stdout, base_matsq);
  printf("\n");

  printf("OPTM Matrix Squared :\n");
  matrix_write(stdout, optm_matsq);
  printf("\n");

  printf("BASE/OPTM Element Comparison:\n");

  printf("[%3s][%3s]: %6s %6s\n","i","j","BASE","OPTM");
  for(int i=0; i<rows; i++){
    for(int j=0; j<cols; j++){
      int base = MGET(base_matsq,i,j);
      int optm = MGET(optm_matsq,i,j);
      char *diff = (base != optm) ? "***" : "";
      printf("[%3d][%3d]: %6d %6d %s\n",
             i,j,base,optm,diff);
    }
  }
    
  matrix_free_data(&mat);       // clean up data
  matrix_free_data(&base_matsq);
  matrix_free_data(&optm_matsq);
  return 0;
}
