/*
 * $Id: factorial_env.c
 * $Author: rockins
 * $All copyrights reserved by rockins
 * $Detail:
 * 这个程序用来计算正整数n的阶数，需要给程序提供一个参数n
 * 提供参数的方式可以是命令行传入，也可以通过FAC_ARG环境变量传入
 * 利用如下的命令可编译生成可执行文件，并执行程序：
 * $ gcc -o factorial_env factorial_env.c
 * $ ./factorial_env 5
 * Factorial of 5 is 120.
 * $ FAC_ARG=5 ./factorial_env
 * Factorial of 5 is 120.
 * 如果要调试程序，那么应该像这样编译程序：
 * $ gcc -g -o factorial_env factorial_env.c
 */

int factorial (int n);

#include <stdio.h>
#include <stdlib.h>

int factorial (int n)
{
    if (n <= 1)
	return 1;
    else
	return (n * factorial(n - 1));
}


int main (int argc, char **argv)
{
    int n;
    char *env;


    if (argc < 2) { 
	if ((env = getenv("FAC_ARG")) != NULL) {
	    n = atoi(env);
	} else {
	    printf ("Usage: %s n\n", argv [0]);
	    return (-1);
	}
    } else {
	n = atoi (argv[1]);
    }
    
    printf ("Factorial of %d is %d.\n", n, factorial (n));

    return 0;
}
