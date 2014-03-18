#include "../headers/gametes.h"


/**-----------------------------------------------------------------------------
	## FUNCTION:
	bool stl_comp_gam(const s_gam_search_index & i1, const s_gam_search_index & i2)
	-----------------------------------------------------------------------------
	## RETURN:
	bool : true if i1.geno_b10 < i2.geno_b10, false otherwise.
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ const s_gam_search_index & i1 : a const reference to the 1st s_gam_search_index
	@ const s_gam_search_index & i2 : a const reference to the 2nd s_gam_search_index
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	function used by the STL sort function to compare 2 s_gam_search_index values.
	-----------------------------------------------------------------------------
*/

bool stl_comp_gam(const s_gam_search_index & g1, const s_gam_search_index & g2){
	return g1.geno_b10 < g2.geno_b10;
}

// idem, called in case of residual heterozygous in IL.
bool stl_comp_gam_hr(const s_gam_search_index & g1, const s_gam_search_index & g2){
	
	if(g1.geno_b10 == g2.geno_b10){ // coded are equal but read may not (if parents are residual hetrerozygous).
		return read1_eq_read2(g1.gset->read[g1.i_gam], g2.gset->read[g2.i_gam], g1.nb_locus) < 0;
	}
	return g1.geno_b10 < g2.geno_b10;
}



/**-----------------------------------------------------------------------------
	## FUNCTION:
	int **crea_mat_rec(int nb_locus)
	-----------------------------------------------------------------------------
	## RETURN:
	int **mat_rec : matrix of possible recombination of gametes
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int nb_locus : number of loci (markers + QTL) for a QTL position (or chromosome)
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of the gamete recombination matrix. (000,001,010,011...111)
	-----------------------------------------------------------------------------
*/

int **crea_mat_rec(int nb_locus){

	// Number of possible combinations according to the number of loci
	int nb_comb = (int)pow(2.0, (double)nb_locus); // nb_locus max = 31 (int codé sur 4 octets => 2^31 valeur positives)
	int denom = 0; // Avoid calling pow( 2.0, nb_locus-(j+1) ) in the double loop

	// Memory allocation for the recombination matrix
	int **mat_rec = NULL;
	calloc2d(mat_rec, nb_comb, nb_locus, int);

	for(int i=0; i<nb_comb; i++){
		denom = 2;
		for(int j=0; j<nb_locus;j++){
			// mat_rec[i][j] = (int) fmod((double)i/( pow( 2.0, (double)(nb_locus-(j+1)) ) ), 2);
			// nb_comb/denom is equivalent to 2^nb_locus / 2^(j+1) that is equivalent to 2^(nb_locus-(j+1))
			mat_rec[i][j] = (i/(nb_comb/denom))%2; //// meme resultat avec modulo entier (%) et 2x plus rapide que fmod
			denom *= 2;
		}
	}
	return mat_rec;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int **crea_mat_rec2(int nb_locus, int nb_poly_loci, int* loca_poly_loci)
	-----------------------------------------------------------------------------
	## RETURN:
	int **mat_rec : matrix of possible recombination of gametes
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int nb_locus : number of loci (markers + QTL) for a QTL position (or chromosome)
	@ int nb_poly_loci : number of polymorphic loci among these loci.
	@ int* loca_poly_loci : array showing which loci are polymorphic or not (respectively 1 or 0).
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of the gamete recombination matrix. (000,001,010,011...111)
	-----------------------------------------------------------------------------
*/

int** crea_mat_rec2(int nb_locus, int nb_poly_loci, int* loca_poly_loci ){

	int nb_comb = (int)pow(2.0, (double)nb_poly_loci); // nb_locus max = 31 (int codé sur 4 octets => 2^31 valeur positives)
	int denom = 0;
	
	int **mat_rec = NULL;
	malloc2d(mat_rec, nb_comb, nb_locus, int);
	
	for(int i=0; i<nb_comb; i++){
		denom = 2;
		for(int j=0; j<nb_locus;j++){
			if( loca_poly_loci[j] == 0 ) mat_rec[i][j] = 0;
			else {
				mat_rec[i][j] = (i/(nb_comb/denom))%2;
				denom *= 2;
			}
		}
	}
	return mat_rec;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int **crea_mat_rec3(int nb_locus, int & nb_comb, int* loca_poly_loci, int nrecmax=-1)
	-----------------------------------------------------------------------------
	## RETURN:
	int **mat_rec : matrix of possible recombination of gametes
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int nb_locus : number of loci (markers + QTL) for a QTL position (or chromosome)
	@ int & nb_comb : number of combination, has to be 2^(number of polymorphic loci), may be modified.
	@ int* loca_poly_loci : array indicating which loci are polymorphic or not (respectively 1 or 0).
	@ int nrecmax : max number of recombination allowed per gamete, according to a Poisson distribution.
	                Optional parameter.
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Creation of the gamete recombination matrix. (000,001,010,011...111) depending
	on the the localisation of the polymorphic loci and the nrecmax filter.
	The value of nb_comb may therefore be reduced by the number of gametes ignored.
	-----------------------------------------------------------------------------
*/
//// A FAIRE : char** au lieu de int**
int** crea_mat_rec3(int nb_locus, int & nb_comb, int* loca_poly_loci, int nrecmax){

	int denom = 0;
	int i2, nrec, shift=0;
	
	
	int **mat_rec = NULL;
	malloc2d(mat_rec, nb_comb, nb_locus, int);
	
	if( nrecmax == -1 ) nrecmax = nb_locus; // => don't filter the gametes (using the 1% Poisson estimation).
	
	for(int i=0; i<nb_comb; i++){
		////denom=2;
		i2 = i-shift;
		nrec = 0;
		// first locus
		if( loca_poly_loci[0] == 0){
			denom = 2;
			mat_rec[i2][0] = 0;
		}
		else {
			mat_rec[i2][0] = (i/(nb_comb/2))%2;
			denom = 4;
		}
		// 2nd locus and furthers.
		for(int j=1; j<nb_locus;j++){ //// j=0
			if( loca_poly_loci[j] == 0 ) mat_rec[i2][j] = 0;
			else {
				mat_rec[i2][j] = (i/(nb_comb/denom))%2;
				denom *= 2;
			}
			if( mat_rec[i2][j] != mat_rec[i2][j-1] ){
				nrec += 1;
				if( nrec > nrecmax ){
					shift++;
					break;
				}
			}
		}
	}
	nb_comb -= shift;
	return mat_rec;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void print_mat_rec(int **mat_rec, int nb_locus)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int **mat_rec : matrix of possible recombination of gametes
	@ int nb_locus : number of loci for a QTL position (or chromosome)
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Display of the matrix of possible recombination of gametes.
	-----------------------------------------------------------------------------
*/

void print_mat_rec(int **mat_rec, int nb_locus){
	int nb_comb = (int)pow(2.0, (double)nb_locus);

	for(int i=0; i<nb_comb; i++){
		for(int j=0; j<nb_locus;j++){
			printf("%d ", mat_rec[i][j]);
		}
		printf("\n");
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void init_gam_set(s_gametes *gam_set)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_gametes *gam_set : pointer to the set of gametes
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Initialization of the set of gametes.
	-----------------------------------------------------------------------------
*/

void init_gam_set(s_gametes *gam_set){

	// Matrix of possible gametes read (after genotyping compared to bands)
	gam_set->read = NULL;

	// Matrix of possible gametes "real" (parental alleles)
	gam_set->coded = NULL;

	// Probability to obtain each gamete
	gam_set->proba = NULL;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void alloc_gam_set(s_gametes *gam_set, int nb_gam, int nb_locus)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_gametes *gam_set : pointer to the set of gametes
	@ int nb_gam : number of gametes in the set
	@ s_params *params : structure containing all program parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Memory allocation for the set of gametes (depending on the number of possible
	gametes based on the genotyping data).
	-----------------------------------------------------------------------------
*/

void alloc_gam_set(s_gametes *gam_set, int nb_gam, s_params *params){

	// Matrix of possible gametes read (after genotyping compared to bands)
	gam_set->read = NULL;
	malloc2d(gam_set->read, nb_gam, params->nb_locus, geno_read);

	// Matrix of possible gametes "real" (parental alleles)
	gam_set->coded = NULL;
	malloc2d(gam_set->coded, nb_gam, params->nb_locus, geno_coded);

	// Probability to obtain each gamete
	gam_set->proba = NULL;
	calloc1d(gam_set->proba, nb_gam, double);
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void realloc_gam_set(s_haplo *gam_set, int new_nb_gam, int old_nb_gam, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_haplo *gam_set : pointer to the set of gametes
	@ int new_nb_gam : new number of gametes in the set
	@ int old_nb_gam : old number of gametes in the set
	@ s_params *params : structure containing all data parameters
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Memory reallocation for the set of gametes. Here we always reduce the matrix.
	-----------------------------------------------------------------------------
*/

void realloc_gam_set(s_gametes *gam_set, int new_nb_gam, s_params *params){

	if( new_nb_gam == 0 ){
		free_gam_set(gam_set);
	}
	else{
		// Free memory for non-existent haplotypes (or redundant)
		realloc2d(gam_set->read, new_nb_gam, params->nb_locus, geno_read);
		realloc2d(gam_set->coded, new_nb_gam, params->nb_locus, geno_coded);
		gam_set->proba = (double *)realloc(gam_set->proba, new_nb_gam*sizeof(double));
	}
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void free_gam_set(s_gametes *gam_set)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_gametes *gam_set : pointer to the possible gametes of the current individual
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Free memory for the current individual gametes.
	-----------------------------------------------------------------------------
*/

void free_gam_set(s_gametes *gam_set){

	free_alloc2d(gam_set->read);
	free_alloc2d(gam_set->coded);
	free(gam_set->proba);
	gam_set->proba = NULL;
}


/**-----------------------------------------------------------------------------
	## FONCTION:
	void free_tab_gam_set(s_gametes *tab_gam_set, int nb_gam_set)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETRES:
	@ s_gametes *tab_gam_set : array of sets of gametes
	@ int nb_gam_set : number of set of gametes ( = number of haplotype for the individual)
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Free memory for the array of sets of gametes.
	-----------------------------------------------------------------------------
*/

void free_tab_gam_set(s_gametes *tab_gam_set, int nb_gam_set){

	// Loop on the array of sets of gametes
	for(int i=0; i<nb_gam_set; i++){
		free_gam_set(&tab_gam_set[i]);
	}
	free(tab_gam_set);
	tab_gam_set = NULL;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void print_gam_set(s_gametes *gam_set, int nb_gametes, int nb_locus)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETRES:
	@ s_gametes *gam_set : pointer to a set of gametes
	@ int nb_gametes : number of gametes in the set
	@ int nb_locus : number of loci (markers + QTL) for a QTL position (or chromosome)
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Display informations regarding the gametes.
	-----------------------------------------------------------------------------
*/

void print_gam_set(s_gametes *gam_set, int nb_gametes, int nb_locus){

	printf("\n---------------\n");
	printf(".: GAMETE_SET:.");
	printf("\n---------------\n");

	printf("Read Matrix:\n");
	for(int j=0; j<nb_gametes; j++){
		for(int i=0; i<nb_locus; i++){
			printf("%d ", gam_set->read[j][i]); //// imprimer aussi la correspondance.
		}
		printf("\n");
	}

	printf("\nCoded Matrix:\n");
	for(int j=0; j<nb_gametes; j++){
		for(int i=0; i<nb_locus; i++){
			printf("%d ", gam_set->coded[j][i]); //// imprimer aussi la correspondance.
		}
		printf("\n");
	}

	printf("\nVector of proba:\n");
	for(int j=0; j<nb_gametes; j++){
		printf("%f ", gam_set->proba[j]);
	}
	printf("\n\n");
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	int comp_gam(char **gam1, char **gam2, int nb_locus)
	-----------------------------------------------------------------------------
	## RETURN: [0] if the gametes are the same and [1] otherwise
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ char **gam1 : 1st gamete
	@ char **gam2 : 2nd gamete
	@ int nb_locus : number of loci for a QTL position (or chromosome)
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Compare if 2 gametes are the same.
	-----------------------------------------------------------------------------
*/

int comp_gam(char **gam1, char **gam2, int nb_locus){

	for(int i=0; i<nb_locus; i++){
		if( strcmp(gam1[i], gam2[i]) != 0 ){
			return 1;
		}
	}
	return 0;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void copy_gam1_in_gam_set2(s_gametes *gam_set1, s_gametes *gam_set2, s_params *params, int num_gam1, int num_gam2)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_gametes *gam_set1 : total set of gametes ( >= gam_set2 )
	@ s_gametes *gam_set2 : set of filtered gametes
	@ s_params *params : structure containing all program parameters
	@ int num_gam1 : index of the gamete tested for adding (gam_set1)
	@ int num_gam2 : index of the current gamete (gam_set2)
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Copy of a gamete attributes in another set of gametes.
	-----------------------------------------------------------------------------
*/

void copy_gam1_in_gam_set2(s_gametes *gam_set1, s_gametes *gam_set2, s_params *params, int num_gam1, int num_gam2){

	// Copy of the probability of this gamete
	gam_set2->proba[num_gam2] = gam_set1->proba[num_gam1];

	// Copy of the genotypes read and real
	for(int i=0; i<(params->nb_locus); i++){
		gam_set2->read[num_gam2][i] = gam_set1->read[num_gam1][i];
		gam_set2->coded[num_gam2][i] = gam_set1->coded[num_gam1][i];
	}
}
