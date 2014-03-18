#include "../headers/ComplementationWindow.h"

using namespace std;


ComplementationWindow::ComplementationWindow(QWidget *parent) : QDialog(parent){
	ui_complementation.setupUi(this);

	// Action buttons (cancel)
	connect(ui_complementation.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));
}


ComplementationWindow::ComplementationWindow(const vector<double> &vect_comp_params, const QStringList &list_cycles, const QStringList &list_groups, QWidget *parent) : QDialog(parent){
	ui_complementation.setupUi(this);

	m_list_cycles = list_cycles;
	m_list_groups = list_groups;

	initComplementationWindow(vect_comp_params);

	// Action buttons (cancel)
	connect(ui_complementation.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));

	// Rest the value to default
	connect(ui_complementation.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(resetComplementationWindow()));

	// Cycle-group comboBoxes
	connect(ui_complementation.switchComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(switchBoxes(int)));
}


ComplementationWindow::~ComplementationWindow() {

}


QPushButton *ComplementationWindow::getApplyButton(){
	return ui_complementation.buttonBox->button(QDialogButtonBox::Apply);
}


double ComplementationWindow::getTetaValue(){
	return ui_complementation.tetaDoubleSpinBox->value();
}


int ComplementationWindow::getNtValue(){
	return ui_complementation.ntSpinBox->value();
}


double ComplementationWindow::getMsMinValue(){
	return ui_complementation.msMinDoubleSpinBox->value();
}


int ComplementationWindow::getNmaxValue(){
	return ui_complementation.nmaxSpinBox->value();
}


bool ComplementationWindow::isCycleButtonSelected(){
	return (ui_complementation.switchComboBox->currentIndex() == 0);
}


bool ComplementationWindow::isGroupButtonSelected(){
	return (ui_complementation.switchComboBox->currentIndex() == 1);
}


int ComplementationWindow::getCycleIndex(){
	return ui_complementation.cycleComboBox->currentIndex();
}


int ComplementationWindow::getGroupIndex(){
	return ui_complementation.groupComboBox->currentIndex();
}

/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int i: the switchComboBox index.
	## RETURN:
	## SPECIFICATION: switch from cycle or group selection comboBox.
	-----------------------------------------------------------------------------
*/

void ComplementationWindow::switchBoxes(int i){
	if( i==1 ){
		ui_complementation.groupComboBox->show();
		ui_complementation.cycleComboBox->hide();
	}
	else{
		ui_complementation.cycleComboBox->show();
		ui_complementation.groupComboBox->hide();
	}
}


void ComplementationWindow::initComplementationWindow(const vector<double> &vect_comp_params){

	if( (int)vect_comp_params.size() != 6 ){
		fprintf(stderr, "Error in the length of the vector of parameters (complementation) !!\n");
		fflush(stderr);
		return;
	}

	// Cut-off value initialized
	ui_complementation.tetaDoubleSpinBox->setValue(vect_comp_params[0]);
	ui_complementation.ntSpinBox->setValue((int)vect_comp_params[1]);
	ui_complementation.msMinDoubleSpinBox->setValue(vect_comp_params[2]);
	ui_complementation.nmaxSpinBox->setValue((int)vect_comp_params[3]);

	// Cycle/Group initialization
	ui_complementation.groupComboBox->addItem("None");
	m_list_cycles.replaceInStrings("All", "None");
	ui_complementation.cycleComboBox->addItems(m_list_cycles);
	ui_complementation.groupComboBox->addItems(m_list_groups);

	// Cycle
	if( vect_comp_params[4] == 0 ){
		ui_complementation.groupComboBox->hide();
		ui_complementation.switchComboBox->setCurrentIndex(0);
		ui_complementation.cycleComboBox->setCurrentIndex((int)vect_comp_params[5]);
	}
	else{
		ui_complementation.cycleComboBox->hide();
		ui_complementation.switchComboBox->setCurrentIndex(1);
		ui_complementation.groupComboBox->setCurrentIndex((int)vect_comp_params[5]);
	}
}


void ComplementationWindow::resetComplementationWindow(){

	// Cut-off values initialized
	ui_complementation.tetaDoubleSpinBox->setValue(0.47);
	ui_complementation.ntSpinBox->setValue(0);
	ui_complementation.msMinDoubleSpinBox->setValue(0.0);
	ui_complementation.nmaxSpinBox->setValue(0);

	// Cycle/Group comboboxes are reseted
	switchBoxes(0);
	ui_complementation.switchComboBox->setCurrentIndex(0);
	ui_complementation.cycleComboBox->setCurrentIndex(0);
}
