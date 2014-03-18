#include "../headers/LicenseWindow.h"

using namespace std;

LicenseWindow::LicenseWindow(QWidget *parent) : QDialog(parent){
	ui_license.setupUi(this);

	/* Action buttons */
	connect(ui_license.closeButtonBox, SIGNAL(accepted()), this, SLOT(close()));
}

LicenseWindow::~LicenseWindow() {

}
