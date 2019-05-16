#include <stdio.h>

int main(int argc, char* argv[])
{
	int	a;
	int	b;
	int	c;
	int	d;

	fflush(stdout);

	sscanf(argv[1], "%d", &a);
	sscanf(argv[2], "%d", &b);

	c = a + b;

	printf("%d + %d = %d\n", a, b, c);

	return 0;
}
