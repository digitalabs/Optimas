#include "../headers/MainWindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
	ui.setupUi(this);

	m_import = NULL;
	m_fill_md = NULL;
	m_about = NULL;
	m_license = NULL;
	m_complementation = NULL;
	m_complementation_results = NULL;
	m_findDialog = NULL;
	m_cross_option = NULL;
	m_truncation_option = NULL;

	m_process = NULL; // To run the external "optimas" program

	m_addListAction = m_deleteLeftListSelectionAction = m_deleteRightListSelectionAction = m_deleteLeftListCrossesAction = m_deleteRightListCrossesAction = NULL;
	m_addListDialog = NULL;

	setDataPath();
	m_lastOutput = m_lastInput = m_lastGraphSave = QDir::homePath();

	m_process = new QProcess(this); // to launch optimas.exe

	connect(m_process, SIGNAL( error(QProcess::ProcessError) ), this, SLOT( onProcessError(QProcess::ProcessError) ) );
	connect(m_process, SIGNAL( readyReadStandardOutput() ), this, SLOT( logOutput() ) );
	connect(m_process, SIGNAL( readyReadStandardError() ), this, SLOT( logError() ) );
	connect(m_process, SIGNAL( finished(int, QProcess::ExitStatus) ), this, SLOT( runOptiMASFinished(int, QProcess::ExitStatus) ) );

	// Action of the QMenu
	connect(ui.actionImportData, SIGNAL(triggered()), this, SLOT(showImportWindow()));
	connect(ui.actionReloadData, SIGNAL(triggered()), this, SLOT(reloadData()));
	connect(ui.actionBiparental, SIGNAL(triggered()), this, SLOT(reloadBiparentalData()));
	connect(ui.actionMultiparental, SIGNAL(triggered()), this, SLOT(reloadMultiparentalData()));
	connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionVisualization, SIGNAL(triggered()), this, SLOT(showVisualizationWindow()));
	connect(ui.actionSaveAll, SIGNAL(triggered()), this, SLOT(saveAllLists()));
	connect(ui.actionClearAll, SIGNAL(triggered()), this, SLOT(clearAll()));
	connect(ui.actionFillMd, SIGNAL(triggered()), this, SLOT(showFillMdWindow()));
	connect(ui.actionAboutOptiMAS, SIGNAL(triggered()), this, SLOT(showAboutWindow()));
	connect(ui.actionLicense, SIGNAL(triggered()), this, SLOT(showLicenseWindow()));
	connect(ui.actionHelpContents, SIGNAL(triggered()), this, SLOT(showHelpWindow()));


	// Click on the step list widget: the page is update (title + interface)
	connect(ui.stepsListWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));

	createActions(); // Creation of the "Action" buttons for the right click in a table of selection/crosses
	createContextMenu(); // The "Action" button are added to the context menu of the interface
	initVisualizationParameters();
	initTruncationOption();
	initComplementationParameters();
	initCrossOption();


	// Score page connection buttons
	// Table of molecular scores with weights
	connect(ui.findPushButton2, SIGNAL(clicked()), this, SLOT(showFindWindow()));
	connect(ui.viewPushButton2, SIGNAL(clicked()), this, SLOT(showTableViewWindow()));
	connect(ui.weightPushButton2, SIGNAL(clicked()), this, SLOT(showQtlWeightWindow()));
	////connect(ui.scoreTableWidget, SIGNAL(cellPressed(int, int)), this, SLOT(displayCellInfoScore(int, int)));
	connect(ui.scoreTableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(displayCellInfoScore(int, int)));

	// Swap table view: 1 or 2 tables of selected individuals
	connect(ui.minusToolButton2, SIGNAL(clicked()), this, SLOT(showOneTableScore()));
	connect(ui.plusToolButton2, SIGNAL(clicked()), this, SLOT(showTwoTableScore()));

	// Customize left graph histograms colors (cycles).
	connect(ui.colorOptionToolButton2, SIGNAL(clicked()), this, SLOT(showHistoColorWindow()));

	// Graphs are saved
	connect(ui.graphLeftSavePushButton2, SIGNAL(clicked()), this, SLOT(saveGraphLeftScorePage()));
	connect(ui.graphRightSavePushButton2, SIGNAL(clicked()), this, SLOT(saveGraphRightScorePage()));


	// Selection page connection buttons
	// The list of selected individuals is updated regarding the list selected by the user
	connect(ui.leftListComboBox6, SIGNAL(currentIndexChanged(int)), this, SLOT(displayLeftListSelection(int)));
	connect(ui.rightListComboBox6, SIGNAL(currentIndexChanged(int)), this, SLOT(displayRightListSelection(int)));

	// Genotype information display on each cell of the table
	////connect(ui.leftListTableWidget6, SIGNAL(cellPressed(int, int)), this, SLOT(displayCellInfoSelectionLeft(int, int)));
	////connect(ui.rightListTableWidget6, SIGNAL(cellPressed(int, int)), this, SLOT(displayCellInfoSelectionRight(int, int)));
	connect(ui.leftListTableWidget6, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(displayCellInfoSelectionLeft(int, int)));
	connect(ui.rightListTableWidget6, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(displayCellInfoSelectionRight(int, int)));

	// enable the drag and drop mode for the right and left selection lists (drag and drop from a list to another)
	ui.rightListTableWidget6->enableDragAndDrop();
	ui.leftListTableWidget6->enableDragAndDrop();

	ui.pedigreeScrollAreaWidgetContents6->setStyleSheet("background-color: white");

	connect(ui.leftListTableWidget6, SIGNAL(rowInserted(OTableWidget*, int)), this, SLOT(updateList(OTableWidget*, int)));
	connect(ui.rightListTableWidget6, SIGNAL(rowInserted(OTableWidget*, int)), this, SLOT(updateList(OTableWidget*, int)));

	// The current lists of selected individuals become editable
	connect(ui.selectionListWidget6, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(editSelectionItemList(QListWidgetItem *)));

	// The name of the current list is updated after user changes
	connect(ui.selectionListWidget6, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(itemListSelectionChanged(QListWidgetItem *)));

	// The current selected list (clicked on the QListWidgetItem) is displayed on the left table
	connect(ui.selectionListWidget6, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(leftListSelectionChanged(QListWidgetItem *)));

	// Truncation selection: display the Nsel best individuals (regarding a chosen criterion) for the selection
	connect(ui.truncOptionToolButton6, SIGNAL(clicked()), this, SLOT(showTruncationOptionWindow()));
	connect(ui.truncRunPushButton6, SIGNAL(clicked()), this, SLOT(runTruncationSelection()));

	// Complementation Selection strategy
	connect(ui.compOptionToolButton6, SIGNAL(clicked()), this, SLOT(showComplementationWindow()));
	connect(ui.compRunPushButton6, SIGNAL(clicked()), this, SLOT(runComplementationSelection()));

	// Lists are saved / appended / added / removed reseted
	connect(ui.selectionSavePushButton6, SIGNAL(clicked()), this, SLOT(saveListSelection()));
	connect(ui.selectionAddPushButton6, SIGNAL(clicked()), this, SLOT(addListSelection()));
	connect(ui.selectionRemovePushButton6, SIGNAL(clicked()), this, SLOT(removeListSelection()));
	connect(ui.selectionResetPushButton6, SIGNAL(clicked()), this, SLOT(resetListSelection()));

	// Swap table view: 1 or 2 tables of selected individuals
	connect(ui.minusToolButton6, SIGNAL(clicked()), this, SLOT(showOneTableSelection()));
	connect(ui.plusToolButton6, SIGNAL(clicked()), this, SLOT(showTwoTableSelection()));

	// Graphs are saved
	connect(ui.graphLeftSavePushButton6, SIGNAL(clicked()), this, SLOT(saveGraphLeftSelectionPage()));
	connect(ui.graphRightSavePushButton6, SIGNAL(clicked()), this, SLOT(saveGraphRightSelectionPage()));

	/*//// to be continued...
	connect(ui.zoomInPushButton,		SIGNAL(clicked()), this, SLOT(zoomInPedigree()));
	connect(ui.zoomOutPushButton,		SIGNAL(clicked()), this, SLOT(zoomOutPedigree()));
	connect(ui.zoomOriginalPushButton,	SIGNAL(clicked()), this, SLOT(zoomOriginalPedigree()));
	connect(ui.zoomFitPushButton,		SIGNAL(clicked()), this, SLOT(zoomFitPedigree()));
	*/

	// Pedigree: display the pedigree of the selected individuals
	connect(ui.pedigreeGeneratePushButton6, SIGNAL(clicked()), this, SLOT(runPedigree()));

	// Pedigree: save graph
	connect(ui.pedigreeSavePushButton6, SIGNAL(clicked()), this, SLOT(savePedigree()));


	// Crossing page connection buttons
	// The list of selected individuals is updated regarding the list selected by the user
	connect(ui.leftListComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(displayLeftListCrosses(int)));
	connect(ui.rightListComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(displayRightListCrosses(int)));

	// Genotype information display on each cell of the table
	////connect(ui.leftListTableWidget4, SIGNAL(cellPressed(int, int)), this, SLOT(displayCellInfoCrossesLeft(int, int)));
	////connect(ui.rightListTableWidget4, SIGNAL(cellPressed(int, int)), this, SLOT(displayCellInfoCrossesRight(int, int)));
	connect(ui.leftListTableWidget4, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(displayCellInfoCrossesLeft(int, int)));
	connect(ui.rightListTableWidget4, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(displayCellInfoCrossesRight(int, int)));

	// The current list of crosses become editable
	connect(ui.crossListWidget4, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(editCrossItemList(QListWidgetItem *)));

	// The name of the current list is updated after user changes
	connect(ui.crossListWidget4, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(itemListCrossChanged(QListWidgetItem *)));

	// The current selected list (clicked on the QListWidgetItem) is displayed on the left table
	connect(ui.crossListWidget4, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(leftListCrossChanged(QListWidgetItem *)));

	// The maximum number of crosses/contribution is updated regarding the selected list of selection (1 and 2 lists)
	connect(ui.selectionListComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(maxCrossContribChanged(int)));
	connect(ui.selectionList2ComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(maxCrossContrib2ListsChanged(int)));

	// The second list of selection is enabled or not depending of the checkbox state
	connect(ui.listSelection2CheckBox4, SIGNAL(stateChanged(int)), this, SLOT(checkBoxListsSelectionChanged(int)));

	// Lists are saved / appended / added / removed reseted
	connect(ui.crossSavePushButton4, SIGNAL(clicked()), this, SLOT(saveListCrosses()));
	connect(ui.crossAddPushButton4, SIGNAL(clicked()), this, SLOT(addListCrosses()));
	connect(ui.crossRemovePushButton4, SIGNAL(clicked()), this, SLOT(removeListCrosses()));
	connect(ui.crossResetPushButton4, SIGNAL(clicked()), this, SLOT(resetListCrosses()));

	// Swap table view: 1 or 2 tables of crossed individuals
	connect(ui.minusToolButton4, SIGNAL(clicked()), this, SLOT(showOneTableCrosses()));
	connect(ui.plusToolButton4, SIGNAL(clicked()), this, SLOT(showTwoTableCrosses()));

	// Graphs are saved
	connect(ui.graphLeftSavePushButton4, SIGNAL(clicked()), this, SLOT(saveGraphLeftCrossesPage()));
	connect(ui.graphRightSavePushButton4, SIGNAL(clicked()), this, SLOT(saveGraphRightCrossesPage()));

	// Show parameters and options settings for crosses
	connect(ui.crossesOptionToolButton4, SIGNAL(clicked()), this, SLOT(showCrossOptionWindow()));

	// Step 3 (crossing schemes): run the "crossing method" program
	connect(ui.runPushButton4, SIGNAL(clicked()), this, SLOT(runCrosses()));
}


MainWindow::~MainWindow() {

}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the color settings window for the histograms according to the cycle.
	-----------------------------------------------------------------------------
*/

void MainWindow::showHistoColorWindow(){

	QMap<QString, QColor> map_cycleColor; // key = the cycle id, value = the cycle color.
	QMap<QString, QColor>::const_iterator it;

	// QwtPlotItemList is a typedef QList<QwtPlotItem *> QwtPlotItemList.
	// QwtPlot::itemList() return a const QwtPlotItemList &,
	// => pointers are constant but the QwtPlotItem objects pointed are not constant => their color can be changed.
	const QwtPlotItemList & histoList = ui.graphLeftQwtPlot2->itemList();
	for (int i=0; i<histoList.size(); ++i){
		HistogramItem& histoItem = *(HistogramItem*)histoList[i];
		if ( histoItem.rtti() == QwtPlotItem::Rtti_PlotHistogram) // skip the gridItem.
			map_cycleColor.insert( histoItem.title().text(), histoItem.color() );
	}

	HistoColorWindow colorWindow(map_cycleColor, m_vect_generation, this); // we must preserve the order set in m_vect_generation.

	if(colorWindow.exec() == QDialog::Accepted){ // apply button has been clicked.
		for (int i=0; i<histoList.size(); ++i){
			HistogramItem& histoItem = *(HistogramItem*)histoList[i];
			if ( histoItem.rtti() == QwtPlotItem::Rtti_PlotHistogram) // skip the gridItem.
				histoItem.setColor( map_cycleColor[histoItem.title().text()] );
		}
		// Update the legend.
		QString legend(ui.graphLeftQwtPlot2->title().text());
		// example pattern : color:#ff0000;">♦ IL
		// => we must replace #ff0000 by the new color value in this pattern
		for(it=map_cycleColor.begin(); it!=map_cycleColor.end(); ++it){
			QRegExp rx( "(color:)(#[0-9a-zA-Z]{6})(;\">. )("+it.key()+")" );
			legend.replace( rx, "\\1"+it.value().name()+"\\3\\4");
		}
		ui.graphLeftQwtPlot2->setTitle(legend);
		ui.graphLeftQwtPlot2->replot();
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the visualization menu window. Colors are attributed
	to cells regarding a specific cut-off based on the molecular score.
	-----------------------------------------------------------------------------
*/

void MainWindow::showVisualizationWindow(){

	if (ui.scoreTableWidget != NULL && ui.scoreTableWidget->rowCount() > 0) {
		VisualizationWindow visWin(m_vect_color, m_vect_cut_off, this);
		if(visWin.exec() == QDialog::Accepted){ // Apply button has been clicked
			applyVisualization();
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the qtl weight window.
	Assign a weight to one or more QTLs and recompute the weight score for each individual.
	-----------------------------------------------------------------------------
*/

void MainWindow::showQtlWeightWindow(){
	QtlWeightWindow qtlWeightWin(m_vect_qtl_weight, this);
	if(qtlWeightWin.exec() == QDialog::Accepted){ // Apply button has been clicked
		// update weight scores
		runComputeWeight();
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the  table view window (show/hide some individuals rows and/or QTL columns).
	-----------------------------------------------------------------------------
*/

void MainWindow::showTableViewWindow(){
	
	TableViewWindow tableView(ui.scoreTableWidget, this);
	if(tableView.exec() == QDialog::Rejected){
		//cerr<<"oups"<<endl;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the import menu window (input files / parameters).
	-----------------------------------------------------------------------------
*/

void MainWindow::showImportWindow(){
	if (m_import != NULL) { delete m_import; m_import  = NULL; }
	m_import = new ImportWindow(this, m_lastInput, m_lastOutput);
	m_import->show();
	m_import->activateWindow();

	// Run OptiMAS algorithm
	connect(m_import->getRunPushButton(), SIGNAL(clicked()), this, SLOT(runProcess()));

	// Run Examples
	connect(m_import->getExampleRunPushButton(), SIGNAL(clicked()), this, SLOT(runExamples()));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the import menu window (input files / parameters).
	-----------------------------------------------------------------------------
*/

void MainWindow::showFillMdWindow(){
	if (m_fill_md != NULL) { delete m_fill_md; m_fill_md  = NULL; }
	m_fill_md = new FillMdWindow(this);
	m_fill_md->show();
	m_fill_md->activateWindow();

	// Run the FillMd@Mks tool
	connect(m_fill_md->getRunPushButton(), SIGNAL(clicked()), this, SLOT(runFillMd()));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the information window regarding OptiMAS.
	-----------------------------------------------------------------------------
*/

void MainWindow::showAboutWindow(){
	if (m_about != NULL) { delete m_about;	m_about = NULL; }
	m_about = new AboutWindow(this);
	m_about->show();
	m_about->activateWindow();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the GSL license.
	-----------------------------------------------------------------------------
*/

void MainWindow::showLicenseWindow(){
	if (m_license != NULL) { delete m_license; m_license = NULL; }
	m_license = new LicenseWindow(this);
	m_license->show();
	m_license->activateWindow();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the online help website.
	-----------------------------------------------------------------------------
*/

void MainWindow::showHelpWindow(){
	QString dir_path = "";
	QString app_dir_path(QApplication::applicationDirPath());

	#if defined(__WINDOWS__) || defined(__APPLE__)
		dir_path = QString("file:///%1/%2").arg(QApplication::applicationDirPath()).arg("/website/index.html");
	#else //#elif defined(__linux__)
		if(app_dir_path == "/usr/local/bin") dir_path = QString::fromUtf8("/usr/share/OptiMAS/website/index.html");
		else dir_path = QDir::homePath() + QString::fromUtf8("/.OptiMAS/website/index.html");
	#endif

	QDesktopServices::openUrl(QUrl(dir_path, QUrl::TolerantMode));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the find window to follow a specific individual.
	-----------------------------------------------------------------------------
*/

void MainWindow::showFindWindow(){
	if (m_findDialog != NULL) { delete m_findDialog; m_findDialog = NULL; }
	m_findDialog = new FindDialog(this);
    m_findDialog->show();
    m_findDialog->activateWindow();

    connect(m_findDialog, SIGNAL(findNext(const QString &, Qt::CaseSensitivity)), ui.scoreTableWidget, SLOT(findNext(const QString &, Qt::CaseSensitivity)));
    connect(m_findDialog, SIGNAL(findPrevious(const QString &, Qt::CaseSensitivity)), ui.scoreTableWidget, SLOT(findPrevious(const QString &, Qt::CaseSensitivity)));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the truncation selection option window.
	-----------------------------------------------------------------------------
*/

void MainWindow::showTruncationOptionWindow(){
	if (m_truncation_option != NULL) { delete m_truncation_option; m_truncation_option = NULL; }
	m_truncation_option = new TruncationOptionWindow(m_vect_truncation_option, m_map_gene_scores.keys(), m_map_group.keys(), this);
	m_truncation_option->show();
	m_truncation_option->activateWindow();

	connect(m_truncation_option->getApplyButton(), SIGNAL(clicked()), this, SLOT(applyTruncationOption()));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the complementation window to set parameters.
	-----------------------------------------------------------------------------
*/

void MainWindow::showComplementationWindow(){
	if (m_complementation != NULL) { delete m_complementation; m_complementation = NULL; }
	m_complementation = new ComplementationWindow(m_vect_complementation_params, m_map_gene_scores.keys(), m_map_group.keys(), this);
	m_complementation->show();
	m_complementation->activateWindow();

	connect(m_complementation->getApplyButton(), SIGNAL(clicked()), this, SLOT(applyComplementation()));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Display the cross option window to set parameters.
	-----------------------------------------------------------------------------
*/

void MainWindow::showCrossOptionWindow(){
	if (m_cross_option != NULL) { delete m_cross_option; m_cross_option = NULL; }
	m_cross_option = new CrossOptionWindow(m_vect_cross_option, this);
	m_cross_option->show();
	m_cross_option->activateWindow();

	connect(m_cross_option->getApplyButton(), SIGNAL(clicked()), this, SLOT(applyCrossOption()));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ string message: the message that will appear in the interface.
	## RETURN:
	## SPECIFICATION: An error message is sent (displayed in the interface).
	-----------------------------------------------------------------------------
*/

void MainWindow::showCustomErrorMessage(const QString & message){
	QMessageBox::warning(this, "Input/Output files", message);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QListWidgetItem *current : the current widget item (page)
	@ QListWidgetItem *previous : the previous page
	## RETURN:
	## SPECIFICATION: Change the current page view and title (by clicking)
	-----------------------------------------------------------------------------
*/

void MainWindow::changePage(QListWidgetItem *current, QListWidgetItem *previous) {
	if (!current){
		current = previous;
	}
	ui.stackedWidget->setCurrentIndex(ui.stepsListWidget->row(current));

	// Computation of genotypic probabilities - Estimation of genetic values
	if(ui.stackedWidget->currentIndex() == 0){
		ui.titleLabel->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
		"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
		"p, li { white-space: pre-wrap; }\n"
		"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
		"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:"TITLE_SIZE"; text-decoration: underline; \"></span><span style=\" font-size:"TITLE_SIZE"; text-decoration: underline;\">Computation of genotypic probabilities - Estimation of genetic values</span></p></body></html>", 0, QApplication::UnicodeUTF8));
	}
	// Selection of individuals
	else if(ui.stackedWidget->currentIndex() == 1){
		ui.titleLabel->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
		"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
		"p, li { white-space: pre-wrap; }\n"
		"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
		"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:"TITLE_SIZE"; text-decoration: underline; \"></span><span style=\" font-size:"TITLE_SIZE"; text-decoration: underline;\">Selection of individuals</span></p></body></html>", 0, QApplication::UnicodeUTF8));
	}
	// Identification of crosses to be made among selected individuals
	else if(ui.stackedWidget->currentIndex() == 2){
		ui.titleLabel->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
		"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
		"p, li { white-space: pre-wrap; }\n"
		"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
		"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:"TITLE_SIZE"; text-decoration: underline; \"></span><span style=\" font-size:"TITLE_SIZE"; text-decoration: underline;\">Identification of crosses to be made among selected individuals</span></p></body></html>", 0, QApplication::UnicodeUTF8));
	}
	// Simulation
	else if(ui.stackedWidget->currentIndex() == 3){
		ui.titleLabel->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
		"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
		"p, li { white-space: pre-wrap; }\n"
		"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
		"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:"TITLE_SIZE"; text-decoration: underline; \"></span><span style=\" font-size:"TITLE_SIZE"; text-decoration: underline;\">Simulation</span></p></body></html>", 0, QApplication::UnicodeUTF8));
	}
	// Tools
	else if(ui.stackedWidget->currentIndex() == 4){
		ui.titleLabel->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
		"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
		"p, li { white-space: pre-wrap; }\n"
		"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
		"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:"TITLE_SIZE"; text-decoration: underline; \"></span><span style=\" font-size:"TITLE_SIZE"; text-decoration: underline;\">Tools</span></p></body></html>", 0, QApplication::UnicodeUTF8));
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: after the user chooses its viewing parameters, colors are
	applied to tables (step 1, 2, 3) of molecular scores. The No.(+/+), No.(-/-),
	No.(+/-), No.(?) are updated in each map (scores/crosses).
	-----------------------------------------------------------------------------
*/

void MainWindow::applyVisualization(){
	// m_vect_color and m_vect_cut_off have been updated via VisualizationWindow::onApply() slot.

	// Apply colors to the score page table
	int nb_rows = ui.scoreTableWidget->rowCount();
	int nb_columns = ui.scoreTableWidget->columnCount();
	QString ind_name("");

	// Loop on individuals (Step 1 table)
	for(int i=0; i<nb_rows; i++){
		vector<int> vect_nb_geno(4, 0);
		ind_name = ((QTableWidgetItem *)ui.scoreTableWidget->item(i, 0))->text();
		// Loop on QTL
		for(int j=COL_FIRST_QTL; j<nb_columns; j++){
			// Homozygous favorable
			if( m_homo_fav[ind_name][j - COL_FIRST_QTL + 1][0].toDouble() >= m_vect_cut_off[0] ){
				if( m_vect_color[0].value() != ui.findPushButton2->palette().button().color().value() ){
					((QTableWidgetItem *)ui.scoreTableWidget->item(i, j))->setBackground(QBrush(m_vect_color[0]));
				}
				else{
					((QTableWidgetItem *)ui.scoreTableWidget->item(i, j))->setBackground(((QTableWidgetItem *)ui.scoreTableWidget->item(i, 1))->background());
				}
				vect_nb_geno[0]++;
			}
			// Homozygous unfavorable
			else if( m_homo_unfav[ind_name][j - COL_FIRST_QTL + 1][0].toDouble() >= m_vect_cut_off[1] ){
				if( m_vect_color[1].value() != ui.findPushButton2->palette().button().color().value() ){
					((QTableWidgetItem *)ui.scoreTableWidget->item(i, j))->setBackground(QBrush(m_vect_color[1]));
				}
				else{
					((QTableWidgetItem *)ui.scoreTableWidget->item(i, j))->setBackground(((QTableWidgetItem *)ui.scoreTableWidget->item(i, 1))->background());
				}
				vect_nb_geno[1]++;
			}
			// Hetero
			else if( m_hetero[ind_name][j - COL_FIRST_QTL + 1][0].toDouble() >= m_vect_cut_off[2] ){
				if( m_vect_color[2].value() != ui.findPushButton2->palette().button().color().value() ){
					((QTableWidgetItem *)ui.scoreTableWidget->item(i, j))->setBackground(QBrush(m_vect_color[2]));
				}
				else{
					((QTableWidgetItem *)ui.scoreTableWidget->item(i, j))->setBackground(((QTableWidgetItem *)ui.scoreTableWidget->item(i, 1))->background());
				}
				vect_nb_geno[2]++;
			}
			// ? uncertain genotypes
			else{
				if( m_vect_color[3].value() != ui.findPushButton2->palette().button().color().value() ){
					((QTableWidgetItem *)ui.scoreTableWidget->item(i, j))->setBackground(QBrush(m_vect_color[3]));
				}
				else{
					((QTableWidgetItem *)ui.scoreTableWidget->item(i, j))->setBackground(((QTableWidgetItem *)ui.scoreTableWidget->item(i, 1))->background());
				}
				vect_nb_geno[3]++;
			}
		}
		// Update No(+/+), No(-/-), No(+/-), No(?)
		for(int j=COL_NO_FAV; j<COL_FIRST_QTL; j++){
			// Update map scores and table
			((QTableWidgetItem *)ui.scoreTableWidget->item(i, j))->setData(0, vect_nb_geno[j - COL_NO_FAV]);
			m_map_scores[ind_name][j - 1] = QString(" ").setNum(vect_nb_geno[j - COL_NO_FAV]);
		}
	}

	// Update on the lists of crosses
	for(int i=0; i<m_vect_map_crosses.size(); i++){
		QMap <QString, QStringList> & map_crosses_list_i = *(m_vect_map_crosses[i]);
		// Loop on new virtual individuals of the list
		for( QMap <QString, QStringList>::iterator iter_list = map_crosses_list_i.begin(); iter_list != map_crosses_list_i.end(); ++iter_list ){
			const QString & ind = iter_list.key(); // Individual of the list
			const QStringList & ind_data = iter_list.value(); // Data for each individual
			vector<int> vect_nb_geno(4, 0); // No.(+/+) / No.(-/-) / No.(+/-) / No.(?)

			// Loop on QTLs
			for(int j=(COL_FIRST_QTL - 3); j<ind_data.size(); j++){

				// Visualization of genotypes via colors for each QTL
				double score_homo_fav = (m_homo_fav[ind_data[COL_P1 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_fav[ind_data[COL_P2 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
				double score_homo_unfav = (m_homo_unfav[ind_data[COL_P1 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_unfav[ind_data[COL_P2 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
				double score_hetero = (m_hetero[ind_data[COL_P1 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_hetero[ind_data[COL_P2 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;

				// Homozygous favorable
				if( score_homo_fav >= m_vect_cut_off[0] ){
					vect_nb_geno[0]++;
				}
				// Homozygous unfavorable
				else if( score_homo_unfav >= m_vect_cut_off[1] ){
					vect_nb_geno[1]++;
				}
				// Hetero
				else if( score_hetero >= m_vect_cut_off[2] ){
					vect_nb_geno[2]++;
				}
				// ? uncertain genotypes
				else{
					vect_nb_geno[3]++;
				}
			}

			// Update No(+/+), No(-/-), No(+/-), No(?)
			for(int k=(COL_NO_FAV - 2); k<=(COL_NO_UNCERTAIN - 2); k++){
				// Update map
				map_crosses_list_i[ind][k - 1] = QString(" ").setNum(vect_nb_geno[k - COL_NO_FAV + 2]);
			}
		}
	}

	// The Tables in the selection/intermating page are updated
	displayListSelection(ui.leftListTableWidget6, ui.leftListComboBox6->currentIndex());
	displayListSelection(ui.rightListTableWidget6, ui.rightListComboBox6->currentIndex());
	displayListCrosses(ui.leftListTableWidget4, ui.leftListComboBox4->currentIndex());
	displayListCrosses(ui.rightListTableWidget4, ui.rightListComboBox4->currentIndex());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: parameters chosen by the user are stored.
	-----------------------------------------------------------------------------
*/

void MainWindow::applyTruncationOption(){
	// New parameters information are stored
	if( m_truncation_option->isCycleButtonSelected() ){
		m_vect_truncation_option[0] = 0;
		m_vect_truncation_option[1] = m_truncation_option->getCycleIndex();
	}
	else{
		m_vect_truncation_option[0] = 1;
		m_vect_truncation_option[1] = m_truncation_option->getGroupIndex();
	}

	m_truncation_option->close();
}



/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: after the user chooses its viewing parameters, colors are
	applied to the table of molecular scores.
	-----------------------------------------------------------------------------
*/

void MainWindow::applyComplementation(){
	// New parameters information are stored
	m_vect_complementation_params[0] = m_complementation->getTetaValue();
	m_vect_complementation_params[1] = m_complementation->getNtValue();
	m_vect_complementation_params[2] = m_complementation->getMsMinValue();
	m_vect_complementation_params[3] = m_complementation->getNmaxValue();

	// Cycle/Group option
	if( m_complementation->isCycleButtonSelected() ){
		m_vect_complementation_params[4] = 0;
		m_vect_complementation_params[5] = m_complementation->getCycleIndex();
	}
	else{
		m_vect_complementation_params[4] = 1;
		m_vect_complementation_params[5] = m_complementation->getGroupIndex();
	}

	m_complementation->close();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: parameters chosen by the user are stored.
	-----------------------------------------------------------------------------
*/

void MainWindow::applyCrossOption(){
	// New parameters information are stored
	if( m_cross_option->isMaxButtonChecked() ){
		m_vect_cross_option[0] = 1;
	}
	else{
		m_vect_cross_option[0] = 0;
	}

	if( m_cross_option->isUnlimitedButtonChecked() ){
		m_vect_cross_option[1] = 1;
	}
	else{
		m_vect_cross_option[1] = 0;
	}

	m_vect_cross_option[2] = m_cross_option->getNbCrosses();
	m_vect_cross_option[3] = m_cross_option->getIndContrib();
	m_vect_cross_option[4] = m_cross_option->getCriterionCurrentIndex();
	m_vect_cross_option[5] = m_cross_option->getMethodCurrentIndex();

	m_cross_option->close();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: run OptiMAS algorithm with parameters/options coming from
	the import data window or Stop it if it is still running (user choice).
	-----------------------------------------------------------------------------
*/

void MainWindow::runProcess(){
	// Run
	if( m_import->getTextRunPushButton().toStdString().compare("Run") == 0 ){
		runOptiMAS();
	}
	// Stop
	else{
		m_process_killed = true;
		m_process->kill();
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: run OptiMAS algorithm with parameters/options coming from
	the import data window.
	-----------------------------------------------------------------------------
*/

void MainWindow::runOptiMAS(){

	m_process_killed = false;

	// If the 3 input/output files are loaded: the program starts
	if( m_import->isOK() ){

		m_lastInput = m_import->getMapLineEdit().section(QRegExp("[/\\\\]"), 0,-2); // sep is '/' (unix like OS) or '\' (win OS)
		if (m_lastInput.isEmpty()) m_lastInput = "/";
		m_lastOutput = m_import->getResultsLineEdit();

		m_import->setRunPushButton("Stop");

		// Buttons are disabled when the program is running
		m_import->setImportDisabled(true);

		m_date = QDateTime::currentDateTime();

		QString new_res = m_import->getResultsLineEdit() + m_date.toString("dd_MM_yyyy_HH_mm_ss"), program = "";
		QString app_dir_path(QCoreApplication::applicationDirPath());
		m_file_res = new_res;
		m_import->setResultsLineEdit(new_res);

		#if defined(__WINDOWS__)
			program = QCoreApplication::applicationDirPath() + "\\software\\optimas.exe";
		#elif defined(__APPLE__)
			program = QCoreApplication::applicationDirPath() + "/software/optimas";
		#else // #elif defined(__linux__)
			program = app_dir_path + QString::fromUtf8("/optimas");
		#endif

		QStringList arguments;
		QString numQtl, verbose = "verb";
		QString haploCutoff = m_import->getTextHaplo();
		QString gamCutoff = m_import->getTextGam();

		// All QTL selected
		if( m_import->isAllRadioButtonChecked() ){
			numQtl = "0";
		}
		// Only one specific QTL is selected
		else{
			numQtl = QString(" ").setNum(m_import->getNumQtl());
		}

		// Verbose mode enabled
		if( m_import->isVerboseChecked() ){
			arguments << m_import->getGenoLineEdit() << m_import->getMapLineEdit() << haploCutoff.replace(',','.') << gamCutoff.replace(',','.') << new_res << numQtl << verbose;
		}
		else{
			arguments << m_import->getGenoLineEdit() << m_import->getMapLineEdit() << haploCutoff.replace(',','.') << gamCutoff.replace(',','.') << new_res << numQtl;
		}

		if (m_process != NULL) {
			m_process->start(program, arguments);
		}
		else{
			cerr << "error: m_process error";
		}
	}
	// Some input files are missing
	else{
		m_import->showCustomErrorMessage("Please select/load every Input/Output files before running the program !!");
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Read the outputs when the program "Sam" is running in order
	to have an estimation of the run progression.
	-----------------------------------------------------------------------------
*/

void MainWindow::logOutput() {
	QByteArray bytes = m_process->readAllStandardOutput();
	QStringList lines = QString(bytes).split(" ");

	m_import->setNumQtlProgressBar(lines[2]);
	m_import->setNumindivProgressBar(lines[5]);
	m_import->setValueProgressBar(atoi(lines[7].toStdString().c_str()));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Read the standard error when the program "optimas" is running.
	-----------------------------------------------------------------------------
*/

void MainWindow::logError() {
	QString error_message(m_process->readAllStandardError());
	// the message may contain several lines !!
	QStringList lines = error_message.split("\n", QString::SkipEmptyParts);
	QString mess("");

	for(int i=0; i<lines.size(); i++){
		// Total number of QTL/Id/Mks (not really an error)
		if( lines[i].startsWith(".:") ){
			QStringList line = lines[i].split(" ");
			m_import->setTotalQtlMksValues("QTL #" + line[2] + "\nMarkers #" + line[8] + "\nIndividuals #" + line[5]);
		}
		else{
			mess += (lines[i] + "\n");
		}
	}
	if (!mess.isEmpty()) showCustomErrorMessage(mess);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QProcess::ProcessError error: The type of the error sent by the signal QProcess::error()
	## RETURN:
	## SPECIFICATION: An appropriate error message is displayed.
	-----------------------------------------------------------------------------
*/

void MainWindow::onProcessError(QProcess::ProcessError error){
	QString mess;

	//qDebug()<<m_process_killed;
	if (m_process_killed) return; // process killed by the user => ignore

	switch(error){
		case QProcess::FailedToStart:
			mess = "The process failed to start.\nEither the program optimas is missing,\nor you may have insufficient permissions.";
			break;
		case QProcess::Crashed:
			mess = "The process crashed some time after starting successfully.";
			break;
		case QProcess::UnknownError:
			mess = "An unknown error occurred.";
		default: break; // Timedout, ReadError and WriteError should not happen.
	}
	QMessageBox::critical(this, "Process Error", mess);

	//ui.scorePage->setDisabled(false);
	m_import->setResultsLineEdit(m_import->getResultsLineEdit().remove(m_import->getResultsLineEdit().size() - 19, 19));
	m_import->setRunPushButton("Run");
	m_import->setImportDisabled(false);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int exit_code: the exit code of the process.
	@ int ExitStatus: [0] if the process exited normally and [1] if a known error occurred.
	## RETURN:
	## SPECIFICATION: This signal is emitted when the process finishes (OptiMAS algorithm).
	-----------------------------------------------------------------------------
*/

void MainWindow::runOptiMASFinished(int exit_code, QProcess::ExitStatus){

	// The process is finished (or killed)
	if( (exit_code == 0) && (!m_process_killed) ){
		QString tab_res = m_file_res + "/tab_scores.txt";

		// Program finished (no problems)
		if(QFile::exists(tab_res)){
			char run_time[256];
			int time = m_date.secsTo(QDateTime::currentDateTime()), hour, min, sec;

			hour = time / 3600;
			time = time % 3600;
			min = time / 60;
			time = time % 60;
			sec = time;
			sprintf(run_time, "Run done in %dh%dm%ds", hour, min, sec);
			ui.bottomTextBrowser->setText(run_time);

			// Creation of the 3 folders for the next steps
			QStringList vect_folders = (QStringList()<<"input"<<"list_selection"<<"list_crosses");
			QString new_folder("");
			QDir dir(m_file_res);

			for(int i=0; i<3; i++){
				new_folder = m_file_res + vect_folders[i];
				if( ! dir.mkdir(vect_folders[i]) ){
					showCustomErrorMessage("Error in creating the folder " + new_folder + " !!\nCheck user's permission. OptiMAS will be unstable...");
					return; //// exit(1) !!
				}
			}

			// New location of the map file that will be copied
			new_folder = m_file_res + "/input/" + m_import->getMapLineEdit().section('/', -1).replace(QRegExp("([^.]+)(\\.[^.]+)?$"), "\\1.map");

			// Copy the map file into the new input folder
			if( m_import->copyFile(m_import->getMapLineEdit(), new_folder) ){
				ui.bottomTextBrowser->setText("File " + m_import->getMapLineEdit() + " saved into " + new_folder + ".");
			}

			// Creation of the new map file for checking genotyping errors (along pedigree)
			createMapFileCheck(m_import->getMapLineEdit());

			// New location of the genotype file that will be copied
			new_folder = m_file_res + "/input/" + m_import->getGenoLineEdit().section('/', -1).replace(QRegExp("([^.]+)(\\.[^.]+)?$"), "\\1.dat");

			// Copy the genotype file into the new input folder
			if( m_import->copyFile(m_import->getGenoLineEdit(), new_folder ) ){
				ui.bottomTextBrowser->setText("File " + m_import->getGenoLineEdit() + " saved into " + new_folder + ".");
			}

			m_import->setValueProgressBar(100); // 100% Done
			ui.bottomTextBrowser->setText(run_time);

			// Reset all the tables and graphs
			resetAll();

			// Display and plot all the tables and graphs of the score page
			displayAllPages();

			getFavAlleles();

			// Fetching number of qtl
			int n_qtl = ui.scoreTableWidget->columnCount() - COL_FIRST_QTL;

			for (int i_qtl=1; i_qtl<=n_qtl; i_qtl++){
				parseHomoHeteroFile(i_qtl);
			}

			// Plot the right graph of the Score page regarding the QTL selection and the type of graph selected (Histogram or BoxPlot)
			connect(ui.graphRightQtlComboBox2, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphScorePageQtl(int)));
			connect(ui.graphRightParamsComboBox2, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphScorePage(int)));

			// Selection page graphs
			connect(ui.graphLeftListComboBox6, SIGNAL(currentIndexChanged(int)), this, SLOT(plotLeftGraphSelectionPageList(int)));
			connect(ui.graphRightListComboBox6, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphSelectionPageList(int)));
			connect(ui.graphLeftQtlComboBox6, SIGNAL(currentIndexChanged(int)), this, SLOT(plotLeftGraphSelectionPageQtl(int)));
			connect(ui.graphRightQtlComboBox6, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphSelectionPageQtl(int)));

			// Crosses page graphs
			connect(ui.graphLeftListComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotLeftGraphCrossPageList(int)));
			connect(ui.graphRightListComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphCrossPageList(int)));
			connect(ui.graphLeftQtlComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotHistoLeftGraphCrossPageQtl(int))); ////
			connect(ui.graphRightQtlComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotHistoRightGraphCrossPageQtl(int))); ////
			connect(ui.graphLeftPlotComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotLeftGraphCrossPage(int))); ////
			connect(ui.graphRightPlotComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphCrossPage(int))); ////

		}
	}
	// Program stopped by the user
	else if( m_process_killed ){
		// 0% completed for the progress bar
		m_import->setValueProgressBar(0);
		m_import->setNumQtlProgressBar("000");
		m_import->setNumindivProgressBar("0000");
		ui.tabWidget2->setDisabled(true); // Tables of the score page
		ui.selectionPage->setDisabled(true); // Selection Page
		ui.crossingPage->setDisabled(true); // Crossing page
	}
	else {
		showCustomErrorMessage("Error running the program !!");
		ui.scorePage->setDisabled(false); // Table of the score page
	}

	m_import->setResultsLineEdit(m_import->getResultsLineEdit().remove(m_import->getResultsLineEdit().size() - 19, 19));
	m_import->setRunPushButton("Run");

	m_import->setImportDisabled(false);

	//delete m_process;
	//m_process = NULL;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: run FillMd@Mks tool with parameters coming from
	the FillMdWindow import data.
	-----------------------------------------------------------------------------
*/

void MainWindow::runFillMd(){

	// If the 3 input/output files are loaded: the program starts
	if( m_fill_md->isOK() ){
		m_fill_md->runFillMD();
	}
	//runFillMDatMks(m_fill_md->getEventsLineEdit(), m_fill_md->getGenoLineEdit(), m_fill_md->getResultsLineEdit());
	// Some input files are missing
	else{
		showCustomErrorMessage("Please select/load every Input/Output files before running the program !!");
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QString path_map_file: location of the map file upload by the user
	## RETURN:
	## SPECIFICATION: Creation of a new map file that will be used in case of
	genotyping error in the log file. Each SNP will be treated one by one regarding
	the pedigree of individuals. Thus, genotyping can be found.
	-----------------------------------------------------------------------------
*/

void MainWindow::createMapFileCheck(QString path_map_file){
	QMap <QString, QString> map_qtl;
	QStringList list_locus, list_chr, list_qtl, list_pos;
	QString header = "", new_map_file_path = m_file_res + "/input/check_" + path_map_file.section('/', -1).replace(QRegExp("([^.]+)(\\.[^.]+)?$"), "\\1.cmap"); // New location of the map file that will be created
	QFile NewMapFile(new_map_file_path);
	int nb_lines = 0;

	if( QFile::exists(path_map_file) ){
		QFile MapFile(path_map_file); // file descriptor
		if(!MapFile.open(QIODevice::ReadOnly | QIODevice::Text)){
			QMessageBox::information(this, tr("New map file creation to check genotyping error"), tr("Error: opening the file [%1]").arg(path_map_file));
			return;
		}
		QTextStream in(&MapFile); // read only text stream on file

		while( !in.atEnd() ) {
			QString line = in.readLine();
			QStringList SplitedLine = line.split('\t',QString::SkipEmptyParts);
			int size_column = SplitedLine.size();

			if( nb_lines != 0 ){
				// Marker position
				if( size_column == 4 ){
					list_locus.push_back(SplitedLine[0]);
					list_chr.push_back(SplitedLine[1]);
					list_qtl.push_back(SplitedLine[2]);
					list_pos.push_back(SplitedLine[3]);
				}
				// QTL position
				else if( size_column == 5 ){
					map_qtl[SplitedLine[2]] = SplitedLine[4];
				}
			}
			nb_lines++;
		}
		MapFile.close();
	}

	if( NewMapFile.open(QIODevice::WriteOnly | QIODevice::Text) ){
		QTextStream out_map_file(&NewMapFile); // write only text stream on file
		// Header
		out_map_file << "Locus\tChr\tQTL\tPos\tAll+\n";

		for(int i=0; i<list_locus.size(); i++){
			out_map_file << "qtl" << QString(" ").setNum(i + 1) << "\t" << list_chr[i] << "\t" << QString(" ").setNum(i + 1) << "\t" << list_pos[i] << "\t" << map_qtl[list_qtl[i]] << "\n";
			out_map_file << list_locus[i] << "\t" << list_chr[i] << "\t" << QString(" ").setNum(i + 1) << "\t" << list_pos[i] << "\n";
		}
		NewMapFile.close();
	}
	else {
		QMessageBox::information(this, tr("New map file creation to check genotyping error"), tr("Error: creation of the file [%1] failed.").arg(new_map_file_path));
		return;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: reset all tables and comboboxes for a new run.
	-----------------------------------------------------------------------------
*/

void MainWindow::resetAll(){

	m_map_scores.clear();
	m_map_gene_scores.clear();
	m_map_gene_quantiles.clear();
	m_vect_generation.clear();
	m_map_group.clear();
	m_homo_fav.clear();
	m_hetero.clear();
	m_homo_unfav.clear();
	m_parents.clear();
	m_vect_fav_all.clear();
	m_vect_color.clear();
	m_vect_cut_off.clear();
	m_vect_truncation_option.clear();
	m_vect_complementation_params.clear();
	m_vect_cross_option.clear();
	m_vect_group_scores.clear();

	initMainWindow();
	initVisualizationParameters();
	initTruncationOption();
	initComplementationParameters();
	initCrossOption();

	m_vect_qtl_weight.clear();

	// Reset table and graphs of scores
	resetScorePage();

	// Reset lists and graphs of selection
	resetSelectionPage(QMessageBox::Yes);

	// Reset lists and graphs of crosses
	resetCrossingPage(QMessageBox::Yes);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION:
	copy the example folders in the user's personal folder if needed.
	-----------------------------------------------------------------------------
*/

void MainWindow::setDataPath(){

	QString app_dir_path = QApplication::applicationDirPath();
	QString install_data_path;

	#if defined(__WINDOWS__) || defined(__APPLE__)
	install_data_path = app_dir_path;
	#else //if defined (__linux__)
	if ( app_dir_path == QDir::homePath() + QString::fromUtf8("/bin") ) install_data_path = QDir::homePath() + QString::fromUtf8("/.OptiMAS");
	else install_data_path = "/usr/share/OptiMAS";
	#endif

	QDir homeDir( QDir::homePath() );
	if(!homeDir.exists("OptiMAS/input")){
		if (homeDir.mkpath("OptiMAS/input"))
			copyFolder(install_data_path+"/input", QDir::homePath()+"/OptiMAS/input");
		else QMessageBox::warning(this, tr(""), tr("input example files could not be copied in your personal folder."));
	}
	if(!homeDir.exists("OptiMAS/output/blanc")){
		if (homeDir.mkpath("OptiMAS/output/blanc"))
			copyFolder(install_data_path+"/output/blanc", QDir::homePath()+"/OptiMAS/output/blanc");
		else QMessageBox::warning(this, tr(""), tr("Multiparental example files could not be copied in your personal folder."));
	}
	if(!homeDir.exists("OptiMAS/output/moreau")){
		if (homeDir.mkpath("OptiMAS/output/moreau"))
			copyFolder(install_data_path+"/output/moreau", QDir::homePath()+"/OptiMAS/output/moreau");
		else QMessageBox::warning(this, tr(""), tr("Biparental example files could not be copied in your personal folder."));
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION:
	Initialization of containers / parameters / buttons at the start.
	-----------------------------------------------------------------------------
*/

void MainWindow::initMainWindow(){
	// Buttons / parameters by Default
	ui.stackedWidget->setCurrentIndex(0); // The 1st page is the step 1
	////ui.menuVisualization->setEnabled(false);
	////ui.menuData->setEnabled(false);
	ui.tabWidget2->setDisabled(true); // Tables of the score page
	ui.selectionPage->setDisabled(true); // Selection Page
	ui.crossingPage->setDisabled(true); // Crossing page
	ui.listSelection2CheckBox4->setChecked(false);
	ui.selectionList2ComboBox4->setEnabled(false);
	////ui.pedigreeZoomWidget->setEnabled(false);

	m_process_killed = false;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: reset all the tables and graphs of the score page.
	-----------------------------------------------------------------------------
*/

void MainWindow::resetScorePage(){

	ui.scoreTableWidget->clearContents();
	ui.scoreTableWidget->setRowCount(0);
	ui.scoreTableWidget->setColumnCount(0);

	ui.homoHeteroTableWidget->clearContents();
	ui.homoHeteroTableWidget->setRowCount(0);
	ui.homoHeteroTableWidget->setColumnCount(0);

	ui.parentsTableWidget->clearContents();
	ui.parentsTableWidget->setRowCount(0);
	ui.parentsTableWidget->setColumnCount(0);

	ui.graphLeftQwtPlot2->detachItems();
	ui.graphLeftQwtPlot2->replot();
	ui.graphRightQwtPlot2->detachItems();
	ui.graphRightQwtPlot2->replot();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: reset all the tables, lists and graphs of the selection page.
	-----------------------------------------------------------------------------
*/

void MainWindow::resetSelectionPage(int answer){

	// Yes : all the lists are deleted
	if( answer == QMessageBox::Yes ) {

		clearSelectionPage();

		// Creation of the 2 empty list by default
		for(int i=0; i<2; i++){
			QString listName = "List Selection " + QString(" ").setNum(i+1);

			// Update of the containers for the new list added
			m_list_selection.push_back(listName);
			ui.selectionListWidget6->addItem(listName);
			m_vect_map_selection.push_back(new QMap<QString, int>());

			// Update of the combobox lists
			ui.leftListComboBox6->addItem(listName);
			ui.rightListComboBox6->addItem(listName);
			ui.truncListComboBox6->addItem(listName);
			ui.compListComboBox6->addItem(listName);
			ui.selectionListComboBox4->addItem(listName);
			ui.selectionList2ComboBox4->addItem(listName);
			ui.graphLeftListComboBox6->addItem(listName);
			ui.graphRightListComboBox6->addItem(listName);
			ui.pedigreeListComboBox6->addItem(listName);
		}
		ui.selectionListWidget6->setCurrentRow(0);
		ui.leftListComboBox6->setCurrentIndex(0);
		ui.rightListComboBox6->setCurrentIndex(1);
		ui.graphLeftListComboBox6->setCurrentIndex(0);
		ui.graphRightListComboBox6->setCurrentIndex(1);
	}
	else if( answer == QMessageBox::No ){
		return;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: reset all the tables, lists and graphs of the crossing page.
	-----------------------------------------------------------------------------
*/

void MainWindow::resetCrossingPage(int answer){

	// Yes : all the current lists are deleted
	if( answer == QMessageBox::Yes ) {

		clearCrossingPage();

		// Creation of the 2 empty list by default
		for(int i=0; i<2; i++){
			QString listName = QString("List Crosses %1").arg(i+1, 0, 10);

			// Update of the containers for the new list added
			m_list_crosses.push_back(listName);
			m_vect_notes_crosses.push_back(QStringList()<<""<<""<<"");
			ui.crossListWidget4->addItem(listName);
			m_vect_map_crosses.push_back(new QMap<QString, QStringList >());

			// Update of the combobox lists
			ui.leftListComboBox4->addItem(listName);
			ui.rightListComboBox4->addItem(listName);
			ui.crossesListComboBox4->addItem(listName);
			ui.graphLeftListComboBox4->addItem(listName);
			ui.graphRightListComboBox4->addItem(listName);
		}
		ui.crossListWidget4->setCurrentRow(0);
		ui.leftListComboBox4->setCurrentIndex(0);
		ui.rightListComboBox4->setCurrentIndex(1);
		ui.graphLeftListComboBox4->setCurrentIndex(0);
		ui.graphRightListComboBox4->setCurrentIndex(1);
	}
	else if( answer == QMessageBox::No ){
		return;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: reset all tables and comboboxes for a new run.
	-----------------------------------------------------------------------------
*/

void MainWindow::clearAll(){

	m_map_scores.clear();
	m_map_gene_scores.clear();
	m_map_gene_quantiles.clear();
	m_vect_generation.clear();
	m_map_group.clear();
	m_homo_fav.clear();
	m_hetero.clear();
	m_homo_unfav.clear();
	m_parents.clear();
	m_vect_fav_all.clear();
	m_vect_group_scores.clear();
	m_vect_color.clear();
	m_vect_cut_off.clear();
	m_vect_truncation_option.clear();
	m_vect_complementation_params.clear();
	m_vect_cross_option.clear();

	initMainWindow();
	initVisualizationParameters();
	initTruncationOption();
	initComplementationParameters();
	initCrossOption();

	m_vect_qtl_weight.clear();

	// Reset table and graphs of scores
	resetScorePage();

	// Clear lists and graphs of the selection page
	clearSelectionPage();

	// Clear lists and graphs of the crosses page
	clearCrossingPage();

	ui.actionVisualization->setEnabled(false);
	ui.actionSaveAll->setEnabled(false);
	ui.actionClearAll->setEnabled(false);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: reset all the tables, lists and graphs of the selection page.
	-----------------------------------------------------------------------------
*/

void MainWindow::clearSelectionPage(){

	int nb_lists = (int)m_vect_map_selection.size();

	// All the lists of map are removed
	for(int i=(nb_lists - 1); i>=0; i--){
		// Delete each map
		delete m_vect_map_selection[i];
	}

	m_list_selection.clear();
	m_vect_map_selection.clear();

	// Reset all the tables, graphs and comboboxes
	ui.selectionListWidget6->clear();
	ui.leftListComboBox6->clear();
	ui.rightListComboBox6->clear();
	ui.truncListComboBox6->clear();
	ui.compListComboBox6->clear();
	ui.graphLeftListComboBox6->clear();
	ui.graphRightListComboBox6->clear();
	ui.pedigreeListComboBox6->clear();
	ui.selectionListComboBox4->clear();
	ui.selectionList2ComboBox4->clear();
	ui.leftListTableWidget6->clearContents();
	ui.leftListTableWidget6->setRowCount(0);
	ui.rightListTableWidget6->clearContents();
	ui.rightListTableWidget6->setRowCount(0);
	ui.graphLeftQwtPlot6->detachItems();
	ui.graphLeftQwtPlot6->replot();
	ui.graphRightQwtPlot6->detachItems();
	ui.graphRightQwtPlot6->replot();
	ui.pedigreeLabel6->clear();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: delete all the tables, lists and graphs of the crossing page.
	-----------------------------------------------------------------------------
*/

void MainWindow::clearCrossingPage(){

	int nb_lists = (int)m_vect_map_crosses.size();

	// All the lists are removed
	for(int i=(nb_lists - 1); i>=0; i--){
		delete m_vect_map_crosses[i];
	}

	m_list_crosses.clear();
	m_vect_map_crosses.clear();

	// Clear all the tables, lists, comboboxes and graphs
	for(int i=0; i<(int)m_vect_notes_crosses.size(); i++){
		m_vect_notes_crosses[i].clear();
	}
	m_vect_notes_crosses.clear();
	ui.crossesListComboBox4->clear();
	ui.leftListComboBox4->clear();
	ui.rightListComboBox4->clear();
	ui.graphLeftListComboBox4->clear();
	ui.graphRightListComboBox4->clear();
	ui.crossListWidget4->clear();
	ui.leftListTableWidget4->clearContents();
	ui.leftListTableWidget4->setRowCount(0);
	ui.rightListTableWidget4->clearContents();
	ui.rightListTableWidget4->setRowCount(0);
	ui.graphLeftQwtPlot4->detachItems();
	ui.graphLeftQwtPlot4->replot();
	ui.graphRightQwtPlot4->detachItems();
	ui.graphRightQwtPlot4->replot();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: display the 3 tables of the Score page.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayAllPages(){

	try{
		// The table of molecular scores is displayed and informations are stored (for other graphical representation)
		displayTableScore(m_file_res + "/tab_scores.txt");

		// The results of the run are displayed in the tables
		displayTable(m_file_res + "/tab_homo_hetero.txt", ui.homoHeteroTableWidget);
		displayTable(m_file_res + "/tab_parents.txt", ui.parentsTableWidget);
	}
	catch(const QString& name_file){
		showCustomErrorMessage("Error: opening the file [" + name_file + "] !!");
		return;
	}

	// Data are loaded => enable Visualization and Data menus.
	ui.actionVisualization->setEnabled(true);
	ui.actionSaveAll->setEnabled(true);
	ui.actionClearAll->setEnabled(true);

	// Maximum number of individuals to select
	ui.truncNSelSpinBox6->setMaximum(ui.scoreTableWidget->rowCount());

	plotLeftGraphScorePage(); // The graph of the evolution of the molecular score for each generation is plotted

	if( ui.graphRightParamsComboBox2->currentIndex() == 0 ){
		plotHistoScorePage(ui.graphRightQtlComboBox2->currentIndex()); // Distribution of individuals regarding the MS
	}
	else if( ui.graphRightParamsComboBox2->currentIndex() == 1 ){
		plotBoxPlotScorePage(ui.graphRightQtlComboBox2->currentIndex()); // Group variation
	}

	// Score graph page
	connect(ui.graphRightQtlComboBox2, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphScorePageQtl(int)));
	connect(ui.graphRightParamsComboBox2, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphScorePage(int)));

	// Selection graph page
	connect(ui.graphLeftListComboBox6, SIGNAL(currentIndexChanged(int)), this, SLOT(plotLeftGraphSelectionPageList(int)));
	connect(ui.graphRightListComboBox6, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphSelectionPageList(int)));
	connect(ui.graphLeftQtlComboBox6, SIGNAL(currentIndexChanged(int)), this, SLOT(plotLeftGraphSelectionPageQtl(int)));
	connect(ui.graphRightQtlComboBox6, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphSelectionPageQtl(int)));

	// Crosses graph page
	connect(ui.graphLeftListComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotLeftGraphCrossPageList(int)));
	connect(ui.graphRightListComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphCrossPageList(int)));
	connect(ui.graphLeftQtlComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotHistoLeftGraphCrossPageQtl(int))); ////
	connect(ui.graphRightQtlComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotHistoRightGraphCrossPageQtl(int))); ////
	connect(ui.graphLeftPlotComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotLeftGraphCrossPage(int))); ////
	connect(ui.graphRightPlotComboBox4, SIGNAL(currentIndexChanged(int)), this, SLOT(plotRightGraphCrossPage(int))); ////

	// Weight slide updated if it is not empty (user's preferencies exist)
	if(m_vect_qtl_weight.empty()){
		m_vect_qtl_weight.resize(ui.scoreTableWidget->columnCount() - COL_FIRST_QTL, 1.0);
	}

	// Step page and current lists updated
	ui.stepsListWidget->setCurrentRow(0);
	ui.tabWidget2->setCurrentIndex(0);
	ui.leftListComboBox6->setCurrentIndex(ui.selectionListWidget6->currentRow());

	// Buttons are enabled when the program is finished
	ui.tabWidget2->setDisabled(false); // Tables of the score page
	ui.selectionPage->setDisabled(false); // Selection Page
	ui.crossingPage->setDisabled(false); // Crossing page
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ string name_file: path of the molecular scores output file.
	## RETURN:
	## SPECIFICATION: The table of molecular scores is displayed and informations
	are stored (for other graphical representation).
	-----------------------------------------------------------------------------
*/

void MainWindow::displayTableScore(const QString & name_file){

	QStringList horizontalHeaders, combobox_qtl_list, crossesHeaders;

	ui.scoreTableWidget->setSortingEnabled(false);

	ui.graphRightQtlComboBox2->clear();
	ui.graphLeftQtlComboBox6->clear();
	ui.graphRightQtlComboBox6->clear();
	ui.graphLeftQtlComboBox4->clear();
	ui.graphRightQtlComboBox4->clear();
	m_map_scores.clear();
	m_map_gene_scores.clear();
	m_map_gene_quantiles.clear();
	m_map_group.clear();
	m_vect_generation.clear();
	m_vect_group_scores.clear();

	QFile tabFile(name_file); // file descriptor

	// If the file exists and opened for reading.
	if ( QFile::exists(name_file) && tabFile.open(QIODevice::ReadOnly | QIODevice::Text) ){
		QTextStream in(&tabFile); // read only text stream on file

		int cpt_lines = 0, ncol = 0, cpt = 0;
		double d = 0.0;

		// Loop on the input file (line by line)
		while(!in.atEnd()){

			QString line = in.readLine();
			QStringList SplitedLine = line.split('\t',QString::SkipEmptyParts); // We split each tab separated column of each line in an array
			
			ncol = SplitedLine.size();
			// 1st line: header
			if( cpt_lines == 0 && ncol > 0 ){
				// New name of generation: All = all the individuals together
				m_vect_generation.push_back("All");
				m_map_gene_scores["All"].push_back(0.0); // 1st column = number of individuals in the class
				m_vect_group_scores.resize(ncol-COL_FIRST_QTL+2);

				for(int i=0; i<ncol; i++){
					horizontalHeaders << SplitedLine[i];

					if( (i != COL_GENE) && (i != COL_GROUP) ){
						crossesHeaders << SplitedLine[i];
					}

					if( (i == COL_MS) || (i == COL_WEIGHT) || (i >= COL_FIRST_QTL) ){
						combobox_qtl_list << SplitedLine[i];
						m_map_gene_scores["All"].push_back(0.0);
						m_map_gene_quantiles["All"].push_back(QVector<int>(20, 0)); // Step = 0.05 until 1.0
					}
				}

				ui.scoreTableWidget->setColumnCount(ncol);
				ui.scoreTableWidget->setHorizontalHeaderLabels(horizontalHeaders);
				ui.scoreTableWidget->verticalHeader()->hide();

				// Same headers for the selection lists
				ui.leftListTableWidget6->setColumnCount(ncol);
				ui.leftListTableWidget6->setHorizontalHeaderLabels(horizontalHeaders);
				ui.rightListTableWidget6->setColumnCount(ncol);
				ui.rightListTableWidget6->setHorizontalHeaderLabels(horizontalHeaders);

				// Headers for the tables of crosses
				ui.leftListTableWidget4->setColumnCount(crossesHeaders.size());
				ui.leftListTableWidget4->setHorizontalHeaderLabels(crossesHeaders);
				ui.rightListTableWidget4->setColumnCount(crossesHeaders.size());
				ui.rightListTableWidget4->setHorizontalHeaderLabels(crossesHeaders);

				// The items (score table headers) are added to the right comboBox
				ui.graphRightQtlComboBox2->addItems(combobox_qtl_list);
				ui.graphLeftQtlComboBox6->addItems(combobox_qtl_list);
				ui.graphRightQtlComboBox6->addItems(combobox_qtl_list);
				ui.graphLeftQtlComboBox4->addItems(combobox_qtl_list);
				ui.graphRightQtlComboBox4->addItems(combobox_qtl_list);

				// ToolTips for columns in all the tables
				ui.scoreTableWidget->horizontalHeaderItem(COL_P1)->setToolTip("Parent 1");
				ui.leftListTableWidget6->horizontalHeaderItem(COL_P1)->setToolTip("Parent 1");
				ui.rightListTableWidget6->horizontalHeaderItem(COL_P1)->setToolTip("Parent 1");
				ui.rightListTableWidget4->horizontalHeaderItem(COL_P1)->setToolTip("Parent 1");
				ui.leftListTableWidget4->horizontalHeaderItem(COL_P1)->setToolTip("Parent 1");
				ui.scoreTableWidget->horizontalHeaderItem(COL_P2)->setToolTip("Parent 2");
				ui.leftListTableWidget6->horizontalHeaderItem(COL_P2)->setToolTip("Parent 2");
				ui.rightListTableWidget6->horizontalHeaderItem(COL_P2)->setToolTip("Parent 2");
				ui.rightListTableWidget4->horizontalHeaderItem(COL_P2)->setToolTip("Parent 2");
				ui.leftListTableWidget4->horizontalHeaderItem(COL_P2)->setToolTip("Parent 2");
				ui.scoreTableWidget->horizontalHeaderItem(COL_MS)->setToolTip("Molecular Score (All QTL)");
				ui.leftListTableWidget6->horizontalHeaderItem(COL_MS)->setToolTip("Molecular Score (All QTL)");
				ui.rightListTableWidget6->horizontalHeaderItem(COL_MS)->setToolTip("Molecular Score (All QTL)");
				ui.rightListTableWidget4->horizontalHeaderItem(COL_MS - 2)->setToolTip("Molecular Score (All QTL)");
				ui.leftListTableWidget4->horizontalHeaderItem(COL_MS - 2)->setToolTip("Molecular Score (All QTL)");
				ui.scoreTableWidget->horizontalHeaderItem(COL_WEIGHT)->setToolTip("Attributed to each QTL in order to weight its importance in the molecular score.");
				ui.leftListTableWidget6->horizontalHeaderItem(COL_WEIGHT)->setToolTip("Attributed to each QTL in order to weight its importance in the molecular score.");
				ui.rightListTableWidget6->horizontalHeaderItem(COL_WEIGHT)->setToolTip("Attributed to each QTL in order to weight its importance in the molecular score.");
				ui.rightListTableWidget4->horizontalHeaderItem(COL_WEIGHT - 2)->setToolTip("Attributed to each QTL in order to weight its importance in the molecular score.");
				ui.leftListTableWidget4->horizontalHeaderItem(COL_WEIGHT - 2)->setToolTip("Attributed to each QTL in order to weight its importance in the molecular score.");
				ui.scoreTableWidget->horizontalHeaderItem(COL_UC)->setToolTip("Utility criterion: combines the molecular score with the expected\nvariance of the gametes that can be produced by the individual.");
				ui.leftListTableWidget6->horizontalHeaderItem(COL_UC)->setToolTip("Utility criterion: combines the molecular score with the expected\nvariance of the gametes that can be produced by the individual.");
				ui.rightListTableWidget6->horizontalHeaderItem(COL_UC)->setToolTip("Utility criterion: combines the molecular score with the expected\nvariance of the gametes that can be produced by the individual.");
				ui.rightListTableWidget4->horizontalHeaderItem(COL_UC - 2)->setToolTip("Utility criterion: combines the molecular score with the expected\nvariance of the gametes that can be produced by the individual.");
				ui.leftListTableWidget4->horizontalHeaderItem(COL_UC - 2)->setToolTip("Utility criterion: combines the molecular score with the expected\nvariance of the gametes that can be produced by the individual.");
				ui.scoreTableWidget->horizontalHeaderItem(COL_NO_FAV)->setToolTip("Number of QTL fixed homozygous favourable regarding a threshold.");
				ui.leftListTableWidget6->horizontalHeaderItem(COL_NO_FAV)->setToolTip("Number of QTL fixed homozygous favourable regarding a threshold.");
				ui.rightListTableWidget6->horizontalHeaderItem(COL_NO_FAV)->setToolTip("Number of QTL fixed homozygous favourable regarding a threshold.");
				ui.leftListTableWidget4->horizontalHeaderItem(COL_NO_FAV - 2)->setToolTip("Number of QTL fixed homozygous favourable regarding a threshold.");
				ui.rightListTableWidget4->horizontalHeaderItem(COL_NO_FAV - 2)->setToolTip("Number of QTL fixed homozygous favourable regarding a threshold.");
				ui.scoreTableWidget->horizontalHeaderItem(COL_NO_UNFAV)->setToolTip("Number of QTL fixed homozygous unfavourable regarding a threshold.");
				ui.leftListTableWidget6->horizontalHeaderItem(COL_NO_UNFAV)->setToolTip("Number of QTL fixed homozygous unfavourable regarding a threshold.");
				ui.rightListTableWidget6->horizontalHeaderItem(COL_NO_UNFAV)->setToolTip("Number of QTL fixed homozygous unfavourable regarding a threshold.");
				ui.leftListTableWidget4->horizontalHeaderItem(COL_NO_UNFAV - 2)->setToolTip("Number of QTL fixed homozygous unfavourable regarding a threshold.");
				ui.rightListTableWidget4->horizontalHeaderItem(COL_NO_UNFAV - 2)->setToolTip("Number of QTL fixed homozygous unfavourable regarding a threshold.");
				ui.scoreTableWidget->horizontalHeaderItem(COL_NO_HETERO)->setToolTip("Number of QTL fixed heterozygous regarding a threshold.");
				ui.leftListTableWidget6->horizontalHeaderItem(COL_NO_HETERO)->setToolTip("Number of QTL fixed heterozygous regarding a threshold.");
				ui.rightListTableWidget6->horizontalHeaderItem(COL_NO_HETERO)->setToolTip("Number of QTL fixed heterozygous regarding a threshold.");
				ui.leftListTableWidget4->horizontalHeaderItem(COL_NO_HETERO - 2)->setToolTip("Number of QTL fixed heterozygous regarding a threshold.");
				ui.rightListTableWidget4->horizontalHeaderItem(COL_NO_HETERO - 2)->setToolTip("Number of QTL fixed heterozygous regarding a threshold.");
				ui.scoreTableWidget->horizontalHeaderItem(COL_NO_UNCERTAIN)->setToolTip("Number of uncertain genotypes.");
				ui.leftListTableWidget6->horizontalHeaderItem(COL_NO_UNCERTAIN)->setToolTip("Number of uncertain genotypes.");
				ui.rightListTableWidget6->horizontalHeaderItem(COL_NO_UNCERTAIN)->setToolTip("Number of uncertain genotypes.");
				ui.leftListTableWidget4->horizontalHeaderItem(COL_NO_UNCERTAIN - 2)->setToolTip("Number of uncertain genotypes.");
				ui.rightListTableWidget4->horizontalHeaderItem(COL_NO_UNCERTAIN - 2)->setToolTip("Number of uncertain genotypes.");
			}
			// > 1st line
			else if ( ncol > 0 ){
				// Number of individuals per population/generation
				m_map_gene_scores["All"][0] += 1.0;

				const QString & cycle = SplitedLine[COL_GENE];
				const QString & grp = SplitedLine[COL_GROUP];

				// The generation/cycle is found (available data)
				if( (cycle.compare("-")) != 0 ){
					// New name of generation found
					if( m_map_gene_scores.find(cycle) == m_map_gene_scores.end() ){
						m_vect_generation.push_back(cycle);

						// Initialization of the map vector
						for(int j=0; j<(ncol - COL_FIRST_QTL + 3); j++){
							m_map_gene_scores[cycle].push_back(0.0);
							if( j != 0 ){
								m_map_gene_quantiles[cycle].push_back(QVector<int>(20, 0));
							}
						}
					}
					// Number of individuals in each generation
					m_map_gene_scores[cycle][0] += 1.0;
				}

				// The group is found (available data)
				if( grp.compare("-") != 0 ){
					// Individual added to the corresponding group
					m_map_group[grp].push_back(SplitedLine[0]);
				}

				// Display table
				ui.scoreTableWidget->setRowCount(cpt_lines);
				cpt = 0;

				// Loop on the columns of the table
				for(int i=0; i<ncol; i++){
					// Utility criterion
					if( i == COL_UC ){
						// The data are filled to the table (values are doubles)
						QTableWidgetItem *newItem = new QTableWidgetItem();
						newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
						//newItem->setData(0, SplitedLine[i].toDouble());
						newItem->setData(0, QString().setNum( SplitedLine[i].toDouble(), 'f', 4).toDouble() );
						ui.scoreTableWidget->setItem(cpt_lines - 1, i, newItem);
						((QTableWidgetItem *)ui.scoreTableWidget->item(cpt_lines - 1, i))->setTextColor(Qt::darkGreen);
					}
					// No (+/+, -/-, +/-)
					else if( (i > COL_UC) && (i < COL_FIRST_QTL) ){
						// The data are filled to the table (values are integers)
						QTableWidgetItem *newItem = new QTableWidgetItem();
						newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
						newItem->setData(0, SplitedLine[i].toInt());
						ui.scoreTableWidget->setItem(cpt_lines - 1, i, newItem);
					}
					else{
						// The table of scores is filled
						QTableWidgetItem *newItem = NULL;
						if(i>=COL_FIRST_QTL || i == COL_MS || i == COL_WEIGHT){
							//newItem = new QTableWidgetItem(SplitedLine[i].left(6)); // 4 decimal digits.
							newItem = new QTableWidgetItem( QString().setNum(SplitedLine[i].toDouble(), 'f', 4) );
						}
						else newItem = new QTableWidgetItem(SplitedLine[i]);
						newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
						ui.scoreTableWidget->setItem(cpt_lines - 1, i, newItem);

						// Molecular score (All)
						if( i == COL_MS ){
							((QTableWidgetItem *)ui.scoreTableWidget->item(cpt_lines - 1, i))->setTextColor(Qt::red);
						}
						// Weight
						else if( i == COL_WEIGHT ){
							((QTableWidgetItem *)ui.scoreTableWidget->item(cpt_lines - 1, i))->setTextColor(Qt::blue);
						}
					}

					if( i > 0 ){
						m_map_scores[SplitedLine[0]].push_back(SplitedLine[i]);
					}

					// Probabilities for All/each QTL
					if( (i == COL_MS) || (i == COL_WEIGHT) || (i >= COL_FIRST_QTL) ){

						d = SplitedLine[i].toDouble();

						// The generation is found (available data)
						if( (cycle.compare("...") != 0) && (cycle.compare("-") != 0) ){
							// Sum of all scores for each generation
							m_map_gene_scores[cycle][cpt + 1] += d;

							// Sum of the number of individuals for each quantile
							m_map_gene_quantiles[cycle][cpt][whichQuantile(d)] += 1;
						}
						// Sum of all scores for each generation
						m_map_gene_scores["All"][cpt + 1] += d;

						// Sum of the number of individuals for each quantile
						m_map_gene_quantiles["All"][cpt][whichQuantile(d)] += 1;
						cpt++;
					}
				}
			}
			cpt_lines++;
		}

		// Mean of each scores
		for( QMap <QString, QList<double> >::iterator iter_mean = m_map_gene_scores.begin(); iter_mean != m_map_gene_scores.end(); ++iter_mean ){
			QList<double>& scores = iter_mean.value();
			for(int i=1; i<(int)scores.size(); i++)	scores[i] /= scores[0];
		}
	}
	else {
		ui.scorePage->setDisabled(false); // Table of the score page
		throw name_file;
	}

	tabFile.close();

	ui.scoreTableWidget->setSortingEnabled(true);
	ui.scoreTableWidget->setAlternatingRowColors(true);
	ui.scoreTableWidget->resizeColumnsToContents();
	ui.scoreTableWidget->resizeRowsToContents();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ double score: molecular score.
	## RETURN: (int) the index of the quantile (position of the molecular score).
	## SPECIFICATION: return the index of the position of the current molecular
	score regarding the number of quantiles that we want to display (accuracy of
	the histogram).
	-----------------------------------------------------------------------------
*/

int MainWindow::whichQuantile(double score) const {
	
	if (score >=0 && score < 1) return (int)(score/0.05);
	if (score==1) return 19;
	return 0;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: find the favorable allele(s) for each QTL (in the map file).
	-----------------------------------------------------------------------------
*/

int MainWindow::getFavAlleles(){

	QDir resInputDir(m_file_res + "/input");
	QStringList map_files = resInputDir.entryList(QStringList() << "*.map");
	QString path = "";

	if( !m_vect_fav_all.empty() ){
		cerr << "warning:  m_vect_fav_all not empty !" << endl;
		return 0;
	}

	// Analysis has been runned with the command line version or there is a problem
	if( map_files.size() != 1 ) {
		cerr << "Error: too many or no map file(s) found !" << endl;

		// The reload analysis function may has been invoked: we try to find the .map file written in the .log file
		QString path_log_file = m_file_res + "/events.log";
		QFile LogFile(path_log_file); // file descriptor

		if( !LogFile.open(QIODevice::ReadOnly | QIODevice::Text) ) return 0;
		QTextStream in_log_file(&LogFile); // read only text stream on file

		QString first_line = in_log_file.readLine();
		QStringList SplitedFirstLine = first_line.split(' ', QString::SkipEmptyParts); // field separator : tab

		//cout << "size=" << SplitedFirstLine.size() << " map=" << SplitedFirstLine[3].toStdString() << endl;

		if( SplitedFirstLine.size() < 8 ){
			return 0;
		}
		path = SplitedFirstLine[3]; // path to the .map file
	}
	// Analysis has been runned with the GUI
	else{
		path = m_file_res + "/input/" + map_files[0];
	}

	QFile MapFile(path); // file descriptor
	if( !MapFile.open(QIODevice::ReadOnly | QIODevice::Text) ){
		cerr << "Error: the .map file [" << path.toStdString() << "] that has been used to run the analysis is not present anymore !" << endl;
		return 0;
	}
	QTextStream in(&MapFile); // read only text stream on file

	QString line = in.readLine(); // skip the header
	while (!in.atEnd()) {
		QString line = in.readLine();
		QStringList SplitedLine = line.split('\t',QString::SkipEmptyParts); // field separator : tab
		if (SplitedLine.size()==5) m_vect_fav_all.push_back(SplitedLine[4]);
	}
	MapFile.close();
	return 1;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int num_qtl: the index of the QTL (each QTL scores are stored in different files).
	## RETURN: the number of line read.
	## SPECIFICATION:
	Parse qtl_n_homo_hetero.txt file and store the qtl's scores in 3 maps:
	m_homo_fav, m_hetero, and m_homo_unfav, regarding to their allele categories,
	Homo(+/+) , Hetero(+/-) and  Homo(-/-) respectively.
	These map will then be used in void MainWindow::displaycellInfo(int row, int column).
	-----------------------------------------------------------------------------
*/

int MainWindow::parseHomoHeteroFile(int num_qtl){

	QMap <QString, QVector< QVector<QString> > >::iterator it;
	QString path = m_file_res + QString("/each_qtl/qtl_%1/qtl_%1_homo_hetero.txt").arg(num_qtl, 0, 10);

	//bool new_ind;
	int nb_line = 0;
	QString ind_name;
	QVector<QString> v_scores(2);

	if ( QFile::exists(path) ){

		QFile HomoHeteroFile(path); // file descriptor
		if (!HomoHeteroFile.open(QIODevice::ReadOnly | QIODevice::Text)) return -1;
		QTextStream in(&HomoHeteroFile); // read only text stream on file

		while (!in.atEnd()) {
			QString line = in.readLine();

			if (nb_line%4==0) {
				ind_name = line; // indiv name

				if ( (it = m_homo_fav.find(ind_name)) == m_homo_fav.end() ){ // MS
					v_scores[0] = m_map_scores[ind_name][COL_MS-1];
					v_scores[1] = "";
					m_homo_fav[ind_name].push_back(v_scores);
					m_hetero[ind_name].push_back(v_scores);
					m_homo_unfav[ind_name].push_back(v_scores);
				}
			}
			else { // ex: Homo(-/-)=1.000000	d:d=0.750000	f:f=0.250000
				QStringList SplitedLine = line.split('\t',QString::SkipEmptyParts);
				int size = SplitedLine.size();

				if (size < 1) {
					cerr << "PB: empty line " << nb_line+1 << endl;
					showCustomErrorMessage("Error: empty line in the Homo/Hetero file (l." + QString::number(nb_line+1) + ") OptiMAS will stop !!");
					exit(1);
				}

				//QStringList global_score = SplitedLine[0].split('=');
				QStringList global_score = SplitedLine.takeFirst().split('='); // takeFirst() remove the first element and return it.
				if (SplitedLine.size()>1) qSort(SplitedLine.begin(), SplitedLine.end(), customMoreThan); // Geno probabilities Descendant sorting.

				if (global_score[0] == "Homo(+/+)"){
					v_scores[0] = global_score[1];
					v_scores[1] = SplitedLine.isEmpty() ? "" : SplitedLine.join("  ");
					m_homo_fav[ind_name].push_back(v_scores);
				}
				else if (global_score[0] == "Hetero(+/-)"){
					v_scores[0] = global_score[1];
					v_scores[1] = SplitedLine.isEmpty() ? "" : SplitedLine.join("  ");
					m_hetero[ind_name].push_back(v_scores);
					//m_hetero
				}
				else if (global_score[0] == "Homo(-/-)"){
					v_scores[0] = global_score[1];
					v_scores[1] = SplitedLine.isEmpty() ? "" : SplitedLine.join("  ");
					m_homo_unfav[ind_name].push_back(v_scores);
					//m_homo_unfav
				}
				else {
					cerr << "PB: incorrect line "<< nb_line+1 << endl;
					showCustomErrorMessage("Error: empty line in the Homo/Hetero file (l." + QString::number(nb_line+1) + ") OptiMAS will stop !!");
					exit(1);
				}
			}
			nb_line++;
		}
		HomoHeteroFile.close();
	}
	else { // file doesn't exist => scores are set to 0
		QMap<QString, QStringList>::const_iterator it2;

		for( it2 = m_map_scores.begin(); it2 != m_map_scores.end(); it2++) {

			const QString & ind = it2.key(); // indiv name
			v_scores[0]="0.0";
			v_scores[1]="";

			if ( (it = m_homo_fav.find(ind)) == m_homo_fav.end() ){ // push the score for MS (All) if not done yet.
				m_homo_fav[ind].push_back(v_scores);
				m_hetero[ind].push_back(v_scores);
				m_homo_unfav[ind].push_back(v_scores);
			}
			m_homo_fav[ind].push_back(v_scores); // then push the score for this QTL.
			m_hetero[ind].push_back(v_scores);
			m_homo_unfav[ind].push_back(v_scores);
		}
	}
	return nb_line;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ string name_file: path of the output file.
	@ QTableWidget *tab: the table to fill.
	## RETURN:
	## SPECIFICATION: display text delimited output files into OptiMAS tables.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayTable(const QString & name_file, QTableWidget *tab){

	int parents=0, nb_qtl=0;
	if (tab == ui.parentsTableWidget) parents = 2; // default is biparental
	QStringList horizontalHeaders;
	tab->setSortingEnabled(false);

	// The genotype is opened for reading
	QFile tabFile(name_file); // file descriptor

	// If the file exists
	if ( QFile::exists(name_file) && tabFile.open(QIODevice::ReadOnly | QIODevice::Text) ){

		QTextStream in(&tabFile); // read only text stream on file
		int cpt_lines = 0, ncol = 0;

		// Loop on the input file (line by line)
		while (!in.atEnd()){

			QString line = in.readLine();
			QStringList SplitedLine = line.split('\t',QString::SkipEmptyParts); // We split each tab separated column of each line in an array

			// 1st line: header
			if( cpt_lines == 0 ){
				ncol = SplitedLine.size();
				for(int i=0; i<ncol; i++){
					horizontalHeaders << SplitedLine[i];
				}
				if(parents){
					parents = horizontalHeaders.filter(QRegExp("All(.+)")).size(); // multiparental
					nb_qtl = (ncol-5)/parents; // the first QTL column is the 6th column.
				}
				tab->setColumnCount(ncol);
				tab->setHorizontalHeaderLabels(horizontalHeaders);
				tab->verticalHeader()->hide();
			}
			// > 1st line
			else{
				QString ind(SplitedLine[0]);
				tab->setRowCount(cpt_lines);
				// allocate the 2D vector for the current indiv (ind). // See the m_parents map description in the header file.
				if (parents){
					// nb_qtl+1 because of the "All" columns before QTL columns.
					m_parents[ind]=QVector<QVector<QString> >( nb_qtl+1, QVector<QString> (parents) );
					//qDebug()<<"nb ind "<<m_parents.size()<<" ; nb QTLs "<<m_parents[ind].size();
				}
				else { // parents=0 => parsing tab_homo_hetero.txt => fill the field MS of m_homo_fav, m_hetero, m_homo_unfav.
					if ( m_homo_fav.find(ind) != m_homo_fav.end() ) cerr<<"OUPS ! this indiv already exists in m_homo_fav: "<<ind.toStdString()<<endl; //// for debugging only
					// MS values
					m_homo_fav[ind].push_back(QVector<QString>(2));
					m_homo_fav[ind][0][0] = SplitedLine[3];
					m_homo_fav[ind][0][1] = SplitedLine[4];
					m_homo_unfav[ind].push_back(QVector<QString>(2));
					m_homo_unfav[ind][0][0] = SplitedLine[3];
					m_homo_unfav[ind][0][1] = SplitedLine[5];
					m_hetero[ind].push_back(QVector<QString>(2));
					m_hetero[ind][0][0] = SplitedLine[3];
					m_hetero[ind][0][1] = SplitedLine[6];
				}
				for(int i=0; i<ncol; i++){
					QTableWidgetItem *newItem = NULL;
					//if (i>2) { newItem = new QTableWidgetItem(SplitedLine[i].left(6)); }// 4 decimal digits.
					if (i>2) { newItem = new QTableWidgetItem( QString().setNum(SplitedLine[i].toDouble(), 'f', 4) ); }// 4 decimal digits.
					else newItem = new QTableWidgetItem(SplitedLine[i]);
					newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
					tab->setItem(cpt_lines - 1, i, newItem);

					if (parents && i>3){
							//qDebug()<<"ind "<<SplitedLine[0]<<" ; i_qtl "<<(i-4)/parents<<" ; col "<<(i-4)%parents<<" ; p "<<SplitedLine[i];
							m_parents[ind][(i-4)/parents][(i-4)%parents] = SplitedLine[i];
					}
					if( horizontalHeaders[i].compare("MS") == 0){ // MS
						((QTableWidgetItem *)tab->item(cpt_lines - 1, i))->setTextColor(Qt::red);
					}
					else if( horizontalHeaders[i].compare("Weight") == 0 ){
						((QTableWidgetItem *)tab->item(cpt_lines - 1, i))->setTextColor(Qt::blue);
					}
				}
			}
			cpt_lines++;
		}
	}
	else {
		ui.scorePage->setDisabled(false); // Table of the score page
		throw name_file;
	}

	tabFile.close();
	tab->setSortingEnabled(true);
	tab->setAlternatingRowColors(true);
	tab->resizeColumnsToContents();
	tab->resizeRowsToContents();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: plot the left graph (histogram) of the score page. Evolution
	of the mean favorable frequency regarding generation of selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotLeftGraphScorePage(){

	// Left graph based on the molecular score table
	double pos = 0.0, width = 20.0, x_width = 500.0;
	int nb_generation = (int)m_vect_generation.size() - 1, nb_all_qtl = ui.scoreTableWidget->columnCount() - COL_FIRST_QTL + 1, cpt = 0, cpt_histo = 0;
	int numValues = (nb_generation * nb_all_qtl) + (nb_all_qtl-1); // nb_generation * nb_qtl (0.115125	0.091708	0.108203) [number of histograms]

	// ATTENTION CHANGER CODE AVEC ENUM POUR NE PAS LE LIMITER A 10 COULEURS !! (Must: utilisateur peut changer sa couleur puis update) <= ;) (clin d'oeil bruyant) t'inquiète t'as vu !
	//QList<Qt::GlobalColor> vect_color = (QList<Qt::GlobalColor>()<<Qt::red<<Qt::green<<Qt::blue<<Qt::darkCyan<<Qt::darkRed<<Qt::yellow<<Qt::black<<Qt::cyan<<Qt::darkGreen<<Qt::darkBlue<<Qt::darkMagenta<<Qt::magenta);
	//QStringList vect_code_color = (QStringList()<<"#ff0000"<<"#00ff00"<<"#0000ff"<<"#008080"<<"#800000"<<"#ffff00"<<"#000000"<<"#00ffff"<<"#008000"<<"#000080"<<"#800080"<<"#ff00ff");

	// list_refcolor is an array of 6 reference QColor object (R,G,B,C,M,Y), from which we could define supplementary colors if needed.
	//QList<QColor> list_refcolor; list_refcolor<<QColor(Qt::red)<<QColor(Qt::green)<<QColor(Qt::blue)<<QColor(Qt::cyan)<<QColor(Qt::magenta)<<QColor(Qt::yellow);
	//int nRefCol = list_refcolor.size();

	QString text = QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
			"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
			"p, li { white-space: pre-wrap; }\n"
			"</style></head><body style=\" font-family:'Sans Serif'; font-size:"TEXT_SIZE"; font-weight:400; font-style:normal;\">\n"
			"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">No. individuals = %1\t\tNo. QTL = %2</p></body></html>").arg((int)m_map_scores.size(), 0, 10).arg(nb_all_qtl-1, 0, 10);

	// Set the number of individuals and QTL in the population studied
	//ui.graphLeftMeanLabel2->setText(QApplication::translate("MainWindow", text.c_str(), 0, QApplication::UnicodeUTF8));
	ui.graphLeftMeanLabel2->setText(text);

	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->enableXMin(true);
	grid->enableX(false);
	grid->enableYMin(true);
	grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
	grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
	grid->attach(ui.graphLeftQwtPlot2);

	// The histogram is resized (if too big)
	if( numValues > (x_width / width) ){
		width = x_width / numValues;
	}

	// Vector of intervals for all qtl
	QwtArray<QwtDoubleInterval> intervals(numValues);

	for(int i=0; i< nb_all_qtl; i++){
		for(int j=0; j<nb_generation; j++){
			intervals[cpt] = QwtDoubleInterval(pos, pos + double(width));
			pos += width;
			cpt++;
		}
		// White space in histogram (between each qtl)
		if( i != (nb_all_qtl-1)){
			intervals[cpt] = QwtDoubleInterval(pos, pos + double(width));
			pos += width;
			cpt++;
		}
	}

	cpt = 0;

	// Vector of histograms for each qtl (each generation)
	QVector<HistogramItem *> vect_histo(nb_generation * nb_all_qtl);

	// Loop on All/ each QTL
	for(int i=0; i< (nb_all_qtl + 1); i++){
		// Weight column
		if( i != 1 ){
			// Loop on generations
			for(int j=0; j<nb_generation; j++){
				// Creation of an histogram
				vect_histo[cpt_histo] = new HistogramItem(m_vect_generation[j+1]); // m_vect_generation[j+1] as a title of the histogram (j+1 to skip "All").

				// Values found in the output files of OptiMAS
				QwtArray<double> values(numValues, 0.0);

				////vect_histo[cpt_histo]->setColor(vect_color[j]);
				vect_histo[cpt_histo]->setColor(refColors::getRefColor(j));
				values[cpt] = m_map_gene_scores[m_vect_generation[j+1]][i+1];

				// Histograms are superimposed
				vect_histo[cpt_histo]->setData(QwtIntervalData(intervals, values));
				vect_histo[cpt_histo]->attach(ui.graphLeftQwtPlot2);
				cpt_histo++;
				cpt++;
			}
			// White space in histogram (between each qtl)
			if( i != nb_all_qtl){
				cpt++;
			}
		}
	}

	// The legend is placed regarding histograms position
	vector<double> vect_legend(nb_all_qtl, 0.0);

	for(int i=0; i<nb_all_qtl; i++){
		//vect_legend[i] = ((nb_generation * width) * (i + 1)) + (width * i) - ((width * nb_generation) / 2.0);
		vect_legend[i] = (width*( 2*nb_generation*(i+1) + 2*i - nb_generation))/2.0;
	}

	// Legend
	QString legend, perso = "", end = "</p></body></html>", leg = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
			"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
			"p, li { white-space: pre-wrap; }\n"
			"</style></head><body style=\" font-family:'Sans Serif'; font-size:"TEXT_SIZE"; font-weight:400; font-style:normal;\">\n"
			"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">- <span style=\" text-decoration: underline;\">Generation</span>: ";

	for( int i = 0; i<nb_generation; i++){
		perso += "<span style=\" color:" + refColors::getRefColor(i).name() + trUtf8(";\">\342\231\246") + " " + m_vect_generation[i+1] + " </span><span style=\" color:#000000;\">- </span>";
	}
	legend = leg + perso + end;

	//ui.graphLeftQwtPlot2->setTitle(QApplication::translate("MainWindow", legend.c_str(), 0, QApplication::UnicodeUTF8));
	ui.graphLeftQwtPlot2->setTitle(legend);

	ui.graphLeftQwtPlot2->setAxisScaleDraw(QwtPlot::xBottom, new MyScaleDraw(vect_legend));

	ui.graphLeftQwtPlot2->setAxisScale(QwtPlot::yLeft, 0.0, 1.0);
	ui.graphLeftQwtPlot2->setAxisScale(QwtPlot::xBottom, 0.0, x_width, width/2.0);
	ui.graphLeftQwtPlot2->setAxisTitle(QwtPlot::yLeft, "Frequency of favorable alleles");
	ui.graphLeftQwtPlot2->setAxisTitle(QwtPlot::xBottom, "QTL");
	ui.graphLeftQwtPlot2->setCanvasBackground(QColor(Qt::white));
	ui.graphLeftQwtPlot2->replot();
}


void MainWindow::print(QwtPlot* plot, const QString& fileName){
    //QPrinter printer(QPrinter::HighResolution);
    QPrinter printer;		// default printer mode is QPrinter::ScreenResolution.

    printer.setOutputFileName(fileName);
    if (fileName.endsWith(".pdf")) printer.setOutputFormat(QPrinter::PdfFormat);
    else printer.setOutputFormat(QPrinter::PostScriptFormat);

    QString docName = plot->title().text();
    if ( !docName.isEmpty() ) {
        docName.replace (QRegExp (QString::fromLatin1 ("\n")), tr (" -- "));
        printer.setDocName (docName);
    }
    printer.setCreator("OptiMAS");
    printer.setOrientation(QPrinter::Landscape);

	QwtPlotPrintFilter filter;
	if ( printer.colorMode() == QPrinter::GrayScale )	// ???
	{
		int options = QwtPlotPrintFilter::PrintAll;
		options &= ~QwtPlotPrintFilter::PrintBackground;
		options |= QwtPlotPrintFilter::PrintFrameWithScales;
		filter.setOptions(options);
	}
	plot->print(printer, filter);
	//QPainter paint(&printer);
	//Plot2D->print(&paint,Plot2D->rect(), filter);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QwtPlot* plot : the address of a QwtPlot object.
	## RETURN:
	## SPECIFICATION: the given graph plot is saved in a PNG format.
	-----------------------------------------------------------------------------
*/

void MainWindow::saveGraph(QwtPlot* plot){

	QString filter = tr("PNG files (*.png);;SVG files (*.svg);;Encapsuled PostScript Files (*.eps)");
	//QFileDialog fileDialog(this, "Export Graph", QDir::currentPath(), filter);
	ExportGraphDialog fileDialog(this, "Export Graph", m_lastGraphSave, filter);
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);

	if(fileDialog.exec() == QDialog::Accepted){
		QString fileName = fileDialog.selectedFiles().value(0);

		if( fileName.isEmpty() ){
			//QMessageBox::information(this, tr("Save graph"), tr("Cannot load %1.").arg(fileName));
			return;
		}

		QString selected_filter = fileDialog.selectedNameFilter(), ext = selected_filter.section('.', -1);
		ext.chop(1); // skip parenthesis

		if( fileName.section('.', -1) != ext ){
			fileName+= "." + ext;	// add the extention to filname if needed.
		}

		m_lastGraphSave = fileName.section(QRegExp("[/\\\\]"), 0,-2); // sep is either '/' on unix like OS, or '\' on win OS.
		if (m_lastGraphSave.isEmpty()) m_lastGraphSave = "/";

		QStringList res = fileDialog.selectedResolution().split('x');

		if( fileName.endsWith(".png") ){
			QPixmap pic(QSize(res[0].toInt(), res[1].toInt()));
			pic.fill(Qt::white);
			plot->print(pic);
			pic.save(fileName, "PNG");
		}
		else if(fileName.endsWith(".svg")){
			#ifdef	QT_SVG_LIB
			QSvgGenerator generator;
			generator.setFileName(fileName);
			generator.setSize( QSize( res[0].toInt(), res[1].toInt() ) );

			//plot->print(generator);
			QPainter paint(&generator);
			paint.fillRect(QRect(0, 0, res[0].toInt(), res[1].toInt()), Qt::white);
			//paint.setRenderHint(QPainter::Antialiasing);
			plot->print(&paint,plot->rect());
			#endif
		}
		else if(fileName.endsWith(".eps")){
			print(plot, fileName);
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the left graph (evolution of allele frequency regarding
	generation of selection) of the score page is saved in a PNG format.
	-----------------------------------------------------------------------------
*/

void MainWindow::saveGraphLeftScorePage(){
	saveGraph(ui.graphLeftQwtPlot2);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_plot: the index of the selected plot(histogram or boxplot).
	## RETURN:
	## SPECIFICATION: plot the corresponding graph (histogram or boxplot) of the
	score page after user selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotRightGraphScorePage(int index_plot){
	// Histogram
	if( index_plot == 0 ){
		plotHistoScorePage(ui.graphRightQtlComboBox2->currentIndex());
	}
	// BoxPlot
	else if( index_plot == 1 ){
		plotBoxPlotScorePage(ui.graphRightQtlComboBox2->currentIndex());
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_qtl: the index of the QTL selected that we want to plot.
	## RETURN:
	## SPECIFICATION: plot the corresponding graph (histogram or boxplot) of the
	score page after user QTL selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotRightGraphScorePageQtl(int index_qtl){
	// Histogram
	if( ui.graphRightParamsComboBox2->currentIndex() == 0 ){
		plotHistoScorePage(index_qtl);
	}
	// BoxPlot
	else if( ui.graphRightParamsComboBox2->currentIndex() == 1 ){
		plotBoxPlotScorePage(index_qtl);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_qtl: the index of the QTL selected that we want to plot.
	## RETURN:
	## SPECIFICATION: plot the right graph (histogram) of the score page.
	Distribution of the individuals for each QTL regarding the molecular score.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotHistoScorePage(int index_qtl){

	if( ! m_map_gene_quantiles.empty() ){

		// Right graph based on the molecular score table
		double pos = 0.0, width = 0.05, mean = m_map_gene_scores["All"][index_qtl + 1], var = 0.0;
		int numValues = (int)m_map_gene_quantiles["All"][index_qtl].size(); // [0.0 - 1.0] step = 0.05

		ui.graphRightQwtPlot2->detachItems();

		QwtPlotGrid *grid = new QwtPlotGrid();
		grid->enableXMin(true);
		grid->enableX(false);
		grid->enableYMin(true);
		grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
		grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
		grid->attach(ui.graphRightQwtPlot2);

		HistogramItem *histogram = new HistogramItem();
		histogram->setColor(Qt::darkCyan);

		// Vector of intervals for all quantiles
		QwtArray<QwtDoubleInterval> intervals(numValues);
		QwtArray<double> values(numValues);

		for(int i=0; i<numValues; i++){
			intervals[i] = QwtDoubleInterval(pos, pos + double(width));
			values[i] = m_map_gene_quantiles["All"][index_qtl][i];
			pos += width;
		}

		histogram->setData(QwtIntervalData(intervals, values));
		histogram->attach(ui.graphRightQwtPlot2);

		// Variance
		for( QMap<QString, QStringList>::iterator iter_list = m_map_scores.begin(); iter_list != m_map_scores.end(); ++iter_list ){
			const QStringList & ind_data = iter_list.value(); // individual of the list
			// All / Weight
			if( index_qtl < 2 ){
				var += pow(ind_data[index_qtl + COL_MS - 1].toDouble() - mean, 2.0);
			}
			else{
				var += pow(ind_data[index_qtl + COL_FIRST_QTL - 3].toDouble() - mean, 2.0);
			}
		}

		var = var / (double)m_map_gene_scores["All"][0];

		setLabelScorePage(ui.graphRightMeanLabel2, mean, var);

		QwtScaleDraw* scaleDraw = new QwtScaleDraw();
		scaleDraw->setTickLength(QwtScaleDiv::MediumTick, scaleDraw->majTickLength()/2);
		ui.graphRightQwtPlot2->setAxisScaleDraw( QwtPlot::xBottom, scaleDraw);
		ui.graphRightQwtPlot2->setAxisMaxMinor(QwtPlot::xBottom, 2);
		ui.graphRightQwtPlot2->setAxisScale(QwtPlot::yLeft, 0.0, m_map_gene_scores["All"][0]);
		ui.graphRightQwtPlot2->setAxisScale(QwtPlot::xBottom, 0.0, 1.0, 0.1);
		ui.graphRightQwtPlot2->setAxisTitle(QwtPlot::yLeft, "Number of Individuals");
		ui.graphRightQwtPlot2->setAxisTitle(QwtPlot::xBottom, "Molecular Score (MS)");
		ui.graphRightQwtPlot2->setCanvasBackground(QColor(Qt::white));
		ui.graphRightQwtPlot2->replot();
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_qtl: the index of the QTL selected that we want to plot.
	## RETURN:
	## SPECIFICATION: plot the right graph (boxplot) of the score page.
	Intrapopulation variation (group) regarding the molecular score.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotBoxPlotScorePage(int index_qtl){

	ui.graphRightMeanLabel2->setText("");

	ui.graphRightQwtPlot2->detachItems();
	ui.graphRightQwtPlot2->setCanvasBackground(QColor(Qt::white)); // background color
	//plot.setTitle("Variation intra epis");

	QwtPlotGrid *grid = new QwtPlotGrid; // a grid
	grid->enableXMin(true);	// minor vertical lines of the grid
	grid->enableYMin(true); // minor horizontal
	grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine)); // grid major lines color
	grid->setMinPen(QPen(Qt::lightGray, 0 , Qt::DotLine)); // grid minor lines color
	grid->attach(ui.graphRightQwtPlot2); // link the grid with the plot
	grid->enableX(false);

/* **** faire une fonction.
	if(m_vect_group_scores[index_qtl].empty()){ // => first call of this fonction with THIS index_qtl.

		// Map_ref iqtl_grps_scores: key = a grp; value = a list of scores (at index_qtl) of all the indivs belonging to this grp.
		//QMap<QString, QVector<double> > & iqtl_grps_scores = m_vect_group_scores[index_qtl];
		QMap <QString, QStringList>::const_iterator it_grp, it_ind;

		for(it_grp = m_map_group.begin(); it_grp != m_map_group.end(); ++it_grp){	// for each group
		
			const QString & grp = it_grp.key();
			const QStringList & vect_indiv = it_grp.value();
			int nind = vect_indiv.size();
			
			// Vector_ref grp_scores stores all indivs scores belonging to this group at the pos index_qtl.
			// grp_scores is empty at this moment.
			//QVector<double> & grp_scores = iqtl_grps_scores[grp]; //// si operator[] ne retourne pas une ref, peut etre casse-gueule !?
			//QVector<double> & grp_scores = iqtl_grps_scores.find(grp).value(); // value() retourne une ref. OK :)
			//grp_scores.resize(nind);
			m_vect_group_scores[index_qtl][grp].resize(nind);
			
			for (int i_ind=0; i_ind<nind; ++i_ind){ // fill grp_scores.

				it_ind = m_map_scores.find(vect_indiv[i_ind]);
				if(it_ind != m_map_scores.end()){
					const QStringList & ind_data = it_ind.value();
					// grp_scores[i_ind] = ind_data[index_qtl+2].toDouble(); //// a quoi ca sert ??
					// All / Weight
					if( index_qtl < 2 ){
						//grp_scores[i_ind] = ind_data[index_qtl + COL_MS - 1].toDouble();
						m_vect_group_scores[index_qtl][grp][i_ind] = ind_data[index_qtl + COL_MS - 1].toDouble();
					}
					// QTL1...QTLn
					else{
						//grp_scores[i_ind] = ind_data[index_qtl + COL_FIRST_QTL - 3].toDouble();
						m_vect_group_scores[index_qtl][grp][i_ind] = ind_data[index_qtl + COL_FIRST_QTL - 3].toDouble();
					}
				}
			}
		}
	}
**** */
	BoxPlotItem* BoxPlot = new BoxPlotItem("MS = f(grp)");

	BoxPlot->setData( m_map_scores, m_map_group, index_qtl, 1);
	//BoxPlot->setData( m_vect_group_scores[index_qtl], 1);

	BoxPlot->attach(ui.graphRightQwtPlot2); // link the BoxPlotitem with the plot
	BoxPlot->setColor(Qt::darkCyan);

	CustomScaleDraw* scaleDraw = new CustomScaleDraw(m_map_group, 1);
	scaleDraw->setTickLengths(0,0,0); // no Ticks and no  Backbone.
	ui.graphRightQwtPlot2->setAxisScaleDraw( QwtPlot::xBottom, scaleDraw);
	// NB: No need to explicitly delete scaleDraw. See the Qwt user guide statement about void QwtPlot::setAxisScaleDraw():
	// => "Please note that scaleDraw has to be created with new and will be deleted by the corresponding QwtScale member (like a child object)."

	ui.graphRightQwtPlot2->setAxisScale(QwtPlot::yLeft, 0.0, 1);
	ui.graphRightQwtPlot2->setAxisScale(QwtPlot::xBottom, 0.0, m_map_group.size()+1, 1);
	ui.graphRightQwtPlot2->setAxisTitle(QwtPlot::yLeft, "Molecular Score (MS)");
	ui.graphRightQwtPlot2->setAxisTitle(QwtPlot::xBottom, "Group");
	ui.graphRightQwtPlot2->replot();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the right graph (histogram) of the score page is saved in
	a PNG format.
	-----------------------------------------------------------------------------
*/

void MainWindow::saveGraphRightScorePage(){
	saveGraph(ui.graphRightQwtPlot2);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QLabel *label: graph label (Mean - Var)
	@ double mean: the mean of molecular score for all individuals
	@ double var: the variance of molecular score for all individuals
	## RETURN:
	## SPECIFICATION: display the label (legend) in the score page (histogram).
	-----------------------------------------------------------------------------
*/

void MainWindow::setLabelScorePage(QLabel *label, double mean, double var){

	if( label == ui.graphRightMeanLabel2 ){
		QString mean_str = QString(" ").setNum(mean), var_str = QString(" ").setNum(var);

		QString text = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
				"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
				"p, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Sans Serif'; font-size:"TEXT_SIZE"; font-weight:400; font-style:normal;\">\n"
				"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Mean = " + mean_str + "\tVar = " + var_str + "</p></body></html>";
		label->setText(text);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QLabel *label: graph label (Mean - Var)
	@ double mean: the mean of molecular score for the list (selection)
	@ double var: the variance of molecular score for the list (selection)
	## RETURN:
	## SPECIFICATION: display the label (legend) in the different lists of
	selection (graph).
	-----------------------------------------------------------------------------
*/

void MainWindow::setLabelSelectionPage(QLabel *label, double mean, double var){

	if( (label == ui.graphLeftMeanLabel6) || (label == ui.graphRightMeanLabel6) ){
		QString mean_str = QString(" ").setNum(mean), var_str = QString(" ").setNum(var);

		QString text = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
		"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
		"p, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Sans Serif'; font-size:"TEXT_SIZE"; font-weight:400; font-style:normal;\">\n"
		"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Mean = " + mean_str + "\tVar = " + var_str + "</p></body></html>";

		label->setText(text);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QLabel *label1: graph label (No.Id - Mean)
	@ QLabel *label2: graph label (No.Pop - Var)
	@ double mean: the mean of molecular score for the list (selection)
	@ double var: the variance of molecular score for the list (selection)
	@ int nb_ind: the number of individuals in the list (selection)
	@ int nb_grp: the number of groups in the list (selection)
	## RETURN:
	## SPECIFICATION: display the label (legend) in the different lists of
	selection (histograms).
	-----------------------------------------------------------------------------
*/

void MainWindow::setLabelSelectionPage(QLabel *label1, QLabel *label2, double mean, double var, int nb_ind, int nb_grp){

	if( ((label1 == ui.leftList1MeanLabel6) && (label2 == ui.leftList2MeanLabel6)) || ((label1 == ui.rightList1MeanLabel6) && (label2 == ui.rightList2MeanLabel6)) ){
		QString nb_ind_str = QString(" ").setNum(nb_ind), nb_grp_str = QString(" ").setNum(nb_grp);
		QString mean_str = QString(" ").setNum(mean), var_str = QString(" ").setNum(var);

		QString text1 = "No. ind = " + nb_ind_str + "\nMean = " + mean_str;
		QString text2 = "No. group = " + nb_grp_str + "\nVar = " + var_str;

		label1->setText(text1);
		label2->setText(text2);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QFrame *frame: frame containing the crosses notes labels.
	@ int index_list_crosses: the index of the selected list of crosses.
	## RETURN:
	## SPECIFICATION: display the labels (legend) of the crossing list.
	-----------------------------------------------------------------------------
*/

void MainWindow::setLabelCrossPage(QFrame* frame, int index_list_crosses){

	const QStringList & vect_labels = m_vect_notes_crosses[index_list_crosses];

	QList<QLabel*> labels = frame->findChildren<QLabel *>();
	if(!labels.isEmpty()){
		if (vect_labels[0].isEmpty()){
			labels[0]->setText("Method:");
			labels[1]->setText("List:");
			labels[2]->setText("Criterion:");
		}
		else{
			labels[0]->setText(vect_labels[0]);
			labels[1]->setText(vect_labels[1]);
			labels[2]->setText(vect_labels[2]);
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QLabel *label: graph label (Mean - Var)
	@ double mean: the mean of molecular score for the list (crosses)
	@ double var: the variance of molecular score for the list (crosses)
	## RETURN:
	## SPECIFICATION: display the label (legend) in the different lists of
	crosses (graph).
	-----------------------------------------------------------------------------
*/

void MainWindow::setLabelCrossPage(QLabel *label, double mean, double var){

	if( (label == ui.graphLeftMeanLabel4) || (label == ui.graphRightMeanLabel4) ){
		QString mean_str = QString(" ").setNum(mean), var_str = QString(" ").setNum(var);

		QString text = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
		"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
		"p, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Sans Serif'; font-size:"TEXT_SIZE"; font-weight:400; font-style:normal;\">\n"
		"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Mean = " + mean_str + "\tVar = " + var_str + "</p></body></html>";

		label->setText(text);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_qtl: the index of the selected QTL.
	## RETURN:
	## SPECIFICATION: the left graph of the current list of selection is plotted
	regarding the QTL selected.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotLeftGraphSelectionPageQtl(int index_qtl){
	if( ! m_vect_map_selection.empty() ){
		plotGraphSelectionPage(ui.graphLeftQwtPlot6, ui.graphLeftListComboBox6->currentIndex(), index_qtl);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_qtl: the index of the selected QTL.
	## RETURN:
	## SPECIFICATION: the right graph of the current list of selection is plotted
	regarding the QTL selected.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotRightGraphSelectionPageQtl(int index_qtl){
	if( ! m_vect_map_selection.empty() ){
		plotGraphSelectionPage(ui.graphRightQwtPlot6, ui.graphRightListComboBox6->currentIndex(), index_qtl);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: the index of the selected list (selection).
	## RETURN:
	## SPECIFICATION: the left graph for the current QTL is plotted regarding
	the list selected.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotLeftGraphSelectionPageList(int index_list_selected){
	if( ! m_vect_map_selection.empty() ){
		plotGraphSelectionPage(ui.graphLeftQwtPlot6, index_list_selected, ui.graphLeftQtlComboBox6->currentIndex());
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: the index of the selected list (selection).
	## RETURN:
	## SPECIFICATION: the right graph for the current QTL is plotted regarding
	the list selected.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotRightGraphSelectionPageList(int index_list_selected){
	if( ! m_vect_map_selection.empty() ){
		plotGraphSelectionPage(ui.graphRightQwtPlot6, index_list_selected, ui.graphRightQtlComboBox6->currentIndex());
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QwtPlot *graph: the graph where the results will be plotted.
	@ int index_list_selected: the index of the selected list (selection).
	@ int index_qtl: the index of the selected QTL.
	## RETURN:
	## SPECIFICATION: for the selected list, the distribution of individuals
	regarding the molecular score (histogram) is plotted into graphs (selection).
	-----------------------------------------------------------------------------
*/

void MainWindow::plotGraphSelectionPage(QwtPlot *graph, int index_list_selected, int index_qtl){

	if( ! m_vect_map_selection.empty() ){
		int numValues = 20;//(int)m_map_gene_quantiles["All"][index_qtl].size(); // [0.0 - 1.0] step = 0.05 (20)
		double d, pos = 0.0, width = 0.05, mean = 0.0, var = 0.0;
		vector<int> vect_quantiles(numValues, 0);

		QMap<QString, int> & map_selected_list_i = *(m_vect_map_selection[index_list_selected]);
		
		//// je crois qu'on peut calculer mean et var avec une seule boucle (formule altenative equivalente (voir ds coda)).
		// Loop on individuals
		for( QMap<QString, int>::iterator iter_list = map_selected_list_i.begin(); iter_list != map_selected_list_i.end(); ++iter_list ){
			const QString & key = iter_list.key(); // individual of the list

			// All / Weight
			if( index_qtl < 2 ){
				d = m_map_scores[key][index_qtl + COL_MS - 1].toDouble();
			}
			else{
				d = m_map_scores[key][index_qtl + COL_FIRST_QTL - 3].toDouble();
			}
			vect_quantiles[whichQuantile(d)] += 1;
			mean += d; // Mean score value
		}

		if( ((int)map_selected_list_i.size()) != 0 ){
			mean = mean / (double)map_selected_list_i.size();
		}
		else{
			mean = 0.0;
		}

		// Variance
		for( QMap<QString, int>::iterator iter_list = map_selected_list_i.begin(); iter_list != map_selected_list_i.end(); ++iter_list ){
			const QString & key = iter_list.key(); // individual of the list
			// All / Weight
			if( index_qtl < 2 ){
				d = m_map_scores[key][index_qtl + COL_MS - 1].toDouble();
			}
			else{
				d = m_map_scores[key][index_qtl + COL_FIRST_QTL - 3].toDouble();
			}
			var += pow(d - mean, 2.0);
		}

		if( ((int)map_selected_list_i.size()) != 0 ){
			var = var / (double)map_selected_list_i.size();
		}
		else{
			var = 0.0;
		}

		if( graph == ui.graphLeftQwtPlot6 ){
			setLabelSelectionPage(ui.graphLeftMeanLabel6, mean, var);
		}
		else if( graph == ui.graphRightQwtPlot6 ){
			setLabelSelectionPage(ui.graphRightMeanLabel6, mean, var);
		}

		graph->detachItems();

		QwtPlotGrid *grid = new QwtPlotGrid();
		grid->enableXMin(true);
		grid->enableX(false);
		grid->enableYMin(true);
		grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
		grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
		grid->attach(graph);

		HistogramItem *histogram = new HistogramItem();
		histogram->setColor(Qt::darkCyan);

		// Vector of intervals for all quantiles
		QwtArray<QwtDoubleInterval> intervals(numValues);
		QwtArray<double> values(numValues);

		for(int i=0; i<numValues; i++){
			intervals[i] = QwtDoubleInterval(pos, pos + double(width));
			values[i] = vect_quantiles[i];
			pos += width;
		}

		histogram->setData(QwtIntervalData(intervals, values));
		histogram->attach(graph);

		QwtScaleDraw* scaleDraw = new QwtScaleDraw();
		scaleDraw->setTickLength(QwtScaleDiv::MediumTick, scaleDraw->majTickLength()/2);
		graph->setAxisScaleDraw( QwtPlot::xBottom, scaleDraw);
		graph->setAxisMaxMinor(QwtPlot::xBottom, 2);

		graph->setAxisScale(QwtPlot::yLeft, 0.0, (double)map_selected_list_i.size());
		graph->setAxisScale(QwtPlot::xBottom, 0.0, 1.0, 0.1);
		graph->setAxisTitle(QwtPlot::yLeft, "Number of Individuals");
		graph->setAxisTitle(QwtPlot::xBottom, "Molecular Score (MS)");
		graph->setCanvasBackground(QColor(Qt::white));
		graph->replot();
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the left graph (histogram) of the selection page is saved in
	a PNG format.
	-----------------------------------------------------------------------------
*/

void MainWindow::saveGraphLeftSelectionPage(){
	saveGraph(ui.graphLeftQwtPlot6);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the right graph (histogram) of the selection page is saved in
	a PNG format.
	-----------------------------------------------------------------------------
*/

void MainWindow::saveGraphRightSelectionPage(){
	saveGraph(ui.graphRightQwtPlot6);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the new molecular score (weight - blue column) is updated
	regarding the new values entered by the user.
	-----------------------------------------------------------------------------
*/

void MainWindow::runComputeWeight(){

	double sum_weights = 0.0, new_indiv_score = 0.0, mean = 0.0;
	int nb_qtl = ui.scoreTableWidget->columnCount() - COL_FIRST_QTL, index_weight = 1;
	bool uniform = true; // true if all QTL weights are equal.

	ui.scoreTableWidget->setSortingEnabled(false);

	// New total weight (sum of all QTL)
	for(int q = 0; q < nb_qtl; q++){
		if (m_vect_qtl_weight[q] != m_vect_qtl_weight[0]) uniform = false;
		sum_weights += m_vect_qtl_weight[q];
	}

	// The map of quantiles is reseted
	QVector<int> & quantiles = m_map_gene_quantiles["All"][index_weight];
	for(int i=0; i< (int)quantiles.size(); i++)	quantiles[i] = 0;

	// Loop on individuals
	for(int i = 0; i < ui.scoreTableWidget->rowCount(); i++){
		new_indiv_score = 0.0;
		QString ind = ((QTableWidgetItem *)ui.scoreTableWidget->item(i, 0))->text();

		// The weight column is update
		if (uniform) {
			double ms = m_map_scores[ind][COL_MS - 1].toDouble();
			ui.scoreTableWidget->item(i, COL_WEIGHT)->setText( ui.scoreTableWidget->item(i, COL_MS)->text() );
			m_map_scores[ind][COL_WEIGHT - 1] = QString(" ").setNum(ms, 'f', 6);
			m_map_gene_quantiles["All"][index_weight][whichQuantile(ms)] += 1;
			mean += ms;
		}
		else {
			// Loop on each QTL
			for(int q = 0; q < nb_qtl; q++) {
				// The new score with weights is computed
				new_indiv_score += ( m_map_scores[ind][q + COL_FIRST_QTL - 1].toDouble() * m_vect_qtl_weight[q]);
			}
			((QTableWidgetItem *)ui.scoreTableWidget->item(i, COL_WEIGHT))->setText( QString(" ").setNum(new_indiv_score / sum_weights, 'f', 4) );
			m_map_scores[ind][COL_WEIGHT - 1] = QString(" ").setNum(new_indiv_score / sum_weights, 'f', 6);
			m_map_gene_quantiles["All"][index_weight][whichQuantile(new_indiv_score / sum_weights)] += 1;
			mean += new_indiv_score / sum_weights;
		}
	}

	// Update weight mean
	m_map_gene_scores["All"][index_weight + 1] = mean / ui.scoreTableWidget->rowCount();

	if( ui.graphRightQtlComboBox2->currentIndex() == index_weight ){
		plotRightGraphScorePage(index_weight);
	}
	ui.scoreTableWidget->setSortingEnabled(true);

	// Update of graphs if the current list was selected
	displayLeftListSelection(ui.leftListComboBox6->currentIndex());
	displayRightListSelection(ui.rightListComboBox6->currentIndex());

	if( ui.graphLeftQtlComboBox6->currentIndex() == index_weight ){
		plotGraphSelectionPage(ui.graphLeftQwtPlot6, ui.graphLeftListComboBox6->currentIndex(), ui.graphLeftQtlComboBox6->currentIndex());
	}
	if( ui.graphRightQtlComboBox6->currentIndex() == index_weight ){
		plotGraphSelectionPage(ui.graphRightQwtPlot6, ui.graphRightListComboBox6->currentIndex(), ui.graphRightQtlComboBox6->currentIndex());
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Creation of the "Action" buttons for the right click in
	a table of selection / crosses.
	-----------------------------------------------------------------------------
*/

void MainWindow::createActions(){
	// if(addListAction != NULL) delete addListAction; // createActions() is called only once.
	m_addListAction = new QAction(tr("&Add to list..."), this);
	m_addListAction->setStatusTip(tr("Add the current selection to the chosen list"));
	connect(m_addListAction, SIGNAL(triggered()), this, SLOT(addToList()));

	m_deleteLeftListSelectionAction = new QAction(tr("&Delete..."), this);
	m_deleteLeftListSelectionAction->setStatusTip(tr("Delete the current individuals to the list"));
	connect(m_deleteLeftListSelectionAction, SIGNAL(triggered()), this, SLOT(deleteIndToLeftListSelection()));

	m_deleteRightListSelectionAction = new QAction(tr("&Delete..."), this);
	m_deleteRightListSelectionAction->setStatusTip(tr("Delete the current individuals to the list"));
	connect(m_deleteRightListSelectionAction, SIGNAL(triggered()), this, SLOT(deleteIndToRightListSelection()));

	m_deleteLeftListCrossesAction = new QAction(tr("&Delete..."), this);
	m_deleteLeftListCrossesAction->setStatusTip(tr("Delete the current individuals to the list"));
	connect(m_deleteLeftListCrossesAction, SIGNAL(triggered()), this, SLOT(deleteIndToLeftListCrosses()));

	m_deleteRightListCrossesAction = new QAction(tr("&Delete..."), this);
	m_deleteRightListCrossesAction->setStatusTip(tr("Delete the current individuals to the list"));
	connect(m_deleteRightListCrossesAction, SIGNAL(triggered()), this, SLOT(deleteIndToRightListCrosses()));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: The "Action" button are added to the context menu of the interface.
	-----------------------------------------------------------------------------
*/

void MainWindow::createContextMenu(){
	ui.scoreTableWidget->addAction(m_addListAction);
	ui.scoreTableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

	ui.leftListTableWidget6->addAction(m_deleteLeftListSelectionAction);
	ui.leftListTableWidget6->setContextMenuPolicy(Qt::ActionsContextMenu);

	ui.rightListTableWidget6->addAction(m_deleteRightListSelectionAction);
	ui.rightListTableWidget6->setContextMenuPolicy(Qt::ActionsContextMenu);

	ui.leftListTableWidget4->addAction(m_deleteLeftListCrossesAction);
	ui.leftListTableWidget4->setContextMenuPolicy(Qt::ActionsContextMenu);

	ui.rightListTableWidget4->addAction(m_deleteRightListCrossesAction);
	ui.rightListTableWidget4->setContextMenuPolicy(Qt::ActionsContextMenu);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Initialization of the colors and parameters (cut-off) of the
	visualization window.
	-----------------------------------------------------------------------------
*/

void MainWindow::initVisualizationParameters(){
	for(int i=0; i<4; i++){
		m_vect_color.push_back(ui.findPushButton2->palette().button().color()); // Color to put in the score page regarding the MS at each QTL
	}
	m_vect_cut_off.push_back(0.75); // Proba(+/+) >=
	m_vect_cut_off.push_back(0.75); // Proba(+/-) >=
	m_vect_cut_off.push_back(0.75); // Proba(-/-) >=
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Initialization of the parameters of the truncation
	selection window.
	-----------------------------------------------------------------------------
*/

void MainWindow::initTruncationOption(){
	m_vect_truncation_option.push_back(0); // cycle selected
	m_vect_truncation_option.push_back(0); // "None" cycle selected
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Initialization of the parameters (cut-off) of the
	complementation window.
	-----------------------------------------------------------------------------
*/

void MainWindow::initComplementationParameters(){
	m_vect_complementation_params.push_back(0.47);
	m_vect_complementation_params.push_back(0.0);
	m_vect_complementation_params.push_back(0.0);
	m_vect_complementation_params.push_back(0.0);
	m_vect_complementation_params.push_back(0.0);
	m_vect_complementation_params.push_back(0.0);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Initialization of the parameters of the crosses schemes window.
	-----------------------------------------------------------------------------
*/

void MainWindow::initCrossOption(){
	m_vect_cross_option.push_back(1); // nbCrossesAllRadioButton Checked
	m_vect_cross_option.push_back(1); // indContribAllRadioButton Checked
	m_vect_cross_option.push_back(2); // nbCrossesSpinBox
	m_vect_cross_option.push_back(1); // indContribSpinBox
	m_vect_cross_option.push_back(0); // criterionComboBox CurrentIndex
	m_vect_cross_option.push_back(0); // crossingMethodsComboBox CurrentIndex
	m_vect_cross_option.push_back(1); // selfingSpinBox (Number of selfing process) //// [Obsolete]
	m_vect_cross_option.push_back(0); // Maximum number of crosses to be done with the current list of selection
	m_vect_cross_option.push_back(0); // Maximum number of contribution for each individual with the current list of selection
	m_vect_cross_option.push_back(0); // Intercrossing or Selfing option //// [Obsolete]
	m_vect_cross_option.push_back(0); // Second list of selection checked or not
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION:
	A list of lists of selection appears when individuals are selected to be added.
	-----------------------------------------------------------------------------
*/

void MainWindow::addToList(){

	m_addListDialog = new AddListDialog(m_list_selection, this);
	m_addListDialog->show();
	m_addListDialog->activateWindow();

	connect(m_addListDialog, SIGNAL(okAccepted(int, const QString &)), this, SLOT(addIndToList(int, const QString &)));

	//	delete m_addListDialog;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: index of the selected list.
	## RETURN:
	## SPECIFICATION:
	The selected individuals are added to the selected list. This list is displayed
	in the interface (table of selection).
	-----------------------------------------------------------------------------
*/

void MainWindow::addIndToList(int index_list_selected, const QString & ListName){

	int topRow, bottomRow;

	if (index_list_selected > (int)m_list_selection.size()-1) addListSelection(ListName);

	// Each individuals selected are added to the list of selection previously chosen
	QList<QTableWidgetSelectionRange> listQTableWidgetSelectionRange = ui.scoreTableWidget->selectedRanges();
	
	QMap<QString, int> & map_selected_list_i = *(m_vect_map_selection[index_list_selected]);

	for(int i=0; i<(int)listQTableWidgetSelectionRange.size(); i++){
		topRow = listQTableWidgetSelectionRange[i].topRow();
		bottomRow = listQTableWidgetSelectionRange[i].bottomRow();

		for(int j=topRow; j<=bottomRow; j++){
			// The current selected individual
			QString ind = (ui.scoreTableWidget->item(j, 0))->text();

			// The individual is added to the selected list if it is not already present on it
			if( map_selected_list_i.find(ind) == map_selected_list_i.end() ){
				map_selected_list_i[ind] = index_list_selected;
				////m_vect_map_selection[index_list_selected]->insert(ind,index_list_selected);
			}
		}
	}

	// Update the list of selection if it's the current one shown
	if( ui.leftListComboBox6->currentIndex() == index_list_selected){
		displayListSelection(ui.leftListTableWidget6, index_list_selected);
	}
	if( ui.rightListComboBox6->currentIndex() == index_list_selected){
		displayListSelection(ui.rightListTableWidget6, index_list_selected);
	}

	// Update of the combobox (list of selection) in the crosses page
	if( ui.selectionListComboBox4->currentIndex() == index_list_selected ){
		maxCrossContribChanged(index_list_selected);
	}

	// Update of graphs if the current list was selected
	if( ui.graphLeftListComboBox6->currentIndex() == index_list_selected ){
		plotGraphSelectionPage(ui.graphLeftQwtPlot6, ui.graphLeftListComboBox6->currentIndex(), ui.graphLeftQtlComboBox6->currentIndex());
	}
	if( ui.graphRightListComboBox6->currentIndex() == index_list_selected ){
		plotGraphSelectionPage(ui.graphRightQwtPlot6, ui.graphRightListComboBox6->currentIndex(), ui.graphRightQtlComboBox6->currentIndex());
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ OTableWidget* table, int row: the selection list table emiting the signal, and the inserted row index.
	## RETURN:
	## SPECIFICATION: (SLOT)
	Update the container corresponding to the selection list currently displayed in the table.
	-----------------------------------------------------------------------------
*/

void MainWindow::updateList(OTableWidget* table, int row){
	//qDebug()<<"signal captured table "<<table->objectName()<<" row "<<table->item(row,0)->text();

	int index_list_selected = -1;
	index_list_selected = table == ui.leftListTableWidget6 ? ui.leftListComboBox6->currentIndex() : ui.rightListComboBox6->currentIndex();

	QMap<QString, int> & map_selected_list_i = *(m_vect_map_selection[index_list_selected]);

	// The current selected individual
	QString ind = table->item(row,0)->text();

	// The individual is added to the selected list if it is not already present on it
	if( map_selected_list_i.find(ind) == map_selected_list_i.end() ){
		map_selected_list_i[ind] = index_list_selected;
		////m_vect_map_selection[index_list_selected]->insert(ind, index_list_selected);
	}

	// Update the list of selection if it's the current one shown
	if( ui.leftListComboBox6->currentIndex() == index_list_selected){
		displayListSelection(ui.leftListTableWidget6, index_list_selected);
	}
	if( ui.rightListComboBox6->currentIndex() == index_list_selected){
		displayListSelection(ui.rightListTableWidget6, index_list_selected);
	}

	// Update of the combobox (list of selection) in the crosses page
	if( ui.selectionListComboBox4->currentIndex() == index_list_selected ){
		maxCrossContribChanged(index_list_selected);
	}

	// Update of graphs if the current list was selected
	if( ui.graphLeftListComboBox6->currentIndex() == index_list_selected ){
		plotGraphSelectionPage(ui.graphLeftQwtPlot6, ui.graphLeftListComboBox6->currentIndex(), ui.graphLeftQtlComboBox6->currentIndex());
	}
	if( ui.graphRightListComboBox6->currentIndex() == index_list_selected ){
		plotGraphSelectionPage(ui.graphRightQwtPlot6, ui.graphRightListComboBox6->currentIndex(), ui.graphRightQtlComboBox6->currentIndex());
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int row: the clicked cell row index
	@ int column: the clicked cell column index
	@ OTableWidget: the current table (score or selection)
	## RETURN:
	## SPECIFICATION: (SLOT)
	A more precise information is displayed in a tooltip when a QTL score cell is clicked.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayCellInfo(int row, int column, OTableWidget *tab){

	if (m_homo_fav.empty()) return;

	int i_qtl = 0; // 0 => MS
	QString cellinfo;
	QString ind_name = ((QTableWidgetItem *)tab->item(row, 0))->text();
	QString html_line("<hr align=\"center\" width=\"100%\" color=\"midnightblue\" size=\"3\">");
	const QString & global_MS = m_map_scores[ind_name][COL_MS-1]; // 6 decimal digits

	// ************** GENO PROBA (+/+, +/-, -/- ) ******************
	if (column >= COL_FIRST_QTL){

		i_qtl = column+1-COL_FIRST_QTL;
		const QString & qtl_score = m_map_scores[ind_name][COL_FIRST_QTL+i_qtl-2]; // 6 decimal digits

		// Build the regexp.
		// Example: "s/v/x" gives the following regexp motif: "(s|v|x)" => s or v or x.
		QRegExp regexp (QString("#######"));

		if( !m_vect_fav_all.isEmpty() ){
			regexp = QRegExp(QString("(" + m_vect_fav_all[i_qtl-1] + ")").replace('/','|'));
			cellinfo = "Id: " + ind_name + QString("<li>MS: %1</li><li>QTL%2: %3</li>All+: <b><font color='#22aaff'>%4</font></b>").arg(global_MS).arg(i_qtl, 0, 10).arg(qtl_score).arg(m_vect_fav_all[i_qtl-1]);
		}
		// Reloading analysis case (the .map file can't be reached)
		else{
			cellinfo = "Id: " + ind_name + QString("<li>MS: %1</li><li>QTL%2: %3</li>All+: ?").arg(global_MS).arg(i_qtl, 0, 10).arg(qtl_score);
		}
		//QRegExp regexp( QString("(" + m_vect_fav_all[i_qtl-1] + ")").replace('/','|') );

		//cellinfo = "Id: " + ind_name + QString("<li>MS: %1</li><li>QTL%2: %3</li>All+: <b><font color='#22aaff'>%4</font></b>").arg(global_MS).arg(i_qtl, 0, 10).arg(qtl_score).arg(m_vect_fav_all[i_qtl-1]);
		cellinfo += html_line + "<li><U>Genotype</U></li>";

		QVector<QString> & v_homof = m_homo_fav[ind_name][i_qtl];
		QString homo_fav_proba = v_homof[1].replace(QRegExp("((?:[^ ]+  ){3})"), "\\1</li><li>"); // 3 proba per line max.
		homo_fav_proba.replace(regexp, "<font color='#22aaff'>\\1</font>");
		QString score_homo_fav = "<li>Homo(+/+)=" + v_homof[0] + "<font size='+2'>&nbsp;</font></li><li>" + homo_fav_proba + "</li>";

		QVector<QString> & v_hetero = m_hetero[ind_name][i_qtl];
		QString hetero_proba = v_hetero[1].replace(QRegExp("((?:[^ ]+  ){3})"), "\\1</li><li>");
		hetero_proba.replace(regexp, "<font color='#22aaff'>\\1</font>");
		QString score_hetero = "Hetero(+/-)=" + v_hetero[0] + "<font size='+2'>&nbsp;</font></li><li>" + hetero_proba + "</li>";

		QVector<QString> & v_homouf = m_homo_unfav[ind_name][i_qtl];
		QString homo_unfav_proba = v_homouf[1].replace(QRegExp("((?:[^ ]+  ){3})"), "\\1</li><li>");
		QString score_homo_unfav = "Homo(-/-)=" + v_homouf[0]+"<font size='+2'>&nbsp;</font></li><li>" + homo_unfav_proba + QString("</li>");

		cellinfo +=	score_homo_fav + score_hetero + score_homo_unfav + html_line;
	}
	else if (column == COL_MS){
		QString homof_MS = "<li>Homo(+/+)=" + m_homo_fav[ind_name][0][1] + "<font size='+2'>&nbsp;</font></li>";
		QString hetero_MS = "<li>Hetero(+/-)=" + m_hetero[ind_name][0][1] + "</li>";
		QString homouf_MS = "<li>Homo(-/-)=" + m_homo_unfav[ind_name][0][1] + "</li>";
		cellinfo = QString("Id: %1<li>MS: %2</li>%3<li><U>Genotype</U></li>%4%5%6%7").arg(ind_name).arg(global_MS).arg(html_line).arg(homof_MS).arg(hetero_MS).arg(homouf_MS).arg(html_line);
	}
	else return; // neither MS nor QTLs => do nothing.

	// ******** FOUNDERS PROBA for both MS and QTLs. ***************
	const QVector<QString> & parents_p = m_parents[ind_name][i_qtl];	// parents probabilities for this QTL or MS.
	int parents_nb = parents_p.size();							// nb of parents (2 if biparental)

	// -------------- get the parents allelic codes ----------------
	int all_col = 4;										// first "All" column index in parentsTableWidget.
	QVector<QString> codes(parents_nb); 					// parental allelic codes (s, f, x, d in the "blanc" example).
	for (int i=0; i<parents_nb; i++) {
		codes[i] = ((QTableWidgetItem *)ui.parentsTableWidget->horizontalHeaderItem(all_col+i))->text().mid(4).remove(')');
	}
	// -------------------------------------------------------------
	QString parents_scores("");
	for (int i=0; i<parents_nb; i++) {
		if(i==0) parents_scores += "<li>"+codes[i]+"="+parents_p[i]+"<font size='+2'>&nbsp;</font></li>";
		else parents_scores += "<li>"+codes[i]+"="+parents_p[i]+"</li>";
	}

	cellinfo += QString("<li><U>Founders</U></li>%4").arg(parents_scores);
	// *************************************************************

	//QToolTip::showText(QCursor::pos(), cellinfo, this);
	QWhatsThis::showText(QCursor::pos(), cellinfo, this);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int row, int column: the clicked cell row and column indexes.
	## RETURN:
	## SPECIFICATION: (SLOT)
	A more precise information is displayed in a tooltip when a QTL score cell is
	clicked in the table of scores.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayCellInfoScore(int row, int column){
	displayCellInfo(row, column, ui.scoreTableWidget);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int row, int column: the clicked cell row and column indexes.
	## RETURN:
	## SPECIFICATION: (SLOT)
	A more precise information is displayed in a tooltip when a QTL score cell is
	clicked in the left table/list of selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayCellInfoSelectionLeft(int row, int column){
	displayCellInfo(row, column, ui.leftListTableWidget6);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int row, int column: the clicked cell row and column indexes.
	## RETURN:
	## SPECIFICATION: (SLOT)
	A more precise information is displayed in a tooltip when a QTL score cell is
	clicked in the right table/list of selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayCellInfoSelectionRight(int row, int column){
	displayCellInfo(row, column, ui.rightListTableWidget6);
}



/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int row, int column: the clicked cell row and column indexes.
	## RETURN:
	## SPECIFICATION: (SLOT)
	A more precise information is displayed in a tooltip when a QTL score cell is
	clicked in the left table/list of crosses.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayCellInfoCrossesLeft(int row, int column){
	displayCellInfoCrosses(row, column, ui.leftListTableWidget4);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int row, int column: the clicked cell row and column indexes.
	## RETURN:
	## SPECIFICATION: (SLOT)
	A more precise information is displayed in a tooltip when a QTL score cell is
	clicked in the right table/list of crosses.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayCellInfoCrossesRight(int row, int column){
	displayCellInfoCrosses(row, column, ui.rightListTableWidget4);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int row, int column: the clicked cell row and column indexes.
	@ OTableWidget *tab: the corresponding table of crosses.
	## RETURN:
	## SPECIFICATION: (SLOT)
	A more precise information is displayed in a tooltip when a QTL score cell is clicked.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayCellInfoCrosses(int row, int column, OTableWidget *tab){

	if (m_homo_fav.empty()) return;

	int index_list_crosses;

	if(tab == ui.leftListTableWidget4) index_list_crosses = ui.leftListComboBox4->currentIndex();
	else if(tab == ui.rightListTableWidget4) index_list_crosses = ui.rightListComboBox4->currentIndex();
	else return;

	QMap <QString, QStringList> & map_crosses_list_i = *(m_vect_map_crosses[index_list_crosses]);

	int i_qtl = 0; // 0 => MS
	QString cellinfo;
	QString ind_name = ((QTableWidgetItem *)tab->item(row, 0))->text();
	QString name_p1 = ((QTableWidgetItem *)tab->item(row, 1))->text();
	QString name_p2 = ((QTableWidgetItem *)tab->item(row, 2))->text();
	QString html_line("<hr align=\"center\" width=\"100%\" color=\"midnightblue\" size=\"3\">");
	const QString & global_MS = map_crosses_list_i[ind_name][COL_MS-2]; // 6 decimal digits

	// ************** GENO PROBA (+/+, +/-, -/-) ******************
	if( column >= (COL_FIRST_QTL - 2) ){

		i_qtl = column + 1 - COL_FIRST_QTL + 2;
		const QString & qtl_score = map_crosses_list_i[ind_name][COL_FIRST_QTL+i_qtl-4]; // 6 decimal digits

		// Build the regexp
		// Example: "s/v/x" gives the following regexp motif: "(s|v|x)" => s or v or x
		QRegExp regexp( QString("(" + m_vect_fav_all[i_qtl-1] + ")").replace('/','|') );

		cellinfo = "Id: " + ind_name + QString("<li>MS: %1</li><li>QTL%2: %3</li>All+: <b><font color='#22aaff'>%4</font></b>").arg(global_MS).arg(i_qtl, 0, 10).arg(qtl_score).arg(m_vect_fav_all[i_qtl-1]);
		cellinfo += html_line + "<li><U>Genotype</U></li>";

		// Computation of gametes favorable vs. unfavorable via probabilities to be [+/+] [+/-] [-/-]
		double gam_ind1_plus = m_homo_fav[name_p1][i_qtl][0].toDouble() + (0.5 * m_hetero[name_p1][i_qtl][0].toDouble()), gam_ind1_minus = m_homo_unfav[name_p1][i_qtl][0].toDouble() + (0.5 * m_hetero[name_p1][i_qtl][0].toDouble());
		double gam_ind2_plus = m_homo_fav[name_p2][i_qtl][0].toDouble() + (0.5 * m_hetero[name_p2][i_qtl][0].toDouble()), gam_ind2_minus = m_homo_unfav[name_p2][i_qtl][0].toDouble() + (0.5 * m_hetero[name_p2][i_qtl][0].toDouble());

		QString score_homo_fav = "<li>Homo(+/+)=" + QString().setNum(gam_ind1_plus * gam_ind2_plus, 'f', 6) + "<font size='+2'>&nbsp;</font></li>";
		QString score_hetero = "<li>Hetero(+/-)=" + QString().setNum((gam_ind1_plus * gam_ind2_minus) + (gam_ind1_minus * gam_ind2_plus), 'f', 6) + "<font size='+2'>&nbsp;</font></li>";
		QString score_homo_unfav = "<li>Homo(-/-)=" + QString().setNum(gam_ind1_minus * gam_ind2_minus, 'f', 6) +"<font size='+2'>&nbsp;</font></li>";

		cellinfo +=	score_homo_fav + score_hetero + score_homo_unfav + html_line;
	}
	else if( column == (COL_MS - 2) ){

		// Computation of gametes favorable vs. unfavorable via probabilities to be [+/+] [+/-] [-/-]
		double gam_ind1_plus = m_homo_fav[name_p1][0][1].toDouble() + (0.5 * m_hetero[name_p1][0][1].toDouble()), gam_ind1_minus = m_homo_unfav[name_p1][0][1].toDouble() + (0.5 * m_hetero[name_p1][0][1].toDouble());
		double gam_ind2_plus = m_homo_fav[name_p2][0][1].toDouble() + (0.5 * m_hetero[name_p2][0][1].toDouble()), gam_ind2_minus = m_homo_unfav[name_p2][0][1].toDouble() + (0.5 * m_hetero[name_p2][0][1].toDouble());

		QString homof_MS = "<li>Homo(+/+)=" + QString().setNum( gam_ind1_plus * gam_ind2_plus, 'f', 6) + "<font size='+2'>&nbsp;</font></li>";
		QString hetero_MS = "<li>Hetero(+/-)=" + QString().setNum( (gam_ind1_plus * gam_ind2_minus) + (gam_ind1_minus * gam_ind2_plus), 'f', 6) + "</li>";
		QString homouf_MS = "<li>Homo(-/-)=" + QString().setNum( gam_ind1_minus * gam_ind2_minus, 'f', 6) + "</li>";
		cellinfo = QString("Id: %1<li>MS: %2</li>%3<li><U>Genotype</U></li>%4%5%6%7").arg(ind_name).arg(global_MS).arg(html_line).arg(homof_MS).arg(hetero_MS).arg(homouf_MS).arg(html_line);
	}
	else return; // neither MS nor QTLs => do nothing

	QWhatsThis::showText(QCursor::pos(), cellinfo, this);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: the index of the selected list (selection)
	## RETURN:
	## SPECIFICATION: (SLOT)
	The selected list of selection is displayed in the left table.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayLeftListSelection(int index_list_selected){
	if( ! m_vect_map_selection.empty() ){
		displayListSelection(ui.leftListTableWidget6, index_list_selected);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: the index of the selected list (selection)
	## RETURN:
	## SPECIFICATION: (SLOT)
	The selected list of selection is displayed in the right table.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayRightListSelection(int index_list_selected){
	if( ! m_vect_map_selection.empty() ){
		displayListSelection(ui.rightListTableWidget6, index_list_selected);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QTableWidget *tab: the selected table (selection).
	@ int index_list_selected: the index of the selected list.
	## RETURN:
	## SPECIFICATION:
	The selected list of selection is displayed in the selected table.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayListSelection(QTableWidget *tab, int index_list_selected){
	if( ! m_vect_map_selection.empty() ){

		tab->setSortingEnabled(false);

		int nb_columns = tab->columnCount(), cpt_ind = 0;
		double mean = 0.0, var = 0.0;
		QSet<QString> set_group; // list of groups.

		QMap<QString, int> & map_selected_list_i = *(m_vect_map_selection[index_list_selected]);

		tab->setRowCount(0);
		tab->setRowCount((int)map_selected_list_i.size());

		for( QMap<QString, int>::iterator iter_list = map_selected_list_i.begin(); iter_list != map_selected_list_i.end(); ++iter_list ){

			const QString & key = iter_list.key(); // individual of the list
			const QStringList & ind_data = m_map_scores[key]; // m_map_scores[key] returns a reference => OK.

			for(int j=0; j<nb_columns; j++){
				QTableWidgetItem *newItem = new QTableWidgetItem();
				// items are not editable and must allow Drag and drop.
				newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled);
	
				// Id
				if( j == 0 ){
					newItem->setText(key);
					tab->setItem(cpt_ind, j, newItem);
				}
				// Utility criterion
				else if( j == COL_UC ){
					// The data are filled to the table (values are double)
					newItem->setData(0, QString().setNum( ind_data[j-1].toDouble(), 'f', 4).toDouble() );
					tab->setItem(cpt_ind, j, newItem);
					((QTableWidgetItem *)tab->item(cpt_ind, j))->setTextColor(Qt::darkGreen);
				}
				// No (+/+, -/-, +/-, ?)
				else if( (j > COL_UC) && (j < COL_FIRST_QTL) ){
					// The data are filled to the table (values are integer)
					newItem->setData(0, ind_data[j-1].toInt());
					tab->setItem(cpt_ind, j, newItem);
				}
				// QTL
				else if( (j >= COL_FIRST_QTL) && (j < nb_columns) ){
					// The data are filled to the table
					newItem->setText( QString().setNum(ind_data[j-1].toDouble(), 'f', 4) );
					tab->setItem(cpt_ind, j, newItem);

					// Homozygous favorable
					if( m_homo_fav[key][j - COL_FIRST_QTL + 1][0].toDouble() >= m_vect_cut_off[0] ){
						if( m_vect_color[0].value() != ui.findPushButton2->palette().button().color().value() ){
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(QBrush(m_vect_color[0]));
						}
						else{
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(((QTableWidgetItem *)tab->item(cpt_ind, 1))->background());
						}
					}
					// Homozygous unfavorable
					else if( m_homo_unfav[key][j - COL_FIRST_QTL + 1][0].toDouble() >= m_vect_cut_off[1] ){
						if( m_vect_color[1].value() != ui.findPushButton2->palette().button().color().value() ){
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(QBrush(m_vect_color[1]));
						}
						else{
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(((QTableWidgetItem *)tab->item(cpt_ind, 1))->background());
						}
					}
					// Hetero
					else if( m_hetero[key][j - COL_FIRST_QTL + 1][0].toDouble() >= m_vect_cut_off[2] ){
						if( m_vect_color[2].value() != ui.findPushButton2->palette().button().color().value() ){
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(QBrush(m_vect_color[2]));
						}
						else{
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(((QTableWidgetItem *)tab->item(cpt_ind, 1))->background());
						}
					}
					// ? uncertain genotypes
					else{
						if( m_vect_color[3].value() != ui.findPushButton2->palette().button().color().value() ){
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(QBrush(m_vect_color[3]));
						}
						else{
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(((QTableWidgetItem *)tab->item(cpt_ind, 1))->background());
						}
					}
				}
				// p1, p2, cycle, group, MS, Weight
				else{
					// The data are filled to the table
					if (j != COL_MS && j != COL_WEIGHT)	newItem->setText(ind_data[j-1]);
					else newItem->setText(QString(" ").setNum(ind_data[j-1].toDouble(), 'f', 4)); // 4 decimal rounding after 0.
					tab->setItem(cpt_ind, j, newItem);

					// Molecular score
					if( j == COL_MS ){
						((QTableWidgetItem *)tab->item(cpt_ind, j))->setTextColor(Qt::red);
						mean += ind_data[j-1].toDouble();
					}
					// Weight
					else if( j == COL_WEIGHT ){
						((QTableWidgetItem *)tab->item(cpt_ind, j))->setTextColor(Qt::blue);
					}
				}
			}
			cpt_ind++;
			if( ind_data[COL_GROUP-1].compare("-") != 0 ) set_group << ind_data[COL_GROUP-1];
		}

		if( ((int)map_selected_list_i.size()) != 0 ){
			mean = mean / (double)map_selected_list_i.size();
		}
		else{
			mean = 0.0;
		}

		// Variance
		for( QMap<QString, int>::iterator iter_list = map_selected_list_i.begin(); iter_list != map_selected_list_i.end(); ++iter_list ){
			const QString & key = iter_list.key(); // individual of the list
			// All (MS)
			var += pow(m_map_scores[key][COL_MS - 1].toDouble() - mean, 2.0); //// Check COL_MS - 1
		}

		if( ((int)map_selected_list_i.size()) != 0 ){
			var = var / (double)map_selected_list_i.size();
		}
		else{
			var = 0.0;
		}

		// The mean, variance and number of individuals in the selected list is displayed in the interface (label)
		if( tab == ui.leftListTableWidget6 ){
			setLabelSelectionPage(ui.leftList1MeanLabel6, ui.leftList2MeanLabel6, mean, var, (int)map_selected_list_i.size(), set_group.size());
		}
		else if( tab == ui.rightListTableWidget6 ){
			setLabelSelectionPage(ui.rightList1MeanLabel6, ui.rightList2MeanLabel6, mean, var, (int)map_selected_list_i.size(), set_group.size());
		}

		tab->setSortingEnabled(true);
		tab->setAlternatingRowColors(true);
		tab->resizeColumnsToContents();
		tab->resizeRowsToContents();
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: the index of the selected list (selection).
	## RETURN:
	## SPECIFICATION: update of the labels and combobox (list of selection) in
	the crosses page.
	-----------------------------------------------------------------------------
*/

void MainWindow::maxCrossContribChanged(int index_list_selection){

	if( (! m_vect_map_selection.empty()) && (! m_vect_map_selection[index_list_selection]->empty()) ){
		int nb_indiv_selected = (int)m_vect_map_selection[index_list_selection]->size();

		// Number of maximum crosses to be made with the selected list of selection
		m_vect_cross_option[7] = (nb_indiv_selected * (nb_indiv_selected - 1)) / 2;

		// Maximum contribution for each individual regarding the list of selection
		m_vect_cross_option[8] = nb_indiv_selected - 1;
	}
	else{
		m_vect_cross_option[7] = 0;
		m_vect_cross_option[8] = 0;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list2_selected: the index of the selected list (2nd list of selection)
	## RETURN:
	## SPECIFICATION: update of the parameters in the cross option.
	-----------------------------------------------------------------------------
*/

void MainWindow::maxCrossContrib2ListsChanged(int index_list2_selection){

	if( (! m_vect_map_selection.empty()) && (! m_vect_map_selection[index_list2_selection]->empty()) && (! m_vect_map_selection[ui.selectionListComboBox4->currentIndex()]->empty()) ){
		int nb_indiv_list1_selected = (int)m_vect_map_selection[ui.selectionListComboBox4->currentIndex()]->size();
		int nb_indiv_list2_selected = (int)m_vect_map_selection[index_list2_selection]->size();

		// Number of maximum crosses to be made with the selected list of selection
		m_vect_cross_option[7] = nb_indiv_list1_selected * nb_indiv_list2_selected;

		// Maximum contribution for each individual regarding the list of selection
		m_vect_cross_option[8] = max(nb_indiv_list1_selected, nb_indiv_list2_selected);
	}
	else{
		m_vect_cross_option[7] = 0;
		m_vect_cross_option[8] = 0;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int state: the current state of the checkbox.
	## RETURN:
	## SPECIFICATION: The second list of selection is enabled or not depending
	of the checkbox state.
	-----------------------------------------------------------------------------
*/
void MainWindow::checkBoxListsSelectionChanged(int state){
	if( state == Qt::Unchecked ){
		ui.selectionList2ComboBox4->setEnabled(false);
		m_vect_cross_option[10] = 0;

		if( ! m_vect_map_selection.empty() ){
			maxCrossContribChanged(ui.selectionListComboBox4->currentIndex());
		}
	}
	else if( state == Qt::Checked ){
		ui.selectionList2ComboBox4->setEnabled(true);
		m_vect_cross_option[10] = 1;

		if( ! m_vect_map_selection.empty() ){
			maxCrossContrib2ListsChanged(ui.selectionList2ComboBox4->currentIndex());
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: (SLOT)
	Selected individuals are removed to the left list of selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::deleteIndToLeftListSelection(){
	deleteIndToListSelection(ui.leftListTableWidget6, ui.leftListComboBox6->currentIndex());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: (SLOT)
	Selected individuals are removed to the right list of selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::deleteIndToRightListSelection(){
	deleteIndToListSelection(ui.rightListTableWidget6, ui.rightListComboBox6->currentIndex());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QTableWidget *tab: table (list of selection)
	@ int index_list_selected: index of the selected list
	## RETURN:
	## SPECIFICATION:
	Selected individuals are removed to the selected list of selection and table.
	-----------------------------------------------------------------------------
*/

void MainWindow::deleteIndToListSelection(QTableWidget *tab, int index_list_selected){

	int topRow, bottomRow;

	// Each individuals selected are added to the list of selection previously chosen
	QList<QTableWidgetSelectionRange> listQTableWidgetSelectionRange = tab->selectedRanges();

	for(int i=0; i<(int)listQTableWidgetSelectionRange.size(); i++){
		topRow = listQTableWidgetSelectionRange[i].topRow();
		bottomRow = listQTableWidgetSelectionRange[i].bottomRow();

		for(int j=topRow; j<=bottomRow; j++){
			// The individual is deleted from the selected list
			m_vect_map_selection[index_list_selected]->remove((tab->item(j, 0))->text());
		}
	}

	// Update the list of selection if it's the current one shown
	if( ui.leftListComboBox6->currentIndex() == index_list_selected){
		displayListSelection(ui.leftListTableWidget6, index_list_selected);
	}
	if( ui.rightListComboBox6->currentIndex() == index_list_selected){
		displayListSelection(ui.rightListTableWidget6, index_list_selected);
	}

	// Update of the combobox (list of selection) in the crosses page
	if( ui.selectionListComboBox4->currentIndex() == index_list_selected){
		maxCrossContribChanged(index_list_selected);
	}

	// Update of graphs if the current list was selected
	if( ui.graphLeftListComboBox6->currentIndex() == index_list_selected ){
		plotGraphSelectionPage(ui.graphLeftQwtPlot6, ui.graphLeftListComboBox6->currentIndex(), ui.graphLeftQtlComboBox6->currentIndex());
	}
	if( ui.graphRightListComboBox6->currentIndex() == index_list_selected ){
		plotGraphSelectionPage(ui.graphRightQwtPlot6, ui.graphRightListComboBox6->currentIndex(), ui.graphRightQtlComboBox6->currentIndex());
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QListWidgetItem *item: current double clicked item in the list
	## RETURN:
	## SPECIFICATION: the list of selected individuals become editable.
	-----------------------------------------------------------------------------
*/

void MainWindow::editSelectionItemList(QListWidgetItem *item){
	item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable );
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QListWidgetItem *item: current double clicked item in the list
	## RETURN:
	## SPECIFICATION: the list (of selection) name of the current item is updated
	when the user renames the list.
	-----------------------------------------------------------------------------
*/

void MainWindow::itemListSelectionChanged(QListWidgetItem * item){
	if( ! m_list_selection.empty() ){
		if( item->text().compare(m_list_selection[ui.selectionListWidget6->currentRow()]) != 0 ){
			m_list_selection[ui.selectionListWidget6->currentRow()] = item->text();

			// Update of the combobox list
			ui.leftListComboBox6->setItemText(ui.selectionListWidget6->currentRow(), item->text());
			ui.rightListComboBox6->setItemText(ui.selectionListWidget6->currentRow(), item->text());
			ui.truncListComboBox6->setItemText(ui.selectionListWidget6->currentRow(), item->text());
			ui.compListComboBox6->setItemText(ui.selectionListWidget6->currentRow(), item->text());
			ui.selectionListComboBox4->setItemText(ui.selectionListWidget6->currentRow(), item->text());
			ui.selectionList2ComboBox4->setItemText(ui.selectionListWidget6->currentRow(), item->text());
			ui.graphLeftListComboBox6->setItemText(ui.selectionListWidget6->currentRow(), item->text());
			ui.graphRightListComboBox6->setItemText(ui.selectionListWidget6->currentRow(), item->text());
			ui.pedigreeListComboBox6->setItemText(ui.selectionListWidget6->currentRow(), item->text());
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QListWidgetItem *item: current clicked item in the list
	## RETURN:
	## SPECIFICATION: The current selected list (clicked on the QListWidgetItem)
	is updated on the left table.
	-----------------------------------------------------------------------------
*/

void MainWindow::leftListSelectionChanged(QListWidgetItem *item){

	if( ! m_list_selection.empty() ){
		if( item->text().compare(ui.leftListComboBox6->currentText()) != 0 ){
			ui.leftListComboBox6->setCurrentIndex(ui.selectionListWidget6->currentRow());
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: take the n best individuals (based on a selected criterion)
	of the panel and put it in a list of selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::runTruncationSelection(){
	// If a list of selection is present
	if( (int)m_vect_map_selection.size() > 0 ){
		int nsel = 0, index_list_selected = ui.truncListComboBox6->currentIndex(), index_criterion_selected = ui.truncParamsComboBox6->currentIndex();

		QMap<QString, int> & map_selected_list_i = *(m_vect_map_selection[index_list_selected]);
		//QVector<QString> vect_indiv_sorted; // Vector of future sorted individuals
		QList<map_iterator> vect_it_sorted; // List of future sorted individuals

		// Copy of the current list of individuals (not ordered)
		for( QMap<QString, QStringList>::iterator iter_list = m_map_scores.begin(); iter_list != m_map_scores.end(); ++iter_list ){
			const QStringList & ind_data = iter_list.value();

			// The list is filtered regarding the individual's cycle
			if( (m_vect_truncation_option[1] != 0) && (m_vect_truncation_option[0] == 0) ){
				//if( m_map_scores[ind_name][COL_GENE - 1].compare( (m_map_gene_scores.begin() + m_vect_truncation_option[1]).key() ) == 0 ){
				if( ind_data[COL_GENE - 1].compare( (m_map_gene_scores.begin() + m_vect_truncation_option[1]).key() ) == 0 ){
					//vect_indiv_sorted.push_back(ind_name); // individual of the list
					vect_it_sorted.push_back(iter_list);
				}
			}
			// The list is filtered regarding the individual's group
			else if( (m_vect_truncation_option[1] != 0) && (m_vect_truncation_option[0] == 1) ){
				//if( m_map_scores[ind_name][COL_GROUP - 1].compare( (m_map_group.begin() + (m_vect_truncation_option[1] - 1)).key() ) == 0 ){
				if( ind_data[COL_GROUP - 1].compare( (m_map_group.begin() + (m_vect_truncation_option[1] - 1)).key() ) == 0 ){
					//vect_indiv_sorted.push_back(ind_name); // individual of the list
					vect_it_sorted.push_back(iter_list);
				}
			}
			// No filter
			else{
				//vect_indiv_sorted.push_back(ind_name); // individual of the list
				vect_it_sorted.push_back(iter_list);
			}
		}

		// Individuals are sorted regarding their criterion (molecular score...) for all QTL
		//triFusion(vect_indiv_sorted, (int)vect_indiv_sorted.size(), m_map_scores, index_criterion_selected + COL_MS - 1);
		crit_index = index_criterion_selected + COL_MS - 1;
		qSort(vect_it_sorted.begin(), vect_it_sorted.end(), criterionMoreThan);

		// Swap if not enough individuals in the selected list compared to the number chosen by the user
		if( (int)vect_it_sorted.size() < ui.truncNSelSpinBox6->value() ){
			//nsel = (int)vect_indiv_sorted.size();
			nsel = (int)vect_it_sorted.size();
		}
		else{
			nsel = ui.truncNSelSpinBox6->value();
		}

		// The chosen selection list is updated
		for(int i=0; i<nsel; i++){
			// The individual is added to the selected list if it is not already present on it
			//if( map_selected_list_i.find(vect_indiv_sorted[i]) == map_selected_list_i.end() ){
			if( map_selected_list_i.find(vect_it_sorted[i].key()) == map_selected_list_i.end() ){
				////map_selected_list_i[vect_indiv_sorted[i]] = index_list_selected;
				//m_vect_map_selection[index_list_selected]->insert(vect_indiv_sorted[i], index_list_selected);
				map_selected_list_i[vect_it_sorted[i].key()] = index_list_selected;
			}
		}

		// Update the list of selection if it's the current one shown
		ui.leftListComboBox6->setCurrentIndex(index_list_selected);
		displayListSelection(ui.leftListTableWidget6, index_list_selected);

		if( ui.rightListComboBox6->currentIndex() == index_list_selected){
			displayListSelection(ui.rightListTableWidget6, index_list_selected);
		}

		// Update of the combobox (list of selection) in the crosses page
		if( ui.selectionListComboBox4->currentIndex() == index_list_selected){
			maxCrossContribChanged(index_list_selected);
		}

		// Update of graphs if the current list was selected
		if( ui.graphLeftListComboBox6->currentIndex() == index_list_selected ){
			plotGraphSelectionPage(ui.graphLeftQwtPlot6, ui.graphLeftListComboBox6->currentIndex(), ui.graphLeftQtlComboBox6->currentIndex());
		}
		if( ui.graphRightListComboBox6->currentIndex() == index_list_selected ){
			plotGraphSelectionPage(ui.graphRightQwtPlot6, ui.graphRightListComboBox6->currentIndex(), ui.graphRightQtlComboBox6->currentIndex());
		}
	}
	else{
		showCustomErrorMessage("A list of selection need to be created before the run !");
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Individuals are selected such that their QTL composition
	complements those individuals already selected.
	-----------------------------------------------------------------------------
*/

void MainWindow::runComplementationSelection(){
	// If a list of selection is present
	if( (int)m_vect_map_selection.size() > 0 ){
		int index_list_selected = ui.compListComboBox6->currentIndex(), nb_qtl = ui.scoreTableWidget->columnCount() - COL_FIRST_QTL;

		vector<int> list_qtl_to_complement;
		QStringList vect_ind_added;
		bool nt_all = false, nmax = false, new_ind = true;

		QMap<QString, int> & map_selected_list_i = *(m_vect_map_selection[index_list_selected]);

		QString res = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
				"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }"
				"</style></head><body style=\" font-family:'Sans Serif'; font-size:"TEXT_SIZE"; font-weight:400; font-style:normal;\">";
	
		// Individual in the selected subset reaches a given max value (Nmax)
		if( (int)map_selected_list_i.size() >= m_vect_complementation_params[3] ){
			nmax = true;
			res += "<p style=\" margin-top:6px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">The given Nmax value has already been reached!</p>";
		}

		// Until: fav all at all QTL are present in at least nt ind (nt)
		// Individual in the selected subset reaches a given max value (Nmax)
		// Not possible to find a new individual to add (new_ind)
		while( (nt_all == false) && (nmax == false) && (new_ind == true) ){
			vector<int> vect_nb_ind_per_qtl(nb_qtl, 0);
			list_qtl_to_complement.clear();

			// Identification of the number of individuals having favorable allele present for each QTL
			for(int i=0; i<nb_qtl; i++){
				// Loop on each individual
				for( QMap<QString, int>::iterator iter_list = map_selected_list_i.begin(); iter_list != map_selected_list_i.end(); ++iter_list ){
					const QString & ind_name = iter_list.key(); // individual of the list
					// The threshold for Molecular Score (MS), above which a favorable QTL allele is declared 'present'.
					if( m_map_scores[ind_name][COL_FIRST_QTL - 1 + i].toDouble() >= m_vect_complementation_params[0] ){
						vect_nb_ind_per_qtl[i]++; // List of number of individuals having MS >= Cut-off (teta) for each QTL
					}
				}
			}

			// Beginning of the table
			res += "<p style=\" margin-top:6px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
					"Number of individuals carrying the favorable QTL alleles (nt=" + QString("%1").arg((int)m_vect_complementation_params[1], 0, 10) + ", N=" + QString("%1").arg((int)m_vect_map_selection[index_list_selected]->size(), 0, 10) + "):"MAJ_LINEHEIGHT"</p>"
					"<table border=\"1\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"0\" cellpadding=\"2\" bgcolor=\"#ffffff\"><tr>";

			// Identification of QTL with favorable allele present in fewer than nt selected individuals
			for(int i=0; i<nb_qtl; i++){
				// At least nt individuals
				if( vect_nb_ind_per_qtl[i] < (int)m_vect_complementation_params[1] ){
					list_qtl_to_complement.push_back(i);
				}
				res += "<td><p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-weight:600;\"> "
						"QTL" + QString("%1").arg(i+1, 0, 10) + " <span style=\" font-weight:400;\"> </span></p></td>";
			}
			res += "</tr><tr>";

			for(int i=0; i<nb_qtl; i++){
				// QTL to complement
				if( vect_nb_ind_per_qtl[i] < (int)m_vect_complementation_params[1] ){
					res += "<td><p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
							"<font color=\"red\"> " + QString("%1/%2").arg(vect_nb_ind_per_qtl[i], 0, 10).arg((int)m_vect_map_selection[index_list_selected]->size(), 0, 10) + " </font></p></td>";
				}
				else{
					res += "<td><p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
							+ QString("%1/%2").arg(vect_nb_ind_per_qtl[i], 0, 10).arg((int)m_vect_map_selection[index_list_selected]->size(), 0, 10) + " </p></td>";
				}
			}
			// End of the table
			res += "</tr></table>";

			if( list_qtl_to_complement.size() == 0 ){
				res += "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">QTLs to complement: No"MIN_LINEHEIGHT"</p>";
			}
			else{
				res += "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">QTLs to complement: ";
				for(int i=0; i<(int)list_qtl_to_complement.size(); i++){
					res += QString("%1 ").arg(list_qtl_to_complement[i] + 1, 0, 10);
				}
				res += ""MIN_LINEHEIGHT"</p>";
			}

			// Initialization
			QString name_ind1("");
			double ms_ind1 = -1.0, ms_ind2 = 0.0;
			int nb_qtl_fav_ind1 = -1, nb_qtl_fav_ind2 = 0;

			// Favorable allele at all QTL are present in at least nt individuals (nt)
			if( list_qtl_to_complement.empty() ){
				nt_all = true;
				res += "<p style=\" margin-top:6px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">All the favorable QTLs alleles are present in at least nt individuals.</p>";
			}
			else{
				// Take the best individual with favorable alleles present in previous list of QTL to complement
				for( QMap<QString, QStringList>::iterator iter_list = m_map_scores.begin(); iter_list != m_map_scores.end(); ++iter_list ){
					bool keep_ind = false; // Possible selected individual (Not filtered by cycle or group)
					const QString & name_ind2 = iter_list.key(); // individual of the total list
					ms_ind2 = m_map_scores[name_ind2][COL_MS - 1].toDouble();

					// The list is filtered regarding the individual's cycle
					if( (((int)m_vect_complementation_params[5]) != 0) && (((int)m_vect_complementation_params[4]) == 0) ){
						if( m_map_scores[name_ind2][COL_GENE - 1].compare( (m_map_gene_scores.begin() + ((int)m_vect_complementation_params[5])).key() ) == 0 ){
							keep_ind = true;
						}
					}
					// The list is filtered regarding the individual's group
					else if( (((int)m_vect_complementation_params[5]) != 0) && ( ((int)m_vect_complementation_params[4]) == 1) ){
						if( m_map_scores[name_ind2][COL_GROUP - 1].compare( (m_map_group.begin() + ( ((int)m_vect_complementation_params[5]) - 1)).key() ) == 0 ){
							keep_ind = true;
						}
					}
					// No filter
					else{
						keep_ind = true;
					}

					// If the current individual is not found in the list of selection and if he belong to the cycle/group
					if( (map_selected_list_i.find(name_ind2) == map_selected_list_i.end()) && keep_ind ){
						// If the molecular score of the current individual is higher than a cut-off
						if( ms_ind2 >= m_vect_complementation_params[2] ){
							nb_qtl_fav_ind2 = 0;
							// Find the number of favorable QTL which complement the subset of already selected individuals
							for(int i=0; i<(int)list_qtl_to_complement.size(); i++){
								if( m_map_scores[name_ind2][COL_FIRST_QTL - 1 + list_qtl_to_complement[i]].toDouble() >= m_vect_complementation_params[0] ){
									nb_qtl_fav_ind2++;
								}
							}
							if( nb_qtl_fav_ind2 > 0 ){
								// Only the best individual remains
								if( nb_qtl_fav_ind2 > nb_qtl_fav_ind1 ){
									nb_qtl_fav_ind1 = nb_qtl_fav_ind2;
									ms_ind1 = ms_ind2;
									name_ind1 = name_ind2;
								}
								// Same number of favorable QTL
								else if( nb_qtl_fav_ind2 == nb_qtl_fav_ind1 ){
									// Selection of the individual with the best MS
									if( ms_ind2 > ms_ind1 ){
										nb_qtl_fav_ind1 = nb_qtl_fav_ind2;
										ms_ind1 = ms_ind2;
										name_ind1 = name_ind2;
									}
								}
							}
						} // Cut-off
					}
				} // End loop on each ind of the panel
			}

			// Check if an individual had been selected
			if( (ms_ind1 < 0.0) && (nb_qtl_fav_ind1 == -1) ){
				new_ind = false;
				if( nt_all == false ){
					res += "<p style=\" margin-top:6px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">No (more) individual(s) has/have been selected to complement the current list of selection.</p>";
				}
			}
			// The selected individual is added to the selection list
			else{
				vect_ind_added.push_back(name_ind1);
				map_selected_list_i[name_ind1] = index_list_selected;
				res += "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
						"Individual <b>" + name_ind1 + "</b> has been added to the list of selection.</p>";
			}

			// Individual in the selected subset reaches a given max value (Nmax)
			if( (int)map_selected_list_i.size() >= m_vect_complementation_params[3] ){
				nmax = true;
				res += "<p style=\" margin-top:6px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">The given Nmax value has been reached.</p>";
			}
			vect_nb_ind_per_qtl.clear();
		} // End While

		if( !vect_ind_added.empty() ){
			res += "<p style=\" margin-top:6px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Individual(s)";
			for(int i=0; i< (int)vect_ind_added.size(); i++){
				res += " <b>" + vect_ind_added[i] + "</b>";
			}
			res += " has/have been added to " + ui.compListComboBox6->currentText() + ".</p>";
		}

		// Last table displayed
		vector<int> vect_nb_ind_per_qtl_last(nb_qtl, 0);

		// Identification of the number of individuals having favorable allele present for each QTL
		for(int i=0; i<nb_qtl; i++){
			// Loop on each individual
			for( QMap<QString, int>::iterator iter_list = map_selected_list_i.begin(); iter_list != map_selected_list_i.end(); ++iter_list ){
				const QString & ind_name = iter_list.key(); // individual of the list
				// The threshold for Molecular Score (MS), above which a favorable QTL allele is declared 'present'.
				if( m_map_scores[ind_name][COL_FIRST_QTL - 1 + i].toDouble() >= m_vect_complementation_params[0] ){
					vect_nb_ind_per_qtl_last[i]++; // List of number of individuals having MS >= Cut-off (teta) for each QTL
				}
			}
		}

		res += "<p style=\" margin-top:6px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
				"Table at the end of the QCS process:"MAJ_LINEHEIGHT"</p>"
				"<table border=\"1\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"0\" cellpadding=\"2\" bgcolor=\"#ffffff\"><tr>";

		// Identification of QTL with favorable allele present in fewer than nt selected individuals
		for(int i=0; i<nb_qtl; i++){
			// At least nt individuals
			res += "<td><p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-weight:600;\"> "
					"QTL" + QString("%1").arg(i+1, 0, 10) + " <span style=\" font-weight:400;\"> </span></p></td>";
		}
		res += "</tr><tr>";

		for(int i=0; i<nb_qtl; i++){
			// QTL to complement
			if( vect_nb_ind_per_qtl_last[i] < (int)m_vect_complementation_params[1] ){
				res += "<td><p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
						"<font color=\"red\"> " + QString("%1/%2").arg(vect_nb_ind_per_qtl_last[i], 0, 10).arg((int)m_vect_map_selection[index_list_selected]->size(), 0, 10) + " </font></p></td>";
			}
			else{
				res += "<td><p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
						+ QString("%1/%2").arg(vect_nb_ind_per_qtl_last[i], 0, 10).arg((int)m_vect_map_selection[index_list_selected]->size(), 0, 10) + " </p></td>";
			}
		}
		// End of the table
		res += "</tr></table>";
		vect_nb_ind_per_qtl_last.clear();

		// The Tables and graphs in the selection page are updated
		if( ui.leftListComboBox6->currentIndex() == index_list_selected){
			displayListSelection(ui.leftListTableWidget6, index_list_selected);
		}
		if( ui.rightListComboBox6->currentIndex() == index_list_selected){
			displayListSelection(ui.rightListTableWidget6, index_list_selected);
		}

		// Update of the combobox (list of selection) in the crosses page
		if( ui.selectionListComboBox4->currentIndex() == index_list_selected ){
			maxCrossContribChanged(index_list_selected);
		}

		// Update of graphs if the current list was selected
		if( ui.graphLeftListComboBox6->currentIndex() == index_list_selected ){
			plotGraphSelectionPage(ui.graphLeftQwtPlot6, ui.graphLeftListComboBox6->currentIndex(), ui.graphLeftQtlComboBox6->currentIndex());
		}
		if( ui.graphRightListComboBox6->currentIndex() == index_list_selected ){
			plotGraphSelectionPage(ui.graphRightQwtPlot6, ui.graphRightListComboBox6->currentIndex(), ui.graphRightQtlComboBox6->currentIndex());
		}

		// The results are displayed via a QDialog
		if (m_complementation_results != NULL) { delete m_complementation_results; m_complementation_results = NULL; }
		m_complementation_results = new CompResultsWindow(this);
		res += "</body></html>";
		m_complementation_results->setTextResultsLabel(res);

		m_complementation_results->show();
		m_complementation_results->activateWindow();
		vect_ind_added.clear();
	}
	else{
		showCustomErrorMessage("A list of selection need to be created before the run !");
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: save the selected list of selected individuals.
	-----------------------------------------------------------------------------
*/

void MainWindow::saveListSelection(){

	if( (ui.selectionListWidget6->count() == 0) || ((int)m_vect_map_selection[ui.selectionListWidget6->currentRow()]->size() == 0) ){
		QMessageBox::information(this, tr("Save list selection"), tr("The list of selection is empty !!"));
		return;
	}

	// The name of the current list is proposed
	QString dir_path = m_file_res + "/list_selection/" + ui.selectionListWidget6->currentItem()->text() + ".txt";
	QString filter = tr("text files (*.txt)");

	QFileDialog fileDialog(this, tr("Export list of selection: %1").arg(ui.selectionListWidget6->currentItem()->text()), dir_path, filter);
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);

	if( fileDialog.exec() == QDialog::Accepted ){
		QString fileName = fileDialog.selectedFiles().value(0), selected_filter = fileDialog.selectedNameFilter(), ext = selected_filter.section('.', -1);

		// The name of the file is empty
		if( fileName.isEmpty() ){
			QMessageBox::information(this, tr("Save list selection"), tr("Cannot load %1.").arg(fileName));
			return;
		}
		else{
			ext.chop(1);	// skip parenthesis

			if( fileName.section('.', -1) != ext ){
				fileName+= "." + ext;	// add the extention to filname if needed.
			}
			saveTableSelection(ui.selectionListWidget6->currentRow(), fileName);
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: index of the selected list (selection)
	@ string fileName: file path where will be written the selected individuals.
	## RETURN:
	## SPECIFICATION: save the selected list of selected individuals into the
	specified file.
	-----------------------------------------------------------------------------
*/

void MainWindow::saveTableSelection(int index_list_selected, const QString & fileName){
	
	QFile file(fileName);
	int nb_columns = ui.leftListTableWidget6->columnCount();

	if ( file.open(QIODevice::WriteOnly | QIODevice::Text) ){
		
		QTextStream list_selection_file(&file); // write only text stream on file
		// Headers
		for(int i=0; i<ui.leftListTableWidget6->columnCount(); i++){
			if( i == 0){
				list_selection_file << ui.leftListTableWidget6->horizontalHeaderItem(i)->text();
			}
			else{
				list_selection_file << "\t" << ui.leftListTableWidget6->horizontalHeaderItem(i)->text();
			}
		}
		list_selection_file << "\n";

		// Table
		for( QMap<QString, int>::iterator iter_list = m_vect_map_selection[index_list_selected]->begin(); iter_list != m_vect_map_selection[index_list_selected]->end(); ++iter_list ){
			const QString & ind = iter_list.key(); // individual of the list
			const QStringList & ind_data = m_map_scores.find(ind).value();
			//// A tester : list_selection_file << ind << "\t" << ind_data.join('\t'); au lieu de la boucle for.
			for(int j=0; j<nb_columns; j++){
				if( j == 0 ){
					list_selection_file << ind;
				}
				else{
					list_selection_file << "\t" << ind_data[j-1];
				}
			}
			list_selection_file << "\n";
		}
		file.close();
	}
	else {
		QMessageBox::information(this, tr("Save list selection"), tr("Error: opening the file [%1] in write mode failed.").arg(fileName));
		return;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: a new empty list of selection is added to the list.
	-----------------------------------------------------------------------------
*/

void MainWindow::addListSelection(){

	QString listName = "List Selection " + QString(" ").setNum(ui.selectionListWidget6->count() + 1);

	// Update of the containers for the new list added
	m_list_selection.push_back(listName);
	ui.selectionListWidget6->addItem(listName);
	m_vect_map_selection.push_back(new QMap<QString, int>());

	// Update of the combobox lists
	ui.leftListComboBox6->addItem(listName);
	ui.rightListComboBox6->addItem(listName);
	ui.truncListComboBox6->addItem(listName);
	ui.compListComboBox6->addItem(listName);
	ui.selectionListComboBox4->addItem(listName);
	ui.selectionList2ComboBox4->addItem(listName);
	ui.graphLeftListComboBox6->addItem(listName);
	ui.graphRightListComboBox6->addItem(listName);
	ui.pedigreeListComboBox6->addItem(listName);
	ui.selectionListWidget6->setCurrentRow(ui.selectionListWidget6->count() - 1);
	ui.leftListComboBox6->setCurrentIndex(ui.selectionListWidget6->currentRow());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the selected list of selection is removed from the list.
	-----------------------------------------------------------------------------
*/

void MainWindow::removeListSelection(){

	if( ! m_list_selection.empty() ){

		int answer = QMessageBox::question(this, tr("Deletion of the list of selection"), tr("Do you really want to delete the list [%1] ?").arg(ui.selectionListWidget6->currentItem()->text()), QMessageBox::Yes | QMessageBox::No);

		// Yes : the current list is removed
		if( answer == QMessageBox::Yes ) {

			int currentRow = ui.selectionListWidget6->currentRow();

			// If yes the current list is removed
			m_list_selection.erase(m_list_selection.begin() + currentRow);
			ui.selectionListWidget6->takeItem(ui.selectionListWidget6->row(ui.selectionListWidget6->currentItem()));
			delete m_vect_map_selection[currentRow]; m_vect_map_selection[currentRow] = NULL;
			m_vect_map_selection.erase(m_vect_map_selection.begin() + currentRow);

			//cout << "currentRow=" << currentRow << " m_list_selection=" << (int)m_list_selection.size() << " ui.selectionListWidget6="<< ui.selectionListWidget6->count() << " m_vect_map_selection=" << (int)m_vect_map_selection.size() << endl;

			// Last list to remove
			if( ui.leftListComboBox6->count() == 1){
				ui.leftListTableWidget6->clearContents();
				ui.leftListTableWidget6->setRowCount(0);
				ui.rightListTableWidget6->clearContents();
				ui.rightListTableWidget6->setRowCount(0);
				ui.graphLeftQwtPlot6->detachItems();
				ui.graphLeftQwtPlot6->replot();
				ui.graphRightQwtPlot6->detachItems();
				ui.graphRightQwtPlot6->replot();
			}

			// Update of the combobox lists
			ui.leftListComboBox6->removeItem(currentRow);
			ui.rightListComboBox6->removeItem(currentRow);
			ui.truncListComboBox6->removeItem(currentRow);
			ui.compListComboBox6->removeItem(currentRow);
			ui.selectionListComboBox4->removeItem(currentRow);
			ui.selectionList2ComboBox4->removeItem(currentRow);
			ui.graphLeftListComboBox6->removeItem(currentRow);
			ui.graphRightListComboBox6->removeItem(currentRow);
			ui.pedigreeListComboBox6->removeItem(currentRow);

			if( ! m_list_selection.empty() ){
				ui.selectionListWidget6->setCurrentRow(ui.selectionListWidget6->count() - 1);
				ui.leftListComboBox6->setCurrentIndex(ui.selectionListWidget6->currentRow());
			}
		}
		else if( answer == QMessageBox::No ){
			return;
		}
	}
	else{
		QMessageBox::information(this, tr("Deletion of the list of selection"), tr("All the lists have already been removed !"));
		return;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the lists of selection are removed and reset to the default
	(2 empty lists).
	-----------------------------------------------------------------------------
*/

void MainWindow::resetListSelection(){
	int answer = QMessageBox::question(this, tr("Reset of the lists of selection"), tr("Do you really want to delete all the lists ?"), QMessageBox::Yes | QMessageBox::No);

	resetListSelection(answer);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int answer: 0 or 1 depending on if you want to reset the lists.
	## RETURN:
	## SPECIFICATION: the lists of selection are removed and reset to the default
	(2 empty lists) if the button Yes is clicked.
	-----------------------------------------------------------------------------
*/

void MainWindow::resetListSelection(int answer){

	// Yes : the current list is removed
	if( answer == QMessageBox::Yes ) {

		int nbLists = (int)m_list_selection.size();

		// If yes all the lists are removed
		for(int i=0; i<nbLists; i++){
			m_list_selection.erase(m_list_selection.begin());
			delete m_vect_map_selection.front(); m_vect_map_selection.front() = NULL;
			m_vect_map_selection.erase(m_vect_map_selection.begin());

			// Initialization case
			if( ui.selectionListWidget6->count() != 0 ){

				ui.selectionListWidget6->takeItem(0);

				// Update of the combobox lists
				ui.leftListComboBox6->removeItem(0);
				ui.rightListComboBox6->removeItem(0);
				ui.truncListComboBox6->removeItem(0);
				ui.compListComboBox6->removeItem(0);
				ui.selectionListComboBox4->removeItem(0);
				ui.selectionList2ComboBox4->removeItem(0);
				ui.graphLeftListComboBox6->removeItem(0);
				ui.graphRightListComboBox6->removeItem(0);
				ui.pedigreeListComboBox6->removeItem(0);
			}
		}

		// Reset all the tables and graphs
		ui.leftListTableWidget6->clearContents();
		ui.leftListTableWidget6->setRowCount(0);
		ui.rightListTableWidget6->clearContents();
		ui.rightListTableWidget6->setRowCount(0);
		ui.graphLeftQwtPlot6->detachItems();
		ui.graphLeftQwtPlot6->replot();
		ui.graphRightQwtPlot6->detachItems();
		ui.graphRightQwtPlot6->replot();

		// Creation of the 2 empty list by default
		for(int i=0; i<2; i++){
			QString listName = "List Selection " + QString(" ").setNum(i+1);

			// Update of the containers for the new list added
			m_list_selection.push_back(listName);
			ui.selectionListWidget6->addItem(listName);
			m_vect_map_selection.push_back(new QMap<QString, int>());

			// Update of the combobox lists
			ui.leftListComboBox6->addItem(listName);
			ui.rightListComboBox6->addItem(listName);
			ui.truncListComboBox6->addItem(listName);
			ui.compListComboBox6->addItem(listName);
			ui.selectionListComboBox4->addItem(listName);
			ui.selectionList2ComboBox4->addItem(listName);
			ui.graphLeftListComboBox6->addItem(listName);
			ui.graphRightListComboBox6->addItem(listName);
			ui.pedigreeListComboBox6->addItem(listName);
		}
		ui.selectionListWidget6->setCurrentRow(0);
		ui.leftListComboBox6->setCurrentIndex(0);
		ui.rightListComboBox6->setCurrentIndex(1);
		ui.graphLeftListComboBox6->setCurrentIndex(0);
		ui.graphRightListComboBox6->setCurrentIndex(1);
	}
	else if( answer == QMessageBox::No ){
		return;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: A set of slots that allow zooming the pedigree image.
	-----------------------------------------------------------------------------
*/

void MainWindow::zoomInPedigree(){
	;
}
void MainWindow::zoomOutPedigree(){
	;
}
void MainWindow::zoomOriginalPedigree(){
	;
}
void MainWindow::zoomFitPedigree(){
	;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the pedigree information is shown (drawn) after the user click.
	-----------------------------------------------------------------------------
*/

void MainWindow::runPedigree(){
	//ui.pedigreeZoomWidget->setEnabled(false);
	// All individuals pedigree together
	if( ui.pedigreeAllRadioButton6->isChecked() ){
		runPedigreeAll();
	}
	// Only the list of selected individuals is drawn
	else if( ui.pedigreeAloneRadioButton6->isChecked() ){
		runPedigreeAlone();
	}
	//ui.pedigreeZoomWidget->setEnabled(true);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the pedigree of all individuals in the panel is drawn.
	-----------------------------------------------------------------------------
*/

void MainWindow::runPedigreeAll(){

	int index_list_selected = ui.pedigreeListComboBox6->currentIndex();
	string ind = "", pedigree_image_file = m_file_res.toStdString() + "/graph_pedigree.png";

	GVC_t *gvc = gvContext();
	Agraph_t *g = agopen((char *)"g", AGDIGRAPHSTRICT);

	// Pedigree graph for all the individuals
	for( QMap<QString, QStringList>::iterator iter_list = m_map_scores.begin(); iter_list != m_map_scores.end(); ++iter_list ){
		ind = iter_list.key().toStdString(); // individual of the list
		createAllPedigree(g, ind);
	}

	if( ! m_vect_map_selection.empty() ){
		// Display the pedigree graph only for the selected individuals (with color)
		for( QMap<QString, int>::iterator iter_list = m_vect_map_selection[index_list_selected]->begin(); iter_list != m_vect_map_selection[index_list_selected]->end(); ++iter_list ){
			ind = iter_list.key().toStdString(); // individual of the list
			Agnode_t *ind_node = agnode(g, (char *)ind.c_str());
			agsafeset(ind_node, (char*)"fontcolor", (char*)"blue", (char*)"");
			createSelectedPedigree(g, ind, 1);
		}
	}

	gvLayout(gvc, g, (char *)"dot"); //usershape_t
	//qDebug()<<gvc;
	//GVC_s* toto = (GVC_s*)gvc;
	//qDebug()<<toto->job;
	//GVJ_t *job = gvc->job;
	//qDebug()<<agget(g, (char*)"bb");
	QStringList graphSize = QString(agget(g, (char*)"bb")).split(' ');

	if(graphSize[2].toDouble() < 24568 && graphSize[3].toDouble() < 24568) {
		gvRenderFilename(gvc, g, (char *)"png", (char *)pedigree_image_file.c_str());
	}
	else { QMessageBox::warning(this, tr("Pedigree Viewer"), tr("Graph cannot be rendered.\nImage size too large."));
		gvFreeLayout(gvc, g);
		agclose(g);
		gvFreeContext(gvc);
		return;
	}

	gvFreeLayout(gvc, g);
	agclose(g);
	gvFreeContext(gvc);

	QImage image(pedigree_image_file.c_str());

	if( image.isNull() ){
		QMessageBox::information(this, tr("Pedigree Viewer"), tr("Cannot load %1.").arg(pedigree_image_file.c_str()));
	}

	int flag=0;
	int w_height = ui.pedigreeScrollArea6->height();	// scrollArea window height
	int w_width = ui.pedigreeScrollArea6->width();		// scrollArea window width

	if (image.height() < w_height) flag += 1;
	if (image.width()  < w_width)  flag += 2;

	ui.pedigreeLabel6->setMaximumSize(16777215, 16777215);

	switch(flag){
		case 0 : // image larger than label => fit to window size.
			break;
		case 1 : // image height is smaller => fit height only.
			ui.pedigreeLabel6->setMaximumHeight(image.height());
			break;
		case 2 : // image width is smaller => fit width only.
			ui.pedigreeLabel6->setMaximumWidth(image.width());
			break;
		case 3 : // image height and width are smaller => no fit
			ui.pedigreeLabel6->setMaximumSize(image.width(), image.height());
			break;
	}
	ui.pedigreeLabel6->setPixmap(QPixmap::fromImage(image));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ Agraph_t *g: the graph (tree) structure.
	@ string ind: the name of the current individual.
	## RETURN:
	## SPECIFICATION: display the pedigree graph for all the individuals in the
	panel. Arrows are transparent (for a better visibility). Only circle are visible.
	-----------------------------------------------------------------------------
*/

void MainWindow::createAllPedigree(Agraph_t *g, const string & ind){
	Agnode_t *ind_node, *p1_node, *p2_node;
	Agedge_t *e1_edge, *e2_edge;

	const QStringList & ind_data = m_map_scores.find(ind.c_str()).value();
	string p1 = ind_data[0].toStdString();
	string p2 = ind_data[1].toStdString();

	// The current individual is not an inbred line
	if( (m_map_scores.find(ind_data[0]) != m_map_scores.end()) && (m_map_scores.find(ind_data[1]) != m_map_scores.end()) ){
	//if( m_map_scores[ind][0].compare(".") != 0 ){
		// Crosse
		if( ind_data[0].compare(ind_data[1]) != 0 ){
			ind_node = agnode(g, (char *)ind.c_str());
			p1_node = agnode(g, (char *)p1.c_str());
			p2_node = agnode(g, (char *)p2.c_str());
			e1_edge = agedge(g, p1_node, ind_node);
			e2_edge = agedge(g, p2_node, ind_node);
			agsafeset(e1_edge, (char*)"color", (char*)"transparent", (char*)"");
			agsafeset(e2_edge, (char*)"color", (char*)"transparent", (char*)"");
		}
		// Selfing / RIL / HD (one arrow)
		else{
			ind_node = agnode(g, (char *)ind.c_str());
			p1_node = agnode(g, (char *)p1.c_str());
			e1_edge = agedge(g, p1_node, ind_node);
			agsafeset(e1_edge, (char*)"color", (char*)"transparent", (char*)"");
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ Agraph_t *g: the graph (tree) structure.
	@ string ind: the name of the current individual.
	@ int color: [1] if the pedigree need to be colored and [0] otherwise.
	## RETURN:
	## SPECIFICATION: display the pedigree graph of the selected individuals.
	Recursive function.
	-----------------------------------------------------------------------------
*/

void MainWindow::createSelectedPedigree(Agraph_t *g, const string & ind, int color){

	Agnode_t *ind_node, *p1_node, *p2_node;
	Agedge_t *e1_edge, *e2_edge;
	
	const QStringList & ind_data = m_map_scores.find(ind.c_str()).value();
	string p1 = ind_data[0].toStdString();
	string p2 = ind_data[1].toStdString();

	//if( m_map_scores[ind][0].compare(".") != 0 ){
	if( (m_map_scores.find(ind_data[0]) != m_map_scores.end()) && (m_map_scores.find(ind_data[1]) != m_map_scores.end()) ){
		// Selfing / RIL / HD
		if( ind_data[0].compare(ind_data[1]) == 0){
			ind_node = agnode(g, (char *)ind.c_str());
			p1_node = agnode(g, (char *)p1.c_str());
			e1_edge = agedge(g, p1_node, ind_node);

			if( color ){
				// Set an attribute - in this case one that affects the visible rendering
				agsafeset(ind_node, (char*)"color", (char*)"red", (char*)"");
				agsafeset(p1_node, (char*)"color", (char*)"red", (char*)"");
				agsafeset(e1_edge, (char*)"color", (char*)"red", (char*)"");
			}

			createSelectedPedigree(g, p1, color);
		}
		else{
			ind_node = agnode(g, (char *)ind.c_str());
			p1_node = agnode(g, (char *)p1.c_str());
			p2_node = agnode(g, (char *)p2.c_str());
			e1_edge = agedge(g, p1_node, ind_node);
			e2_edge = agedge(g, p2_node, ind_node);

			if( color ){
				// Set an attribute - in this case one that affects the visible rendering
				agsafeset(ind_node, (char*)"color", (char*)"red", (char*)"");
				agsafeset(p1_node, (char*)"color", (char*)"red", (char*)"");
				agsafeset(p2_node, (char*)"color", (char*)"red", (char*)"");
				agsafeset(e1_edge, (char*)"color", (char*)"red", (char*)"");
				agsafeset(e2_edge, (char*)"color", (char*)"red", (char*)"");
			}
			// Recursive function (parents)
			createSelectedPedigree(g, p1, color);
			createSelectedPedigree(g, p2, color);
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the pedigree of the selected list of individuals is drawn.
	-----------------------------------------------------------------------------
*/

void MainWindow::runPedigreeAlone(){

	int index_list_selected = ui.pedigreeListComboBox6->currentIndex();
	string ind = "", pedigree_image_file = m_file_res.toStdString() + "/graph_pedigree.png";

	// set up a graphviz context - but only once even for multiple graphs
	static GVC_t *gvc;
	Agraph_t *g;

	if(!gvc){
		gvc = gvContext();
	}

	// Create a simple digraph
	g = agopen((char *)"g", AGDIGRAPHSTRICT);

	if( ! m_vect_map_selection.empty() ){
		// Display the pedigree graph only for the selected individuals
		for( QMap<QString, int>::iterator iter_list = m_vect_map_selection[index_list_selected]->begin(); iter_list != m_vect_map_selection[index_list_selected]->end(); ++iter_list ){
			ind = iter_list.key().toStdString(); // individual of the list
			Agnode_t *ind_node = agnode(g, (char *)ind.c_str());
			agsafeset(ind_node, (char*)"fontcolor", (char*)"blue", (char*)"");
			createSelectedPedigree(g, ind, 0);
		}
	}

	// Use the directed graph layout engine
	gvLayout(gvc, g, (char *)"dot");
	QStringList graphSize = QString(agget(g, (char*)"bb")).split(' ');

	if(graphSize[2].toDouble() < 24568 && graphSize[3].toDouble() < 24568) {
		gvRenderFilename(gvc, g, (char *)"png", (char *)pedigree_image_file.c_str());
	}
	else { QMessageBox::warning(this, tr("Pedigree Viewer"), tr("Graph cannot be rendered.\nImage size too large."));
		gvFreeLayout(gvc, g);
		agclose(g);
		return;
	}

	gvRenderFilename(gvc, g, (char *)"png", (char *)pedigree_image_file.c_str());

	gvFreeLayout(gvc, g);
	agclose(g);

	QImage image(pedigree_image_file.c_str());

	if( image.isNull() ){
		QMessageBox::information(this, tr("Pedigree Viewer"), tr("Cannot load %1.").arg(pedigree_image_file.c_str()));
	}

	int flag=0;
	int w_height = ui.pedigreeScrollArea6->height();
	int w_width = ui.pedigreeScrollArea6->width();
	//qDebug()<<" IMAGE hauteur : "<<image.height()<<" largeur : "<<image.width();
	//qDebug()<<" WIN hauteur : "<<w_height<<" largeur : "<<w_width;
	if (image.height() < w_height) flag += 1;
	if (image.width()  < w_width)  flag += 2;

	ui.pedigreeLabel6->setMaximumSize(16777215, 16777215);

	switch(flag){
		case 0 : // image larger than label => fit to window size.
			//image = image.scaled(w_width, w_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			//ui.pedigreeLabel6->resize(image.width(), image.height());
			////ui.pedigreeLabel6->adjustSize();
			//ui.pedigreeLabel6->setMaximumSize(w_width, w_height);
			////ui.pedigreeScrollArea6->setWidgetResizable(false);	// avec un connect qui label->adjustSize() a chaque changement de size.
			break;
		case 1 : // image height is smaller => fit height only.
			//image = image.scaledToHeight(w_height);
			//ui.pedigreeLabel6->resize(image.width(), image.height());
			ui.pedigreeLabel6->setMaximumHeight(image.height());
			//ui.pedigreeScrollArea6->setWidgetResizable(true);
			 break;
		case 2 : // image width is smaller => fit width only.
			//image = image.scaledToWidth(w_width);
			//ui.pedigreeLabel6->resize(image.width(), image.height());
			ui.pedigreeLabel6->setMaximumWidth(image.width());
			//ui.pedigreeScrollArea6->setWidgetResizable(true);
			break;
		case 3 : // image height and width are smaller => no fit
			//ui.pedigreeLabel6->resize(image.width(), image.height());
			ui.pedigreeLabel6->setMaximumSize(image.width(), image.height());
			//	ui.pedigreeScrollArea6->setWidgetResizable(true);
			break;
	}

	//ui.pedigreeScrollArea6->setWidgetResizable(false);
	//else ui.pedigreeScrollArea6->setWidgetResizable(true);
	ui.pedigreeLabel6->setPixmap(QPixmap::fromImage(image));
	//ui.pedigreeLabel6->adjustSize();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: save the current pedigree graph.
	-----------------------------------------------------------------------------
*/

void MainWindow::savePedigree(){

	//QString filter = tr("PNG files (*.png);;JPEG files (*.jpg);;BMP files (*.bmp)");
	QString filePedigree = m_file_res + "/graph_pedigree.png"; // Pedigree graph previously generated
	QString filter = tr("PNG files (*.png)");
	QFileDialog fileDialog(this, "Export File Name", m_lastGraphSave, filter);
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);

	if(fileDialog.exec() == QDialog::Accepted){
		QString fileName = fileDialog.selectedFiles().value(0);
		//qDebug() << "filename : " << fileName;

		if( fileName.isEmpty() ){
			QMessageBox::information(this, tr("Save pedigree"), tr("Cannot load %1.").arg(fileName));
			return;
		}

		QString selected_filter = fileDialog.selectedNameFilter();
		//qDebug() << "filter : " << selected_filter;

		QString ext = selected_filter.section('.', -1);
		ext.chop(1);	// skip parenthesis

		if( fileName.section('.', -1) != ext ){
			fileName+= "." + ext;	// add the extention to filname if needed.
		}

		m_lastGraphSave = fileName.section(QRegExp("[/\\\\]"), 0,-2); // sep is either '/' on unix like OS, or '\' on win OS.
		if (m_lastGraphSave.isEmpty()) m_lastGraphSave = "/";

		if( fileName.endsWith(".png") ){
			QFile f(filePedigree);
			if ( f.exists() ) {
				// QFile::copy() does not overwrite an existing file.
				if (QFile::exists(fileName)) QFile::remove(fileName);
				f.copy(fileName);
				ui.bottomTextBrowser->setText("Image " + fileName + " saved.");
			}
			else{
				QMessageBox::information(this, tr("Save pedigree"), tr("Cannot save image %1.").arg(fileName));
				return;
			}
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QListWidgetItem *item: current double clicked item in the list
	## RETURN:
	## SPECIFICATION: the list of crosses become editable.
	-----------------------------------------------------------------------------
*/

void MainWindow::editCrossItemList(QListWidgetItem *item){
	item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable );
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QListWidgetItem *item: current double clicked item in the list
	## RETURN:
	## SPECIFICATION: the list (of crosses) name of the current item is updated
	when the user renames the list.
	-----------------------------------------------------------------------------
*/

void MainWindow::itemListCrossChanged(QListWidgetItem * item){
	if( ! m_list_crosses.empty() ){
		if( item->text().compare(m_list_crosses[ui.crossListWidget4->currentRow()]) != 0 ){
			m_list_crosses[ui.crossListWidget4->currentRow()] = item->text();

			// Update of the combobox list
			ui.leftListComboBox4->setItemText(ui.crossListWidget4->currentRow(), item->text());
			ui.rightListComboBox4->setItemText(ui.crossListWidget4->currentRow(), item->text());
			ui.crossesListComboBox4->setItemText(ui.crossListWidget4->currentRow(), item->text());
			ui.graphLeftListComboBox4->setItemText(ui.crossListWidget4->currentRow(), item->text());
			ui.graphRightListComboBox4->setItemText(ui.crossListWidget4->currentRow(), item->text());
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QListWidgetItem *item: current clicked item in the list of crosses
	## RETURN:
	## SPECIFICATION: The current selected list (clicked on the QListWidgetItem)
	is updated on the left table.
	-----------------------------------------------------------------------------
*/

void MainWindow::leftListCrossChanged(QListWidgetItem *item){

	if( ! m_list_crosses.empty() ){
		if( item->text().compare(ui.leftListComboBox4->currentText()) != 0 ){
			ui.leftListComboBox4->setCurrentIndex(ui.crossListWidget4->currentRow());
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: run the correspondent crossing method after that the user
	pressed the Run button.
	-----------------------------------------------------------------------------
*/

void MainWindow::runCrosses(){
	// If the lists of selection and crosses are not empty
	if( (! m_vect_map_selection.empty()) && (! m_vect_map_crosses.empty()) ){
		// Intermating
		int sum = m_vect_cross_option[0] + m_vect_cross_option[1];

		// 1 list
		if( m_vect_cross_option[10] == 0 ){
			if( sum == 2 ){
			//if( (m_vect_cross_option[0] == 1) && (m_vect_cross_option[1] == 1) ){
				// Complete crosses method
				if( m_vect_cross_option[5] == 0 ){
					runCompleteCrosses();
				}
				// Better Half (Bernardo et al)
				else if( m_vect_cross_option[5] == 1 ){
					runBetterHalf();
				}
			}
			// Complete crosses method sorted by criterion and based on constraints
			else if( sum == 0 || sum == 1 ){
			//else if( ((m_vect_cross_option[0] == 0) && (m_vect_cross_option[1] == 0)) || ((m_vect_cross_option[0] == 1) && (m_vect_cross_option[1] == 0)) || ((m_vect_cross_option[0] == 0) && (m_vect_cross_option[1] == 1)) ){
				runCompleteCrossesConstraints();
			}
		}
		// 2 lists crossed
		else if( m_vect_cross_option[10] == 1 ){
			if( sum == 2 ){
				run2ListsCrosses();
			}
			else{
				run2ListsCrossesConstraints();
			}
		}
	}
	else{
		showCustomErrorMessage("A list of selection and crosses need to be created before the run !");
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: all the crosses between the selected individuals will be done.
	Note that an individual is not crosses with itself.
	-----------------------------------------------------------------------------
*/

void MainWindow::runCompleteCrosses(){

	double d = 0.0, mu = 0.0, sigma = 0.0, uc = 0.0, score_ind1 = 0.0, score_ind2 = 0.0;
	int cpt_crosses = 0, cpt = 0, index_list_selection = ui.selectionListComboBox4->currentIndex(), index_list_crosses = ui.crossesListComboBox4->currentIndex();
	int index_criterion_selected = 0, nb_indiv_selected = (int)m_vect_map_selection[index_list_selection]->size();
	double nb_qtl = ui.scoreTableWidget->columnCount() - COL_FIRST_QTL;
	
	QMap<QString, QStringList> & map_crosses_list_i = *(m_vect_map_crosses[index_list_crosses]);
	
	if( ! map_crosses_list_i.empty() ){

		int answer = QMessageBox::question(this, tr("Creation of a new list of crosses"), tr("The List %1 is not empty. Do you want to replace and overwrite its contents ?").arg(ui.crossesListComboBox4->currentText()), QMessageBox::Yes | QMessageBox::No);

		// Yes : the current list is removed
		if( answer == QMessageBox::Yes ) {
			map_crosses_list_i.clear();
		}
		else if( answer == QMessageBox::No ){
			return;
		}
	}

	// Notes for the crossing method
	m_vect_notes_crosses[index_list_crosses][0] = "Method: complete (half-diallel)"; // Method
	m_vect_notes_crosses[index_list_crosses][1] = QString("List: ") + m_list_selection[index_list_selection]; // List of selection (coming from)
	m_vect_notes_crosses[index_list_crosses][2] = "Criterion: no"; // Criterion

	setLabelCrossPage(ui.leftListResumeFrame4, index_list_crosses);

	// Vector of future sorted individuals (by molecular score)
	QVector<map_iterator> vect_it(nb_indiv_selected);

	// Copy of the current list of individuals (not ordered)
	for( QMap<QString, int>::iterator iter_list = m_vect_map_selection[index_list_selection]->begin(); iter_list != m_vect_map_selection[index_list_selection]->end(); ++iter_list ){
		vect_it[cpt] = m_map_scores.find(iter_list.key()); // iterator on the pair that has this indiv as a key in m_map_scores.
		cpt++;
	}

	// Individuals are sorted regarding their criterion (molecular score...) for all QTL
	crit_index = index_criterion_selected + (COL_MS - 1); // crit_index is global.
	qSort(vect_it.begin(), vect_it.end(), criterionMoreThan);

	int nb_columns = ui.leftListTableWidget4->columnCount();

	for(int i=0; i< (nb_indiv_selected - 1); i++){
		for(int j=(i+1); j< nb_indiv_selected; j++){
			mu = 0.0;
			sigma = 0.0;
			uc = 0.0;

			const QString & p1 = vect_it[i].key();
			const QString & p2 = vect_it[j].key();

			QString str = p1 + "x" + p2; // New virtual individual
			vector<int> vect_nb_geno(4, 0); // No.(+/+) / No.(-/-) / No.(+/-) / No.(?)

			map_crosses_list_i[str].push_back(p1); // p1
			map_crosses_list_i[str].push_back(p2); // p2


			for(int k = (COL_MS - 2); k<nb_columns; k++){
				// MS / Weight
				if( k == (COL_MS - 2) || k == (COL_WEIGHT - 2) ){
					// Means of molecular scores for couples are computed
					d = (m_map_scores[p1][k+1].toDouble() + m_map_scores[p2][k+1].toDouble()) / 2.0;
					map_crosses_list_i[str].push_back(QString(" ").setNum(d, 'f', 6));
				}
				// UC (Utility Criterion)
				else if( k == (COL_UC - 2) ){
					map_crosses_list_i[str].push_back("0.000000");
				}
				// No.(+/+) / No.(-/-) / No.(+/-) / No.(?)
				else if( (k >= (COL_NO_FAV - 2)) && (k <=(COL_NO_UNCERTAIN - 2)) ){
					map_crosses_list_i[str].push_back("0.000000");
				}
				// Each QTL
				else if( k >= (COL_FIRST_QTL - 2) ){
					score_ind1 = m_map_scores[p1][k+1].toDouble();
					score_ind2 = m_map_scores[p2][k+1].toDouble();

					// Means of molecular scores for couples are computed at each QTL position
					d = (score_ind1 + score_ind2) / 2.0;
					map_crosses_list_i[str].push_back(QString(" ").setNum(d, 'f', 6));

					// Visualization of genotypes via colors for each QTL
					double score_homo_fav = (m_homo_fav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_fav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_homo_unfav = (m_homo_unfav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_unfav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_hetero = (m_hetero[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_hetero[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;

					// Homozygous favorable
					if( score_homo_fav >= m_vect_cut_off[0] ){
						vect_nb_geno[0]++;
					}
					// Homozygous unfavorable
					else if( score_homo_unfav >= m_vect_cut_off[1] ){
						vect_nb_geno[1]++;
					}
					// Hetero
					else if( score_hetero >= m_vect_cut_off[2] ){
						vect_nb_geno[2]++;
					}
					// ? uncertain genotypes
					else{
						vect_nb_geno[3]++;
					}

					vector<double> val_homo_hetero_p1(3, 0.0), val_homo_hetero_p2(3, 0.0); // Score homo fav / *Score hetero* / Score Homo unfav
					val_homo_hetero_p1[0] = m_homo_fav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p1[1] = m_hetero[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p1[2] = m_homo_unfav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[0] = m_homo_fav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[1] = m_hetero[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[2] = m_homo_unfav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();

					// Utility criterion
					mu = mu + (score_ind1 + score_ind2); // Mean of Molecular Score
					//mu = mu + ((discretization(score_ind1) + discretization(score_ind2)) / 2.0);

					// Computation of the number of heterozygous for the UC method (discretization via the genotype with the higher probability)
					sigma = sigma + (nb_hetero(maxIndex(val_homo_hetero_p1, 3), maxIndex(val_homo_hetero_p2, 3)) * 0.25);
					//sigma = sigma + (nb_hetero(discretization(score_ind1), discretization(score_ind2)) * 0.25);

					val_homo_hetero_p1.clear();
					val_homo_hetero_p2.clear();
				}
			}

			// Utility criterion
			uc = mu + (2 * sqrt(sigma));

			if( uc > (nb_qtl*2) ) uc = (nb_qtl * 2); // Utility criterion borned

			map_crosses_list_i[str][COL_MS - 1] = QString(" ").setNum(uc/2.0, 'f', 6);

			// Update No(+/+), No(-/-), No(+/-), No(?)
			for(int k=(COL_NO_FAV - 2); k<=(COL_NO_UNCERTAIN - 2); k++){
				// Update map
				map_crosses_list_i[str][k - 1] = QString(" ").setNum(vect_nb_geno[k - COL_NO_FAV + 2]);
			}

			vect_nb_geno.clear();
			cpt_crosses++;
		}
	}

	displayListCrosses(ui.leftListTableWidget4, index_list_crosses);

	// Change the name of the current list in the left table
	ui.leftListComboBox4->setCurrentIndex(index_list_crosses);

	// The current list is in the left and right
	if( ui.rightListComboBox4->currentIndex() == index_list_crosses ){
		displayListCrosses(ui.rightListTableWidget4, index_list_crosses);
	}

	// The current plots are updated
	if( ui.graphLeftListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphLeftQwtPlot4, index_list_crosses);
	}
	if( ui.graphRightListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphRightQwtPlot4, index_list_crosses);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int disc_ind1: discretization of the indivual 1 of the couple.
	@ int disc_ind2: discretization of the indivual 2 of the couple.
	## RETURN: [0, 1 or 2] the number of heterozygous individuals in the couple.
	## SPECIFICATION: count the number of heterozygous individuals in the couple
	after discretization.
	-----------------------------------------------------------------------------
*/

int MainWindow::nb_hetero(int disc_ind1, int disc_ind2){
	int nb_hetero = 0;

	if( disc_ind1 == 1 ){
		nb_hetero++;
	}
	if( disc_ind2 == 1 ){
		nb_hetero++;
	}
	return nb_hetero;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: all the crosses between the selected individuals will be done.
	Note that an individual is not crosses with himself. The crosses are then filtered
	regarding such constraints as the number of crosses to be made or each individual
	can be crosses only n time(s).
	-----------------------------------------------------------------------------
*/

void MainWindow::runCompleteCrossesConstraints(){
	QString str = "";
	double d = 0.0, mu = 0.0, sigma = 0.0, uc = 0.0, score_ind1 = 0.0, score_ind2 = 0.0;
	double nb_qtl = ui.scoreTableWidget->columnCount() - COL_FIRST_QTL;
	int cpt_crosses = 0, cpt = 0, index_list_selection = ui.selectionListComboBox4->currentIndex(), index_list_crosses = ui.crossesListComboBox4->currentIndex();
	int index_criterion_selected = m_vect_cross_option[4], nb_indiv_selected = (int)m_vect_map_selection[index_list_selection]->size();
	int nb_total_crosses = (nb_indiv_selected * (nb_indiv_selected - 1)) / 2, nb_crosses_selected = 0, nb_cross_per_ind = 0;

	QMap<QString, QStringList> & map_crosses_list_i = *(m_vect_map_crosses[index_list_crosses]);
	QMap<QString, int> & map_selected_list_i = *(m_vect_map_selection[index_list_selection]);

	// Notes for the crossing method
	m_vect_notes_crosses[index_list_crosses][0] = "Method: constraints ("; // Method
	m_vect_notes_crosses[index_list_crosses][1] = QString("List: ") + m_list_selection[index_list_selection]; // List of selection (coming from)
	m_vect_notes_crosses[index_list_crosses][2] = QString("Criterion: ") + getCriterion(index_criterion_selected); // Criterion

	// All crosses
	if( m_vect_cross_option[0] == 1 ){
		nb_crosses_selected = nb_total_crosses;
		m_vect_notes_crosses[index_list_crosses][0] += "No.crosses: max - "; // No crosses
	}
	// Constraint on the number of crosses to be made
	else{
		nb_crosses_selected = m_vect_cross_option[2];
		m_vect_notes_crosses[index_list_crosses][0] += QString("No.crosses: %1 - ").arg(nb_crosses_selected, 0, 10); // No crosses
	}

	// All crosses for each individual
	if( m_vect_cross_option[1] == 1 ){
		nb_cross_per_ind = nb_indiv_selected - 1;
		m_vect_notes_crosses[index_list_crosses][0] += "Ind contrib: unltd)"; // Individual contribution
	}
	// Each individual can be crossed n times
	else{
		nb_cross_per_ind = m_vect_cross_option[3];
		m_vect_notes_crosses[index_list_crosses][0] += QString("Ind contrib: %1)").arg(nb_cross_per_ind, 0, 10); // Individual contribution
	}

	setLabelCrossPage(ui.leftListResumeFrame4, index_list_crosses);

	if( ! map_crosses_list_i.empty() ){

		int answer = QMessageBox::question(this, tr("Creation of a new list of crosses"), tr("The List %1 is not empty. Do you want to replace and overwrite its contents ?").arg(ui.crossesListComboBox4->currentText()), QMessageBox::Yes | QMessageBox::No);

		// Yes : the current list is removed
		if( answer == QMessageBox::Yes ) {
			map_crosses_list_i.clear();
		}
		else if( answer == QMessageBox::No ){
			return;
		}
	}

	ui.leftListTableWidget4->setSortingEnabled(false);

	int cpt_couples = 0, nb_columns = ui.leftListTableWidget4->columnCount();

	QVector<QString> vect_couples_to_remove; // Vector of couples to remove
	QMap<QString, int> map_ind_selected; // Map of the contribution of each selected individual in couples that will be selected

	QVector<map_iterator> vect_it_ind(nb_indiv_selected); // Vector of future sorted individuals (by molecular score)
	QVector<map_iterator> vect_it_couples(nb_total_crosses); // Vector of future sorted couples (by criterion: molecular score)
	QVector<map_iterator> vect_it_couples_to_remove; // Vector of couples to remove

	// Copy of the current list of individuals (not ordered)
	for( QMap<QString, int>::iterator iter_list = map_selected_list_i.begin(); iter_list != map_selected_list_i.end(); ++iter_list ){
		vect_it_ind[cpt] = m_map_scores.find(iter_list.key());  // iterator on m_map_scores corresponding to the individual of the list
		map_ind_selected[iter_list.key()] = 0; // Initialization of the map of individual contribution
		cpt++;
	}

	// Individuals are sorted regarding their criterion (molecular score...)
	crit_index = index_criterion_selected + COL_MS - 1;
	qSort(vect_it_ind.begin(), vect_it_ind.end(), criterionMoreThan);

	// Creation of the complete list of crosses
	for(int i=0; i< (nb_indiv_selected - 1); i++){
		for(int j=(i+1); j< nb_indiv_selected; j++){
			mu = 0.0;
			sigma = 0.0;
			uc = 0.0;

			const QString & p1 = vect_it_ind[i].key();
			const QString & p2 = vect_it_ind[j].key();

			str = p1 + "x" + p2; // New virtual individual
			vector<int> vect_nb_geno(4, 0); // No.(+/+) / No.(-/-) / No.(+/-) / No.(?)

			map_crosses_list_i[str].push_back(p1); // p1
			map_crosses_list_i[str].push_back(p2); // p2
			vect_it_couples[cpt_crosses] = m_vect_map_crosses[index_list_crosses]->find(str); // ind (couples)


			for(int k = (COL_MS - 2); k<nb_columns; k++){
				// MS / Weight
				if( k == (COL_MS - 2) || k == (COL_WEIGHT - 2) ){
					// Means of molecular scores for couples are computed
					d = ( m_map_scores[p1][k+1].toDouble() + m_map_scores[p2][k+1].toDouble() ) / 2.0;
					map_crosses_list_i[str].push_back(QString(" ").setNum(d, 'f', 6));
				}
				// Utility Criterion
				else if( k == (COL_UC - 2) ){
					map_crosses_list_i[str].push_back("0.000000");
				}
				// No.(+/+) / No.(-/-) / No.(+/-) / No.(?)
				else if( (k >= (COL_NO_FAV - 2)) && (k <=(COL_NO_UNCERTAIN - 2)) ){
					map_crosses_list_i[str].push_back("0.000000");
				}
				// Each QTL
				else if( k >= (COL_FIRST_QTL - 2) ){
					score_ind1 = m_map_scores[p1][k+1].toDouble();
					score_ind2 = m_map_scores[p2][k+1].toDouble();

					d = (score_ind1 + score_ind2) / 2.0;
					map_crosses_list_i[str].push_back(QString(" ").setNum(d, 'f', 6));

					// Visualization of genotypes via colors for each QTL
					double score_homo_fav = (m_homo_fav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_fav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_homo_unfav = (m_homo_unfav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_unfav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_hetero = (m_hetero[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_hetero[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;

					// Homozygous favorable
					if( score_homo_fav >= m_vect_cut_off[0] ){
						vect_nb_geno[0]++;
					}
					// Homozygous unfavorable
					else if( score_homo_unfav >= m_vect_cut_off[1] ){
						vect_nb_geno[1]++;
					}
					// Hetero
					else if( score_hetero >= m_vect_cut_off[2] ){
						vect_nb_geno[2]++;
					}
					// ? uncertain genotypes
					else{
						vect_nb_geno[3]++;
					}

					vector<double> val_homo_hetero_p1(3, 0.0), val_homo_hetero_p2(3, 0.0); // Score homo fav / *Score hetero* / Score Homo unfav
					val_homo_hetero_p1[0] = m_homo_fav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p1[1] = m_hetero[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p1[2] = m_homo_unfav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[0] = m_homo_fav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[1] = m_hetero[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[2] = m_homo_unfav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();

					// Utility criterion
					mu = mu + (score_ind1 + score_ind2); // Mean of Molecular Score
					//mu = mu + ((discretization(score_ind1) + discretization(score_ind2)) / 2.0);

					// Computation of the number of heterozygous for the UC method (discretization via the genotype with the higher probability)
					sigma = sigma + (nb_hetero(maxIndex(val_homo_hetero_p1, 3), maxIndex(val_homo_hetero_p2, 3)) * 0.25);
					//sigma = sigma + (nb_hetero(discretization(score_ind1), discretization(score_ind2)) * 0.25);

					val_homo_hetero_p1.clear();
					val_homo_hetero_p2.clear();
				}
			}

			// Utility criterion
			uc = mu + (2 * sqrt(sigma));

			if( uc > (nb_qtl * 2) ) uc = (nb_qtl * 2); // Utility criterion borned

			map_crosses_list_i[str][COL_MS - 1] = QString(" ").setNum(uc/2.0, 'f', 6);

			// Update No(+/+), No(-/-), No(+/-), No(?)
			for(int k=(COL_NO_FAV - 2); k<=(COL_NO_UNCERTAIN - 2); k++){
				// Update map and table
				map_crosses_list_i[str][k - 1] = QString(" ").setNum(vect_nb_geno[k - COL_NO_FAV + 2]);
			}

			cpt_crosses++;
		}
	}

	// Couples are sorted regarding the criterion selected (on the m_vect_map_crosses map iterator)
	crit_index = index_criterion_selected + (COL_MS - 1) - 2; // crit_index is global
	qSort(vect_it_couples.begin(), vect_it_couples.end(), criterionMoreThan);

	// Loop on all the possible couples
	for(int i=0; i < nb_total_crosses; i++){

		const QString & p1 = map_crosses_list_i[vect_it_couples[i].key()][0];
		const QString & p2 = map_crosses_list_i[vect_it_couples[i].key()][1];

		// The couple is selected if the individuals can still contribute
		if( (map_ind_selected[p1] < nb_cross_per_ind) && (map_ind_selected[p2] < nb_cross_per_ind) && (cpt_couples < nb_crosses_selected) ){
			map_ind_selected[p1]++;
			map_ind_selected[p2]++;
			cpt_couples++;
		}
		// The couple is removed from the list of selected crosses
		else{
			vect_couples_to_remove.push_back(vect_it_couples[i].key());
		}
	}

	// Resume of the selected individuals who have not contributed
	//// map_ind_selected[ind] == 0

	// The unselected couples are removed from the list
	for(int i=0; i<(int)vect_couples_to_remove.size(); i++){
		map_crosses_list_i.remove(vect_couples_to_remove[i]);
	}

	// The table of selected couples is displayed
	displayListCrosses(ui.leftListTableWidget4, index_list_crosses);

	// Change the name of the current list in the left table
	ui.leftListComboBox4->setCurrentIndex(index_list_crosses);

	// The current list is in the left and right
	if( ui.rightListComboBox4->currentIndex() == index_list_crosses ){
		displayListCrosses(ui.rightListTableWidget4, index_list_crosses);
	}

	// The current plots are updated
	if( ui.graphLeftListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphLeftQwtPlot4, index_list_crosses);
	}
	if( ui.graphRightListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphRightQwtPlot4, index_list_crosses);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_criterion: index of the selected criterion.
	## RETURN: (QString) the name of the selected criterion.
	## SPECIFICATION: notes (label) for the crossing method.
	-----------------------------------------------------------------------------
*/
QString MainWindow::getCriterion(int index_criterion){
	QString criterion("no");

	if( index_criterion == 0 ){
		return "MS";
	}
	else if( index_criterion == 1 ){
		return "weight";
	}
	else if( index_criterion == 2 ){
		return "UC";
	}
	return criterion;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: avoid crosses between the last selected genotypes
	(following Bernardo et al., 2006).
	-----------------------------------------------------------------------------
*/

void MainWindow::runBetterHalf(){

	double d = 0.0, mu = 0.0, sigma = 0.0, uc = 0.0, score_ind1 = 0.0, score_ind2 = 0.0;
	double nb_qtl = ui.scoreTableWidget->columnCount() - COL_FIRST_QTL;
	int cpt_crosses = 0, cpt = 0, index_list_selection = ui.selectionListComboBox4->currentIndex(), index_list_crosses = ui.crossesListComboBox4->currentIndex();
	int index_criterion_selected = 0, nb_indiv_selected = (int)m_vect_map_selection[index_list_selection]->size();

	QMap <QString, QStringList > & map_crosses_list_i = *(m_vect_map_crosses[index_list_crosses]);

	if( ! map_crosses_list_i.empty() ){

		int answer = QMessageBox::question(this, tr("Creation of a new list of crosses"), tr("The List %1 is not empty. Do you want to replace and overwrite its contents ?").arg(ui.crossesListComboBox4->currentText()), QMessageBox::Yes | QMessageBox::No);

		// Yes : the current list is removed
		if( answer == QMessageBox::Yes ) {
			map_crosses_list_i.clear();
		}
		else if( answer == QMessageBox::No ){
			return;
		}
	}

	// Notes for the crossing method
	m_vect_notes_crosses[index_list_crosses][0] = "Method: better half"; // Method
	m_vect_notes_crosses[index_list_crosses][1] = QString("List: ") + m_list_selection[index_list_selection]; // List of selection (coming from)
	m_vect_notes_crosses[index_list_crosses][2] = "Criterion: no"; // Criterion

	setLabelCrossPage(ui.leftListResumeFrame4, index_list_crosses);

	// Vector of future sorted individuals (by molecular score)
	QVector<map_iterator> vect_ind(nb_indiv_selected);

	// Copy of the current list of individuals (not ordered)
	for( QMap<QString, int>::iterator iter_list = m_vect_map_selection[index_list_selection]->begin(); iter_list != m_vect_map_selection[index_list_selection]->end(); ++iter_list ){
		vect_ind[cpt] = m_map_scores.find(iter_list.key());
		cpt++;
	}

	// Individuals are sorted regarding their criterion (molecular score...) for all QTL
	crit_index = index_criterion_selected + (COL_MS - 1); // crit_index is global.
	qSort(vect_ind.begin(), vect_ind.end(), criterionMoreThan);

	int nb_columns = ui.leftListTableWidget4->columnCount();

	for(int i=0; i< (nb_indiv_selected / 2); i++){
		for(int j=(i+1); j< (nb_indiv_selected - i); j++){
			mu = 0.0;
			sigma = 0.0;
			uc = 0.0;

			const QString & p1 = vect_ind[i].key();
			const QString & p2 = vect_ind[j].key();

			QString str = p1 + "x" + p2;
			vector<int> vect_nb_geno(4, 0); // No.(+/+) / No.(-/-) / No.(+/-) / No.(?)

			map_crosses_list_i[str].push_back(p1); // p1
			map_crosses_list_i[str].push_back(p2); // p2


			for(int k = (COL_MS - 2); k<nb_columns; k++){
				// MS / Weight
				if( k == (COL_MS - 2) || k == (COL_WEIGHT - 2) ){
					// Means of couples molecular scores are computed
					d = (m_map_scores[p1][k+1].toDouble() + m_map_scores[p2][k+1].toDouble()) / 2.0;
					map_crosses_list_i[str].push_back(QString(" ").setNum(d, 'f', 6));
				}
				// Utility Criterion
				else if( k == (COL_UC - 2) ){
					map_crosses_list_i[str].push_back("0.000000");
				}
				// No.(+/+) / No.(-/-) / No.(+/-) / No.(?)
				else if( (k >= (COL_NO_FAV - 2)) && (k <=(COL_NO_UNCERTAIN - 2)) ){
					map_crosses_list_i[str].push_back("0.000000");
				}
				// Each QTL
				else if( k >= (COL_FIRST_QTL - 2) ){
					score_ind1 = m_map_scores[p1][k+1].toDouble();
					score_ind2 = m_map_scores[p2][k+1].toDouble();

					d = (score_ind1 + score_ind2) / 2.0;
					map_crosses_list_i[str].push_back(QString(" ").setNum(d, 'f', 6));

					// Visualization of genotypes via colors for each QTL
					double score_homo_fav = (m_homo_fav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_fav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_homo_unfav = (m_homo_unfav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_unfav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_hetero = (m_hetero[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_hetero[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;

					// Homozygous favorable
					if( score_homo_fav >= m_vect_cut_off[0] ){
						vect_nb_geno[0]++;
					}
					// Homozygous unfavorable
					else if( score_homo_unfav >= m_vect_cut_off[1] ){
						vect_nb_geno[1]++;
					}
					// Hetero
					else if( score_hetero >= m_vect_cut_off[2] ){
						vect_nb_geno[2]++;
					}
					// ? uncertain genotypes
					else{
						vect_nb_geno[3]++;
					}

					vector<double> val_homo_hetero_p1(3, 0.0), val_homo_hetero_p2(3, 0.0); // Score homo fav / *Score hetero* / Score Homo unfav
					val_homo_hetero_p1[0] = m_homo_fav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p1[1] = m_hetero[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p1[2] = m_homo_unfav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[0] = m_homo_fav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[1] = m_hetero[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[2] = m_homo_unfav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();

					// Utility criterion
					mu = mu + (score_ind1 + score_ind2); // Mean of Molecular Score
					//mu = mu + ((discretization(score_ind1) + discretization(score_ind2)) / 2.0);

					// Computation of the number of heterozygous for the UC method (discretization via the genotype with the higher probability)
					sigma = sigma + (nb_hetero(maxIndex(val_homo_hetero_p1, 3), maxIndex(val_homo_hetero_p2, 3)) * 0.25);
					//sigma = sigma + (nb_hetero(discretization(score_ind1), discretization(score_ind2)) * 0.25);

					val_homo_hetero_p1.clear();
					val_homo_hetero_p2.clear();
				}
			}

			// Utility criterion
			uc = mu + (2 * sqrt(sigma));

			if( uc > (nb_qtl * 2) ) uc = (nb_qtl * 2); // Utility criterion borned

			map_crosses_list_i[str][COL_MS - 1] = QString(" ").setNum(uc/2.0, 'f', 6);

			// Update No(+/+), No(-/-), No(+/-), No(?)
			for(int k=(COL_NO_FAV - 2); k<=(COL_NO_UNCERTAIN - 2); k++){
				// Update map
				map_crosses_list_i[str][k - 1] = QString(" ").setNum(vect_nb_geno[k - COL_NO_FAV + 2]);
			}

			cpt_crosses++;
		}
	}

	// Change the name of the current list in the left table
	ui.leftListComboBox4->setCurrentIndex(index_list_crosses);

	// The table of selected couples is displayed
	displayListCrosses(ui.leftListTableWidget4, index_list_crosses);

	// The current list is in the left and right
	if( ui.rightListComboBox4->currentIndex() == index_list_crosses ){
		displayListCrosses(ui.rightListTableWidget4, index_list_crosses);
	}

	// The current plots are updated
	if( ui.graphLeftListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphLeftQwtPlot4, index_list_crosses);
	}
	if( ui.graphRightListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphRightQwtPlot4, index_list_crosses);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: all the crosses between the 2 lists of selected individuals
	will be done.
	-----------------------------------------------------------------------------
*/

void MainWindow::run2ListsCrosses(){

	double d = 0.0, mu = 0.0, sigma = 0.0, uc = 0.0, score_ind1 = 0.0, score_ind2 = 0.0;
	double nb_qtl = ui.scoreTableWidget->columnCount() - COL_FIRST_QTL;
	int cpt_crosses = 0, nb_columns = ui.leftListTableWidget4->columnCount();
	int index_list1_selection = ui.selectionListComboBox4->currentIndex(), index_list2_selection = ui.selectionList2ComboBox4->currentIndex();
	int index_list_crosses = ui.crossesListComboBox4->currentIndex();
	//int nb_indiv_list1_selected = (int)m_vect_map_selection[index_list1_selection]->size(), nb_indiv_list2_selected = (int)m_vect_map_selection[index_list2_selection]->size();

	QMap<QString, QStringList> & map_crosses_list_i = *(m_vect_map_crosses[index_list_crosses]);

	if( ! map_crosses_list_i.empty() ){

		int answer = QMessageBox::question(this, tr("Creation of a new list of crosses"), tr("The List %1 is not empty. Do you want to replace and overwrite its contents ?").arg(ui.crossesListComboBox4->currentText()), QMessageBox::Yes | QMessageBox::No);

		// Yes : the current list is removed
		if( answer == QMessageBox::Yes ) {
			map_crosses_list_i.clear();
		}
		else if( answer == QMessageBox::No ){
			return;
		}
	}

	// Notes for the crossing method
	m_vect_notes_crosses[index_list_crosses][0] = "Method: 2 lists (factorial design)"; // Method
	m_vect_notes_crosses[index_list_crosses][1] = QString("List: ") + m_list_selection[index_list1_selection] + QString(" x ") + m_list_selection[index_list2_selection]; // List of selection (coming from)
	m_vect_notes_crosses[index_list_crosses][2] = "Criterion: no"; // Criterion

	setLabelCrossPage(ui.leftListResumeFrame4, index_list_crosses);

	for( QMap<QString, int>::iterator iter_list1 = m_vect_map_selection[index_list1_selection]->begin(); iter_list1 != m_vect_map_selection[index_list1_selection]->end(); ++iter_list1){
		const QString & p1 = iter_list1.key();

		for( QMap<QString, int>::iterator iter_list2 = m_vect_map_selection[index_list2_selection]->begin(); iter_list2 != m_vect_map_selection[index_list2_selection]->end(); ++iter_list2){
			const QString & p2 = iter_list2.key();

			mu = 0.0;
			sigma = 0.0;
			uc = 0.0;

			QString str = p1 + "x" + p2;
			vector<int> vect_nb_geno(4, 0); // No.(+/+) / No.(-/-) / No.(+/-) / No.(?)

			map_crosses_list_i[str].push_back(p1); // p1
			map_crosses_list_i[str].push_back(p2); // p2


			for(int k = (COL_MS - 2); k<nb_columns; k++){
				// All / Weight
				if( k == (COL_MS - 2) || k == (COL_WEIGHT - 2) ){
					// Means of molecular scores for couples are computed
					d = (m_map_scores[p1][k+1].toDouble() + m_map_scores[p2][k+1].toDouble()) / 2.0;
					map_crosses_list_i[str].push_back(QString(" ").setNum(d));
				}
				// Utility Criterion
				else if( k == (COL_UC - 2) ){
					map_crosses_list_i[str].push_back("0.000000");
				}
				// No.(+/+) / No.(-/-) / No.(+/-) / No.(?)
				else if( (k >= (COL_NO_FAV - 2)) && (k <=(COL_NO_UNCERTAIN - 2)) ){
					map_crosses_list_i[str].push_back("0.000000");
				}
				// Each QTL
				else if( k >= (COL_FIRST_QTL - 2) ){
					score_ind1 = m_map_scores[p1][k+1].toDouble();
					score_ind2 = m_map_scores[p2][k+1].toDouble();

					d = (score_ind1 + score_ind2) / 2.0;
					map_crosses_list_i[str].push_back(QString(" ").setNum(d));

					// Visualization of genotypes via colors for each QTL
					double score_homo_fav = (m_homo_fav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_fav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_homo_unfav = (m_homo_unfav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_unfav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_hetero = (m_hetero[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_hetero[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;

					// Homozygous favorable
					if( score_homo_fav >= m_vect_cut_off[0] ){
						vect_nb_geno[0]++;
					}
					// Homozygous unfavorable
					else if( score_homo_unfav >= m_vect_cut_off[1] ){
						vect_nb_geno[1]++;
					}
					// Hetero
					else if( score_hetero >= m_vect_cut_off[2] ){
						vect_nb_geno[2]++;
					}
					// ? uncertain genotypes
					else{
						vect_nb_geno[3]++;
					}

					vector<double> val_homo_hetero_p1(3, 0.0), val_homo_hetero_p2(3, 0.0); // Score homo fav / *Score hetero* / Score Homo unfav
					val_homo_hetero_p1[0] = m_homo_fav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p1[1] = m_hetero[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p1[2] = m_homo_unfav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[0] = m_homo_fav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[1] = m_hetero[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[2] = m_homo_unfav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();

					// Utility criterion
					mu = mu + (score_ind1 + score_ind2); // Mean of Molecular Score
					//mu = mu + ((discretization(score_ind1) + discretization(score_ind2)) / 2.0);

					// Computation of the number of heterozygous for the UC method (discretization via the genotype with the higher probability)
					sigma = sigma + (nb_hetero(maxIndex(val_homo_hetero_p1, 3), maxIndex(val_homo_hetero_p2, 3)) * 0.25);
					//sigma = sigma + (nb_hetero(discretization(score_ind1), discretization(score_ind2)) * 0.25);

					val_homo_hetero_p1.clear();
					val_homo_hetero_p2.clear();
				}
			}

			// Utility criterion
			uc = mu + (2 * sqrt(sigma));

			if( uc > (nb_qtl * 2 ) ) uc = (nb_qtl * 2); // Utility criterion borned

			map_crosses_list_i[str][COL_MS - 1] = QString(" ").setNum(uc/2.0);

			// Update No(+/+), No(-/-), No(+/-), No(?)
			for(int k=(COL_NO_FAV - 2); k<=(COL_NO_UNCERTAIN - 2); k++){
				// Update map
				map_crosses_list_i[str][k - 1] = QString(" ").setNum(vect_nb_geno[k - COL_NO_FAV + 2]);
			}

			cpt_crosses++;
		}
	}

	// Change the name of the current list in the left table
	ui.leftListComboBox4->setCurrentIndex(index_list_crosses);

	displayListCrosses(ui.leftListTableWidget4, index_list_crosses);

	// The current list is in the left and right
	if( ui.rightListComboBox4->currentIndex() == index_list_crosses ){
		displayListCrosses(ui.rightListTableWidget4, index_list_crosses);
	}

	// The current plots are updated
	if( ui.graphLeftListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphLeftQwtPlot4, index_list_crosses);
	}
	if( ui.graphRightListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphRightQwtPlot4, index_list_crosses);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: all the crosses between the selected individuals of the 2
	selected lists will be done. The crosses are then filtered regarding such
	constraints as the number of crosses to be made or each individual can be
	crosses only n time(s).
	-----------------------------------------------------------------------------
*/

void MainWindow::run2ListsCrossesConstraints(){
	QString str = "";
	double d = 0.0, mu = 0.0, sigma = 0.0, uc = 0.0, score_ind1 = 0.0, score_ind2 = 0.0;
	double nb_qtl = ui.scoreTableWidget->columnCount() - COL_FIRST_QTL;
	int nb_columns = ui.leftListTableWidget4->columnCount();
	int index_list1_selection = ui.selectionListComboBox4->currentIndex(), index_list2_selection = ui.selectionList2ComboBox4->currentIndex();
	int index_list_crosses = ui.crossesListComboBox4->currentIndex(), index_criterion_selected = m_vect_cross_option[4];
	int nb_indiv_list1_selected = (int)m_vect_map_selection[index_list1_selection]->size(), nb_indiv_list2_selected = (int)m_vect_map_selection[index_list2_selection]->size();
	int nb_total_crosses = nb_indiv_list1_selected * nb_indiv_list2_selected, nb_crosses_selected = 0, nb_cross_per_ind = 0, cpt_crosses = 0, cpt_couples = 0;

	QMap<QString, QStringList> & map_crosses_list_i = *(m_vect_map_crosses[index_list_crosses]);
	QMap<QString, int> & map_selected_list1_i = *(m_vect_map_selection[index_list1_selection]);
	QMap<QString, int> & map_selected_list2_i = *(m_vect_map_selection[index_list2_selection]);

	// Notes for the crossing method
	m_vect_notes_crosses[index_list_crosses][0] = "Method: 2 lists constraints "; // Method
	m_vect_notes_crosses[index_list_crosses][1] = QString("List: ") + m_list_selection[index_list1_selection] + QString(" x ") + m_list_selection[index_list2_selection]; // List of selection (coming from)
	m_vect_notes_crosses[index_list_crosses][2] = QString("Criterion: ") + getCriterion(index_criterion_selected); // Criterion


	// All crosses
	if( m_vect_cross_option[0] == 1 ){
		nb_crosses_selected = nb_total_crosses;
		m_vect_notes_crosses[index_list_crosses][0] += "(No.crosses: max - "; // No crosses
	}
	// Constraint on the number of crosses to be made
	else{
		nb_crosses_selected = m_vect_cross_option[2];
		m_vect_notes_crosses[index_list_crosses][0] += QString("No.crosses: %1 - ").arg(nb_crosses_selected, 0, 10); // No crosses
	}

	// All crosses for each individual
	if( m_vect_cross_option[1] == 1 ){
		nb_cross_per_ind = nb_total_crosses; // Overestimated
		m_vect_notes_crosses[index_list_crosses][0] += "Ind contrib: unltd)"; // Individual contribution
	}
	// Each individual can be crossed n times
	else{
		nb_cross_per_ind = m_vect_cross_option[3];
		m_vect_notes_crosses[index_list_crosses][0] += QString("Ind contrib: %1)").arg(nb_cross_per_ind, 0, 10); // Individual contribution
	}

	setLabelCrossPage(ui.leftListResumeFrame4, index_list_crosses);

	if( ! map_crosses_list_i.empty() ){

		int answer = QMessageBox::question(this, tr("Creation of a new list of crosses"), tr("The List %1 is not empty. Do you want to replace and overwrite its contents ?").arg(ui.crossesListComboBox4->currentText()), QMessageBox::Yes | QMessageBox::No);

		// Yes : the current list is removed
		if( answer == QMessageBox::Yes ) {
			map_crosses_list_i.clear();
		}
		else if( answer == QMessageBox::No ){
			return;
		}
	}

	QVector<QString> vect_couples_to_remove; // Vector of couples to remove
	QMap<QString, int> map_ind_selected; // Map of the contribution of each selected individual in couples that will be selected

	QVector<map_iterator> vect_it_couples; // Vector of future sorted couples (by a criterion)
	QVector<map_iterator> vect_it_couples_to_remove; // Vector of couples to remove

	// Copy of the current individuals of the 2 selected lists (not ordered)
	for( QMap<QString, int>::iterator iter_list1 = map_selected_list1_i.begin(); iter_list1 != map_selected_list1_i.end(); ++iter_list1 ){
		map_ind_selected[iter_list1.key()] = 0; // Initialization of the map of individual contribution
	}
	for( QMap<QString, int>::iterator iter_list2 = map_selected_list2_i.begin(); iter_list2 != map_selected_list2_i.end(); ++iter_list2 ){
		map_ind_selected[iter_list2.key()] = 0; // Initialization of the map of individual contribution
	}

	// Creation of the complete list of crosses
	for( QMap<QString, int>::iterator iter_list1 = m_vect_map_selection[index_list1_selection]->begin(); iter_list1 != m_vect_map_selection[index_list1_selection]->end(); ++iter_list1){
		const QString & p1 = iter_list1.key();

		for( QMap<QString, int>::iterator iter_list2 = m_vect_map_selection[index_list2_selection]->begin(); iter_list2 != m_vect_map_selection[index_list2_selection]->end(); ++iter_list2){
			const QString & p2 = iter_list2.key();

			mu = 0.0;
			sigma = 0.0;
			uc = 0.0;

			QString str = p1 + "x" + p2;
			vector<int> vect_nb_geno(4, 0); // No.(+/+) / No.(-/-) / No.(+/-) / No.(?)

			map_crosses_list_i[str].push_back(p1); // p1
			map_crosses_list_i[str].push_back(p2); // p2

			vect_it_couples.push_back(m_vect_map_crosses[index_list_crosses]->find(str)); // ind (couples)

			// Means of molecular scores for couples are computed
			for(int k = (COL_MS - 2); k<nb_columns; k++){
				// All / Weight
				if( k == (COL_MS - 2) || k == (COL_WEIGHT - 2) ){
					d = ( m_map_scores[p1][k+1].toDouble() + m_map_scores[p2][k+1].toDouble() ) / 2.0;
					map_crosses_list_i[str].push_back(QString(" ").setNum(d, 'f', 6));
				}
				// Utility Criterion
				else if( k == (COL_MS - 1 + 1) ){
					map_crosses_list_i[str].push_back("0.000000");
				}
				// No.(+/+) / No.(-/-) / No.(+/-) / No.(?)
				else if( (k >= (COL_NO_FAV - 2)) && (k <=(COL_NO_UNCERTAIN - 2)) ){
					map_crosses_list_i[str].push_back("0.000000");
				}
				// Each QTL
				else if( k >= (COL_FIRST_QTL - 2) ){
					score_ind1 = m_map_scores[p1][k+1].toDouble();
					score_ind2 = m_map_scores[p2][k+1].toDouble();

					// Means of molecular scores for couples are computed at each QTL position
					d = (score_ind1 + score_ind2) / 2.0;
					map_crosses_list_i[str].push_back(QString(" ").setNum(d, 'f', 6));

					// Visualization of genotypes via colors for each QTL
					double score_homo_fav = (m_homo_fav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_fav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_homo_unfav = (m_homo_unfav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_unfav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_hetero = (m_hetero[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_hetero[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;

					// Homozygous favorable
					if( score_homo_fav >= m_vect_cut_off[0] ){
						vect_nb_geno[0]++;
					}
					// Homozygous unfavorable
					else if( score_homo_unfav >= m_vect_cut_off[1] ){
						vect_nb_geno[1]++;
					}
					// Hetero
					else if( score_hetero >= m_vect_cut_off[2] ){
						vect_nb_geno[2]++;
					}
					// ? uncertain genotypes
					else{
						vect_nb_geno[3]++;
					}

					vector<double> val_homo_hetero_p1(3, 0.0), val_homo_hetero_p2(3, 0.0); // Score homo fav / *Score hetero* / Score Homo unfav
					val_homo_hetero_p1[0] = m_homo_fav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p1[1] = m_hetero[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p1[2] = m_homo_unfav[p1][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[0] = m_homo_fav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[1] = m_hetero[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();
					val_homo_hetero_p2[2] = m_homo_unfav[p2][k + 1 - COL_FIRST_QTL + 2][0].toDouble();

					// Utility criterion
					mu = mu + (score_ind1 + score_ind2); // Mean of Molecular Score
					//mu = mu + ((discretization(score_ind1) + discretization(score_ind2)) / 2.0);

					// Computation of the number of heterozygous for the UC method (discretization via the genotype with the higher probability)
					sigma = sigma + (nb_hetero(maxIndex(val_homo_hetero_p1, 3), maxIndex(val_homo_hetero_p2, 3)) * 0.25);
					//sigma = sigma + (nb_hetero(discretization(score_ind1), discretization(score_ind2)) * 0.25);

					val_homo_hetero_p1.clear();
					val_homo_hetero_p2.clear();
				}
			}

			// Utility criterion
			uc = mu + (2 * sqrt(sigma));

			if( uc > (nb_qtl * 2) ) uc = (nb_qtl * 2); // utility criterion borned

			map_crosses_list_i[str][COL_MS - 1] = QString(" ").setNum(uc/2.0, 'f', 6);

			// Update No(+/+), No(-/-), No(+/-), No(?)
			for(int k=(COL_NO_FAV - 2); k<=(COL_NO_UNCERTAIN - 2); k++){
				// Update map
				map_crosses_list_i[str][k - 1] = QString(" ").setNum(vect_nb_geno[k - COL_NO_FAV + 2]);
			}

			cpt_crosses++;
		}
	}

	// Couples are sorted regarding the criterion selected
	crit_index = index_criterion_selected + (COL_MS - 1) - 2; // crit_index is global
	qSort(vect_it_couples.begin(), vect_it_couples.end(), criterionMoreThan); //// Must be sorted only if sum < 2 ?

	// Loop on all the possible couples
	for(int i=0; i < (int)vect_it_couples.size(); i++){

		const QString & p1 = map_crosses_list_i[vect_it_couples[i].key()][0];
		const QString & p2 = map_crosses_list_i[vect_it_couples[i].key()][1];

		// The couple is selected if the individuals can still contribute
		if( (map_ind_selected[p1] < nb_cross_per_ind) && (map_ind_selected[p2] < nb_cross_per_ind) && (cpt_couples < nb_crosses_selected) ){
			map_ind_selected[p1]++;
			map_ind_selected[p2]++;
			cpt_couples++;
		}
		// The couple is removed from the list of selected crosses
		else{
			vect_couples_to_remove.push_back(vect_it_couples[i].key());
		}
	}

	//// Resume of the selected individuals who have not contributed
	//// map_ind_selected[ind] == 0

	// The unselected couples are removed from the list
	for(int i=0; i<(int)vect_couples_to_remove.size(); i++){
		map_crosses_list_i.remove(vect_couples_to_remove[i]);
	}

	// The table of selected couples is displayed
	displayListCrosses(ui.leftListTableWidget4, index_list_crosses);

	// Change the name of the current list in the left table
	ui.leftListComboBox4->setCurrentIndex(index_list_crosses);

	// The current list is in the left and right
	if( ui.rightListComboBox4->currentIndex() == index_list_crosses ){
		displayListCrosses(ui.rightListTableWidget4, index_list_crosses);
	}

	// The current plots are updated
	if( ui.graphLeftListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphLeftQwtPlot4, index_list_crosses);
	}
	if( ui.graphRightListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphRightQwtPlot4, index_list_crosses);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the selected list of crosses is saved in .txt file. The aim
	is to use this list in the field. (Also connect to the field book).
	-----------------------------------------------------------------------------
*/

void MainWindow::saveListCrosses(){

	if( (ui.crossListWidget4->count() == 0) || ((int)m_vect_map_crosses[ui.crossListWidget4->currentRow()]->size() == 0) ){
		QMessageBox::information(this, tr("Save list crosses"), tr("The list of crosses is empty !!"));
		return;
	}

	// The name of the current list is proposed
	QString dir_path = m_file_res + "/list_crosses/" + ui.crossListWidget4->currentItem()->text() + ".txt";
	QString filter = tr("text files (*.txt)");

	QFileDialog fileDialog(this, tr("Export list of crosses: %1").arg(ui.crossListWidget4->currentItem()->text()), dir_path, filter);
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);

	if( fileDialog.exec() == QDialog::Accepted ){
		QString fileName = fileDialog.selectedFiles().value(0), selected_filter = fileDialog.selectedNameFilter(), ext = selected_filter.section('.', -1);

		// The name of the file is empty
		if( fileName.isEmpty() ){
			QMessageBox::information(this, tr("Save list crosses"), tr("Cannot load %1.").arg(fileName));
			return;
		}
		// The file already exists
		else{
			ext.chop(1); // skip parenthesis

			if( fileName.section('.', -1) != ext ){
				fileName+= "." + ext; // add the extention to filname if needed.
			}
			saveTableCrosses(ui.crossListWidget4->currentRow(), fileName);
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: the index of the selected list (crosses).
	@ string fileName: the path of the file where will be saved the list.
	## RETURN:
	## SPECIFICATION: the selected list of crosses is saved in .txt file. The aim
	is to use this list in the field. (Also connect to the field book).
	-----------------------------------------------------------------------------
*/

void MainWindow::saveTableCrosses(int index_list_selected, const QString & fileName){

	QFile file(fileName);
	int nb_columns = ui.leftListTableWidget4->columnCount();

	if ( file.open(QIODevice::WriteOnly | QIODevice::Text) ){
		
		QTextStream list_crosses_file(&file); // write only text stream on file
		// Headers
		for(int i=0; i<ui.leftListTableWidget4->columnCount(); i++){
			if( i == 0){
				list_crosses_file << ui.leftListTableWidget4->horizontalHeaderItem(i)->text();
			}
			else{
				list_crosses_file << "\t" << ui.leftListTableWidget4->horizontalHeaderItem(i)->text();
			}
		}
		list_crosses_file << "\n";

		// Table
		QMap <QString, QStringList> & map_crosses_list_i = *(m_vect_map_crosses[index_list_selected]);
		
		for( QMap <QString, QStringList>::iterator iter_list = map_crosses_list_i.begin(); iter_list != map_crosses_list_i.end(); ++iter_list ){
			const QString & ind = iter_list.key(); // individual of the list
			const QStringList & ind_data = iter_list.value();
			
			//// A tester : list_crosses_file << ind << "\t" << ind_data.join('\t'); au lieu de la boucle for.
			for(int j=0; j<nb_columns; j++){
				if( j == 0 ){
					list_crosses_file << ind;
				}
				else{
					list_crosses_file << "\t" << ind_data[j-1];	
				}
			}
			list_crosses_file << "\n";
		}
		list_crosses_file << "--\n";

		// Resume
		list_crosses_file << m_vect_notes_crosses[index_list_selected][0] << "\n";
		list_crosses_file << m_vect_notes_crosses[index_list_selected][1] << "\n";
		list_crosses_file << m_vect_notes_crosses[index_list_selected][2];
		file.close();
	}
	else {
		QMessageBox::information(this, tr("Save list crosses"), tr("Error: opening the file [%1] in write mode failed.").arg(fileName));
		return;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: add a new empty list of crosses (couples). By default the
	name of the list will be "List 1, ..., List n".
	-----------------------------------------------------------------------------
*/

void MainWindow::addListCrosses(){

	QString listName = QString("List Crosses %1").arg(ui.crossListWidget4->count() + 1, 0, 10);

	// Update of the containers for the new list added
	m_list_crosses.push_back(listName);
	ui.crossListWidget4->addItem(listName);
	m_vect_map_crosses.push_back(new QMap<QString, QStringList>());
	m_vect_notes_crosses.push_back(QStringList()<<""<<""<<"");

	// Update of the combobox lists
	ui.leftListComboBox4->addItem(listName);
	ui.rightListComboBox4->addItem(listName);
	ui.crossesListComboBox4->addItem(listName);
	ui.graphLeftListComboBox4->addItem(listName);
	ui.graphRightListComboBox4->addItem(listName);
	ui.crossListWidget4->setCurrentRow(ui.crossListWidget4->count() - 1);
	ui.leftListComboBox4->setCurrentIndex(ui.crossListWidget4->currentRow());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: remove a selected list of crosses (couples).
	-----------------------------------------------------------------------------
*/

void MainWindow::removeListCrosses(){

	if( ! m_list_crosses.empty() ){

		int answer = QMessageBox::question(this, tr("Deletion of the list of crosses"), tr("Do you really want to delete the list [%1] ?").arg(ui.crossListWidget4->currentItem()->text()), QMessageBox::Yes | QMessageBox::No);

		// Yes : the current list is removed
		if( answer == QMessageBox::Yes ) {

			int currentRow = ui.crossListWidget4->currentRow();

			// If yes the current list is removed
			m_list_crosses.erase(m_list_crosses.begin() + currentRow);
			m_vect_notes_crosses.erase(m_vect_notes_crosses.begin() + currentRow);
			ui.crossListWidget4->takeItem(ui.crossListWidget4->row(ui.crossListWidget4->currentItem()));

			delete m_vect_map_crosses[currentRow]; m_vect_map_crosses[currentRow] = NULL;
			m_vect_map_crosses.erase(m_vect_map_crosses.begin() + currentRow);

			// Last list to remove
			if( ui.leftListComboBox4->count() == 1 ){
				ui.leftListTableWidget4->clearContents();
				ui.leftListTableWidget4->setRowCount(0);
				ui.rightListTableWidget4->clearContents();
				ui.rightListTableWidget4->setRowCount(0);
				ui.graphLeftQwtPlot4->detachItems();
				ui.graphLeftQwtPlot4->replot();
				ui.graphRightQwtPlot4->detachItems();
				ui.graphRightQwtPlot4->replot();
			}

			// Update of the combobox lists
			ui.leftListComboBox4->removeItem(currentRow);
			ui.rightListComboBox4->removeItem(currentRow);
			ui.crossesListComboBox4->removeItem(currentRow);
			ui.graphLeftListComboBox4->removeItem(currentRow);
			ui.graphRightListComboBox4->removeItem(currentRow);

			if( ! m_list_crosses.empty() ){
				ui.crossListWidget4->setCurrentRow(ui.crossListWidget4->count() - 1);
				ui.leftListComboBox4->setCurrentIndex(ui.crossListWidget4->currentRow());
			}
		}
		else if( answer == QMessageBox::No ){
			return;
		}
	}
	else{
		QMessageBox::information(this, tr("Deletion of the list of crosses"), tr("All the lists have already been removed !"));
		return;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: reset the set of lists of crosses (couples). By default:
	2 empty lists.
	-----------------------------------------------------------------------------
*/

void MainWindow::resetListCrosses(){
	int answer = QMessageBox::question(this, tr("Reset of the lists of crosses"), tr("Do you really want to delete all the lists ?"), QMessageBox::Yes | QMessageBox::No);

	resetListCrosses(answer);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int answer: [1] if you want to rest all the lists and [0] otherwise.
	## RETURN:
	## SPECIFICATION: delete all the current set of lists of crosses (couples) and
	rest the set by default to 2 empty lists.
	-----------------------------------------------------------------------------
*/

void MainWindow::resetListCrosses(int answer){

	// Yes : the current list is removed
	if( answer == QMessageBox::Yes ) {

		int nbLists = (int)m_list_crosses.size();

		// If yes all the lists are removed
		for(int i=0; i<nbLists; i++){
			m_list_crosses.erase(m_list_crosses.begin());
			m_vect_notes_crosses.erase(m_vect_notes_crosses.begin());
			delete m_vect_map_crosses.front(); m_vect_map_crosses.front() = NULL;
			m_vect_map_crosses.erase(m_vect_map_crosses.begin());

			// Initialization case
			if( ui.crossListWidget4->count() != 0 ){

				ui.crossListWidget4->takeItem(0);

				// Update of the combobox lists
				ui.leftListComboBox4->removeItem(0);
				ui.rightListComboBox4->removeItem(0);
				ui.crossesListComboBox4->removeItem(0);
				ui.graphLeftListComboBox4->removeItem(0);
				ui.graphRightListComboBox4->removeItem(0);
			}
		}

		// Reset all the tables and graphs
		ui.leftListTableWidget4->clearContents();
		ui.leftListTableWidget4->setRowCount(0);
		ui.rightListTableWidget4->clearContents();
		ui.rightListTableWidget4->setRowCount(0);
		ui.graphLeftQwtPlot4->detachItems();
		ui.graphLeftQwtPlot4->replot();
		ui.graphRightQwtPlot4->detachItems();
		ui.graphRightQwtPlot4->replot();

		// Creation of the 2 empty list by default
		for(int i=0; i<2; i++){
			QString listName = QString("List Crosses %1").arg(i+1, 0, 10);

			// Update of the containers for the new list added
			m_list_crosses.push_back(listName);
			m_vect_notes_crosses.push_back(QStringList()<<""<<""<<"");
			ui.crossListWidget4->addItem(listName);
			m_vect_map_crosses.push_back(new QMap<QString, QStringList>());

			// Update of the combobox lists
			ui.leftListComboBox4->addItem(listName);
			ui.rightListComboBox4->addItem(listName);
			ui.crossesListComboBox4->addItem(listName);
			ui.graphLeftListComboBox4->addItem(listName);
			ui.graphRightListComboBox4->addItem(listName);
		}
		ui.crossListWidget4->setCurrentRow(0);
		ui.leftListComboBox4->setCurrentIndex(0);
		ui.rightListComboBox4->setCurrentIndex(1);
		ui.graphLeftListComboBox4->setCurrentIndex(0);
		ui.graphRightListComboBox4->setCurrentIndex(1);
	}
	else if( answer == QMessageBox::No ){
		return;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: (SLOT)
	Selected individuals are removed to the left list of crosses.
	-----------------------------------------------------------------------------
*/

void MainWindow::deleteIndToLeftListCrosses(){
	deleteIndToListCrosses(ui.leftListTableWidget4, ui.leftListComboBox4->currentIndex());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: (SLOT)
	Selected individuals are removed to the right list of crosses.
	-----------------------------------------------------------------------------
*/

void MainWindow::deleteIndToRightListCrosses(){
	deleteIndToListCrosses(ui.rightListTableWidget4, ui.rightListComboBox4->currentIndex());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QTableWidget *tab: the table of selected individuals
	@ int index_list_selected: the index of the selected list.
	## RETURN:
	## SPECIFICATION:
	Selected individuals are removed to the selected list of crosses and table.
	-----------------------------------------------------------------------------
*/

void MainWindow::deleteIndToListCrosses(QTableWidget *tab, int index_list_selected){

	int topRow, bottomRow;

	// Each individuals selected are added to the list of selection previously chosen
	QList<QTableWidgetSelectionRange> listQTableWidgetSelectionRange = tab->selectedRanges();

	for(int i=0; i<(int)listQTableWidgetSelectionRange.size(); i++){
		topRow = listQTableWidgetSelectionRange[i].topRow();
		bottomRow = listQTableWidgetSelectionRange[i].bottomRow();

		for(int j=topRow; j<=bottomRow; j++){
			// The individual is deleted from the selected list
			m_vect_map_crosses[index_list_selected]->remove((tab->item(j, 0))->text());
		}
	}

	// Update the list of selection if it's the current one shown
	if( ui.leftListComboBox4->currentIndex() == index_list_selected){
		displayListCrosses(ui.leftListTableWidget4, index_list_selected);
	}
	if( ui.rightListComboBox4->currentIndex() == index_list_selected){
		displayListCrosses(ui.rightListTableWidget4, index_list_selected);
	}

	// Update of graphs if the current list was selected
	if( ui.graphLeftListComboBox4->currentIndex() == index_list_selected ){
		plotGraphCrossPage(ui.graphLeftQwtPlot4, index_list_selected);
	}
	if( ui.graphRightListComboBox6->currentIndex() == index_list_selected ){
		plotGraphCrossPage(ui.graphRightQwtPlot4, index_list_selected);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: the index of the selected list (crosses).
	## RETURN:
	## SPECIFICATION: (SLOT)
	The selected list of crosses is displayed in the left table.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayLeftListCrosses(int index_list_selected){
	if( ! m_vect_map_crosses.empty() ){
		displayListCrosses(ui.leftListTableWidget4, index_list_selected);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: the index of the selected list (crosses).
	## RETURN:
	## SPECIFICATION: (SLOT)
	The selected list of crosses is displayed in the right table.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayRightListCrosses(int index_list_selected){
	if( ! m_vect_map_crosses.empty() ){
		displayListCrosses(ui.rightListTableWidget4, index_list_selected);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QTableWidget *tab: the table of selected individuals
	@ int index_list_selected: the index of the selected list (crosses).
	## RETURN:
	## SPECIFICATION: the selected list of crosses is displayed in the selected table.
	-----------------------------------------------------------------------------
*/

void MainWindow::displayListCrosses(QTableWidget *tab, int index_list_selected){
	if( ! m_vect_map_crosses.empty() ){

		tab->setSortingEnabled(false);

		int nb_columns = tab->columnCount(), cpt_ind = 0;
		QMap <QString, QStringList> & map_crosses_list_i = *(m_vect_map_crosses[index_list_selected]);

		tab->setRowCount(0);
		tab->setRowCount((int)map_crosses_list_i.size());

		for( QMap <QString, QStringList>::iterator iter_list = map_crosses_list_i.begin(); iter_list != map_crosses_list_i.end(); ++iter_list ){
			const QString & ind = iter_list.key(); // individual of the list
			const QStringList & ind_data = iter_list.value();
			for(int j=0; j<nb_columns; j++){

				QTableWidgetItem *newItem = new QTableWidgetItem();
				// items are not editable and must allow Drag and drop.
				newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled);
	
				// Ind
				if( j == COL_ID ){
					newItem->setText(ind);
					tab->setItem(cpt_ind, j, newItem);
				}
				// p1 / p2
				else if ( (j == COL_P1) || (j == COL_P2) ){
					newItem->setText(ind_data[j-1]);
					tab->setItem(cpt_ind, j, newItem);
				}
				// MS
				else if( j == (COL_MS - 2) ){
					newItem->setText( QString().setNum(ind_data[j-1].toDouble(), 'f', 4) ); // 4 decimal digits.
					tab->setItem(cpt_ind, j, newItem);
					((QTableWidgetItem *)tab->item(cpt_ind, j))->setTextColor(Qt::red);
				}
				// Weight
				else if( j == (COL_WEIGHT - 2) ){
					newItem->setText( QString().setNum(ind_data[j-1].toDouble(), 'f', 4) ); // 4 decimal digits.
					tab->setItem(cpt_ind, j, newItem);
					((QTableWidgetItem *)tab->item(cpt_ind, j))->setTextColor(Qt::blue);
				}
				// Utility Criterion
				else if( j == (COL_UC - 2) ){
					newItem->setData( 0, QString().setNum( ind_data[j-1].toDouble(), 'f', 4 ).toDouble() );
					tab->setItem(cpt_ind, j, newItem);
					((QTableWidgetItem *)tab->item(cpt_ind, j))->setTextColor(Qt::darkGreen);
				}
				// No.(+/+) / No.(-/-) / No.(+/-) / No.(?)
				else if( (j >= (COL_NO_FAV - 2)) && (j <=(COL_NO_UNCERTAIN - 2)) ){
					newItem->setData( 0, ind_data[j-1].toInt());
					tab->setItem(cpt_ind, j, newItem);
				}
				// QTLs
				else if( j >= (COL_FIRST_QTL - 2) ){
					newItem->setText( QString().setNum(ind_data[j-1].toDouble(), 'f', 4) ); // 4 decimal digits.
					tab->setItem(cpt_ind, j, newItem);

					// Visualization of genotypes via colors for each QTL
					double score_homo_fav = (m_homo_fav[ind_data[COL_P1 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_fav[ind_data[COL_P2 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_homo_unfav = (m_homo_unfav[ind_data[COL_P1 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_homo_unfav[ind_data[COL_P2 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;
					double score_hetero = (m_hetero[ind_data[COL_P1 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble() + m_hetero[ind_data[COL_P2 - 1]][j + 1 - COL_FIRST_QTL + 2][0].toDouble()) / 2.0;

					// Homozygous favorable
					if( score_homo_fav >= m_vect_cut_off[0] ){
						if( m_vect_color[0].value() != ui.findPushButton2->palette().button().color().value() ){
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(QBrush(m_vect_color[0]));
						}
						else{
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(((QTableWidgetItem *)tab->item(cpt_ind, COL_P1))->background());
						}
					}
					// Homozygous unfavorable
					else if( score_homo_unfav >= m_vect_cut_off[1] ){
						if( m_vect_color[1].value() != ui.findPushButton2->palette().button().color().value() ){
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(QBrush(m_vect_color[1]));
						}
						else{
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(((QTableWidgetItem *)tab->item(cpt_ind, COL_P1))->background());
						}
					}
					// Hetero
					else if( score_hetero >= m_vect_cut_off[2] ){
						if( m_vect_color[2].value() != ui.findPushButton2->palette().button().color().value() ){
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(QBrush(m_vect_color[2]));
						}
						else{
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(((QTableWidgetItem *)tab->item(cpt_ind, COL_P1))->background());
						}
					}
					// ? uncertain genotypes
					else{
						if( m_vect_color[3].value() != ui.findPushButton2->palette().button().color().value() ){
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(QBrush(m_vect_color[3]));
						}
						else{
							((QTableWidgetItem *)tab->item(cpt_ind, j))->setBackground(((QTableWidgetItem *)tab->item(cpt_ind, COL_P1))->background());
						}
					}
				}
			}
			cpt_ind++;
		}

		// Notes for the crossing method is updated
		if( tab == ui.leftListTableWidget4 ){
			setLabelCrossPage(ui.leftListResumeFrame4, index_list_selected);
		}
		else if( tab == ui.rightListTableWidget4 ){
			setLabelCrossPage(ui.rightListResumeFrame4, index_list_selected);
		}

		tab->setSortingEnabled(true);
		tab->setAlternatingRowColors(true);
		tab->resizeColumnsToContents();
		tab->resizeRowsToContents();
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_plot: the index of the selected graph (plot or histogram).
	## RETURN:
	## SPECIFICATION: plot the corresponding graph (plot or histogram) of the
	cross page after user selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotLeftGraphCrossPage(int index_plot){
	// Plot
	if( index_plot == 0 ){
		ui.graphLeftQtlComboBox4->setEnabled(false);
		plotGraphCrossPage(ui.graphLeftQwtPlot4, ui.graphLeftListComboBox4->currentIndex());
	}
	// Histo
	else if( index_plot == 1 ){
		ui.graphLeftQtlComboBox4->setEnabled(true);
		plotHistoCrossPage(ui.graphLeftQwtPlot4, ui.graphLeftListComboBox4->currentIndex(), ui.graphLeftQtlComboBox4->currentIndex());
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_plot: the index of the selected graph (plot or histogram).
	## RETURN:
	## SPECIFICATION: plot the corresponding graph (plot or histogram) of the
	cross page after user selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotRightGraphCrossPage(int index_plot){
	// Plot
	if( index_plot == 0 ){
		ui.graphRightQtlComboBox4->setEnabled(false);
		plotGraphCrossPage(ui.graphRightQwtPlot4, ui.graphRightListComboBox4->currentIndex());
	}
	// Histo
	else if( index_plot == 1 ){
		ui.graphRightQtlComboBox4->setEnabled(true);
		plotHistoCrossPage(ui.graphRightQwtPlot4, ui.graphRightListComboBox4->currentIndex(), ui.graphRightQtlComboBox4->currentIndex());
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_qtl: the index of the QTL selected that we want to plot.
	## RETURN:
	## SPECIFICATION: plot the histogram of the cross page after user QTL selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotHistoLeftGraphCrossPageQtl(int index_qtl){
	if( (ui.graphLeftPlotComboBox4->currentIndex() == 1) && (! m_vect_map_crosses.empty()) ){
		plotHistoCrossPage(ui.graphLeftQwtPlot4, ui.graphLeftListComboBox4->currentIndex(), index_qtl);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_qtl: the index of the QTL selected that we want to plot.
	## RETURN:
	## SPECIFICATION: plot the histogram of the cross page after user QTL selection.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotHistoRightGraphCrossPageQtl(int index_qtl){
	if( (ui.graphRightPlotComboBox4->currentIndex() == 1) && (! m_vect_map_crosses.empty()) ){
		plotHistoCrossPage(ui.graphRightQwtPlot4, ui.graphRightListComboBox4->currentIndex(), index_qtl);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: the index of the selected list (crosses).
	## RETURN:
	## SPECIFICATION: the left graph for the current QTL is plotted regarding
	the list selected.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotLeftGraphCrossPageList(int index_list_selected){
	if( ! m_vect_map_crosses.empty() ){
		// Plot
		if( ui.graphLeftPlotComboBox4->currentIndex() == 0 ){
			plotGraphCrossPage(ui.graphLeftQwtPlot4, index_list_selected);
		}
		// Histogram
		else if( ui.graphLeftPlotComboBox4->currentIndex() == 1 ){
			plotHistoCrossPage(ui.graphLeftQwtPlot4, index_list_selected, ui.graphLeftQtlComboBox4->currentIndex());
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index_list_selected: the index of the selected list (crosses).
	## RETURN:
	## SPECIFICATION: the left graph for the current QTL is plotted regarding
	the list selected.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotRightGraphCrossPageList(int index_list_selected){
	if( ! m_vect_map_crosses.empty() ){
		// Plot
		if( ui.graphRightPlotComboBox4->currentIndex() == 0 ){
			plotGraphCrossPage(ui.graphRightQwtPlot4, index_list_selected);
		}
		// Histogram
		else if( ui.graphRightPlotComboBox4->currentIndex() == 1 ){
			plotHistoCrossPage(ui.graphRightQwtPlot4, index_list_selected, ui.graphRightQtlComboBox4->currentIndex());
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QwtPlot *graph: the graph where the results will be plotted.
	@ int index_list_selected: the index of the selected list (crosses).
	@ int index_qtl: the index of the selected QTL.
	## RETURN:
	## SPECIFICATION: for the selected list, the distribution of individuals (couples)
	regarding the molecular score (histogram) is plotted into graphs (selection).
	-----------------------------------------------------------------------------
*/

void MainWindow::plotHistoCrossPage(QwtPlot *graph, int index_list_selected, int index_qtl){

	if( ! m_vect_map_crosses.empty() ){
		int numValues = 20; //(int)m_map_gene_quantiles["All"][index_qtl].size(); // [0.0 - 1.0] step = 0.05 (20)
		double d, pos = 0.0, width = 0.05, mean = 0.0, var = 0.0;
		vector<int> vect_quantiles(numValues, 0);

		QMap<QString, QStringList> & map_selected_list_i = *(m_vect_map_crosses[index_list_selected]);

		//// je crois qu'on peut calculer mean et var avec une seule boucle (formule altenative equivalente (voir ds coda)).
		// Loop on individuals
		for( QMap<QString, QStringList>::iterator iter_list = map_selected_list_i.begin(); iter_list != map_selected_list_i.end(); ++iter_list ){
			const QStringList & ind_data = iter_list.value();

			// All / Weight
			if( index_qtl < 2 ){
				d = ind_data[index_qtl + (COL_MS - 3)].toDouble();
			}
			else{
				d = ind_data[index_qtl - 2 + (COL_FIRST_QTL - 3)].toDouble();
			}
			vect_quantiles[whichQuantile(d)] += 1;
			mean += d; // Mean score value
		}

		if( ((int)map_selected_list_i.size()) != 0 ){
			mean = mean / (double)map_selected_list_i.size();
		}
		else{
			mean = 0.0;
		}

		// Variance
		for( QMap<QString, QStringList>::iterator iter_list = map_selected_list_i.begin(); iter_list != map_selected_list_i.end(); ++iter_list ){
			const QStringList & ind_data = iter_list.value();
			// All / Weight
			if( index_qtl < 2 ){
				d = ind_data[index_qtl + (COL_MS - 3)].toDouble();
			}
			else{
				d = ind_data[index_qtl - 2 + (COL_FIRST_QTL - 3)].toDouble();
			}
			var += pow(d - mean, 2.0);
		}

		if( ((int)map_selected_list_i.size()) != 0 ){
			var = var / (double)map_selected_list_i.size();
		}
		else{
			var = 0.0;
		}

		if( graph == ui.graphLeftQwtPlot4 ){
			setLabelCrossPage(ui.graphLeftMeanLabel4, mean, var);
		}
		else if( graph == ui.graphRightQwtPlot4 ){
			setLabelCrossPage(ui.graphRightMeanLabel4, mean, var);
		}

		graph->detachItems();

		QwtPlotGrid *grid = new QwtPlotGrid();
		grid->enableXMin(true);
		grid->enableX(false);
		grid->enableYMin(true);
		grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
		grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
		grid->attach(graph);

		HistogramItem *histogram = new HistogramItem();
		histogram->setColor(Qt::darkCyan);

		// Vector of intervals for all quantiles
		QwtArray<QwtDoubleInterval> intervals(numValues);
		QwtArray<double> values(numValues);

		for(int i=0; i<numValues; i++){
			intervals[i] = QwtDoubleInterval(pos, pos + double(width));
			values[i] = vect_quantiles[i];
			pos += width;
		}

		histogram->setData(QwtIntervalData(intervals, values));
		histogram->attach(graph);

		QwtScaleDraw* scaleDraw = new QwtScaleDraw();
		scaleDraw->setTickLength(QwtScaleDiv::MediumTick, scaleDraw->majTickLength()/2);
		graph->setAxisScaleDraw(QwtPlot::xBottom, scaleDraw);
		graph->setAxisMaxMinor(QwtPlot::xBottom, 2);
		graph->setAxisScale(QwtPlot::yLeft, 0.0, (double)map_selected_list_i.size());
	}
	else{
		graph->detachItems();
		graph->setAxisScale(QwtPlot::yLeft, 0.0, 0.0);
		graph->setAxisScaleDraw(QwtPlot::xBottom, new QwtScaleDraw());
	}

	graph->setAxisScaleDraw(QwtPlot::yLeft, new QwtScaleDraw());
	graph->setAxisScale(QwtPlot::xBottom, 0.0, 1.0, 0.1);
	graph->setAxisTitle(QwtPlot::yLeft, "Number of Couples");
	graph->setAxisTitle(QwtPlot::xBottom, "Molecular Score (MS)");
	graph->setCanvasBackground(QColor(Qt::white));
	graph->replot();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QwtPlot *graph: the graph where the couples will be plotted.
	@ QwtPlotCurve *crossCurve: the point in the graph (1 cross = 1 couple).
	@ int index_list_selected: the index of the selected list (crosses).
	## RETURN:
	## SPECIFICATION: for the selected list, each couple is plotted into a
	selected graph (left or right). Axes of the graph: Each individual is sorted
	regarding is molecular score.
	-----------------------------------------------------------------------------
*/

void MainWindow::plotGraphCrossPage(QwtPlot *graph, int index_list_selected){

	if( ! m_vect_map_crosses.empty() ){
		int index_criterion_selected = 0;
		double mean = 0.0, var = 0.0;

		QMap <QString, QStringList> & map_crosses_list_i = *(m_vect_map_crosses[index_list_selected]);

		QVector<double> x(0), y(0); // Axis for crosses between individuals (0 = ind with the best score)
		QList<map_iterator> vect_ind_xy; // Vector of future sorted individuals (by molecular score)
		QMap<QString, int> map_ind_xy_sorted; // Hash map of sorted individuals (by MS); key: indiv; value: the index

		// Copy of the current lists of individuals (not ordered)
		for( QMap <QString, QStringList>::iterator iter_list = map_crosses_list_i.begin(); iter_list != map_crosses_list_i.end(); ++iter_list ){
			const QStringList & ind_data = iter_list.value();
			const QString & p1 = ind_data[0]; // QStringList::operator[] returns a reference.
			const QString & p2 = ind_data[1];

			// Mean
			mean += ind_data[index_criterion_selected + 2].toDouble();

			// The individual is added to the selected list if it is not already present on it
			if( map_ind_xy_sorted.find(p1) == map_ind_xy_sorted.end() ){
				vect_ind_xy.push_back(m_map_scores.find(p1));
				map_ind_xy_sorted[p1] = 0;
			}
			if( map_ind_xy_sorted.find(p2) == map_ind_xy_sorted.end() ){
				vect_ind_xy.push_back(m_map_scores.find(p2));
				map_ind_xy_sorted[p2] = 0;
			}
		}
		if( ((int)map_crosses_list_i.size()) != 0 ){
			mean = mean / (double)map_crosses_list_i.size();
		}
		else{
			mean = 0.0;
		}

		// Individuals are sorted regarding their criterion (molecular score...)
		crit_index = index_criterion_selected + COL_MS - 1; // crit_index = index is global.
		qSort(vect_ind_xy.begin(), vect_ind_xy.end(), criterionMoreThan);

		QVector<QString> x_legend(vect_ind_xy.size()), y_legend(vect_ind_xy.size());

		// Individuals are sorted and ranked
		for(int i=0; i<(int)vect_ind_xy.size(); i++){
			map_ind_xy_sorted[vect_ind_xy[i].key()] = i;
			x_legend[i] = vect_ind_xy[i].key();
			y_legend[i] = vect_ind_xy[i].key();
		}

		// Creation of each cross for the plot
		for( QMap <QString, QStringList>::iterator iter_list = map_crosses_list_i.begin(); iter_list != map_crosses_list_i.end(); ++iter_list ){
			const QStringList & ind_data = iter_list.value();
			const QString & p1 = ind_data[0];
			const QString & p2 = ind_data[1];

			// Variance
			var += pow(ind_data[index_criterion_selected + 2].toDouble() - mean, 2.0);

			// if x greater than y => swap the values.
			int xi = map_ind_xy_sorted[p1];
			int yi = map_ind_xy_sorted[p2];
			if (xi>yi) qSwap(xi, yi);
			x.append((double)xi);
			y.append((double)yi);
		}

		if( ((int)map_crosses_list_i.size()) != 0 ){
			var = var / (double)map_crosses_list_i.size();
		}
		else{
			var = 0.0;
		}

		// Set mean and variance in the interface (bottom label)
		if( graph == ui.graphLeftQwtPlot4 ){
			setLabelCrossPage(ui.graphLeftMeanLabel4, mean, var);
		}
		else if( graph == ui.graphRightQwtPlot4 ){
			setLabelCrossPage(ui.graphRightMeanLabel4, mean, var);
		}

		graph->detachItems();
		graph->setCanvasBackground(Qt::white);

		QwtPlotCurve *crossCurve = new QwtPlotCurve();
		crossCurve->attach(graph);

		QwtSymbol sym;
		sym.setStyle(QwtSymbol::Cross);
		sym.setPen(QPen(Qt::darkCyan, 2));
		sym.setSize(7);
		crossCurve->setStyle(QwtPlotCurve::NoCurve);
		crossCurve->setSymbol(sym);

		if( vect_ind_xy.size() > 0 ){
			// set crossCurve data using rescaled x and y values.
			crossCurve->setData(x.data(),y.data(), x.size());

			// Define custom axes scales using x and y axis labels vector x_legend and y_legend.
			graph->setAxisScaleDraw( QwtPlot::xBottom, new CustomScaleDraw(x_legend) );
			graph->setAxisScaleDraw( QwtPlot::yLeft, new CustomScaleDraw(y_legend) );

			graph->setAxisScale(QwtPlot::yLeft, 0.0, y_legend.size() - 1);
			graph->setAxisScale(QwtPlot::xBottom, 0.0, x_legend.size() - 1);

			// labels rotation.
			graph->setAxisLabelRotation(QwtPlot::xBottom, -45);
			graph->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
			graph->setAxisMaxMinor(QwtPlot::xBottom, 0);
			graph->setAxisMaxMajor(QwtPlot::xBottom, x_legend.size());
			graph->setAxisMaxMinor(QwtPlot::yLeft, 0);
			graph->setAxisMaxMajor(QwtPlot::yLeft, y_legend.size());
		}
	}
	else{
		graph->detachItems();
		graph->setAxisScale(QwtPlot::yLeft, 0.0, 0.0);
		graph->setAxisScale(QwtPlot::xBottom, 0.0, 0.0);
	}

	graph->setAxisTitle(QwtPlot::xBottom, QString("Individuals (+ MS -)"));
	graph->setAxisTitle(QwtPlot::yLeft,QString("Individuals (+ MS -)"));
	graph->replot();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the left graph (scatter plot) of the crossing page is saved
	in a PNG format.
	-----------------------------------------------------------------------------
*/

void MainWindow::saveGraphLeftCrossesPage(){
	saveGraph(ui.graphLeftQwtPlot4);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the right graph (scatter plot) of the crossing page is saved
	in a PNG format.
	-----------------------------------------------------------------------------
*/

void MainWindow::saveGraphRightCrossesPage(){
	saveGraph(ui.graphRightQwtPlot4);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: a dialog is shown to select an output directory. We reload
	this previous analysis (run) done by OptiMAS. All the settings (tables,
	graphs, lists) are displayed.
	-----------------------------------------------------------------------------
*/

void MainWindow::reloadData(){
	// Show a dialog window to load the appropriate directory (previous analysis)
	QString directory = QFileDialog::getExistingDirectory(this, tr("Select the directory of results to reload"), m_lastOutput, QFileDialog::ShowDirsOnly);

	// ATTENTION: checker tout le repertoire avant de faire qq chose

	// Check if the directory is not empty
	if( !directory.isEmpty() ){

		// The result of QFileDialog differs on different OS
		if( (directory.lastIndexOf("/") + 1) != (int)directory.size() ){
			directory = directory + "/";
		}

		m_file_res = m_lastOutput = directory;

		reloadDirectory();
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: true if user_settings.conf is found and has valid data, false otherwise.
	## SPECIFICATION: reload user's QTLs weights and cutoff color scheme settings
	from a previous backup.
	-----------------------------------------------------------------------------
*/

bool MainWindow::reloadUserSettings(){

	bool ret = false;
	QString path = m_file_res+"user_settings.conf";

	if(!QFile::exists(path)) return ret;

	std::vector<double>::const_iterator it;

	QFile file(path);

	if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ){

		QTextStream in(&file); // read only text stream on file
		QString line;
		int size;

		while (!in.atEnd()) {
			line = in.readLine();
			QStringList SplitedLine = line.split('\t',QString::SkipEmptyParts); // field separator : tab

			if ( (size=SplitedLine.size()) > 0){

				if(SplitedLine.startsWith("#QTL WEIGHT:")){
					m_vect_qtl_weight.clear();
					ret = true;
					for (int i=1; i<size; i++){
						m_vect_qtl_weight.push_back(SplitedLine[i].toDouble());
					}
				}
				else if (SplitedLine.startsWith("#CUTOFF=>COLOR:")) {
					m_vect_color.clear();
					m_vect_cut_off.clear();
					QStringList cutoffColor;
					ret = true;
					for (int i=1; i<size-1; i++){
						// split each pair of cutoff:color
						cutoffColor = SplitedLine[i].split(':');
						m_vect_cut_off.push_back(cutoffColor[0].toDouble());
						m_vect_color.push_back(QColor(cutoffColor[1]));
					}
					// uncertain genotype
					// QString::right(7) returns a sub-string built from the 7 last characters of the initial string ("REST:#ff0000" => #ff0000).
					m_vect_color.push_back(QColor(SplitedLine[size-1].right(7)));
				}
			}
		}
	}
	else {
		QMessageBox::warning(this, tr("Save user settings"), tr("Error: opening the file [%1] in read mode failed.").arg(path));
		return ret;
	}

	return ret;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: reload a previous analysis (run) done by OptiMAS. All the
	settings (tables, graphs, lists) are displayed.
	-----------------------------------------------------------------------------
*/

void MainWindow::reloadDirectory(){

	char *run_time = (char *)malloc(256*sizeof(char));

	m_date = QDateTime::currentDateTime();
	ui.bottomTextBrowser->setText("Data are loading...");

	clearAll(); // Clear and reset everything

	// Display and plot all the tables and graphs of the score page (m_map_score filled)
	displayAllPages();

	getFavAlleles();

	// Fetching number of qtl
	int n_qtl = ui.scoreTableWidget->columnCount() - COL_FIRST_QTL;

	for (int i_qtl=1; i_qtl<=n_qtl; i_qtl++){
		parseHomoHeteroFile(i_qtl);
	}

	// Create, display and plot all the lists, tables and graphs of the selection page
	reloadListsSelection();

	// Create, display and plot all the lists, tables and graphs of the crosses page
	reloadListsCrosses();

	// reload user's QTLs weights and cutoff color scheme settings if needed.
	if(reloadUserSettings()){
		// Apply this settings.
		runComputeWeight();
		applyVisualization();
	}

	// Step page updated
	ui.stepsListWidget->setCurrentRow(0);
	ui.tabWidget2->setCurrentIndex(0);

	int time = m_date.secsTo(QDateTime::currentDateTime()), hour, min, sec;
	hour = time / 3600;
	time = time % 3600;
	min = time / 60;
	time = time % 60;
	sec = time;
	sprintf(run_time, "Data loaded in %dh%dm%ds", hour, min, sec);
	ui.bottomTextBrowser->setText(run_time);
	free(run_time);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: reload the lists of selection saved in a previous analysis
	(run) done by OptiMAS. All the settings (tables, graphs, lists) are displayed.
	-----------------------------------------------------------------------------
*/

void MainWindow::reloadListsSelection(){

	QString path_dir = m_file_res + "list_selection";
	QDir myDir(path_dir);
	QStringList list_files = myDir.entryList(QDir::Files);
	int cpt_files = 0;

	// No list to reload: the lists of selection (default) are added (2 empty lists)
	if( list_files.size() == 0 ){
		resetListSelection(QMessageBox::Yes);
	}
	// The previous saved lists are reloaded
	else{
		// Loop on the lists of selection in the results folder
		for(QStringList::iterator it = list_files.begin(); it != list_files.end(); ++it) {
			QString current_file = *it, file_name = current_file;

			// The current list of selection is added
			addListSelection(file_name.replace(QString(".txt"), QString("")));

			try{
				// Individuals from the file are added to the current list
				addIndFromListSelection(path_dir + "/" + current_file, cpt_files);
			}
			catch(const QString& name_file){
				showCustomErrorMessage("Error: opening the file [" + name_file + "] !!");
				return;
			}
			cpt_files++;
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QString listName: the name of the list of selection.
	## RETURN:
	## SPECIFICATION: a new list (named) of selection is added to the list.
	-----------------------------------------------------------------------------
*/

void MainWindow::addListSelection(const QString & listName){

	// Update of the containers for the new list added
	m_list_selection.push_back(listName);
	ui.selectionListWidget6->addItem(listName);
	m_vect_map_selection.push_back(new QMap<QString, int>());

	// Update of the combobox lists
	ui.leftListComboBox6->addItem(listName);
	ui.rightListComboBox6->addItem(listName);
	ui.truncListComboBox6->addItem(listName);
	ui.compListComboBox6->addItem(listName);
	ui.selectionListComboBox4->addItem(listName);
	ui.selectionList2ComboBox4->addItem(listName);
	ui.graphLeftListComboBox6->addItem(listName);
	ui.graphRightListComboBox6->addItem(listName);
	ui.pedigreeListComboBox6->addItem(listName);
	ui.selectionListWidget6->setCurrentRow(ui.selectionListWidget6->count() - 1);
	ui.leftListComboBox6->setCurrentIndex(ui.selectionListWidget6->currentRow());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ string name_file: path of the current list of selection.
	@ int index_list_selected: index of the selected list of selection.
	## RETURN:
	## SPECIFICATION: add the lists of selection saved in a previous analysis
	(run) done by OptiMAS. All the settings (tables, graphs, lists) are displayed.
	-----------------------------------------------------------------------------
*/

void MainWindow::addIndFromListSelection(const QString & name_file, int index_list_selected){

	QFile tabFile(name_file); // file descriptor

	if ( QFile::exists(name_file) && tabFile.open(QIODevice::ReadOnly | QIODevice::Text) ){

		QMap <QString, int> & selectedList = *(m_vect_map_selection[index_list_selected]);
		QTextStream in(&tabFile); // read only text stream on file

		int cpt_lines = 0;

		while (!in.atEnd()) {
			QString line = in.readLine();
			QStringList SplitedLine = line.split('\t',QString::SkipEmptyParts); // field separotor : tab

			const QString & ind = SplitedLine[0];

			// 1st line: header skipped
			if( cpt_lines > 0 ){
				// The individual is added to the selected list if it is not already present on it and if it exists on the table of scores
				if( (selectedList.find(ind) == selectedList.end()) && (m_map_scores.find(ind) != m_map_scores.end()) ){
					selectedList[ind] = index_list_selected;
					////m_vect_map_selection[index_list_selected]->insert(ind,index_list_selected);
				}
			}
			cpt_lines++;
		}
	}
	else throw name_file;

	tabFile.close();

	// The list of selection is updated if it's the current one shown
	if( ui.leftListComboBox6->currentIndex() == index_list_selected){
		displayListSelection(ui.leftListTableWidget6, index_list_selected);
	}
	if( ui.rightListComboBox6->currentIndex() == index_list_selected){
		displayListSelection(ui.rightListTableWidget6, index_list_selected);
	}

	// The combobox (list of selection) is updated in the crosses page
	if( ui.selectionListComboBox4->currentIndex() == index_list_selected){
		maxCrossContribChanged(index_list_selected);
	}

	// Graphs are updated if the current list was selected
	if( ui.graphLeftListComboBox6->currentIndex() == index_list_selected ){
		plotGraphSelectionPage(ui.graphLeftQwtPlot6, ui.graphLeftListComboBox6->currentIndex(), ui.graphLeftQtlComboBox6->currentIndex());
	}
	if( ui.graphRightListComboBox6->currentIndex() == index_list_selected ){
		plotGraphSelectionPage(ui.graphRightQwtPlot6, ui.graphRightListComboBox6->currentIndex(), ui.graphRightQtlComboBox6->currentIndex());
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: reload the lists of crosses saved in a previous analysis
	(run) done by OptiMAS. All the settings (tables, graphs, lists) are displayed.
	-----------------------------------------------------------------------------
*/

void MainWindow::reloadListsCrosses(){

	QString path_dir = m_file_res + "list_crosses";
	QDir myDir(path_dir);
	QStringList list_files = myDir.entryList(QDir::Files);
	int cpt_files = 0;

	// No list to reload: the lists of crosses (default) are added (2 empty lists)
	if( list_files.size() == 0 ){
		resetListCrosses(QMessageBox::Yes);
	}
	// The previous saved lists are reloaded
	else{
		// Loop on the lists of selection in the results folder
		for(QStringList::iterator it = list_files.begin(); it != list_files.end(); ++it) {
			QString current_file = *it, file_name = current_file;

			// The current list of selection is added
			addListCrosses(file_name.replace(QString(".txt"), QString("")));

			try{
				// Individuals from the file are added to the current list
				addIndFromListCrosses(path_dir + "/" + current_file, cpt_files);
			}
			catch(const QString& name_file){
				showCustomErrorMessage("Error: opening the file [" + name_file + "] !!");
				return;
			}
			cpt_files++;
		}
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QString listName: the name of the list to add.
	## RETURN:
	## SPECIFICATION: add a named list of crosses (couples).
	-----------------------------------------------------------------------------
*/

void MainWindow::addListCrosses(const QString & listName){

	// Update of the containers for the new list added
	m_list_crosses.push_back(listName);
	ui.crossListWidget4->addItem(listName);
	m_vect_map_crosses.push_back(new QMap<QString, QStringList>());
	m_vect_notes_crosses.push_back(QStringList()<<""<<""<<"");

	// Update of the combobox lists
	ui.leftListComboBox4->addItem(listName);
	ui.rightListComboBox4->addItem(listName);
	ui.crossesListComboBox4->addItem(listName);
	ui.graphLeftListComboBox4->addItem(listName);
	ui.graphRightListComboBox4->addItem(listName);
	ui.crossListWidget4->setCurrentRow(ui.crossListWidget4->count() - 1);
	ui.leftListComboBox4->setCurrentIndex(ui.crossListWidget4->currentRow());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ string name_file: path of the current list of crosses.
	@ int index_list_selected: index of the selected list of crosses.
	## RETURN:
	## SPECIFICATION: add the lists of crosses (couples) saved in a previous analysis
	(run) done by OptiMAS. All the settings (tables, graphs, lists) are displayed.
	-----------------------------------------------------------------------------
*/

void MainWindow::addIndFromListCrosses(const QString & name_file, int index_list_crosses){

	QFile tabFile(name_file); // file descriptor

	// The crosses list is opened for reading
	if ( QFile::exists(name_file) && tabFile.open(QIODevice::ReadOnly | QIODevice::Text) ){
		int cpt_lines = 0, nb_col = 0;
		
		QMap <QString, QStringList> & map_crosses_list_i = *(m_vect_map_crosses[index_list_crosses]);
		
		QTextStream in(&tabFile); // read only text stream on file

		// Loop on the file (line by line)
		while (!in.atEnd()) {
			QString line = in.readLine();
			QStringList SplitedLine = line.split('\t',QString::SkipEmptyParts); // field separotor : tab

			// ATTENTION: TEST IF THE LIST IS OF CROSSES CORRESPOND TO THE TABLE OF SCORES (TO DO)
			if( cpt_lines == 0 ){
				nb_col = (int)SplitedLine.size();
			}
			// 1st line: header skipped
			else{
				if (nb_col == (int)SplitedLine.size()) {
					for(int i=1; i<nb_col; i++){
						map_crosses_list_i[SplitedLine[0]].push_back(SplitedLine[i]); // p1
						////m_vect_map_crosses[index_list_crosses]->operator[](SplitedLine[0]).push_back(SplitedLine[i]);
					}
				}
				else if( (!SplitedLine.isEmpty()) && (int)SplitedLine[0].compare("--") != 0){ // we have reached the ending notes
					if ( SplitedLine[0].startsWith("Method:") ) { m_vect_notes_crosses[index_list_crosses][0] = line; }
					else if( SplitedLine[0].startsWith("List:") ) { m_vect_notes_crosses[index_list_crosses][1] = line; }
					else if( SplitedLine[0].startsWith("Criterion:") ) { m_vect_notes_crosses[index_list_crosses][2] = line; }
				}
			}
			cpt_lines++;
		}
	}
	else throw name_file;

	tabFile.close();

	// The current list is updated in the left and right table
	if( ui.leftListComboBox4->currentIndex() == index_list_crosses ){
		displayListCrosses(ui.leftListTableWidget4, index_list_crosses);
	}
	if( ui.rightListComboBox4->currentIndex() == index_list_crosses ){
		displayListCrosses(ui.rightListTableWidget4, index_list_crosses);
	}

	// The current plots are updated
	if( ui.graphLeftListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphLeftQwtPlot4, index_list_crosses);
	}
	if( ui.graphRightListComboBox4->currentIndex() == index_list_crosses ){
		plotGraphCrossPage(ui.graphRightQwtPlot4, index_list_crosses);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the multiparental example is displayed.
	-----------------------------------------------------------------------------
*/

void MainWindow::reloadBiparentalData(){

	QString biparental_output = QDir::homePath() + QString::fromUtf8("/OptiMAS/output/moreau/");
	m_file_res = biparental_output;

	reloadDirectory();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the multiparental example is displayed.
	-----------------------------------------------------------------------------
*/

void MainWindow::reloadMultiparentalData(){

	QString multiparental_output = QDir::homePath() + QString::fromUtf8("/OptiMAS/output/blanc/");
	m_file_res = multiparental_output;

	reloadDirectory();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: save the user's qtl weights and cut-off settings,
	in respective directories (list_selection/ and list_crosses/).
	-----------------------------------------------------------------------------
*/

void MainWindow::saveUserSettings(const QString & path){

	std::vector<double>::const_iterator it;
	QFile file(path);

	if ( file.open(QIODevice::WriteOnly | QIODevice::Text) ){

		QTextStream outStream(&file); // write only text stream on file

		// writing QTLs weights.
		outStream << "#QTL WEIGHT:";
		for(it=m_vect_qtl_weight.begin(); it!=m_vect_qtl_weight.end(); it++){
			outStream << "\t" << *it;
		}
		outStream << "\n";

		//writing cutoffs color scheme
		size_t i;
		outStream << "#CUTOFF=>COLOR:";
		for(i=0; i<m_vect_cut_off.size(); i++){
			// ex: 0.75=>#ff0000
			outStream << "\t" << m_vect_cut_off[i] << ":" << m_vect_color[i].name();
		}
		// the last field concerns uncertain genotypes.
		outStream << "\tREST:" << m_vect_color[i].name() << "\n";
		file.close();
	}
	else {
		QMessageBox::warning(this, tr("Save user settings"), tr("Error: opening the file [%1] in write mode failed.").arg(path));
		return;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: save all list selections and all list crosses as text files,
	in respective directories (list_selection/ and list_crosses/).
	-----------------------------------------------------------------------------
*/

void MainWindow::saveAllLists(){
	
	QFileDialog DirBrowserDialog(this, tr("Select the directory where to save the data."), m_file_res);
	DirBrowserDialog.setFileMode(QFileDialog::DirectoryOnly);

	QGridLayout* gridLayout = (QGridLayout*)DirBrowserDialog.layout();

	// display the name of the directory in the filename edit because the user may panic if not !!! mouhahaha !!! pfff !
	for(int i=0; i<gridLayout->count(); i++){
		QLineEdit* fileNameLineEdit = NULL;
		if(gridLayout->itemAt(i)->widget()!=NULL && gridLayout->itemAt(i)->widget()->objectName() == "fileNameEdit"){
			fileNameLineEdit = (QLineEdit*)(gridLayout->itemAt(i)->widget());
			fileNameLineEdit->setText(m_file_res);
		}
	}
	//QString dir_path = QFileDialog::getExistingDirectory(this, tr("Select the directory where to save the data."), m_file_res, QFileDialog::ShowDirsOnly);
	// open a browser dialog window and return the user selected directory.
	if (DirBrowserDialog.exec() == QDialog::Accepted){

		QString dir_path = DirBrowserDialog.selectedFiles().value(0);
		if (dir_path.isEmpty()) return; // operation canceled by the user.

		QString crosses_dir_path = QDir::toNativeSeparators(dir_path + "/list_crosses/");
		QString selection_dir_path = QDir::toNativeSeparators(dir_path + "/list_selection/");

		QDir dir(dir_path);

		if( !dir.exists("list_crosses") && !dir.mkdir("list_crosses") ){ // the directory doesn't exist and could not be created.
			QMessageBox::critical(this, "Error", "Error in creating the folder " + crosses_dir_path + " !!");
			return;
		}
		if( !dir.exists("list_selection") && !dir.mkdir("list_selection") ){  // the directory doesn't exist and could not be created.
			QMessageBox::critical(this, "Error", "Error in creating the folder " + selection_dir_path + " !!");
			return;
		}

		// save user's QTLs weights and cutoff color scheme settings.
		saveUserSettings( QDir::toNativeSeparators(dir_path+"/user_settings.conf") );

		int nselections = m_vect_map_selection.size();	// number of list selection.
		int ncrosses = m_vect_map_crosses.size();		// number of list crosses.

		bool overwrite=false;

		// see if some files already exist and propose to overwrite them.
		for(int i=0; i<nselections; ++i){
			QString filepath(selection_dir_path + ui.selectionListWidget6->item(i)->text() + ".txt");
			if(overwrite == false && QFile::exists(filepath)){
				if(QMessageBox::No == QMessageBox::question(this, QString("Warning"), QString("some files already exist !\nDo you want to overwrite them ?"), QMessageBox::No|QMessageBox::Yes)){
					return;
				}
				else overwrite=true;
			}
		}
		for(int i=0; i<ncrosses; ++i){

			QString filepath(crosses_dir_path + ui.crossListWidget4->item(i)->text() + ".txt");
			if(overwrite == false && QFile::exists(filepath)){
				if(QMessageBox::No == QMessageBox::question(this, QString("Warning"), QString("some files already exist !\nDo you want to overwrite them ?"), QMessageBox::No|QMessageBox::Yes)){
					return;
				}
				else overwrite=true;
			}
		}
		// Save the data to the specified directories.
		for(int i=0; i<nselections; ++i) saveTableSelection(i, selection_dir_path + ui.selectionListWidget6->item(i)->text() + ".txt");
		for(int i=0; i<ncrosses; ++i) saveTableCrosses(i, crosses_dir_path + ui.crossListWidget4->item(i)->text() + ".txt");

		QMessageBox::information(this, "File Saved", "Files saved successfully in " + dir_path + ".");
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the multiparental example is displayed.
	-----------------------------------------------------------------------------
*/

void MainWindow::runExamples(){
	// Biparental example
	if( m_import->getIndexExample() == 0 ){
		reloadBiparentalData();
	}
	// Multiparental example
	else if( m_import->getIndexExample() == 1 ){
		reloadMultiparentalData();
	}
	m_import->close();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: hide the histogram of the score page.
	-----------------------------------------------------------------------------
*/

void MainWindow::showOneTableScore(){
	ui.rightFrame2->setVisible(false);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: hide the histogram of the score page.
	-----------------------------------------------------------------------------
*/

void MainWindow::showTwoTableScore(){
	ui.rightFrame2->setVisible(true);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: hide one selection table view.
	-----------------------------------------------------------------------------
*/

void MainWindow::showOneTableSelection(){
	showTableView(ui.rightListVLayout6, false);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: hide one selection table view.
	-----------------------------------------------------------------------------
*/

void MainWindow::showTwoTableSelection(){
	showTableView(ui.rightListVLayout6, true);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: hide one cross table view.
	-----------------------------------------------------------------------------
*/

void MainWindow::showOneTableCrosses(){
	showTableView(ui.rightListVLayout4, false);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: hide one cross table view.
	-----------------------------------------------------------------------------
*/

void MainWindow::showTwoTableCrosses(){
	showTableView(ui.rightListVLayout4, true);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QTableWidget* table : the table from the table view (comboBox + tablewidget + Labels) to hide.
	## RETURN:
	## SPECIFICATION: show or hide some selection/cross table view.
	-----------------------------------------------------------------------------
*/

void MainWindow::showTableView(QVBoxLayout* layout, bool flag){

	for(int i=0; i<layout->count(); i++){
		if(layout->itemAt(i)->widget() != NULL){
			layout->itemAt(i)->widget()->setVisible(flag);
		}
	}
}

/*
void MainWindow::runFillMDatMks(const QString &path_log_file, const QString &path_dat_file, const QString &path_output_folder){
cout << ".: TEST :." << endl;

	QMap <QString, QStringList> map_ind_qtl; // {Id} {QTL2, QTL5, ..., QTLn}
	//QString path_output_folder = "/home/gqms/valente/workspace/optimas_gui/output/";
	//QString path_dat_file = "/home/gqms/valente/workspace/optimas_gui/output/blanc_fake.dat";
	//QString path_log_file = "/home/gqms/valente/workspace/optimas_gui/output/events_summary.log";
	QString new_dat_file_path = path_output_folder + "new_" + path_dat_file.section('/', -1).replace(QRegExp("([^.]+)(\\.[^.]+)?$"), "\\1.dat"); // New location of the .dat file that will be created
	QFile NewDatFile(new_dat_file_path), DatFile(path_dat_file), LogFile(path_log_file); // file descriptors
	int nb_lines = 0;

	if( !QFile::exists(path_dat_file) ){
		QMessageBox::information(this, tr("Check .dat file"), tr("Error: opening the file [%1]").arg(path_dat_file));
		return;
	}

	if( !QFile::exists(path_log_file) ){
		QMessageBox::information(this, tr("Check events_summary.log file"), tr("Error: opening the file [%1]").arg(path_log_file));
		return;
	}

	if(!DatFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		QMessageBox::information(this, tr("Loading .dat file"), tr("Error: opening the file [%1]").arg(path_dat_file));
		return;
	}
	if(!LogFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		QMessageBox::information(this, tr("Loading events_summary.log file"), tr("Error: opening the file [%1]").arg(path_log_file));
		return;
	}

	QTextStream in_dat(&DatFile), in_log(&LogFile); // read only text stream on file

	// Find genotyping error in the .log file (Individuals and Markers)
	while( !in_log.atEnd() ) {
		QString line = in_log.readLine();
		QStringList SplitedLine = line.split('\t', QString::SkipEmptyParts);
		int size_column = SplitedLine.size();

		// Check the 1st line match with the events_summary.log file
		if( nb_lines == 0 ){
			if( !line.contains("SUMMARY [Questionable results at markers/QTL sorted by Id]") ){
				QMessageBox::information(this, tr("Loading events_summary.log"), tr("Error: you are not using the good file [events_summary.log]."));
				return;
			}
		}

		if( (nb_lines > 1) && (size_column == 5) ){
			QStringList SplitedLine2 = SplitedLine[0].split(' ', QString::SkipEmptyParts); // Id
			QStringList SplitedLine3 = SplitedLine[4].split(' ', QString::SkipEmptyParts); // QTL indexes
			SplitedLine3.removeFirst();
			int size_column2 = SplitedLine3.size();

			for(int i=0; i<size_column2; i++){
				map_ind_qtl[SplitedLine2[1]].push_back(SplitedLine3[i]);
			}
		}
		nb_lines++;
	}
	LogFile.close();

	// Creation of the new .dat file with missing data in markers with genotyping error
	if( ! NewDatFile.open(QIODevice::WriteOnly | QIODevice::Text) ){
		QMessageBox::information(this, tr("New dat file created - genotyping error checked"), tr("Error: creation of the file [%1] failed.").arg(new_dat_file_path));
		return;
	}

	nb_lines = 0;

	QTextStream out_dat_file(&NewDatFile); // write only text stream on file

	// Find genotyping error in the .dat file and fill it with missing data
	while( !in_dat.atEnd() ) {
		QString line = in_dat.readLine();
		QStringList SplitedLine = line.split('\t');
		int size_column = SplitedLine.size();

		if( (nb_lines > 0) && (size_column > 6) ){
			// Id in .log match Id in .dat
			if( map_ind_qtl.find(SplitedLine[0]) != map_ind_qtl.end() ){
				int nb_md = map_ind_qtl[SplitedLine[0]].size();
				// The genotyping error is replaced by "-"
				for(int i=0; i<nb_md;i++){
					int qtl_index = map_ind_qtl[SplitedLine[0]][i].toInt();
					if( (qtl_index + 5) <= size_column ){
						SplitedLine.replace(qtl_index + 5, QString("-"));
					}
					else{
						QMessageBox::warning(this, tr("Fill missing data for marker with genotyping error"), tr("Error: marker index out of range - Id: %1 mrk %2 - file [%3].").arg(SplitedLine[0].toStdString().c_str()).arg(map_ind_qtl[SplitedLine[0]][i].toStdString().c_str()).arg(path_dat_file));
						return;
					}
				}
			}
		}

		if( size_column > 0 ){
			out_dat_file << SplitedLine[0];
			// The new .dat file is written
			for(int i=1; i<size_column; i++){
				out_dat_file << "\t" << SplitedLine[i];
			}
			out_dat_file << "\n";
		}
		nb_lines++;
	}
	DatFile.close();
	NewDatFile.close();

	QString mess = "The new .dat file has been created and filled with missing data.\nFile location: " + new_dat_file_path;
	QMessageBox::information(this, "FillMd@Mks", mess);

	cout << ".: END TEST:." << endl;
}
*/

/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QCloseEvent* event: event raised when the mainWindow is closed.
	## RETURN:
	## SPECIFICATION: open a dialog window to know if the user really wants to 
	close OptiMAS.
	-----------------------------------------------------------------------------
*/
void MainWindow::closeEvent(QCloseEvent* event) {

	int answer = QMessageBox::question(this, QString(""), tr("Do you really want to quit OptiMAS ?"), QMessageBox::Yes | QMessageBox::No);
	if (answer == QMessageBox::Yes) event->accept();
	else event->ignore();
}
