TEMPLATE = app
TARGET = optimas_gui
QT += core gui
QT *= svg

# qDebug: CONFIG += console
linux-g++|linux-g++-32|linux-g++-64 { 
	INCLUDEPATH += /usr/include/qwt-qt4/ /usr/include/graphviz/
	LIBS += -lqwt-qt4 -lgvc -lgraph
	QMAKE_CXXFLAGS_RELEASE -= -g
	QMAKE_CFLAGS_RELEASE -= -g
}
macx {
	LIBS += -framework Carbon
	INCLUDEPATH += /opt/local/include/ 		/opt/local/include/qwt/ 		/opt/local/include/graphviz
	LIBS += -L/opt/local/lib -lqwt -lgvc -lgraph #-lcdt #-lpathplan
	LIBS += -L/opt/local/lib/graphviz -lgvplugin_dot_layout -lgvplugin_pango 
	LIBS += -L/opt/local/lib/pango/1.6.0/modules/pango-basic-fc.so
}
win32 { 
	INCLUDEPATH += "C:/qwt-5.2.2/src" "C:/Program Files/Graphviz 2.28/include/graphviz"
	LIBS += -L"C:/qwt-5.2.2/lib" -lqwt5 -L"C:/Program Files/Graphviz 2.28/bin" -lgvc -lgraph -lpathplan -lcdt -L"C:/Program Files/Graphviz 2.28/lib/release/lib" -lgvc
	DEFINES += QWT_DLL
}
HEADERS += headers/FillMdWindow.h 	headers/OImageViewer.h 	headers/QtlWeightWindow.h 	headers/HistoColorWindow.h 	headers/TruncationOptionWindow.h 	headers/CrossOptionWindow.h 	headers/TableViewWindow.h 	headers/FindDialog.h 	headers/OTableWidget.h 	headers/CompResultsWindow.h 	headers/ComplementationWindow.h 	headers/VisualizationWindow.h 	headers/BoxPlotItem.h 	headers/utils.h 	headers/MainWindow.h 	headers/LicenseWindow.h 	headers/ImportWindow.h 	headers/HistogramItem.h 	headers/AddListDialog.h 	headers/AboutWindow.h
SOURCES += src/FillMdWindow.cpp 	src/OImageViewer.cpp 	src/QtlWeightWindow.cpp 	src/HistoColorWindow.cpp 	src/TruncationOptionWindow.cpp 	src/CrossOptionWindow.cpp 	src/TableViewWindow.cpp 	src/FindDialog.cpp 	src/OTableWidget.cpp 	src/CompResultsWindow.cpp 	src/ComplementationWindow.cpp 	src/VisualizationWindow.cpp 	src/BoxPlotItem.cpp 	src/utils.cpp 	src/MainWindow.cpp 	src/main.cpp 	src/LicenseWindow.cpp 	src/ImportWindow.cpp 	src/HistogramItem.cpp 	src/AddListDialog.cpp 	src/AboutWindow.cpp
FORMS += ui/fillMdWindow.ui 	ui/histoColorWindow.ui 	ui/qtlWeightWindow.ui 	ui/truncationOptionWindow.ui 	ui/crossOptionWindow.ui 	ui/tableViewWindow.ui 	ui/compResultsWindow.ui 	ui/complementationWindow.ui 	ui/visualizationWindow.ui 	ui/aboutWindow.ui 	ui/importWindow.ui 	ui/licenseWindow.ui 	ui/mainWindow.ui
RESOURCES += ui/configdialog.qrc

