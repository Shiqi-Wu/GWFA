#pragma once

#include<stdio.h>
#include<stdlib.h>

#include<stdbool.h>
#include<string>
#include<time.h>
#include<math.h>
#include<climits>

using namespace std;

/*
Special Characters
*/
#define EOS '\0'
#define EOL '\n'
#define TAB '\t'
#define DOS_EOL '\r'
#define PLUS '+'
#define MINUS '-'
#define FORMAT '%'
#define SPACE ' '
#define SLASH '/'
#define STAR '*'
#define DOT '.'
#define COMA ','
#define SEMICOLON ';'
#define COLON '#'
#define UNDERSCORE '_'

/*
 * Common numerical data processing/formating
 */
#define MIN(a,b) (((a)<=(b))?(a):(b))
#define MAX(a,b) (((a)>=(b))?(a):(b))
#define ABS(a) (((a)>=0)?(a):-(a))

 /*
  * Pseudo-Random number generator
  */
#define rand_init() srand(time(0))
#define rand_i(min,max) ( min + ( rand()%(max-min+1) ) )
#define rand_f(min,max) ( min + ((double)rand()/(double)(RAND_MAX+1)) * (max-min+1) )

/*
minimum prime number
*/

int prime(int infnum)
{
	int i = infnum;
	while (true)
	{
		bool factor = 0;
		for (int k = 2; k <= sqrt(i); k++)
		{
			if (i % k == 0)
			{
				factor = 1;
				break;
			}
		}
		if (!factor)
			break;
		i++;
	}
	return i;
}

/* malloc */
#define MALLOC(n,type)\
((type*)malloc(n*sizeof(type)))

int tran(char a) 
{
	int c;
	if (a == 'A') {
		c = 0;
	}
	if (a == 'G') {
		c = 1;
	}
	if (a == 'C') {
		c = 2;
	}
	if (a == 'T')
	{
		c = 3;
	}
	return c;
}
