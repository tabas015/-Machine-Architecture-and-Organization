#include "matvec.h"


int matsquare_BASE_NORMAL(matrix_t mat, matrix_t matsq) {
  for(int i=0; i<mat.rows; i++){
    for(int j=0; j<mat.cols; j++){
      MSET(matsq,i,j,0);                          
      for(int k=0; k<mat.rows; k++){
        int mik = MGET(mat, i, k);  
        int mkj = MGET(mat, k, j);  
        int cur = MGET(matsq, i, j); 
        int new = cur + mik*mkj;
        MSET(matsq, i, j, new);   
      }
    }
  }

  return 0;                                      
}

int matsquare_BASE(matrix_t mat, matrix_t matsq){
  if(mat.rows != mat.cols ||                       
     mat.rows != matsq.rows || mat.cols != matsq.cols)
  {
    printf("matsquare_BASE: dimension mismatch\n");
    return 1;
  }

  return matsquare_BASE_NORMAL(mat, matsq);
}
