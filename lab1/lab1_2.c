#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	int c = 0;
	int a = 0;

	fflush(stdout);

	for(int i=1;i<argc;i++){
		sscanf(argv[i], "%d", &a);
		c += a;
	}

	printf("Result is %d\n", c);

	FILE * fp;
	
	fp=fopen("fileout.txt", "w");
	fprintf(fp, "Result is %d\n",  c);
	fclose(fp);
	

	return 0;
}
