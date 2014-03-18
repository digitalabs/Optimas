#ifndef ABOUTWINDOW_H_
#define ABOUTWINDOW_H_

#include <QtGui>
#include <QDialog>

#include "../ui_aboutWindow.h"
#include "utils.h"

class AboutWindow : public QDialog {
	Q_OBJECT

	public:
		AboutWindow(QWidget *parent = 0);
		virtual ~AboutWindow();

	private:
			Ui::AboutWindow ui_about;

};

#endif /* ABOUTWINDOW_H_ */
