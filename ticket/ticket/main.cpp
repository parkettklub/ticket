#include "ticket.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ticket w;
	w.show();
	return a.exec();
}
