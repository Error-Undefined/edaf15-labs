#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef arena_mem
#define ARENA_SIZE (300000000)
#endif

typedef struct list_t	list_t;

struct list_t {
	list_t*		succ;
	list_t*		pred;
	void*		data;
};


#ifdef arena_mem



typedef struct arena_t arena_t;

struct arena_t {
	char* current;
	char* buffer;
	size_t size;
};	

arena_t *arena_create(size_t size)
{
	arena_t* arena=calloc(1, sizeof(arena_t));
	if(!arena)
		return NULL;
	
	arena->buffer=calloc(size, sizeof(char));
	if(!arena->buffer){
		free(arena);
		return NULL;
	}
	arena->current=arena->buffer;
	arena->size=size;
}


void* arena_alloc(arena_t* arena, size_t size)
{
	size_t remains;
	void* data;
	
	remains=arena->size - (arena->current - arena->buffer);
	if(size>remains){
		printf("Not enough memory in arena\n");
		exit(1);
	}
	data=arena->current;
	arena->current+=size;

	
	return data;
}

void arena_free(arena_t* arena)
{
	free(arena->buffer);
	free(arena);
}

#endif


#ifdef freelist

list_t* free_list;


void add_to_free(list_t* list)
{
	if(free_list==NULL){
		free_list=list;
		free_list->succ=NULL;
		free_list->pred=NULL;
	} else {
		free_list->succ=list;
		list->pred=free_list;
		list->succ=NULL;
		free_list=list;
	}	
}

list_t *remove_from_free()
{
	list_t* list=free_list;
	free_list=free_list->pred;
	list->pred=list->succ=NULL;
	if(free_list!=NULL){
		free_list->succ=NULL;
	}
	
	return list;
}

void free_free_list()
{	
	while(1)
	{
		list_t* list=free_list;
		free_list=free_list->pred;
		free(list);
	
		if(free_list==NULL)
			return;
	}
}
#endif

static double sec(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

int empty(list_t* list)
{
	return list == list->succ;
}

list_t *new_list(void* data)
{
	list_t*		list;

#ifdef freelist
	
	if(free_list!=NULL){
		list=remove_from_free();
	} else {
		list = malloc(sizeof(list_t));
	}
#else
	list = malloc(sizeof(list_t));
#endif
	assert(list != NULL);

	list->succ = list->pred = list;
	list->data = data;

	return list;
}

void add(list_t* list, void* data)
{
	list_t*		link;
	list_t*		temp;

	link		= new_list(data);

	list->pred->succ= link;
	link->succ	= list;
	temp		= list->pred;
	list->pred	= link;
	link->pred	= temp;
}

void take_out(list_t* list)
{
	list->pred->succ = list->succ;
	list->succ->pred = list->pred;
	list->succ = list->pred = list;
}

void* take_out_first(list_t* list)
{
	list_t*	succ;
	void*	data;

	if (list->succ->data == NULL)
		return NULL;

	data = list->succ->data;

	succ = list->succ;
	take_out(succ);

#ifdef freelist
	if(succ!=NULL){
		add_to_free(succ);
	} else {
		free(succ);
	}
#else
	free(succ);
#endif
	return data;
}

static size_t nextsize()
{
#if 1
	return rand() % 4096;
#else
	size_t		size;
	static int	i;
	static size_t	v[] = { 24, 520, 32, 32, 72, 8000, 16, 24, 212 };

	size = v[i];

	i = (i + 1) % (sizeof v/ sizeof v[0]);
	
	return size;
#endif
}

static void fail(char* s)
{
	fprintf(stderr, "check: %s\n", s);
	abort();
}

int main(int ac, char** av)
{
	int		n = 500000;		/* mallocs in main. */
	int		n0;
	list_t*		head;
	double		begin;
	double		end;
	double		t = 2.5e-9;
	
#ifdef freelist
	free_list=NULL;
#endif

	if (ac > 1)
		n = atoi(av[1]);

	n0 = n;

	head = new_list(NULL);

	printf("check starts\n");

	begin = sec();
	
	#ifdef arena_mem
	
		
	arena_t* arena=arena_create(ARENA_SIZE*sizeof(int));
	if(!arena){
		printf("Arena was not allocated.\n");
		exit(2);
	}
	
	while (n > 0) {
		add(head, arena_alloc(arena,nextsize()));
		n -= 1;
			
		
		if ((n & 1) && !empty(head)) {
			take_out_first(head);
		}
	}
	
	while (!empty(head))
		take_out_first(head);
	
	#ifdef freelist
	free_free_list();
	#endif
	
	arena_free(arena);
	
	#else
	while (n > 0) {
		add(head, malloc(nextsize()));
		n -= 1;
		
		if ((n & 1) && !empty(head)) 
			free(take_out_first(head));
	}

	while (!empty(head))
		free(take_out_first(head));

	#ifdef freelist
	free_free_list();
	#endif
	
	#endif

		
	free(head);

	end = sec();

	printf("check is ready\n");
	printf("total = %1.3lf s\n", end-begin);
	printf("m+f   = %1.3g s\n", (end-begin)/(2*n0));
	printf("cy    = %1.3lf s\n", ((end-begin)/(2*n0))/t);


	return 0;
}
