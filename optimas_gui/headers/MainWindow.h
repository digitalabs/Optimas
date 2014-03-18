#ifndef HEADER_MAINWINDOW
#define HEADER_MAINWINDOW

//#include <QDebug>
#include <QtGui>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QProcess>
#include <QTime>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <cmath>
#include <qapplication.h>
#include <qpen.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_interval_data.h>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qsvggenerator.h>


#include "../ui_mainWindow.h"

#include "TableViewWindow.h"
#include "ImportWindow.h"
#include "FillMdWindow.h"
#include "AboutWindow.h"
#include "LicenseWindow.h"
#include "HistogramItem.h"
#include "AddListDialog.h"
#include "utils.h"
#include "BoxPlotItem.h"
#include "OTableWidget.h"
#include "VisualizationWindow.h"
#include "ComplementationWindow.h"
#include "CompResultsWindow.h"
#include "FindDialog.h"
#include "CrossOptionWindow.h"
#include "TruncationOptionWindow.h"
#include "HistoColorWindow.h"
#include "QtlWeightWindow.h"


class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
		virtual ~MainWindow();

		void showCustomErrorMessage(const QString & message);

	private slots:
		void showTableViewWindow();
		void showHistoColorWindow();
		void showQtlWeightWindow();
		void showVisualizationWindow();
		void showImportWindow();
		void showFillMdWindow();
		void showAboutWindow();
		void showLicenseWindow();
		void showHelpWindow();
		void showFindWindow();
		void showTruncationOptionWindow();
		void showComplementationWindow();
		void showCrossOptionWindow();
		void changePage(QListWidgetItem *current, QListWidgetItem *previous);
		void applyVisualization();
		void applyTruncationOption();
		void applyComplementation();
		void applyCrossOption();
		void runProcess();
		void runOptiMAS();
		void logOutput();
		void logError();
		void onProcessError(QProcess::ProcessError error);
		void runOptiMASFinished(int exit_code, QProcess::ExitStatus);
		void runFillMd();
		void saveGraphLeftScorePage();
		void plotRightGraphScorePageQtl(int index_qtl);
		void plotRightGraphScorePage(int index_graph);
		void saveGraphRightScorePage();
		void plotLeftGraphSelectionPageQtl(int index_qtl);
		void plotRightGraphSelectionPageQtl(int index_qtl);
		void plotLeftGraphSelectionPageList(int index_list_selected);
		void plotRightGraphSelectionPageList(int index_list_selected);
		void saveGraphLeftSelectionPage();
		void saveGraphRightSelectionPage();
		void runComputeWeight();
		void addToList();
		void addIndToList(int index_list_selected, const QString & listName);
		void updateList(OTableWidget* table, int row);
		void displayCellInfo(int row, int column, OTableWidget *tab);
		void displayCellInfoScore(int row, int column);
		void displayCellInfoSelectionLeft(int row, int column);
		void displayCellInfoSelectionRight(int row, int column);
		void displayCellInfoCrossesLeft(int row, int column);
		void displayCellInfoCrossesRight(int row, int column);
		void displayLeftListSelection(int index_list_selected);
		void displayRightListSelection(int index_list_selected);
		void maxCrossContribChanged(int index_list_selection);
		void maxCrossContrib2ListsChanged(int index_list2_selection);
		void checkBoxListsSelectionChanged(int state);
		void deleteIndToLeftListSelection();
		void deleteIndToRightListSelection();
		void editSelectionItemList(QListWidgetItem *item);
		void itemListSelectionChanged(QListWidgetItem * item);
		void leftListSelectionChanged(QListWidgetItem *item);
		void runTruncationSelection();
		void runComplementationSelection();
		void saveListSelection();
		void addListSelection();
		void removeListSelection();
		void resetListSelection();
		void zoomInPedigree();
		void zoomOutPedigree();
		void zoomOriginalPedigree();
		void zoomFitPedigree();
		void runPedigree();
		void savePedigree();
		void runCrosses();
		void editCrossItemList(QListWidgetItem *item);
		void itemListCrossChanged(QListWidgetItem * item);
		void leftListCrossChanged(QListWidgetItem *item);
		void saveListCrosses();
		void addListCrosses();
		void removeListCrosses();
		void resetListCrosses();
		void deleteIndToLeftListCrosses();
		void deleteIndToRightListCrosses();
		void displayLeftListCrosses(int index_list_selected);
		void displayRightListCrosses(int index_list_selected);
		void plotLeftGraphCrossPage(int index_plot);
		void plotRightGraphCrossPage(int index_plot);
		void plotHistoLeftGraphCrossPageQtl(int index_qtl);
		void plotHistoRightGraphCrossPageQtl(int index_qtl);
		void plotLeftGraphCrossPageList(int index_list_selected);
		void plotRightGraphCrossPageList(int index_list_selected);
		void saveGraphLeftCrossesPage();
		void saveGraphRightCrossesPage();
		void reloadData();
		void reloadBiparentalData();
		void reloadMultiparentalData();
		void saveAllLists();
		void clearAll();
		void runExamples();
		void showOneTableScore();
		void showTwoTableScore();
		void showOneTableSelection();
		void showTwoTableSelection();
		void showOneTableCrosses();
		void showTwoTableCrosses();

	private:
		Ui::MainWindow ui;

		ImportWindow *m_import;
		FillMdWindow *m_fill_md;
		AboutWindow *m_about;
		LicenseWindow *m_license;
		ComplementationWindow *m_complementation;
		CompResultsWindow *m_complementation_results;
		FindDialog *m_findDialog;
		CrossOptionWindow *m_cross_option;
		TruncationOptionWindow *m_truncation_option;

		QProcess *m_process; // To run the external "optimas" program
		bool m_process_killed;
		QDateTime m_date; // Date of the starting run
		QString m_file_res; // Results file created during the run
		QString m_lastOutput; // Last selected output folder
		QString m_lastInput; // Last selected input folder
		QString m_lastGraphSave; // Last selected destination folder for any graph.

		QAction *m_addListAction, *m_deleteLeftListSelectionAction, *m_deleteRightListSelectionAction, *m_deleteLeftListCrossesAction, *m_deleteRightListCrossesAction;
		AddListDialog *m_addListDialog;

		//QVector<map_iterator> m_vect_iter; // vector of m_map_scores elements iterators;
		QMap <QString, QStringList> m_map_scores; // [Ind1, ..., Indn] --> [p1, p2, Cycle, Group, MS, w, UC, nb +/+, ..., qtl1, ..., qtln]
		QMap <QString, QList<double> > m_map_gene_scores; // [All, F2, C, CM] --> [nb, All, w, qtl1, ..., qtln]
		QMap <QString, QList<QVector<int> > > m_map_gene_quantiles; // [All, F2, C, CM] --> [All, w, qtl1, ..., qtln] --> [0.1, ..., 1.0]
		QMap <QString, QStringList> m_map_group; // [Group1, ..., Groupn] --> [Ind2, Ind5, Ind7...]
		QVector<QMap <QString, QVector<double> > > m_vect_group_scores; //   [MS, Weight, QTL1, ...] --> [ grp1, ...] --> [indivs scores...]
																						// i  0   1       2
		//							  MS     ,  qtl1     , ..., 					qtln
		// [Ind1, ..., Indn] --> [ ["0.8",""], ["0.0",""], ..., ["0.073", "d:d=0.010	f:d=0.036	f:f=0.026"] ]
		QMap <QString, QVector< QVector<QString> > > m_homo_fav;	// Homo(+/+)
		QMap <QString, QVector< QVector<QString> > > m_hetero;		// Hetero(+/-)
		QMap <QString, QVector< QVector<QString> > > m_homo_unfav;	// Homo(-/-)

		//							All (a,b)		qtl1 (a,b)				qtln (a,b)
		// [Ind1, ..., Indn] --> [ ["0.47","0.53"], ["0.88","0.12"] , ... , ["0.50","0.50"]]
		QMap <QString, QVector< QVector<QString> > > m_parents;
		
		QStringList m_vect_generation; // Number of classes (generation/cycles of MAS)
		QStringList m_list_selection; // List of selected individuals
		QStringList m_list_crosses; // List of crosses between selected individuals
		std::vector<double> m_vect_qtl_weight; // Vector of QTL weights

		QList<QMap <QString, int> *> m_vect_map_selection; // List of lists of selected individuals [List1, ..., Listn] --> [ [Ind1, List1], [Ind2, List1], ..., [Indn, List1]
		QList<QMap <QString, QStringList> *> m_vect_map_crosses; // List of lists of crosses [List1, ..., Listn] --> [ [Ind1, ..., Indn] --> [p1, p2, MS, w, UC, nb +/+, ..., qtl1, ..., qtln] ]
		QList<QStringList> m_vect_notes_crosses;

		QStringList m_vect_fav_all; // list of the favorable alleles for each QTL
		std::vector<QColor> m_vect_color; // Color to put in the score page regarding the MS at each QTL
		std::vector<double> m_vect_cut_off; // Parameters (cut-off) in the visualization page
		std::vector<int> m_vect_truncation_option; // Parameters/Options settings for the truncation selection strategy [0/1 (cycle or group), 0...n (each group or cycle)]
		std::vector<double> m_vect_complementation_params; // Parameters settings for the complementation strategy
		std::vector<int> m_vect_cross_option; // Parameters/Options settings for the intermating scheme


		void createMapFileCheck(QString path_map_file);
		void resetAll();
		void setDataPath();
		void initMainWindow();
		void resetScorePage();
		void resetSelectionPage(int answer);
		void resetCrossingPage(int answer);
		void clearSelectionPage();
		void clearCrossingPage();
		void displayAllPages();
		void displayTableScore(const QString & name_file);
		int whichQuantile(double score) const;
		int getFavAlleles();
		int parseHomoHeteroFile(int num_qtl);
		void displayTable(const QString & name_file, QTableWidget *tab);
		void plotLeftGraphScorePage();
		void plotHistoScorePage(int index_qtl);
		void plotBoxPlotScorePage(int index_qtl);
		void setLabelScorePage(QLabel *label, double mean, double var);
		void setLabelSelectionPage(QLabel *label1, QLabel *label2, double mean, double var, int nb_ind, int nb_grp);
		void setLabelSelectionPage(QLabel *label, double mean, double var);
		void setLabelCrossPage(QFrame* frame, int index_list_crosses);
		void setLabelCrossPage(QLabel *label, double mean, double var);
		void plotGraphSelectionPage(QwtPlot *graph, int index_list_selected, int index_qtl);
		void createActions();
		void createContextMenu();
		void initVisualizationParameters();
		void initTruncationOption();
		void initComplementationParameters();
		void initCrossOption();
		void displayListSelection(QTableWidget *tab, int index_list_selected);
		void deleteIndToListSelection(QTableWidget *tab, int index_list_selected);
		void saveTableSelection(int index_list_selected, const QString & fileName);
		void resetListSelection(int answer);
		void runPedigreeAll();
		void createAllPedigree(Agraph_t *g, const std::string & ind);
		void createSelectedPedigree(Agraph_t *g, const std::string & ind, int color);
		void runPedigreeAlone();
		void runCompleteCrosses();
		int nb_hetero(int disc_ind1, int disc_ind2);
		void runCompleteCrossesConstraints();
		QString getCriterion(int index_criterion);
		void runBetterHalf();
		void run2ListsCrosses();
		void run2ListsCrossesConstraints();
		void saveTableCrosses(int index_list_selected, const QString & fileName);
		void resetListCrosses(int answer);
		void deleteIndToListCrosses(QTableWidget *tab, int index_list_selected);
		void displayListCrosses(QTableWidget *tab, int index_list_selected);
		void displayCellInfoCrosses(int row, int column, OTableWidget *tab);
		void plotHistoCrossPage(QwtPlot *graph, int index_list_selected, int index_qtl);
		void plotGraphCrossPage(QwtPlot *graph, int index_list_selected);
		bool reloadUserSettings();
		void reloadDirectory();
		void reloadListsSelection();
		void addListSelection(const QString & listName);
		void addIndFromListSelection(const QString & name_file, int index_list_selected);
		void reloadListsCrosses();
		void addListCrosses(const QString & listName);
		void addIndFromListCrosses(const QString & name_file, int index_list_crosses);
		void showTableView(QVBoxLayout* layout, bool flag);
		void print(QwtPlot* plot, const QString& fileName);
		void saveGraph(QwtPlot* plot);
		void saveUserSettings(const QString & path);
		//void runFillMDatMks(const QString &path_log_file, const QString &path_dat_file, const QString &path_output_folder);
        void closeEvent(QCloseEvent* event); // closeEvent redefinition (inherited from parent class).
};


#endif
