#ifndef HAPLOTYPES_H_
#define HAPLOTYPES_H_

/*-------------------------------- Includes ---------------------------------*/

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include "../headers/markers.h"
#include "../headers/gametes.h"
#include "../headers/params.h"
#include "../headers/utils.h"

/*---------------------------- Public structures ----------------------------*/

typedef struct s_haplo{

	geno_read **read; /* Matrix: haplotypes read on the gel (genotyping) */
	geno_coded **coded; /* Matrix "real" coded from parental inbred lines */
	double *proba; /* Array: probability to obtain an haplotype */

	int *nb_poly_loci; /* Number of polymorphic loci for each possible haplotype */
	int **loca_poly_loci; /* Localization of polymorphic loci */

} s_haplo;


// Used to avoid time expensive sequential search of duplicates
typedef struct s_haplo_search_index{

	unsigned long long geno_b10;
	s_haplo* hset;
	long i_haplo;
	unsigned char nb_locus;
	bool swap; // mutable char swap; // Modified in stl_comp

} s_haplo_search_index;

/*---------------------------- Public prototypes ----------------------------*/


bool stl_comp_dihaplo(const s_haplo_search_index & i1, const s_haplo_search_index & i2);

bool stl_comp_dihaplo_hr(const s_haplo_search_index & dh1, const s_haplo_search_index & dh2);

int **crea_haplo_poss(int nb_haplo_poss, int nb_gam_p1, int nb_gam_p2);

int **crea_haplo_poss_autofec(int nb_haplo_poss, int nb_locus);

int **crea_haplo_poss_ril(int nb_gam);

void init_haplo_set(s_haplo *haplo_set);

void alloc_haplo_set(s_haplo *haplo_set, int nb_haplo, s_params *params);

void free_haplo_set(s_haplo *haplo_set);

void realloc_haplo_set(s_haplo *haplo_set, int new_nb_haplo, s_params *params);

void print_haplo_set(s_haplo *haplo_set, int nb_haplo, int nb_locus);

int haplo_in_haplo_set(s_haplo *haplo_set_max, s_haplo *haplo_set, s_params *params, int index_haplo_max, int h);

void copy_haplo_in_haplo_set(s_haplo *haplo_set_max, s_haplo *haplo_set, s_params *params, int index_haplo_max, int index_haplo);

s_gametes haplo2gam_set(s_haplo *haplo_set, s_params *params, int num_haplo, int *p_nb_gam, int mode);

void multiply_gam_per_haplo(s_gametes *gam_set, s_haplo *haplo_set, int nb_gam, int num_haplo);

void free_tab_haplo_set(s_haplo *tab_haplo_set, int nb_haplo_prec);


#endif /* HAPLOTYPES_H_ */
