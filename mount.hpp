#include <QCoreApplication>
#include <QObject>
#include <QThread>
class KeyBrdRdr: public QThread
{
  Q_OBJECT
  public:
    KeyBrdRdr(void);
    ~KeyBrdRdr(void);
    void run();
  signals:
    void KeyPressed(char);
};

class KeyBrdHndlr: public QObject
{
  Q_OBJECT
  public:
    KeyBrdHndlr(void);
    ~KeyBrdHndlr(void);
    QString strString;
    quint16 uiAttempts;
    KeyBrdRdr *kbUser;
  public slots:
    void OnKeyPressed(char);
};
