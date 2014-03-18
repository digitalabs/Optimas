#include "../headers/params.h"

using namespace std;

/**-----------------------------------------------------------------------------
	## FUNCTION:
	int get_geno_max(char **tabsrc, size_t & max)
	-----------------------------------------------------------------------------
	## RETURN:
	int nb : the number of elements in the array of strings tabsrc.
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ char** tabsrc : the array of strings.
	@ size_t & max : A reference length with which is compared each tabsrc string length.
	-----------------------------------------------------------------------------
	## SPECIFICATION: Count the number of elements into the tab of strings,
	and set max to the bigest string length if biger than max initial value.
	NB : tabsrc last element has to be NULL.
	-----------------------------------------------------------------------------
*/

int get_geno_max(char **tabsrc, int & max){
	int i = 0;
	int len;
	char* p = NULL;
	
	while(tabsrc[i]!=NULL){
		//cout<<i<<" : ";
		if ( (p=strchr(tabsrc[i],'/')) != NULL ){		// a '/' character is found => heterozygous.
			if( (len = strlen(p+1)+1) > max) max = len;	// length of the 2nd allele genotype.
			//cout<<len<<" / ";
			*p = '\0';									// for the 1st allele length measuring (the string should not be changed => we restore the initial value below)
		}
		if( (len = strlen(tabsrc[i])+1) > max) max = len; // homozygous or heterozygous first allele.
		//cout<<len<<endl;
		if (p!=NULL) *p = '/';							// restore the initial string if needed.
		i++;
	}
	return i;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int set_read2str(char **il_geno)
	-----------------------------------------------------------------------------
	## RETURN:
	int nb : the number of elements in the array il_geno.
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ char** il_geno : the array of strings representing the genotype of the IL 
	individual at each locus position.
	-----------------------------------------------------------------------------
	## SPECIFICATION: Count the number of elements into the tab of strings
	and set the correspondence containers read2str and str2read.
	This function should ONLY be called for IL individuals.
	NB : il_geno last element has to be NULL.
	-----------------------------------------------------------------------------
*/

int set_read2str(char **il_geno, s_params* params , geno_read & j){
	
	if(il_geno == NULL) return 0;
	
	int i = 0;
	//geno_read j = 0;
	char* p = NULL;
	map<string, geno_read>::iterator it;
	
	while(il_geno[i]!=NULL){

		if ( (p=strchr(il_geno[i],'/')) != NULL ){							// a '/' character is found => heterozygous.
			if( params->str2read.find(p+1) == params->str2read.end()){		// heterozygous 2nd allele genotype.
				if(j==255) return -1;										// we have reached the maximum number of different genotype !
				params->str2read[p+1] = j++;
			}
			*p = '\0';														// mark the end the 1st allele string ('/' will be restored afterward).
		}
		if( params->str2read.find(il_geno[i]) == params->str2read.end() ){	// homozygous or heterozygous first allele.
			if(j==255) return -1;											// we have reached the maximum number of different genotype !
			params->str2read[il_geno[i]] = j++;
		}
		if (p!=NULL) *p = '/';												// restore the initial string if needed.
		i++;
	}

	realloc1d(params->read2str, (int)params->str2read.size(), const char*);
	
	for(it = params->str2read.begin(); it != params->str2read.end(); ++it){
		params->read2str[it->second] = it->first.c_str();
	}
	return i;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int check_params(int argc, char** argv)
	-----------------------------------------------------------------------------
	## RETURN:
	int code: value corresponding to each option case
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int argc: number of parameters/options passed in argument to the program
	@ char **argv: main argument of the program
	-----------------------------------------------------------------------------
	## SPECIFICATION: verification of parameters and options passed in argument
	to the program.
	-----------------------------------------------------------------------------
*/

int check_params(int argc, char** argv){
	if (argc > 1) {
		if (strcmp(argv[1], "-v") == 0) return 'v'; // display optimas version.
		if (strcmp(argv[1], "-h") == 0) return 'h'; // usage message.
		if (argc < 7 && argc > 9) return 'e';
		return 1; // ok, next step is alloc_sparams();
	}
	return 'h'; // no arguments => usage message.
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	s_params* alloc_sparams(char **argv)
	-----------------------------------------------------------------------------
	## RETURN:
	s_params *params : structure containing all data parameters
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ char **argv: main argument of the program
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	A structure containing all the parameters needed by the program is created.
	-----------------------------------------------------------------------------
*/

s_params *alloc_sparams(int argc, char **argv){

	char* conv_check = NULL;

	// The structure containing all the parameters is created
	//// malloc deosn't call map_indiv_index constructor !!! => crash with VC++.
	////s_params *params = (s_params *)malloc(sizeof(s_params));
	s_params *params = new s_params;

	if(params){

		if( (argc < 7) || (argc > 8) ) {
			fprintf(stderr, "Error: number of arguments - format is not conform\n");
			exit(1);
		}

		// Initialization
		strcpy(params->file_geno, argv[1]);
		strcpy(params->file_mks, argv[2]);
		strcpy(params->file_root, argv[5]);
		sprintf(params->file_log, "%s/events.log", params->file_root);

		params->cut_off_haplo = strtod(argv[3],&conv_check);
		if( (argv[3] == conv_check) || (params->cut_off_haplo < 0.0) || (params->cut_off_haplo > 1.0) ) {
			fprintf(stderr, "Error: argument 3 (diplotype cut off) format is not conform\n");
			exit(1);
		}

		params->poisson_rate = strtod(argv[4],&conv_check);
		if(argv[4] == conv_check || (params->poisson_rate < 0.0) || (params->poisson_rate > 1.0) ) {
			fprintf(stderr, "Error: argument 4 (probability of the crossover filter) format is not conform\n");
			exit(1);
		}
		
		params->cut_off_locus_fixed_fav = 0.75; //// TO PUT IN ARGUMENT
		params->cut_off_locus_fixed_unfav = 0.75; //// TO PUT IN ARGUMENT
		params->cut_off_locus_fixed_hetero = 0.75; //// TO PUT IN ARGUMENT
		
		params->verbose = false;
		if( (argc == 8) && strcmp(argv[argc-1], "verb") == 0) {
			params->verbose = true; // write each individual gametes_set and diplotypes_set in output files
		}

		params->qtl_selected = atoi(argv[6]);
		if( params->qtl_selected < 0 ) {
			fprintf(stderr, "Error: argument 6 (QTL selected) format is not conform\n");
			exit(1);
		}
		params->mode = 0; // [0] by default and [1] for simulation option

		params->nb_indiv = 0; // Total number of individuals
		params->nb_founders = 0; // Total number of founders (parental lines)
		params->nb_qtl = 0; // Total number of QTL
		params->nb_mks = 0; // Total number of markers
		params->nb_mks_qtl = 0; // Total number of QTL + markers
		params->geno_loci_max = 2; // Geno + '\0' (parental alleles hve to be coded with a single character).

		params->nb_locus = 0;
		params->num_mk_inf = 0;
		params->num_mk_sup = 0;
		params->bool_warning = 0;

		params->coded2str = NULL;
		params->read2str = NULL;
		
		params->tab_index_qtl = NULL; // Vector of boolean for QTL index (all mks included)
		params->tab_scores_final = NULL; // Final table of scores (probabilities of favorable alleles for all QTL / each QTL)
		params->tab_homo_hetero_final = NULL; // Final table of homozygous +/-, heterozygous probabilities (Probabilities of the phased genotypes)
		params->tab_founders_final = NULL; // Final table of the origin of each individuals at QTL (probabilities of parental alleles for all QTL / each QTL)
		params->tab_check_diplo = NULL; // Probabilities of the phased genotypes checked

		params->map_indiv_index.clear();
	}
	else{
		ERROR;
		perror("Alloc s_params failed");
		exit(1);
	}
	return params;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void free_params(s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_params *params : structure containing all the parameters needed by the program.
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	The structure is released.
	-----------------------------------------------------------------------------
*/

void free_params(s_params *params){

	free(params->tab_index_qtl);
	params->tab_index_qtl = NULL;
	free(params->coded2str); // pointed strings are deleted via params->str2coded.clear() (see below)
	params->coded2str = NULL;
	free(params->read2str); // pointed strings are deleted via params->str2read.clear() (see below)
	params->read2str = NULL;

	free_alloc2d(params->tab_scores_final); // Probabilities of favorable alleles for each individual for all/each QTL
	free_alloc3d(params->tab_homo_hetero_final); // Probabilities of the phased genotypes (homo fav +/-, hetero)
	free_alloc3d(params->tab_founders_final); // Probabilities of parental alleles for all/each QTL
	free_alloc2d(params->tab_check_diplo); // probabilities of the phased genotypes checked
	params->map_indiv_index.clear();
	params->map_ind_qtl_events.clear();
	params->vect_rec_frac.clear();
	params->str2coded.clear();
	params->str2read.clear();
	
	delete params; // params is allocated with new => see alloc_params()
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void print_params(s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_params *params : structure containing all data parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Parameters informations are printed on the screen.
	-----------------------------------------------------------------------------
*/

void print_params(s_params *params){

	printf("file_geno: %s\n", params->file_geno);
	printf("file_map: %s\n", params->file_mks);
	printf("file_root: %s\n", params->file_root);
	printf("file_log: %s\n", params->file_log);

	printf("cut_off_haplo: %f\n", params->cut_off_haplo);
	//printf("cut_off_gam: %f\n", params->cut_off_gam);

	printf("qtl_selected: %d\n", params->qtl_selected);
	printf("mode: %d\n", params->mode);

	printf("nb_indiv: %d\n", params->nb_indiv);
	printf("nb_qtl: %d\n", params->nb_qtl);
	printf("nb_mks: %d\n", params->nb_mks);
	printf("nb_mks_qtl: %d\n", params->nb_mks_qtl);
	printf("nb_founders: %d\n", params->nb_founders);

	printf("nb_locus: %d\n", params->nb_locus);
	printf("num_mk_inf: %d\n", params->num_mk_inf);
	printf("num_mk_sup: %d\n", params->num_mk_sup);

	printf("bool_warning: %d\n", params->bool_warning);

	for(int i=0; i<params->nb_mks_qtl; i++){
		printf("%d ", params->tab_index_qtl[i]);
	}
	printf("\n");

	printf("Map individual/Index\n");
	for( map<string, int >::iterator iter_list = params->map_indiv_index.begin(); iter_list != params->map_indiv_index.end(); ++iter_list ){
		printf("%s - %d\n", ((*iter_list).first).c_str(), (*iter_list).second); // individual of the list
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void crea_folders(s_params *params, int argc, char **argv)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_params *params : structure containing all data parameters
	@ int argc : number of arguments of the main program
	@ char **argv : list of argument in the main
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creating folders and subfolders that will be used for storing results.
	-----------------------------------------------------------------------------
*/

void crea_folders(s_params *params, int argc, char **argv){

	char file_rep[NB_PATH_MAX], file_rep3[NB_PATH_MAX];
	int j, status;

	// Default
	if( params->mode == 0 ){
		/* Root path folder create */
		status = my_mkdir(params->file_root);

		if( status != 0 ){
			fprintf(stderr, "> Error in creating the file %s !!\n", params->file_root);
			fflush(stderr);
			exit(1);
		}
	}

	FILE *file_log = open_file(params->file_log, "a+");

	/* User command line for running the program */
	fprintf(file_log, "$ ");
	for(j=0; j<argc; j++){
		fprintf(file_log, "%s ", argv[j]);
	}
	fprintf(file_log, "\n\n");

	fclose(file_log);

	sprintf(file_rep, "%s/each_qtl", params->file_root);

	/* The folder for each QTL is created */
	status = my_mkdir(file_rep);

	if(status != 0){
		fprintf(stderr, "> Error in creating the file %s !!\n\n", file_rep);
		fflush(stderr);
		exit(1);
	}

	/* Only 1 QTL is selected */
	if( params->qtl_selected != 0 ){
		sprintf(file_rep3, "%s/qtl_%d", file_rep, params->qtl_selected);

		/* Creation of the repositories for each QTL */
		status = my_mkdir(file_rep3);

		if( status != 0 ){
			fprintf(stderr, "> Error in creating the file %s !!\n\n", file_rep3);
			fflush(stderr);
			exit(1);
		}
	}
	/* All QTL selected */
	else{
		/* Loop on all the qtl */
		for(j=0; j<params->nb_qtl; j++){

			sprintf(file_rep3, "%s/qtl_%d", file_rep, j + 1);

			/* Creation of the repositories for each QTL */
			status = my_mkdir(file_rep3);

			if( status != 0 ){
				fprintf(stderr, "> Error in creating the file %s !!\n\n", file_rep3);
				fflush(stderr);
				exit(1);
			}
		}
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void alloc_tab_results(s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_params *params : structure containing all data parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Memory allocation for the results tables (scores + probabilities of phased
	genotypes + parental origin).
	-----------------------------------------------------------------------------
*/

void alloc_tab_results(s_params *params){

	calloc2d(params->tab_scores_final, params->nb_indiv, params->nb_qtl + 1, double); // Probabilities of favorable alleles for each individual for all/each QTL
	calloc3d(params->tab_homo_hetero_final, params->nb_indiv, params->nb_qtl + 1, 3, double); // Probabilities of the phased genotypes (homo fav +/-, hetero)
	calloc3d(params->tab_founders_final, params->nb_indiv, params->nb_qtl + 1, params->nb_founders, double); // Probabilities of parental alleles for all/each QTL
	calloc2d(params->tab_check_diplo, params->nb_indiv, params->nb_qtl + 1, double); // Probabilities of the phased genotypes checked
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void progress_bar(s_params *params, int q, int i)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_params *params : structure containing all data parameters
	@ int q : the current QTL
	@ int i : the current individual
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Display the progress status of the run.
	-----------------------------------------------------------------------------
*/

void progress_bar(s_params *params, int q, int i){

	if( params->qtl_selected == 0 ){
		fprintf(stdout, ".: QTL %d - Ind %d - %d :.\n", q, i+1, (int)(((((q-1)*params->nb_indiv)+i+1.0)/(params->nb_qtl*params->nb_indiv*1.0))*100.0));
		fflush(stdout);
	}
	else{
		fprintf(stdout, ".: QTL %d - Ind %d - %d :.\n", params->qtl_selected, i+1, (int)(((i+1.0)/(params->nb_indiv*1.0))*100.0));
		fflush(stdout);
	}
}
