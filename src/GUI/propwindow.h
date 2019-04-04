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

/**
 * @brief The PropLoadFail class
 * @details
 * Child of QEvent. A PropLoadFail of type (QEvent::Type) eventPLF,
 * will be sent by @class PropWindow to warn PropWindow's warnOnFail QWidget that
 * PropWindow has no more Prop to load and display.
 */
class PropLoadFail : public QEvent
{
public:
    explicit PropLoadFail();
    virtual ~PropLoadFail();

    static QEvent::Type type();
    static QEvent::Type eventPLF;
};

/**
 * @brief The PropWindow class QWidget for Prop display
 *
 * @details
 * @see Prop
 * @see MainWindow
 */
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
    std::unique_ptr<Prop> property; /**< Property being displayed.
@note because of @see Prop::load, @see Prop::save property is never replaced just updated @see @function nextProp*/
    QWidget* warnOnFail; /**< QWidget to receive PropLoadFail event in case of @see @function loadFailed */

protected slots:
    void showProp();
    void evaluateProp(kEval const& eval);

};

#endif // PROPWINDOW_H
