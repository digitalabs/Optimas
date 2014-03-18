#include "../headers/results.h"

using namespace std;


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void crea_results_final_tabs(s_indiv *tab_indiv, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *tab_indiv : array of individuals
	@ s_params *params : structure containing all data parameters
   -----------------------------------------------------------------------------
   ## SPECIFICATION: All the result files/tables are written (Molecular Score,
   homozygous favorable/unfavorable heterozygous, founders alleles at all/each
   QTL for each individual.
   -----------------------------------------------------------------------------
*/

void crea_results_final_tabs(s_indiv *tab_indiv, s_params *params){

	int index_cycles = 2;
	double uc = 0.0; // Utility criterion

	char name_file_scores[NB_PATH_MAX], name_file_parents[NB_PATH_MAX], name_file_homo_hetero[NB_PATH_MAX], name_file_check_haplo[NB_PATH_MAX];

	sprintf(name_file_scores, "%s/tab_scores.txt", params->file_root);
	sprintf(name_file_parents, "%s/tab_parents.txt", params->file_root);
	sprintf(name_file_homo_hetero, "%s/tab_homo_hetero.txt", params->file_root);
	sprintf(name_file_check_haplo, "%s/tab_check_diplo.txt", params->file_root);

	FILE *file_scores = open_file(name_file_scores, "w");
	FILE *file_parents = open_file(name_file_parents, "w");
	FILE *file_homo_hetero = open_file(name_file_homo_hetero, "w");
	FILE *file_check_haplo = open_file(name_file_check_haplo, "w");

	// Headers
	for(int i=0; i<params->nb_qtl; i++){
		if(i == 0){
			fprintf(file_scores, "Id\tP1\tP2\tCycle\tGroup\tMS\tWeight\tUC\tNo.(+/+)\tNo.(-/-)\tNo.(+/-)\tNo.(?)");
			fprintf(file_homo_hetero, "Id\tCycle\tGroup\tMS\tAll(+/+)\tAll(-/-)\tAll(+/-)");
			fprintf(file_parents, "Id\tCycle\tGroup\tMS");
			fprintf(file_check_haplo, "Id\tMS\tAll");

			// First individuals are founders
			for(int j=0; j<params->nb_founders; j++){
				fprintf(file_parents, "\tAll(%s)", params->coded2str[j]); // ex indiv->parental_allele
			}
		}
		fprintf(file_scores, "\tQTL%d", i + 1);
		fprintf(file_check_haplo, "\tQTL%d", i + 1);
		fprintf(file_homo_hetero, "\tQTL%d(+/+)\tQTL%d(-/-)\tQTL%d(+/-)", i + 1, i + 1, i + 1);
		for(int j=0; j<params->nb_founders; j++){
			fprintf(file_parents, "\tQTL%d(%s)", i + 1, params->coded2str[j]); // ex indiv->parental_allele
		}
	}
	fprintf(file_scores, "\n");
	fprintf(file_homo_hetero, "\n");
	fprintf(file_parents, "\n");
	fprintf(file_check_haplo, "\n");

	for(int i=0; i<params->nb_indiv; i++){
		fprintf(file_scores, "%s\t%s\t%s\t%s\t%s", tab_indiv[i].name, tab_indiv[i].par1, tab_indiv[i].par2, tab_indiv[i].cycle, tab_indiv[i].group);
		fprintf(file_homo_hetero, "%s\t%s\t%s", tab_indiv[i].name, tab_indiv[i].cycle, tab_indiv[i].group);
		fprintf(file_parents, "%s\t%s\t%s", tab_indiv[i].name, tab_indiv[i].cycle, tab_indiv[i].group);
		fprintf(file_check_haplo, "%s", tab_indiv[i].name);

		int *tab_nb_homo_hetero = NULL, *tab_nb_homo_hetero_uc = NULL;
		calloc1d(tab_nb_homo_hetero, 4, int);
		calloc1d(tab_nb_homo_hetero_uc, 3, int); // The genotype with the higher probability (++/--/+-)

		// Computation of No.(+/+)/No.(-/-)/No.(+/-)/No(?) regarding the table of homo/hetero (no more based on MS)
		for(int j=1; j< (params->nb_qtl + 1); j++){
			// Fixed homozygous favorable
			if( params->tab_homo_hetero_final[i][j][0] >= params->cut_off_locus_fixed_fav ){
				tab_nb_homo_hetero[0] += 1;
			}
			// Fixed homozygous unfavorable
			else if ( params->tab_homo_hetero_final[i][j][1] >= params->cut_off_locus_fixed_unfav ){
				tab_nb_homo_hetero[1] += 1;
			}
			// Fixed heterozygous
			else if( params->tab_homo_hetero_final[i][j][2] >= params->cut_off_locus_fixed_hetero ){
				tab_nb_homo_hetero[2] += 1;
			}
			// Uncertain genotypes
			else{
				tab_nb_homo_hetero[3] += 1;
			}

			double *val_homo_hetero = NULL;
			calloc1d(val_homo_hetero, 3, double);
			val_homo_hetero[0] = params->tab_homo_hetero_final[i][j][0];
			val_homo_hetero[1] = params->tab_homo_hetero_final[i][j][1];
			val_homo_hetero[2] = params->tab_homo_hetero_final[i][j][2];

			// Computation of the number of homo fav/unfav, hetero for the UC method (discretization via the genotype with the higher probability)
			tab_nb_homo_hetero_uc[maxIndex(val_homo_hetero, 3)] += 1;

			//printf("%f %f %f -> %d\n", val_homo_hetero[0], val_homo_hetero[1], val_homo_hetero[2], maxIndex(val_homo_hetero, 3));

			free(val_homo_hetero);
			val_homo_hetero = NULL;
		}

		for(int j=0; j< (params->nb_qtl + 1); j++){
			// Molecular score (All) and Weight (if classification with allelic affects)
			if( j == 0 ){
				// Utility criterion
				uc = (2 * params->tab_scores_final[i][j]) + (index_cycles * sqrt(tab_nb_homo_hetero_uc[2] * 0.25));

				fprintf(file_scores, "\t%f\t%f", params->tab_scores_final[i][j] / params->nb_qtl, params->tab_scores_final[i][j] / params->nb_qtl);
				fprintf(file_scores, "\t%f\t%d\t%d\t%d\t%d", uc / 2.0, tab_nb_homo_hetero[0], tab_nb_homo_hetero[1], tab_nb_homo_hetero[2], tab_nb_homo_hetero[3] );

				fprintf(file_check_haplo, "\t%f\t%f", params->tab_scores_final[i][j] / params->nb_qtl, params->tab_check_diplo[i][j] / params->nb_qtl);

				fprintf(file_homo_hetero, "\t%f\t%f\t%f\t%f", params->tab_scores_final[i][j] / params->nb_qtl, params->tab_homo_hetero_final[i][j][0] / params->nb_qtl, params->tab_homo_hetero_final[i][j][1] / params->nb_qtl, params->tab_homo_hetero_final[i][j][2]/ params->nb_qtl);
				fprintf(file_parents, "\t%f", params->tab_scores_final[i][j] / params->nb_qtl);

				for(int k=0; k<params->nb_founders; k++){
					fprintf(file_parents, "\t%f", params->tab_founders_final[i][j][k] / params->nb_qtl);
				}
			}
			else{
				fprintf(file_scores, "\t%f", params->tab_scores_final[i][j]);
				fprintf(file_check_haplo, "\t%f", params->tab_check_diplo[i][j]);
				fprintf(file_homo_hetero, "\t%f\t%f\t%f", params->tab_homo_hetero_final[i][j][0], params->tab_homo_hetero_final[i][j][1], params->tab_homo_hetero_final[i][j][2]);
				for(int k=0; k<params->nb_founders; k++){
					fprintf(file_parents, "\t%f", params->tab_founders_final[i][j][k]);
				}
			}
		}
		free(tab_nb_homo_hetero);
		tab_nb_homo_hetero = NULL;
		free(tab_nb_homo_hetero_uc);
		tab_nb_homo_hetero_uc = NULL;

		fprintf(file_scores, "\n");
		fprintf(file_homo_hetero, "\n");
		fprintf(file_parents, "\n");
		fprintf(file_check_haplo, "\n");
	}

	fclose(file_scores);
	fclose(file_parents);
	fclose(file_homo_hetero);
	fclose(file_check_haplo);
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	void crea_summary_events(s_indiv *tab_indiv, s_params *params)
	-----------------------------------------------------------------------------
	## RETURN:
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ s_indiv *tab_indiv : array of individuals
	@ s_params *params : structure containing all data parameters
   -----------------------------------------------------------------------------
   ## SPECIFICATION: questionable results entered in the events.log file are sorted
   by individuals regarding the QTL/mks that can have genotyping errors (if .cmap
   file is used).
   -----------------------------------------------------------------------------
*/

void crea_summary_events(s_indiv *tab_indiv, s_params *params){
	// Creation of the new events.log file (sorted by Id)
	char path_file_sum_log[NB_PATH_MAX];
	sprintf(path_file_sum_log, "%s/events_summary.log", params->file_root);

	FILE *file_sum_log = open_file(path_file_sum_log, "w");
	fprintf(file_sum_log, "SUMMARY [Questionable results at markers/QTL sorted by Id]\n--\n");

	for(int i=0; i<params->nb_indiv; i++){
		if( params->map_ind_qtl_events.find(string(tab_indiv[i].name)) != params->map_ind_qtl_events.end() ){
			fprintf(file_sum_log, "Id: %s\tP1: %s\t P2: %s\tCycle:%s\tQTL:", tab_indiv[i].name, tab_indiv[i].par1, tab_indiv[i].par2, tab_indiv[i].cycle);
			for(int j=0; j<(int)params->map_ind_qtl_events[tab_indiv[i].name].size(); j++){
				fprintf(file_sum_log, " %d", params->map_ind_qtl_events[tab_indiv[i].name][j]);
			}
			fprintf(file_sum_log, "\n");
		}
	}
	fclose(file_sum_log);
}
