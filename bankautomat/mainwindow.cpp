#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pRFID = new RFID_DLL;

    connect(this,SIGNAL(getNumber()),
            pRFID,SLOT(getCardNumberFromEngine(void)));

    connect(pRFID,SIGNAL(sendCardNumberToExe(QString)),
            this,SLOT(recvCardNumberFromDll(QString)));

    pLOGIN = new LOGIN_DLL;

    connect(this,SIGNAL(sendCardNumberToLogin(QString)),
            pLOGIN,SLOT(recvCardNumberFromExe(QString)));

    connect(pLOGIN,SIGNAL(sendTokenToExe(QByteArray)),
            this,SLOT(recvTokenFromLogin(QByteArray)));

    connect(pLOGIN,SIGNAL(restartRFID(void)),
            pRFID,SLOT(restartEngine(void)));

    connect(this,SIGNAL(loggedOutRestartEngine(void)),
            pRFID,SLOT(restartEngine(void)));

    pREST = new REST_DLL;

    connect(this, SIGNAL(getREST(QByteArray, QString, QString, QString)),
            pREST,SLOT(ExecuteRestOperation(QByteArray, QString, QString, QString)));

    connect(pREST,SIGNAL(sendResultToExe(QByteArray)),
            this,SLOT(recvResultsFromREST(QByteArray)));

    // This signal starts the process of reading the RFID-device
    emit getNumber();
}

MainWindow::~MainWindow()
{
    delete ui;
    ui = nullptr;

    delete pRFID;
    pRFID = nullptr;
    
    delete pLOGIN;
    pLOGIN = nullptr;

    delete pBankMain;
    pBankMain = nullptr;
    
    delete pREST;
    pREST = nullptr;

}

void MainWindow::restOps(int id)
{
//    qDebug() << id;
//    switch (id) {
//    case 1: // Get account owner name from databse
//        emit getREST(myToken, "GET", "cards/name/"+cardNumber, "");
//        connect(this,SIGNAL(sendRestResToBankmain(QByteArray)),
//                pBankMain,SLOT(setName(QByteArray)));
//        emit sendRestResToBankmain(restAnsw);
//        disconnect(this,SIGNAL(sendRestResToBankmain(QByteArray)),
//                pBankMain,SLOT(setName(QByteArray)));
//        qDebug() << "here?";
//        break;
//    case 2: // Get account balance
//        emit getREST(myToken, "GET", "cards/balance/"+cardNumber, "");
//        connect(this,SIGNAL(sendRestResToBankmain(QByteArray)),
//                pBankMain,SLOT(setBalance(QByteArray)));
//        emit sendRestResToBankmain(restAnsw);
//        disconnect(this,SIGNAL(sendRestResToBankmain(QByteArray)),
//                pBankMain,SLOT(setName(QByteArray)));
//        break;
//    }
}

void MainWindow::recvCardNumberFromDll(QString recvd)
{
    cardNumber = recvd;
    emit sendCardNumberToLogin(cardNumber);
}

void MainWindow::recvTokenFromLogin(QByteArray token)
{
    myToken = "Bearer " + token;
    qDebug() << "Token: " << myToken;
    this->close();
    bool loggedIn = false;
    if (!loggedIn) {
        pBankMain = new bankmain;
    };
    connect(pBankMain,SIGNAL(loggingOut(void)),
            this,SLOT(loggedOut(void)));

    connect(pBankMain,SIGNAL(updateBalance(void)),
            this,SLOT(getBalance(void)));

    connect(this,SIGNAL(sendRestResToBankmain(QByteArray)),
            pBankMain,SLOT(setName(QByteArray)));

    emit getREST(myToken, "GET", "cards/name/"+cardNumber, "");
    pBankMain->show();
}

void MainWindow::loggedOut()
{
    qDebug() << "Logged out";
//    delete pBankMain;
    pBankMain->close();
    this->show();
    emit loggedOutRestartEngine();
}

void MainWindow::on_exitApp_clicked()
{
    qDebug() << "Exiting bank application.";
    exit(0);
}

void MainWindow::recvResultsFromREST(QByteArray msg)
{
    restAnsw = msg;
    qDebug() << "Rest done, result: " << restAnsw;
    emit sendRestResToBankmain(restAnsw);
    //    ui->REST_results->setText(msg);
}

void MainWindow::getBalance()
{
    restOps(2);
}

void MainWindow::on_Button_rest_clicked()
{
    // Parametrit: Token, metodi tarkenne, body
    QString token = ui->lineEdit_Token->text();
    QString metodi = ui->lineEdit_metodi->text();
    QString tarkenne = ui->lineEdit_tarkenne->text();
    QString body = ui->lineEdit_body->text();

    if(token != "" && metodi != "" && tarkenne != "" && body != ""){
        emit getREST(myToken, metodi, tarkenne, body);
    }
    else{
        ui->REST_results->setText("Täytä joka kenttä ennen lähetystä!");
    }
}
