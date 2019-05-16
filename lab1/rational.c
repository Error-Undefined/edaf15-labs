#include <stdio.h>
#include <stdlib.h>

typedef struct rational rational;

struct rational {
	int p;
	int q;
};

void printrational(rational* r)
{
	printf("%d/%d\n",r->p,r->q);
}

void reduce(rational* r)
{
	int sign=-1;
	
	
	
	if((r->p > 0 && r->q > 0)||(r->p < 0 && r->q < 0)){
		sign = 1;
	}
	
	int p = (r->p > 0) ? r->p : -(r->p);
	int q = (r->q > 0) ? r->q : -(r->q);
	
	while(p != q){
		if( p > q ){
			p -= q;
		} else {
			q -= p;
		}
	}
	
	r->p = sign * (r->p);
	
	if(p==1){
		return;
	}
	
	r->p = (r->p)/p;
	r->q = (r->q)/p;
}

void addq(rational* r1,rational* r2)
{
	r1->p =(r1->p)*(r2->q)+(r2->p)*(r1->q);
	r1->q *= r2->q;
}

void subq(rational* r1,rational* r2)
{
	r1->p =(r1->p)*(r2->q)-(r2->p)*(r1->q);
	r1->q *= r2->q;
}

void mulq(rational* r1,rational* r2)
{
	r1->p *= r2->p;
	r1->q *= r2->q;
}

void divq(rational* r1,rational* r2)
{
	r1->p *= r2->q;
	r1->q *= r2->p;
}

int main(int argc, char** argv)
{
	rational r1={37, 13};
	rational r2={33, 15};
	
	addq(&r1, &r2);
	
	printrational(&r1);
	reduce(&r1);
	printrational(&r1);
}
