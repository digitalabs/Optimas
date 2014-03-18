#include "../headers/haplotypes.h"

/**-----------------------------------------------------------------------------
	## FUNCTION:
	bool stl_comp_dihaplo(const s_haplo_search_index & i1, const s_haplo_search_index & i2)
	-----------------------------------------------------------------------------
	## RETURN:
	bool : true if i1.index < i2.index, false otherwise.
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ const s_haplo_search_index & i1 : a const reference to the 1st s_haplo_search_index
	@ const s_haplo_search_index & i2 : a const reference to the 2nd s_haplo_search_index
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	function used by the STL sort function to compare 2 s_haplo_search_index values.
	-----------------------------------------------------------------------------
*/

bool stl_comp_dihaplo(const s_haplo_search_index & dh1, const s_haplo_search_index & dh2){
	return dh1.geno_b10 < dh2.geno_b10;
}

// idem, called in case of residual heterozygous in IL.
bool stl_comp_dihaplo_hr(const s_haplo_search_index & dh1, const s_haplo_search_index & dh2){
	
	if(dh1.geno_b10 == dh2.geno_b10){ // coded are equal but read may not (if parents are residual heterozygous)
		char first;
		// No swap for dh1 & dh2
		if( (dh1.swap == false) && (dh2.swap == false) ){
			return read1_eq_read2(dh1.hset->read[dh1.i_haplo], dh2.hset->read[dh2.i_haplo], dh1.nb_locus*2) < 0; //sort fct return true or false
		}
		// Swap for dh1 & dh2
		else if( (dh1.swap == true) && (dh2.swap == true) ){
			if( (first = read1_eq_read2(dh1.hset->read[dh1.i_haplo] + dh1.nb_locus, dh2.hset->read[dh2.i_haplo] + dh2.nb_locus, dh1.nb_locus)) == 0){
				return read1_eq_read2(dh1.hset->read[dh1.i_haplo], dh2.hset->read[dh2.i_haplo], dh1.nb_locus) < 0;
			}
			else{
				return first < 0;
			}
		}
		// Swap for dh1 & no swap for dh2
		else if(dh1.swap){
			if( (first = read1_eq_read2(dh1.hset->read[dh1.i_haplo] + dh1.nb_locus, dh2.hset->read[dh2.i_haplo], dh1.nb_locus)) == 0){
				return read1_eq_read2(dh1.hset->read[dh1.i_haplo], dh2.hset->read[dh2.i_haplo] + dh2.nb_locus, dh1.nb_locus) < 0;
			}
			else{
				return first < 0;
			}
		}
		// No swap for dh1 & swap for dh2
		else{ // dh2.swap == true
			if( (first = read1_eq_read2(dh1.hset->read[dh1.i_haplo], dh2.hset->read[dh2.i_haplo] + dh2.nb_locus, dh1.nb_locus)) == 0){
				return read1_eq_read2(dh1.hset->read[dh1.i_haplo] + dh1.nb_locus, dh2.hset->read[dh2.i_haplo], dh1.nb_locus) < 0;
			}
			else{
				return first < 0;
			}
		}
	}
	return dh1.geno_b10 < dh2.geno_b10;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int **crea_haplo_poss(int nb_haplo_poss, int nb_gam_p1, int nb_gam_p2)
	-----------------------------------------------------------------------------
	## RETURN:
	@ int **haplo_poss : combination matrix for possible haplotypes
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int nb_haplo_poss : number of possible haplotypes (calculated using gametes)
	@ int nb_gam_p1 : gamete number of the parent 1
	@ int nb_gam_p2 : gamete number of the parent 2
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of the matrix of possible haplotypes according to gametes.
	-----------------------------------------------------------------------------
*/

int **crea_haplo_poss(int nb_haplo_poss, int nb_gam_p1, int nb_gam_p2){
	int h = 0;

	int **haplo_poss = NULL;
	calloc2d(haplo_poss, nb_haplo_poss, 2, int);

	/* Creation of the combination matrix of possible haplotypes */
	for(int i = 0; i < nb_gam_p1; i++){
		for(int j = 0; j < nb_gam_p2; j++){
			haplo_poss[h][0] = i;
			haplo_poss[h][1] = j;
			h++;
		}
	}
	return haplo_poss;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int **crea_haplo_poss_autofec(int nb_haplo_poss, int nb_gam)
	-----------------------------------------------------------------------------
	## RETURN:
	@ int **haplo_poss : combination matrix for possible haplotypes (selfing)
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int nb_haplo_poss : number of possible haplotypes (calculated using gametes)
	@ int nb_gam : number of gametes
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of the matrix of possible haplotypes (selfing) according to gametes.
	-----------------------------------------------------------------------------
*/

int **crea_haplo_poss_autofec(int nb_haplo_poss, int nb_gam){
	int h = 0;

	int **haplo_poss = NULL;
	calloc2d(haplo_poss, nb_haplo_poss, 2, int);

	// Creation of the combination matrix of possible haplotypes
	for(int i = 0; i < nb_gam; i++){
		for(int j = i; j < nb_gam; j++){
			haplo_poss[h][0] = i;
			haplo_poss[h][1] = j;
			h++;
		}
	}
	return haplo_poss;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int **crea_haplo_poss_ril(int nb_gam)
	-----------------------------------------------------------------------------
	## RETURN:
	@ int **haplo_poss : combination matrix for possible haplotypes (selfing ril)
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int nb_gam : number of gametes
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of the matrix of possible homozygous haplotypes (selfing for RILs )
	according to gametes.
	-----------------------------------------------------------------------------
*/

int **crea_haplo_poss_ril(int nb_gam){

	int **haplo_poss = NULL;
	calloc2d(haplo_poss, nb_gam, 2, int);

	// Creation of the combination matrix of possible haplotypes
	for(int i = 0; i < nb_gam; i++){
		haplo_poss[i][0] = i;
		haplo_poss[i][1] = i;
	}
	return haplo_poss;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void init_haplo_set(s_haplo *haplo_set)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_haplo *haplo_set : pointer to the set of haplotypes
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Initialization of the set of haplotypes.
	-----------------------------------------------------------------------------
*/

void init_haplo_set(s_haplo *haplo_set){

	// Matrix of possible haplotypes "read" (from genotyping data)
	haplo_set->read = NULL;

	// Matrix of possible haplotypes "real"
	haplo_set->coded = NULL;

	// Probability of obtaining each haplotype
	haplo_set->proba = NULL;

	// Vector of the number of polymorphic loci for each haplotype
	haplo_set->nb_poly_loci = NULL;

	// Matrix containing the location of polymorphic loci for all haplotypes of an individual
	haplo_set->loca_poly_loci = NULL;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void alloc_haplo_set(s_haplo *haplo_set, int nb_haplo, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_haplo *haplo_set : pointer to the set of haplotypes
	@ int nb_haplo : number of haplotypes in the set
	@ s_params *params : structure containing all data parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Memory allocation for the set of haplotypes.
	-----------------------------------------------------------------------------
*/

void alloc_haplo_set(s_haplo *haplo_set, int nb_haplo, s_params *params){

	// Matrix of possible haplotypes "read" (from genotyping data)
	haplo_set->read = NULL;
	malloc2d(haplo_set->read, nb_haplo, params->nb_locus * 2, geno_read);

	// Matrix of possible haplotypes "real"
	haplo_set->coded = NULL;
	malloc2d(haplo_set->coded, nb_haplo, params->nb_locus * 2, geno_coded);

	// Probability of obtaining each haplotype
	haplo_set->proba = NULL;
	calloc1d(haplo_set->proba, nb_haplo, double);

	// Vector of the number of polymorphic loci for each haplotype
	haplo_set->nb_poly_loci = NULL;
	calloc1d(haplo_set->nb_poly_loci, nb_haplo, int);

	// Matrix containing the location of polymorphic loci for all haplotypes of an individual
	haplo_set->loca_poly_loci = NULL;
	calloc2d(haplo_set->loca_poly_loci, nb_haplo, params->nb_locus, int);
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void realloc_haplo_set(s_haplo *haplo_set, int new_nb_haplo, int old_nb_haplo, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_haplo *haplo_set : pointer to the set of haplotypes
	@ int new_nb_haplo : new number of haplotypes in the set
	@ int old_nb_haplo : old number of haplotypes in the set
	@ s_params *params : structure containing all data parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Memory reallocation for the set of haplotypes. Here we always reduce the matrix.
	-----------------------------------------------------------------------------
*/

void realloc_haplo_set(s_haplo *haplo_set, int new_nb_haplo, s_params *params){

	if( new_nb_haplo == 0 ){
		free_haplo_set(haplo_set);
	}
	else{
		// Free memory for non-existent haplotypes (or redundant)
		realloc2d(haplo_set->read, new_nb_haplo, params->nb_locus*2, geno_read);
		realloc2d(haplo_set->coded, new_nb_haplo, params->nb_locus*2, geno_coded);
		realloc2d(haplo_set->loca_poly_loci, new_nb_haplo, params->nb_locus, int);
		haplo_set->proba = (double *)realloc(haplo_set->proba, new_nb_haplo*sizeof(double));
		haplo_set->nb_poly_loci = (int *)realloc(haplo_set->nb_poly_loci, new_nb_haplo*sizeof(int));
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void free_haplo_set(s_haplo *haplo_set)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_haplo *haplo_set : pointer to the set of haplotypes
	-----------------------------------------------------------------------------
	## SPECIFICATION: Free memory for the current set of haplotypes.
	-----------------------------------------------------------------------------
*/

void free_haplo_set(s_haplo *haplo_set){

	free_alloc2d(haplo_set->read);
	free_alloc2d(haplo_set->coded);
	free_alloc2d(haplo_set->loca_poly_loci);

	free(haplo_set->proba);
	haplo_set->proba = NULL;

	free(haplo_set->nb_poly_loci);
	haplo_set->nb_poly_loci = NULL;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void print_haplo_set(s_haplo *haplo_set, int nb_haplo, int nb_locus)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_haplo *haplo_set : pointer to the set of haplotypes
	@ int nb_haplo : number of haplotypes in the set
	@ int nb_locus : number of loci for a QTL position (or chromosome)
	-----------------------------------------------------------------------------
	## SPECIFICATION: Display informations regarding the haplotypes.
	-----------------------------------------------------------------------------
*/

void print_haplo_set(s_haplo *haplo_set, int nb_haplo, int nb_locus){

	printf("\n-------------\n");
	printf(".:HAPLO_SET:.");
	printf("\n-------------\n");

	// Loop on the individuals haplotypes
	for(int h = 0; h < nb_haplo; h++){
		for (int m = 0; m < nb_locus; m++){
			printf("%d ", haplo_set->read[h][m]);  //// imprimer l'allele correspondant aussi ?
		}
		printf(" : ");
		for(int m = 0; m < nb_locus; m++){
			printf("%d ", haplo_set->read[h][m + nb_locus]);
		}
		printf(" | ");
		for(int m = 0; m < nb_locus; m++){
			printf("%d ", haplo_set->coded[h][m]); //// imprimer l'allele correspondant aussi ?
		}
		printf(" : ");
		for(int m = 0; m < nb_locus; m++){
			printf("%d ", haplo_set->coded[h][m + nb_locus]);
		}
		printf(" | ");
		for(int m = 0; m < nb_locus; m++){
			printf("%d ", haplo_set->loca_poly_loci[h][m]);
		}
		printf(" | ");
		printf("%d : %f\n", haplo_set->nb_poly_loci[h], haplo_set->proba[h]);
	}
	printf("\n\n");
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void copy_haplo_in_haplo_set(s_haplo *haplo_set_max, s_haplo *haplo_set, s_params *params, int index_haplo_max, int index_haplo)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_haplo *haplo_set_max : set of haplotypes max (filter on genotypes)
	@ s_haplo *haplo_set : set of haplotypes (filter on genotypes + without doubles)
	@ s_params *params : structure containing all program parameters
	@ int index_haplo_max : index of the haplotype tested for adding
	@ int index_haplo : index of the haplotype that will be added
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Copy of the haplotype attributes in the individual set of haplotypes.
	-----------------------------------------------------------------------------
*/

void copy_haplo_in_haplo_set(s_haplo *haplo_set_max, s_haplo *haplo_set, s_params *params, int index_haplo_max, int index_haplo){

	// Copy of the haplotype probability
	haplo_set->proba[index_haplo] = haplo_set_max->proba[index_haplo_max];

	// Copy of the number of polymorphic loci in this haplotype
	haplo_set->nb_poly_loci[index_haplo] = haplo_set_max->nb_poly_loci[index_haplo_max];

	// Copy of the localisation of polymorphic loci
	for(int i=0; i<params->nb_locus; i++){
		haplo_set->loca_poly_loci[index_haplo][i] = haplo_set_max->loca_poly_loci[index_haplo_max][i];
	}

	// Copy of the matrix of phased genotype read and coded
	for(int i=0; i<(2*params->nb_locus); i++){
		haplo_set->read[index_haplo][i] = haplo_set_max->read[index_haplo_max][i];
		haplo_set->coded[index_haplo][i] = haplo_set_max->coded[index_haplo_max][i];
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	s_gametes haplo2gam_set(s_haplo *haplo_set, s_mk *markers, s_params *params, int num_haplo, int *p_nb_gam, int mode)
	-----------------------------------------------------------------------------
	## RETURN:
	@ s_gametes gam_set : set of gametes corresponding to the selected haplotype
	-----------------------------------------------------------------------------
	## PARAMETRES:
	@ s_haplo *haplo_set : set of haplotypes
	@ s_mk *markers : array of markers
	@ s_params *params : structure containing all program parameters
	@ int num_haplo : index of the selected haplotype
	@ int *p_nb_gam : pointer to the number of gametes
	@ int mode : [1] r --> recombination fraction per meiosis [2] R --> observed among RILs
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of possible gametes for an haplotype in the set.
	-----------------------------------------------------------------------------
*/

s_gametes haplo2gam_set(s_haplo *haplo_set, s_params *params, int num_haplo, int *p_nb_gam, int mode){

	int k = 0, nb_poly_loci = haplo_set->nb_poly_loci[num_haplo];
	double r = 0.0, R = 0.0; // Recombination fraction observed (normal or among RILs)

	int prec_nb_gamete = (*p_nb_gam) = (int) pow(2.0, (double)nb_poly_loci);

	// Creation of the gamete recombination matrix depending on the the localization of the polymorphic loci and the est_max_rec_nb filter.
	// NB: params->est_max_rec_nb is set to -1.0 if no filter is required. 
	// CAUTION : The value of *p_nb_gam may therefore be reduced by the number of gametes ignored.
	int** mat_rec = crea_mat_rec3(params->nb_locus, *p_nb_gam, haplo_set->loca_poly_loci[num_haplo], params->est_max_rec_nb);

	// Sum of proba
	double sum=0;

	// Creation the vector of index corresponding to the positions of polymorphic loci
	int *index_loca_poly_loci = NULL;
	calloc1d(index_loca_poly_loci, nb_poly_loci, int);

	// Creation of a set of gametes (+memory allocation)
	s_gametes gam_set;

	gam_set.read = NULL;
	malloc2d(gam_set.read, *p_nb_gam, params->nb_locus, geno_read);

	gam_set.coded = NULL;
	malloc2d(gam_set.coded, (*p_nb_gam), params->nb_locus, geno_coded);

	gam_set.proba = NULL;
	calloc1d(gam_set.proba, (*p_nb_gam), double);

	for(int j = 0; j < params->nb_locus; j++){
		// A polymorphic locus is found (hetero residual taking into account)
		if( haplo_set->loca_poly_loci[num_haplo][j] == 1 ){
			// The column number correspond to the polymorphic locus
			index_loca_poly_loci[k] = j;
			k++;
		}
	}

	// Initialization of the probabilities regarding the presence of polymorphic loci
	int l; int shift=0;

	for(int g = 0; g < (*p_nb_gam); g++){
		if(haplo_set->nb_poly_loci[num_haplo] == 0){
			gam_set.proba[g-shift] = 1.0;
		}
		else{
			gam_set.proba[g-shift] = 0.5;
		}
		// Probabilities are calculated
		for(l = 1; l < haplo_set->nb_poly_loci[num_haplo]; l++){
			// r = 0.5 * (1.0 - exp(- 2.0 * (markers[params->num_mk_inf + index_loca_poly_loci[l]].pos - markers[params->num_mk_inf + index_loca_poly_loci[l - 1]].pos) / 100));
			// R = (2.0 * r) / (1.0 + (2.0 * r));

			r = params->vect_rec_frac[index_loca_poly_loci[l - 1]][index_loca_poly_loci[l]].r;
			R = params->vect_rec_frac[index_loca_poly_loci[l - 1]][index_loca_poly_loci[l]].R;

			// If 2 loci are different
			if( mat_rec[g][index_loca_poly_loci[l]] != mat_rec[g][index_loca_poly_loci[l-1]] ){
				if(r==0){		// => the proba of this gamete is 0. ignore it.
					shift++;	// index offset (for g)
					break;
				}
				if( mode == 1 ){
					gam_set.proba[g-shift] *= r;
				}
				// RIL's
				else if( mode == 2 ){
					gam_set.proba[g-shift] *= R;
				}
			}
			// If 2 loci are equals
			else if( mat_rec[g][index_loca_poly_loci[l]] == mat_rec[g][index_loca_poly_loci[l-1]] ){
				if( mode == 1 ){
					gam_set.proba[g-shift] *= (1 - r);
				}
				else if( mode == 2 ){
					gam_set.proba[g-shift] *= (1 - R);
				}
			}
		}
		if (l < haplo_set->nb_poly_loci[num_haplo]) {
			continue; // this gamete is ignored because its proba = 0 (see the break statement above)
		}
		sum += gam_set.proba[g-shift];

		// Possible gametes are created (from the haplotype) regarding the recombination matrix
		for(int il = 0; il < params->nb_locus; il++){
			if( mat_rec[g][il] == 0 ){
				gam_set.read[g-shift][il] = haplo_set->read[num_haplo][il];
				gam_set.coded[g-shift][il] = haplo_set->coded[num_haplo][il];
			}
			else if( mat_rec[g][il] == 1 ){
				gam_set.read[g-shift][il] = haplo_set->read[num_haplo][il + params->nb_locus];
				gam_set.coded[g-shift][il] = haplo_set->coded[num_haplo][il + params->nb_locus];
			}
		}
	}
	(*p_nb_gam) -= shift; // subtract the number of gametes with a null proba from the initial number of gametes. 
	
	// We rescale the proba of each gamete such that the sum is still 1 (only if some gametes have been ignored by the "Poisson" filter)
	if (prec_nb_gamete-shift > *p_nb_gam){
		for (int i=0; i<*p_nb_gam; i++) gam_set.proba[i]/=sum;
	}
	
	if (shift>0) realloc_gam_set(&gam_set, *p_nb_gam, params);
	
	free_alloc2d(mat_rec);
	free(index_loca_poly_loci);
	index_loca_poly_loci = NULL;

	return gam_set;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void multiply_gam_per_haplo(s_gametes *gam_set, s_haplo *haplo_set, int nb_gam, int num_haplo)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETRES:
	@ s_gametes *gam_set : pointer to a set of gametes
	@ s_haplo *haplo_set : pointer to a set of haplotypes
	@ int nb_gam : number of gametes in the set
	@ int num_haplo : index of the current haplotype
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Each gamete is multiplied by the probability of its coming from haplotype.
	-----------------------------------------------------------------------------
*/
void multiply_gam_per_haplo(s_gametes *gam_set, s_haplo *haplo_set, int nb_gam, int num_haplo){

	for(int g=0; g<nb_gam; g++){
		gam_set->proba[g] = gam_set->proba[g] * haplo_set->proba[num_haplo];
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void free_tab_haplo_set(s_haplo *tab_haplo_set, int *tab_nb_haplo_set, int nb_haplo_prec, int nb_locus)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_haplo *tab_haplo_set : array of sets of haplotypes
	@ int nb_haplo_prec : number of haplotypes that the individual had
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Free memory for the array of sets of haplotypes.
	-----------------------------------------------------------------------------
*/

void free_tab_haplo_set(s_haplo *tab_haplo_set, int nb_haplo_prec){

	// Loop on the array of sets of haplotypes
	for(int i=0; i<nb_haplo_prec; i++){
		free_haplo_set(&tab_haplo_set[i]);
	}
	free(tab_haplo_set);
	tab_haplo_set = NULL;
}
