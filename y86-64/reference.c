#include <stdio.h>

void setValueTo200(int *a){
    *a = 200;
}
r
void add24AndPrint(int a){
    *a += 24;
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

