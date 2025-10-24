#include <stdio.h>

int main(){
    char s[] = "hello";
    char*p = s;
    printf("size of char array :%zu\n ", sizeof(s));
    printf("size of char pinter : %zu\n", sizeof(p));

    return 0;
}