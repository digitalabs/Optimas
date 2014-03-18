#include "../headers/QtlWeightWindow.h"

using namespace std;


QtlWeightWindow::QtlWeightWindow(QWidget *parent = NULL) : QDialog(parent){
	ui.setupUi(this);
}


QtlWeightWindow::QtlWeightWindow(vector<double>& vect_qtl_weight, QWidget *parent = NULL) : QDialog(parent){
	ui.setupUi(this);
	m_vect_ptr = &vect_qtl_weight;
	initWindow();
	connect(ui.updateWeightPushButton, SIGNAL(clicked()), this, SLOT(setQtlWeight()));
	connect(ui.allPushButton, SIGNAL(clicked()), this, SLOT(selectAllQtl()));
	connect(ui.nonePushButton, SIGNAL(clicked()), this, SLOT(selectNoQtl()));

	// Action buttons
	connect(ui.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(resetDefaultWeight()));
	connect(ui.buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(applyWeight()));
	connect(ui.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));
}


QtlWeightWindow::~QtlWeightWindow() {}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: instantiate the QTLs table items.
	-----------------------------------------------------------------------------
*/

void QtlWeightWindow::initWindow(){

	int nqtl = m_vect_ptr->size();

	ui.qtlTableWidget->setRowCount(nqtl);
	ui.qtlTableWidget->setColumnCount(2); // 2 columns: QTL + weight spin-boxes.
	ui.qtlTableWidget->setHorizontalHeaderLabels(QStringList()<<"QTL"<<"Weight");

	// QTLs view
	for(int i=0; i<nqtl; i++){
		QTableWidgetItem* newItem = new QTableWidgetItem(QString("QTL%1").arg(i+1, 0, 10));
		newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		ui.qtlTableWidget->setItem(i, 0, newItem);

		QDoubleSpinBox* spinBox = new QDoubleSpinBox(ui.qtlTableWidget);
		//spinBox->setValue(m_vect_ptr->at(i)); //// Ne fonctionne pas !!!! pourquoi ?
		ui.qtlTableWidget->setCellWidget(i, 1, spinBox);
		((QDoubleSpinBox*)ui.qtlTableWidget->cellWidget(i, 1))->setValue(m_vect_ptr->at(i));
		((QDoubleSpinBox*)ui.qtlTableWidget->cellWidget(i, 1))->setSingleStep(0.01);
		//qDebug() << m_vect_ptr->at(i) << " " << spinBox->value() << " " << ((QDoubleSpinBox*)ui.qtlTableWidget->cellWidget(i, 1))->value() << endl;
	}
	ui.qtlTableWidget->resizeRowsToContents();
	ui.qtlTableWidget->resizeColumnsToContents();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: set each selected QTL to the selected weight.
	-----------------------------------------------------------------------------
*/

void QtlWeightWindow::setQtlWeight(){
	int nrow = ui.qtlTableWidget->rowCount();
	double val = ui.weightSpinBox->value();
	for(int i=0; i<nrow; i++)
		if(ui.qtlTableWidget->item(i,0)->isSelected())
			((QDoubleSpinBox*)ui.qtlTableWidget->cellWidget(i,1))->setValue(val);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: select all QTL items.
	-----------------------------------------------------------------------------
*/

void QtlWeightWindow::selectAllQtl(){
	int nrow = ui.qtlTableWidget->rowCount();
	for(int i=0; i<nrow; i++)
		ui.qtlTableWidget->item(i,0)->setSelected(true);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: unselect all QTL items.
	-----------------------------------------------------------------------------
*/

void QtlWeightWindow::selectNoQtl(){
	int nrow = ui.qtlTableWidget->rowCount();
	for(int i=0; i<nrow; i++)
		ui.qtlTableWidget->item(i,0)->setSelected(false);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: reset the default Weight (1.0) to the selected QTLs.
	-----------------------------------------------------------------------------
*/

void QtlWeightWindow::resetDefaultWeight(){
	int nrow = ui.qtlTableWidget->rowCount();
	for(int i=0; i<nrow; i++)
		//if(ui.qtlTableWidget->item(i,0)->isSelected())
			((QDoubleSpinBox*)ui.qtlTableWidget->cellWidget(i,1))->setValue(1.0);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: hide/show rows (individual) and columns (QTLs) of table,
	according to their corresponding check state in the TableViewWindow.
	-----------------------------------------------------------------------------
*/

void QtlWeightWindow::applyWeight(){
	int nqtl = ui.qtlTableWidget->rowCount();

	// update column.
	for(int i=0; i<nqtl; i++){
		m_vect_ptr->at(i) = ((QDoubleSpinBox*)ui.qtlTableWidget->cellWidget(i,1))->value();
	}
	accept();
}
