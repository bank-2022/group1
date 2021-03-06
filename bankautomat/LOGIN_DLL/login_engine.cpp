#include "login_engine.h"

LOGIN_ENGINE::LOGIN_ENGINE(QObject *parent) : QObject(parent)
{
    tries = 3;
    pLOGIN_UI = new LoginUi();
    connect(pLOGIN_UI,SIGNAL(sendPinToEngine(QString)),
            this,SLOT(recvPin(QString)));
    connect(this,SIGNAL(startAuth(QString)),
            this,SLOT(tokenReq(QString)));
    connect(this,SIGNAL(wrongPinMsg(QString)),
            pLOGIN_UI,SLOT(wrongPin(QString)));
    connect(pLOGIN_UI,SIGNAL(aboutToQuit(void)),
            this,SLOT(rejected(void)));
    connect(this,SIGNAL(beginTimer(void)),
            pLOGIN_UI,SLOT(startTimer(void)));
    connect(this,SIGNAL(killTimer(void)),
            pLOGIN_UI,SLOT(stopTimer(void)));
    connect(this,SIGNAL(resetTimer(void)),
            pLOGIN_UI,SLOT(resetTimer(void)));
    connect(this,SIGNAL(cardLock(QString)),
            this,SLOT(cardLockHandler(QString)));
    connect(this,SIGNAL(askForDebitOrCredit(QString)),
            this,SLOT(askDebitOrCredit(QString)));

    pDebitCredit = new debitCreditWindow();
    connect(pDebitCredit,SIGNAL(sendCardType(QString)),
            this,SLOT(recvCardType(QString)));
    connect(pDebitCredit,SIGNAL(aboutToClose(void)),
            this,SLOT(creditWinClosed(void)));
}

LOGIN_ENGINE::~LOGIN_ENGINE()
{
    delete pLOGIN_UI;
    pLOGIN_UI = nullptr;

    delete pDebitCredit;
    pDebitCredit = nullptr;
}

void LOGIN_ENGINE::askDebitOrCredit(QString credit)
{
    if (credit == "1") {
        pDebitCredit->show();
    } else if (credit == "0") {
        loginSuccesful = true;
        pLOGIN_UI->close();
        emit sendTokenToLogin(myToken, "debit");
    } else {
        qDebug() << "Something went wrong.";
    }
}

void LOGIN_ENGINE::checkForCredit(QString method)
{
    manager = new QNetworkAccessManager();
    QObject::connect(manager, &QNetworkAccessManager::finished,
                     this, [=](QNetworkReply *reply) {
        if (reply->error()) {
            QString answer = reply->errorString();
            qDebug() << "error" << reply->errorString();
            return;
        }
        QByteArray answer=reply->readAll();
        qDebug() << "Answer: " << answer;
        if (method == "accID") {
            QJsonDocument json_doc = QJsonDocument::fromJson(answer);
            QJsonObject json_obj = json_doc.object();
            accountId = QString::number(json_obj["accounts_account_id"].toInt());
            qDebug() << "Account ID: " << accountId;
            manager->deleteLater();
            reply->deleteLater();
            checkForCredit("credit");
        } else if (method == "credit") {
            QJsonDocument json_doc = QJsonDocument::fromJson(answer);
            QJsonObject json_obj = json_doc.object();
            QString credit = QString::number(json_obj["credit"].toInt());
//            qDebug() << "Card type: " << credit;
            manager->deleteLater();
            reply->deleteLater();
            emit askForDebitOrCredit(credit);
        }
    });
    if (method == "accID") {
        qDebug() << "Getting account ID";
        QNetworkRequest request(("http://banksimul-api.herokuapp.com/cards/accountId/"+cardNumber));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader(QByteArray("Authorization"),("Bearer " + myToken));
        manager->get(request);
    } else if (method == "credit") {
        qDebug() << "Getting card type";
        QNetworkRequest request(("http://banksimul-api.herokuapp.com/accounts/credit/"+accountId));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader(QByteArray("Authorization"),("Bearer " + myToken));
        manager->get(request);
    }
}

void LOGIN_ENGINE::recvCardType(QString type)
{
    pDebitCredit->close();
    loginSuccesful = true;
    pLOGIN_UI->close();
    emit sendTokenToLogin(myToken, type);
}

void LOGIN_ENGINE::recvPin(QString code)
{
    emit startAuth(code);
}

void LOGIN_ENGINE::recvCardNumber(QString num)
{
    cardNumber = num;
    loginSuccesful = false;
    cardLockHandler("status");
}

void LOGIN_ENGINE::tokenReq(QString pin)
{
    QString site_url = "http://banksimul-api.herokuapp.com/login/";
    QNetworkRequest request((site_url));
    QJsonObject jsonObj;
    jsonObj.insert("card_number", cardNumber);
    jsonObj.insert("pin_code", pin);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    manager = new QNetworkAccessManager(this);

    connect(manager,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(tokenRes(QNetworkReply*)));

    reply = manager->post(request, QJsonDocument(jsonObj).toJson());
}

void LOGIN_ENGINE::tokenRes(QNetworkReply *reply)
{
    myToken=reply->readAll();

    if (myToken != "false") {
        tries = 3;
        qDebug() << "Correct pin.";
        reply->deleteLater();
        manager->deleteLater();
        emit wrongPinMsg("Enter 4 digit pin.");
        checkForCredit("accID");
    } else {
        tries--;
        qDebug() << "Incorrect pin.";
        QString s = QString::number(tries);
        QString msg = "Incorrect pin, "+ s +" tries left";
        if (tries > 0) {
            emit resetTimer();
            emit wrongPinMsg(msg);
        } else {
            qDebug() << "Locking card.";
            pLOGIN_UI->close();
            reply->deleteLater();
            manager->deleteLater();
            cardLockHandler("lock");
        }
    }
}

void LOGIN_ENGINE::cardLockHandler(QString method)
{
    manager = new QNetworkAccessManager();
    QObject::connect(manager, &QNetworkAccessManager::finished,
                     this, [=](QNetworkReply *reply) {
        if (reply->error()) {
            QString answer = reply->errorString();
            qDebug() << "error" << reply->errorString();
            return;
        }
        QByteArray answer=reply->readAll();
        qDebug() << answer;
        if (method == "status") {
            QJsonDocument json_doc = QJsonDocument::fromJson(answer);
            QJsonObject json_obj = json_doc.object();
            QString res = QString::number(json_obj["locked"].toInt());
//            qDebug() << res;
            manager->deleteLater();
            reply->deleteLater();
            if (res == "0") {
                pLOGIN_UI->show();
                emit beginTimer();
            } else if (res == "1"){
                qDebug() << "Card is locked.";
                emit wrongPinMsg("Enter 4 digit pin.");
                emit loginFailedInEngine();
            } else {
                qDebug() << "Can't determine locked status.";
            }
        }
    });
    if (method == "lock") {
        QNetworkRequest request(("http://banksimul-api.herokuapp.com/lock/"+cardNumber));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QJsonObject jsonObj;
        jsonObj.insert("locked", 1);
        qDebug() << jsonObj;
        manager->put(request, QJsonDocument(jsonObj).toJson());
    } else if (method == "status") {
        QNetworkRequest request(("http://banksimul-api.herokuapp.com/lock/status/"+cardNumber));
        manager->get(request);
    }
}

void LOGIN_ENGINE::rejected()
{
    qDebug() << "Login closed.";
    tries = 3;
    emit killTimer();
    emit wrongPinMsg("Enter 4 digit pin.");
    if (pDebitCredit->isActiveWindow()) {
        pDebitCredit->close();
    }
    if (!loginSuccesful) {
        emit loginFailedInEngine();
    }
}

void LOGIN_ENGINE::creditWinClosed()
{
    qDebug() << "Credit/Debit closed.";
    if (pLOGIN_UI->isActiveWindow()) {
        pLOGIN_UI->close();
    }
}
