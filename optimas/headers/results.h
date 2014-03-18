#ifndef RESULTS_H_
#define RESULTS_H_


/*-------------------------------- Includes ---------------------------------*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <map>
#include <string>

#include "../headers/markers.h"
#include "../headers/individual.h"
#include "../headers/params.h"
#include "../headers/utils.h"


/*---------------------------- Public prototypes ----------------------------*/

void crea_results_final_tabs(s_indiv *tab_indiv, s_params *params);

void crea_summary_events(s_indiv *tab_indiv, s_params *params);

#endif /* RESULTS_H_ */
