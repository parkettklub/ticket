#include "ticket.h"

ticket::ticket(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	timer = new QTimer(this);
	timer2 = new QTimer(this);

	connect(ui.Felnott_button, &QPushButton::clicked, this, &ticket::felnott);
	connect(ui.Sch_button, &QPushButton::clicked, this, &ticket::kolis);
	connect(ui.Felvesz_button, &QPushButton::clicked, this, &ticket::felvesz);
	connect(ui.VIP_button, &QPushButton::clicked, this, &ticket::vip);
	connect(ui.Diak_button, &QPushButton::clicked, this, &ticket::diak);
	connect(ui.storno_button, &QPushButton::clicked, this, &ticket::storno);
	connect(ui.Scan_button, &QPushButton::clicked, this, &ticket::ennyit_fizetett);
	connect(ui.Kedvezmeny_button, &QPushButton::clicked, this, &ticket::kedvezmeny);
	connect(ui.config, &QPushButton::clicked, this, &ticket::make_combobox);
	connect(ui.Vip_box, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated),[=](const QString &text) { /* ... */ });
	connect(ui.Vip_box, static_cast<void(QComboBox::*)(const QString &text)>(&QComboBox::currentTextChanged), this, &ticket::set_felvesz_button_enabled);
	connect(timer, SIGNAL(timeout()), this, SLOT(stornoDisable()));
	connect(timer2, SIGNAL(timeout()), this, SLOT(set_time_label()));
	connect(ui.Spec_enable, &QPushButton::clicked, this, &ticket::enable_spec);
	connect(ui.OkButton, &QPushButton::clicked, this, &ticket::config);
}

int ticket::fizetett;
int ticket::minit;
int ticket::hh;
int ticket::jelszo;
int ticket::szam = 0;
double ticket::kedvezmeny_;
int ticket::kedvezmeny_felnott = 500;
int ticket::kedvezmeny_diak = 300;
int ticket::kedvezmeny_special;
int ticket::k;

int ticket::bevetel = 0;
int ticket::letszam = 0;

int ticket::diakjegy = 0;
//Diák jegy ára

int ticket::teljes = 0;
//Felnőtt jegy ára

int ticket::special = 0;

int ticket::teljes_later = 0;
//Ezzel a váltózóval állítom be a felnőttet 1500-ra
//A  beállítás ezt 1.5-re állítja, így a felnott*var 1000 ->1500 lesz



int ticket::diak_szam = 0;
//Ebben tárolom, hogy hány darab diák van

int ticket::koli_szam = 0;
//Ebben tárolom külön a kolis vendégek létszámát


int ticket::VIP_szam = 0;
//Ebben azt tárolom hány VIP vendég jött eddig


int ticket::Teljes_szam = 0;
//A változó neve megtévesztő lehet
//A felnőtt vendégek létszámát tárolom, azokét akik a BEÁLLÍT előtt jöttek


int ticket::f211 = 0;
int ticket::f21 = 0;
//felnőtt vendégek száma a BEÁLLÍT után

int ticket::nyitoegyenleg;

QString ticket::special_;


void ticket::felnott()
{
	ui.Felvesz_button->setEnabled(true);
	ui.konzol->clear();
	ui.Kedvezmeny_button->setEnabled(true);
	v.setvendeg("Felnőtt", teljes);
	ticket::enable_all();
	ui.Felnott_button->setDisabled(true);
	QString str = QString("%1 FT").arg(v.egyenleg);
	ui.Ar_Cimke->setText(str);
	ui.Vip_box->setCurrentText("-");
	ui.Vip_box->setDisabled(true);
}

void ticket::diak()
{
	ui.konzol->clear();
	ui.Felvesz_button->setEnabled(true);
	ui.Kedvezmeny_button->setEnabled(true);
	v.setvendeg("Diák", diakjegy);
	ticket::enable_all();
	ui.Diak_button->setDisabled(true);
	QString str = QString("%1 FT").arg(v.egyenleg);
	ui.Ar_Cimke->setText(str);
	ui.Vip_box->setCurrentText("-");
	ui.Vip_box->setDisabled(true);
}

void ticket::vip()
{
	ui.konzol->clear();
	ui.Kedvezmeny_button->setDisabled(true);
	ui.Felvesz_button->setDisabled(true);
	v.setvendeg("VIP", 0);
	ticket::enable_all();
	ui.VIP_button->setDisabled(true);
	QString str = QString("%1 FT").arg(v.egyenleg);
	ui.Ar_Cimke->setText(str);
	ui.Vip_box->setEnabled(true);
	//ui.Vip_box->setEditable(true);
}

void ticket::kolis()
{
	ui.konzol->clear();
	ui.Felvesz_button->setEnabled(true);
	ui.Kedvezmeny_button->setDisabled(true);
	ticket::enable_all();
	v.setvendeg("Kolis/ÁB/KB", 0);
	ui.Sch_button->setDisabled(true);
	QString str = QString("%1 FT").arg(v.egyenleg);
	ui.Ar_Cimke->setText(str);
	ui.Vip_box->setCurrentText("-");
	ui.Vip_box->setDisabled(true);
	
}

void ticket::felvesz()
{
	ui.Felvesz_button->setDisabled(true);
	index = ui.Vip_box->currentIndex();
	v.megjegyzes = ui.note->text();
	v.indok = ui.Vip_box->currentText();
	v.indok.remove("\n");
	//if (v.type == "VIP") ui.Vip_box->removeItem(index);
	vlist << v;
	ticket::enable_all();
	QString str = QString("%1 vendég sikeresen felvéve!\nEgyeneleg + %2\n").arg(v.type).arg(v.egyenleg);
	ui.konzol->insertPlainText(str);
	ui.Kedvezmeny_button->setText("KEDVEZMÉNY");
	ui.Ar_Cimke->setText("0 FT");
	if(v.type == "Felnőtt")
	{
		if (f21 == 1) f211++;
		else Teljes_szam++;
	}
	if (v.type == "Diák") diak_szam+=1;
	if (v.type == "Kolis/ÁB/KB") koli_szam++;
	if (v.type == "VIP") VIP_szam++;
	letszam++; bevetel += v.egyenleg;
	ticket::print_stat();
	ui.note->clear();
	ui.Vip_box->setCurrentText("-");
	ui.Vip_box->setDisabled(true);
	ui.visszajaro_cimke->setText("-");
	print_to_file();
	ui.Kedvezmeny_button->setDisabled(true);
	ui.storno_button->setEnabled(true);
	timer->start(15000);
	QString str2;
	str2 = QString("%1").arg(0);
	ui.time_label->setText(str2);
	timer2->start(1000);
}

void ticket::enable_all()
{
	ui.Felnott_button->setEnabled(true);
	ui.Diak_button->setEnabled(true);
	ui.VIP_button->setEnabled(true);
	ui.Sch_button->setEnabled(true);
	ui.note->clear();
	ui.Kedvezmeny_button->setText("NORMÁL");
	ui.prize_line->clear();
}

void ticket::ennyit_fizetett()
{
	QString str = ui.prize_line->text();
	int x = str.toInt();
	str = QString("%1").arg(x - v.egyenleg);
	ui.visszajaro_cimke->setText(str);
}

void ticket::kedvezmeny()
{
	if (ui.Kedvezmeny_button->text() == "NORMÁL")
	{
		k = 1;
		ui.Kedvezmeny_button->setText("KEDVEZMÉNY");
		ui.note->insert("Kedvezményes");
		if (v.type == "Felnőtt") v.setvendeg(kedvezmeny_felnott);
		if (v.type == "Diák") v.setvendeg(kedvezmeny_diak);
		if (v.type == "Speciális") v.setvendeg(kedvezmeny_special);
		QString str = QString("%1 FT").arg(v.egyenleg);
		ui.Ar_Cimke->setText(str);
		v.megjegyzes = ui.note->text();
	}
	else
	{
		k = 0;
		ui.note->clear();
		ui.Kedvezmeny_button->setText("NORMÁL");
		if (v.type == "Felnőtt") v.setvendeg(teljes);
		if (v.type == "Diák") v.setvendeg(diakjegy);
		if (v.type == "Speciális") v.setvendeg(special);
		QString str = QString("%1 FT").arg(v.egyenleg);
		ui.Ar_Cimke->setText(str);
		v.megjegyzes = ui.note->text();
	}
}

void ticket::make_combobox()
{
	if (szam == 0)
	{
		QFile QF("text.txt");
		if (!QF.open(QIODevice::ReadOnly | QIODevice::Text))
			return;

		while (!QF.atEnd()) {
			QString str = QF.readLine();
			QS << str;

		}

		//cb.addItems(QS);
		ui.Vip_box->addItems(QS);
	}
	szam = 1;
	set_all_disabled();
	config_enable();
	ui.line_s_normal->setDisabled(true);
	//ui.OkButton->setEnabled(true);
	ui.storno_button->setDisabled(true);
}

void ticket::set_felvesz_button_enabled()
{
	if (ui.Vip_box->currentText() != "-")
		ui.Felvesz_button->setEnabled(true);
	else ui.Felvesz_button->setDisabled(true);
	
}

void ticket::print_stat()
{
	QString str = QString("Teljes létszám: %1\nFelnőttek: %2\nFelnőttek %3:%4 utan: %5\nDiákok: %6\nVIP: %7\nKolis: %8\n").arg(letszam).arg(Teljes_szam).arg(tm.hour()).arg(tm.minute()).arg(f211).arg(diak_szam).arg(VIP_szam).arg(koli_szam);
	ui.stat_box->clear();
	ui.stat_box->insertPlainText(str);
}

void ticket::print_to_file()
{
	QFile file("out.xls");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);
	for (int i = 0; i < vlist.length(); i++)
	{
		out << vlist.at(i).type << "\t" <<
			vlist.at(i).egyenleg << "\t" <<
			vlist.at(i).megjegyzes << "\t" <<
			vlist.at(i).indok << "\t" <<
			vlist.at(i).ido << "\n";
	}

}

void ticket::storno()
{
	vendeg ve = vlist.at(vlist.length()-1);
	QString str = QString("Utolsó %1 vendég törölve!").arg(ve.type);
	ui.konzol->clear();
	ui.konzol->insertPlainText(str);
	if (ve.type == "Felnőtt")
	{
		if (f21 == 1) f211--;
		else Teljes_szam--;
	}
	if (ve.type == "Diák")
	{
		diak_szam--;
	}
	if (ve.type == "Kolis/ÁB/KB")
	{
		koli_szam--;
	}
	if (ve.type == "VIP")
	{
		VIP_szam--;
	}
	bevetel -= ve.egyenleg;
	letszam--;
	vlist.pop_back();
	ticket::print_stat();
	QString str2 = QString("%1").arg(0);
	ui.time_label->setText(str2);
	timer2->start(1000);
	ticket::print_to_file();
	ui.storno_button->setDisabled(true);
	
}

void ticket::stornoDisable()
{
	ui.storno_button->setDisabled(true);
}

void ticket::set_time_label()
{
	int x = ui.time_label->text().toInt();
	x++; 
	QString str = QString("%1").arg(x);
	ui.time_label->setText(str);
	ui.Vendeg_tipus->setText(vlist.at(vlist.length() - 1).type);
	timer2->start(1000);
}

void ticket::set_all_disabled()
{
	ui.Felnott_button->setDisabled(true);
	ui.Felvesz_button->setDisabled(true);
	ui.Diak_button->setDisabled(true);
	ui.VIP_button->setDisabled(true);
	ui.Kedvezmeny_button->setDisabled(true);
	ui.Special_button->setDisabled(true);
	ui.Special_button_2->setDisabled(true);
	//ui.Scan_button->setDisabled(true);
	ui.Sch_button->setDisabled(true);
	ui.Vip_box->setDisabled(true);
	//ui.note->setDisabled(true);
}

void ticket::config()
{
	teljes = ui.line_f_normal->text().toInt();
	teljes_later = ui.line_f_later->text().toInt();
	diakjegy = ui.line_d_normal->text().toInt();
	kedvezmeny_ = ui.line_kedvezmeny->text().toInt();
	kedvezmeny_felnott = (1-kedvezmeny_/100)*teljes;
	kedvezmeny_diak = diakjegy * (1-kedvezmeny_/100);
	if (ui.Spec_enable->isEnabled())
	{
		special = ui.line_s_normal->text().toInt();
		kedvezmeny_special = (1-kedvezmeny_/100)*special;
		ui.Special_button->setEnabled(true);
	}
	QTime tm1(ui.line_h->text().toInt(), ui.line_min->text().toInt(), 0);
	tm = tm1;
	jelszo = ui.lineEdit->text().toInt();
	ui.config->setDisabled(true);
	ui.Vip_box->setEditable(true);
	ui.Vip_box->setCurrentText("-");
	enable_all();
	config_disable();
}

void ticket::enable_spec()
{
	ui.line_s_normal->setEnabled(true);
}

void ticket::config_disable()
{
	ui.line_d_normal->setDisabled(true);
	ui.line_f_normal->setDisabled(true);
	ui.line_f_later->setDisabled(true);
	ui.line_d_normal->setDisabled(true);
	ui.line_s_normal->setDisabled(true);
	ui.line_h->setDisabled(true);
	ui.line_min->setDisabled(true);
	ui.lineEdit->clear();
	ui.lineEdit->setDisabled(true);
	ui.line_kedvezmeny->setDisabled(true);
	ui.Spec_enable->setDisabled(true);
	ui.OkButton->setDisabled(true);
	ui.Kassza_zar_button->setDisabled(true);
	//ui.config->setEnabled(true);
	ui.txt4->setEnabled(true);
	ui.br->setEnabled(true);
	ui.pushButton->setEnabled(true);
}

void ticket::config_enable()
{
	ui.line_d_normal->setEnabled(true);
	ui.line_f_normal->setEnabled(true);
	ui.line_f_later->setEnabled(true);
	ui.line_d_normal->setEnabled(true);
	//ui.line_s_normal->setEnabled(true);
	ui.line_h->setEnabled(true);
	ui.line_min->setEnabled(true);
	ui.lineEdit->setEnabled(true);
	ui.line_kedvezmeny->setEnabled(true);
	ui.Spec_enable->setEnabled(true);
	ui.OkButton->setEnabled(true);
	ui.Kassza_zar_button->setEnabled(true);
	ui.txt4->setDisabled(true);
	ui.br->setDisabled(true);
	ui.pushButton->setDisabled(true);
	ui.config->setDisabled(true);
}