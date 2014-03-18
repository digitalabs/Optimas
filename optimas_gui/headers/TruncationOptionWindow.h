#ifndef TRUNCATIONOPTIONWINDOW_H_
#define TRUNCATIONOPTIONWINDOW_H_

#include "../ui_truncationOptionWindow.h"

#include <QtGui>
#include <QDialog>


class TruncationOptionWindow : public QDialog {
	Q_OBJECT

	public:
		TruncationOptionWindow(QWidget *parent);
		TruncationOptionWindow(const std::vector<int> &vect_truncation_option, const QStringList &list_cycles, const QStringList &list_groups, QWidget *parent = 0);
		~TruncationOptionWindow();

		QPushButton *getApplyButton();
		bool isCycleButtonSelected();
		bool isGroupButtonSelected();
		int getCycleIndex();
		int getGroupIndex();

	private slots:
		void switchBoxes(int i);
		void resetTruncationOptionWindow();

	private:
		// attributes
		Ui::TruncationOptionWindow ui_truncationOption;
		QStringList m_list_cycles;
		QStringList m_list_groups;

		void initTruncationOptionWindow(const std::vector<int> &vect_truncation_option);

};


#endif /* TRUNCATIONOPTIONWINDOW_H_ */
