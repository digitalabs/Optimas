#include "../headers/utils.h"

using namespace std;


/**-----------------------------------------------------------------------------
	## FUNCTION:
	FILE *open_file(const char *fpath, const char *mode)
	-----------------------------------------------------------------------------
	## RETURN:
	FILE *f : a pointer to the file.
	-----------------------------------------------------------------------------
	## PARAMETRES:
	@ const char *fpath : pointer to the path of the file that we want to open.
	@ const char *mode  : the mode for opening the file (reading, writting...)
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	The input file is opened (if possible).
	-----------------------------------------------------------------------------
*/

FILE *open_file(const char *fpath, const char *mode){

	FILE *f = fopen(fpath, mode);

	if( f == NULL){
		fprintf(stderr, "> Error opening the file %s !!\n", fpath);
		fflush(stderr);
		exit(1);
	}
	return f ;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int my_mkdir(char *filename)
	-----------------------------------------------------------------------------
	## RETURN: status of the function mkdir
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ char *filename : the name of the file
	-----------------------------------------------------------------------------
	## SPECIFICATION: creation of a directory (for Linux, Windows and Mac)
	-----------------------------------------------------------------------------
*/

int my_mkdir(char *filename){
	int status = -1;

	#ifdef __WINDOWS__
	status = mkdir(filename);
	#elif __linux__
	status = mkdir(filename, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#else
	status = mkdir(filename, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#endif

	return status;
}


/*-
 *  * Copyright (c) 1990, 1993
 *  *	The Regents of the University of California.  All rights reserved.
 *  *
 *  * Redistribution and use in source and binary forms, with or without
 *  * modification, are permitted provided that the following conditions
 *  * are met:
 *  * 1. Redistributions of source code must retain the above copyright
 *  *    notice, this list of conditions and the following disclaimer.
 *  * 2. Redistributions in binary form must reproduce the above copyright
 *  *    notice, this list of conditions and the following disclaimer in the
 *  *    documentation and/or other materials provided with the distribution.
 *  * 3. All advertising materials mentioning features or use of this software
 *  *    must display the following acknowledgement:
 *  *	This product includes software developed by the University of
 *  *	California, Berkeley and its contributors.
 *  * 4. Neither the name of the University nor the names of its contributors
 *  *    may be used to endorse or promote products derived from this software
 *  *    without specific prior written permission.
 *  *
 *  * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 *  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 *  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  * SUCH DAMAGE.
 *  */

//extern "C"

char* strsep2(char **stringp, const char *delim)
{
	register char *s;
	register const char *spanp;
	register int c, sc;
	char *tok;

	if ((s = *stringp) == NULL)
		return (NULL);
	for (tok = s;;) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*stringp = s;
				return (tok);
			}
		} while (sc != 0);
	}
	// NOTREACHED
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	char **str_split(char *s, const char *ct)
	-----------------------------------------------------------------------------
	## RETURN:
	char **tab : the tab of the line splitted in string (ending by NULL).
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ char *s : string
	@ const char *ct : delimiter
	-----------------------------------------------------------------------------
	## SPECIFICATION: A line is splitted by strings regarding the delimiter and
	placed into the table tab (ending by NULL).
   -----------------------------------------------------------------------------
*/

char **str_split(char *s, const char *ct){
	char **tab = NULL;

	if(s != NULL && ct != NULL){
		size_t i;
		char *cs = NULL;
		size_t size = 1;

		//for(i = 0; (cs = strtok(s, ct)); i++){
		for(i = 0; (cs = strsep2(&s, ct)); i++){
			if(size <= i + 1){
				void *tmp = NULL;
				size <<= 1;	//equivalent to size *= 2 => avoid calling realloc too many times.
				tmp = realloc(tab, sizeof (*tab) * size);

				if(tmp != NULL) tab = (char**)tmp;
				else{
					ERROR; //fprintf (stderr, "Insufficient memory! [str_split => utils.cpp]\n");
					perror("Error: memory allocation!");
					free(tab);
					tab = NULL;
					exit(EXIT_FAILURE);
				}
			}
			tab[i] = cs; //s = NULL;
		}
		tab[i] = NULL;
	}
	return tab;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int count(char **tabsrc)
	-----------------------------------------------------------------------------
	## RETURN:
	int nb : the number of elements of the matrix of string.
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ char** tabsrc : the array of strings.
	-----------------------------------------------------------------------------
	## SPECIFICATION: Count the number of elements into the tab of strings.
	NB : tabsrc last element has to be NULL.
	-----------------------------------------------------------------------------
*/

int count(char **tabsrc){
	int i = 0;
	
	if(tabsrc == NULL) return 0;

	while(tabsrc[i]!=NULL){
		i++;
	}
	return i;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void search(char *str, const char delim)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ char *str : a string (1 line of the file)
	@ const char delim : the delimiter to remove
	-----------------------------------------------------------------------------
	## SPECIFICATION: the delimiter (i.e '\n') is removed at the end of the line.
   ------------------------------------------------------------------------------
*/

void search(char *str, const char delim){
	char *p = strchr(str, delim);

	if (p){
		*p = 0;
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int count_nb_elt_in_cut_off(double *vect, int nb_elts, double cut_off)
	-----------------------------------------------------------------------------
	## RETURN:
	int cpt: number of elements in the vector >= cut off.
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ double *vect : vector of doubles
	@ int lenght_vect : number of element in the vector
	@ double cut_off : cut off studied
	-----------------------------------------------------------------------------
	## SPECIFICATION: return the number of values in the vector >= cut off.
	-----------------------------------------------------------------------------
*/

int count_nb_elts_in_cut_off(double *vect, int lenght_vect, double cut_off){
	int cpt = 0;

	for(int i=0; i<lenght_vect; i++){
		if(vect[i] >= cut_off){
			cpt++;
		}
	}
	return cpt;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int sum_vect_int(int *tab, int nb_colonnes)
	-----------------------------------------------------------------------------
	## RETURN:
	int sum : sum of the elements of the vector of integers
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int *tab : vector of integers
	@ int nb_colonnes : number of elements in the vector (dim)
	-----------------------------------------------------------------------------
	## SPECIFICATION: Sum of the vector of integers.
	-----------------------------------------------------------------------------
*/

int sum_vect_int(int *tab, int nb_columns){
	int sum = 0;

	for(int i = 0; i < nb_columns; i++){
		sum += tab[i];
	}
	return sum;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	double sum_vect_double(double *tab, int nb_colonnes)
	-----------------------------------------------------------------------------
	## RETURN:
	int sum: sum of the vector of doubles
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ double *tab : vector of doubles
	@ int nb_colonnes : number of columns
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Sum of the vector of doubles.
	-----------------------------------------------------------------------------
*/

double sum_vect_double(double *tab, int nb_columns){
	double sum = 0.0;

	for(int i = 0; i < nb_columns; i++){
		sum += tab[i];
	}
	return sum;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	maxIndex(double *a, int size_a)
	-----------------------------------------------------------------------------
	## RETURN:
	int max_index: the index of the maximum value of the array
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ double *a : array of doubles
	@ int size_a : size of the array
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Returns the index of the maximum value in an array
	-----------------------------------------------------------------------------
*/

int maxIndex(double *a, int size_a){
    assert(size_a > 0);
    int maxIndex = 0;
    for(int i=1; i<size_a; i++){
        if( a[i] > a[maxIndex] ){
            maxIndex = i;
        }
    }
    return maxIndex;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	unsigned long long genotype2base10(geno_coded* from, long n, long base, bool swap)
	-----------------------------------------------------------------------------
	## RETURN:
	ulong long result: the corresponding number in base 10 (see NB below).
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ geno_coded* from : array of unsigned character that we want to convert.
	  => in optimas, this could be a gamete, haplotype or di-haplotype genotype.
	@ long n : size of the array
	@ long base : base with which we must consider the array of string.
	  => in optimas, this is the number of (parental) alleles.
	@ bool swap : if true, swap each part of the given di-haplotype.
	  default is false (see the function prototype in utils.h).
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Considering an array of string as an number written in some base,
	convert this number	to a decimal (base 10) one.
	NB: - This function uses the std::map AllelicCode::code to fetch the digital code
	      of each marker genotype (each string of the array char** from).
	    - The returned integer value could be very big, depending on
	      the number of alleles and loci, so that the type long long is needed.
	    - Di-haplotype swap is needing to deals with duplicates by symmetry.
	-----------------------------------------------------------------------------
*/

unsigned long long genotype2base10(geno_coded* from, long n, long base, bool swap) {
	
	long i = n-2;

	unsigned long long result = from[n-1], prod = 1, tmp = 0; // for i=n-1

	while(i > -1){
		tmp = from[i--]*(prod*=base);

		// Check if the number of possibilities is higher than a unsigned long long
		if( (ULLONG_MAX - tmp) < result ){
			fprintf(stderr, "Error: the number of possibilities (gametes or diplotypes) are higher than 2^(64) which is the limit for this optimized version of OptiMAS algorithm!!\n");
			fflush(stderr);
			exit(1);
		}
		result += tmp;
	}
	if(swap == true) {
		i = n*2 - 1;

		while(i > n-1){
			tmp = from[i--]*(prod*=base);
			// Check if the number of possibilities is higher than a unsigned long long
			if( (ULLONG_MAX - tmp) < result ){
				fprintf(stderr, "Error: the number of possibilities (gametes or diplotypes) are higher than 2^(64) which is the limit for this optimized version of OptiMAS algorithm!!\n");
				fflush(stderr);
				exit(1);
			}
			result += tmp;
		}
	}
	return result;
}

/*
unsigned long long genotype2base10(geno_coded* from, long n, long base, bool swap) {

	long i = n-2;
	//long long result=AllelicCode::getCode(from[n-1]), prod = 1; // for i=n-1
	long long result=from[n-1], prod = 1; // for i=n-1
	while(i>-1) result += from[i--]*(prod*=base);
	if(swap==true) {
		i=n*2 - 1;
		while(i>n-1) result += from[i--]*(prod*=base);
	}
	return result;
}
*/

char haplo1_gt_haplo2(geno_coded* h1, geno_coded* h2, int nb_locus){
	for (int i=0; i<nb_locus; i++){
		if ( h1[i] != h2[i] ) return h1[i] - h2[i];
	}
	return 0;
}


int read1_eq_read2(geno_read* h1, geno_read* h2, int nb_locus){
	for (int i=0; i<nb_locus; i++){
		if (h1[i] != h2[i]) return h1[i] - h2[i];
	}
	return 0;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	fact(long n)
	-----------------------------------------------------------------------------
	## RETURN:
	long f: the factorial of n.
	return 0 for wrong (i.e. negative) values of n.
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ long n : the number which factorial is computed.
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Compute n! (iterative).
	-----------------------------------------------------------------------------
*/

long fact(long n){
	if(n<0) return 0; // error => return 0 (fasle).
	if(n<2) return 1;
	long f=n;
	while(--n>1) f*=n;
	return f;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	comb(long n, long p)
	-----------------------------------------------------------------------------
	## RETURN:
	long : the number of combinations of p element among a set of n element.
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ long n : the total number of element in the set.
	@ long p : the number of element of the subset. 
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Compute the combination number of p element among a set of n element.
	C(n,p) = n! / (n-p)!p!
	We use it to compute the number of gametes with exactly p COs,
	among the 2^n possible gametes,
	where n is the number of loci - 1
	-----------------------------------------------------------------------------
*/

long comb(long n, long p){		// C(n,p) = n! / (n-p)!p!
	if(p>n) return 0;			// error => return 0 (false)
	if(p==n || p==0) return 1;	// C(n,n) = C(n,0) = 1
	if(n/p > 1) p = n-p;		// computing C(10,4) gives the same result as C(10,6), but C(10,6) is faster. 
	long denom = fact(n-p);
	long num = n;
	while(--n>p) num*=n;		// simplify the numerator and the denominator by p!.
	return num/denom;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	find_k(double l)
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ double l : the parameter of the Poisson distribution.
	@ double pf : the probability we want to reach (default is 0.01).
	-----------------------------------------------------------------------------
	## RETURN:
	long k : the number of occurrences k we want to know for a given l and pf.
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Following a Poisson distribution with parameter l,
	compute the number of occurrences (k) such that p(X=k) <= pf (default is 0.01),
	=> Compute the max COs number that could happen within a given window (l in Morgan)
	that let catch 99% of the possible gametes.
	-----------------------------------------------------------------------------
*/

long find_k(double l, double pf){	// l in Morgan, pf default value is 0.01.
	long k=0;			// Nb de CO
	double p, expminusl, lpowk, factk;
	lpowk = factk = 1;
	p = expminusl = exp(-l); // for k=0
	
	while (p>pf){
		k++;
		//p = pow(l, (double)k) / fact(k) * expminusl;
		p = (lpowk*=l) / (factk*=k) * expminusl;
	}
	return k;
}
