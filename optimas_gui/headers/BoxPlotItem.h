#ifndef BOXPLOTITEM_H_
#define BOXPLOTITEM_H_

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <QtCore>
#include <qglobal.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <QColor>
#include <qwt_scale_draw.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <QVector>
#include <QtAlgorithms>
#include <QString>
#include <QStringList>
#include <QPainter>
#include <qwt_plot.h>
#include <qwt_interval_data.h>
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <qwt_plot_item.h>

#include "utils.h"

using namespace std;

class QwtIntervalData;
class QString;

////////////////////////////////////////////////////////////////////////
class BoxPlotItem: public QwtPlotItem
{
public:
    explicit BoxPlotItem(const QString &title = QString::null);
    explicit BoxPlotItem(const QwtText &title);
    virtual ~BoxPlotItem();

	double compute_quartiles(QVector<double> v);
    void setData(const QwtIntervalData &data);
	double setData(QMap<QString, QVector<double> > &m, double xunity=1.);
	double setData(const QMap<QString, QStringList> &m_map_score, const QMap<QString, QStringList> &m_map_grp, int num_qtl, double xunity=1.);
    const QwtIntervalData &data() const;

    void setColor(const QColor &);
    QColor color() const;

    virtual QwtDoubleRect boundingRect() const;

    virtual int rtti() const;

    virtual void draw(QPainter *, const QwtScaleMap & xMap,
        const QwtScaleMap & yMap, const QRect &) const;

    void setBaseline(double reference);
    double baseline() const;

    enum BoxPlotAttribute
    {
        Auto = 0,
        Xfy = 1
    };

    void setBoxPlotAttribute(BoxPlotAttribute, bool on = true);
    bool testBoxPlotAttribute(BoxPlotAttribute) const;

protected:
    virtual void drawBoxPlot(QPainter *, Qt::Orientation o, const QRect &, const unsigned int current) const;
    vector< vector<double> > all_boxes;	// [[min max q1 q2 q3], [min max q1 q2 q3], ...]
											//			0					1
private:
    void init();

    class PrivateData;
    PrivateData *d_data;
};

////////////////////////////////////////////////////////////////////////
// custom scaleDraw for x (bottom) axis :
// inherite QwtScaleDraw
class CustomScaleDraw: public QwtScaleDraw {

	public:
		CustomScaleDraw(){}
		CustomScaleDraw(const QMap<QString, QStringList> & seq_legend, int dec=0) {	// utiliser les templates ?
			QMap<QString, QStringList>::const_iterator it;
			for (it=seq_legend.begin(); it!=seq_legend.end(); ++it) {
				m_vect_legend.push_back(it.key()); //// voir plus bas.
			}
			d=dec;
			//setTickLenths(0,0,0);
		}
		CustomScaleDraw(const QVector<QString> &vect_legend, int dec=0) {	// utiliser les templates ?
			for(int i=0; i<(int)vect_legend.size(); i++){
				m_vect_legend.push_back(vect_legend[i]); //// voir plus bas.
			}
			d=dec;
			//setTickLenths(0,0,0);
		}
		void setTickLengths(int min, int med, int maj){
			setTickLength(QwtScaleDiv::MinorTick, min);
			setTickLength(QwtScaleDiv::MediumTick, med);
			setTickLength(QwtScaleDiv::MajorTick, maj);
			if (min+med+maj == 0) enableComponent(QwtScaleDraw::Backbone, false);
		}
		// redefinition of QwtScaleDraw::label() virtual method.
		virtual QwtText label(double v) const {
			size_t s = m_vect_legend.size();
			int i = (int)v;
			if (s<1) return QwtScaleDraw::label(v);
			if ( (v != (double)i) || i<d || (size_t)i>=s+d) return QString("");
			return m_vect_legend[i-d];
		}

	private :
		QStringList m_vect_legend; //// la taille est connue => QVector<QString> est mieux.
		int d;
};


#endif /* BOXPLOTITEM_H_ */
