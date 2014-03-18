#include "../headers/markers.h"

using namespace std;


/**-----------------------------------------------------------------------------
	## FUNCTION:
	s_mk *init_tab_mks(s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	@ s_mk *markers : array of markers (QTL included)
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_params *params : structure containing all data parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Initializing an array of markers (parsing data).
	-----------------------------------------------------------------------------
*/

s_mk *init_tab_mks(s_params *params){

	s_mk *markers = NULL;

	int cpt_lines = 0, nb_columns_map = 5;
	char *temp = (char *)malloc(NB_LINE_MEAN*sizeof(char));
	int last_chr = 0, last_qtl = 0;
	double last_pos = 0.0;

	// Open the marker file with the reading mode
	FILE *file = open_file(params->file_mks, "r") ;

	// We read the input file (markers file) line by line
	while(fgets(temp, NB_LINE_MEAN, file) && (strcmp(temp, "\n") != 0) && (strcmp(temp, "\r\n") != 0) ){

		search(temp, '\n'); // New line at the end of the string is removed
		search(temp, '\r');

		//We split each column of each line in an array
		char **line = str_split(temp, "\t");

		// The number of columns for each line is checked
		if( count(line) != nb_columns_map && count(line) != nb_columns_map - 1 ){
			fprintf(stderr, "Error: number of information l.%d of the map file\n", cpt_lines + 1);
			fflush(stderr);
			exit(1);
		}

		// The first line is checked [geno/chr/qtl/pos/all+]
		if( cpt_lines == 0 ){
			if( (strncmp(line[0], "Locus", 5) != 0) || (strncmp(line[1], "Chr", 3) != 0) || (strncmp(line[2], "QTL", 3) != 0) || (strncmp(line[3], "Pos", 3) != 0) || (strncmp(line[4], "All+", 4) != 0) ){
				fprintf(stderr, "Error: the header of the map file is not conform!\n");
				fflush(stderr);
				exit(1);
			}
		}
		else{
			// Memory allocation for each marker
			realloc1d(markers, params->nb_mks_qtl + 1, s_mk);

			// We take information for each marker
			strcpy(markers[params->nb_mks_qtl].name, line[0]); // Marker name
			markers[params->nb_mks_qtl].chr = atoi(line[1]); // Index of the chromosome
			markers[params->nb_mks_qtl].num_qtl = atoi(line[2]); // Index of the QTL
			markers[params->nb_mks_qtl].pos = atof(line[3]); // Position of the marker/QTL

			/*
			if( markers[params->nb_mks_qtl].chr < last_chr ){
				fprintf(stderr, "Error: line %d of the map file:\nChromosomes indexes have to be in ascendant order\n", cpt_lines + 1);
				fflush(stderr); exit(1);
			}
			*/
			if( markers[params->nb_mks_qtl].num_qtl < last_qtl ){
				fprintf(stderr, "Error: line %d of the map file:\nQTLs indexes have to be in ascendant order\n", cpt_lines + 1);
				fflush(stderr); exit(1);
			}
			if( markers[params->nb_mks_qtl].num_qtl == last_qtl && markers[params->nb_mks_qtl].pos < last_pos ){
				fprintf(stderr, "Error: line %d of the map file:\nThe QTL position and its Flanking Markers positions, have to be in ascendant order\n", cpt_lines + 1);
				fflush(stderr); exit(1);
			}
			last_chr = markers[params->nb_mks_qtl].chr;
			last_qtl = markers[params->nb_mks_qtl].num_qtl;
			last_pos = markers[params->nb_mks_qtl].pos;

			// Memory allocation for each marker
			realloc1d(params->tab_index_qtl, params->nb_mks_qtl + 1, int);

			// QTL position: keep its index
			if( strncmp(markers[params->nb_mks_qtl].name, "qtl", 3) == 0 ){

				// The number of columns for each QTL line is checked
				if( count(line) != nb_columns_map ){
					fprintf(stderr, "Error: number of information for the QTL %s l.%d of the map file\n", line[0], cpt_lines + 1);
					fflush(stderr);
					exit(1);
				}

				// We split each favorable allele
				char **line2 = str_split(line[4], "/");
				markers[params->nb_mks_qtl].nb_all_fav = count(line2);

				// Memory allocation for the array of favorable alleles for each marker/QTL
				markers[params->nb_mks_qtl].all_fav = NULL;
				malloc2d(markers[params->nb_mks_qtl].all_fav, markers[params->nb_mks_qtl].nb_all_fav, params->geno_loci_max, char);

				// Loop for each favorable allele
				for(int j=0; j<markers[params->nb_mks_qtl].nb_all_fav; j++){
					// The favorable alleles names are copied
					if( (int)strlen(line2[j]) >= params->geno_loci_max ){
						fprintf(stderr, "Error l.%d of the map file: the favorable allele name must be only one character (i.e a, b, c...) !\n", cpt_lines + 1);
						fflush(stderr);
						exit(1);
					}
					strcpy(markers[params->nb_mks_qtl].all_fav[j], line2[j]);
				}
				free(line2);

				params->nb_qtl++;
				params->tab_index_qtl[params->nb_mks_qtl] = 1;
			}
			// Marker position
			else{
				markers[params->nb_mks_qtl].nb_all_fav = 0;
				markers[params->nb_mks_qtl].all_fav = NULL;
				params->nb_mks++;
				params->tab_index_qtl[params->nb_mks_qtl] = 0;
			}
			params->nb_mks_qtl++;
		}
		free(line);
		cpt_lines++;
	}

	// Check if the QTL selected is not out of range
	if( (params->qtl_selected != 0) && (params->qtl_selected > params->nb_qtl) ){
		fprintf(stderr, "Error: the QTL selected for the analysis is out of range regarding the map file !\n");
		fflush(stderr);
		exit(1);
	}

	if( fclose(file) == EOF ){
		fprintf(stderr, "Error closing file %s !\n", params->file_mks);
		fflush(stderr);
		exit(1);
	}
	else {
		//printf("The file %s was read correctly...\n",params->file_mks); // RECAP NB MKS / QTL IN THE INTERFACE
	}
	free(temp);

	return markers;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	free_markers(s_mk *markers, int nb_mks)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_mk *markers : array of markers (QTL included)
	@ int nb_mks_qtl : total number of markers (QTL included)
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Freeing memory for the names of favorable alleles for each marker.
	-----------------------------------------------------------------------------
*/

void free_markers(s_mk *markers, int nb_mks_qtl){

	for(int i=0; i<nb_mks_qtl; i++){
		free_alloc2d(markers[i].all_fav);
	}
	free(markers);
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void print_tab_mks(s_mk *markers, int nb_mks)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_mk *markers : array of markers (QTL included)
	@ int nb_mks_qtl : total number of markers (QTL included)
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Print table markers.
	-----------------------------------------------------------------------------
*/

void print_tab_mks(s_mk *markers, int nb_mks_qtl){
	int i, j;

	printf("AFFICHAGE DU TABLEAU DE MARQUEURS:\n");

	for(i=0; i<nb_mks_qtl; i++){
		printf("%s | K: %d | n: %d | pos: %1.1f | nb_a_f: %d | all_fav: ", markers[i].name, markers[i].chr, markers[i].num_qtl, markers[i].pos, markers[i].nb_all_fav);
		for(j=0; j<markers[i].nb_all_fav; j++){
			printf("%s ", markers[i].all_fav[j]);
		}
		printf("|\n");
	}
	printf("\n");
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void find_index(s_mk *markers, s_params *params, int q)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_mk *markers : table of markers
	@ s_params *params : structure containing all data parameters
	@ int q : current QTL
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Assignment of the lower and upper marker flanking the current QTL.
	-----------------------------------------------------------------------------
*/

void find_index(s_mk *markers, s_params *params, int q){

	/* We find the index for the lower and upper markers */
	for(int m=0; m<(params->nb_mks_qtl); m++){
		/* We check if the marker QTL index is the same as the current QTL */
		if( markers[m].num_qtl == q ){
			/* We find the lower marker corresponding to that QTL */
			if( params->nb_locus == 0 ){
				params->num_mk_inf = m;
			}
			params->nb_locus++;
		}
	}
	/* Index of the upper marker at the current QTL position */
	params->num_mk_sup = params->nb_locus + params->num_mk_inf - 1;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void compute_rec_frac(s_mk *markers, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_mk *markers : table of markers
	@ s_params *params : structure containing all data parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Compute the recombination fraction r (and R) for each possible interval
	between num_mk_inf and num_mk_sup.
	Store the results in vect_rec_frac (attribute of params).
	-----------------------------------------------------------------------------
*/

void compute_rec_frac(s_mk* markers, s_params* params){
	
	double delta_pos;
	
	s_rec_frac rf;
	params->vect_rec_frac.clear();
	params->vect_rec_frac.resize( params->nb_locus-1, vector<s_rec_frac>(params->nb_locus) );
	
	// j > i
	for(int i=0; i<params->nb_locus; i++){
		for(int j=i+1; j<params->nb_locus; j++){
			
			delta_pos = markers[params->num_mk_inf + j].pos - markers[params->num_mk_inf + i].pos;

			rf.r = 0.5 * (1.0 - exp(- 2.0 * delta_pos / 100));	// r
			rf.R = (2.0 * rf.r) / (1.0 + (2.0 * rf.r));			// R

			params->vect_rec_frac[i][j] = rf;
		}
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int is_fav(char *allele, s_mk *markers, int index_mk)
	-----------------------------------------------------------------------------
	## RETURN:
	[1] if the tested allele is favorable, [0] otherwise.
	-----------------------------------------------------------------------------
	## PARAMETRES:
	@ char *allele : allele searched
	@ s_mk *markers : array of markers
	@ int index_mk : index of the current marker
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Evaluates the presence or absence of a favorable allele at a QTL.
	-----------------------------------------------------------------------------
*/

int is_fav(const char *allele, s_mk *markers, int index_mk){

	// We read the table of string containing the list of favorable alleles
	for(int i=0; i<markers[index_mk].nb_all_fav; i++){
		if( strcmp(markers[index_mk].all_fav[i], allele) == 0 ){
			return 1;
		}
	}
	return 0; // The allele was not found in the list
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void check_fav_alleles_input_files(s_indiv *tab_indiv, s_mk *markers, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *tab_indiv : array of individuals
	@ s_mk *markers : array of all markers
	@ s_params *params : structure containing all data parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION: check the correspondence of the favorable alleles present
	in the 2 input files after table initialization.
	-----------------------------------------------------------------------------
*/

void check_fav_alleles_input_files(s_mk *markers, s_params *params){

	int qtl = 1;

	// Loop on all the markers
	for(int m = 0; m < params->nb_mks_qtl; m++){
		// We are at a QTL position
		if( params->tab_index_qtl[m] == 1 ){
			// We read the table of string containing the list of favorable alleles
			for(int i=0; i<markers[m].nb_all_fav; i++){
				if( params->str2coded.find(markers[m].all_fav[i]) == params->str2coded.end() ){
					fprintf(stderr, "Error: favorable alleles at QTL %d mentioned in the .map file don't match the parental alleles of the genotypes/pedigre file (.dat)!\n", qtl);
					exit(1);
				}
			}
			qtl++;
		}
	}
	// The different favorable alleles correspond in both files
}
