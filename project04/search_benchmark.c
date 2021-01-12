#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "search.h"


void print_usage() {          //helper function which will be used when the user will not mention any argument
  printf("usage: ./search_benchmark <minpow> <maxpow> <repeats> [which]\n");
  printf(" which is a combination of:\n"); 
  printf(" a : Linear Array Search\n"); 
  printf(" l : Linked List Search\n"); 
  printf(" b : Binary Array Search\n"); 
  printf(" t : Binary Tree Search\n"); 
  printf(" (default all)\n"); 
}    //helper function

int main(int argc, char *argv[]){     //main function
  if (argc < 4) { 
    print_usage();                //herlper function
    return 1; 
  }

  int min = atoi(argv[1]);            //taking arg 1 as a min val
  int max = atoi(argv[2]);           //taking arg 2 as a max val
  int REPEATS = atoi(argv[3]);        // taking arg 3 as Repeats
  
  int do_linear_array = 0;      // initializing the variable which will run for array to 0
  int do_binary_array = 0;      // initializing the variable which will run for binary array to 0
  int do_linear_list = 0;      // initializing the variable which will run for list to 0
  int do_binary_tree = 0;     // initializing the variable which will run for binary tree to 0

  if (argc >= 4) {            //checking the number of arguments if it's grater than 4 or equal to it
    if (argc == 4){          // if equal to 4 then perform the all algorithms for search
      do_linear_array = 1; 
      do_binary_array = 1;
      do_linear_list = 1;
      do_binary_tree = 1;
    }
    else if (argc >4){       // if greater than 4 then check whether it's indicating 'a' or 'b' or 'l' or 't'
      char *algs_string = argv[4];  
      int length = strlen(algs_string);
      for(int i=0; i < length ; i++){   
        if(algs_string[i] == 'a'){    //if 'a' then do array search
          do_linear_array = 1;   
        }
        if (algs_string[i] == 'b'){ //if 'b' then do binary array search
          do_binary_array = 1;
        }
        if  (algs_string[i] == 'l'){ //if  'l' then do list search
          do_linear_list = 1;
        }
        if (algs_string[i] == 't'){  //if 't' then do binary tree search
          do_binary_tree = 1;
        }
      }
    }
  }
    
  
  printf("\n");
  printf("%8s ","LENGTH    ");
  printf("%8s ","SEARCHES   ");
  if(do_linear_array){               // for array search print array
    printf("%10s ","array");
  }
  if(do_linear_list){               //for list search print ist on the header
    printf("%10s ", "list");
  }
  if (do_binary_array){
    printf("%10s "," binary ");        // for binary array print array on the header
  }
  if (do_binary_tree){             //for binary tree print tree on the top
    printf("%10s ","tree");
  }
  printf("\n");
    

  clock_t begin, end;             //clock begin and end variable
  double cpu_time_BASE;           
  int ret;
  for(int i= min; i <= max; i++){       //checking min and max
    int len = 1<<i;                     // shifting left to get the 2 to the power of the input for both min and max
    int searches = 2*len;               // searches is 2 into length
    printf("%8d ", len); 
    printf("%8d ",searches*REPEATS );
    if(do_linear_array){                //linear array
      int *a = make_evens_array(len);   
      begin = clock();                //beginning the clock to calculate the time
      for(int i=0; i<REPEATS; i++){
        for(int i=0; i<((2*len)-1); i++){
          ret += linear_array_search( a , len, i); 
        } 
      }  
    end = clock();                    //ending the clock 
    cpu_time_BASE = ((double) (end - begin)) / CLOCKS_PER_SEC;
    printf("    %10.4e ", cpu_time_BASE );
    free (a);
   }
  
    if(do_linear_list){           //linear list
      list_t *b = make_evens_list(len);
      begin = clock();
      for(int i=0; i<REPEATS; i++){
        for(int i=0; i<((2*len)-1); i++){
          ret += linkedlist_search(b,len, i);
        }
      }   
    end = clock();
    cpu_time_BASE = ((double) (end - begin)) / CLOCKS_PER_SEC;
    printf("    %10.4e ", cpu_time_BASE );
    list_free(b);  
    }
    
    if (do_binary_array){           //binary array
      int *c = make_evens_array(len); 
      begin = clock();
      for(int i=0; i<REPEATS; i++){
        for(int i=0; i<((2*len)-1); i++){
          ret += binary_array_search(c,len, i);
        }
      }   
    end = clock();
    cpu_time_BASE = ((double) (end - begin)) / CLOCKS_PER_SEC;
    printf("  %10.4e ", cpu_time_BASE );
    free(c);
    }

    if (do_binary_tree){              //binary tree
      bst_t *d = make_evens_tree(len);
      begin = clock(); //binary tree 
      for(int i=0; i<REPEATS; i++){
        for(int i=0; i<((2*len)-1); i++){
          ret +=  binary_tree_search(d, len, i);
        }
      }   
      
    end = clock();
    cpu_time_BASE = ((double) (end - begin)) / CLOCKS_PER_SEC;
    printf("   %10.4e ", cpu_time_BASE );
    bst_free(d);
    }
  printf("\n");
  } //for loop
  return 0;
} //main 
