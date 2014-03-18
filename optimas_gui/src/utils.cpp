#include "../headers/utils.h"
#include <QDir>

using namespace std;


bool customMoreThan(const QString & s1, const QString & s2){

	// 4 is the position of the character immediately after the '=' character in the strings.
	//QStringRef rs1 = s1.midRef(4);
	return s1.midRef(4) > s2.midRef(4);
}


bool criterionMoreThan(map_iterator & it1, map_iterator & it2){
	// crit_index is a global integer.
	double v1 = it1.value()[crit_index].toDouble();
	double v2 = it2.value()[crit_index].toDouble();
	if (v1 == v2) return it1.key() > it2.key();
	return v1 > v2;
}


void copyFolder(const QString & srcFolder, const QString & destFolder){
	QDir sourceDir(srcFolder);

	if(!sourceDir.exists())	return;

	QDir destDir(destFolder);
	if(!destDir.exists()) destDir.mkdir(destFolder);

	QStringList files = sourceDir.entryList(QDir::Files);

	for(int i = 0; i< files.count(); i++){
		QString srcName = srcFolder + "/" + files[i];
		QString destName = destFolder + "/" + files[i];
		QFile::copy(srcName, destName);
	}
	files.clear();
	files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	for(int i = 0; i< files.count(); i++) {
		QString srcName = srcFolder + "/" + files[i];
		QString destName = destFolder + "/" + files[i];
		copyFolder(srcName, destName);
	}
}


/**------------------------------------------------------------------------------
	## RETURN: a new QColor object.
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ const QColor & color :
	@ int n :
	-----------------------------------------------------------------------------
	## SPECIFICATION: redefinition of operator "/".
	All the RGB component of a QColor object are divided by an integer.
	The new resulting QColor object is returned.
	-----------------------------------------------------------------------------
*/

QColor operator/(const QColor & color, int n){
	return QColor( color.red()/n, color.green()/n, color.blue()/n );
}


/**------------------------------------------------------------------------------
	## refColors class methods.
	-----------------------------------------------------------------------------
*/

QList<QColor> refColors::list_refColor;	// static attribute

refColors::refColors(){} // constructor.

refColors::~refColors(){} // destructor.

// get the ith reference color even for a large number of histograms, and from everywhere in the code source.
QColor refColors::getRefColor(int i) {
	if (i<0) { cerr<<"ERROR in refColors::getRefColor(int i): i < 0 !"<<endl; exit(1); }
	if (list_refColor.isEmpty()){
		list_refColor<<QColor(Qt::red)<<QColor(Qt::green)<<QColor(Qt::blue)<<QColor(Qt::cyan)<<QColor(Qt::magenta)<<QColor(Qt::yellow);
	}
	int nRefCol = list_refColor.size();
	return list_refColor[i%nRefCol]/(1+i/nRefCol);
}


/**------------------------------------------------------------------------------
	## OLabel class methods.
	-----------------------------------------------------------------------------
*/

bool OLabel::event(QEvent* e){
	if(e->type() == QEvent::Resize){
		QString info = text();
		if( info.contains( QChar((ushort)8230) )==false ) { // (ushort)8230 is the unicode for the '...' character.
			full_text = info;
		}
		if( !info.isEmpty() ){
			info = QFontMetrics(font()).elidedText(full_text, Qt::ElideRight, width()+1 );

			if(info.endsWith( QChar((ushort)8230)) ){
				setText(info);
				setToolTip(full_text);
			}
			else{
				setText(full_text);
				setToolTip("");
			}
		}
	}
	return QLabel::event(e);
}

QString OLabel::fullText() const{
	return full_text;
}


/**-----------------------------------------------------------------------------
	## FUNCTION:
	maxIndex(vector<double> &a, int size_a)
	-----------------------------------------------------------------------------
	## RETURN:
	int max_index: the index of the maximum value of the array
	-----------------------------------------------------------------------------
	## PARAMETERS:
	@ vector<double> &a : array of doubles
	@ int size_a : size of the array
	-----------------------------------------------------------------------------
	## SPECIFICATION:
	Returns the index of the maximum value in an array
	-----------------------------------------------------------------------------
*/

int maxIndex(vector<double> &a, int size_a){
    assert(size_a > 0);
    int maxIndex = 0;
    for(int i=1; i<size_a; i++){
        if( a[i] > a[maxIndex] ){
            maxIndex = i;
        }
    }
    return maxIndex;
}
