#include "../headers/AddListDialog.h"

using namespace std;


AddListDialog::AddListDialog(QStringList & list_selection, QWidget *parent) : QDialog(parent){

	list_selection_ptr = &list_selection;
	resize(168, 150);
	setSizeGripEnabled(false);
	addListDialogVLayout = new QVBoxLayout(this);
	addListDialogVLayout->setSpacing(3);
	addListDialogVLayout->setContentsMargins(0, 3, 0, 0);

	addListWidget = new QListWidget(this);
	addListDialogVLayout->addWidget(addListWidget);

	addListButtonBox = new QDialogButtonBox(this);
	addListButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
	addListDialogVLayout->addWidget(addListButtonBox);

	setWindowTitle(tr("Add to list"));
	addListWidget->setSortingEnabled(false);

	for(int i=0; i<(int)list_selection.size(); i++){
		addListWidget->addItem(list_selection[i]);
	}

	QListWidgetItem* lastItem = new QListWidgetItem("new list");
	//QFont* lastItemFont = new QFont();
	QFont lastItemFont;
	lastItemFont.setItalic(true);
	lastItem->setFont(lastItemFont);

	lastItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	addListWidget->addItem(lastItem);

	connect(addListWidget, SIGNAL( currentItemChanged(QListWidgetItem*,QListWidgetItem*) ), this, SLOT( onItemChanged(QListWidgetItem*,QListWidgetItem*) ) );
	connect(addListButtonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));
	connect(addListButtonBox, SIGNAL(accepted()), this, SLOT(okClicked()));
	// delete lastItemFont;
}


void AddListDialog::onItemChanged(QListWidgetItem* curItem, QListWidgetItem* prevItem){
	prevItem = NULL; // avoid warnings compilation.

	int last_row = addListWidget->count()-1; // "new list" row.

	QListWidgetItem* lastItem = addListWidget->item(last_row);

	if ( addListWidget->row(curItem) < last_row && !lastItem->font().italic() ){
		QFont afont;
		afont.setItalic(true);
		lastItem->setFont(afont);
		lastItem->setText("new list");
		return;
	}
	curItem->setFont(QFont());	// new list_name => inactive italic style.
}


void AddListDialog::okClicked(){

	int current_row = addListWidget->currentRow();
	const QString & listName = addListWidget->item(current_row)->text();
	int r=-1;
	if(current_row > (int)list_selection_ptr->size()-1){ // new list
		// Sequential search
		for (int i=0; i<(int)list_selection_ptr->size(); ++i){
			if( list_selection_ptr->at(i) == listName ) {	// this list already exist.
				r=i;
				QString mess("This list already exists.\nPlease, enter another name (double click).");
				QMessageBox::warning(this, "List exists", mess);
				addListWidget->item(current_row)->setText("new list");
				break;
			}
		}
	}
	if(r!=-1){
		QFont afont;
		afont.setItalic(true);
		addListWidget->item(current_row)->setFont(afont);
		return;
	}
	accept();
	emit okAccepted(current_row, listName);
}


ExportGraphDialog::ExportGraphDialog(QWidget *parent, Qt::WindowFlags flags) : QFileDialog(parent, flags), resLabel(NULL), resComboBox(NULL){}

ExportGraphDialog::ExportGraphDialog(QWidget* parent, const QString & caption, const QString & directory, const QString & filter):QFileDialog(parent,caption,directory,filter){

	QGridLayout* gridLayout = (QGridLayout*)layout();

	resLabel = new QLabel("Image size:", this);
	resComboBox = new QComboBox(this);
	resComboBox->addItems(QStringList()<<"800x600"<<"1024x768"<<"1200x900"<<"1400x1050"<<"1600x1200");

	int numRows = gridLayout->rowCount();
	
	//qDebug()<<layout->itemAtPosition(2,2)->widget()->objectName();
	//layout->addItem(new QWidgetItem(resComboBox),2,2);
	gridLayout->addWidget(resLabel, numRows, 0);
	gridLayout->addWidget(resComboBox, numRows, 1);
}

QString ExportGraphDialog::selectedResolution() const{
	return resComboBox->currentText();
}
