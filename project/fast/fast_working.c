#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

typedef struct rational rational;

struct rational {
	long p;
	long q;
};

/**
 * Creates a new rational on the heap.
 */
rational *new_rational(long p, long q)
{
	rational* r=calloc(1, sizeof(rational));
	r->p=p;
	r->q=q;
	return r;
}

/**
 *Prints the rational r. 
 */
void printrational(rational* r)
{
	printf("%d/%d\n",r->p,r->q);
}

/**
 * Reduces the rational r to its lowest factors.
 */
void reduce(rational* r)
{
	if(r->p == 0){
        r->q = 1;
        return;
    }

	int sign=-1;
	
	if((r->p > 0 && r->q > 0)||(r->p < 0 && r->q < 0)){
		sign = 1;
	}
	
	long p = (r->p > 0) ? r->p : -(r->p);
	long q = (r->q > 0) ? r->q : -(r->q);
	
	while(p != q){
		if( p > q ){
			p -= q;
		} else {
			q -= p;
		}
	}
	
	r->p = sign * (r->p);
	r->q = sign * (r->q);
	
	if(p==1){
		return;
	}
	
	r->p = (r->p)/p;
	r->q = (r->q)/p;
}

/**
 * Subtracts the rationals r1 and r2, stores the result in r1.
 */
void subq(rational* r1,rational* r2)
{
	reduce(r1);
	reduce(r2);
	r1->p =(r1->p)*(r2->q)-(r2->p)*(r1->q);
	r1->q *= r2->q;
	reduce(r1);
}

/**
 * Subtracts the rational r2 from r1, gives the result in a new rational pointer.
 */
rational *subq_n(rational* r1, rational* r2)
{
	rational* r = new_rational((r1->p)*(r2->q)-(r2->p)*(r1->q), (r1->q)*(r2->q));
	reduce(r);
	return r;
}

/**
 * Multiplies the rational r with the factor f
 */
void const_mulq(rational* r, int factor)
{
	r->p = r->p * factor;
	reduce(r);
}

/**
 * Divides the rationals r1 and r2, stores the result in r1.
 */
void divq(rational* r1,rational* r2)
{
	reduce(r1);
	reduce(r2);
	r1->p *= r2->q;
	r1->q *= r2->p;
	reduce(r1);
}

/**
 * Gives the sign of the rational r, or 0 if it is 0.
 */
int sign(rational* r)
{
	if(r->p == 0){
		return 0;
	}
	if((r->p > 0 && r->q > 0)||(r->p < 0 && r->q < 0)){
		return 1;
	}
	return -1;
}

/**
 * Returns true if p1 is strictly greater to r2
 */ 
bool compare_r(rational* r1, rational* r2)
{
	rational r;
	r.p = r1->p;
	r.q = r1->q;
	subq(&r, r2);
	if (sign(&r) == 1){
		return true;
	} 
	return false;
}

/**
 * Frees T and q.
 */
void free_all(rational*** T, rational** q, size_t r, size_t s)
{
    for(size_t i = 0; i < r; i++){
        for(size_t j = 0; j < s; j++){
            free(T[i][j]);
        }
        free(q[i]);
    }

    for(size_t i = 0; i < r; i++){
        free(T[i]);
    }
    free(T);
    free(q);
}

/**
 * Makes the solution, on the premise that b1 > B1 gives false
 */
bool make_solution(rational** q, rational* b1, rational* B1, int n2, size_t r)
{
	if(compare_r(b1, B1)){
		return false;
	}

    for(size_t i = n2; i < r; i++){
        if(sign(q[i]) == -1){
            return false;
        }
    }
    return true;
}

bool fm(size_t rows, size_t cols, signed char a[rows][cols], signed char c[rows])
{
    rational*** T;
	rational** q;

    size_t r = rows;
    size_t s = cols;

    int n1 = 0;
    int n2 = 0;

    T = calloc(r, sizeof(rational**));
	for(size_t i = 0; i < r; i++){
		T[i] = calloc(s, sizeof(rational*));
	}
	q = calloc(r, sizeof(rational*));

    for(size_t i = 0; i < rows; i++){
		for(size_t j = 0; j < cols; j++){
			T[i][j] = new_rational(a[i][j], 1);
		}
        q[i] = new_rational(c[i], 1);
	}

    while(1)
    {
        n1 = 0;
        n2 = 0;

        /* Set n1 and n2 */
        for(size_t i = 0; i < r; i++){
			if(sign(T[i][s - 1]) == 1){
				n1++;
			} else if (sign(T[i][s - 1]) == -1) {
				n2++;
			}
		}

		n2 += n1;

        /* Sort the inequalities */
        {
			int cpos = 0;
			int cneg = 0;
			int czero = 0;
			int positive[n1];
			int negative[n2 - n1];
			int zero[r - n2];
			
			/* Order the indexes */
			for(size_t i = 0; i < r; i++){
				if(sign(T[i][s - 1]) == 0){
					zero[czero] = i;
					czero++;
				} else if (sign(T[i][s - 1]) == 1){
					positive[cpos] = i;
					cpos++;
				} else {
					negative[cneg] = i;
					cneg++;
				}
			}

			rational* Ttemp[r][s];
			rational* qtemp[r];

			size_t count = 0;

			for(size_t i = 0; i < r; i++){
				for(size_t j = 0; j < s; j++){
					Ttemp[i][j] = T[i][j];
				}
				qtemp[i] = q[i];
			}
			for(int i = 0; i < n1; i++){
				for(size_t j = 0; j < s; j++){
					T[i][j] = Ttemp[positive[count]][j];
				}
				q[i] = qtemp[positive[count]];
				count++;
			}

			count = 0;

			for(int i = n1; i < n2; i++){
				for(size_t j = 0; j < s; j++){
					T[i][j] = Ttemp[negative[count]][j];
				}
				q[i] = qtemp[negative[count]];
				count++;
			}

			count = 0;

			for(size_t i = n2; i < r; i++){
				for(size_t j = 0; j < s; j++){
					T[i][j] = Ttemp[zero[count]][j];
				}
				q[i] = qtemp[zero[count]];
				count++;
			}
			
		}

        /* Divide the inequalities */
        for(int i = 0; i < n2; i++){
            divq(q[i], T[i][s - 1]);
            for(size_t j = 0; j < s - 1; j++){
                divq(T[i][j], T[i][s - 1]);
            }
            T[i][s - 1]->p = 1;
            T[i][s - 1]->q = 1; 
        }

        if(s == 1){
            rational b1;
            rational B1;

            if(n1 <= 0 || n2 <= n1){
                free_all(T, q, r, s);
                return true;
            }

			/* Maximize b1 and minimize B1 */

			b1.p = q[0]->p;
			b1.q = q[0]->q;

			B1.p = q[n1]->p;
			B1.q = q[n1]->q;

			for(int i = 1; i < n1; i++){
				if(!compare_r(q[i], &b1)){
					b1.p = q[i]->p;
					b1.q = q[i]->q;
				}
			}

			for(int i = n1 + 1; i < n2; i++){
				if(!compare_r(&B1, q[i])){
					B1.p = q[i]->p;
					B1.q = q[i]->q;
				}
			}

            bool result = make_solution(q, &B1, &b1, n2, r);
            free_all(T, q, r, s);

            return result;
        }

        /* Create rprime and new inequalities */
        int rprime = r - n2 + n1 * (n2 - n1);
        if(rprime == 0){
            free_all(T, q, r, s);
            return true;
        }

        rational*** oldT = T;
        rational** oldq = q;

        T = calloc(rprime, sizeof(rational**));
	    for(size_t i = 0; i < rprime; i++){
		    T[i] = calloc(s - 1, sizeof(rational*));
	    }
	    q = calloc(rprime, sizeof(rational*));

        int m = 0;

        for(int k = 0; k < n1; k++){
			for(int i = n1; i < n2; i++){
				for(size_t j = 0; j < s - 1; j++){
					T[m][j] = subq_n(oldT[k][j], oldT[i][j]);
				}
    			q[m] = subq_n(oldq[k], oldq[i]);
				m++;
			}
		}

        /* Bring back the zero coeff */
        for(int i = n2; i < r; i++){
            for(size_t j = 0; j < s - 1; j++){
                T[m][j] = new_rational(oldT[i][j]->p, oldT[i][j]->q);
            }
            q[m] = new_rational(oldq[i]->p, oldq[i]->q);
            m++;
        }        

        /* Free the old ineqs */
        free_all(oldT, oldq, r, s);

        s = s - 1;
        r = rprime;
    }
}