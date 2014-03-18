#include "../headers/HistoColorWindow.h"

using namespace std;


HistoColorWindow::HistoColorWindow(QWidget *parent) : QDialog(parent){
	ui.setupUi(this);
}


HistoColorWindow::HistoColorWindow(QMap<QString, QColor>& map_cycleColor, const QStringList& vect_generation, QWidget *parent) : QDialog(parent){
	ui.setupUi(this);
	initHistoColorWindow(map_cycleColor, vect_generation);
	m_map_cycleColor_ptr = &map_cycleColor;

	// Action buttons
	connect(ui.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(resetDefaultColors()));
	connect(ui.buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(applyColor()));

	for (int i=0; i<(int)m_cycleColorButtons.size(); ++i){
		// Open the corresponding color dialog
		connect(m_cycleColorButtons[i], SIGNAL(clicked()), this, SLOT(openCycleColorDialog()));
	}
}


HistoColorWindow::~HistoColorWindow() {}


void HistoColorWindow::initHistoColorWindow(const QMap<QString, QColor>& map_cycleColor, const QStringList& vect_generation){

	int numRow = 0, buttonCol = 1, spacerCol = 2;
	QMap<QString, QColor>::const_iterator it;
	m_cycleLabels.resize(map_cycleColor.size());
	m_cycleColorButtons.resize(map_cycleColor.size());
	m_hSpacers.resize(map_cycleColor.size());

	for(int i=1; i<vect_generation.size(); ++i){ // we start to i = 1 to skip "All" cycle.

		it = map_cycleColor.find(vect_generation[i]);

		m_cycleLabels[numRow] = new QLabel(ui.groupBox);
		m_cycleLabels[numRow]->setObjectName(QString::fromUtf8("cycleLabel"));
		//m_cycleLabels[numRow]->setMinimumSize(QSize(60, 29));
		m_cycleLabels[numRow]->setMinimumSize(QSize(0, 0));
		m_cycleLabels[numRow]->setMaximumSize(QSize(16777215, 16777215));
		m_cycleLabels[numRow]->setText(it.key());
		m_cycleLabels[numRow]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
		ui.gridLayout->addWidget(m_cycleLabels[numRow], numRow, 0, 1, 1);

		m_cycleColorButtons[numRow] = new QPushButton(ui.groupBox);
		m_cycleColorButtons[numRow]->setObjectName(QString::fromUtf8("cycleColorButton"));
		m_cycleColorButtons[numRow]->setMinimumSize(QSize(0, 0));
		m_cycleColorButtons[numRow]->setMaximumSize(QSize(16777215, 16777215));
		QPalette palette;
		palette.setColor(QPalette::Button, it.value());
		m_cycleColorButtons[numRow]->setPalette(palette);
		ui.gridLayout->addWidget(m_cycleColorButtons[numRow], numRow, buttonCol, 1, 1);

		m_hSpacers[numRow] = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		ui.gridLayout->addItem(m_hSpacers[numRow], numRow, spacerCol, 1, 1);


		//QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		ui.gridLayout->addItem(spacer, numRow, buttonCol+1, 1, 1);

		numRow++;
	}
}


void HistoColorWindow::resetDefaultColors(){
	for (int i = 0; i<(int)m_cycleLabels.size(); ++i){
		m_cycleColorButtons[i]->setPalette(QPalette(refColors::getRefColor(i)));
	}
}


void HistoColorWindow::applyColor(){
	for (int i = 0; i<(int)m_cycleLabels.size(); ++i){
		m_map_cycleColor_ptr->find(m_cycleLabels[i]->text()).value() = m_cycleColorButtons[i]->palette().color(QPalette::Button);
	}
	accept();
}


void HistoColorWindow::openCycleColorDialog(){

	QPushButton *button = (QPushButton*)sender(); // the cycle button that send the signal clicked();

	QColor cycle_color = QColorDialog::getColor(button->palette().color(QPalette::Button), this);

	if( cycle_color.isValid () ) button->setPalette(QPalette(cycle_color));
}
