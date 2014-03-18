#include "../headers/CompResultsWindow.h"

using namespace std;


CompResultsWindow::CompResultsWindow(QWidget *parent) : QDialog(parent){
	ui_complementation_results.setupUi(this);

	// Action buttons (cancel)
	connect(ui_complementation_results.buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(close()));
}


CompResultsWindow::~CompResultsWindow() {

}


void CompResultsWindow::setTextResultsLabel(QString &str){
	ui_complementation_results.resultsLabel->setText(str);
}
