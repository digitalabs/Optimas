#ifndef LICENSEWINDOW_H_
#define LICENSEWINDOW_H_

#include <QtGui>
#include <QDialog>

#include "../ui_licenseWindow.h"
#include "utils.h"

class LicenseWindow : public QDialog {
	Q_OBJECT

	public:
		LicenseWindow(QWidget *parent = 0);
		virtual ~LicenseWindow();

	private:
			Ui::LicenseWindow ui_license;

};

#endif /* LICENSEWINDOW_H_ */
