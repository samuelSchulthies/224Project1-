#include <stdio.h>

void setValueTo200(int *a){
    //WRITE CODE TO SET THE VALUE OF a TO 200
    //THIS SHOULD CHANGE THE VARIABLE IN MAIN
    *a = 200;
}

void add24AndPrint(int a){
    //WRITE CODE TO ADD 24 TO a AND THEN PRINT OUT
    a += 24;
    printf("The value of a is: %d\n", a);
}

int main()
{
    //Initialize our value
    int a = 0;

    //Call first function by reference
    setValueTo200(&a);

    //Call second function by value
    add24AndPrint(a);

    //What is the value of a here?
    printf("The value of a in main is: %d", a);

    return 0;
}

