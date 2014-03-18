#ifndef HISTOGRAMITEM_H_
#define HISTOGRAMITEM_H_

#include <qglobal.h>
#include <qcolor.h>
#include <qwt_scale_draw.h>
#include "qwt_plot_item.h"
#include <iostream>
#include <algorithm>

class QwtIntervalData;
class QString;

class HistogramItem: public QwtPlotItem {

	public:
		explicit HistogramItem(const QString &title = QString::null);
		explicit HistogramItem(const QwtText &title);
		virtual ~HistogramItem();

		void setData(const QwtIntervalData &data);
		const QwtIntervalData &data() const;

		void setColor(const QColor &);
		QColor color() const;

		virtual QwtDoubleRect boundingRect() const;

		virtual int rtti() const;

		virtual void draw(QPainter *, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const;

		void setBaseline(double reference);
		double baseline() const;

		enum HistogramAttribute {
			Auto = 0,
			Xfy = 1
		};

		void setHistogramAttribute(HistogramAttribute, bool on = true);
		bool testHistogramAttribute(HistogramAttribute) const;

	protected:
		virtual void drawBar(QPainter *, Qt::Orientation o, const QRect &) const;

	private:
		void init();

		class PrivateData;
		PrivateData *d_data;
};


// custom scaleDraw for x (bottom) axis of the plot
class MyScaleDraw: public QwtScaleDraw {

	public:
		MyScaleDraw(){}
		MyScaleDraw(const std::vector<double> &vect_legend) {
			m_vect_legend = &vect_legend;
			setTickLengths(0, 0, 0);
		}

		void setTickLengths(int min, int med, int maj){
			setTickLength(QwtScaleDiv::MinorTick, min);
			setTickLength(QwtScaleDiv::MediumTick, med);
			setTickLength(QwtScaleDiv::MajorTick, maj);
			if (min+med+maj == 0) enableComponent(QwtScaleDraw::Backbone, false);
		}
		// redefinition of QwtScaleDraw::label() virtual method.
		virtual QwtText label(double v) const {

			std::vector<double>::const_iterator it = lower_bound(m_vect_legend->begin(), m_vect_legend->end(), v);
			if(it == m_vect_legend->end() || *it != v) return QString("");
			if(it == m_vect_legend->begin()) return QString("All");
			return QString().setNum(int(it - m_vect_legend->begin()));
		}

	private:
		const std::vector<double>* m_vect_legend;
};


#endif /* HISTOGRAMITEM_H_ */
