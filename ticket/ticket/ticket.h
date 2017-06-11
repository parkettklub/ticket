#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ticket.h"
//#include <time.h>
//#include <string.h>
//#include <stdio.h>
#include <QTime>
#include <QFile>
#include <QTimer>
#include <qtextstream.h>

class vendeg
{
	QString type;
	QString megjegyzes;
	QString indok;
	QString ido;
	int egyenleg;
	/*int sec;
	int min;
	int h;*/
public:
	void setvendeg(QString _type, int n)
	{
		type = _type;
		egyenleg = n;
		QTime time = QTime::currentTime();
		/*sec = time.second();
		min = time.minute();
		h = time.hour();*/
		ido = time.toString();
	}


	void setvendeg(int n)
	{
		egyenleg = n;
		QTime time = QTime::currentTime();
		ido = time.toString();
	}

	friend class ticket;
};


class ticket : public QMainWindow
{
	Q_OBJECT

public:
	ticket(QWidget *parent = Q_NULLPTR);

signals:
	void buttonsignal();
	void comboboxsignal();
	void timersignal();
	//void on_push_button_clicked();

public slots:
	void stornoDisable();
	void set_time_label();

private:
	Ui::ticketClass ui;
	vendeg v;
	QList<vendeg> vlist;
	//QComboBox cb;
	QStringList QS;
	int index;
	QTimer *timer, *timer2;
	QTime tm;


public:
	void felnott();
	void diak();
	void vip();
	void felvesz();
	void kolis();
	void enable_all();
	void ennyit_fizetett();
	void kedvezmeny();
	void appear();
	void print_stat();
	void set_felvesz_button_enabled(); 
	void print_to_file();
	void storno();
	void set_all_disabled();
	void config();
	void enable_spec();
	void config_disable();
	void config_enable();
	//void stornoDisable();

	void make_combobox();
	//void disable_all();

	static int minit;
	static int hh;
	static int jelszo;
	static int vjelszo;
	static int szam;
	static double kedvezmeny_;
	static QString special_;

	static int bevetel;
	static int letszam;
	static int diakjegy;
	static int teljes;
	static int special;

	static int teljes_later;
	static int diak_szam;
	static int koli_szam;
	static int VIP_szam;
	static int Teljes_szam;
	static int f211;
	static int f21;
	static int nyitoegyenleg;
	static int fizetett;
	static int kedvezmeny_felnott;
	static int kedvezmeny_diak;
	static int kedvezmeny_special;
	static int k;
};


