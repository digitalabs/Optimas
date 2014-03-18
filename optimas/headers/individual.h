#ifndef INDIVIDUAL_H_
#define INDIVIDUAL_H_


/*-------------------------------- Includes ---------------------------------*/

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <map>
#include <algorithm>

#include "../headers/utils.h"
#include "../headers/params.h"
#include "../headers/markers.h"
#include "../headers/gametes.h"
#include "../headers/haplotypes.h"

/*---------------------------- Public structures ----------------------------*/

typedef struct s_indiv{

	char name[NB_STRING_MAX]; /* Name of the individual */
	char par1[NB_STRING_MAX]; /* Name of the 1st parent */
	char par2[NB_STRING_MAX]; /* Name of the 2nd parent */

	geno_read *geno1; /* Array of the individual genotype for the 1st allele for each marker */
	geno_read *geno2; /* Array of the individual genotype for the 2nd allele for each marker */

	s_gametes gam_set; /* Individual set of gametes */
	int nb_gametes; /* Number of gametes in the set */

	s_haplo haplo_set; /* Individual set of haplotypes */
	int nb_haplo; /* Number of haplotypes in the set */

	int index; /* Index of the individual (read on the genotype file) */
	int geno; /* [0] if there is no genotyping data and [1] otherwise */
	int children_total; // Total nb of children (individuals that have this individual as parent).
	int children; // Remaining children all along the process (will be (re)set to children_total for each QTL).
	//char parental_allele[NB_STRING_MAX]; /* NULL if the individual hasn't the type/step IL (Inbred Line) */
	char step[NB_STRING_MAX]; /* IL: parental lines, CR: cross, S#: selfing, HD: doubled haploid, RIL: Recombinant inbred line */
	char cycle[NB_STRING_MAX]; /* Generation of SAM cycles */
	char group[NB_STRING_MAX]; /* Classification of individuals in sub population */

} s_indiv;


/*---------------------------- Public prototypes ----------------------------*/

s_indiv *init_tab_indiv(s_mk *markers, s_params *params);

void print_tab_indiv(s_indiv *tab_indiv, s_params *params);

void free_tab_indiv(s_indiv *tab_indiv, int nb_indiv);

/*
int find_index_indiv(s_indiv *tab_indiv, char *parent_name, s_params *params, int index_current_indiv);

void init_gam_set_inbred_lines(s_indiv *tab_indiv, int index_indiv, s_params *params);

void uniq_gam_set_indiv(s_indiv *indiv, s_gametes *tab_gam_set, s_params *params, int *tab_nb_gam_set);

int fill_haplo_set_indiv(s_indiv *parent, s_params *params, int q);

int fill_gam_set_indiv(s_indiv *parent, s_params *params, int q);

int init_tab_parents(s_indiv *parent, s_params *params, int q);
*/

void crosses_steps(s_indiv *tab_indiv, s_mk *markers, s_params *params, int q);

void init_inbred_lines_indiv(s_indiv *indiv, s_params *params);

void cross_indiv(s_indiv *tab_indiv, s_indiv *indiv, int index_p1, int index_p2, s_params *params, int q);

int comp_geno_read_poss(s_indiv *indiv, s_gametes *gam_par1, s_gametes *gam_par2, int haplo_poss1, int haplo_poss2, s_params *params);

void haplo_set2gam_set_indiv(s_indiv *indiv, s_params *params);

void uniq_gam_set_cutoff_indiv(s_indiv *indiv, s_gametes *tab_gam_set, s_params *params, int *tab_nb_gam_set);

void copy_haplo_set_in_indiv(s_indiv *indiv, s_haplo *haplo_set, s_params *params, int nb_haplo);

void self_indiv(s_indiv *indiv, s_params *params, int nb_autofec, int q);

s_haplo cross_gam_self_indiv(s_indiv *indiv, s_gametes *gam_set, s_params *params, double *tab_sum_geno, int nb_gam, int num_haplo, int *p_nb_haplo, int bool_self);

void uniq_haplo_set_indiv(s_indiv *indiv, s_haplo *tab_haplo_set, s_params *params, int *tab_nb_haplo_set, int nb_haplo_prec);

int haplo_in_haplo_set_indiv(s_indiv * indiv, s_haplo *haplo_set, s_params *params, int num_haplo, int h);

void copy_haplo_in_haplo_set_indiv(s_indiv *indiv, s_haplo *haplo_set, s_params *params, int num_haplo_indiv, int num_haplo);

void free_gam_haplo_indiv(s_indiv *indiv);

void free_gam_haplo_tab_indiv(s_indiv *tab_indiv, int nb_indiv);

void ril_indiv(s_indiv *indiv, s_params *params, int q);

s_haplo cross_gam_ril_indiv(s_indiv *indiv, s_gametes *gam_set, s_params *params, double *tab_sum_geno, int nb_gam, int num_haplo, int *p_nb_haplo);

void hd_indiv(s_indiv *indiv, s_gametes *gam_set, int nb_gam, s_params *params, int q);

void write_haplo_gam_output_indiv(FILE *file_haplo, FILE *file_gam, s_indiv *indiv, s_params *params, int num_qtl);

void calcul_scores_indiv(s_indiv *indiv, s_mk *markers, s_params *params, int q);

void write_homo_hetero_output_indiv(FILE *fi, s_indiv *indiv, s_mk *markers, s_params *params, int num_qtl);


#endif /*INDIVIDUAL_H_*/
