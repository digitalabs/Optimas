#ifndef UTILS_H_
#define UTILS_H_

/*-------------------------------- Includes ---------------------------------*/

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <climits>


#if defined (_WIN64) || defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#define __WINDOWS__
#endif


#ifdef __WINDOWS__
	#include <io.h>
	#include <direct.h>
#else
	#include <sys/stat.h>
#endif


// Depends on the total number of markers (genotype file)
#define NB_LINE_MAX 3000000

// map file
#define NB_LINE_MEAN 3000

// Path of the file
#define NB_PATH_MAX 1024

#define NB_STRING_MAX 256

// Maximum number of selfing operation allowed
#define NB_SELFING_MAX 20

typedef unsigned char geno_coded;	// coded matrix data type => allow 255 possibilities (alleles)
typedef unsigned char geno_read; 	// read matrix data type => allow 255 possibilities (alleles)


#define ERROR { fprintf(stderr, "== %s, error line %d\n", __FILE__, __LINE__); fflush(stderr);}


#define malloc1d(p, nc, type){ \
	(p) = (type*)malloc((nc)*sizeof(type)); \
	if( (p) == NULL ){ \
		ERROR; \
		fprintf(stderr, "nc: %d ; sizeof: %zu\n", (nc), sizeof(type)); \
		perror("malloc1d failed"); \
		exit(1); \
	} \
}


#define realloc1d(ptr, n, type){ \
	type* ptr2 = (type*)realloc((ptr), (n)*sizeof(type)); \
	if( (ptr2) != NULL ){ \
		(ptr) = (ptr2); \
	} \
	else{ \
		free((ptr)); \
		ERROR; \
		fprintf(stderr, "n: %d\n", (n)); \
		perror("realloc1d failed"); \
		exit(1); \
	} \
}


#define calloc1d(p, nc, type){ \
	(p) = (type*)calloc((nc), sizeof(type)); \
	if( (p) == NULL ){ \
		ERROR; \
		fprintf(stderr, "nc: %d ; sizeof: %zu\n", (nc), sizeof(type)); \
		perror("calloc1d failed"); \
		exit(1); \
	} \
}


#define malloc2d(p, nl, nc, type){ \
	int i; \
	if( (nl > 0) && (nc > 0) ){ \
		(p) = (type**)malloc((nl)*sizeof(type*)); \
		if( (p) == NULL ){ \
			ERROR; \
			fprintf(stderr, "nl: %d\n", (nl)); \
			perror("malloc2d failed"); \
			exit(1); \
		} \
		(p)[0] = (type*)malloc((nl)*(nc)*sizeof(type)); \
		if( ((p)[0]) == NULL ){ \
			ERROR; \
			fprintf(stderr, "nl: %d ; nc: %d ; sizeof: %zu\n", (nl), (nc), sizeof(type)); \
			perror("malloc2d failed"); \
			exit(1); \
		} \
		for(i=1; i<(nl); i++){ \
			(p)[i] = (p)[0]+i*(nc); \
		} \
	} \
	else{ \
		(p) = NULL; \
	} \
}

//// faire un malloc pour (p) et un vrai calloc pour (p)[0]
#define calloc2d(p, nl, nc, type){ \
	int i; \
	if( (nl > 0) && (nc > 0) ){ \
		(p) = (type**)calloc(nl, sizeof(type*)); \
		if( (p) == NULL ){ \
			ERROR; \
			fprintf(stderr, "nl: %d\n", (nl)); \
			perror("calloc2d failed"); \
			exit(1); \
		} \
		(p)[0] = (type*)calloc((nl)*(nc), sizeof(type)); \
		if( ((p)[0]) == NULL ){ \
			ERROR; \
			fprintf(stderr, "nl: %d ; nc: %d ; sizeof: %zu\n", (nl), (nc), sizeof(type)); \
			perror("calloc2d failed"); \
			exit(1); \
		} \
		for(i=1; i<(nl); i++){ \
			(p)[i] = (p)[0]+i*(nc); \
		} \
	} \
	else{ \
		(p) = NULL; \
	} \
}


// CAUTION !!! : use realloc2d to REMOVE THE FIRST DIMENSION ENDINGS ELEMENTS (LINES) ONLY !!!
#define realloc2d(p, nl, nc, type) \
{ \
	type* p2 = (type*)realloc((p)[0], (nl)*(nc)*sizeof(type) ); \
	if (p2==NULL){ERROR;perror("realloc2d failed");exit(1);} \
	if (p2 != (p)[0]) { \
		/* reassign each pointer p[i]; */ \
		for (int i=0; i<(nl); i++) (p)[i]=p2+(i*(nc)); \
	} \
	type** p1 = (type**)realloc((p), (nl)*sizeof(type*) ); \
	if (p1==NULL){ERROR;perror("realloc2d failed");exit(1);} \
	if (p1 != (p)) (p) = p1; \
}


#define free_alloc2d(p){ \
	if( (p) != NULL ){ \
		free((p)[0]); \
		free((p)); \
		(p) = NULL; \
	} \
}


#define malloc3d(p , nl, nc, nh, type){ \
	int i,j; \
	if( (nl > 0) && (nc > 0) && (nh > 0) ){ \
		(p) = (type***)malloc((nl)*sizeof(type**)); \
		if((p) == NULL){ \
			ERROR; \
			fprintf(stderr, "nl: %d\n", (nl)); \
			perror("malloc3d failed"); \
			exit(1); \
		} \
		(p)[0] = (type**)malloc((nl)*(nc)*sizeof(type*)); \
		if( ((p)[0]) == NULL){ \
			ERROR; \
			fprintf(stderr, "nl: %d ; nc: %d\n", (nl), (nc)); \
			perror("malloc3d failed"); \
			exit(1); \
		} \
		(p)[0][0] = (type*)malloc((nl)*(nc)*(nh)*sizeof(type)); \
		if( ((p)[0][0]) == NULL){ \
			ERROR; \
			fprintf(stderr, "nl: %d ; nc: %d ; nh: %d ; sizeof: %zu\n", (nl), (nc), (nh), sizeof(type)); \
			perror("malloc3d failed"); \
			exit(1); \
		} \
		for(j=1; j<(nc); j++) (p)[0][j]=(p)[0][0]+j*(nh); \
		for(i=1; i<(nl); i++) { \
			(p)[i] = (p)[0]+i*(nc); \
			for(j=0; j<(nc); j++) (p)[i][j] = (p)[0][j]+i*(nh)*(nc); \
		} \
	} \
	else{ \
		(p) = NULL; \
	} \
}


// CAUTION !!! : use realloc3d to REMOVE THE FIRST DIMENSION ENDINGS ELEMENTS (LINES) ONLY !!!
#define realloc3d(p, nl, nc, nh, type) \
{ \
	type* p3 = (type*)realloc((p)[0][0], (nl)*(nc)*(nh)*sizeof(type) ); \
	if (p3==NULL){ERROR;perror("realloc3d failed");exit(1);} \
	if (p3 != (p)[0][0]) { \
		/* reassign each pointer p[i][j]; */ \
		for (int i=0; i<(nl)*(nc); i++) (p)[0][i]= p3+(i*(nh)); \
	} \
	type** p2 = (type**)realloc((p)[0], (nl)*(nc)*sizeof(type*) ); \
	if (p2==NULL){ERROR;perror("realloc3d failed");exit(1);} \
	if (p2 != (p)[0]) { \
		/* reassign each pointer p[i]; */ \
		for (int i=0; i<(nl); i++) (p)[i]=p2+(i*(nc)); \
	} \
	type*** p1 = (type***)realloc((p), (nl)*sizeof(type**) ); \
	if (p1==NULL){ERROR;perror("realloc3d failed");exit(1);} \
	if (p1 != (p)) (p) = p1; \
}

//// faire un malloc pour (p) et (p)[0], et un vrai calloc pour (p)[0][0]
#define calloc3d(p , nl, nc, nh, type){ \
	int i,j; \
	if( (nl > 0) && (nc > 0) && (nh > 0) ){ \
		(p) = (type***)calloc((nl), sizeof(type**)); \
		if((p) == NULL){ \
			ERROR; \
			fprintf(stderr, "nl: %d\n", (nl)); \
			perror("calloc3d failed"); \
			exit(1); \
		} \
		(p)[0] = (type**)calloc((nl)*(nc), sizeof(type*)); \
		if( ((p)[0]) == NULL){ \
			ERROR; \
			fprintf(stderr, "nl: %d ; nc: %d\n", (nl), (nc)); \
			perror("calloc3d failed"); \
			exit(1); \
		} \
		(p)[0][0] = (type*)calloc((nl)*(nc)*(nh), sizeof(type)); \
		if( ((p)[0][0]) == NULL){ \
			ERROR; \
			fprintf(stderr, "nl: %d ; nc: %d ; nh: %d ; sizeof: %zu\n", (nl), (nc), (nh), sizeof(type)); \
			perror("calloc3d failed"); \
			exit(1); \
		} \
		for(j=1; j<(nc); j++) (p)[0][j]=(p)[0][0]+j*(nh); \
		for(i=1; i<(nl); i++) { \
			(p)[i] = (p)[0]+i*(nc); \
			for(j=0; j<(nc); j++) (p)[i][j] = (p)[0][j]+i*(nh)*(nc); \
		} \
	} \
	else{ \
		(p) = NULL; \
	} \
}


#define free_alloc3d(p){ \
	if( (p) != NULL ){ \
		free((p)[0][0]); \
		free((p)[0]); \
		free((p)); \
		(p) = NULL; \
	} \
}


/*---------------------------- Public prototypes ----------------------------*/

FILE *open_file(const char *fpath, const char *mode);

int my_mkdir(char *filename);

char* strsep2(char **stringp, const char *delim);

char **str_split(char *s, const char *ct);

int count(char **tabsrc);

void search(char *str, const char delim);

int count_nb_elts_in_cut_off(double *vect, int lenght_vect, double cut_off);

int sum_vect_int(int *tab, int nb_columns);

double sum_vect_double(double *tab, int nb_columns);

int maxIndex(double *a, int size_a);

unsigned long long genotype2base10(geno_coded* from, long n, long base, bool swap=false);

char haplo1_gt_haplo2(geno_coded* h1, geno_coded* h2, int nb_locus);

int read1_eq_read2(geno_read* h1, geno_read* h2, int nb_locus);

long fact(long n);

long comb(long n, long p);

long find_k(double l, double pf=0.01);

#endif /* UTILS_H_ */
