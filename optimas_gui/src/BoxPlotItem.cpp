#include "../headers/BoxPlotItem.h"


class BoxPlotItem::PrivateData{
	public:
		int attributes;
		QwtIntervalData data;
		QColor color;
		double reference;
};


BoxPlotItem::BoxPlotItem(const QwtText &title):QwtPlotItem(title){
	init();
}


BoxPlotItem::BoxPlotItem(const QString &title):QwtPlotItem(QwtText(title)){
	init();
}


BoxPlotItem::~BoxPlotItem(){
	delete d_data;
}


void BoxPlotItem::init(){

	d_data = new PrivateData();
	d_data->reference = 0.0;
	d_data->attributes = BoxPlotItem::Auto;

	setItemAttribute(QwtPlotItem::AutoScale, true);
	setItemAttribute(QwtPlotItem::Legend, true);

	setZ(20.0);
}


void BoxPlotItem::setBaseline(double reference){
	if( d_data->reference != reference ){
		d_data->reference = reference;
		itemChanged();
	}
}


double BoxPlotItem::baseline() const{
	return d_data->reference;
}


void BoxPlotItem::setData(const QwtIntervalData &data){
	d_data->data = data;
	itemChanged();
}

// return sup whisker extremity value.
double BoxPlotItem::compute_quartiles(QVector<double> v){ // this method has to work on a copy => no ref here.

	int size = v.size();
	
	qSort(v);	// sort the vector

	double min = v.front();
	double max = v.back();
	//cout<<size<<" "<<min<<" "<<max<<endl;
	vector<double> box_i(5,0);

	if (min==max) {box_i[0]=box_i[1]=min; all_boxes.push_back(box_i); return max;}

	// computing the 1st quartile q1, the median and the 3th quantile q3 values.
	int i_med = size/2;
	int i_q1 = i_med/2;
	int i_q3 = i_med + (size - i_med)/2;

	double median = v[i_med];
	if (size%2 == 0) median = (median + v[i_med-1])/2;

	double q1 =  v[i_q1];
	double q3 =  v[i_q3];

	if (i_med%2 == 0) {
		q1 = (q1 + v[i_q1-1])/2;
		q3 = (q3 + v[i_q3-1])/2;
	}
	if (size<4) {q1=min; q3=max;}
	//cout<<min<<" "<<q1<<" "<<median<<" "<<q3<<" "<<max<<endl;

	// all_boxes is an attributes of BoxPlotItem. It stores all what we need to draw each box and its whiskers.
	box_i[0]=min; box_i[1]=max;
	box_i[2]=(q1-min)/(max-min);
	box_i[3]=(median-min)/(max-min);
	box_i[4]=(q3-min)/(max-min);
	all_boxes.push_back(box_i);
	return max;
}


double BoxPlotItem::setData(const QMap<QString, QStringList> &m_map_score, const QMap<QString, QStringList> &m_map_grp, int num_qtl, double xunity){

	QwtArray<QwtDoubleInterval> box_intervals; // min max (whiskers extremities)
	QwtArray<double> x; // abscisse values (default is 1, 2, 3 etc).
	double xi = 1;

	double max_height=0;

	QMap<QString, QStringList>::const_iterator it_grp, it_ind;

	for(it_grp = m_map_grp.begin(); it_grp != m_map_grp.end(); it_grp++){	// for each group

		const QStringList & v_indiv = it_grp.value();
		int nind = v_indiv.size();

		QVector<double> grp_scores(nind); // stores each individual score belonging to this group at the pos i_qtl.

		for (int i_ind=0; i_ind<nind; i_ind++){ // fill grp_scores.

			it_ind = m_map_score.find(v_indiv[i_ind]);
			if(it_ind != m_map_score.end()){
				const QStringList & ind_data = it_ind.value();
				// grp_scores[i_ind] = ind_data[index_qtl+2].toDouble(); //// a quoi ca sert ??
				// All / Weight
				if( num_qtl < 2 ){
					// converting string to double
					grp_scores[i_ind] = ind_data[num_qtl + COL_MS - 1].toDouble();
				}
				// QTL1...QTLn
				else{
					grp_scores[i_ind] = ind_data[num_qtl + COL_FIRST_QTL - 3].toDouble();
				}
			}
		}

		double height = compute_quartiles(grp_scores);
		if (height>max_height) max_height = height;

		vector<double>& box_i = all_boxes.back();
		box_intervals.append( QwtDoubleInterval(box_i[0], box_i[1]));
		x.append(xi);
		xi += xunity;
	}

	setBoxPlotAttribute(BoxPlotItem::Xfy, true); // for vertical boxes.
	setData( QwtIntervalData(box_intervals, x ) ); // stores the extrem ordinate and abscisse values.

	return max_height;
}


double BoxPlotItem::setData(QMap<QString, QVector<double> > &m, double xunity){
	
	QwtArray<QwtDoubleInterval> box_intervals; // each interval: min max (whiskers extremities)
	QwtArray<double> x; // abscisse values (default is 1, 2, 3 etc).
	double xi = 1;

	double max_height=0;

	QMap<QString, QVector<double> >::const_iterator it;

	for(it = m.begin(); it != m.end(); ++it){	// for each group

		double height = compute_quartiles(it.value());
		
		if (height>max_height) max_height = height;

		vector<double>& box_i = all_boxes.back();
		box_intervals.append( QwtDoubleInterval(box_i[0], box_i[1])); // min max
		x.append(xi);
		xi += xunity;
	}

	setBoxPlotAttribute(BoxPlotItem::Xfy, true); // for vertical boxes.
	setData( QwtIntervalData(box_intervals, x ) ); // stores the extrem ordinate and abscisse values.

	return max_height;
}


const QwtIntervalData &BoxPlotItem::data() const{
	return d_data->data;
}


void BoxPlotItem::setColor(const QColor &color){
	if ( d_data->color != color ){
		d_data->color = color;
		itemChanged();
	}
}


QColor BoxPlotItem::color() const{
	return d_data->color;
}


QwtDoubleRect BoxPlotItem::boundingRect() const{

	QwtDoubleRect rect = d_data->data.boundingRect();

	if( !rect.isValid() )
        return rect;

	if ( d_data->attributes & Xfy ){
		rect = QwtDoubleRect( rect.y(), rect.x(), rect.height(), rect.width() );

		if ( rect.left() > d_data->reference )
			rect.setLeft( d_data->reference );
		else if ( rect.right() < d_data->reference )
			rect.setRight( d_data->reference );
	}
	else{
		if ( rect.bottom() < d_data->reference )
			rect.setBottom( d_data->reference );
		else if ( rect.top() > d_data->reference )
			rect.setTop( d_data->reference );
	}
	return rect;
}


int BoxPlotItem::rtti() const{
	return QwtPlotItem::Rtti_PlotUserItem;
}


void BoxPlotItem::setBoxPlotAttribute(BoxPlotAttribute attribute, bool on){
	if( bool(d_data->attributes & attribute) == on )
		return;

	if( on )
		d_data->attributes |= attribute;
	else
		d_data->attributes &= ~attribute;

	itemChanged();
}


bool BoxPlotItem::testBoxPlotAttribute(BoxPlotAttribute attribute) const{
	return d_data->attributes & attribute;
}


void BoxPlotItem::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const{
	painter->setPen(QPen(d_data->color));
	const QwtIntervalData &iData = d_data->data;

    const int x0 = xMap.transform(baseline());
    //const int y0 = yMap.transform(baseline());

	for (unsigned int i = 0; i < iData.size(); i++){	// for each point
		if ( d_data->attributes & BoxPlotItem::Xfy ){
			const int xi = xMap.transform(iData.value(i));	// from value to pixel

			double min = iData.interval(i).minValue();	// bottom of the caneva (value).
			double max = iData.interval(i).maxValue();	// top of the caneva (value).

			int ymin = yMap.transform(min);				// from value to pixel.
			int ymax = yMap.transform(max);				// from value to pixel.
			int yi = (ymin + ymax)/2;					// center of the caneva.

			int h = ymin - ymax;						// caneva's height
			int w = 0;									// caneva's width

			if(i<1)	w = ( xi -x0 )/4;	// 4 is arbitrary
			else	w = ( xi - xMap.transform(iData.value(i-1)) )/4;

			if(w<1) w=1;
			/*
			cout<<"xi "<<xi<<" yi "<<yi<<endl;
			cout<<"min "<<min<<" max "<<max<<endl;
			cout<<"ymin "<<ymin<<" ymax "<<ymax<<endl;
			cout<<"h "<<h<<" w "<<w<<endl;
			*/
			QRect rect;
			rect.setSize( QSize(w, (size_t)h) );
			rect.moveCenter(QPoint(xi, yi));
			drawBoxPlot(painter, Qt::Vertical, rect, i);// draw the current (ith) box and its whiskers within the caneva.
		}
		else {}	// horizontal boxes. to do
	}
}


void BoxPlotItem::drawBoxPlot(QPainter *painter, Qt::Orientation, const QRect& rect, const unsigned current) const{
	painter->save();
	const QRect r = rect.normalized();
	const QColor color(painter->pen().color());

	
	double q1_ratio = all_boxes[current][2];		// if q1_ratio == 0 => q1 == min.
	double med_ratio = all_boxes[current][3];
	double q3_ratio = all_boxes[current][4];		// if q3_ratio == 1 => q3 == max.
	
	// lighten or darken the selected color.
	const int factor = 125;
	const QColor light(color.light(factor));
	const QColor dark(color.dark(factor));

	//painter->setBrush(color);				// background color of the box.
	//painter->setPen(Qt::NoPen);			// no color for the pen.
	painter->setPen(QPen(light,1));			// thickness of the pen is 1 pixel.

	const int w2 = r.width() / 2;
	//const int h2 = r.height() / 2;
	const int q1 = r.height() * q1_ratio;	// q1 ratio
	const int med = r.height() * med_ratio;	// median ratio
	const int q3 = r.height() * q3_ratio;	// q3 ratio

	/*
	// drawLine trace une ligne entre un point A et un point B.
	// usage : QwtPainter::drawLine(painter, xA, yA, xB, yB);
	QwtPainter::drawLine(painter, r.left(), r.bottom(), r.right(), r.bottom());	// bas de la boite.
	QwtPainter::drawLine(painter, r.left(), r.top(), r.right(), r.top());		// haut de la boite.
	QwtPainter::drawLine(painter, r.left(), r.bottom(), r.left(), r.top());		// cote gauche.
	QwtPainter::drawLine(painter, r.right(), r.bottom(), r.right(), r.top());	// cote droit.
	*/
	
	if (q1_ratio != 0){ // => q1 != min. draw the min whisker.
		QwtPainter::drawLine(painter, r.left(), r.bottom()+1, r.right(), r.bottom()+1);		// inf whisker horizontal bar.
		QwtPainter::drawLine(painter, r.left()+w2, r.bottom()-q1+1, r.left()+w2, r.bottom()+1);	// inf whisker vertical bar.			
	}
	if ( q3_ratio != 1 && q3_ratio != 0 ){ // => q3 != max. draw the min whisker.
		QwtPainter::drawLine(painter, r.left(), r.top(), r.right(), r.top());			// sup whisker horizontal bar.
		QwtPainter::drawLine(painter, r.left()+w2, r.bottom()-q3, r.left()+w2, r.top());	// sup whisker vertical bar.
	}
	
	painter->setPen(QPen(light,2));	// thickness 2.
	// drawRect dessine les contours du caneva r (le perimetre r) et le remplit si painter->setBrush(color).

	QRect r2;	// rect of the box (without the whiskers).
	r2.setSize( QSize(r.width(), (size_t)(q1-q3+2)) );
	r2.moveCenter( QPoint(r.left()+w2-1, (r.bottom()-q1 + r.bottom()-q3 +1)/2) );
	QwtPainter::drawRect(painter, r2);										// draw the box.

	//QwtPainter::drawRect( painter, r.left()+w2, (r.bottom()*2 - q1 - q3)/2, r.width(), q3 - q1 );

	QwtPainter::drawLine(painter, r.left(), r.bottom() - med, r.right(), r.bottom() - med);	// draw the median.
	//QwtPainter::drawLine(painter, r.left()+1, r.bottom() - q1, r.right(), r.bottom() - q1);		// draw q1.
	//QwtPainter::drawLine(painter, r.left()+1, r.bottom() - q3, r.right(), r.bottom() - q3);		// draw q3.
	//QwtPainter::drawLine(painter, r.left()+1, r.bottom() - q1, r.left()+1, r.bottom() - q3);	// draw left side.
	//QwtPainter::drawLine(painter, r.right()+1, r.bottom() - q1, r.right()+1, r.bottom() - q3);	// draw right side.

	// NB :	les coordonnées des points sont en pixels => l'origine (0,0) est en haut a gauche.
	//		=> la valeur (en pixel) renvoyee par top() est inferieure a celle renvoyee par bottom().

	painter->restore();
}
