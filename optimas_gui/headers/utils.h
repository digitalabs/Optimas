#ifndef UTILS_H_
#define UTILS_H_

//#include <QDebug>
#include <QFontMetrics>
#include <string>
#include <QColor>
#include <QString>
#include <QLabel>
#include <QEvent>
#include <QStringList>
#include <QMap>
#include <QVector>
#include <vector>
#include <map>
#include <iostream>

#if defined (_WIN64) || defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#define __WINDOWS__
#endif


#ifdef __linux__
	#include <sys/stat.h>
	#include <gvc.h>
#elif defined(__WINDOWS__)
	#include <io.h>
	#include <gvc.h>
#elif defined(__APPLE__)
	#include <sys/stat.h>
	#include <gvc.h>
#endif


#define COL_ID 0
#define COL_P1 1
#define COL_P2 2
#define COL_GENE 3
#define COL_GROUP 4
#define COL_MS 5
#define COL_WEIGHT 6
#define COL_UC 7
#define COL_NO_FAV 8
#define COL_NO_UNFAV 9
#define COL_NO_HETERO 10
#define COL_NO_UNCERTAIN 11
#define COL_FIRST_QTL 12

// html "rich text" issues.
#define MIN_LINEHEIGHT "<font size='+2'>&nbsp;</font>"
#define MAJ_LINEHEIGHT "<font size='+4'>&nbsp;</font>"

#ifdef __APPLE__
	#define TEXT_SIZE "13pt"
	#define TITLE_SIZE "20pt"
#else
	#define TEXT_SIZE "9pt"
	#define TITLE_SIZE "14pt"
#endif

#ifdef GLOBAL
#define Global
#else
#define Global extern
#endif

Global int crit_index;


using namespace std;

////enum refColors {Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow};

typedef QMap<QString, QStringList>::iterator map_iterator;

bool customMoreThan(const QString & s1, const QString & s2);

bool criterionMoreThan(map_iterator & it1, map_iterator & it2);

void copyFolder(const QString & srcFolder, const QString & destFolder);

QColor operator/(const QColor & color, int n);

template <typename Container>
void stringtok(Container &container, string const &in, const char * const delimiters = " \t\n\r", int freq = -1) {

	const string::size_type len = in.length();
	string::size_type i = 0;
	int cpt = 0;

	while (i < len) {

		// Eat leading whitespace
		i = in.find_first_not_of(delimiters, i);

		if (i == string::npos) {
			return;   // Nothing left but white space
		}

		// Find the end of the token
		string::size_type j = in.find_first_of(delimiters, i);

		// Push token
		if (j == string::npos || cpt == freq) {
			container.push_back(in.substr(i));
			return;
		}
		else {
			container.push_back(in.substr(i, j-i));
			cpt++;
		}
		i = j + 1; // Set up for next loop
    }
}

// A class with a static method to get histogram reference colors even for a large number of histograms, and from everywhere in the code source.
// list_refColor size is 6 (RGB + CMJ). If needed, supplementary colors are computed starting from these 6 colors.
class refColors {
	private:
		static QList<QColor> list_refColor; // static attributes are declared outside the class (see utils.cpp).
	public:
		refColors();
		~refColors();
		static QColor getRefColor(int i);
};

////////////////////////////////////////////////////////////////////////
// custom QLabel for crosses labels :
// inherite QLabel
class OLabel: public QLabel {

	public:
		OLabel( QWidget * parent = 0, Qt::WindowFlags f = 0 ):QLabel(parent, f),full_text(""){}
		OLabel( const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0 ):QLabel(text, parent, f),full_text(text){}
		virtual ~OLabel(){}
		QString fullText() const;
	protected:
		virtual bool event(QEvent* event);
	private:
		QString full_text;

};

int maxIndex(std::vector<double> &a, int size_a);


#endif /* UTILS_H_ */
