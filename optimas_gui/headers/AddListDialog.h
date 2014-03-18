#ifndef ADDLISTDIALOG_H_
#define ADDLISTDIALOG_H_

#include <QtGui>
#include <QFileDialog>
#include <QFont>


class AddListDialog : public QDialog {
	Q_OBJECT

	private:
		QStringList* list_selection_ptr;

	public:
		AddListDialog(QStringList & list_selection, QWidget *parent = 0);

		QVBoxLayout *addListDialogVLayout;
		QListWidget *addListWidget;
		QDialogButtonBox *addListButtonBox;

	public slots:
		void onItemChanged(QListWidgetItem*, QListWidgetItem*);
		void okClicked();

	signals:
		void okAccepted(int, const QString &);
};


class ExportGraphDialog : public QFileDialog {
	Q_OBJECT

	public:
		ExportGraphDialog(QWidget* parent, Qt::WindowFlags flags);
		ExportGraphDialog(QWidget* parent = 0, const QString & caption = QString(), const QString & directory = QString(), const QString & filter = QString() );
		QString selectedResolution() const;
	private:
		QLabel* resLabel;
		QComboBox* resComboBox;
};

#endif /* ADDLISTDIALOG_H_ */
