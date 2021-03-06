#include "drawmoney.h"
#include "ui_drawmoney.h"

drawmoney::drawmoney(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::drawmoney)
{
    ui->setupUi(this);
    amount = 0;
    cAmount = 0;
    ui->customAmountLineEdit->setValidator(new QIntValidator(0, 10000, this));
    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(20000);
    connect(timer,SIGNAL(timeout()),
            this,SLOT(timeout()));
    timer->start();
}

drawmoney::~drawmoney()
{
    delete ui;
    ui = nullptr;
}

QString drawmoney::moneyHandler(int money)
{
    amount = ui->drawMoneyLineEdit->text().toInt();
    amount += money;
    return QString::number(amount);
}

void drawmoney::timeout()
{
    qDebug() << "Draw money timeout";
    this->close();
}

void drawmoney::resetTimer()
{
 //   qDebug() << "Bankmain timer stopped. Time: " << timer->remainingTime();
    timer->stop();
    timer->start();
 //   qDebug() << "Bankmain timer restarted. Time: " << timer->remainingTime();
}

void drawmoney::closeEvent(QCloseEvent *)
{
    if (timer->isActive()) {
        timer->stop();
    }
    timer->deleteLater();
    emit updateBalance();
    emit startBankmainTimer();
    this->deleteLater();
}

void drawmoney::negativeBal(QString bal)
{
    ui->drawMoneyLineEdit->setPlaceholderText(bal);
}

void drawmoney::on_closeButton_clicked()
{
    this->close();
}

void drawmoney::on_drawButton_clicked()
{
    resetTimer();
    drawThis = ui->drawMoneyLineEdit->text();
    if (drawThis == "") {
        ui->drawMoneyLineEdit->setPlaceholderText("Can't draw 0");
    } else {
        ui->drawMoneyLineEdit->clear();
        ui->drawMoneyLineEdit->setPlaceholderText("Insert amount");
        ui->customAmountLineEdit->clear();
        qDebug() << "Draw this amout: " << drawThis;
        emit drawThisAmount(drawThis);
    }
}

void drawmoney::on_amountCustomButton_clicked()
{
    resetTimer();
    cAmount = ui->customAmountLineEdit->text().toInt();
    ui->drawMoneyLineEdit->setText(moneyHandler(cAmount));
}

void drawmoney::on_amount20Button_clicked()
{
    resetTimer();
    ui->drawMoneyLineEdit->setText(moneyHandler(20));
}

void drawmoney::on_amount40Button_clicked()
{
    resetTimer();
    ui->drawMoneyLineEdit->setText(moneyHandler(40));
}

void drawmoney::on_amount60Button_clicked()
{
    resetTimer();
    ui->drawMoneyLineEdit->setText(moneyHandler(60));
}

void drawmoney::on_amount80Button_clicked()
{
    resetTimer();
    ui->drawMoneyLineEdit->setText(moneyHandler(80));
}

void drawmoney::on_amount100Button_clicked()
{
    resetTimer();
    ui->drawMoneyLineEdit->setText(moneyHandler(100));
}

void drawmoney::on_amount200Button_clicked()
{
    resetTimer();
    ui->drawMoneyLineEdit->setText(moneyHandler(200));
}

void drawmoney::on_amount500Button_clicked()
{
    resetTimer();
    ui->drawMoneyLineEdit->setText(moneyHandler(500));
}
