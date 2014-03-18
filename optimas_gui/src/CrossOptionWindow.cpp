#include "../headers/CrossOptionWindow.h"

using namespace std;


CrossOptionWindow::CrossOptionWindow(QWidget *parent) : QDialog(parent){
	ui_crossOption.setupUi(this);

	// Action buttons (cancel)
	connect(ui_crossOption.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));
}


CrossOptionWindow::CrossOptionWindow(const vector<int> &vect_cross_option, QWidget *parent) : QDialog(parent){
	ui_crossOption.setupUi(this);

	m_vect_cross_option = &vect_cross_option;

	// Action buttons (cancel)
	connect(ui_crossOption.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));

	// Open the corresponding color dialog
	connect(ui_crossOption.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(resetCrossOptionWindow()));

	initCrossOptionWindow(vect_cross_option);

	// All individuals crossed together Vs each individual crossed one time
	connect(ui_crossOption.nbCrossesAllRadioButton, SIGNAL(clicked()), this, SLOT(selectAllIndCrossedTogether()));
	connect(ui_crossOption.nbCrossesRadioButton, SIGNAL(clicked()), this, SLOT(selectNbCross()));
	connect(ui_crossOption.indContribAllRadioButton, SIGNAL(clicked()), this, SLOT(selectAllIndContribution()));
	connect(ui_crossOption.indContribNbRadioButton, SIGNAL(clicked()), this, SLOT(selectNbIndContribution()));
}


CrossOptionWindow::~CrossOptionWindow() {

}


QPushButton *CrossOptionWindow::getApplyButton(){
	return ui_crossOption.buttonBox->button(QDialogButtonBox::Apply);
}


bool CrossOptionWindow::isMaxButtonChecked(){
	return ui_crossOption.nbCrossesAllRadioButton->isChecked();
}


bool CrossOptionWindow::isUnlimitedButtonChecked(){
	return ui_crossOption.indContribAllRadioButton->isChecked();
}


int CrossOptionWindow::getNbCrosses(){
	return ui_crossOption.nbCrossesSpinBox->value();
}


int CrossOptionWindow::getIndContrib(){
	return ui_crossOption.indContribSpinBox->value();
}


int CrossOptionWindow::getCriterionCurrentIndex(){
	return ui_crossOption.criterionComboBox->currentIndex();
}


int CrossOptionWindow::getMethodCurrentIndex(){
	return ui_crossOption.crossingMethodsComboBox->currentIndex();
}


void CrossOptionWindow::initCrossOptionWindow(const vector<int> &vect_cross_option){
	if( (int)vect_cross_option.size() != 11 ){
		fprintf(stderr, "Error in the length of the vector of options (crosses schemes options) !!\n");
		fflush(stderr);
		return;
	}

	//ui_crossOption.criterionComboBox->setDisabled(true); // Criterion disabled by default

	if( vect_cross_option[0] == 0 ){
		ui_crossOption.nbCrossesRadioButton->setChecked(true);
		selectNbCross();
		//ui_crossOption.nbCrossesSpinBox->setEnabled(true);
	}
	else{
		ui_crossOption.nbCrossesAllRadioButton->setChecked(true);
		selectAllIndCrossedTogether();
		//ui_crossOption.nbCrossesSpinBox->setEnabled(false);
		//ui_crossOption.criterionComboBox->setEnabled(true);
		//ui_crossOption.crossingMethodsComboBox->setEnabled(false);
	}

	if( vect_cross_option[1] == 0 ){
		ui_crossOption.indContribNbRadioButton->setChecked(true);
		selectNbIndContribution();
		//ui_crossOption.indContribSpinBox->setEnabled(true);
	}
	else{
		ui_crossOption.indContribAllRadioButton->setChecked(true);
		selectAllIndContribution();
		//ui_crossOption.indContribSpinBox->setEnabled(false);
		//ui_crossOption.criterionComboBox->setEnabled(true);
		//ui_crossOption.crossingMethodsComboBox->setEnabled(false);
	}

	// Options initialization
	ui_crossOption.nbCrossesSpinBox->setValue(vect_cross_option[2]);
	ui_crossOption.indContribSpinBox->setValue(vect_cross_option[3]);
	ui_crossOption.criterionComboBox->setCurrentIndex(vect_cross_option[4]);
	ui_crossOption.crossingMethodsComboBox->setCurrentIndex(vect_cross_option[5]);

	ui_crossOption.nbCrossesSpinBox->setMaximum(vect_cross_option[7]);
	ui_crossOption.indContribSpinBox->setMaximum(vect_cross_option[8]);
}


void CrossOptionWindow::resetCrossOptionWindow(){

	// Options are reseted
	ui_crossOption.nbCrossesAllRadioButton->setChecked(true);
	ui_crossOption.indContribAllRadioButton->setChecked(true);
	ui_crossOption.nbCrossesSpinBox->setValue(2);
	ui_crossOption.indContribSpinBox->setValue(1);
	ui_crossOption.criterionComboBox->setCurrentIndex(0);
	ui_crossOption.crossingMethodsComboBox->setCurrentIndex(0);
	ui_crossOption.nbCrossesSpinBox->setEnabled(false);
	ui_crossOption.indContribSpinBox->setEnabled(false);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: all individuals will be crossed together.
	-----------------------------------------------------------------------------
*/

void CrossOptionWindow::selectAllIndCrossedTogether() {
	ui_crossOption.nbCrossesSpinBox->setEnabled(false);

	if( ui_crossOption.indContribAllRadioButton->isChecked() ){
		// Second list of selection not checked (cross between 2 lists)
		if( (*m_vect_cross_option)[10] == 0 ){
			ui_crossOption.crossingMethodsComboBox->setDisabled(false);
		}
		ui_crossOption.criterionComboBox->setDisabled(true);
	}
	else{
		ui_crossOption.crossingMethodsComboBox->setDisabled(true);
		ui_crossOption.criterionComboBox->setDisabled(false);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: constraint on the maximum number of crosses to be done.
	-----------------------------------------------------------------------------
*/

void CrossOptionWindow::selectNbCross() {
	ui_crossOption.nbCrossesSpinBox->setEnabled(true);
	ui_crossOption.crossingMethodsComboBox->setEnabled(false);
	ui_crossOption.criterionComboBox->setEnabled(true);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: all individuals will contribute to crosses.
	-----------------------------------------------------------------------------
*/

void CrossOptionWindow::selectAllIndContribution() {
	ui_crossOption.indContribSpinBox->setEnabled(false);

	if( ui_crossOption.nbCrossesAllRadioButton->isChecked() ){
		// Second list of selection not checked (cross between 2 lists)
		if( (*m_vect_cross_option)[10] == 0 ){
			ui_crossOption.crossingMethodsComboBox->setEnabled(true);
		}
		else{
			ui_crossOption.crossingMethodsComboBox->setEnabled(false);
		}
		ui_crossOption.criterionComboBox->setEnabled(false);
	}
	else{
		ui_crossOption.criterionComboBox->setEnabled(true);
		ui_crossOption.crossingMethodsComboBox->setEnabled(false);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: constraint on the maximum number of contribution for each
	individual.
	-----------------------------------------------------------------------------
*/

void CrossOptionWindow::selectNbIndContribution() {
	ui_crossOption.indContribSpinBox->setEnabled(true);
	ui_crossOption.crossingMethodsComboBox->setEnabled(false);
	ui_crossOption.criterionComboBox->setEnabled(true);
}
