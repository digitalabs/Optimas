#ifndef HISTOCOLORWINDOW_H_
#define HISTOCOLORWINDOW_H_

#include <QtGui>
#include <QDialog>
#include <QColor>
#include "utils.h"

#include "../ui_histoColorWindow.h"
#include "utils.h"

class  HistoColorWindow : public QDialog {
	Q_OBJECT

	public:
		HistoColorWindow(QWidget *parent = 0);
		HistoColorWindow(QMap<QString, QColor>& map_cycleColor, const QStringList& vect_generation, QWidget *parent = 0);
		~HistoColorWindow();

	private slots:
		void openCycleColorDialog();
		void applyColor();
		void resetDefaultColors();

	private:
		QVector<QLabel*> m_cycleLabels;
		QVector<QPushButton*> m_cycleColorButtons;
		QVector<QSpacerItem*> m_hSpacers;

		QMap<QString, QColor>* m_map_cycleColor_ptr;

		Ui::HistoColorWindow ui;
		void initHistoColorWindow(const QMap<QString, QColor>& map_cycleColor, const QStringList& vect_generation);

};

#endif /* HISTOCOLORWINDOW_H_ */
