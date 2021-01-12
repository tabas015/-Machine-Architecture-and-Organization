#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "matvec.h"

double total_points = 0;
double actual_score = 0;

#include "data.c"

void check_hostname();

#define NSIZES 7
int nsizes = NSIZES;
int REPEATS = 1;

int main(int argc, char *argv[]){
  check_hostname();

  printf("==== Matrix Square Benchmark Version 1 ====\n");

  int sizes[NSIZES] = {
     273,
     512,
     722,
     801,
    1024,
    1101,
    1309,
  };

  if(argc > 1 && strcmp(argv[1],"-test")==0){
    nsizes = 2;                 // for valgrind testing
  }

  printf("%6s ","SIZE");
  printf("%10s ","BASE");
  printf("%10s ","OPTM");
  printf("%6s ", "SPDUP");
  printf("%6s ", "LOG2");
  printf("%6s ", "FACTOR");
  printf("%6s ", "POINTS");
  printf("\n");


  // Iterate over different sizes of the matrix
  for(int i=0; i<nsizes; i++){
    long size = sizes[i];
    long rows=size, cols=size;

    matrix_t base_mat, base_matsq;
    matrix_t optm_mat, optm_matsq;

    if( matrix_init(&base_mat,rows,cols)   ||
        matrix_init(&base_matsq,rows,cols) ||
        matrix_init(&optm_mat,rows,cols)   ||
        matrix_init(&optm_matsq,rows,cols))
    {
      printf("\nERROR: failure to initialize at size %ld\n",size);
      printf("ABORTING\n");
      exit(EXIT_FAILURE);
    }

    matrix_fill_sequential(base_mat);
    matrix_fill_sequential(base_matsq);
    matrix_fill_sequential(optm_mat);
    matrix_fill_sequential(optm_matsq);

    clock_t begin, end;
    int ret;

    begin = clock();
    for(int i=0; i<REPEATS; i++){
      ret = matsquare_BASE(base_mat,base_matsq);
      if(ret){
        printf("\nERROR: failure on matsquare_BASE() at size %ld\n",size);
        printf("ABORTING\n");
        exit(EXIT_FAILURE);
      }        
    }
    end = clock();
    double cpu_time_BASE = ((double) (end - begin)) / CLOCKS_PER_SEC;

    begin = clock();
    for(int i=0; i<REPEATS; i++){
      ret = matsquare_OPTM(optm_mat,optm_matsq);
      if(ret){
        printf("\nERROR: failure on matsquare_OPTM() at size %ld\n",size);
        printf("ABORTING\n");
        exit(EXIT_FAILURE);
      }        
    }
    end = clock();
    double cpu_time_OPTM = ((double) (end - begin)) / CLOCKS_PER_SEC;

    double speedup_OPTM = (cpu_time_BASE / cpu_time_OPTM);

    double factor =                               // scale based on size: speedup on 
      (( ((double) size) / ((double) sizes[0]))); // larger matrix worth more points

    double log2spdup = log2(speedup_OPTM);        // log_2 so equal speed is 0 points, 2X is 1pt, etc
    if(log2spdup < 0){                            // no negative scores
      log2spdup = 0.0;
    }
    double points = log2spdup * factor;           // scale speedup by size factor

    int mismatch = 0;
    for(int i=0; i<base_matsq.rows; i++){
      for(int j=0; j<base_matsq.cols; j++){
        int base_ij = MGET(base_matsq,i,j);
        int optm_ij = MGET(optm_matsq,i,j);
        if(base_ij != optm_ij){
          printf("ERROR: base and optm results differ\n");
          printf("ERROR: base_matsq[%d][%d] = %d\n",i,j,base_ij);
          printf("ERROR: optm_matsq[%d][%d] = %d\n",i,j,optm_ij);
          printf("ERROR: Skipping checks on remaining elements\n");
          printf("ERROR: Try running the 'matsquare_print <size>' program to see differences\n");
          speedup_OPTM = -1.0;
          points = 0;
          mismatch = 1;
          break;
        }
      }
      if(mismatch==1){
        break;
      }
    }

    total_points += points;

    printf("%6ld ", size);
    printf("%10.4e ",cpu_time_BASE);
    printf("%10.4e ",cpu_time_OPTM);
    printf("%6.2f ", speedup_OPTM);
    printf("%6.2f ", log2spdup);
    printf("%6.2f ", factor);
    printf("%6.2f ", points);
    printf("\n");

    
    matrix_free_data(&base_mat);       // clean up data before starting next loop iteration
    matrix_free_data(&base_matsq);
    matrix_free_data(&optm_mat);
    matrix_free_data(&optm_matsq);
  }

  double max_score = 35.0;
  actual_score = total_points;
  printf("RAW POINTS: %.2f\n",actual_score);

  if(actual_score > max_score){
    actual_score = max_score;
    final_check();
  }

  printf("TOTAL POINTS: %.0f / %.0f\n",actual_score,max_score);

  check_hostname();

  return 0;
}
  
#define MAXHOSTNAMELEN 1024
// #define EXPECT_HOST "csel-atlas"
#define EXPECT_HOST "csel-kh1250"

void check_hostname(){
  char actual_host[MAXHOSTNAMELEN];
  char *expect_host = EXPECT_HOST;
  if (gethostname(actual_host, MAXHOSTNAMELEN) != 0) {
    printf("WARNING: Couldn't get machine hostname\n");
  }
  else if(strncasecmp(expect_host, actual_host, strlen(expect_host)) != 0){
    printf("WARNING: expected host '%s-NN' but got host '%s'\n",expect_host,actual_host);
    printf("WARNING: timing results / scoring will not reflect actual scoring\n");
    printf("WARNING: run on host '%s-NN' for accurate results\n",expect_host);
  }    
}
