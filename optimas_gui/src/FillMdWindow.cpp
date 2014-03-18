#include "../headers/FillMdWindow.h"

using namespace std;


FillMdWindow::FillMdWindow(QWidget *parent) : QDialog(parent){
	ui_fill_md.setupUi(this);

	// Import data: each Input/Output button is connected to a QFileDialog (the QLineEdit is updated)
	connect(ui_fill_md.cmapBrowseToolButton, SIGNAL(clicked()), this, SLOT(browseCmap()));
	connect(ui_fill_md.genoBrowseToolButton, SIGNAL(clicked()), this, SLOT(browseGeno()));
	connect(ui_fill_md.resultsBrowseToolButton, SIGNAL(clicked()), this, SLOT(browseRes()));

	// Help tool buttons
	connect(ui_fill_md.cmapToolButton, SIGNAL(clicked()), this, SLOT(helpCmap()));
	connect(ui_fill_md.genoToolButton, SIGNAL(clicked()), this, SLOT(helpGeno()));
	connect(ui_fill_md.resultsToolButton, SIGNAL(clicked()), this, SLOT(helpRes()));

	// Close button
	connect(ui_fill_md.closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	m_progress_bar = NULL;
	m_process = NULL; // To run the external "optimas" program

	m_process = new QProcess(this); // to launch optimas.exe

	// optimas process
	connect(m_process, SIGNAL( readyReadStandardError() ), this, SLOT( logError() ) );
	connect(m_process, SIGNAL( finished(int, QProcess::ExitStatus) ), this, SLOT( runOptiMASFinished(int, QProcess::ExitStatus) ) );
}

FillMdWindow::~FillMdWindow() {

}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: (QString) the input.cmap file path loaded by the user.
	## SPECIFICATION: get information regarding the input.cmap file path
	loaded by the user.
	-----------------------------------------------------------------------------
*/

QString FillMdWindow::getCmapLineEdit(){
	return ui_fill_md.cmapLineEdit->text();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: (QString) the genotypes/pedigree file path loaded by the user.
	## SPECIFICATION: get information regarding the genotypes/pedigree file path
	loaded by the user.
	-----------------------------------------------------------------------------
*/

QString FillMdWindow::getGenoLineEdit(){
	return ui_fill_md.genoLineEdit->text();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: (QString) the directory path loaded by the user.
	## SPECIFICATION: get information regarding the path directory where the
	results will be stored at the end of the run.
	-----------------------------------------------------------------------------
*/

QString FillMdWindow::getResultsLineEdit(){
	return ui_fill_md.resultsLineEdit->text();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: (QPushButton *) pointer to the Run button
	## SPECIFICATION: return the Run button to start the FillMD algorithm.
	-----------------------------------------------------------------------------
*/

QPushButton * FillMdWindow::getRunPushButton(){
	return ui_fill_md.runPushButton;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the .cmap file containing the map file created to run optimas
	marker by marker is loaded.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::browseCmap() {

	QString directory = QFileDialog::getOpenFileName(this, tr("Select the .cmap file"));

	if( !directory.isEmpty() ){
		ui_fill_md.cmapLineEdit->setText(directory);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the file containing genotypes/pedigrees is loaded.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::browseGeno() {

	QString directory = QFileDialog::getOpenFileName(this, tr("Select the genotype/pedigree file (.dat)"));

	if( !directory.isEmpty() ){
		ui_fill_md.genoLineEdit->setText(directory);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: the repository containing future results is selected.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::browseRes() {

	QString directory = QFileDialog::getExistingDirectory(this, tr("Select the directory of results"), QDir::homePath());

	// The result of QFileDialog differs on different OS
	if( (directory.lastIndexOf("/") + 1) != (int)directory.size() ){
		directory = directory + "/";
	}

	if(!directory.isEmpty()){
		ui_fill_md.resultsLineEdit->setText(directory);
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: opens the given url in the appropriate Web browser.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::helpCmap() {

	QString dir_path = "";
	QString app_dir_path(QApplication::applicationDirPath());

	#if defined(__WINDOWS__) || defined(__APPLE__)
		dir_path = QString("file:///%1/%2").arg(QApplication::applicationDirPath()).arg("/website/tools.html");
	#else
		if(app_dir_path == "/usr/local/bin") dir_path = QString::fromUtf8("/usr/share/OptiMAS/website/tools.html");
		else dir_path = QDir::homePath() + QString::fromUtf8("/.OptiMAS/website/tools.html");
	#endif

	QDesktopServices::openUrl(QUrl(dir_path, QUrl::TolerantMode));
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: opens the given url in the appropriate Web browser.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::helpGeno() {

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
	## SPECIFICATION: opens a pop-up to explain what is the results file.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::helpRes() {
	QMessageBox::information(this, "OptiMAS Help", "The <strong>output directory</strong> is the folder where the new genotypes/pedigree file (i.e. /input/new_input.dat) will be stored (<strong>do not select the Program Files folder</strong>).");
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ string message: the message that will appear in the interface.
	## RETURN:
	## SPECIFICATION: An error message is sent (displayed in the interface).
	-----------------------------------------------------------------------------
*/

void FillMdWindow::showCustomErrorMessage(QString message){
	QMessageBox::warning(this, "Input/Output files", message);
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: pop-up window to wait during the process.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::showProgressBarWindow(){
	if (m_progress_bar != NULL) {
		delete m_progress_bar;
		m_progress_bar = NULL;
	}
	m_progress_bar = new QProgressDialog("Operation in progress...", "Cancel", 0, 10000, this);
	//m_progress_bar->setWindowModality(Qt::WindowModal);
	m_progress_bar->show();
	m_progress_bar->activateWindow();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN: TRUE if the 3 input/output files are not empty and FALSE otherwise.
	## SPECIFICATION: check if the input/output files were loaded before the run.
	-----------------------------------------------------------------------------
*/

bool FillMdWindow::isOK(){
	return (! ui_fill_md.cmapLineEdit->text().isEmpty()) && (! ui_fill_md.genoLineEdit->text().isEmpty()) && (! ui_fill_md.resultsLineEdit->text().isEmpty());
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

bool FillMdWindow::copyFile(const QString &input_file, const QString &output_file){

	QFile q_file(input_file);

	if ( q_file.exists() ) {
		if( q_file.copy(output_file) ){
			return true;
		}
		else{
			QMessageBox::information(this, tr("Save file"), tr("Cannot save the file %1 !").arg(output_file));
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
	## RETURN:
	## SPECIFICATION: Genotyping errors coming from the genotypes/pedigree file
	(.dat) file and recorded in the “events_summary.log” file (if present) can
	be filled with missing data (“-”) after that the consistency of marker
	genotyping information has been checked along generations of selection.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::runFillMD(){

	m_map_ind_mk.clear();
	m_map_ind_level.clear();
	m_nb_run = 0;

	m_path_dat_file = getGenoLineEdit(); // m_path_dat_file = "/home/valente/Bureau/test_fill_md/blanc_fake.dat";
	m_path_cmap_file = getCmapLineEdit(); //m_path_cmap_file = "/home/valente/Bureau/test_fill_md/check_blanc.cmap";
	m_file_res = getResultsLineEdit(); //m_file_res = "/home/valente/Bureau/test_fill_md/";

	QDir dir(m_file_res);

	QString new_file_res = m_file_res + "FillMD_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss");

	if( ! dir.mkdir(new_file_res) ){
		showCustomErrorMessage("Error in creating the folder " + new_file_res + " !!");
		return;
	}

	m_file_res = new_file_res; // Creation of a folder where will be stored the different run

	// Creation of the input folder that will contain the .dat file, the .cmap file and the new .dat file without genotyping errors
	if( ! dir.mkdir(new_file_res + "/input") ){
		showCustomErrorMessage("Error in creating the folder " + new_file_res + " !!\nCheck user's permission.");
		return;
	}

	// New location of the .cmap file that will be copied
	QFileInfo info_cmap_file(m_path_cmap_file), info_dat_file(m_path_dat_file);
	QString new_folder = m_file_res + "/input/" + info_cmap_file.fileName();

	// Copy the .map file into the new input folder
	if( ! copyFile(m_path_cmap_file, new_folder) ){
		showCustomErrorMessage("Error in copying the file " + new_folder + " !!\nCheck user's permission.");
		return;
	}

	// New location of the .dat file that will be copied
	new_folder = m_file_res + "/input/" + info_dat_file.fileName();

	// Copy the .dat file into the new input folder
	if( ! copyFile(m_path_dat_file, new_folder) ){
		showCustomErrorMessage("Error in copying the file " + new_folder + " !!\nCheck user's permission.");
		return;
	}

	// New location of the new .dat file (without genotyping errors)
	m_path_new_dat_file = m_file_res + "/input/new_input.dat";

	// Copy the .dat file into the new input folder
	if( ! copyFile(m_path_dat_file, m_path_new_dat_file) ){
		showCustomErrorMessage("Error in copying the file " + m_path_new_dat_file + " !!\nCheck user's permission.");
		return;
	}

	// New location of the new .dat file (without genotyping errors)
	m_path_log_file = m_file_res + "/input/new_events_summary.log";

	// Show the wait... window
	showProgressBarWindow();

	// Parse .dat file (m_map_ind_level, m_min_level & m_max_level updated)
	parseDatFile(m_path_dat_file);

	// First run of optimas
	runOptiMAS();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QString path_dat_file: pathway to the genotypes/pedigree file (.dat)
	## RETURN:
	## SPECIFICATION: parsing of the genotypes/pedigree file (.dat). Each individual
	of the data set is attributed to a cycle/generation value depending on its
	pedigree relationship. e.g. IL1 = 0, F1 = 1 etc.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::parseDatFile(const QString &path_dat_file){

	int nb_lines = 0;
	m_first_min_level = 0;
	m_first_max_level = 0;
	m_map_ind_level.clear();

	if( QFile::exists(path_dat_file) ){
		QFile DatFile(path_dat_file); // file descriptor

		if( !DatFile.open(QIODevice::ReadOnly | QIODevice::Text) ){
			cerr << "Error opening the file " << path_dat_file.toStdString() << endl;
			return;
		}

		QTextStream in(&DatFile); // read only text stream on file

		while( !in.atEnd() ){
			QString line = in.readLine();
			QStringList SplitedLine = line.split('\t', QString::SkipEmptyParts);

			// Individuals in the .dat file are supposed to be sorted regarding the selection cycle where they belong to
			if( (nb_lines != 0) && (SplitedLine.size() > 0) ){
				m_map_ind_level[SplitedLine[0]] = 0; // Hash map [name_indiv] --> level_indiv

				// Inbred lines (founders) = Level 0
				if( SplitedLine[3].compare("IL") != 0 ){
					m_map_ind_level[SplitedLine[0]] = max(m_map_ind_level[SplitedLine[1]], m_map_ind_level[SplitedLine[2]]) + 1; // Max level P1/P2 + 1
					m_first_max_level = max(m_first_max_level, m_map_ind_level[SplitedLine[0]]);
					m_first_min_level = min(m_first_min_level, m_map_ind_level[SplitedLine[0]]);
				}
			}
			nb_lines++;
		}
		DatFile.close();
	}
	else{
		QMessageBox::information(this, tr("Check .dat file"), tr("Error: opening the file [%1]").arg(path_dat_file));
		return;
	}

	//qDebug() << m_map_ind_level;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ QString path_log_file: pathway to the events_summary file (.log)
	## RETURN:
	## SPECIFICATION: parsing of the events_summary file (.log). Genotyping errors
	for each individual of the data set at marker positions are stored.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::parseLogFile(const QString &path_log_file){

	m_last_min_level = m_first_max_level;  // min_level is max at this step
	m_last_max_level = m_first_min_level;  // max_level is min at this step

	int nb_lines = 0;

	if( QFile::exists(path_log_file) ){
		QFile LogFile(path_log_file); // file descriptor

		if( !LogFile.open(QIODevice::ReadOnly | QIODevice::Text) ){
			cerr << "Error opening the file " << path_log_file.toStdString() << endl;
			return;
		}

		QTextStream in(&LogFile); // read only text stream on file

		while( !in.atEnd() ){
			QString line = in.readLine();
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
				QString ind_name = SplitedLine[0].split(' ', QString::SkipEmptyParts)[1]; // Id
				QStringList qtl_list = SplitedLine[4].split(' ', QString::SkipEmptyParts); // QTL indexes
				qtl_list.removeFirst();
				int nb_mks_error = qtl_list.size();

				if ( m_map_ind_level.find(ind_name) != m_map_ind_level.end() ){
					m_last_min_level = min(m_last_min_level, m_map_ind_level[ind_name]); // min_level became min at this step
					m_last_max_level = max(m_last_max_level, m_map_ind_level[ind_name]); // max_level became max at this step
				}
				else{
					QMessageBox::information(this, tr("Checking events_summary.log"), tr("Error: individual %1 from the events_summary.log file is not present in the .dat file").arg(ind_name));
					return;
				}

				for(int i=0; i<nb_mks_error; i++){
					m_map_ind_mk[ind_name].push_back(qtl_list[i].toInt());
				}
			}
			nb_lines++;
		}
		LogFile.close();
	}
	else{
		QMessageBox::information(this, tr("Check .log file"), tr("Error: opening the file [%1]").arg(path_log_file));
		return;
	}

	//qDebug() << m_map_ind_mk;
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: run OptiMAS algorithm with specified parameters/options.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::runOptiMAS(){

	m_nb_run++;
	m_map_ind_mk.clear();

	//cout << "m_first_min_level=" << m_first_min_level << " m_first_max_level=" << m_first_max_level << endl;
	//cout << "m_last_min_level=" << m_last_min_level << " m_last_max_level=" << m_last_max_level << endl;

	m_date = QDateTime::currentDateTime();
	QString new_res = m_file_res + "/" + m_date.toString("yyyy_MM_dd_HH_mm_ss_zzz"), program = ""; // Milliseconds are used to differentiate each run
	m_file_last_res = new_res;

	QString app_dir_path(QCoreApplication::applicationDirPath());

	#if defined(__WINDOWS__)
		program = QCoreApplication::applicationDirPath() + "\\software\\optimas.exe";
	#elif defined(__APPLE__)
		program = QCoreApplication::applicationDirPath() + "/software/optimas";
	#else // #elif defined(__linux__)
		program = app_dir_path + QString::fromUtf8("/optimas");
	#endif

	QStringList arguments;
	QString numQtl = "0", haploCutoff = "0.0", gamCutoff = "0.0"; // New default parameters (no cut-off applied and no verbose mode)

	arguments << m_path_new_dat_file << m_path_cmap_file << haploCutoff << gamCutoff << new_res << numQtl;

	if( m_process != NULL ) {
		m_process->start(program, arguments);
	}
	else{
		cerr << "error: m_process [runOptiMAS]" << endl;
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	@ int exit_code: the exit code of the process.
	@ int ExitStatus: [0] if the process exited normally and [1] if a known error
	occurred.
	## RETURN:
	## SPECIFICATION: This signal is emitted when the process finishes (OptiMAS
	algorithm). Here optimas will be executed for each generation of selection.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::runOptiMASFinished(int exit_code, QProcess::ExitStatus){

	// optimas run finished
	if( exit_code == 0 ){

		// Creation of the /input folder in the current directory (run)
		QDir dir(m_file_last_res);

		if( ! dir.mkdir(m_file_last_res + "/input") ){
			showCustomErrorMessage("Error in creating the folder " + m_file_last_res + "/input !!\nCheck user's permission.");
			return;
		}

		QString path_log_file = m_file_last_res + "/events_summary.log";

		// Parse events_summary.log file (Genotyping errors that we need to find and fill with missing data)
		if( QFile::exists(path_log_file) ){

			// Parse .log file
			parseLogFile(path_log_file);

			// Security
			if( m_nb_run > m_first_max_level ){
				cerr << "Something unexpected happened !! To much Run to do: OptiMAS stopped by security.";
				return;
			}

			// Fill .dat with missing data when an error occurred (cycle by cycle starting with oldest)
			fillDatFile(); // Summary of the genotyping errors (check.log file created)

			// Run optimas cycle by cycle for All QTL (if necessary - errors occured)
			runOptiMAS();
		}
		else{
			for(int i=0; i<=10000; i++){
				m_progress_bar->setValue(i);
			}
			m_progress_bar->setValue(10000); // Wait window will be closed
			QMessageBox::information(this, tr("Finding genotyping errors..."), tr("Check of genotyping errors finished with success.\n\nPlease have a look to:\n\n- The .log file to identify individuals with genotyping errors at marker positions.\n\n- The new .dat file corresponding to the new genotypes/pedigree file which has been filled/cleaned with missing data.\n\nBoth files are present in the folder [%1].\n\nUse this new file (new_input.dat) in OptiMAS (new run) to continue your analysis.").arg(m_file_res + "/input/"));
		}
	}
	else {
		showCustomErrorMessage("Error running the program !! Check the optimas.exe pathway...");
	}
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: genotyping errors found in the genoypes/pedigree file (.dat)
	will be filled with missing data generation by generation.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::fillDatFile(){

	QString last_path_new_dat_file = m_file_last_res + "/input/new_input.dat";

	// Copy of the last .dat file in the appropriate run folder
	if( ! copyFile(m_path_new_dat_file, last_path_new_dat_file) ){
		showCustomErrorMessage("Error in copying the file " + last_path_new_dat_file + " !!\nCheck user's permission.");
		return;
	}

	// QFile::copy() does not overwrite an existing file.
	if( QFile::exists(m_path_new_dat_file) ) QFile::remove(m_path_new_dat_file);

	QFile DatFile(last_path_new_dat_file), NewDatFile(m_path_new_dat_file), NewLogFile(m_path_log_file); // file descriptors

	if( !QFile::exists(last_path_new_dat_file) ){
		QMessageBox::information(this, tr("Check .dat file"), tr("Error: opening the file [%1]").arg(last_path_new_dat_file));
		return;
	}

	if( !DatFile.open(QIODevice::ReadOnly | QIODevice::Text) ){
		QMessageBox::information(this, tr("Loading .dat file"), tr("Error: opening the file [%1]").arg(last_path_new_dat_file));
		return;
	}

	// Creation of the new .dat file with missing data in markers with genotyping error
	if( ! NewDatFile.open(QIODevice::WriteOnly | QIODevice::Text) ){
		QMessageBox::information(this, tr("New dat file created - genotyping error checked"), tr("Error: creation of the file [%1] failed.").arg(m_path_new_dat_file));
		return;
	}

	// Creation of the new .log file
	if( ! NewLogFile.open(QIODevice::Append | QIODevice::Text) ){
		QMessageBox::information(this, tr("New log file created - genotyping error filled with missing data"), tr("Error: creation of the file [%1] failed.").arg(m_path_log_file));
		return;
	}

	int nb_lines = 0;

	QTextStream in_dat_file(&DatFile), out_dat_file(&NewDatFile), out_log_file(&NewLogFile); // write only text stream on file

	// Find genotyping error in the .dat file and fill it with missing data
	while( !in_dat_file.atEnd() ) {
		QString line = in_dat_file.readLine();
		QStringList SplitedLine = line.split('\t');
		int size_column = SplitedLine.size();

		if( (nb_lines > 0) && (size_column > 6) ){
			// Id in .log match Id in .dat
			if( m_map_ind_mk.find(SplitedLine[0]) != m_map_ind_mk.end() ){
				// The Id correspond to the current cycle/generation of selection (with genotyping errors)
				if( (m_map_ind_level.find(SplitedLine[0]) != m_map_ind_level.end()) && (m_map_ind_level[SplitedLine[0]] == m_last_min_level) ){
					int nb_md = m_map_ind_mk[SplitedLine[0]].size();
					out_log_file << SplitedLine[0]; // Name of the current individual

					// The genotyping error is replaced by "-" (missing data)
					for(int i=0; i<nb_md; i++){
						int mk_index = m_map_ind_mk[SplitedLine[0]][i];
						if( (mk_index + 5) <= size_column ){
							if( !SplitedLine[mk_index + 5].isEmpty() ){
								SplitedLine.replace(mk_index + 5, QString("-"));
								out_log_file << "\t" << mk_index;
							}
						}
						else{
							QMessageBox::warning(this, tr("Fill missing data for marker with genotyping error"), tr("Error: marker index out of range - Id: %1 mrk %2 - file [%3].").arg(SplitedLine[0].toStdString().c_str()).arg(m_map_ind_mk[SplitedLine[0]][i]).arg(m_path_dat_file));
							return;
						}
					}
					out_log_file << "\n";
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
	NewDatFile.close();
	DatFile.close();
	NewLogFile.close();
}


/**-----------------------------------------------------------------------------
	## PARAMETERS:
	## RETURN:
	## SPECIFICATION: Read the standard error when the program optimas is running.
	-----------------------------------------------------------------------------
*/

void FillMdWindow::logError() {
	QString error_message(m_process->readAllStandardError());
	// the message may contain several lines !!
	QStringList lines = error_message.split("\n", QString::SkipEmptyParts);
	QString mess("");

	for(int i=0; i<lines.size(); i++){
		// Total number of QTL/Id/Mks (not really an error)
		if( ! (lines[i].startsWith(".:") || lines[i].startsWith("[Warning]")) ){
			mess += (lines[i] + "\n");
		}
	}
	if (!mess.isEmpty()) showCustomErrorMessage(mess);
}
