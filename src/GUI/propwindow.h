#ifndef PROPWINDOW_H
#define PROPWINDOW_H

#include <QWidget>
#include <QDate>
#include <memory>
#include <QEvent>

#include "dbconnection/dbprop.h"

namespace Ui {
class PropWindow;
}

class PropLoadFail : public QEvent
{
public:
    explicit PropLoadFail();
    virtual ~PropLoadFail();

    static QEvent::Type type();
    static QEvent::Type eventPLF;
};

class PropWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PropWindow(QWidget *parent = nullptr, QWidget *warnOnFail = nullptr);
    ~PropWindow();

    void nextProp();
protected:
    void loadFailed();
    enum struct kEval { perfect, good, hard, again };
    Ui::PropWindow *ui;
    std::unique_ptr<Prop> property;
    QWidget* warnOnFail; /**< Should a PropLoadFail (QEvent) be sent to parent() when load fail  */

protected slots:
    void showProp();
    void evaluateProp(kEval const& eval);

};

#endif // PROPWINDOW_H
