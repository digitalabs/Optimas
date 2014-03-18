#include "../headers/TruncationOptionWindow.h"

using namespace std;


TruncationOptionWindow::TruncationOptionWindow(QWidget *parent) : QDialog(parent){
	ui_truncationOption.setupUi(this);

	// Action buttons (cancel)
	connect(ui_truncationOption.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));
}


TruncationOptionWindow::TruncationOptionWindow(const vector<int> &vect_truncation_option, const QStringList &list_cycles, const QStringList &list_groups, QWidget *parent) : QDialog(parent){
	ui_truncationOption.setupUi(this);

	m_list_cycles = list_cycles;
	m_list_groups = list_groups;

	initTruncationOptionWindow(vect_truncation_option);

	// Action buttons (cancel)
	connect(ui_truncationOption.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));

	// Open the corresponding color dialog
	connect(ui_truncationOption.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(resetTruncationOptionWindow()));

	// Cycle-group comboBoxes
	connect(ui_truncationOption.switchComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(switchBoxes(int)));
}


TruncationOptionWindow::~TruncationOptionWindow() {

}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: initialization of the window.
	-----------------------------------------------------------------------------
*/

void TruncationOptionWindow::initTruncationOptionWindow(const vector<int> &vect_truncation_option){
	ui_truncationOption.groupComboBox->addItem("None");
	m_list_cycles.replaceInStrings("All", "None");
	ui_truncationOption.cycleComboBox->addItems(m_list_cycles);
	ui_truncationOption.groupComboBox->addItems(m_list_groups);

	// Cycle
	if( vect_truncation_option[0] == 0 ){
		ui_truncationOption.groupComboBox->hide();
		ui_truncationOption.switchComboBox->setCurrentIndex(0);
		ui_truncationOption.cycleComboBox->setCurrentIndex(vect_truncation_option[1]);
	}
	else{
		ui_truncationOption.cycleComboBox->hide();
		ui_truncationOption.switchComboBox->setCurrentIndex(1);
		ui_truncationOption.groupComboBox->setCurrentIndex(vect_truncation_option[1]);
	}
}


QPushButton *TruncationOptionWindow::getApplyButton(){
	return ui_truncationOption.buttonBox->button(QDialogButtonBox::Apply);
}


bool TruncationOptionWindow::isCycleButtonSelected(){
	return (ui_truncationOption.switchComboBox->currentIndex() == 0);
}


bool TruncationOptionWindow::isGroupButtonSelected(){
	return (ui_truncationOption.switchComboBox->currentIndex() == 1);
}


int TruncationOptionWindow::getCycleIndex(){
	return ui_truncationOption.cycleComboBox->currentIndex();
}


int TruncationOptionWindow::getGroupIndex(){
	return ui_truncationOption.groupComboBox->currentIndex();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int i: the switchComboBox index.
	## RETURN:
	## SPECIFICATION: switch from cycle or group selection comboBox.
	-----------------------------------------------------------------------------
*/

void TruncationOptionWindow::switchBoxes(int i){
	if( i==1 ){
		ui_truncationOption.groupComboBox->show();
		ui_truncationOption.cycleComboBox->hide();
	}
	else{
		ui_truncationOption.cycleComboBox->show();
		ui_truncationOption.groupComboBox->hide();
	}
}


void TruncationOptionWindow::resetTruncationOptionWindow(){

	// Options are reseted
	switchBoxes(0);
	ui_truncationOption.switchComboBox->setCurrentIndex(0);
	ui_truncationOption.cycleComboBox->setCurrentIndex(0);
}
