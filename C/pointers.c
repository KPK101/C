#include<stdio.h>

int main(){

    int x = 10;
    int *ptr1 = &x;
    int **ptr2 = &ptr1;
    int ***ptr3 = &ptr2;


    printf("x address = %p\n", ptr1);
    printf("x address = %p\n", &x);
    printf("x address = %p\n", *ptr2);
    printf("x address = %p\n", **ptr3);

    
    printf("x value = %d\n", *&x);
    printf("x value = %d\n", *ptr1);
    printf("x value = %d\n", **ptr2);
    printf("x value = %d\n", ***ptr3);

    return 0;
}