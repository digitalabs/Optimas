#define GLOBAL

#include <QtCore>
#include <QCoreApplication>
#include <QtGui>
#include "../headers/MainWindow.h"
#include "../headers/utils.h"

using namespace std;

int main(int argc, char *argv[])
{
	//Q_INIT_RESOURCE(configdialog);

	QApplication::setStyle("cleanlooks");

	QApplication app(argc, argv);

	MainWindow main_window;
	main_window.show();

	return app.exec();
}
