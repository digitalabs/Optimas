#ifndef FILLMDWINDOW_H_
#define FILLMDWINDOW_H_

#include <QtGui>
#include <QDialog>
#include <iostream>

#include "../ui_fillMdWindow.h"
#include "utils.h"

// Depends on the total number of markers (genotype file)
#define NB_LINE_MAX 3000000


class FillMdWindow : public QDialog {
	Q_OBJECT

	public:
		FillMdWindow(QWidget *parent = 0);
		virtual ~FillMdWindow();

		QString getCmapLineEdit();
		QString getGenoLineEdit();
		QString getResultsLineEdit();
		QPushButton *getRunPushButton();
		bool isOK();
		void runFillMD();

	private slots:
		void browseCmap();
		void browseGeno();
		void browseRes();
		void helpCmap();
		void helpGeno();
		void helpRes();
		void runOptiMASFinished(int exit_code, QProcess::ExitStatus);
		void logError();

	private:
		Ui::FillMdWindow ui_fill_md;
		QProcess *m_process; // To run the external "optimas" program
		QProgressDialog *m_progress_bar; // Wait... dialog
		QDateTime m_date; // Date of the starting run
		QString m_file_res; // Folder where will be stored the results
		QString m_file_last_res; // Folder containing results file(s) created during the different run
		QString m_path_dat_file;
		QString m_path_new_dat_file; // New .dat file (without genotyping errors)
		QString m_path_cmap_file; // Map file created in a previous analysis (to run optimas mk by mk in order to find genotyping errors)
		QString m_path_log_file; // Names of individuals and markers having "true" genotyping errors will be stored

		QMap <QString, QList<int> > m_map_ind_mk; // [Ind1, ..., Indn] --> [1, 3, 7, ...] individuals having genotyping errors
		QMap <QString, int> m_map_ind_level; // MARS cycle of each individual regarding its pedigree (height of the tree)

		int m_first_min_level, m_first_max_level; // MARS cycles (after parsing the .dat file)
		int m_last_min_level, m_last_max_level; // Cycles with genotyping error (after parsing the .log file)
		int m_nb_run;

		void showCustomErrorMessage(QString message);
		void showProgressBarWindow();
		bool copyFile(const QString &input_file, const QString &output_file);
		void runOptiMAS();
		void parseDatFile(const QString &path_dat_file);
		void parseLogFile(const QString &path_log_file);
		void fillDatFile();
};


#endif /* FILLMDWINDOW_H_ */
