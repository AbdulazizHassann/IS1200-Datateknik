/*
 print-primes.c
 By David Broman.
 Last modified: 2015-09-15
 This file is in the public domain.
*/


#include <stdio.h>
#include <stdlib.h>

#define COLUMNS 6
int counter = 0;


void print_Number(int n ){
  counter++;
    printf("%10d ", n);
    if(counter == COLUMNS){
      printf("\n");
      counter = 0; 
    }

}
void print_sieves(int max)
{
    // populate array
    unsigned char sieve[max];
    // we asume that every number that bigger then 2 is 'prime number ' so we set to 1.
    for (int i = 2; i < max; i++)
    {
        sieve[i] = 1;
    }

    // find multiples and sets to zero
      /* 
        We have two for-loops. 
        the outer loop goes through each number i. 
        the inner loop sets sieve[j*i] = 0 for all multiples of i. 
        exp: when i = 2 → 2*2 = 4 → sieve[4] = 0; then 2*3 = 6 → sieve[6] = 0; and so on. 
        the inner loop stops when j*i >= max. 
        exp: if max = 10, then we get sieve[4] = 0, sieve[6] = 0, sieve[8] = 0. 
        next would be 2*5 = 10, but that equals max, so the loop ends. 
        then i is incremented to 3, and the process repeats until i < max.  
       */
    for (int i = 2; i < max; i++)
    {
        for (int j = 2; j * i < max; j++)
        {
            sieve[j * i] = 0;
        }
    }

    // print primes
    int preprime = 2; // starting from 2 as preprime
    int distance = 0; 
    for (int i = 2; i < max; i++)
    {
     
       if (sieve[i]) // if sieve[i] == true then it is prime number and print out with print_Number() function.
                      // in C  1 == true and 0 == false.so it checks what sieve[i] if it's 1 then it's prime number. 
        {
          if(i - preprime == 4){ // checking if prime now - preprime == 4 distance == 4
                                // then we incrument variable distance by one.
             distance++; 
             //here we print out the numbers that have distance 4.
             printf("\n(1).number one :%d and number two :%d\n" , preprime , i);
          }
          // we changing premprime to i so we can have the preprime after i changes.
          preprime = i;
            print_Number(i); 
        } 
    } // here we print out amout times we found distance 4.
      printf("\nthe pairs we got is :%d\n", distance);
}



// 'argc' contains the number of program arguments, and
// 'argv' is an array of char pointers, where each
// char pointer points to a null-terminated string.
int main(int argc, char *argv[]){
  
    if(argc == 2)
    {
         print_sieves(atoi(argv[1]));
    }
  else
    printf("Please state an integer number.\n");
  return 0;
}

 
