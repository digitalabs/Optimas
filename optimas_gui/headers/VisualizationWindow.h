#ifndef VISUALIZATIONWINDOW_H_
#define VISUALIZATIONWINDOW_H_

#include <QtGui>
#include <QDialog>
#include <vector>
#include <QColor>

#include "../ui_visualizationWindow.h"
#include "utils.h"

class VisualizationWindow : public QDialog {
	Q_OBJECT

	public:
		VisualizationWindow(QWidget *parent = 0);
		VisualizationWindow(std::vector<QColor> &vect_color, std::vector<double> &vect_cut_off, QWidget *parent = 0);
		virtual ~VisualizationWindow();

		QPushButton *getApplyButton();
		QColor getHomoFavButtonColor();
		QColor getHomoUnfavButtonColor();
		QColor getHeteroButtonColor();
		QColor getQuestionTagButtonColor();
		double getHomoFavValue();
		double getHomoUnfavValue();
		double getHeteroValue();

	private slots:
			void openHomoFavColorDialog();
			void openHomoUnfavColorDialog();
			void openHeteroColorDialog();
			void openQuestionTagColorDialog();
			void resetVisualizationWindow();
			void onApply();

	private:
			Ui::VisualizationWindow ui_visualization;
			void initVisualizationWindow();
			std::vector<QColor>* m_vect_color;
			std::vector<double>* m_vect_cut_off;
};


#endif /* VISUALIZATIONWINDOW_H_ */
