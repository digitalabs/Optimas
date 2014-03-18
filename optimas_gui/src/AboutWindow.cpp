#include "../headers/AboutWindow.h"

using namespace std;

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent){
	ui_about.setupUi(this);

	/* Action buttons */
	connect(ui_about.closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

AboutWindow::~AboutWindow() {

}
