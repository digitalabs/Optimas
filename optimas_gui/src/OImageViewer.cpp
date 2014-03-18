/****************************************************************************
 **
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 **     the names of its contributors may be used to endorse or promote
 **     products derived from this software without specific prior written
 **     permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

 #include <QtGui>

 #include "../headers/OImageViewer.h"

 OImageViewer::OImageViewer(QWidget* zWidget){
	QList<QPushButton*> zoomButtons = zWidget->findChildren<QPushButton *>();
	if(!zoomButtons.isEmpty()){
	     zoomInButton		= zoomButtons[0];
	     zoomOutButton		= zoomButtons[1];
	     normalSizeButton	= zoomButtons[2];
	     fitToWindowButton	= zoomButtons[3];
	}

	 imageLabel = new QLabel;
     imageLabel->setBackgroundRole(QPalette::Base);
     imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
     imageLabel->setScaledContents(true);

     scrollArea = new QScrollArea;
     scrollArea->setBackgroundRole(QPalette::Dark);
     scrollArea->setWidget(imageLabel);
     //setCentralWidget(scrollArea);
 }


 void OImageViewer::clearImage(){
	 imageLabel->clear();
 }


 void OImageViewer::open(const QString & fileName){
     //QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
     if (!fileName.isEmpty()) {
         QImage image(fileName);
         if (image.isNull()) {
             QMessageBox::information(this, tr("Image Viewer"),
                                      tr("Cannot load %1.").arg(fileName));
             return;
         }
         imageLabel->setPixmap(QPixmap::fromImage(image));
         scaleFactor = 1.0;
/*
         printAct->setEnabled(true);
         fitToWindowAct->setEnabled(true);
         updateActions();

         if (!fitToWindowAct->isChecked())
             imageLabel->adjustSize();
*/
     }

     imageLabel->adjustSize();
 }

/*
 void OImageViewer::print()
 {
     Q_ASSERT(imageLabel->pixmap());
 #ifndef QT_NO_PRINTER
     QPrintDialog dialog(&printer, this);
     if (dialog.exec()) {
         QPainter painter(&printer);
         QRect rect = painter.viewport();
         QSize size = imageLabel->pixmap()->size();
         size.scale(rect.size(), Qt::KeepAspectRatio);
         painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
         painter.setWindow(imageLabel->pixmap()->rect());
         painter.drawPixmap(0, 0, *imageLabel->pixmap());
     }
 #endif
 }
*/

 void OImageViewer::zoomIn(){
     scaleImage(1.25);
 }

 void OImageViewer::zoomOut(){
     scaleImage(0.8);
 }

 void OImageViewer::normalSize(){
     imageLabel->adjustSize();
     scaleFactor = 1.0;
 }

 void OImageViewer::fitToWindow(){
     //bool fitToWindow = fitToWindowAct->isChecked();
	 bool fitToWindow = true;
     scrollArea->setWidgetResizable(fitToWindow);
     if (!fitToWindow) {
         normalSize();
     }
     //updateActions();
 }

/*
 void OImageViewer::createActions()
 {
     openAct = new QAction(tr("&Open..."), this);
     openAct->setShortcut(tr("Ctrl+O"));
     connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

     printAct = new QAction(tr("&Print..."), this);
     printAct->setShortcut(tr("Ctrl+P"));
     printAct->setEnabled(false);
     connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

     exitAct = new QAction(tr("E&xit"), this);
     exitAct->setShortcut(tr("Ctrl+Q"));
     connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

     zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
     zoomInAct->setShortcut(tr("Ctrl++"));
     zoomInAct->setEnabled(false);
     connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

     zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
     zoomOutAct->setShortcut(tr("Ctrl+-"));
     zoomOutAct->setEnabled(false);
     connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

     normalSizeAct = new QAction(tr("&Normal Size"), this);
     normalSizeAct->setShortcut(tr("Ctrl+S"));
     normalSizeAct->setEnabled(false);
     connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

     fitToWindowAct = new QAction(tr("&Fit to Window"), this);
     fitToWindowAct->setEnabled(false);
     fitToWindowAct->setCheckable(true);
     fitToWindowAct->setShortcut(tr("Ctrl+F"));
     connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

     aboutAct = new QAction(tr("&About"), this);
     connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

     aboutQtAct = new QAction(tr("About &Qt"), this);
     connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
 }

 void OImageViewer::createMenus()
 {
     fileMenu = new QMenu(tr("&File"), this);
     fileMenu->addAction(openAct);
     fileMenu->addAction(printAct);
     fileMenu->addSeparator();
     fileMenu->addAction(exitAct);

     viewMenu = new QMenu(tr("&View"), this);
     viewMenu->addAction(zoomInAct);
     viewMenu->addAction(zoomOutAct);
     viewMenu->addAction(normalSizeAct);
     viewMenu->addSeparator();
     viewMenu->addAction(fitToWindowAct);

     helpMenu = new QMenu(tr("&Help"), this);
     helpMenu->addAction(aboutAct);
     helpMenu->addAction(aboutQtAct);

     menuBar()->addMenu(fileMenu);
     menuBar()->addMenu(viewMenu);
     menuBar()->addMenu(helpMenu);
 }

 void OImageViewer::updateActions()
 {
     zoomInAct->setEnabled(!fitToWindowAct->isChecked());
     zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
     normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
 }
*/
 void OImageViewer::scaleImage(double factor){
     Q_ASSERT(imageLabel->pixmap());
     scaleFactor *= factor;
     imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

     adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
     adjustScrollBar(scrollArea->verticalScrollBar(), factor);

     zoomInButton->setEnabled(scaleFactor < 3.0);
     zoomOutButton->setEnabled(scaleFactor > 0.333);
 }

 void OImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor){
     scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
 }
