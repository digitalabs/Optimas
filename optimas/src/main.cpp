#include "../headers/main.h"

using namespace std;

/**------------------------------------------------------------------------------------------------------
	## Date: 01/12/2009
	## Authors: Fabio VALENTE, Franck GAUTHIER and Guylaine BLANC.
	## Execution: [exe data_geno data_map cut_off_haplo cut_off_gam output_file num_qtl verbose]
	## ./bin/optimas /home/gqms/valente/workspace/optimas/input/moreau.dat /home/gqms/valente/workspace/optimas/input/moreau.map 0.000001 0.0 /home/gqms/valente/workspace/optimas/res/27_10_1983_17h00m00s 0 verb
	## Compilation: make all
	## --> be placed in the current directory (optimas) for the execution and compilation.
	------------------------------------------------------------------------------------------------------
	## Objective: to help breeders in implementing marker-assisted selection (MAS), with the possibility
	to consider a multi-allelic context, which opens new prospects to further accelerate genetic gain by
	assembling favorable alleles issued from diverse parental lines.
	------------------------------------------------------------------------------------------------------
	## Tool: developed to trace parental alleles identified as favorable throughout the selection
	generations, using neighboring markers that are often only partially informative.

	## Algorithms: implemented to compute probabilities of allele transmission in different MAS schemes
	and mating designs (intercrossing, selfing, backcrossing, double haploids, recombinant inbred lines).
	------------------------------------------------------------------------------------------------------
	This software is implemented in a C-ANSI language and will be integrated with the Generation Challenge
	Program (GCP) platform. A graphical interface (using Qt library in C++) is currently being developed
	to visualize results, help the selection of individuals and design intermating.
	------------------------------------------------------------------------------------------------------
*/

int main(int argc, char **argv){

	//mtrace(); // Starts the recording of memory allocations and releases

	int ret = check_params(argc, argv);

	switch(ret){
		case 'v' : fprintf(stdout, OPTIMAS_V); exit(0);
		case 'h' : fprintf(stdout, "%s%s", OPTIMAS_V, OPTIMAS_USAGE); exit(0);
		case 'e' : fprintf(stderr, "optimas error:\n%s", OPTIMAS_USAGE); exit(1);
	}

	// Structure containing all parameters given in the input file
	s_params *params = alloc_sparams(argc, argv);

	// Allocation and initialization of the array containing informations about all the markers
	s_mk *markers = init_tab_mks(params);
	//print_tab_mks(markers, params->nb_mks_qtl);

	// Allocation and initialization of the array containing individuals informations
	s_indiv *tab_indiv = init_tab_indiv(markers, params);
	//print_tab_indiv(tab_indiv, params);

	// Check the correspondence of the favorable alleles present in the 2 input files after table initialization
	check_fav_alleles_input_files(markers, params);

	// Creation of directories for results
	crea_folders(params, argc, argv);

	// Memory allocation for tables of results
	alloc_tab_results(params);
	//print_params(params);

	int current_qtl = 1, last_qtl = params->nb_qtl;

	// Only one QTL is selected
	if( params->qtl_selected != 0 ){
		current_qtl = params->qtl_selected;
		last_qtl = params->qtl_selected;
	}

	// Not really an error
	fprintf(stderr, ".: QTL %d - Ind %d - Mks %d :.\n", params->nb_qtl, params->nb_indiv, params->nb_mks);
	fflush(stderr);

	// Loop on QTL/chromosome (depending on the chosen option)
	for(int q=current_qtl; q <= last_qtl; q++){
		params->nb_locus = 0, params->num_mk_inf = 0, params->num_mk_sup = 0; params->hetero_res = false;

		// We find the number of loci and the index maker of the upper and lower bound of the QTL
		find_index(markers, params, q);
		//fprintf(stdout, "nb_locus = %d num_mk_inf = %d num_mk_sup = %d\n", params->nb_locus, params->num_mk_inf, params->num_mk_sup);
		//fflush(stdout);
		
		// Maximal number of recombinations between num_mk_inf and num_mk_sup according to a Poisson distribution (1%).
		if( params->poisson_rate > 0.0 ){
			double win_size = markers[params->num_mk_sup].pos - markers[params->num_mk_inf].pos; // cM
			params->est_max_rec_nb = find_k(win_size/100, params->poisson_rate); // Morgan
			//cerr<<"QTL "<<markers[params->num_mk_sup].num_qtl<<"est_max_rec_nb : "<<params->est_max_rec_nb<<endl; exit(0);
		}
		else params->est_max_rec_nb = -1; // no filter
		
		// Compute the recombination fraction r (and R) for each possible interval between num_mk_inf and num_mk_sup.
		compute_rec_frac(markers, params);

		// Creation of the gamete and haplotype set for each individual regarding its pedigree, genotype and intermating step
		crosses_steps(tab_indiv, markers, params, q);

		// The probability for each possible phased genotypes and gametes are calculated and written into files
		//crea_output_results(tab_indiv, markers, params, q);

		// Free memory for the set of gametes and haplotypes of all individuals
		free_gam_haplo_tab_indiv(tab_indiv, params->nb_indiv);
	}

	// Tables of probabilities (Molecular Score = prediction of genetic value, Homo/Hetero, Parental allele origin) are written
	crea_results_final_tabs(tab_indiv, params);

	if( params->bool_warning == 1 ){
		fprintf(stderr, "[Warning] Some results are doubtful...have a look to the events file [%s]\n", params->file_log);
		fflush(stderr);

		// Creation of a new file summarizing the questionable results by Individuals or QTL
		crea_summary_events(tab_indiv, params);
	}

	// Memory released
	free_markers(markers, params->nb_mks_qtl);
	free_tab_indiv(tab_indiv, params->nb_indiv);
	free_params(params);

	//muntrace(); // Ends the recording of memory allocations and releases

	return (EXIT_SUCCESS);
}
