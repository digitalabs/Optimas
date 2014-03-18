#ifndef COMPLEMENTATIONWINDOW_H_
#define COMPLEMENTATIONWINDOW_H_

#include <QtGui>
#include <QDialog>
#include <vector>

#include "../ui_complementationWindow.h"
#include "utils.h"


class ComplementationWindow : public QDialog {
	Q_OBJECT

	public:
		ComplementationWindow(QWidget *parent = 0);
		ComplementationWindow(const std::vector<double> &vect_comp_params, const QStringList &list_cycles, const QStringList &list_groups, QWidget *parent = 0);
		virtual ~ComplementationWindow();

		QPushButton *getApplyButton();
		double getTetaValue();
		int getNtValue();
		double getMsMinValue();
		int getNmaxValue();
		bool isCycleButtonSelected();
		bool isGroupButtonSelected();
		int getCycleIndex();
		int getGroupIndex();

	private slots:
			void resetComplementationWindow();
			void switchBoxes(int i);

	private:
			Ui::ComplementationWindow ui_complementation;
			QStringList m_list_cycles;
			QStringList m_list_groups;
			void initComplementationWindow(const std::vector<double> &vect_comp_params);

};


#endif /* COMPLEMENTATIONWINDOW_H_ */
