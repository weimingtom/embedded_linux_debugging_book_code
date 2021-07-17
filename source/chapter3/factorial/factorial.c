/*
 * $Id: factorial.c
 * $Author: rockins
 * $All copyrights reserved by rockins
 * $Detail:
 * ���������������������n�Ľ�������Ҫ�������ṩһ������n
 * �������µ�����ɱ������ɿ�ִ���ļ�����ִ�г���
 * $ gcc -o factorial factorial.c
 * $ ./factorial 5
 * Factorial of 5 is 120.
 * ���Ҫ���Գ�����ôӦ���������������
 * $ gcc -g -o factorial factorial.c
 */

int factorial (int n);

#include <stdio.h>
#include <stdlib.h>

int factorial (int n)
{
    if (n <= 1)
	return 1;
    else
	return n * factorial(n - 1);
}

int main (int argc, char **argv)
{
    int n;
    char *env;

    if (argc < 2) { 
	printf ("Usage: %s n\n", argv [0]);
	return -1;
    } else {
	n = atoi (argv[1]);
	printf ("Factorial of %d is %d.\n", n, factorial (n));
    }

    return 0;
}
