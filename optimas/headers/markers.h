#ifndef MARKERS_H_
#define MARKERS_H_

/*-------------------------------- Includes ---------------------------------*/

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../headers/utils.h"
#include "../headers/params.h"

/*---------------------------- Public structures ----------------------------*/

typedef struct s_mk{

	char name[NB_STRING_MAX]; // Name of the marker/QTL
	int chr; // Chromosome number of the maker/QTL localization
	int num_qtl; // Index of the QTL which the marker is associated to
	double pos; // Position of the marker in the chromosome
	int nb_all_fav; // Number of favorable alleles
	char **all_fav; // Name of the favorable alleles at QTL

} s_mk;


/*---------------------------- Public prototypes ----------------------------*/

s_mk *init_tab_mks(s_params *params);

void free_markers(s_mk *markers, int nb_mks);

void print_tab_mks(s_mk *markers, int nb_mks);

void find_index(s_mk *markers, s_params *params, int q);

void compute_rec_frac(s_mk* markers, s_params* params);

int is_fav(const char *allele, s_mk *markers, int index_mk);

void check_fav_alleles_input_files(s_mk *markers, s_params *params);


#endif /* MARKERS_H_ */
