#ifndef OTABLEWIDGET_H_
#define OTABLEWIDGET_H_

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

//#include <QDebug>
#include <QtCore>
#include <QtGui>
#include <qglobal.h>
#include <QDropEvent>
#include <QTableWidget>
#include <QTableWidgetSelectionRange>
#include <QStringList>
#include <QSet>
#include <iostream>
#include "utils.h"


using namespace std;

////////////////////////////////////////////////////////////////////////
// custom tableWidget :
// Inherits QTableWidget to redefine dropEvent
//

class OTableWidget: public QTableWidget {

	Q_OBJECT

	public:
		OTableWidget();
		OTableWidget(QWidget* parent);
		OTableWidget( int rows, int columns, QWidget * parent);
		~OTableWidget();
		void enableDragAndDrop();

	protected:
		void dropEvent(QDropEvent* e);
		void keyPressEvent(QKeyEvent * e);

	private:
		void copy();
		void paste();

	public slots:
		void findNext(const QString &str, Qt::CaseSensitivity cs);
		void findPrevious(const QString &str, Qt::CaseSensitivity cs);

	signals:
		void rowInserted(OTableWidget*, int);

};


#endif /* OTABLEWIDGET_H_ */
