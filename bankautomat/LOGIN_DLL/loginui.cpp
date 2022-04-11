#include "loginui.h"
#include "ui_loginui.h"

LoginUi::LoginUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginUi)
{
    ui->setupUi(this);
}

LoginUi::~LoginUi()
{
    delete ui;
}

void LoginUi::on_pushButton_Ok_clicked()
{
    pin = ui->lineEdit->text();
}

void LoginUi::on_pushButton_Clear_clicked()
{
    ui->lineEdit->clear();
}

void LoginUi::on_pushButton_0_clicked()
{
    ui->lineEdit->setText("0");
}

void LoginUi::on_pushButton_1_clicked()
{
    ui->lineEdit->setText("1");
}

void LoginUi::on_pushButton_2_clicked()
{
    ui->lineEdit->setText("2");
}

void LoginUi::on_pushButton_3_clicked()
{
    ui->lineEdit->setText("3");
}

void LoginUi::on_pushButton_4_clicked()
{
    ui->lineEdit->setText("4");
}

void LoginUi::on_pushButton_5_clicked()
{
    ui->lineEdit->setText("5");
}

void LoginUi::on_pushButton_6_clicked()
{
    ui->lineEdit->setText("6");
}

void LoginUi::on_pushButton_7_clicked()
{
    ui->lineEdit->setText("7");
}

void LoginUi::on_pushButton_8_clicked()
{
    ui->lineEdit->setText("8");
}

void LoginUi::on_pushButton_9_clicked()
{
    ui->lineEdit->setText("9");
}

