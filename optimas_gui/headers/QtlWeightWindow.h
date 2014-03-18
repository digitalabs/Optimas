#ifndef QTLWEIGHTWINDOW_H_
#define QTLWEIGHTWINDOW_H_

#include "../ui_qtlWeightWindow.h"

#include <QtGui>
#include <QDialog>
#include <QTableWidget>
#include "utils.h"

class QtlWeightWindow : public QDialog {
	Q_OBJECT

	public:
		QtlWeightWindow(QWidget *parent);
		QtlWeightWindow(std::vector<double>& vect_qtl_weight, QWidget *parent);
		~QtlWeightWindow();

	private slots:
		void setQtlWeight();
		void selectAllQtl();
		void selectNoQtl();

		void resetDefaultWeight();
		void applyWeight();

	private:
		void initWindow();

		// attributes
		Ui::QtlWeightWindow ui;
		std::vector<double>* m_vect_ptr;
};

#endif /* QTLWEIGHTWINDOW_H_ */
