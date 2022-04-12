#ifndef LOGIN_DLL_H
#define LOGIN_DLL_H

#include <QObject>
#include "LOGIN_DLL_global.h"
#include "login_engine.h"

class LOGIN_DLL_EXPORT LOGIN_DLL : public QObject
{
    Q_OBJECT
public:
    LOGIN_DLL();
    ~LOGIN_DLL();

private:
    LOGIN_ENGINE *pLOGIN_ENGINE;
    QString cardNumber;
    QByteArray myToken;

signals:
    void restartRFID(void);
    void sendCardNumberToLoginEngine(QString);
    void sendTokenToExe(QByteArray);

private slots:
    void loginFailed(void);
    void recvCardNumberFromExe(QString);
    void recvTokenFromEngine(QByteArray);

};

#endif // LOGIN_DLL_H
