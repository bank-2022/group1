#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pRFID = new RFID_DLL;
    pREST2 = new DLL_REST_2;

    connect(this,SIGNAL(getNumber()),
            pRFID,SLOT(getCardNumberFromEngine()));

    connect(pRFID,SIGNAL(sendCardNumberToExe(QString)),
            this,SLOT(recvCardNumberFromDll(QString)));

    connect(this, SIGNAL(getTransfers(int, QString)),
            pREST2,SLOT(getData(int, QString)));
    connect(pREST2,SIGNAL(sendTransfersToExe(QString)),
            this,SLOT(recvTransfersFromDll(QString)));

    // This signal starts the process of reading the RFID-device
    emit getNumber();
}

MainWindow::~MainWindow()
{
    delete ui;
    ui = nullptr;
    delete pRFID;
    delete pREST2;
    pRFID = nullptr;
    pREST2 = nullptr;
}

void MainWindow::recvCardNumberFromDll(QString recvd)
{
    // Contains the verified card number
    cardNumber = recvd;

    // Displaying the card number for debugging purposes, not needed in final product
    ui->label_2->setText(cardNumber);
}

void MainWindow::recvTransfersFromDll(QString msg)
{
    ui->transfersEdit->setText(msg);
}

void MainWindow::on_transfersButton_clicked()
{
    QString x = ui->transferID->text();
    QString token = ui->tokenEdit->text();
    if(x != "" && token != ""){
        emit getTransfers(x.toInt(), token);
    }
    else{
        ui->transferID->setText("Kirjoita account id");
        ui->tokenEdit->setText("Kirjoita token");
    }
}

