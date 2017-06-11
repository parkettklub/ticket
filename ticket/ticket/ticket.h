#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ticket.h"

class ticket : public QMainWindow
{
	Q_OBJECT

public:
	ticket(QWidget *parent = Q_NULLPTR);

private:
	Ui::ticketClass ui;
};
