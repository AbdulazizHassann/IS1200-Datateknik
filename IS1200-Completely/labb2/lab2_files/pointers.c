/*
 pointers.c
 By David Broman.
 Last modified: 2015-09-15
 This file is in the public domain.
*/


#include <stdio.h>
#include <stdlib.h>

char* text1 = "This is a string.";
char* text2 = "Yet another thing.";

// own code
int list1 [20]; // 80 bytes space int => 80/4 = 20 elements
int list2 [20];
int counter = 0;

void printlist(const int* lst){
  printf("ASCII codes and corresponding characters.\n");
  while(*lst != 0){
    printf("0x%03X '%c' ", *lst, (char)*lst);
    lst++;
  }
  printf("\n");
}

void endian_proof(const char* c){
  printf("\nEndian experiment: 0x%02x,0x%02x,0x%02x,0x%02x\n", 
         (int)*c,(int)*(c+1), (int)*(c+2), (int)*(c+3));
  
}
void copycodes(char* txt ,  int* lst, int* counter){
  // this loop copies every char in txt number to list lst . 
  while(*txt!='\0'){ 
    *lst = (int)*txt;  //convert the char value from txt into an int.So the compiler treats it as an integer before storing it in lst
    lst++; // increase the pointer so it point next index of the list.
    txt++; // here we also increse txt so we can copie next chart in to the list olso so we can loop all chars in text 
    *counter = *counter + 1; // we count everytime this while loop is true and we change the global variabeln counter.
   
  }
}
void work(void){
  
    copycodes(text1,list1,&counter); // first time for text1 and list1 and first time we change counter value.
    copycodes(text2,list2,&counter); // secund time for text2 and list2 and counter will be first value + those value after.
}
int main(void){
 
    work();
    printf("\nlist1: ");
    printlist(list1);
    printf("\nlist2: ");
    printlist(list2);
    printf("\nCount = %d\n", counter);

    endian_proof((char*) &counter);
}
