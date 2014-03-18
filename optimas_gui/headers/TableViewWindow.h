#ifndef TABLEVIEWWINDOW_H_
#define TABLEVIEWWINDOW_H_

#include "../ui_tableViewWindow.h"

#include <QtGui>
#include <QDialog>
#include <QTableWidget>
#include "utils.h"


class TableViewWindow : public QDialog {
	Q_OBJECT

	public:
		TableViewWindow(QTableWidget* t, QWidget *parent);
		~TableViewWindow();

	private slots:
		void checkQtl();
		void uncheckQtl();
		void checkAllQtl();
		void checkNoQtl();

		void checkIndiv();
		void uncheckIndiv();
		void checkAllIndiv();
		void checkNoIndiv();

		void switchBoxes(int i);

		void okClicked();

	private:
		void initWindow();
		void setItemsCheckState(const QList<QListWidgetItem *> & items, Qt::CheckState state);
		void setCycleIndivCheckState(Qt::CheckState state);
		void setGroupIndivCheckState(Qt::CheckState state);

	// attributes
		Ui::TableViewWindow ui;
		QTableWidget* table;
		QMap<QString, QVector<int> > cycleRows;	// stores a list of row indexes for each cycle.
		QMap<QString, QVector<int> > groupRows; // stores a list of row indexes for each group.
};


#endif // TABLEVIEWWINDOW_H_
