#include "../headers/OTableWidget.h"

using namespace std;

OTableWidget::OTableWidget(){}

OTableWidget::OTableWidget(QWidget* parent=0):QTableWidget(parent){
	// Keep the selection active (in blue)
	QPalette OriginalPalette = palette();
	QPalette ModifiedPalette = palette();
	ModifiedPalette.setColor(QPalette::Highlight, OriginalPalette.color(QPalette::Active, QPalette::Highlight));
	ModifiedPalette.setColor(QPalette::HighlightedText, OriginalPalette.color(QPalette::Active, QPalette::HighlightedText));
	setPalette(ModifiedPalette);
}

OTableWidget::OTableWidget( int rows, int columns, QWidget * parent = 0):QTableWidget(rows, columns, parent){}
OTableWidget::~OTableWidget(){}


void OTableWidget::enableDragAndDrop(){
	setDragDropOverwriteMode(true);
	//setDragDropMode(QAbstractItemView::InternalMove);
	//setSelectionMode(QAbstractItemView::SingleSelection);
	setDragEnabled(true);
	setAcceptDrops(true);
	//viewport()->setAcceptDrops(true);
	setDropIndicatorShown(true);
}


void OTableWidget::dropEvent(QDropEvent* event){

	if (event->source() == this) return; // no drop if dest == source.

	// sourceTableWidget: the table from which we import rows.
	OTableWidget* sourceTablewidget = (OTableWidget*)(event->source());

	int dest_col_nb = columnCount();
	int dest_row_nb = rowCount();

	bool sorting_was_enabled = isSortingEnabled();

	QSet<QString> dest_rows;				// the list of rows in the destination TableWidget (one tab-delimited QString for one row).
	for (int i = 0; i < dest_row_nb; i++){
		QStringList l_row;
		for (int j = 0; j < dest_col_nb; j++) l_row << item(i,j)->text();
		dest_rows << l_row.join("\t");
	}

	QList<QTableWidgetSelectionRange> ranges = sourceTablewidget->selectedRanges();
	// rowCount topRow bottomRow
	// columnCount leftColumn rightColumn

	for(int i=0; i<ranges.size(); i++){

		int topRow = ranges[i].topRow();
		int bottomRow = ranges[i].bottomRow();
		int leftColumn = ranges[i].leftColumn();
		int rightColumn = ranges[i].rightColumn();

		for(int r = topRow; r <= bottomRow; r++){	//each row
			QStringList l_row;
			for (int c = leftColumn; c <= rightColumn; c++){
				l_row << sourceTablewidget->item(r,c)->text();
			}
			QString row_str(l_row.join("\t"));
			if( dest_rows.find(row_str) == dest_rows.end() ){		// this row is new.
				dest_rows << row_str;								// append it to the dest row list
				setRowCount(++dest_row_nb);
				setSortingEnabled(false);
				QTableWidgetItem* newItem = NULL;
				for (int c = leftColumn; c <= rightColumn; c++){	// append at the bottom of the dest tableWidget.
					newItem = new QTableWidgetItem( *(sourceTablewidget->item(r,c)) );
					setItem( rowCount()-1, c, newItem );
				}
				if (sorting_was_enabled) setSortingEnabled(true);
				emit rowInserted(this, row(newItem));
			}
			//else qDebug()<<"nok : "<<row_str;
		}
	}
	event->accept();
	event->setDropAction(Qt::CopyAction); // Needed ?

	resizeColumnsToContents();
	resizeRowsToContents();

	//QAbstractItemView::dropEvent(event);
	//QTableView::dropEvent(event);
	QTableWidget::dropEvent(event); // Needed ?
}


void OTableWidget::findNext(const QString &str, Qt::CaseSensitivity cs){
	int row = currentRow() + 1;

	while( (rowCount() > 0) && (row < rowCount()) ) {
		QTableWidgetItem *w_item = item(row, 0);
		QString q_str = "";

		if( w_item ){
			q_str = w_item->text();
		}
		// Here CaseSensitive is replaced by whole worlds only
		if( ((cs == Qt::CaseSensitive) && (q_str == str)) || ((cs == Qt::CaseInsensitive) && (q_str.contains(str, cs))) ){
			//clearSelection();
			setCurrentCell(row, 0);
			//activateWindow();
			return;
		}
		++row;
	}
	QApplication::beep();
}


void OTableWidget::findPrevious(const QString &str, Qt::CaseSensitivity cs){
	int row = currentRow() - 1;

	while( (rowCount() > 0) && (row >= 0) ){
		QTableWidgetItem *w_item = item(row, 0);
		QString q_str = "";

		if( w_item ){
			q_str = w_item->text();
		}
		// Here CaseSensitive is replaced by whole worlds only
		if( ((cs == Qt::CaseSensitive) && (q_str == str)) || ((cs == Qt::CaseInsensitive) && (q_str.contains(str, cs))) ){
			//clearSelection();
			setCurrentCell(row, 0);
			//activateWindow();
			return;
		}
        --row;
    }
    QApplication::beep();
}


void OTableWidget::copy(){
	QItemSelectionModel * selection = selectionModel();
	QModelIndexList indexes = selection->selectedIndexes();

	if(indexes.size() < 1) return;

	// QModelIndex::operator < sorts first by row, then by column.
	// this is what we need
	qSort(indexes.begin(), indexes.end()); //// needed ?

	// You need a pair of indexes to find the row changes
	QModelIndex previous = indexes.first();
	indexes.removeFirst();
	QString selected_text;
	QModelIndex current;
	foreach(current, indexes){
		QVariant data = model()->data(previous);
		QString text = data.toString();
		// At this point `text` contains the text in one cell
		selected_text.append(text);
		// If you are at the start of the row the row number of the previous index
		// isn't the same.  Text is followed by a row separator, which is a newline.
		if (current.row() != previous.row()) selected_text.append(QLatin1Char('\n'));
		// Otherwise it's the same row, so append a column separator, which is a tab.
		else selected_text.append(QLatin1Char('\t'));
		previous = current;
	}

	// add last element
	selected_text.append(model()->data(current).toString());
	selected_text.append(QLatin1Char('\n'));
	qApp->clipboard()->setText(selected_text);
}

void OTableWidget::paste(){/*
	QString selected_text = qApp->clipboard()->text();
	QStringList cells = selected_text.split(QRegExp(QLatin1String("\\n|\\t")));

	while(!cells.empty() && cells.back().size() == 0)
		cells.pop_back(); // strip empty trailing tokens

	int rows = selected_text.count(QLatin1Char('\n'));
	int cols = cells.size() / rows;

	if(cells.size() % rows != 0){
		// error, uneven number of columns, probably bad data
		QMessageBox::critical(this, tr("Error"), tr("Invalid clipboard data, unable to perform paste operation."));
		return;
	}
	if(cols != columnCount()){
		// error, clipboard does not match current number of columns
		QMessageBox::critical(this, tr("Error"), tr("Invalid clipboard data, incorrect number of columns."));
		return;
	}

	// don't clear the grid, we want to keep any existing headers
	setRowCount(rows);
	// setColumnCount(cols);
	int cell = 0;
	for(int row=0; row < rows; ++row){
		for(int col=0; col < cols; ++col, ++cell){
			QTableWidgetItem *newItem = new QTableWidgetItem(cells[cell]);
			setItem(row, col, newItem);
		}
	}*/
}

void OTableWidget::keyPressEvent(QKeyEvent * e){
	if(e->matches(QKeySequence::Copy) )	copy();
	else if(e->matches(QKeySequence::Paste) ) paste();
	else QTableWidget::keyPressEvent(e);
}
