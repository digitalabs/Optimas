#ifndef GAMETES_H_
#define GAMETES_H_

/*-------------------------------- Includes ---------------------------------*/

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include "../headers/markers.h"
#include "../headers/utils.h"
#include "../headers/params.h"

/*---------------------------- Public structures ----------------------------*/

typedef struct s_gametes{

	geno_read **read; /* Matrix: gametes are coded in alleles read on the gel */
	geno_coded **coded; /* Matrix: "real" gametes are coded (origin of alleles in relation to parental inbred lines) */
	double *proba; /* Array: probability to obtain a gamete */

} s_gametes;


// Used to avoid time expensive sequencial search of duplicates
typedef struct s_gam_search_index{ unsigned long long geno_b10; s_gametes* gset; long i_gam; unsigned char nb_locus;} s_gam_search_index;

/*---------------------------- Public prototypes ----------------------------*/
bool stl_comp_gam(const s_gam_search_index & g1, const s_gam_search_index & g2);

bool stl_comp_gam_hr(const s_gam_search_index & g1, const s_gam_search_index & g2);

int **crea_mat_rec(int nb_locus);

int** crea_mat_rec2(int nb_locus, int nb_poly_loci, int* loca_poly_loci );

int** crea_mat_rec3(int nb_locus, int & nb_comb, int* loca_poly_loci, int nrecmax=-1);

void print_mat_rec(int **mat_rec, int nb_locus);

void init_gam_set(s_gametes *gam_set);

void alloc_gam_set(s_gametes *gam_set, int nb_gam, s_params *params);

void free_gam_set(s_gametes *gam_set);

void realloc_gam_set(s_gametes *gam_set, int new_nb_gam, s_params *params);

void free_tab_gam_set(s_gametes *tab_gam_set, int nb_gam_set);

void print_gam_set(s_gametes *gam_set, int nb_gametes, int nb_locus);

int comp_gam(char **gam1, char **gam2, int nb_locus);

void copy_gam1_in_gam_set2(s_gametes *gam_set1, s_gametes *gam_set2, s_params *params, int num_gam1, int num_gam2);


#endif /* GAMETES_H_ */
