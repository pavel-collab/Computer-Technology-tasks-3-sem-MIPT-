#include <stdio.h>
#include <unistd.h>

void a_out() {
    execlp(
        // path to executable
        "/home/pavel/workfile/ComputerTechnology/11/out",
        //argv
        "out", NULL        
    );
    printf("test -1");
}

int main() {
    
    

    return 0;
}