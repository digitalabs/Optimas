#include "../headers/TableViewWindow.h"

using namespace std;

TableViewWindow::TableViewWindow(QTableWidget* t = NULL, QWidget *parent = NULL) : QDialog(parent),table(t){
	ui.setupUi(this);

	initWindow();

	// connects
	// QTLs view buttons
	connect(ui.qtlAddPushButton, SIGNAL(clicked()), this, SLOT(checkQtl()));
	connect(ui.qtlRemovePushButton, SIGNAL(clicked()), this, SLOT(uncheckQtl()));
	connect(ui.qtlAllPushButton, SIGNAL(clicked()), this, SLOT(checkAllQtl()));
	connect(ui.qtlNonePushButton, SIGNAL(clicked()), this, SLOT(checkNoQtl()));

	// Individuals view buttons
	connect(ui.indivAddPushButton, SIGNAL(clicked()), this, SLOT(checkIndiv()));
	connect(ui.indivRemovePushButton, SIGNAL(clicked()), this, SLOT(uncheckIndiv()));
	connect(ui.indivAllPushButton, SIGNAL(clicked()), this, SLOT(checkAllIndiv()));
	connect(ui.indivNonePushButton, SIGNAL(clicked()), this, SLOT(checkNoIndiv()));

	// Individuals comboBoxes.
	connect(ui.switchComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(switchBoxes(int)));

	// OK and Cancel buttons.
	connect(ui.tableViewButtonBox, SIGNAL(accepted()), this, SLOT(okClicked()));
	connect(ui.tableViewButtonBox, SIGNAL(rejected()), this, SLOT(close()));

}

TableViewWindow::~TableViewWindow() {}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: instantiate the QTLs / individuals tables items.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::initWindow(){

	int ncol = table->columnCount();
	int nrow = table->rowCount();
	
	//void setHorizontalHeaderItem ( int column, QTableWidgetItem * item )
	
	ui.qtlTableWidget->setRowCount(ncol - COL_FIRST_QTL);
	ui.qtlTableWidget->setColumnCount(1);
	ui.qtlTableWidget->setHorizontalHeaderLabels(QStringList()<<"QTL");

	ui.indivTableWidget->setColumnCount(5);	// 5 columns: ind P1 P2 Cycle Group
	ui.indivTableWidget->setHorizontalHeaderLabels(QStringList()<<"Id"<<"P1"<<"P2"<<"Cycle"<<"Group");
	ui.indivTableWidget->setRowCount(nrow);

	ui.cycleComboBox->addItem("None");
	ui.groupComboBox->addItem("None");
	ui.groupComboBox->hide();

	// QTLs view.
	for(int i=COL_FIRST_QTL; i<ncol; i++){
		QTableWidgetItem* newItem = new QTableWidgetItem(table->horizontalHeaderItem(i)->text());
		newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		if (table->isColumnHidden(i)) newItem->setCheckState(Qt::Unchecked);
		else newItem->setCheckState(Qt::Checked);
		ui.qtlTableWidget->setItem(i-COL_FIRST_QTL, 0, newItem);
	}
	// Individuals view.
	for(int i=0; i<nrow; i++){
		QTableWidgetItem* newItem = new QTableWidgetItem(*(table->item(i,0)));
		newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		if (table->isRowHidden(i)) newItem->setCheckState(Qt::Unchecked);
		else newItem->setCheckState(Qt::Checked);
		ui.indivTableWidget->setItem(i, 0, newItem);						// individuals ids.
		for(int j=1; j<5; j++) {
			ui.indivTableWidget->setItem( i, j, new QTableWidgetItem( *(table->item(i,j)) ) );
			if (j==3) cycleRows[table->item(i,j)->text()].append(i);		// cycle
			else if (j==4) groupRows[table->item(i,j)->text()].append(i);	// group
		}
	}
	ui.qtlTableWidget->resizeRowsToContents();
	ui.qtlTableWidget->resizeColumnsToContents();
	ui.indivTableWidget->resizeRowsToContents();
	ui.indivTableWidget->resizeColumnsToContents();

	ui.cycleComboBox->addItems(cycleRows.keys());
	ui.groupComboBox->addItems(groupRows.keys());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QList<QListWidgetItem *> & items: an items list.
	@ Qt::CheckState state: a check state (value is Qt::Checked or Qt::Unchecked)
	## RETURN:
	## SPECIFICATION: set the check state of the given items to state.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::setItemsCheckState(const QList<QListWidgetItem *> & items, Qt::CheckState state){
	for(int i=0; i<items.size(); i++) items[i]->setCheckState(state);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: check the selected QTL items.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::checkQtl(){
	int nrow = ui.qtlTableWidget->rowCount();
	for(int i=0; i<nrow; i++)
		if(ui.qtlTableWidget->item(i,0)->isSelected())
			ui.qtlTableWidget->item(i,0)->setCheckState(Qt::Checked);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: uncheck the selected QTL items.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::uncheckQtl(){
	int nrow = ui.qtlTableWidget->rowCount();
	for(int i=0; i<nrow; i++)
		if(ui.qtlTableWidget->item(i,0)->isSelected())
			ui.qtlTableWidget->item(i,0)->setCheckState(Qt::Unchecked);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: check all QTL items.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::checkAllQtl(){
	int nrow = ui.qtlTableWidget->rowCount();
	for(int i=0; i<nrow; i++)
		ui.qtlTableWidget->item(i,0)->setCheckState(Qt::Checked);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: uncheck all QTL items.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::checkNoQtl(){
	int nrow = ui.qtlTableWidget->rowCount();
	for(int i=0; i<nrow; i++)
		ui.qtlTableWidget->item(i,0)->setCheckState(Qt::Unchecked);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: check the selected individual items.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::checkIndiv(){
	int nrow = ui.indivTableWidget->rowCount();
	for(int i=0; i<nrow; i++)
		if(ui.indivTableWidget->item(i,0)->isSelected())
			ui.indivTableWidget->item(i,0)->setCheckState(Qt::Checked);
	setCycleIndivCheckState(Qt::Checked);
	setGroupIndivCheckState(Qt::Checked);
	ui.cycleComboBox->setCurrentIndex(0);
	ui.groupComboBox->setCurrentIndex(0);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: uncheck the selected individual items.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::uncheckIndiv(){
	int nrow = ui.indivTableWidget->rowCount();
	for(int i=0; i<nrow; i++)
		if(ui.indivTableWidget->item(i,0)->isSelected())
			ui.indivTableWidget->item(i,0)->setCheckState(Qt::Unchecked);
	setCycleIndivCheckState(Qt::Unchecked);
	setGroupIndivCheckState(Qt::Unchecked);
	ui.cycleComboBox->setCurrentIndex(0);
	ui.groupComboBox->setCurrentIndex(0);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: check all individual items.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::checkAllIndiv(){
	int nrow = ui.indivTableWidget->rowCount();
	for(int i=0; i<nrow; i++)
		ui.indivTableWidget->item(i,0)->setCheckState(Qt::Checked);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: uncheck all individual items.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::checkNoIndiv(){
	int nrow = ui.indivTableWidget->rowCount();
	for(int i=0; i<nrow; i++)
		ui.indivTableWidget->item(i,0)->setCheckState(Qt::Unchecked);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int i: the index of the chosen cycle in the cycleComboBox.
	## RETURN:
	## SPECIFICATION: check the individual items corresponding to the selected cycle.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::setCycleIndivCheckState(Qt::CheckState state){
	if(ui.cycleComboBox->currentText() > 0){ // not None
		const QVector<int> & v = cycleRows[ui.cycleComboBox->currentText()];
		for (int i = 0; i<v.size(); i++) ui.indivTableWidget->item(v[i],0)->setCheckState(state);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: check the individual items corresponding to the selected group.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::setGroupIndivCheckState(Qt::CheckState state){
	if(ui.groupComboBox->currentText() > 0){ // not None
		const QVector<int> & v = groupRows[ui.groupComboBox->currentText()];
		for (int i = 0; i<v.size(); i++) ui.indivTableWidget->item(v[i],0)->setCheckState(state);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int i: the switchComboBox index.
	## RETURN:
	## SPECIFICATION: switch from cycle or group selection comboBox.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::switchBoxes(int i){
	if(i==1) {
		ui.groupComboBox->show();
		ui.cycleComboBox->hide();
	}
	else{
		ui.cycleComboBox->show();
		ui.groupComboBox->hide();
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: hide/show rows (individual) and columns (QTLs) of table,
	according to their corresponding check state in the TableViewWindow.
	-----------------------------------------------------------------------------
*/

void TableViewWindow::okClicked(){
	int nqtl = ui.qtlTableWidget->rowCount();
	int nind = ui.indivTableWidget->rowCount();

	// update column.
	for(int i=0; i<nqtl; i++){
		if(ui.qtlTableWidget->item(i,0)->checkState() == Qt::Unchecked){
			table->setColumnHidden(i + COL_FIRST_QTL,true);
		}
		else table->setColumnHidden(i + COL_FIRST_QTL,false);
	}
	// update row.
	for(int i=0; i<nind; i++){
		if(ui.indivTableWidget->item(i,0)->checkState() == Qt::Unchecked){
			table->setRowHidden(i,true);
		}
		else table->setRowHidden(i,false);
	}
	accept();
}
