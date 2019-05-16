 /* Warning: this program contains many errors and is used for a lab on Valgrind and the Address Sanitizer. */

#include <stdlib.h>
#include <stdio.h>

#define fail(a)		((test == 0 || test == a) ? fail##a() : 0)

#define N		(10)

int	a[N] = { 1 };
int*	b = &a[0];

void fail1()
{
	printf("a[0] = %d\n", a[0]);
	printf("b[0] = %d\n", b[0]);
	printf("*b   = %d\n", *b);
	*b = 2;
	a[N-1] = 3; //a[N] accesses a[10] which is out of bounds (0-9 allowed). After a[9] there is the b pointer. a[N]=3 is equivalent to "b=3"
	printf("*b = %d\n", *b);
}


void fail2()
{
	int*	a = calloc(N, sizeof(int));
	int*	b = &a[0];
	
	a[0] = 2;
	printf("a[0] = %d\n", a[0]);
	printf("b[0] = %d\n", b[0]);
	printf("*b   = %d\n", *b);
	*b = 2;
	a[N-1] = 3; //Same error. Now a is on the heap and b is on the stack so a[N] will not impact b.
	printf("*b = %d\n", *b);
	// Memory also isn't freed
	free(a);
}

void fail3() //Doesn't crash initially.
{
	int*		a;
	int*		b;
	long long*	c;
	int		i;

	a = calloc(N, sizeof(int));
	b = calloc(N, sizeof(int));
	c = calloc(N, sizeof(long long)); // Calloc on long long pointer with sizeof(int)

	for (i = 0; i < N; ++i) { // should be i++
		a[i] = i;
		b[i] = i;
		c[i] = i;
	}
	
	free(a);
	free(b);
	free(c);
}

void fail4()
{
	int*		a;
	int*		b;
	int*		c;

	a  = b = calloc(N, sizeof(int));
	c = &a[N/2];

	free(NULL);
	//free(&a); // &a gets the adress of the global array a delcared in the beginning of the file.
	free(a);
	a=NULL;
	//free(b); //when a has been freed we can't free b
	//free(c); //we can't free from the middle of an array because we don't have the heap metadata 
	free(a);
}

void fail5()
{
	int**		a;
	int*		b;
	int		i;

	a = calloc(N, sizeof(int*));

	for (i = 0; i < N; i++)
		a[i] = calloc(N, sizeof(int));

	
	for (i = 0; i < N; i++)
		free(a[i]);
	
	free(a); // These were in the wrong order, if we free a first we can't access the allocated a[i]
}

void fail6()
{
	int*		a;

	a = alloca(N * sizeof(int));

	a[N] = 1; // Overflow, not caught by valgrind?

	//free(a);
	
	//valgrind finds an error at free because we can't free memory from the stack. Out of bound errors?
}

void fail7()
{
	int		a; //a was never initialized. We malloc an undefined amount of memory
	a=1;
	int*		p;

	p = malloc(a);

	free(p);
}

//Google sanitizer seems a lot faster, but it does the work at compile time. Valgrind works at runtime.
//Compiling with the google sanitizer doesn't markably impact compiling time
//Although it doesn't seem to catch the errors in fail7().

int main(int argc, char **argv)
{
	int	test = 0;

	if (argc > 1) {
		sscanf(argv[1], "%d", &test);
		printf("doing test %d\n", test);
	} else
		puts("doing all tests");
		
	fail(1);
	fail(2);
	fail(3);
	fail(4);
	fail(5);
	fail(6);
	fail(7);

	puts("lab3 reached the end");

	exit(0);
}
