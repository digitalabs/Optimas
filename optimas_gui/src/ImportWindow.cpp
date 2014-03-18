#include "../headers/ImportWindow.h"

using namespace std;


ImportWindow::ImportWindow(QWidget *parent, const QString & lastInput, const QString & lastOutput) : QDialog(parent), m_lastInput(lastInput), m_lastOutput(lastOutput){
	ui_import.setupUi(this);

	if (m_lastInput.isEmpty()) m_lastInput = QDir::homePath();
	if (m_lastOutput.isEmpty()) m_lastOutput = QDir::homePath();

	// Import data run
	// Import data: each Input/Output button is connected to a QFileDialog (the QLineEdit is updated)
	connect(ui_import.mapBrowseToolButton, SIGNAL(clicked()), this, SLOT(browseMap()));
	connect(ui_import.genoBrowseToolButton, SIGNAL(clicked()), this, SLOT(browseGeno()));
	connect(ui_import.resultsBrowseToolButton, SIGNAL(clicked()), this, SLOT(browseRes()));

	// Selection of all or one specific QTL for the analysis
	connect(ui_import.numQtlRadioButton, SIGNAL(clicked()), this, SLOT(selectQtl()));
	connect(ui_import.allRadioButton, SIGNAL(clicked()), this, SLOT(selectAllQtl()));

	// Help tool buttons
	connect(ui_import.mapToolButton, SIGNAL(clicked()), this, SLOT(helpMap()));
	connect(ui_import.genoToolButton, SIGNAL(clicked()), this, SLOT(helpGeno()));
	connect(ui_import.helpPushButton, SIGNAL(clicked()), this, SLOT(helpIndex()));
	connect(ui_import.resultsToolButton, SIGNAL(clicked()), this, SLOT(helpRes()));
	connect(ui_import.advancedToolButton, SIGNAL(clicked()), this, SLOT(helpAdvanced()));

	// Close button
	connect(ui_import.closePushButton, SIGNAL(clicked()), this, SLOT(closeWindow()));


	// Example data run
	// Swap example data resume
	connect(ui_import.exampleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(swapExampleResume(int)));

	// Cancel button
	connect(ui_import.exampleCancelPushButton, SIGNAL(clicked()), this, SLOT(close()));

	// Help tool buttons
	connect(ui_import.exampleHelpPushButton, SIGNAL(clicked()), this, SLOT(helpIndex()));
}

ImportWindow::~ImportWindow() {

}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: (QString) the map file path loaded by the user.
	## SPECIFICATION: get information regarding the map file path loaded by the user.
	-----------------------------------------------------------------------------
*/

QString ImportWindow::getMapLineEdit() const {
	return ui_import.mapLineEdit->text();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: (QString) the genotype/pedigree file path loaded by the user.
	## SPECIFICATION: get information regarding the genotype/pedigree file path
	loaded by the user.
	-----------------------------------------------------------------------------
*/

QString ImportWindow::getGenoLineEdit() const {
	return ui_import.genoLineEdit->text();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: (QString) the directory path loaded by the user.
	## SPECIFICATION: get information regarding the path directory where the
	results will be stored at the end of the run.
	-----------------------------------------------------------------------------
*/

QString ImportWindow::getResultsLineEdit() const{
	return ui_import.resultsLineEdit->text();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QString text: the new results path directory
	## RETURN:
	## SPECIFICATION: set information regarding the path directory where the
	results will be stored at the end of the run.
	-----------------------------------------------------------------------------
*/

void ImportWindow::setResultsLineEdit(QString text){
	ui_import.resultsLineEdit->setText(text);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: TRUE if the button All QTL is checked.
	## SPECIFICATION: check if the button "All QTL" is checked.
	-----------------------------------------------------------------------------
*/

bool ImportWindow::isAllRadioButtonChecked() const {
	return ui_import.allRadioButton->isChecked();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: TRUE if the verbose mode is checked.
	## SPECIFICATION: check if the verbose mode is checked.
	-----------------------------------------------------------------------------
*/

bool ImportWindow::isVerboseChecked() const {
	return ui_import.verboseCheckBox->isChecked();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: the haplotype cut-off value converted in QString.
	## SPECIFICATION: get information regarding the haplotype cut-off.
	-----------------------------------------------------------------------------
*/

QString ImportWindow::getTextHaplo() const {
	return ui_import.haploDoubleSpinBox->textFromValue(ui_import.haploDoubleSpinBox->value());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: the gamete cut-off value converted in QString.
	## SPECIFICATION: get information regarding the gamete cut-off.
	-----------------------------------------------------------------------------
*/

QString ImportWindow::getTextGam() const {
	return ui_import.gamDoubleSpinBox->textFromValue(ui_import.gamDoubleSpinBox->value());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	@ int index: the index of the selected QTL.
	## SPECIFICATION: get information regarding the selected QTL.
	-----------------------------------------------------------------------------
*/

int ImportWindow::getNumQtl() const {
	return ui_import.numQtlSpinBox->value();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int value: the percentage value of advancement.
	## RETURN:
	## SPECIFICATION: change the progressbar value.
	-----------------------------------------------------------------------------
*/

void ImportWindow::setValueProgressBar(int value){
	ui_import.progressBar->setValue(value);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: change the index of the current QTL value.
	-----------------------------------------------------------------------------
*/

void ImportWindow::setNumQtlProgressBar(QString value){
	ui_import.numQtlLabel->setText(value);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: change the index of the current individual running.
	-----------------------------------------------------------------------------
*/

void ImportWindow::setNumindivProgressBar(QString value){
	ui_import.numIndivLabel->setText(value);
}

/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QString value: the text containing the total number of Id/QTL/Markers
	## RETURN:
	## SPECIFICATION: set the total number of Id/QTL/Markers coming from the
	input files.
	-----------------------------------------------------------------------------
*/

void ImportWindow::setTotalQtlMksValues(QString value){
	ui_import.qtlMksLabel->setText(value);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ string label: the new label of the button
	## RETURN:
	## SPECIFICATION: change the label name of the Run button.
	-----------------------------------------------------------------------------
*/

void ImportWindow::setRunPushButton(string label){
	ui_import.runPushButton->setText(label.c_str());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: (QString) the text written within the Run button.
	## SPECIFICATION: return the text written within the Run button.
	-----------------------------------------------------------------------------
*/

QString ImportWindow::getTextRunPushButton() const {
	return ui_import.runPushButton->text();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: (QPushButton *) pointer to the Run button
	## SPECIFICATION: return the Run button to start OptiMAS algorithm.
	-----------------------------------------------------------------------------
*/

QPushButton * ImportWindow::getRunPushButton() {
	return ui_import.runPushButton;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: (QPushButton *) pointer to the Run button (examples window)
	## SPECIFICATION: return the example Run button to start examples.
	-----------------------------------------------------------------------------
*/

QPushButton * ImportWindow::getExampleRunPushButton() {
	return ui_import.exampleRunPushButton;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: (int) index of the current example.
	## SPECIFICATION: return the index of the selected example.
	-----------------------------------------------------------------------------
*/

int ImportWindow::getIndexExample() const {
	return ui_import.exampleComboBox->currentIndex();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ bool boolean: TRUE or FALSE
	## RETURN:
	## SPECIFICATION: the import data window is set to disable or enable.
	-----------------------------------------------------------------------------
*/

void ImportWindow::setImportDisabled(bool boolean){
	ui_import.importGroupBox->setDisabled(boolean);
	ui_import.outputGroupBox->setDisabled(boolean);
	ui_import.advancedGroupBox->setDisabled(boolean);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the file containing the markers map is loaded.
	-----------------------------------------------------------------------------
*/

void ImportWindow::browseMap() {

	QString directory = QFileDialog::getOpenFileName(this, tr("Select the map file (.map)"), m_lastInput);

	if( !directory.isEmpty() ){
		ui_import.mapLineEdit->setText(directory);
		m_lastInput = directory.section(QRegExp("[/\\\\]"), 0,-2);
		if (m_lastInput.isEmpty()) m_lastInput = "/";
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the file containing genotypes is loaded.
	-----------------------------------------------------------------------------
*/

void ImportWindow::browseGeno() {

	QString directory = QFileDialog::getOpenFileName(this, tr("Select the genotype/pedigree file (.dat)"), m_lastInput);

	if( !directory.isEmpty() ){
		ui_import.genoLineEdit->setText(directory);
		m_lastInput = directory.section(QRegExp("[/\\\\]"), 0,-2);
		if (m_lastInput.isEmpty()) m_lastInput = "/";
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the repository containing future results is selected.
	-----------------------------------------------------------------------------
*/

void ImportWindow::browseRes() {

	QString directory = QFileDialog::getExistingDirectory(this, tr("Select the directory of results"),  m_lastOutput);

	// The result of QFileDialog differs on different OS
	if( (directory.lastIndexOf("/") + 1) != (int)directory.size() ){
		directory = directory + "/";
	}

	if(!directory.isEmpty()){
		ui_import.resultsLineEdit->setText(directory);
		m_lastOutput = directory;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: selection of one specific QTL to run.
	-----------------------------------------------------------------------------
*/

void ImportWindow::selectQtl() {
		ui_import.numQtlSpinBox->setDisabled(false);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: selection of all the QTL for the analysis.
	-----------------------------------------------------------------------------
*/

void ImportWindow::selectAllQtl() {
	ui_import.numQtlSpinBox->setDisabled(true);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: opens the given url in the appropriate Web browser.
	-----------------------------------------------------------------------------
*/

void ImportWindow::helpMap() {

	QString dir_path = "";
	QString app_dir_path(QApplication::applicationDirPath());

	#if defined(__WINDOWS__) || defined(__APPLE__)
		dir_path = QString("file:///%1/%2").arg(QApplication::applicationDirPath()).arg("/website/map.html");
	#else
		if(app_dir_path == "/usr/local/bin") dir_path = QString::fromUtf8("/usr/share/OptiMAS/website/map.html");
		else dir_path = QDir::homePath() + QString::fromUtf8("/.OptiMAS/website/map.html");
	#endif

	QDesktopServices::openUrl(QUrl(dir_path, QUrl::TolerantMode));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: opens the given url in the appropriate Web browser.
	-----------------------------------------------------------------------------
*/

void ImportWindow::helpGeno() {

	QString dir_path = "";
	QString app_dir_path(QApplication::applicationDirPath());

	#if defined(__WINDOWS__) || defined(__APPLE__)
		dir_path = QString("file:///%1/%2").arg(QApplication::applicationDirPath()).arg("/website/genotype.html");
	#else
		if(app_dir_path == "/usr/local/bin") dir_path = QString::fromUtf8("/usr/share/OptiMAS/website/genotype.html");
		else dir_path = QDir::homePath() + QString::fromUtf8("/.OptiMAS/website/genotype.html");
	#endif

	QDesktopServices::openUrl(QUrl(dir_path, QUrl::TolerantMode));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: opens the given url in the appropriate Web browser.
	-----------------------------------------------------------------------------
*/

void ImportWindow::helpIndex() {

	QString dir_path = "";
	QString app_dir_path(QApplication::applicationDirPath());

	#if defined(__WINDOWS__) || defined(__APPLE__)
		dir_path = QString("file:///%1/%2").arg(QApplication::applicationDirPath()).arg("/website/index.html");
	#else
		if(app_dir_path == "/usr/local/bin") dir_path = QString::fromUtf8("/usr/share/OptiMAS/website/index.html");
		else dir_path = QDir::homePath() + QString::fromUtf8("/.OptiMAS/website/index.html");;
	#endif

	QDesktopServices::openUrl(QUrl(dir_path, QUrl::TolerantMode));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: opens a popup to explain what is the results file.
	-----------------------------------------------------------------------------
*/

void ImportWindow::helpRes() {
	QMessageBox::information(this, "OptiMAS Help", "The <strong>output directory</strong> is the folder where will be stored the results of a run (<strong>do not select the Program Files folder</strong>).");
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: opens the given url in the appropriate Web browser.
	-----------------------------------------------------------------------------
*/

void ImportWindow::helpAdvanced() {

	QString dir_path = "";
	QString app_dir_path(QApplication::applicationDirPath());

	#if defined(__WINDOWS__) || defined(__APPLE__)
		dir_path = QString("file:///%1/%2").arg(QApplication::applicationDirPath()).arg("/website/run_gui.html");
	#else
		if(app_dir_path == "/usr/local/bin") dir_path = QString::fromUtf8("/usr/share/OptiMAS/website/run_gui.html");
		else dir_path = QDir::homePath() + QString::fromUtf8("/.OptiMAS/website/run_gui.html");
	#endif

	QDesktopServices::openUrl(QUrl(dir_path, QUrl::TolerantMode));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: opens a popup to explain what is the results file.
	-----------------------------------------------------------------------------
*/

void ImportWindow::closeWindow(){
	if( ui_import.runPushButton->text().compare("Run") == 0 ){
		this->close();
	}
	else{
		cout << ui_import.runPushButton->text().toStdString() << endl;
		QMessageBox::warning(this, "Close Import Data Window", "OptiMAS is still running ! Please stop it before closing the window.");
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ string message: the message that will appear in the interface.
	## RETURN:
	## SPECIFICATION: An error message is sent (displayed in the interface).
	-----------------------------------------------------------------------------
*/

void ImportWindow::showCustomErrorMessage(string message){
	QMessageBox::critical(this, "Input/Output files", message.c_str());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QString input_file: the file to copy
	@ QString output_file: the destination of the copy
	## RETURN: true is the file has been copied and false otherwise.
	## SPECIFICATION:
	Copy a given file "input_file" into a defined location "output_file".
	-----------------------------------------------------------------------------
*/

bool ImportWindow::copyFile(QString input_file, QString output_file){

	QFile q_file(input_file);

	if ( q_file.exists() ) {
		if( q_file.copy(output_file) ){
			return true;
		}
		else{
			QMessageBox::information(this, tr("Save file"), tr("Cannot save the file %1 !").arg(input_file));
			return false;
		}
	}
	else{
		QMessageBox::information(this, tr("Save file"), tr("The file %1 doesn't exists !").arg(input_file));
		return false;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: TRUE if the 3 input/ouput files are not empty and FALSE otherwise.
	## SPECIFICATION: check if the input/output files were loaded before the run.
	-----------------------------------------------------------------------------
*/

bool ImportWindow::isOK(){
	return (! ui_import.mapLineEdit->text().isEmpty()) && (! ui_import.genoLineEdit->text().isEmpty()) && (! ui_import.resultsLineEdit->text().isEmpty());
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int index: the current index of the combobox
	## RETURN:
	## SPECIFICATION: a resume is displayed regarding the appropriate example data.
	-----------------------------------------------------------------------------
*/

void ImportWindow::swapExampleResume(int index){
	if( index == 0 ){
		ui_import.exampleLabel->setText(QApplication::translate("ImportWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
		"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
		"p, li { white-space: pre-wrap; }\n"
		"</style></head><body style=\" font-family:'Sans Serif'; font-size:"TEXT_SIZE"; font-weight:400; font-style:normal;\">\n"
		"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">The data set example is coming from a biparental population (F2-F252) of 300 F3 individuals that were evaluated during 2 years in several trials. The results of the QTL detection were published in Huang et al., Genetics, 2010.</p>"
		"<p align=\"justify\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p>"
		"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">We selected 10 QTL based on an index evaluated for dry grain yield and grain moisture (see moreau_f3_optimas.map). A set of 39 markers were selected with ~4 flanking markers/QTL.</p></body></html>", 0, QApplication::UnicodeUTF8));
	}
	else if( index == 1 ){
		ui_import.exampleLabel->setText(QApplication::translate("ImportWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
		"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
		"p, li { white-space: pre-wrap; }\n"
		"</style></head><body style=\" font-family:'Sans Serif'; font-size:"TEXT_SIZE"; font-weight:400; font-style:normal;\">\n"
		"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">6 F<sub>2</sub> populations, with 150 individuals each, were obtained from a diallel cross between 4 connected unrelated maize inbreds (<i>DE, F283, F810</i> and <i>F9005</i>).</p>"
		"<p align=\"justify\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p>"
		"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">10 QTL were detected for silking date <i>(Blanc et al., Theor Appl Genet, 2006)</i>. A set of 34 markers were selected with ~3 fully informative (microsatellites) flanking markers/QTL. 2 cycles of MARS were performed with each time step of selfing process.</p></body></html>", 0, QApplication::UnicodeUTF8));
	}
}
