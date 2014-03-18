#ifndef PARAMS_H_
#define PARAMS_H_

/*-------------------------------- Includes ---------------------------------*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <climits>
#include <ctime>
#include <string>
#include <map>
#include <vector>


#ifdef __WINDOWS__
	#include <io.h>
#else
	#include <sys/stat.h>
#endif

#include "../headers/utils.h"

#define OPTIMAS_V "optimas version 1.4\n"

#define OPTIMAS_USAGE "\nUsage:\n\n \
optimas -h\t\tDisplay this help\n \
optimas -v\t\tDisplay optimas version information\n\n \
optimas path/to/input.dat path/to/input.map haplo_cutoff gam_cutoff path/to/output_folder qtl_selected [verb]\n\n \
input_file.dat\t\t:Path to the genotype/pedigree file (.dat)\n \
input_file.map\t\t:Path to the genetic map/QTL position input file (.map)\n \
haplo_cutoff\t\t:Cut off (float number, 0.000001), genotypic probability below which a rare diplotype\n\t\t\t is removed and no more considered in subsequent computations (0.0 = no cut off)\n \
gam_cutoff\t\t:Cut off (float number, 0.0 by default) for gametic probability. It corresponds to the probability\n\t\t\t that the number of crossovers expected in the region between flanking markers exceeds a given value.\n\t\t\t Thus, unlikely gametes with number of crossovers over this value are removed and no more considered\n\t\t\t in subsequent computations. Use of this option with values up to 0.01 is recommended in case many\n\t\t\t flanking markers per QTL lead to high computation time with default option.\n \
results_folder\t\t:Path to the output folder where will be stored the results (must not exist)\n \
qtl_selected\t\t:The QTL that we want to study (0 = all QTL)\n \
verb\t\t\t:Verbose mode, write gametes sets and diplotypes sets files for each QTL and each individual \
\n"

/*---------------------------- Public structures ----------------------------*/

typedef struct s_rec_frac {double r; double R;} s_rec_frac;

typedef struct s_params {

	// Input files
	char file_mks[NB_PATH_MAX]; // Markers input file (map)
	char file_geno[NB_PATH_MAX]; // Genotypes and pedigrees input file
	char file_root[NB_PATH_MAX]; // Root file: creation of the results folder based on the date/time
	char file_log[NB_PATH_MAX]; // Error file

	// Files parameters
	int nb_indiv; // Total number of individuals
	int nb_founders; // Total number of founders (parental lines)
	int nb_qtl; // Total number of QTL
	int nb_mks; // Total number of markers
	int nb_mks_qtl; // Total number of QTL + markers
	int geno_loci_max; // 2 if SNP(Geno + '\0'), 1-n for microsatellites... == NB_ALLELE_MAX
	int mode; // [0] Default [1] Simulation

	double cut_off_haplo; // Cut off for the probability to keep an haplotype (0 = no cut off)
	//double cut_off_gam; // Cut off for the probability to keep a gamete (0 = no cut off)
	double cut_off_locus_fixed_fav; // Probability for a locus to be fixed homozygous favorable
	double cut_off_locus_fixed_unfav; // Probability for a locus to be fixed homozygous unfavorable
	double cut_off_locus_fixed_hetero; // Probability for a locus to be fixed heterozygous

	int *tab_index_qtl; // Vector of boolean for QTL index (all mks included)
	int qtl_selected; // QTL that we want to study  0: all QTL selected

	int nb_locus; // Number of markers per QTL (QTL included)
	int num_mk_inf; // Index of the lower marker for a QTL
	int num_mk_sup; // Index of the upper marker for a QTL
	int est_max_rec_nb; // Maximal number of recombinations between num_mk_inf and num_mk_sup according to the Poisson law estimation.
	double poisson_rate; // Poisson law probability we want to reach, error rate (traduire mieux que ca).
	bool hetero_res; // at least one of the parent IL is heterozygous residual for the current QTL interval.
	bool verbose; // write each individual gametes_set and diplotypes_set for each qtl in output files if true.

	int bool_warning; // If there are individuals with no possible haplotypes

	std::map<std::string, int> map_indiv_index; // {IL1}{0}

	std::map<std::string, std::vector<int> > map_ind_qtl_events; // {Id} {QTL2, QTL5, ..., QTLn}
	
	std::vector< std::vector<s_rec_frac> > vect_rec_frac; //// vect_rec_frac[iloc1][iloc2] => {r, R}

	std::map<std::string, geno_coded> str2coded; //// coded genotype at a maker position (string) => its code as an integer (see geno_coded in utils.h)
	//int* coded2str;  // coded2str[geno_coded allele_int] returns the index of the IL indiv from which we can get the parental allele (a string)
	const char** coded2str;  // coded2str[geno_coded allele_int] returns the address of a string set in str2coded (the parental allele)
	std::map<std::string, geno_read> str2read; //// read genotype at a maker position (string) => its code as an integer (see geno_read in utils.h)
	const char** read2str;  // read2str[geno_read allele_int] returns the address of a string set in str2read

	// Probabilities of favorable alleles for each individual
	double **tab_scores_final; // Final table of scores (all QTL / each QTL)

	// Probabilities of the phased genotypes
	double ***tab_homo_hetero_final; // Final table of homozygous +/-, heterozygous probabilities

	// Probabilities of parental alleles
	double ***tab_founders_final; // Final table of the origin of each individuals at QTL (all QTL / each QTL)

	// Check the sum of probabilities of the phased genotypes (cut-off)
	double **tab_check_diplo;

} s_params ;


/*---------------------------- Public prototypes ----------------------------*/

int get_geno_max(char **tabsrc, int & max);

int set_read2str(char **il_geno, s_params* params, geno_read & j);

int check_params(int argc, char** argv);

s_params* alloc_sparams(int argc, char **argv);

void free_params(s_params *params);

void print_params(s_params *params);

void crea_folders(s_params *params, int argc, char **argv);

void alloc_tab_results(s_params *params);

void progress_bar(s_params *params, int q, int i);


#endif /* PARAMS_H_ */
