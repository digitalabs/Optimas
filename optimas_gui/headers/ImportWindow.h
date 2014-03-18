#ifndef IMPORTWINDOW_H_
#define IMPORTWINDOW_H_

#include <QtGui>
#include <QDialog>
#include <iostream>

#include "../ui_importWindow.h"
#include "utils.h"

class ImportWindow : public QDialog {
	Q_OBJECT

	public:
		ImportWindow( QWidget *parent = 0, const QString & lastInput = QString(), const QString & lastOutput = QString() );
		virtual ~ImportWindow();

		QString getMapLineEdit() const;
		QString getGenoLineEdit() const;
		QString getResultsLineEdit() const;
		void setResultsLineEdit(QString text);
		bool isAllRadioButtonChecked() const;
		bool isVerboseChecked() const;
		QString getTextHaplo() const;
		QString getTextGam() const;
		int getNumQtl() const;
		void setValueProgressBar(int value);
		void setNumQtlProgressBar(QString value);
		void setNumindivProgressBar(QString value);
		void setTotalQtlMksValues(QString value);
		void setRunPushButton(std::string label);
		QString getTextRunPushButton() const;
		QPushButton *getRunPushButton();
		QPushButton *getExampleRunPushButton();
		int getIndexExample() const;
		void setImportDisabled(bool boolean);
		void showCustomErrorMessage(std::string message);
		bool copyFile(QString input_file, QString output_file);
		bool isOK();

	private slots:
		void browseMap();
		void browseGeno();
		void browseRes();
		void selectQtl();
		void selectAllQtl();
		void helpMap();
		void helpGeno();
		void helpIndex();
		void helpRes();
		void helpAdvanced();
		void closeWindow();
		void swapExampleResume(int index);

	private:
		Ui::ImportWindow ui_import;
		QString m_lastInput;
		QString m_lastOutput;
};


#endif /* IMPORTWINDOW_H_ */
