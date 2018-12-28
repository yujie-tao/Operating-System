/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

/* @* Yujie Tao *@
 *	@* Oct.25, 2018 *@
 */


#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

int mallocTest(){
	printf("--------------------------\n");
	printf("--------mallocTest--------\n");
	printf("--------------------------\n");
	//mallocTest #1
    void *a = malloc(5);
    printf("mallocTest #1: malloc 5 %p\n", a);

    a = malloc(31);
    printf("mallocTest #1: malloc 31 %p\n",a);

    a = malloc(32);
    printf("mallocTest #1: malloc 32 %p\n",a);

    printf("mallocTest #1 Expected Result: Difference between each address should be 0x20, 32 in decimal\n\n");


    //mallocTest #2
    void *b = malloc(513);
    printf("mallocTest #2: malloc 513 %p\n",b);

    b = malloc(1023);
    printf("mallocTest #2: malloc 1023 %p\n",b);
    printf("mallocTest #2 Expected Result: Difference between two address should be 0x400, 1024 in decimal\n\n");
    b = malloc(1024);
    printf("mallocTest #2: malloc 1024 %p\n",b);
    printf("mallocTest #2 Expected Result: Difference between two address should be 0x400, 1024 in decimal\n\n");
    b = malloc(1024);
    printf("mallocTest #2: malloc 1023 %p\n",b);
    printf("mallocTest #2 Expected Result: Difference between two address should be 0x400, 1024 in decimal\n\n");
    
    //mallocTest #3
    void *c = malloc(1024);
    printf("mallocTest #3: malloc 1024 %p\n",c);

    c = malloc(1025);
    printf("mallocTest #3: malloc 1025 %p\n",c); 

    c = malloc(1025);
    printf("mallocTest #3: malloc 1025 %p\n",c);

    c = malloc(1026);
    printf("mallocTest #3: malloc 1026 %p\n",c); 

    printf("mallocTest #3 Expected Result: Difference between first two malloc should be 0x1400, 5120 in decimal\n");
    printf("mallocTest #3 Expected Result: Difference between the second and the third malloc should be 0x1100, 4096 in decimal\n");
    printf("mallocTest #3 Expected Result: Difference between the third and the fourth malloc should be 0x1100, 4096 in decimal\n\n");

    //mallocTest #4
    void *d = malloc(2048);
    printf("mallocTest #4: malloc 2048 %p\n",d);

    d = malloc(2048);
    printf("mallocTest #4: malloc 2048 %p\n",d);

    printf("mallocTest #4 Expected Result: Difference between two malloc should be 0x1100, 4096 in decimal\n\n");

    //mallocTest #5
    void *e = malloc(-1);
    printf("mallocTest #5: malloc -1 %p\n",e);

    e = malloc(2049);
    printf("mallocTest #5: malloc 2049 %p\n",e);

    e = malloc(1);
    printf("mallocTest #5: malloc 1 %p\n",e);

    e = malloc(1);
    printf("mallocTest #5: malloc 1 %p\n",e);

    e = malloc(0);
    printf("mallocTest #5: malloc 0 %p\n",e);

    printf("mallocTest #5 Expected Result: 0x0 for the first, second and the last malloc\n");
    printf("mallocTest #5 Expected Result: Difference between the third and the fourth malloc should be 0x20, 32 in decimal\n\n");
    
    return (errno);

}

int freeTest(){
	printf("--------------------------\n");
	printf("--------freeTest----------\n");
	printf("--------------------------\n");
	//freeTest #1
	void *a = malloc(32);
	printf("freeTest #1: malloc a 32 %p\n",a);
	a = malloc(32);
	printf("freeTest #1: malloc a 32 %p\n",a);
	free(a);
	printf("freeTest #1: free a\n");
	a = malloc(32);
	printf("freeTest #1: malloc a 32 after free a %p\n",a);
    free(a);
	printf("freeTest #1 Expected Result: malloc address before and after free should be the same\n\n");


	//freeTest #2
	void *b = malloc(127);
	printf("freeTest #2: malloc b 127 %p\n",b);
	b = malloc(129);
	printf("freeTest #2: malloc b 129 %p\n",b);
	free(b);
	printf("freeTest #2: free b\n");
	b = malloc(129);
	printf("freeTest #2: malloc b 129 after free b %p\n",b);
	printf("freeTest #2: malloc address before and after free should be the same\n");
	free(b);
	printf("freeTest #2: free b\n");
	b = malloc(129);
	printf("freeTest #2: malloc b 129 after free b %p\n",b);
	free(b);
	printf("freeTest #2 Expected Result: malloc address before and after free should be the same\n\n");



	//freeTest #3
	void *c = malloc(2048);
	printf("freeTest #3: malloc c 2048 %p\n",c);
	free(c);
	printf("freeTest #3: free c\n");
	c = malloc(2048);
	printf("freeTest #3: malloc c 2047 after free c %p\n",c);
	free(c);
	printf("freeTest #3 Expected Result: malloc address before and after free should be the same\n\n");


	//freeTest #4
	void *d = malloc(256);
	printf("freeTest #4: malloc d 256 %p\n",d);
	free(d);
	printf("freeTest #4: free d\n");
	free(d);
	printf("freeTest #4: free d\n");
	d = malloc(2048);
	printf("freeTest #4: malloc d 2048 after free d %p\n",d);
	printf("freeTest #4 Expected Result: malloc address before and after free should be the same\n\n");

	//freeTest #5
	printf("freeTest #5: free e, which is a NULL\n");
	void *e = NULL;
	//free(e);
	printf("freeTest #5: free e %p\n", e);
	printf("freeTest #5 Expected Result: get an error when the line is uncommented\n\n");

	return (errno);
}


int poisonTest(){
	printf("--------------------------\n");
	printf("--------poisonTest--------\n");
	printf("--------------------------\n");

	//poisonTest #1
	char *a = malloc(20);
	printf("poisonTest #1: malloc a 1024 %p\n",a);
	a[0]= 60;
	printf("poisonTest #1: set a[0] as 60\n");
	a[9]= 61;
	printf("poisonnTest #1: set a[9] as 61\n");
	printf("poisonnTest #1: print full a %p\n",a);
	for(int i=0; i<20; i++){
        printf("%x\n",a[i]);
    }

	//poisonTest #2
	printf("\n");
	free(a);
	printf("poisonTest #2: free a %p\n",a);
	printf("poisonnTest #1: print full a %p\n",a);
	for(int i=0; i<20; i++){
        printf("%x\n",a[i]);
    }

  	printf("\n");

	return (errno);
}

int munmapTest(){
	printf("--------------------------\n");
	printf("--------munmapTest--------\n");
	printf("--------------------------\n");
	//First SB
	int *a = malloc(1024);
	printf("testMunmap #1: malloc a 1024 %p\n",a);
	int *b = malloc(1024);
	printf("testMunmap #1: malloc b 1024 %p\n",b);
	int *c = malloc(1024);
	printf("testMunmap #1: malloc c 1024 %p\n",c);
	
	//Second SB
	int *d = malloc(1024);
	printf("testMunmap #1: malloc d 1024 %p\n",d);
	int *e = malloc(1024);
	printf("testMunmap #1: malloc e 1024 %p\n",e);
	int *f = malloc(1024);
	printf("testMunmap #1: malloc f 1024 %p\n",f);

	//Third SB
	int *g = malloc(1024);
	printf("testMunmap #1: malloc g 1024 %p\n",g);
	int *h = malloc(1024);
	printf("testMunmap #1: malloc h 1024 %p\n",h);
	int *i = malloc(1024);
	printf("testMunmap #1: malloc i 1024 %p\n",i);

	//Fourth SB
	int *j = malloc(1024);
	printf("testMunmap #1: malloc j 1024 %p\n",j);
	int *k = malloc(1024);
	printf("testMunmap #1: malloc k 1024 %p\n",k);
	int *l = malloc(1024);
	printf("testMunmap #1: malloc l 1024 %p\n",l);

    printf("testMunmap #1: Assign all pointer value 1\n");
	*a = 1;
	printf("testMunmap #1: a %p: %d\n",a,*a);
	*b = 1;
	printf("testMunmap #1: b %p: %d\n",b,*b);
	*c = 1;
	printf("testMunmap #1: c %p: %d\n",c,*c);
	*d = 1;
	printf("testMunmap #1: d %p: %d\n",d,*d);
	*e = 1;
	printf("testMunmap #1: e %p: %d\n",e,*e);
	*f = 1;
	printf("testMunmap #1: f %p: %d\n",f,*f);
	*g = 1;
	printf("testMunmap #1: g %p: %d\n",g,*g);
	*h = 1;
	printf("testMunmap #1: h %p: %d\n",h,*h);
	*i = 1;
	printf("testMunmap #1: i %p: %d\n",i,*i);
	*j = 1;
	printf("testMunmap #1: j %p: %d\n",j,*j);
	*k = 1;
	printf("testMunmap #1: k %p: %d\n",k,*k);
	*l = 1;
	printf("testMunmap #1: k %p: %d\n",l,*l);

	free(l);
	printf("testMunmap #1: free l %p\n",l);
	free(k);
	printf("testMunmap #1: free k %p\n",k);
	free(j);
	printf("testMunmap #1: free j %p\n",j);

	free(i);
	printf("testMunmap #1: free i %p\n",i);
	free(h);
	printf("testMunmap #1: free h %p\n",h);
	free(g);
	printf("testMunmap #1: free g %p\n",g);

	free(f);
	printf("testMunmap #1: free f %p\n",f);
	free(e);
	printf("testMunmap #1: free e %p\n",e);
	free(d);
	printf("testMunmap #1: free d %p\n",d);

	free(c);
	printf("testMunmap #1: free c %p\n",c);
	free(b);
	printf("testMunmap #1: free b %p\n",b);
	free(a);
	printf("testMunmap #1: free a %p\n",a);

	printf("testMunmap #1: Check object on address a: %d\n", *a);
        printf("testMunmap #1: Check object on address b: %d\n", *b);
        printf("testMunmap #1: Check object on address c: %d\n", *c);
        printf("testMunmap #1: Check object on address j: %d\n", *j);
        printf("testMunmap #1: Check object on address k: %d\n", *k);
        printf("testMunmap #1: Check object on address l: %d\n", *l);

        printf("testMunmap #1 Expected Result: Sgementation fault for d,e,f,g,h,i\n");
        printf("testMunmap #1: Check object on address d: %d\n", *d);
        printf("testMunmap #1: Check object on address e: %d\n", *e);
        printf("testMunmap #1: Check object on address f: %d\n", *f);
        printf("testMunmap #1: Check object on address g: %d\n", *g);
        printf("testMunmap #1: Check object on address h: %d\n", *h);
        printf("testMunmap #1: Check object on address i: %d\n", *i);



	return (errno);

}

int main() {

	mallocTest();
	freeTest();
	poisonTest();
	munmapTest();
 
    return (errno);
}
