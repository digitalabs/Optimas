#include "../headers/individual.h"

using namespace std;


s_indiv *init_tab_indiv(s_mk *markers, s_params *params){

	s_indiv *tab_indiv = NULL;

	int cpt_lines = 0, nb_columns = 0, nb_columns_geno = 6, cpt_geno = 0, cpt_mks = 0; // Id/p1/p2/Step/Cycle/Group/mk1...mkn
	char *temp = (char *)malloc(NB_LINE_MAX*sizeof(char));
	bool geno = false;

	////std::map<std::string, geno_read> iterator it;
	
	params->str2coded.clear();
	if( params->coded2str != NULL ){
		free(params->coded2str);
		params->coded2str = NULL;
	}

	params->str2read.clear();
	if( params->read2str != NULL ){
		free(params->read2str);
		params->read2str = NULL;
	}
	
	params->str2read["?"] = 0; // 0 is assigned to missing data string "?".
	geno_read j = 1;
	
	FILE *file_geno = open_file(params->file_geno, "r") ;

	// Loop on the input file (line by line)
	while(fgets(temp, NB_LINE_MAX, file_geno) && (strcmp(temp, "\n") != 0) && (strcmp(temp, "\r\n") != 0) ){

		search(temp, '\n'); // New line at the end of the string is removed
		search(temp, '\r');

		// We split each column of each line in an array
		char **line = str_split(temp, "\t");
		nb_columns = count(line);

		// The number of columns for each line is checked
		if( nb_columns != nb_columns_geno + params->nb_mks && nb_columns != nb_columns_geno - 2 && nb_columns != nb_columns_geno - 1 && nb_columns != nb_columns_geno ){
			//printf("nb=%d\n", nb_columns);
			fprintf(stderr, "Error: number of informations l.%d of the genotype file\n", cpt_lines + 1);
			fflush(stderr);
			exit(1);
		}

		// The first line is checked [Id/p1/p2/Step/Cycle/Group/mk1/.../mkn]
		if( cpt_lines == 0 ){
			if( (strcmp(line[0], "Id") != 0) || (strcmp(line[1], "P1") != 0) || (strcmp(line[2], "P2") != 0) || (strcmp(line[3], "Step") != 0) || (strcmp(line[4], "Cycle") != 0) || (strcmp(line[5], "Group") != 0) ){
				fprintf(stderr, "Error: the header of the genotype/pedigree file (.dat) is not conform!\n");
				fflush(stderr);
				exit(1);
			}

			cpt_geno = 0;

			// Check the name of markers (header)
			for(int i=0; i<params->nb_mks_qtl; i++){
				// Marker position
				if( params->tab_index_qtl[i] == 0){
					if( strcmp(line[cpt_geno + nb_columns_geno], markers[i].name) != 0 ){
						fprintf(stderr, "Error: marker %s (header) of the genotype file doesn't match the one of the map file %s !\n", line[cpt_geno + nb_columns_geno], markers[i].name);
						fflush(stderr);
						exit(1);
					}
					cpt_geno++;
				}
			}
		}
		else{
			// Memory allocation for each individual
			realloc1d(tab_indiv, params->nb_indiv + 1, s_indiv);

			// Initialization of the set of haplotypes and gametes
			init_gam_set(&tab_indiv[params->nb_indiv].gam_set);
			init_haplo_set(&tab_indiv[params->nb_indiv].haplo_set);

			// Initialization of attributes for each individual
			if( strcmp(line[0], "") == 0 ){
				fprintf(stderr, "Error: name of the individual l.%d of the genotype/pedigree file\n", cpt_lines + 1);
				fflush(stderr);
				exit(1);
			}
			else{
				tab_indiv[params->nb_indiv].children_total = 0; // this indiv has no children yet.
				strcpy(tab_indiv[params->nb_indiv].name, line[0]); // Name of the individual
				tab_indiv[params->nb_indiv].index = params->nb_indiv; // Index of the individual

				// The name of the individual is added to the selected list if it is not already present on it
				if( params->map_indiv_index.find(line[0]) == params->map_indiv_index.end() ){
					params->map_indiv_index[line[0]] = params->nb_indiv; // Hash map [name_indiv] --> index_indiv
				}
				else{
					fprintf(stderr, "Error: the individual %s on l.%d of the genotype file is found twice\n", line[0], cpt_lines + 1);
					fflush(stderr);
					exit(1);
				}
			}
			// Pedigree
			if( (strcmp(line[1], "") == 0) || (strcmp(line[2], "") == 0) ){
				fprintf(stderr, "Error: pedigree name of the individual l.%d of the genotype file\n", cpt_lines + 1);
				fflush(stderr);
				exit(1);
			}
			else{
				strcpy(tab_indiv[params->nb_indiv].par1, line[1]); //// Store indexes instead of names
				strcpy(tab_indiv[params->nb_indiv].par2, line[2]);
			}
			// Step
			if( (strcmp(line[3], "IL") != 0) && (strcmp(line[3], "CR") != 0) && (strncmp(line[3], "S", 1) != 0) && (strcmp(line[3], "RIL") != 0) && (strcmp(line[3], "DH") != 0) ){
				fprintf(stderr, "Error: mating step (Step column)  of individual l.%d of the genotype/pedigree file\n", cpt_lines + 1);
				fflush(stderr);
				exit(1);
			}
			else{
				strcpy(tab_indiv[params->nb_indiv].step, line[3]);

				// Parental allele: only for inbred lines
				if( strcmp(line[3], "IL") == 0 ){
					//// TO IMPROVE: IL are checked twice
					// Setting read2str and str2read (correspondences between the read genotype as a string and as an integer)
					if (set_read2str(line + nb_columns_geno, params, j) == -1){
						fprintf(stderr, "Error: The number of valid (none missing) different genotypes cannot exceed 255\n");
						exit(1);
					}
					//strcpy(tab_indiv[params->nb_indiv].parental_allele, line[2]);
					
					// Setting coded2str and str2coded (correspondences between the coded genotype as a string and as an integer)
					params->str2coded[line[2]] = params->nb_indiv; // p2 = parental allele to follow through generations = index of individual

					params->nb_founders++;
				}
				else { // the parents of this indiv have one more children.
					int index_p1 = params->map_indiv_index[line[1]];
					int index_p2 = params->map_indiv_index[line[2]];
					tab_indiv[index_p1].children_total++;
					if (index_p2 != index_p1) tab_indiv[index_p2].children_total++;
				}
			}
			// Cycle
			if( (nb_columns >= 5) && (strcmp(line[4], "") != 0) ){
				strcpy(tab_indiv[params->nb_indiv].cycle, line[4]);
			}
			else{
				strcpy(tab_indiv[params->nb_indiv].cycle, "-");
			}
			// Group (sub population)
			if( (nb_columns >= 6) && (strcmp(line[5], "") != 0) ){
				strcpy(tab_indiv[params->nb_indiv].group, line[5]);
			}
			else{
				strcpy(tab_indiv[params->nb_indiv].group, "-");
			}

			cpt_geno = 0;
			geno = false;

			// Genotype
			tab_indiv[params->nb_indiv].geno1 = NULL;
			tab_indiv[params->nb_indiv].geno2 = NULL;

			// Individuals have been genotyped
			if( nb_columns == (nb_columns_geno + params->nb_mks) ){
				// Memory allocation
				malloc1d(tab_indiv[params->nb_indiv].geno1, params->nb_mks_qtl, geno_read);
				malloc1d(tab_indiv[params->nb_indiv].geno2, params->nb_mks_qtl, geno_read);

				cpt_mks = 0;

				// Loop on all markers and QTL
				for(int i=0; i<params->nb_mks_qtl; i++){
					// Marker position
					if( params->tab_index_qtl[i] == 0 ){
						// Homozygous or missing data (no "/")
						if( strchr(line[cpt_geno + nb_columns_geno],'/') == NULL ){
							// Missing data with tabulation (empty field) or "-".
							if( strlen(line[cpt_geno + nb_columns_geno]) == 0 || strcmp(line[cpt_geno + nb_columns_geno], "-") == 0 ){
								if( strcmp(line[3], "IL") == 0 ){
									fprintf(stderr, "Error: Missing data at marker %s, IL parental individual %s\nline %d in the genotype/pedigree file !\n", markers[i].name, tab_indiv[params->nb_indiv].name, cpt_lines + 1);
									fflush(stderr);
									exit(1);
								}
								tab_indiv[params->nb_indiv].geno1[i] = 0; // replace "?"
								tab_indiv[params->nb_indiv].geno2[i] = 0;
							}
							// Homozygous
							else{
								//// Check if each allele is present in str2read map (corresponding to IL)
								if( params->str2read.find(line[cpt_geno + nb_columns_geno]) == params->str2read.end() ){
									fprintf(stderr, "Genotyping error at marker %d:%s (different to founders) Id: %s line %d in the genotype/pedigree file !\n", cpt_mks + 1, markers[i].name, tab_indiv[params->nb_indiv].name, cpt_lines + 1);
									fflush(stderr);
									exit(1);
								}

								tab_indiv[params->nb_indiv].geno1[i] = tab_indiv[params->nb_indiv].geno2[i] = params->str2read[line[cpt_geno + nb_columns_geno]];
								geno = true;
							}
						}
						// Heterozygous
						else{
							// We split the genotype in an array
							char **line2 = str_split(line[cpt_geno + nb_columns_geno], "/");

							if( count(line2) == 2 ){
								// Missing Data allele 1
								if( strcmp(line2[0], "-") == 0 ){
									tab_indiv[params->nb_indiv].geno1[i] = 0;
								}
								else{
									//// Check if each allele is present in str2read map (corresponding to IL)
									if( params->str2read.find(line2[0]) == params->str2read.end() ){
										fprintf(stderr, "Genotyping error at marker %s (different to founders) Id: %s\nline %d in the genotype/pedigree file !\n", markers[i].name, tab_indiv[params->nb_indiv].name, cpt_lines + 1);
										fflush(stderr);
										exit(1);
									}

									tab_indiv[params->nb_indiv].geno1[i] = params->str2read[line2[0]];
								}
								// Missind Data allele 2
								if( strcmp(line2[1], "-") == 0 ){
									tab_indiv[params->nb_indiv].geno2[i] = 0;
								}
								else{
									//// Check if each allele is present in str2read map (corresponding to IL)
									if( params->str2read.find(line2[1]) == params->str2read.end() ){
										fprintf(stderr, "Genotyping error at marker %s (different to founders) Id: %s\nline %d in the genotype/pedigree file !\n", markers[i].name, tab_indiv[params->nb_indiv].name, cpt_lines + 1);
										fflush(stderr);
										exit(1);
									}

									tab_indiv[params->nb_indiv].geno2[i] = params->str2read[line2[1]];
								}
							}
							else{
								fprintf(stderr, "Error: marker %s of the individual %s at l.%d in the genotype file\n", markers[i].name, tab_indiv[params->nb_indiv].name, cpt_lines + 1);
								fflush(stderr);
								exit(1);
							}
							free(line2);
							geno = true;
						}
						cpt_geno++;
						cpt_mks++;
					}
					// QTL position
					else if( params->tab_index_qtl[i] == 1 ){
						tab_indiv[params->nb_indiv].geno1[i] = tab_indiv[params->nb_indiv].geno2[i] = 0;
					}
				}
			}

			// Genotype found for this individual
			if( geno ){
				tab_indiv[params->nb_indiv].geno = 1;
			}
			// The individual has not been genotyped yet
			else{
				tab_indiv[params->nb_indiv].geno = 0; // Memory allocated if line of missing data
			}
			params->nb_indiv++;
		}
		free(line);
		cpt_lines++;
	}
	fclose(file_geno);
	free(temp);

	// Setting coded2str and str2coded (correspondences between the coded genotype as a string and as an integer)
	map<string, geno_read>::iterator it;

	realloc1d(params->coded2str, (int)params->str2coded.size(), const char*);  // int * changed to char ** (indexes of IL no more used)

	for(it = params->str2coded.begin(); it != params->str2coded.end(); ++it){
		params->coded2str[it->second] = it->first.c_str();
	}

	/* ****
	map<string, geno_coded>::const_iterator it;
	printf("CODED\n");
	for (it=params->str2coded.begin(); it!=params->str2coded.end(); ++it){
		printf("alleles : %s => %d\n", it->first.c_str(), it->second);
		printf("alleles : %d => %s\n", it->second, tab_indiv[params->coded2str[it->second]].parental_allele);
	}
	printf("READ\n");
	map<string, geno_read>::const_iterator it2;
	for (it2=params->str2read.begin(); it2!=params->str2read.end(); ++it2){
		printf("alleles : %s => %d\n", it2->first.c_str(), it2->second);
		printf("alleles : %d => %s\n", it2->second, params->read2str[it2->second]);
	}
	exit(0);
	* ****/

	return tab_indiv;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void print_tab_indiv(s_indiv *tab_indiv, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *tab_indiv : array of individuals
	@ s_params *params : structure containing all data parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Display informations/attributes of each individual.
	-----------------------------------------------------------------------------
*/

void print_tab_indiv(s_indiv *tab_indiv, s_params *params){
	int i, j;

	for(i=0; i<params->nb_indiv; i++){
		printf("name: %s\n", tab_indiv[i].name);
		printf("par1: %s\n", tab_indiv[i].par1);
		printf("par2: %s\n", tab_indiv[i].par2);
		printf("index: %d\n", tab_indiv[i].index);
		printf("step: %s\n", tab_indiv[i].step);
		//printf("parental allele: %s\n", tab_indiv[i].parental_allele);
		printf("cycle: %s\n", tab_indiv[i].cycle);
		printf("group: %s\n", tab_indiv[i].group);
		printf("geno: %d\n", tab_indiv[i].geno);

		/* If the individual has been genotyped */
		if(tab_indiv[i].geno == 1){
			for(j=0; j<params->nb_mks_qtl; j++){
				printf("%d ", tab_indiv[i].geno1[j]); //// afficher la correspondance aussi
			}
			printf("\n");
			for(j=0; j<params->nb_mks_qtl; j++){
				printf("%d ", tab_indiv[i].geno2[j]);
			}
			printf("\n");
		}
		printf("\n");
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void free_tab_indiv(s_indiv *tab_indiv, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *tab_indiv : array of individuals
	@ s_params *params : structure containing all data parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Free memory for the set of haplotypes/gametes and the genotypes of all individuals.
	-----------------------------------------------------------------------------
*/

void free_tab_indiv(s_indiv *tab_indiv, int nb_indiv){

	/* Loop on the individuals */
	for(int i=0; i<nb_indiv; i++){
		free(tab_indiv[i].geno1);
		free(tab_indiv[i].geno2);
		tab_indiv[i].geno1 = NULL;
		tab_indiv[i].geno2 = NULL;

		// Free memory for the set of gametes of the current individual
		free_gam_set(&tab_indiv[i].gam_set);

		// Free memory for the set of haplotypes of the current individual
		free_haplo_set(&tab_indiv[i].haplo_set);
	}
	free(tab_indiv);
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void crosses_steps(s_indiv *tab_indiv, s_mk *markers, s_params *params, int q){
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *tab_indiv : pointer to the array of individuals
	@ s_mk *markers : pointer to the array of markers
	@ s_params *params : structure containing all data parameters
	@ int q : index of the current QTL
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of the gamete and haplotype set for each individual regarding its
	pedigree, genotype and intermating step.
	-----------------------------------------------------------------------------
*/
void crosses_steps(s_indiv *tab_indiv, s_mk *markers, s_params *params, int q){

	int index_p1 = -1, index_p2 = -1, nb_self = 0;

	// Optimization (RAM): output files need to be written before to release gametes and diplotypes for each individual
	char name_file_haplo[NB_PATH_MAX], name_file_gam[NB_PATH_MAX], name_file_homo_hetero[NB_PATH_MAX];
	FILE *file_haplo = NULL, *file_gam = NULL, *file_homo_hetero = NULL;

	// Output for all the phased genotypes for each qtl [all loci]
	sprintf(name_file_haplo, "%s/each_qtl/qtl_%d/qtl_%d_diplotypes_set.txt", params->file_root, q, q);

	// Output for all the gametes for each qtl [all loci]
	sprintf(name_file_gam, "%s/each_qtl/qtl_%d/qtl_%d_gametes_set.txt", params->file_root, q, q);

	// Output: probability to be homzygous (un)favorable, heterozygous for each qtl
	sprintf(name_file_homo_hetero, "%s/each_qtl/qtl_%d/qtl_%d_homo_hetero.txt", params->file_root, q, q);

	if( params->verbose == true ){
		file_haplo = open_file(name_file_haplo, "w");
		file_gam = open_file(name_file_gam, "w");

		// The headers are written
		fprintf(file_haplo, "#QTL\tId\thaplo1\thaplo2\tread1\tread2\tproba\tnb_diplo\n"); //// RM nb_haplo by nb_diplo
		fprintf(file_gam, "#QTL\tId\tgamete\tread\tproba\tnb_gam\n");
	}
	file_homo_hetero = open_file(name_file_homo_hetero, "w");


	// Loop on the individuals
	for(int i=0; i<params->nb_indiv; i++){

		progress_bar(params, q, i);

		// Default
		if( params->mode == 0 ){
			// Check if parents exist
			if( (strncmp(tab_indiv[i].step, "IL", 2) != 0) && ((params->map_indiv_index.find(tab_indiv[i].par1) == params->map_indiv_index.end()) || (params->map_indiv_index.find(tab_indiv[i].par2) == params->map_indiv_index.end())) ){
				fprintf(stderr, "Error: the name of the parents of the individual %s is not found in the data set !\n", tab_indiv[i].name);
				fflush(stderr);
				exit(1);
			}
			//// utiliser un iterateur dans le if precedent pour ne pas refaire la recherche dans le map.
			// We find the index of the parents of the current individual (IL not take into account)
			if( (strncmp(tab_indiv[i].step, "IL", 2) != 0) ){
				index_p1 = params->map_indiv_index[tab_indiv[i].par1];
				index_p2 = params->map_indiv_index[tab_indiv[i].par2];
			}

			// Reset the number of children to its initial value 
			tab_indiv[i].children = tab_indiv[i].children_total;
		}

		// Creation of the inbred lines gamete and haplotype set
		if( strncmp(tab_indiv[i].step, "IL", 2) == 0 ){
			init_inbred_lines_indiv(&tab_indiv[i], params);
			//printf("Id: %s - nb_gam: %d - nb_haplo: %d - QTL: %d - Step: IL\n", tab_indiv[i].name, tab_indiv[i].nb_gametes, tab_indiv[i].nb_haplo, q);
			//print_gam_set(&tab_indiv[i].gam_set, tab_indiv[i].nb_gametes, params->nb_locus);
			//print_haplo_set(&tab_indiv[i].haplo_set, tab_indiv[i].nb_haplo, params->nb_locus);
		}
		// Cross between the gametes of the 2 parents of the current individual
		else if( strcmp(tab_indiv[i].step, "CR") == 0 ){
			// default
			if( params->mode == 0 ){
				cross_indiv(tab_indiv, &tab_indiv[i], index_p1, index_p2, params, q);
			}

			// Creation of the possible gametes corresponding to the individual set of haplotypes
			haplo_set2gam_set_indiv(&tab_indiv[i], params);
			//printf("Id: %s - nb_gam: %d - nb_haplo: %d - QTL: %d - Step: CR\n", tab_indiv[i].name, tab_indiv[i].nb_gametes, tab_indiv[i].nb_haplo, q);
			//print_haplo_set(&tab_indiv[i].haplo_set, tab_indiv[i].nb_haplo, params->nb_locus);
			//print_gam_set(&tab_indiv[i].gam_set, tab_indiv[i].nb_gametes, params->nb_locus);
		}
		// Selfing
		else if( strncmp(tab_indiv[i].step, "S", 1) == 0 ){
			// Number of selfing
			nb_self = atoi(tab_indiv[i].step+1);

			if( (nb_self < 1) || (nb_self > NB_SELFING_MAX) ){
				fprintf(stderr, "Error: the number of selfing for the individual %s is not correct [1-%d] !\n", tab_indiv[i].name, NB_SELFING_MAX);
				fflush(stderr);
				exit(1);
			}

			// Default
			if( params->mode == 0 ){
				// Copy of the haplotype set attributes in the individual set of haplotypes //// peut etre pas necessaire et/ou copier les adresses...
				copy_haplo_set_in_indiv(&tab_indiv[i], &tab_indiv[index_p1].haplo_set, params, tab_indiv[index_p1].nb_haplo);
			}

			// Selfing done to the current individual
			self_indiv(&tab_indiv[i], params, nb_self, q);

			// Creation of the possible gametes corresponding to the individual set of haplotypes
			haplo_set2gam_set_indiv(&tab_indiv[i], params);

			//printf("Id: %s - nb_gam: %d - nb_haplo: %d - QTL: %d - Step: S%d\n", tab_indiv[i].name, tab_indiv[i].nb_gametes, tab_indiv[i].nb_haplo, q, nb_self);
			//print_haplo_set(&tab_indiv[i].haplo_set, tab_indiv[i].nb_haplo, params->nb_locus);
			//print_gam_set(&tab_indiv[i].gam_set, tab_indiv[i].nb_gametes, params->nb_locus);
		}
		// RIL
		else if( strncmp(tab_indiv[i].step, "RIL", 3) == 0 ){
			// Default
			if(params->mode == 0){
				// Copy of the haplotype set attributes in the individual set of haplotypes
				copy_haplo_set_in_indiv(&tab_indiv[i], &tab_indiv[index_p1].haplo_set, params, tab_indiv[index_p1].nb_haplo);
			}

			// Infinite selfing done to the current individual
			ril_indiv(&tab_indiv[i], params, q);

			// Creation of the possible gametes corresponding to the individual set of haplotypes
			haplo_set2gam_set_indiv(&tab_indiv[i], params);

			//printf("Id: %s - nb_gam: %d - nb_haplo: %d - QTL: %d - Step: RIL\n", tab_indiv[i].name, tab_indiv[i].nb_gametes, tab_indiv[i].nb_haplo, q);
			//print_haplo_set(&tab_indiv[i].haplo_set, tab_indiv[i].nb_haplo, params->nb_locus);
			//print_gam_set(&tab_indiv[i].gam_set, tab_indiv[i].nb_gametes, params->nb_locus);
		}
		// HD
		else if( strncmp(tab_indiv[i].step, "DH", 2) == 0 ){
			// Default
			if(params->mode == 0){
				hd_indiv(&tab_indiv[i], &tab_indiv[index_p1].gam_set, tab_indiv[index_p1].nb_gametes, params, q);
			}

			// Creation of the possible gametes corresponding to the individual set of haplotypes
			haplo_set2gam_set_indiv(&tab_indiv[i], params);
		}
		else{
			fprintf(stderr, "Error (wrong format): mating Step of individual %s !\n", tab_indiv[i].name);
			fflush(stderr);
			exit(1);
		}

		if( params->verbose == true ){
			// Optimization: new methods to release memory (gametes + diplotypes) if individual is no more used to contribute for the next cycle
			write_haplo_gam_output_indiv(file_haplo, file_gam, &tab_indiv[i], params, q);
		}

		// Probability of presence of favorables alleles at the QTL for each individual (score)
		calcul_scores_indiv(&tab_indiv[i], markers, params, q);

		// The probability to be homozygous favorable/unfavorable and heterozygous for all individuals at each QTL is written
		write_homo_hetero_output_indiv(file_homo_hetero, &tab_indiv[i], markers, params, q);

		//printf("Id: %s - p1: %s - p2: %s - total_child: %d - remaining_child: %d\n", tab_indiv[i].name, tab_indiv[i].par1, tab_indiv[i].par2, tab_indiv[i].children_total, tab_indiv[i].children);

		// Parents of this individual have now one less child
		// If this individual is their last child, we can free the memory blocs occupied by the parents
		if( (strncmp(tab_indiv[i].step, "IL", 2) != 0) && ((--tab_indiv[index_p1].children) == 0) ){
			free_gam_haplo_indiv(&tab_indiv[index_p1]);
			//printf("[P1]%s : free %s\n", tab_indiv[i].name, tab_indiv[index_p1].name);
		}
		if( (strncmp(tab_indiv[i].step, "IL", 2) != 0) && (index_p2 != index_p1) && ((--tab_indiv[index_p2].children) == 0) ){
			free_gam_haplo_indiv(&tab_indiv[index_p2]);
			//printf("[P2]%s : free %s\n", tab_indiv[i].name, tab_indiv[index_p2].name);
		}

		// This indivividual may have no children => we can free the memory blocs occupied by this individual
		if( tab_indiv[i].children == 0 ){
			free_gam_haplo_indiv(&tab_indiv[i]);
			//printf("[NO CHILD]%s : free %s\n", tab_indiv[i].name, tab_indiv[i].name);
		}
	}
	// Output files are closed
	if( params->verbose == true ){
		fclose(file_haplo);
		fclose(file_gam);
	}
	fclose(file_homo_hetero);
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void init_inbred_lines_indiv(s_indiv *indiv, int index_indiv, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : current individual (inbred line - IL - in this case)
	@ s_params *params : structure containing all program parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Initialization of the individual gamete and haplotype set (Inbred lines).
	-----------------------------------------------------------------------------
*/

void init_inbred_lines_indiv(s_indiv *indiv, s_params *params){
	int nb_poly_loci = 0; // k = 0
	int* loca_poly_loci = NULL;
	
	calloc1d(loca_poly_loci, params->nb_locus, int);

	// Number of heterozygous loci
	for(int l = 0; l < params->nb_locus; l++){
		// Polymorphic locus
		// if geno1 != geno2 at this position (mks)
		if( (loca_poly_loci[l] = indiv->geno1[params->num_mk_inf + l] - indiv->geno2[params->num_mk_inf + l]) != 0 ){
			nb_poly_loci++;
		}
	}

	// The IL is homozygous
	if( nb_poly_loci == 0 ){
		indiv->nb_haplo = 1;
	}
	// The IL is heterozygous residual
	else {
		// There are 2^(nb_poly_loci-1) possible haplotypes for this IL indiv at the current QTL
		indiv->nb_haplo = (int) pow(2.0, (double) (nb_poly_loci - 1) );
		params->hetero_res = true;
	}

	// Memory allocation for the set of haplotypes
	indiv->haplo_set.nb_poly_loci = NULL;
	calloc1d(indiv->haplo_set.nb_poly_loci, indiv->nb_haplo, int);

	indiv->haplo_set.proba = NULL;
	calloc1d(indiv->haplo_set.proba, indiv->nb_haplo, double);

	indiv->haplo_set.loca_poly_loci = NULL;
	calloc2d(indiv->haplo_set.loca_poly_loci, indiv->nb_haplo, params->nb_locus, int);

	// Pairs of haplotypes (diplotypes) => nb_locus*2
	indiv->haplo_set.read = NULL;
	malloc2d(indiv->haplo_set.read, indiv->nb_haplo, params->nb_locus*2, geno_read);

	indiv->haplo_set.coded = NULL;
	malloc2d(indiv->haplo_set.coded, indiv->nb_haplo, params->nb_locus*2, geno_coded);

	// Number of possible gametes
	indiv->nb_gametes = (int) pow(2.0, (double) nb_poly_loci);

	// Creation of the recombination matrix
	int** mat_rec = NULL;

	// Creation of the gamete recombination matrix depending on the the localisation of the polymorphic loci and the est_max_rec_nb filter
	// NB: params->est_max_rec_nb is set to -1.0 if no filter is required. 
	// CAUTION : The value of nb_gametes may therefore be reduced by the number of gametes ignored
	mat_rec = crea_mat_rec3(params->nb_locus, indiv->nb_gametes, loca_poly_loci, params->est_max_rec_nb);
	//mat_rec = crea_mat_rec2(params->nb_locus, nb_poly_loci, loca_poly_loci); ////
	free(loca_poly_loci); ////
	
	//geno_coded parental_allele_int = params->str2coded[ tab_indiv[index_indiv].parental_allele ];
	geno_coded parental_allele_int = indiv->index; // the parental allele int correspond to the index of the parental line

	// Creation of the set of haplotypes (pairs of haplotypes)
	for(int h = 0; h < indiv->nb_haplo; h++){
		// Loop on markers
		for(int l = 0; l < params->nb_locus; l++){
			// Read
			if( mat_rec[h][l] == 0 ){
				indiv->haplo_set.read[h][l] = indiv->geno1[params->num_mk_inf + l];
				indiv->haplo_set.read[h][l + params->nb_locus] = indiv->geno2[params->num_mk_inf + l];
			}
			else if( mat_rec[h][l] == 1 ){
				indiv->haplo_set.read[h][l] = indiv->geno2[params->num_mk_inf + l];
				indiv->haplo_set.read[h][l + params->nb_locus] = indiv->geno1[params->num_mk_inf + l];
			}
			// Coded
			indiv->haplo_set.coded[h][l] = parental_allele_int;
			indiv->haplo_set.coded[h][l + params->nb_locus] = parental_allele_int;

			// Polymorphic locus
			if( indiv->haplo_set.read[h][l] != indiv->haplo_set.read[h][l + params->nb_locus] ){
				indiv->haplo_set.nb_poly_loci[h]++;
				indiv->haplo_set.loca_poly_loci[h][l] = 1; // Hetero residual
			}
		}
		indiv->haplo_set.proba[h] = 1.0/indiv->nb_haplo;
	}

	// Memory allocation for the set of gametes
	indiv->gam_set.read = NULL;
	malloc2d(indiv->gam_set.read, indiv->nb_gametes, params->nb_locus, geno_read);

	indiv->gam_set.coded = NULL;
	malloc2d(indiv->gam_set.coded, indiv->nb_gametes, params->nb_locus, geno_coded);

	indiv->gam_set.proba = NULL;
	calloc1d(indiv->gam_set.proba, indiv->nb_gametes, double);


	// Initialization of the probabilities regarding the presence of polymorphic loci
	for(int g = 0; g < indiv->nb_gametes; g++){
		// Possible gametes are created (from the haplotype) regarding the recombination matrix
		for(int l = 0; l < params->nb_locus; l++){
			if( mat_rec[g][l] == 0 ){
				indiv->gam_set.read[g][l] = indiv->geno1[params->num_mk_inf + l];
				indiv->gam_set.coded[g][l] = parental_allele_int;
			}
			else if( mat_rec[g][l] == 1 ){
				indiv->gam_set.read[g][l] = indiv->geno2[params->num_mk_inf + l];
				indiv->gam_set.coded[g][l] = parental_allele_int;
			}
		}
		// Probabilities are calculated
		indiv->gam_set.proba[g] = 1.0/indiv->nb_gametes;
	}
	free_alloc2d(mat_rec);
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void cross_indiv(s_indiv *tab_indiv, int index_indiv, int index_p1, int index_p2, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *tab_indiv : array of individuals
	@ s_indiv *indiv : current individual
	@ int index_indiv : index of the current individual
	@ int index_p1 : index of the 1st parent of the individual
	@ int index_p2 : index of the 2nd parent of the individual
	@ s_params *params : structure containing all data parameters
	@ int q: index of the current QTL
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Generation of an individual by crossing its 2 parents.
	-----------------------------------------------------------------------------
*/

void cross_indiv(s_indiv *tab_indiv, s_indiv *indiv, int index_p1, int index_p2, s_params *params, int q){

	int nb_haplo_poss = 0, nb_gam_p1 = 0, nb_gam_p2 = 0, cpt_haplo = 0, nb_haplo = 0;
	long allele_nb = params->str2coded.size();
	long dihaplo_size = params->nb_locus*2;
	double sum_proba_haplo_poss = 0.0;

	// Number of possible gametes of the individual parents
	nb_gam_p1 = tab_indiv[index_p1].nb_gametes;
	nb_gam_p2 = tab_indiv[index_p2].nb_gametes;

	// Number of possible phased genotypes
    nb_haplo_poss = nb_gam_p1 * nb_gam_p2;

	// Memory allocation for the table of combination of possible phased genotypes
	int **haplo_poss = NULL;
	haplo_poss = crea_haplo_poss(nb_haplo_poss, nb_gam_p1, nb_gam_p2);

	/* Each element of the array (of structure) haplo_search_indexes will store :
	a base 10 number representation of each di-haplotypes genotype (via the function genotype2base10),
	and a pointer to the haplo_set it belongs to (e.g. haplo_set_max.proba + h), which are first set to 0 and NULL respectively (calloc).
	Once the array is filled (in the loop below), it is sorted according to the genotype numbers.
	We can then efficiently search and merge duplicates and update the resulting probabilities (by addition).
	*/
	s_haplo_search_index* haplo_search_indexes = NULL;
	calloc1d(haplo_search_indexes, nb_haplo_poss, s_haplo_search_index);

	// Gametes of the current individual parents
	s_gametes *gam_par1 = &tab_indiv[index_p1].gam_set;
	s_gametes *gam_par2 = &tab_indiv[index_p2].gam_set;

	// Maximum set of possible haplotypes (filter on genotypes)
	s_haplo haplo_set_max;

	// Memory allocation of the possible phased genotypes (Max)
	alloc_haplo_set(&haplo_set_max, nb_haplo_poss, params);

	// Loop on the possible di-haplotypes (filter on genotypes)
	for(int h = 0; h < nb_haplo_poss; h++){

		// Matrix are equal after comparing read and coded or no genotyping data for the individual
		if( comp_geno_read_poss(indiv, gam_par1, gam_par2, haplo_poss[h][0], haplo_poss[h][1], params) ){

			// Calculating the probability of the possible haplotype
			haplo_set_max.proba[cpt_haplo] = gam_par1->proba[haplo_poss[h][0]] * gam_par2->proba[haplo_poss[h][1]];

			// Matrix for haplotypes read and coded are created
			for(int m = 0; m < params->nb_locus; m++){
				haplo_set_max.read[cpt_haplo][m] = gam_par1->read[haplo_poss[h][0]][m];
				haplo_set_max.read[cpt_haplo][m + params->nb_locus] = gam_par2->read[haplo_poss[h][1]][m];
				haplo_set_max.coded[cpt_haplo][m] = gam_par1->coded[haplo_poss[h][0]][m];
				haplo_set_max.coded[cpt_haplo][m + params->nb_locus] = gam_par2->coded[haplo_poss[h][1]][m];

				// We find the number of polymorphic loci and their location (coded) + hetero residual (read)
				if( (haplo_set_max.coded[cpt_haplo][m] != haplo_set_max.coded[cpt_haplo][m + params->nb_locus]) || (haplo_set_max.read[cpt_haplo][m] != haplo_set_max.read[cpt_haplo][m + params->nb_locus]) ){
					haplo_set_max.nb_poly_loci[cpt_haplo]++;
					haplo_set_max.loca_poly_loci[cpt_haplo][m] = 1;
				}
			}
			sum_proba_haplo_poss += haplo_set_max.proba[cpt_haplo];
			
			// haplo1 and haplo2 are 1st and 2nd part of the current di-haplotype.
			// The lower must always be first (=> avoid duplicate di-haplotypes by symmetry issue)
			char first = haplo1_gt_haplo2( haplo_set_max.coded[cpt_haplo], haplo_set_max.coded[cpt_haplo]+params->nb_locus, params->nb_locus);
			// h1 > h2 (coded)
			if ( first > 0 ){
				haplo_search_indexes[cpt_haplo].geno_b10 = genotype2base10(haplo_set_max.coded[cpt_haplo], params->nb_locus, allele_nb, true); //// swap the haplotypes of this di-haplotypes
				haplo_search_indexes[cpt_haplo].swap = true; // needed in case of residual heterozygous IL
			}
			// h1 < h2 (coded)
			else if( first < 0 ){
				haplo_search_indexes[cpt_haplo].geno_b10 = genotype2base10(haplo_set_max.coded[cpt_haplo], dihaplo_size, allele_nb); ////
				haplo_search_indexes[cpt_haplo].swap = false; // needed in case of residual heterozygous IL
			}
			// h1 == h2 (coded)
			else{
				// h1 > h2 (read)
				if( read1_eq_read2(haplo_set_max.read[cpt_haplo], haplo_set_max.read[cpt_haplo]+params->nb_locus, params->nb_locus) > 0 ){
					haplo_search_indexes[cpt_haplo].geno_b10 = genotype2base10(haplo_set_max.coded[cpt_haplo], params->nb_locus, allele_nb, true); //// swap the haplotypes of this di-haplotypes
					haplo_search_indexes[cpt_haplo].swap = true; // needed in case of residual heterozygous IL
				}
				// h1 <= h2 (read)
				else{
					haplo_search_indexes[cpt_haplo].geno_b10 = genotype2base10(haplo_set_max.coded[cpt_haplo], dihaplo_size, allele_nb); ////
					haplo_search_indexes[cpt_haplo].swap = false; // needed in case of residual heterozygous IL
				}
			}

			// Fetch the address of the current di-haplotype proba in haplo_set_max
			haplo_search_indexes[cpt_haplo].i_haplo = cpt_haplo;
			haplo_search_indexes[cpt_haplo].hset = &haplo_set_max; // Pointer incrementation.
			haplo_search_indexes[cpt_haplo].nb_locus = params->nb_locus;
			cpt_haplo++;
		}
	}

	// Sorting haplo_search_indexes based on s_haplo_search_index.geno_b10
	if(params->hetero_res) sort(haplo_search_indexes, haplo_search_indexes + cpt_haplo, stl_comp_dihaplo_hr);
	else sort(haplo_search_indexes, haplo_search_indexes + cpt_haplo, stl_comp_dihaplo);

	// We calculate the probability of each haplotype for each individual
	for(int h=0; h<cpt_haplo; h++){
		haplo_set_max.proba[h] = haplo_set_max.proba[h] / sum_proba_haplo_poss;
	}

	// Memory allocation of the possible phased genotypes (for the current individual)
	alloc_haplo_set(&indiv->haplo_set, cpt_haplo, params);

	// Filter on the set of haplotypes. Each haplotype is found only once (without duplicates) and its proba >= cut_off (filter cut-off rare di-haplo)
	for(int h=0; h<cpt_haplo; h++){
		// The number of real haplotypes of the individual without doubles is updated
		int next_h = h+1;
		long & i_haplo_dest = haplo_search_indexes[h].i_haplo;
		// Duplicates probabilities are added
		while ( (next_h < cpt_haplo) && (haplo_search_indexes[next_h].geno_b10 == haplo_search_indexes[h].geno_b10) ){
			long & i_haplo_src = haplo_search_indexes[next_h].i_haplo;

			if (params->hetero_res){
				// If dihaplotypes are different (break)
				if(haplo_search_indexes[next_h].swap == haplo_search_indexes[h].swap){
					if(read1_eq_read2(haplo_set_max.read[i_haplo_dest], haplo_set_max.read[i_haplo_src], params->nb_locus*2) != 0){
						break;
					}
				}
				else{
					if(read1_eq_read2(haplo_set_max.read[i_haplo_dest] + params->nb_locus, haplo_set_max.read[i_haplo_src], params->nb_locus) != 0
					|| read1_eq_read2(haplo_set_max.read[i_haplo_dest], haplo_set_max.read[i_haplo_src] + params->nb_locus, params->nb_locus) != 0){
						break;
					}
				}
			}
			haplo_set_max.proba[i_haplo_dest] += haplo_set_max.proba[i_haplo_src];
			next_h++;
		}
		
		if ( haplo_set_max.proba[i_haplo_dest] >= params->cut_off_haplo ){ //// => la somme des proba des haplo n'est plus 1, pas grave ?
			copy_haplo_in_haplo_set(&haplo_set_max, &indiv->haplo_set, params, (int)i_haplo_dest, nb_haplo++);	// nb_haplo is post-incremented
		}
		h = next_h - 1;
	}

	// Free memory for the 1st set (max) of haplotypes (filter on genotypes)
	free_haplo_set(&haplo_set_max);
	
	free(haplo_search_indexes);
	haplo_search_indexes = NULL;
	
	indiv->nb_haplo = nb_haplo;
	if (nb_haplo < cpt_haplo) { //// can't be >
		realloc_haplo_set(&indiv->haplo_set, nb_haplo, params);
	}

	// A warning message is written in the log file if the individual has no haplotype in its set
	if( indiv->nb_haplo == 0 ){
		params->bool_warning = 1;
		FILE *file_log = open_file(params->file_log, "a+");
		fprintf(file_log, "Id: %s\tQTL %d\tStep: CR\tNo.haplotypes=%d\tNo.possible haplotypes=%d\t#%d#%d\n", indiv->name, q, indiv->nb_haplo, nb_haplo_poss, indiv->index, q);
		fclose(file_log);
		params->map_ind_qtl_events[string(indiv->name)].push_back(q);
	}

	free_alloc2d(haplo_poss);
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int comp_geno_read_poss(s_indiv *indiv, s_gametes *gam_par1, s_gametes *gam_par2, int haplo_poss1, int haplo_poss2, s_params *params){
	-----------------------------------------------------------------------------
	## RETURN: [1] if the phased genotype is possible according to the data
	(genotypes) [0] otherwise.
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : pointer to the current individual
	@ s_gametes *gam_par1 : set of gametes of the parent 1 of the individual
	@ s_gametes *gam_par2 : set of gametes of the parent 2 of the individual
	@ int haplo_poss1 : index of the current gamete of the parent 1
	@ int haplo_poss2 : index of the current gamete of the parent 2
	@ s_params *params : structure containing all program parameters
   -----------------------------------------------------------------------------
   ## SPECIFICATION:
	Comparison between the current phased genotype tested and the data (genotyping).
	It returns [1] if the phased genotype is possible according to the data
	and [0] otherwise.
   -----------------------------------------------------------------------------
*/

int comp_geno_read_poss(s_indiv *indiv, s_gametes *gam_par1, s_gametes *gam_par2, int haplo_poss1, int haplo_poss2, s_params *params){

	// Test if the individual has no genotyping data
	if(indiv->geno == 0){
		return 1; // All the possible genotypes are taken into account
	}

	for(int m = 0; m < params->nb_locus; m++){
		// We are not on a missing data/QTL and the genotypes are not the same
		if(!
			(
			 (
			  // 0 is the code for "?" (missing data).
			  ((indiv->geno1[params->num_mk_inf + m] == 0) || (indiv->geno1[params->num_mk_inf + m] == gam_par1->read[haplo_poss1][m]))
			  &&
			  ((indiv->geno2[params->num_mk_inf + m] == 0) || (indiv->geno2[params->num_mk_inf + m] == gam_par2->read[haplo_poss2][m]))
			 )
			 ||
			 (
			  ((indiv->geno1[params->num_mk_inf + m] == 0) || (indiv->geno1[params->num_mk_inf + m] == gam_par2->read[haplo_poss2][m]))
			  &&
			  ((indiv->geno2[params->num_mk_inf + m] == 0) || (indiv->geno2[params->num_mk_inf + m] == gam_par1->read[haplo_poss1][m]))
			 )
			)
		  ){
			return 0;
		}
	}

	return 1;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void haplo_set2gam_set_indiv(s_indiv *indiv, s_mk *markers, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETRES:
	@ s_indiv *indiv : pointer to an individual
	@ s_mk *markers : array of markers
	@ s_params *params : structure containing all program parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of the possible gametes corresponding to the individual set of haplotypes.
	-----------------------------------------------------------------------------
*/

void haplo_set2gam_set_indiv(s_indiv *indiv, s_params *params){

	int nb_gam = 0;
	int *p_nb_gam = &nb_gam; // Pointer to the number of gametes

	indiv->nb_gametes = 0; // Reset of the number of gametes

	// Array of set of gametes
	s_gametes *tab_gam_set = NULL;
	malloc1d(tab_gam_set, indiv->nb_haplo, s_gametes);

	// Array of the number of gametes for each set
	int *tab_nb_gam_set = NULL;
	calloc1d(tab_nb_gam_set, indiv->nb_haplo, int);

	// Loop on the haplotypes
	for(int h=0; h<indiv->nb_haplo; h++){
		nb_gam = 0;

		// The set of gametes (for each haplotype) is created and stocked in the array
		tab_gam_set[h] = haplo2gam_set(&indiv->haplo_set, params, h, p_nb_gam, 1);

		// The probability for each gamete is multiplied by the probability of the haplotype
		multiply_gam_per_haplo(&tab_gam_set[h], &indiv->haplo_set, (*p_nb_gam), h);

		// The number of gametes for each set is stocked
		tab_nb_gam_set[h] = (*p_nb_gam);
	}
	// We collect all the gametes by keeping the different ones and adding the probability of the same ones
	uniq_gam_set_cutoff_indiv(indiv, tab_gam_set, params, tab_nb_gam_set);

	free_tab_gam_set(tab_gam_set, indiv->nb_haplo);
	free(tab_nb_gam_set);
	tab_nb_gam_set = NULL;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void uniq_gam_set_cutoff_indiv(s_indiv *indiv, s_gametes *tab_gam_set, s_params *params, int *tab_nb_gam_set)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : pointer to an individual
	@ s_gametes *tab_gam_set : array of set of gametes
	@ s_params *params : structure containing all data parameters
	@ int *tab_nb_gam_set : array of the number of gametes for each set of tab_gam_set
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of an unique set of gametes starting from an array of set of gametes.
	If some gametes are in doubles theirs probabilities are added.
	-----------------------------------------------------------------------------
*/

void uniq_gam_set_cutoff_indiv(s_indiv *indiv, s_gametes *tab_gam_set, s_params *params, int *tab_nb_gam_set){

	int max_nb_gam = sum_vect_int(tab_nb_gam_set, indiv->nb_haplo);
	int cpt_gam = 0;
	
	long allele_nb = params->str2coded.size();
	s_gam_search_index* gam_search_indexes = NULL;
	calloc1d(gam_search_indexes, max_nb_gam, s_gam_search_index);

	// The new set (max) of gametes of the current individual is created
	alloc_gam_set(&indiv->gam_set, max_nb_gam, params);

	// Loop on the array of sets of gametes
	for(int gset=0; gset<indiv->nb_haplo; gset++){
		// Loop on the set "g"
		for(int g=0; g<tab_nb_gam_set[gset]; g++){

			gam_search_indexes[cpt_gam].geno_b10 = genotype2base10(tab_gam_set[gset].coded[g], params->nb_locus, allele_nb);
			
			// Fetch the address of the current gamete proba in the gamset
			gam_search_indexes[cpt_gam].i_gam = g;
			gam_search_indexes[cpt_gam].gset = tab_gam_set + gset; // Pointer incrementation
			gam_search_indexes[cpt_gam].nb_locus = params->nb_locus;
			cpt_gam++;
		}
	}

	// Sorting gam_search_indexes based on s_gam_search_index.geno_b10
	if(params->hetero_res) sort(gam_search_indexes, gam_search_indexes + max_nb_gam, stl_comp_gam_hr);
	else sort(gam_search_indexes, gam_search_indexes + max_nb_gam, stl_comp_gam);

	cpt_gam = 0;

	// Filter on the set of gametes. Each gamete is found only once and its proba >= cut_off
	for(int g=0; g<max_nb_gam; g++){
		int next_g = g+1;
		long & i_gam_dest = gam_search_indexes[g].i_gam;

		s_gametes & gset_dest = *(gam_search_indexes[g].gset);

		// Duplicates probabilities are added.
		while ( (next_g < max_nb_gam) && (gam_search_indexes[next_g].geno_b10 == gam_search_indexes[g].geno_b10) ){
			long & i_gam_src = gam_search_indexes[next_g].i_gam;
			s_gametes & gset_src = *(gam_search_indexes[next_g].gset);

			// If hetero residual parents exist
			if( params->hetero_res && (read1_eq_read2( gset_dest.read[i_gam_dest], gset_src.read[i_gam_src], params->nb_locus) != 0) ){
				break;
			}

			gset_dest.proba[i_gam_dest] += gset_src.proba[i_gam_src];
			next_g++;
		}
		//if ( gset_dest.proba[i_gam_dest] >= params->cut_off_gam ){ // Old version of cut-off gametes (sum proba is not 1 anymore --> apply a ponderation)
		copy_gam1_in_gam_set2(&gset_dest, &indiv->gam_set, params, (int)i_gam_dest, cpt_gam++);	// cpt_gam is post-incremented
		//}
		g = next_g - 1;
	}
	
	free(gam_search_indexes);
	gam_search_indexes = NULL;
	
	indiv->nb_gametes = cpt_gam;

	// Free memory for the non existant gametes (duplicates)
	realloc_gam_set(&indiv->gam_set, indiv->nb_gametes, params);
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void copy_haplo_set_in_indiv(s_indiv *indiv, s_haplo *haplo_set, s_params *params, int nb_haplo)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : pointer to the current individual
	@ s_haplo *haplo_set : set of haplotypes
	@ s_params *params : structure containing all program parameters
	@ int nb_haplo : number of haplotypes
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Copy of the haplotype set attributes in the individual set of haplotypes.
	-----------------------------------------------------------------------------
*/

void copy_haplo_set_in_indiv(s_indiv *indiv, s_haplo *haplo_set, s_params *params, int nb_haplo){

	// Memory allocation for all the attributes of the set of haplotypes
	alloc_haplo_set(&indiv->haplo_set, nb_haplo, params);
	indiv->nb_haplo = nb_haplo;

	for(int i=0; i<nb_haplo; i++){
		// Copy of the haplotypes probabilities
		indiv->haplo_set.proba[i] = haplo_set->proba[i];

		// Copy of the number of polymorphic loci in this haplotype
		indiv->haplo_set.nb_poly_loci[i] = haplo_set->nb_poly_loci[i];

		// Copy of the localization of polymorphic loci
		for(int j=0; j<params->nb_locus; j++){
			indiv->haplo_set.loca_poly_loci[i][j] = haplo_set->loca_poly_loci[i][j];
		}

		// Copy of the matrix of phased genotype read and coded
		for(int j=0; j<(2*params->nb_locus); j++){
			indiv->haplo_set.read[i][j] = haplo_set->read[i][j];
			indiv->haplo_set.coded[i][j] = haplo_set->coded[i][j];
		}
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void self_indiv(s_indiv *indiv, s_mk *markers, s_params *params, int nb_self, int q)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : pointer to the current individual
	@ s_mk *markers : array of markers
	@ s_params *params : structure containing all data parameters
	@ int nb_self : number of selfing to do for the current individual
	@ int q : index of the current QTL
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Selfing done to the current individual.
	-----------------------------------------------------------------------------
*/

void self_indiv(s_indiv *indiv, s_params *params, int nb_self, int q){
	int nb_gam = 0, nb_haplo = 0;
	int *p_nb_gam = &nb_gam; // Pointer to the number of gametes
	int *p_nb_haplo = &nb_haplo; // Pointer to the number of haplotypes
	double sum;

	// Loop on the number of selfing
	for(int a=0; a<nb_self; a++){

		// Creation of the array of set of haplotypes
		s_haplo *tab_haplo_set = NULL;
		malloc1d(tab_haplo_set, indiv->nb_haplo, s_haplo);

		// Creation of the array containing the number of haplotypes for each set
		int *tab_nb_haplo_set = NULL;
		calloc1d(tab_nb_haplo_set, indiv->nb_haplo, int);

		// Creation of the array containing the sum of possible genotypes for each set
		double *tab_sum_geno = NULL;
		calloc1d(tab_sum_geno, indiv->nb_haplo, double);

		// Loop on the haplotypes
		for(int h=0; h<indiv->nb_haplo; h++){
			nb_gam = 0;
			nb_haplo = 0;

			// Creation of gametes from each haplotype
			s_gametes gam_set = haplo2gam_set(&indiv->haplo_set, params, h, p_nb_gam, 1);

			//printf("[%s] --> GAM_SET S%d\tnb_gam=%d\n", indiv->name, nb_self, (*p_nb_gam));
			//print_gam_set(&gam_set, (*p_nb_gam), params->nb_locus);

			// Creation of the possible haplotypes from these gametes (selfing) then this set is stocked in the array
			tab_haplo_set[h] = cross_gam_self_indiv(indiv, &gam_set, params, tab_sum_geno, (*p_nb_gam), h, p_nb_haplo, nb_self-a); // Realloc not done in purpose

			//printf("[%s] --> HAPLO_SET S%d\tnb_haplo=%d\n", indiv->name, nb_self, (*p_nb_haplo));
			//print_haplo_set(&tab_haplo_set[h], (*p_nb_haplo), params->nb_locus);

			// The number of haplotypes for each set is stocked
			tab_nb_haplo_set[h] = (*p_nb_haplo);

			free_gam_set(&gam_set);
		}

		// Last selfing: cut off on the genotype (weighting of each probability)
		if( ((nb_self-a) == 1) && (indiv->geno == 1)){ //// ENLEVER INDIV->GENO == 1 ????
			sum = sum_vect_double(tab_sum_geno, indiv->nb_haplo);
			// Loop on each haploset
			for(int hset=0; hset<indiv->nb_haplo; hset++){
				// For each di-haplotype
				for(int h=0; h<tab_nb_haplo_set[hset]; h++){
					tab_haplo_set[hset].proba[h] /= sum; // Weight the proba
				}
			}
		}

		int nb_haplo_prec = indiv->nb_haplo;

		// All the different haplotypes are collected. The probabilities of identical haplotypes are added
		uniq_haplo_set_indiv(indiv, tab_haplo_set, params, tab_nb_haplo_set, nb_haplo_prec); // A REFAIRE POUR REALLOC CAR SINON MEMOIRE PRISE POUR RIEN

		free_tab_haplo_set(tab_haplo_set, nb_haplo_prec);
		free(tab_nb_haplo_set);
		tab_nb_haplo_set = NULL;
		free(tab_sum_geno);
		tab_sum_geno = NULL;
	}

	// The individual has no possible haplotypes
	if( indiv->nb_haplo == 0 ){
		params->bool_warning = 1;
		FILE *file_log = open_file(params->file_log, "a+");
		fprintf(file_log, "Id: %s\tQTL %d\tStep: S%d\tNo.haplotypes=%d\t#%d#%d\n", indiv->name, q, nb_self, indiv->nb_haplo, indiv->index, q);
		fclose(file_log);
		params->map_ind_qtl_events[string(indiv->name)].push_back(q);
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	s_haplo cross_gam_self_indiv(s_indiv *indiv, s_gametes *gam_set, s_params *params, double *tab_sum_geno, int nb_gam, int num_haplo, int *p_nb_haplo, int bool_self)
	-----------------------------------------------------------------------------
	## RETURN: s_haplo: the set of haplotypes corresponding to the selfing
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : pointer to the individual
	@ s_gametes *gam_set : pointer to the set of gametes
	@ s_params *params : structure containing all program parameters
	@ double *tab_sum_geno : array of the sum of the possible genotypes for the set of haplotypes
	@ int nb_gam : number of gametes in the set
	@ int num_haplo : index of the haplotype which produced the gametes
	@ int *p_nb_haplo : (pointer) number of possible haplotypes regarding the number gametes at the start
	@ int bool_self : [1] if it's the last selfing
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	The set of gametes is crossed with itself (selfing) to create the corresponding
	set of haplotypes. The probabilty of each haplotype is computed. A cut off on
	the haplotypes is done and a filter on the genotypes is applied at the end of
	the last selfing.
	-----------------------------------------------------------------------------
*/

s_haplo cross_gam_self_indiv(s_indiv *indiv, s_gametes *gam_set, s_params *params, double *tab_sum_geno, int nb_gam, int num_haplo, int *p_nb_haplo, int bool_self){

	int cpt_nb_haplo = 0;

	s_haplo haplo_set;

	// Number of possible haplotypes regarding the number of gametes (the mirror effect is taking into account)
	(*p_nb_haplo) = ( nb_gam * ( nb_gam - 1 ) / 2 ) + nb_gam;

	// Memory allocation the combination table of the possible haplotypes
	int **haplo_poss = crea_haplo_poss_autofec((*p_nb_haplo), nb_gam);

	alloc_haplo_set(&haplo_set, (*p_nb_haplo), params);

	// Loop on the possible haplotypes
	for(int h = 0; h < (*p_nb_haplo); h++){

		// The haplotype probability is calculated regarding the gametes (diagonal)
		if( haplo_poss[h][0] == haplo_poss[h][1] ){
			haplo_set.proba[cpt_nb_haplo] = gam_set->proba[haplo_poss[h][0]] * gam_set->proba[haplo_poss[h][1]] * indiv->haplo_set.proba[num_haplo];
		}
		// Mirror effect (*2) for all the combination which are not on the diagonal of the table of crosses
		else {
			haplo_set.proba[cpt_nb_haplo] = 2 * gam_set->proba[haplo_poss[h][0]] * gam_set->proba[haplo_poss[h][1]] * indiv->haplo_set.proba[num_haplo];
		}

		// Only haplotypes with a probability > to the cut off are kept and at the last selfing process these with the good genotype
		//// on entre dans ce test si ( p >= cutoff et pas dans le dernier selfing) ou si ( dans le dernier selfing et genotype compatible )
		if( ( haplo_set.proba[cpt_nb_haplo] >= params->cut_off_haplo && bool_self != 1 ) || ( bool_self == 1 && comp_geno_read_poss(indiv, gam_set, gam_set, haplo_poss[h][0], haplo_poss[h][1], params) ) ){
			// Haplotypes at the last selfing process with the good genotype but < cut off
			tab_sum_geno[num_haplo] += haplo_set.proba[cpt_nb_haplo];
			
			// Within the cut off
			if (haplo_set.proba[cpt_nb_haplo] >= params->cut_off_haplo){
				// The matrix of haplotypes coded and read are filled
				for(int m = 0; m < params->nb_locus; m++){
					haplo_set.read[cpt_nb_haplo][m] = gam_set->read[haplo_poss[h][0]][m];
					haplo_set.read[cpt_nb_haplo][m + params->nb_locus] = gam_set->read[haplo_poss[h][1]][m];
					haplo_set.coded[cpt_nb_haplo][m] = gam_set->coded[haplo_poss[h][0]][m];
					haplo_set.coded[cpt_nb_haplo][m + params->nb_locus] = gam_set->coded[haplo_poss[h][1]][m];

					// If there is a polymorphic site (coded) + hetero residual (read)
					if( (haplo_set.coded[cpt_nb_haplo][m] != haplo_set.coded[cpt_nb_haplo][m + params->nb_locus]) || (haplo_set.read[cpt_nb_haplo][m] != haplo_set.read[cpt_nb_haplo][m + params->nb_locus]) ){
						haplo_set.nb_poly_loci[cpt_nb_haplo]++;
						haplo_set.loca_poly_loci[cpt_nb_haplo][m] = 1;
					}
				}
				cpt_nb_haplo++;
			}
		}
	}
	free_alloc2d(haplo_poss);

	// Memory reallocation after filter (cut off) on the possible haplotypes and the genotypes (at the end)
	if( cpt_nb_haplo == 0 ){
		free_haplo_set(&haplo_set);
	}
	// Realloc not done in purpose
	else if( cpt_nb_haplo != (*p_nb_haplo) ){
		realloc_haplo_set(&haplo_set, cpt_nb_haplo, params);
	}

	(*p_nb_haplo) = cpt_nb_haplo;

  return haplo_set;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void uniq_haplo_set_indiv(s_indiv *indiv, s_haplo *tab_haplo_set, s_params *params, int *tab_nb_haplo_set, int nb_haplo_prec)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : pointer to the current individual
	@ s_haplo *tab_haplo_set : array of sets of haplotypes
	@ s_params *params : structure containing all program parameters
	@ int *tab_nb_haplo_set : array containing the number of haplotypes for each set
	@ int nb_haplo_prec : number of haplotypes that the individual had
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of an unique set of haplotypes beggining with an array of set of
	haplotypes. If haplotypes are the same, their probability is added.
	-----------------------------------------------------------------------------
*/

void uniq_haplo_set_indiv(s_indiv *indiv, s_haplo *tab_haplo_set, s_params *params, int *tab_nb_haplo_set, int nb_haplo_prec){

	int max_nb_haplo = sum_vect_int(tab_nb_haplo_set, nb_haplo_prec);
	int cpt_haplo = 0;

	long allele_nb = params->str2coded.size();
	long dihaplo_size = params->nb_locus*2;

	s_haplo_search_index* haplo_search_indexes = NULL; 
	calloc1d(haplo_search_indexes, max_nb_haplo, s_haplo_search_index);

	// Free memory for the set of haplotypes of the individual
	free_haplo_set(&indiv->haplo_set);
	indiv->nb_haplo = 0;

	// The new set of haplotypes (Max) of the individual is created
	alloc_haplo_set(&indiv->haplo_set, max_nb_haplo, params);

	// Loop on the array of set of haplotypes
	for(int hset=0; hset<nb_haplo_prec; hset++){
		// Loop on the set "i" of haplotypes
		for(int h=0; h<tab_nb_haplo_set[hset]; h++){
			// If a new haplo is present it is added to the individual set, otherwise its probabilities are added
			
			// haplo1 and haplo2 are 1st and 2nd part of the current di-haplotype.
			// The lower must always be first (=> avoid duplicate di-haplotypes by symmetry issue)
			char first = haplo1_gt_haplo2( tab_haplo_set[hset].coded[h], tab_haplo_set[hset].coded[h]+params->nb_locus, params->nb_locus);

			// h1 > h2 (coded)
			if ( first > 0 ){
				haplo_search_indexes[cpt_haplo].geno_b10 = genotype2base10(tab_haplo_set[hset].coded[h], params->nb_locus, allele_nb, true); // swap the haplotypes of this di-haplotypes
				haplo_search_indexes[cpt_haplo].swap = true; // needed in case of residual heterozygous IL
			}
			// h1 < h2 (coded)
			else if( first < 0 ){
				haplo_search_indexes[cpt_haplo].geno_b10 = genotype2base10(tab_haplo_set[hset].coded[h], dihaplo_size, allele_nb); // swap=false by default
				haplo_search_indexes[cpt_haplo].swap = false; // needed in case of residual heterozygous IL
			}
			// h1 == h2 (coded)
			else{
				// h1 > h2 (read)
				if( read1_eq_read2(tab_haplo_set[hset].read[h], tab_haplo_set[hset].read[h]+params->nb_locus, params->nb_locus) > 0 ){
					haplo_search_indexes[cpt_haplo].geno_b10 = genotype2base10(tab_haplo_set[hset].coded[h], params->nb_locus, allele_nb, true); // swap the haplotypes of this di-haplotypes
					haplo_search_indexes[cpt_haplo].swap = true; // needed in case of residual heterozygous IL
				}
				// h1 <= h2 (read)
				else{
					haplo_search_indexes[cpt_haplo].geno_b10 = genotype2base10(tab_haplo_set[hset].coded[h], dihaplo_size, allele_nb); // swap=false by default
					haplo_search_indexes[cpt_haplo].swap = false; // needed in case of residual heterozygous IL
				}
			}

			// Fetch the address of the current di-haplotype proba in haplo_set_max.
			haplo_search_indexes[cpt_haplo].i_haplo = h;
			haplo_search_indexes[cpt_haplo].hset = tab_haplo_set + hset; // Pointer incrementation.
			haplo_search_indexes[cpt_haplo].nb_locus = params->nb_locus;
			cpt_haplo++;
		}
	}
	
	// Sorting haplo_search_indexes based on s_haplo_search_index.geno_b10
	if(params->hetero_res) sort(haplo_search_indexes, haplo_search_indexes + max_nb_haplo, stl_comp_dihaplo_hr);
	else sort(haplo_search_indexes, haplo_search_indexes + max_nb_haplo, stl_comp_dihaplo);
	
	cpt_haplo = 0;

	// Filter on the set of haplotypes. Each haplotype is found only once and its proba >= cut_off
	for(int h=0; h<max_nb_haplo; h++){
		int next_h = h+1;
		long & i_haplo_dest = haplo_search_indexes[h].i_haplo;
		s_haplo & hset_dest = *(haplo_search_indexes[h].hset);
		// Duplicates probabilities are added.
		while ( (next_h < max_nb_haplo) && (haplo_search_indexes[next_h].geno_b10 == haplo_search_indexes[h].geno_b10) ){
			long & i_haplo_src = haplo_search_indexes[next_h].i_haplo;
			s_haplo & hset_src = *(haplo_search_indexes[next_h].hset);

			if(params->hetero_res){
				// Same order
				if(haplo_search_indexes[next_h].swap == haplo_search_indexes[h].swap){
					if(read1_eq_read2(hset_dest.read[i_haplo_dest], hset_src.read[i_haplo_src], params->nb_locus*2) != 0){
						break;
					}
				}
				else{
					if(read1_eq_read2(hset_dest.read[i_haplo_dest], hset_src.read[i_haplo_src] + params->nb_locus, params->nb_locus) != 0
					|| read1_eq_read2(hset_dest.read[i_haplo_dest] + params->nb_locus, hset_src.read[i_haplo_src], params->nb_locus) != 0){
						break;
					}
				}
			}
			hset_dest.proba[i_haplo_dest] += hset_src.proba[i_haplo_src];
			next_h++;
		}
		copy_haplo_in_haplo_set(&hset_dest, &indiv->haplo_set, params, (int)i_haplo_dest, cpt_haplo++);	// cpt_haplo is post-incremented
		h = next_h - 1;
	}
	
	free(haplo_search_indexes);
	haplo_search_indexes = NULL;
	
	indiv->nb_haplo = cpt_haplo;
	// Free memory for the non existent diplotypes (duplicates)
	realloc_haplo_set(&indiv->haplo_set, indiv->nb_haplo, params);
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void free_gam_haplo_indiv(s_indiv *indiv)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : current individual
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Free memory regarding informations/attributes of the current individual.
	-----------------------------------------------------------------------------
*/

void free_gam_haplo_indiv(s_indiv *indiv){
	// Free memory for the set of gametes of the current individual
	free_gam_set(&indiv->gam_set);

	// Free memory for the set of haplotypes of the current individual
	free_haplo_set(&indiv->haplo_set);

	indiv->nb_gametes = 0;
	indiv->nb_haplo = 0;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void free_gam_haplo_tab_indiv(s_indiv *tab_indiv, int nb_indiv)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *tab_indiv : array of individuals
	@ int nb_indiv : number of individuals
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Free memory regarding informations/attributes of each individual.
	-----------------------------------------------------------------------------
*/

void free_gam_haplo_tab_indiv(s_indiv *tab_indiv, int nb_indiv){

	for(int i=0; i<nb_indiv; i++){
		// Free memory for the set of gametes of the current individual
		free_gam_set(&tab_indiv[i].gam_set);

		// Free memory for the set of haplotypes of the current individual
		free_haplo_set(&tab_indiv[i].haplo_set);
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void ril_indiv(s_indiv *indiv, s_mk *markers, s_params *params, int q)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : pointer to the current individual
	@ s_mk *markers : array of markers
	@ s_params *params : structure containing all data parameters
	@ int q : index of the current QTL
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Infinite selfing done to the current individual. (RIL: Recombinant Inbred Lines)
	-----------------------------------------------------------------------------
*/

void ril_indiv(s_indiv *indiv, s_params *params, int q){
	int nb_gam = 0, nb_haplo = 0;
	int *p_nb_gam = &nb_gam; // Pointer to the bumber of gametes
	int *p_nb_haplo = &nb_haplo; // Pointer to the number of haplotypes


	// Creation of the array of set of haplotypes
	s_haplo *tab_haplo_set = NULL;
	malloc1d(tab_haplo_set, indiv->nb_haplo, s_haplo);

	// Creation of the array containing the number of haplotypes for each set
	int *tab_nb_haplo_set = NULL;
	calloc1d(tab_nb_haplo_set, indiv->nb_haplo, int);

	// Creation of the array containing the sum of possible genotypes for each set
	double *tab_sum_geno = NULL;
	calloc1d(tab_sum_geno, indiv->nb_haplo, double);

	// Loop on the haplotypes
	for(int h=0; h<indiv->nb_haplo; h++){
		nb_gam = 0;
		nb_haplo = 0;

		// Creation of gametes from each haplotype
		s_gametes gam_set = haplo2gam_set(&indiv->haplo_set, params, h, p_nb_gam, 2);

		// Creation of the possible haplotypes from these gametes then this set is stocked in the array
		tab_haplo_set[h] = cross_gam_ril_indiv(indiv, &gam_set, params, tab_sum_geno, (*p_nb_gam), h, p_nb_haplo); // MEMORY NOT FREED

		//printf("[%s] --> GAM_SET RIL\tnb_gam=%d - QTL %d\n", indiv->name, (*p_nb_gam), q);
		//print_gam_set(&gam_set, (*p_nb_gam), params->nb_locus);
		//printf("[%s] --> HAPLO_SET RIL\tnb_haplo=%d - QTL %d\n", indiv->name, (*p_nb_haplo), q);
		//print_haplo_set(&tab_haplo_set[h], (*p_nb_haplo), params->nb_locus);

		// The number of haplotypes for each set is stocked
		tab_nb_haplo_set[h] = (*p_nb_haplo);

		free_gam_set(&gam_set);
	}

	// Filter on genotypes (weighting of each probability)
	for(int h=0; h<indiv->nb_haplo; h++){
		for(int i=0; i<tab_nb_haplo_set[h]; i++){
			tab_haplo_set[h].proba[i] = (tab_haplo_set[h].proba[i]) / (sum_vect_double(tab_sum_geno, indiv->nb_haplo));
		}
	}

	int nb_haplo_prec = indiv->nb_haplo;

	// All the different haplotypes are collected. The probabilities of identical haplotypes are added
	uniq_haplo_set_indiv(indiv, tab_haplo_set, params, tab_nb_haplo_set, nb_haplo_prec);

	// The individual has no possible haplotypes
	if( indiv->nb_haplo == 0 ){
		params->bool_warning = 1;
		FILE *file_log = open_file(params->file_log, "a+");
		fprintf(file_log, "Id: %s\tQTL %d\tStep: RIL\tNo.haplotypes=%d #%d#%d\n", indiv->name, q, indiv->nb_haplo, indiv->index, q);
		fclose(file_log);
		params->map_ind_qtl_events[string(indiv->name)].push_back(q);
	}

	free_tab_haplo_set(tab_haplo_set, nb_haplo_prec);
	free(tab_nb_haplo_set);
	tab_nb_haplo_set = NULL;
	free(tab_sum_geno);
	tab_sum_geno = NULL;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	s_haplo cross_gam_ril_indiv(s_indiv *indiv, s_gametes *gam_set, s_params *params, double *tab_sum_geno, int nb_gam, int num_haplo, int *p_nb_haplo)
	-----------------------------------------------------------------------------
	## RETURN: s_haplo: the set of haplotypes corresponding to the selfing
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : pointer to the individual
	@ s_gametes *gam_set : pointer to the set of gametes
	@ s_params *params : structure containing all program parameters
	@ double *tab_sum_geno : array of the sum of the possible genotypes for the set of haplotypes
	@ int nb_gam : number of gametes in the set
	@ int num_haplo : index of the haplotype which produced the gametes
	@ int *p_nb_haplo : (pointer) number of possible haplotypes regarding the number gametes at the start
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	The set of gametes for RILs is crossed with itself (selfing) to create the
	corresponding set of haplotypes (only homozygous). The probabilty of each
	haplotype is computed. A filter on the genotypes is applied.
	-----------------------------------------------------------------------------
*/

s_haplo cross_gam_ril_indiv(s_indiv *indiv, s_gametes *gam_set, s_params *params, double *tab_sum_geno, int nb_gam, int num_haplo, int *p_nb_haplo){

	int cpt_nb_haplo = 0;

	s_haplo haplo_set;

	// Number of possible haplotypes possibles regarding the number of gametes (the mirror effect is taking into account)
	(*p_nb_haplo) = nb_gam;

	// Memory allocation the combination table of the possible haplotypes
	int **haplo_poss = crea_haplo_poss_ril(nb_gam);

	alloc_haplo_set(&haplo_set, (*p_nb_haplo), params);

	// Loop on the possible haplotypes
	for(int h = 0; h < (*p_nb_haplo); h++){
		// The haplotype probability is calculated regarding the gametes (diagonal)
		//haplo_set.proba[cpt_nb_haplo] = gam_set->proba[haplo_poss[h][0]] * gam_set->proba[haplo_poss[h][1]] * indiv->haplo_set.proba[num_haplo]; // Error?
		haplo_set.proba[cpt_nb_haplo] = gam_set->proba[haplo_poss[h][0]] * indiv->haplo_set.proba[num_haplo];

		// Only haplotypes with a probability > to the cut off are kept and at the last selfing process these with the good genotype
		if( comp_geno_read_poss(indiv, gam_set, gam_set, haplo_poss[h][0], haplo_poss[h][1], params) ){
			// Haplotypes with the good genotype
			tab_sum_geno[num_haplo] = tab_sum_geno[num_haplo] + haplo_set.proba[cpt_nb_haplo];

			// The matrix of haplotypes coded and read are filled
			for(int m = 0; m < params->nb_locus; m++){
				haplo_set.read[cpt_nb_haplo][m] = gam_set->read[haplo_poss[h][0]][m];
				haplo_set.read[cpt_nb_haplo][m + params->nb_locus] = gam_set->read[haplo_poss[h][1]][m];
				haplo_set.coded[cpt_nb_haplo][m] = gam_set->coded[haplo_poss[h][0]][m];
				haplo_set.coded[cpt_nb_haplo][m + params->nb_locus] = gam_set->coded[haplo_poss[h][1]][m];

				// If there is a polymorphic site (coded) + hetero residual (read)
				if( (haplo_set.coded[cpt_nb_haplo][m] != haplo_set.coded[cpt_nb_haplo][m + params->nb_locus]) || (haplo_set.read[cpt_nb_haplo][m] != haplo_set.read[cpt_nb_haplo][m + params->nb_locus]) ){
					haplo_set.nb_poly_loci[cpt_nb_haplo]++;
					haplo_set.loca_poly_loci[cpt_nb_haplo][m] = 1;
				}
			}
			cpt_nb_haplo++;
		}
	}
	free_alloc2d(haplo_poss);

	// Memory reallocation after filter (cut off) on the possible haplotypes and the genotypes (at the end)
	if( cpt_nb_haplo == 0 ){
		free_haplo_set(&haplo_set);
	}
	else if( cpt_nb_haplo != (*p_nb_haplo) ){
		realloc_haplo_set(&haplo_set, cpt_nb_haplo, params);
	}

	(*p_nb_haplo) = cpt_nb_haplo;

  return haplo_set;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void hd_indiv(s_indiv *indiv, s_gametes *gam_set, int nb_gam, s_params *params, int q)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : pointer to the current individual
	@ s_gametes *gam_set : set of gametes of the individual's parent
	@ int nb_gam : number of gametes in the set
	@ s_params *params : structure containing all data parameters
	@ int q : index of the current QTL
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of a doubled haploid (DH). Genotype with two sets of identical chromosomes.
	-----------------------------------------------------------------------------
*/

void hd_indiv(s_indiv *indiv, s_gametes *gam_set, int nb_gam, s_params *params, int q){

	int cpt_nb_haplo = 0;
	double sum_geno = 0.0;

	// Memory allocation of the combination table of the possible haplotypes (nb_haplo_poss = nb_gam)
	int **haplo_poss = crea_haplo_poss_ril(nb_gam); // Diagonal

	alloc_haplo_set(&indiv->haplo_set, nb_gam, params);
	indiv->nb_haplo = nb_gam;

	// Loop on the possible haplotypes
	for(int h = 0; h < nb_gam; h++){
		// The haplotype probability is calculated regarding the gametes (diagonal)
		indiv->haplo_set.proba[cpt_nb_haplo] = gam_set->proba[h];

		// Only haplotypes with a probability > to the cut off are kept and at the last selfing process these with the good genotype
		if( comp_geno_read_poss(indiv, gam_set, gam_set, haplo_poss[h][0], haplo_poss[h][1], params) ){
			// Haplotypes with the good genotype
			sum_geno += indiv->haplo_set.proba[cpt_nb_haplo];

			// The matrix of haplotypes coded and read are filled
			for(int m = 0; m < params->nb_locus; m++){
				indiv->haplo_set.read[cpt_nb_haplo][m] = gam_set->read[haplo_poss[h][0]][m];
				indiv->haplo_set.read[cpt_nb_haplo][m + params->nb_locus] = gam_set->read[haplo_poss[h][1]][m];
				indiv->haplo_set.coded[cpt_nb_haplo][m] = gam_set->coded[haplo_poss[h][0]][m];
				indiv->haplo_set.coded[cpt_nb_haplo][m + params->nb_locus] = gam_set->coded[haplo_poss[h][1]][m];

				// If there is a polymorphic site (coded) + hetero residual (read)
				if( (indiv->haplo_set.coded[cpt_nb_haplo][m] != indiv->haplo_set.coded[cpt_nb_haplo][m + params->nb_locus]) || (indiv->haplo_set.read[cpt_nb_haplo][m] != indiv->haplo_set.read[cpt_nb_haplo][m + params->nb_locus]) ){
					indiv->haplo_set.nb_poly_loci[cpt_nb_haplo]++;
					indiv->haplo_set.loca_poly_loci[cpt_nb_haplo][m] = 1;
				}
			}
			cpt_nb_haplo++;
		}
	}
	free_alloc2d(haplo_poss);

	indiv->nb_haplo = cpt_nb_haplo;

	// Memory reallocation after filter on the possible genotypes
	if( cpt_nb_haplo == 0 ){
		free_haplo_set(&indiv->haplo_set);
	}
	else if( cpt_nb_haplo != nb_gam ){
		realloc_haplo_set(&indiv->haplo_set, cpt_nb_haplo, params);
	}

	// Filter on the genotype (weighting of each probability)
	for(int h=0; h<indiv->nb_haplo; h++){
		indiv->haplo_set.proba[h] = indiv->haplo_set.proba[h] / sum_geno;
	}

	// The individual has no possible haplotypes
	if(indiv->nb_haplo == 0){
		params->bool_warning = 1;
		FILE *file_log = open_file(params->file_log, "a+");
		fprintf(file_log, "Id: %s\tQTL %d\tStep: DH\tNo.haplotypes=%d #%d#%d\n", indiv->name, q, indiv->nb_haplo, indiv->index, q);
		fclose(file_log);
		params->map_ind_qtl_events[string(indiv->name)].push_back(q);
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void write_haplo_gam_output_indiv(FILE *file_haplo, FILE *file_gam, s_indiv *tab_indiv, s_indiv *indiv, s_params *params, int num_qtl)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ FILE *file_haplo : file where will be written the haplotype set results
	@ FILE *file_gam : file where will be written the gamete set results
	@ s_indiv *tab_indiv : array of individuals
	@ s_indiv *indiv : current individual
	@ s_params *params : structure containing all data parameters
	@ int num_qtl : Number of the current qtl/chr
	-----------------------------------------------------------------------------
	## SPECIFICATION: The diplotypes/gametes sets are written. Probability to
	obtain a phased genotypes and gametes for each QTL for all the individuals.
	-----------------------------------------------------------------------------
*/

void write_haplo_gam_output_indiv(FILE *file_haplo, FILE *file_gam, s_indiv *indiv, s_params *params, int num_qtl){

	// Haplotypes are written
	for(int h = 0; h < indiv->nb_haplo; h++){
		fprintf(file_haplo, "%d\t%s\t", num_qtl, indiv->name);

		// Coded
		for(int m = 0; m < (2*params->nb_locus); m++){
			//const char* parental_allele = tab_indiv[ params->coded2str[indiv->haplo_set.coded[h][m]] ].parental_allele;
			const char* parental_allele = params->coded2str[indiv->haplo_set.coded[h][m]];

			if( m == (params->nb_locus - 1) ){
				fprintf(file_haplo, "%s\t", parental_allele);
			}
			else if( m == ( (2*params->nb_locus) - 1) ){
				fprintf(file_haplo, "%s\t", parental_allele);
			}
			else{
				fprintf(file_haplo, "%s.", parental_allele);
			}
		}
		// Read
		for(int m = 0; m < (2*params->nb_locus); m++){
			const char* str_allele = params->read2str[ indiv->haplo_set.read[h][m] ];
			if( m == (params->nb_locus - 1) ){
				fprintf(file_haplo, "%s\t", str_allele);
			}
			else if( m == ( (2*params->nb_locus) - 1) ){
				fprintf(file_haplo, "%s", str_allele);
			}
			else{
				fprintf(file_haplo, "%s.", str_allele);
			}
		}
		fprintf(file_haplo, "\t%f\t%d\n", indiv->haplo_set.proba[h], indiv->nb_haplo);
	}

	// Gametes are written
	for(int g = 0; g < indiv->nb_gametes; g++){
		fprintf(file_gam, "%d\t%s\t", num_qtl, indiv->name);

		// Coded
		for(int m = 0; m < (params->nb_locus); m++){
			//const char* parental_allele = tab_indiv[ params->coded2str[indiv->gam_set.coded[g][m]] ].parental_allele;
			const char* parental_allele = params->coded2str[indiv->gam_set.coded[g][m]];

			if( m == (params->nb_locus -1) ){
				fprintf(file_gam, "%s", parental_allele);
			}
			else{
				fprintf(file_gam, "%s.", parental_allele);
			}
		}
		fprintf(file_gam, "\t");
		// Read
		for(int m = 0; m < (params->nb_locus); m++){
			const char* str_allele = params->read2str[ indiv->gam_set.read[g][m] ];
			if( m == (params->nb_locus -1) ){
				fprintf(file_gam, "%s", str_allele);
			}
			else{
				fprintf(file_gam, "%s.", str_allele);
			}
		}
		fprintf(file_gam, "\t%f\t%d\n", indiv->gam_set.proba[g], indiv->nb_gametes);
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void calcul_scores_indiv(s_indiv *indiv, s_mk *markers, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *indiv : current individual
	@ s_mk *markers : array of markers
	@ s_params *params : structure containing all data parameters
	@ int q : index of the current qtl (0 --> all, 1 --> QTL1)
	-----------------------------------------------------------------------------
	## SPECIFICATION: Probability of presence of favorables alleles at the QTL for
	each individual (score). Sum of all the phased genotypes weighted by the dose
	of favorable alleles weighted by the probability of each phased genotype.
	-----------------------------------------------------------------------------
*/

void calcul_scores_indiv(s_indiv *indiv, s_mk *markers, s_params *params, int q){

	int ind = indiv->index; // index of the individual in tab_indiv

	// Loop on the haplotypes
	for(int h = 0; h < indiv->nb_haplo; h++){
		int nb_all_fav = 0;
		double half_proba_h = indiv->haplo_set.proba[h] / 2.0;

		// Loop on all the markers
		for(int m = 0; m < params->nb_locus; m++){
			// We are at a QTL position
			if( params->tab_index_qtl[params->num_mk_inf + m] == 1 ){

				const char* h1_qtl_allele = params->coded2str[indiv->haplo_set.coded[h][m]];
				const char* h2_qtl_allele = params->coded2str[indiv->haplo_set.coded[h][m+params->nb_locus]];
				//const char* h2_mk = tab_indiv[ params->coded2str[ indiv->haplo_set.coded[h][m+params->nb_locus] ] ].parental_allele;

				int index_parental_allele1 = params->str2coded[h1_qtl_allele];
				int index_parental_allele2 = params->str2coded[h2_qtl_allele];

				// If the allele of the haplotype 1 is favorable
				if(is_fav(h1_qtl_allele, markers, m + params->num_mk_inf)){
					nb_all_fav++;
				}
				// If the allele of the haplotype 2 is favorable
				if(is_fav(h2_qtl_allele, markers, m + params->num_mk_inf)){
					nb_all_fav++;
				}

				params->tab_founders_final[ind][0][index_parental_allele1] += half_proba_h; // All qtl
				params->tab_founders_final[ind][0][index_parental_allele2] += half_proba_h; // All qtl

				params->tab_founders_final[ind][q][index_parental_allele1] += half_proba_h; // Each qtl
				params->tab_founders_final[ind][q][index_parental_allele2] += half_proba_h; // Each qtl
			}
		}

		// Probability of homozygous favorable
		if( nb_all_fav == 2 ){
			params->tab_homo_hetero_final[ind][0][0] += indiv->haplo_set.proba[h]; // All qtl
			params->tab_homo_hetero_final[ind][q][0] += indiv->haplo_set.proba[h]; // each qtl
		}
		// Probability of not favorable homozygous
		else if( nb_all_fav == 0 ){
			params->tab_homo_hetero_final[ind][0][1] += indiv->haplo_set.proba[h]; // All qtl
			params->tab_homo_hetero_final[ind][q][1] += indiv->haplo_set.proba[h]; // each qtl
		}
		// Probability for heterozygous
		else{
			params->tab_homo_hetero_final[ind][0][2] += indiv->haplo_set.proba[h]; // All qtl
			params->tab_homo_hetero_final[ind][q][2] += indiv->haplo_set.proba[h]; // each qtl
		}

		// Table of scores: probability of presence of favorable alleles for each individual
		////params->tab_scores_final[ind][0] += ((nb_all_fav * tab_indiv[ind].haplo_set.proba[h])/(2.0)); // All qtl
		params->tab_scores_final[ind][0] += (nb_all_fav * half_proba_h); // All qtl
		params->tab_scores_final[ind][q] += (nb_all_fav * half_proba_h); // each qtl
	}

	// Table of the sum of probabilities of the checked phased genotypes (cut-off)
	double sum = sum_vect_double(indiv->haplo_set.proba, indiv->nb_haplo);

	params->tab_check_diplo[ind][0] += sum;
	params->tab_check_diplo[ind][q] += sum;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void write_homo_hetero_output_indiv(FILE *fi, s_indiv *indiv, s_mk *markers, s_params *params, int num_qtl)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ FILE *fi : output file for the results (homo/hetero)
	@ s_indiv *indiv : current individual
	@ s_mk *markers : array of all markers
	@ s_params *params : structure containing all data parameters
	@ int num_qtl: index of the current QTL
	-----------------------------------------------------------------------------
	## SPECIFICATION: The probability to be homozygous favorable/unfavorable and
	heterozygous for all individuals at each QTL is written.
	-----------------------------------------------------------------------------
*/

void write_homo_hetero_output_indiv(FILE *fi, s_indiv *indiv, s_mk *markers, s_params *params, int num_qtl){

	map <string, double> map_geno_homo_fav, map_geno_hetero, map_geno_homo_unfav;
	map <string, double>::iterator it1, it2;
	string geno1 = "", geno2 = "";

	map_geno_homo_fav.clear();
	map_geno_hetero.clear();
	map_geno_homo_unfav.clear();

	// Loop on the haplotypes
	for(int h = 0; h < indiv->nb_haplo; h++){
		// Loop on all the markers
		for(int m = 0; m < params->nb_locus; m++){
			// We are at a QTL position
			if( params->tab_index_qtl[params->num_mk_inf + m] == 1 ){

				const char* h1_qtl_allele = params->coded2str[indiv->haplo_set.coded[h][m]];
				const char* h2_qtl_allele = params->coded2str[indiv->haplo_set.coded[h][m+params->nb_locus]];

				geno1 = string(h1_qtl_allele) + ":" + string(h2_qtl_allele);
				geno2 = string(h2_qtl_allele) + ":" + string(h1_qtl_allele); // f:x == x:f

				// Homozygous favourable
				if( is_fav(h1_qtl_allele, markers, m + params->num_mk_inf) && is_fav(h2_qtl_allele, markers, m + params->num_mk_inf) ){

					it1 = map_geno_homo_fav.find(geno1);
					it2 = map_geno_homo_fav.find(geno2);

					// The genotype already exists so we add the probabilities
					if( it1 != map_geno_homo_fav.end() ){
						////map_geno_homo_fav[geno1] += tab_indiv[ind].haplo_set.proba[h];
						it1->second += indiv->haplo_set.proba[h];
					}
					else if( it2 != map_geno_homo_fav.end() ){
						////map_geno_homo_fav[geno2] += tab_indiv[ind].haplo_set.proba[h];
						it2->second += indiv->haplo_set.proba[h];
					}
					// 1st time that we found the current genotype
					else{  //// test forcement vrai
						map_geno_homo_fav[geno1] += indiv->haplo_set.proba[h];
					}
				}
				// Homozygous unfavourable
				else if( (is_fav(h1_qtl_allele, markers, m + params->num_mk_inf) == 0) && (is_fav(h2_qtl_allele, markers, m + params->num_mk_inf) == 0) ){

					// The genotype already exists so we add the probabilities
					if( (it1 = map_geno_homo_unfav.find(geno1)) != map_geno_homo_unfav.end() ){
						it1->second += indiv->haplo_set.proba[h];
					}
					else if( (it2 = map_geno_homo_unfav.find(geno2)) != map_geno_homo_unfav.end() ){
						it2->second += indiv->haplo_set.proba[h];
					}
					// 1st time that we found the current genotype
					else { //// it1 and it2 == end()
						map_geno_homo_unfav[geno1] += indiv->haplo_set.proba[h];
					}
				}
				// Heterozygous
				else{
					// The genotype already exists so we add the probabilities
					if( (it1 = map_geno_hetero.find(geno1)) != map_geno_hetero.end() ){
						it1->second += indiv->haplo_set.proba[h];
					}
					else if( (it2 = map_geno_hetero.find(geno2)) != map_geno_hetero.end() ){
						it2->second += indiv->haplo_set.proba[h];
					}
					// 1st time that we found the current genotype
					else{ //// it1 and it2 == end()
						map_geno_hetero[geno1] += indiv->haplo_set.proba[h];
					}
				}
			}
		}
	}

	int ind = indiv->index;

	fprintf(fi, "%s\n", indiv->name);

	fprintf(fi, "Homo(+/+)=%f", params->tab_homo_hetero_final[ind][num_qtl][0]);

	map<string, double>::iterator iter_mean;
	for( iter_mean = map_geno_homo_fav.begin(); iter_mean != map_geno_homo_fav.end(); ++iter_mean ){
		string key = (*iter_mean).first;
		fprintf(fi, "\t%s=%f", key.c_str(), map_geno_homo_fav[key]);
	}
	fprintf(fi, "\n");

	fprintf(fi, "Hetero(+/-)=%f", params->tab_homo_hetero_final[ind][num_qtl][2]);

	for( iter_mean = map_geno_hetero.begin(); iter_mean != map_geno_hetero.end(); ++iter_mean ){
		string key = (*iter_mean).first;
		fprintf(fi, "\t%s=%f", key.c_str(), map_geno_hetero[key]);
	}
	fprintf(fi, "\n");

	fprintf(fi, "Homo(-/-)=%f", params->tab_homo_hetero_final[ind][num_qtl][1]);

	for( iter_mean = map_geno_homo_unfav.begin(); iter_mean != map_geno_homo_unfav.end(); ++iter_mean ){
		string key = (*iter_mean).first;
		fprintf(fi, "\t%s=%f", key.c_str(), map_geno_homo_unfav[key]);
	}
	fprintf(fi, "\n");
}
