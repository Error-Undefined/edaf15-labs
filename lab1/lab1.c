#include <stdio.h>

int main(int argc, char* argv[])
{
	int	a;
	int	b;
	int	c;
	int	d;

	printf("Please enter two numbers: ");
	
	fflush(stdout);

	d = scanf("%d %d", &a, &b);

	c = a + b;

	printf("%d + %d = %d\n", a, b, c);

	printf("Scanf return is %d\n", d);
	
	for(int i = 0;i < argc;i++){
		printf("Argument number %d is %s\n", i, argv[i]); 
	}
	
	return 0;
}
