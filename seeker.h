#ifndef SEEKER_H
#define SEEKER_H

#include <QObject>

class Seeker : public QObject
{
	Q_OBJECT
public:
	explicit Seeker(QObject *parent = 0);
	
	void search();
signals:
	
public slots:
	
};

#endif // SEEKER_H
