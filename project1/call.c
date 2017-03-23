#include<linux/errno.h>
#include<sys/syscall.h>
#include<linux/unistd.h>
int i;
int main()
{
    //for (i = 1; i < 5; ++i)
        syscall(546,1);
    //return 0;
}
