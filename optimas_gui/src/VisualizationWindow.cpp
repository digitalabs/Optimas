#include "../headers/VisualizationWindow.h"

using namespace std;


VisualizationWindow::VisualizationWindow(QWidget *parent) : QDialog(parent){
	ui_visualization.setupUi(this);

	// Action buttons (cancel)
	connect(ui_visualization.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));

	// Open the corresponding color dialog
	connect(ui_visualization.homoFavButton, SIGNAL(clicked()), this, SLOT(openHomoFavColorDialog()));
}


VisualizationWindow::VisualizationWindow(vector<QColor> &vect_color, vector<double> &vect_cut_off, QWidget *parent) : QDialog(parent){
	ui_visualization.setupUi(this);
	m_vect_color = &vect_color;
	m_vect_cut_off = &vect_cut_off;

	// Action buttons (cancel)
	connect(ui_visualization.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));

	// Open the corresponding color dialog
	connect(ui_visualization.homoFavButton, SIGNAL(clicked()), this, SLOT(openHomoFavColorDialog()));
	connect(ui_visualization.homoUnfavButton, SIGNAL(clicked()), this, SLOT(openHomoUnfavColorDialog()));
	connect(ui_visualization.heteroButton, SIGNAL(clicked()), this, SLOT(openHeteroColorDialog()));
	connect(ui_visualization.questionTagButton, SIGNAL(clicked()), this, SLOT(openQuestionTagColorDialog()));
	connect(ui_visualization.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(resetVisualizationWindow()));
	connect(ui_visualization.buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(onApply()));

	initVisualizationWindow();
}


VisualizationWindow::~VisualizationWindow() {

}


QPushButton *VisualizationWindow::getApplyButton(){
	return ui_visualization.buttonBox->button(QDialogButtonBox::Apply);
}


QColor VisualizationWindow::getHomoFavButtonColor(){
	return ui_visualization.homoFavButton->palette().button().color();
}


QColor VisualizationWindow::getHomoUnfavButtonColor(){
	return ui_visualization.homoUnfavButton->palette().button().color();
}


QColor VisualizationWindow::getHeteroButtonColor(){
	return ui_visualization.heteroButton->palette().button().color();
}


QColor VisualizationWindow::getQuestionTagButtonColor(){
	return ui_visualization.questionTagButton->palette().button().color();
}


double VisualizationWindow::getHomoFavValue(){
	return ui_visualization.homoFavDoubleSpinBox->value();
}


double VisualizationWindow::getHomoUnfavValue(){
	return ui_visualization.homoUnfavDoubleSpinBox->value();
}


double VisualizationWindow::getHeteroValue(){
	return ui_visualization.heteroDoubleSpinBox->value();
}


void VisualizationWindow::initVisualizationWindow(){
	if( ((int)m_vect_color->size() != 4) || ((int)m_vect_cut_off->size() != 3) ){
		fprintf(stderr, "Error in the length of the vector of parameters (visualization) !!\n");
		fflush(stderr);
		return;
	}

	QPalette palette1, palette2, palette3, palette4;

	palette1.setColor(QPalette::Button, m_vect_color->at(0));
	palette2.setColor(QPalette::Button, m_vect_color->at(1));
	palette3.setColor(QPalette::Button, m_vect_color->at(2));
	palette4.setColor(QPalette::Button, m_vect_color->at(3));

	// Color initialized
	ui_visualization.homoFavButton->setPalette(palette1);
	ui_visualization.homoUnfavButton->setPalette(palette2);
	ui_visualization.heteroButton->setPalette(palette3);
	ui_visualization.questionTagButton->setPalette(palette4);

	// Cut-off value initialized
	ui_visualization.homoFavDoubleSpinBox->setValue(m_vect_cut_off->at(0));
	ui_visualization.homoUnfavDoubleSpinBox->setValue(m_vect_cut_off->at(1));
	ui_visualization.heteroDoubleSpinBox->setValue(m_vect_cut_off->at(2));
}


void VisualizationWindow::onApply(){

	// New parameters information are stored
	m_vect_color->at(0) = getHomoFavButtonColor();
	m_vect_color->at(1) = getHomoUnfavButtonColor();
	m_vect_color->at(2) = getHeteroButtonColor();
	m_vect_color->at(3) = getQuestionTagButtonColor();

	m_vect_cut_off->at(0) = getHomoFavValue();
	m_vect_cut_off->at(1) = getHomoUnfavValue();
	m_vect_cut_off->at(2) = getHeteroValue();

	accept();
}


void VisualizationWindow::resetVisualizationWindow(){

	QPalette palette;

	palette.setColor(QPalette::Button, ui_visualization.buttonBox->button(QDialogButtonBox::Reset)->palette().button().color());

	// Color initialized
	ui_visualization.homoFavButton->setPalette(palette);
	ui_visualization.homoUnfavButton->setPalette(palette);
	ui_visualization.heteroButton->setPalette(palette);
	ui_visualization.questionTagButton->setPalette(palette);

	// Cut-off value initialized
	ui_visualization.homoFavDoubleSpinBox->setValue(0.75);
	ui_visualization.homoUnfavDoubleSpinBox->setValue(0.75);
	ui_visualization.heteroDoubleSpinBox->setValue(0.75);
}


void VisualizationWindow::openHomoFavColorDialog(){

	QColorDialog colorDial(ui_visualization.homoFavButton->palette().color(QPalette::Button), this);
	colorDial.setOption(QColorDialog::DontUseNativeDialog); // Error with Mac OSX

	// Open the color dialog pop-up
	if(colorDial.exec() == QDialog::Accepted){
		QColor homo_fav_color = colorDial.selectedColor(); // A new color has been chosen

		if( homo_fav_color.isValid () ){
			QPalette palette;
			palette.setColor(QPalette::Button, homo_fav_color);
			ui_visualization.homoFavButton->setPalette(palette);
		}
	}
}


void VisualizationWindow::openHomoUnfavColorDialog(){

	QColorDialog colorDial(ui_visualization.homoUnfavButton->palette().color(QPalette::Button), this);
	colorDial.setOption(QColorDialog::DontUseNativeDialog); // Error with Mac OSX

	// Open the color dialog pop-up
	if(colorDial.exec() == QDialog::Accepted){
		QColor homo_unfav_color = colorDial.selectedColor(); // A new color has been chosen

		if( homo_unfav_color.isValid () ){
			QPalette palette;
			palette.setColor(QPalette::Button, homo_unfav_color);
			ui_visualization.homoUnfavButton->setPalette(palette);
		}
	}
}


void VisualizationWindow::openHeteroColorDialog(){

	QColorDialog colorDial(ui_visualization.heteroButton->palette().color(QPalette::Button), this);
	colorDial.setOption(QColorDialog::DontUseNativeDialog); // Error with Mac OSX

	// Open the color dialog pop-up
	if(colorDial.exec() == QDialog::Accepted){
		QColor hetero_color = colorDial.selectedColor(); // A new color has been chosen

		if( hetero_color.isValid () ){
			QPalette palette;
			palette.setColor(QPalette::Button, hetero_color);
			ui_visualization.heteroButton->setPalette(palette);
		}
	}
}


void VisualizationWindow::openQuestionTagColorDialog(){

	QColorDialog colorDial(ui_visualization.questionTagButton->palette().color(QPalette::Button), this);
	colorDial.setOption(QColorDialog::DontUseNativeDialog); // Error with Mac OSX

	// Open the color dialog pop-up
	if(colorDial.exec() == QDialog::Accepted){
		QColor question_tag_color = colorDial.selectedColor(); // A new color has been chosen

		if( question_tag_color.isValid () ){
			QPalette palette;
			palette.setColor(QPalette::Button, question_tag_color);
			ui_visualization.questionTagButton->setPalette(palette);
		}
	}
}
