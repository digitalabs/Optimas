#ifndef CROSSOPTIONWINDOW_H_
#define CROSSOPTIONWINDOW_H_


#include <QtGui>
#include <QDialog>
#include <vector>

#include "../ui_crossOptionWindow.h"
#include "utils.h"

class CrossOptionWindow : public QDialog {
	Q_OBJECT

	public:
		CrossOptionWindow(QWidget *parent = 0);
		CrossOptionWindow(const std::vector<int> &vect_cross_option, QWidget *parent = 0);
		virtual ~CrossOptionWindow();

		QPushButton *getApplyButton();
		bool isMaxButtonChecked();
		bool isUnlimitedButtonChecked();
		int getNbCrosses();
		int getIndContrib();
		int getCriterionCurrentIndex();
		int getMethodCurrentIndex();

	private slots:
			void resetCrossOptionWindow();
			void selectAllIndCrossedTogether();
			void selectNbCross();
			void selectAllIndContribution();
			void selectNbIndContribution();

	private:
			Ui::CrossOptionWindow ui_crossOption;
			void initCrossOptionWindow(const std::vector<int> &vect_cross_option);
			const std::vector<int> * m_vect_cross_option;

};


#endif /* CROSSOPTIONWINDOW_H_ */
