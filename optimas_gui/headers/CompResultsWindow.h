#ifndef COMPRESULTSWINDOW_H_
#define COMPRESULTSWINDOW_H_

#include <QtGui>
#include <QDialog>
#include <vector>

#include "../ui_compResultsWindow.h"
#include "utils.h"


class CompResultsWindow : public QDialog {
	Q_OBJECT

	public:
		CompResultsWindow(QWidget *parent = 0);
		virtual ~CompResultsWindow();

		void setTextResultsLabel(QString &str);

	private:
			Ui::CompResultsWindow ui_complementation_results;

};


#endif /* COMPRESULTSWINDOW_H_ */
