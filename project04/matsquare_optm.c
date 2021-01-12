// optimized versions of matrix diagonal summing
#include "matvec.h"

int matsquare_VER1(matrix_t mat, matrix_t matsq) {
  int start = mat.rows;          // made a variable for the rows in matrix so that the loop doesn't have to iterate through mat.row everytime as it will take longer time
  int stop= mat.cols;           // made a variable for the rows in matrix so that the loop doesn't have to iterate through mat.col everytime as it will take longer time
  for(int i=0; i<start; i++){            // going over the row
    for(int j=0; j<stop; j++){           // going over the column
      MSET(matsq,i,j,0);                 // initialize to 0s
    }
  }

  for(int i=0; i<start; i++){           //iterating though the row
    for(int j=0; j<stop; j++){         //iterating throught the column
      int lead = MGET(mat, i, j);      //setting up the lead. iterating accross row and multiplying lead elemnt in each row sum
      int k=0;                       //initializing k to 0                      
      for(k =0; k<stop-2; k+=3){       //stop 2 positions early of the matrix. loop unrollling for better timing
      {                                //FIRST body of the for loop for better optimization.[LOOP UNROLLING]
        int cur = MGET(matsq, i, k) ;  //used define MGET rather than calling any function for better optimization 
        int mkj = MGET(mat, j, k);     //used define MGET so that it gets relaced by ((mat).data[((j)*((mat).cols)) + (k)]) // for optimization
        cur += mkj*lead;                 
        MSET(matsq, i, k, cur);        //used define MSET rather than calling any function for better optimization 
  
      }
      {                                  // SECOND body of the for loop. Did loop unrolling in two parts for optimization
        int cur = MGET(matsq, i, k+1);   //NO FUNCTION CALL. USED DEFINE TO COPY AND PASTE WHAT'S IN IT(MACROS)
        int mkj = MGET(mat, j, k+1);   //NO FUNCTION CALL. USED DEFINE TO COPY AND PASTE WHAT'S IN IT(MACROS)
        cur += mkj*lead;          
        MSET(matsq, i, k+1, cur);   //NO FUNCTION CALL. USED DEFINE TO COPY AND PASTE WHAT'S IN IT(MACROS)
  
      }
      {                                  // THIRD body of the for loop. Did loop unrolling in two parts for optimization
        int cur = MGET(matsq, i, k+2);    //NO FUNCTION CALL. USED DEFINE TO COPY AND PASTE WHAT'S IN IT(MACROS)
        int mkj = MGET(mat, j, k+2);      //NO FUNCTION CALL. USED DEFINE TO COPY AND PASTE WHAT'S IN IT(MACROS)
        cur += mkj*lead;          
        MSET(matsq, i, k+2, cur);         //NO FUNCTION CALL. USED DEFINE TO COPY AND PASTE WHAT'S IN IT(MACROS)
  
      }
      }                            // CLEANUP LOOP (got done with the mat.cols-2 part of the matrix )
      for (; k<stop; k++){           //complete the remaining part of the loop where it finished.that's why didn't initialize k to anything
        int cur = MGET(matsq, i, k) ;  //NO FUNCTION CALL. USED DEFINE TO COPY AND PASTE WHAT'S IN IT(MACROS)
        int mkj = MGET(mat, j, k);     //NO FUNCTION CALL. USED DEFINE TO COPY AND PASTE WHAT'S IN IT(MACROS)
        cur += mkj*lead;          
        MSET(matsq, i, k, cur);   //NO FUNCTION CALL. USED DEFINE TO COPY AND PASTE WHAT'S IN IT(MACROS)

      }
    }
  }
  return 0;
}

int matsquare_OPTM(matrix_t mat, matrix_t matsq) {
  if(mat.rows != mat.cols ||                       // must be a square matrix to square it
     mat.rows != matsq.rows || mat.cols != matsq.cols)
  {
    printf("matsquare_OPTM: dimension mismatch\n");
    return 1;
  }

  return matsquare_VER1(mat, matsq);
}
