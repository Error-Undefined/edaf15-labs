#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define ever (;;)

/* A struct that holds a rational number and a reference to another for a freelist */
typedef struct rational_t rational_t;

/* A struct that holds a rational number and a reference to another for a freelist */
struct rational_t
{
	long long p;
	long long q;
	rational_t *next;
};

/* A simple arena */
typedef struct arena_t arena_t;

/* A simple arena */
struct arena_t
{
	char *current;
	char *buffer;
	size_t size;
};

rational_t *head_of_free; /* Holds the head the freelist */

/* Allocates size bytes of memory from the arena_t arena */
static inline void *arena_alloc(arena_t *arena, size_t size)
{
	void *data;

	data = arena->current;
	arena->current += size;

	return data;
}

/**
 * Add a node to the head of the freelist.
 */
static inline void add_to_free(rational_t *r)
{
	r->next = head_of_free->next;
	head_of_free->next = r;
}

/**
 * Gets a node from the freelist, or NULL if it's empty.
 */
static inline rational_t *get_from_free()
{
	rational_t *r = head_of_free->next;
	if (r == NULL)
		return r;

	head_of_free->next = r->next;
	r->next = NULL;
	return r;
}

/**
 * Creates a new rational_t in the arena arena_ptr
 */
static inline rational_t *new_rational(long p, long q, arena_t *arena_ptr)
{
	rational_t *r = get_from_free();
	if (r == NULL)
		r = arena_alloc(arena_ptr, sizeof(rational_t));

	r->p = p;
	r->q = q;

	return r;
}

/* Recursive function for the gcd of p and q */
long long gcd(long long p, long long q)
{
	if (q == 0)
		return p;

	return gcd(q, p % q);
}

/**
 * Reduces the rational_t r to its lowest factors.
 */
static inline void reduce(rational_t *r)
{
	int sign;
	if (r->p == 0)
	{
		return;
	}
	if ((r->p > 0 && r->q > 0) || (r->p < 0 && r->q < 0))
	{
		sign = 1;
	}
	else
	{
		sign = -1;
	}

	r->p = (r->p > 0) ? r->p : -(r->p);
	r->q = (r->q > 0) ? r->q : -(r->q);

	long long p = r->p;
	long long q = r->q;
	long long t;
	while (q != 0)
	{
		t = q;
		q = p % q;
		p = t;
	}


	r->p = r->p / p;
	r->q = r->q / p;

	r->p = sign * (r->p);
	r->q = (r->q);
}

/**
 * Subtracts the rationals r1 and r2, stores the result in r1.
 */
static inline void subq(rational_t *r1, rational_t *r2)
{
	r1->p = (r1->p) * (r2->q) - (r2->p) * (r1->q);
	r1->q *= r2->q;
}

/**
 * Subtracts the rational_t r2 from r1, returns a rational_t pointer to the result.
 */
static inline rational_t *subq_n(rational_t *r1, rational_t *r2, arena_t *arena)
{
	rational_t *r = new_rational((r1->p) * (r2->q) - (r2->p) * (r1->q), (r1->q) * (r2->q), arena);
	return r;
}

/**
 * Divides the rationals r1 and r2, stores the result in r1. May also reduce the rational r1.
 */
static inline void divq(rational_t *r1, rational_t *r2)
{
	int reduce_lim = 550000;
	r1->p *= r2->q;
	r1->q *= r2->p;

	if (r1->p > reduce_lim || r1->q > reduce_lim || r1->p < -reduce_lim || r1->q < -reduce_lim)
		reduce(r1);
}

/**
 * Gives the sign of the rational_t r, or 0 if it is 0.
 */
static inline int sign(rational_t *r)
{

	if ((r->p > 0 && r->q > 0) || (r->p < 0 && r->q < 0))
	{
		return 1;
	}
	if (r->p == 0)
	{
		return 0;
	}
	return -1;
}

/**
 * Returns true if p1 is strictly greater to r2.
 */
static inline bool compare_r(rational_t *r1, rational_t *r2)
{
	rational_t r;
	r.p = r1->p;
	r.q = r1->q;
	subq(&r, r2);
	if (sign(&r) == 1)
	{
		return true;
	}
	return false;
}

/**
 * Makes the solution, on the premise that b1 > B1 gives false
 */
static inline bool make_solution(rational_t *b1, rational_t *B1)
{
	if (compare_r(b1, B1))
	{
		return false;
	}

	return true;
}

/* Sorts the inequalities: positive, then negative, then 0 */
static inline void sort(int n1, int n2, size_t r, size_t s, rational_t ***T, rational_t **q)
{
	int posn = 0;
	int negn = n1;

	rational_t **tempT;
	rational_t *tempq;

	if (n1 != 0)
	{
		for (size_t i = 0; i < r; i++)
		{
			if (sign(T[i][s - 1]) == 1)
			{
				tempT = T[i];
				T[i] = T[posn];
				T[posn] = tempT;

				tempq = q[i];
				q[i] = q[posn];
				q[posn] = tempq;

				posn += 1;
			}
		}
	}

	if (n1 != n2)
	{
		for (size_t i = 0; i < r; i++)
		{
			if (sign(T[i][s - 1]) == -1)
			{
				tempT = T[i];
				T[i] = T[negn];
				T[negn] = tempT;

				tempq = q[i];
				q[i] = q[negn];
				q[negn] = tempq;

				negn += 1;
			}
		}
	}
}

/**
 * Gives true if the system of inequalities Ax <= c has a solution, otherwise false.
 */
bool fm(size_t rows, size_t cols, signed char a[rows][cols], signed char c[rows])
{
	/* One total elimination makes at most 4*(rows*rows)^cols iterations */
	size_t arenasize = (size_t) 8 * pow(rows * rows, cols) + 200;

	char mem[arenasize]; /* Use a VLA for the arena */

	arena_t arena_struct;

	arena_t *arena_ptr = &arena_struct;

	arena_ptr->buffer = &(mem[0]);
	arena_ptr->current = arena_ptr->buffer;
	arena_ptr->size = arenasize;

	rational_t headrational;
	head_of_free = &headrational;

	head_of_free->p = 0;
	head_of_free->q = 0;
	head_of_free->next = NULL;

	rational_t ***T;
	rational_t **q;

	size_t r = rows;
	size_t s = cols;

	int n1 = 0;
	int n2 = 0;

	T = arena_alloc(arena_ptr, r * sizeof(rational_t **));
	for (size_t i = 0; i < r; i++)
	{
		T[i] = arena_alloc(arena_ptr, s * sizeof(rational_t *));
	}
	q = arena_alloc(arena_ptr, r * sizeof(rational_t *));

	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			T[i][j] = new_rational(a[i][j], 1, arena_ptr);
		}
		q[i] = new_rational(c[i], 1, arena_ptr);
	}

	for
		ever
		{
			n1 = 0;
			n2 = 0;

			/* Set n1 and n2 */
			for (size_t i = 0; i < r; i++)
			{
				if (sign(T[i][s - 1]) == 1)
				{
					n1++;
				}
				else if (sign(T[i][s - 1]) == -1)
				{
					n2++;
				}
			}

			n2 += n1;

			/* Sort the inequalities */
			sort(n1, n2, r, s, T, q);

			/* Divide the inequalities */
			for (int i = 0; i < n2; i++)
			{
				divq(q[i], T[i][s - 1]);
				for (size_t j = 0; j < s - 1; j++)
				{
					divq(T[i][j], T[i][s - 1]);
				}
				T[i][s - 1]->p = 1;
				T[i][s - 1]->q = 1;
			}

			/* The problem has been reduced to only one variable */
			if (s == 1)
			{
				rational_t b1;
				rational_t B1;

				/* There is no upper bound or lower bound */
				size_t tempn2 = n2;
				if ((n1 <= 0 || n2 <= n1) && r == tempn2)
				{
					return true;
				}

				/* Maximize b1 and minimize B1 */

				b1.p = q[0]->p;
				b1.q = q[0]->q;

				B1.p = q[n1]->p;
				B1.q = q[n1]->q;

				for (int i = 1; i < n1; i++)
				{
					if (compare_r(&b1, q[i]))
					{
						b1.p = q[i]->p;
						b1.q = q[i]->q;
					}
				}

				for (int i = n1 + 1; i < n2; i++)
				{
					if (compare_r(q[i], &B1))
					{
						B1.p = q[i]->p;
						B1.q = q[i]->q;
					}
				}

				bool result = make_solution(&B1, &b1);
				return result;
			}

			/* Create rprime and new inequalities */
			int rprime = r - n2 + n1 * (n2 - n1);
			if (rprime == 0)
			{
				return true;
			}

			rational_t ***oldT = T;
			rational_t **oldq = q;

			T = arena_alloc(arena_ptr, rprime * sizeof(rational_t **));
			for (size_t i = 0; i < (size_t)rprime; i++)
			{
				T[i] = arena_alloc(arena_ptr, (s - 1) * sizeof(rational_t *));
			}
			q = arena_alloc(arena_ptr, rprime * sizeof(rational_t *));

			int m = 0;

			/* Mathemagics */
			for (int k = 0; k < n1; k++)
			{
				for (int i = n1; i < n2; i++)
				{
					for (size_t j = 0; j < s - 1; j++)
					{
						T[m][j] = subq_n(oldT[k][j], oldT[i][j], arena_ptr);
					}
					q[m] = subq_n(oldq[k], oldq[i], arena_ptr);
					m++;
				}
			}

			/* Bring back the zero coeff */
			for (size_t i = n2; i < r; i++)
			{
				for (size_t j = 0; j < s - 1; j++)
				{
					T[m][j] = oldT[i][j];
				}
				q[m] = oldq[i];
				m++;
			}

			/* Free the old ineqs */
			//free_to_freelist(oldT, oldq, r, s);
			for (int i = 0; i < n2; i++)
			{
				for (size_t j = 0; j < s; j++)
				{
					add_to_free(oldT[i][j]);
				}
				add_to_free(oldq[i]);
			}

			s = s - 1;
			r = rprime;
		}
}
