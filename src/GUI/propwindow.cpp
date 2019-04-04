#include <stdexcept>
#include <math.h>
#include <QDebug>
#include <QDate>
#include <memory>

#include "propwindow.h"
#include "ui_propwindow.h"
#include "dbconnection/dbprop.h"

// @todo read resetUi()

/************** PropLoadFail **********************/

PropLoadFail::PropLoadFail() :
    QEvent(PropLoadFail::type())
{}

PropLoadFail::~PropLoadFail()
{}

/**
 * @brief PropLoadFail::type set value of PropLoadFail::eventPLF if necessary (init)
 * @return PropLoadFail::eventPLF
 */
QEvent::Type PropLoadFail::type()
{
    if (PropLoadFail::eventPLF == QEvent::None)
    {
        int generatedType = QEvent::registerEventType();
        PropLoadFail::eventPLF = static_cast<QEvent::Type>(generatedType);
    }
    return PropLoadFail::eventPLF;
}

QEvent::Type PropLoadFail::eventPLF = QEvent::None;



/************** PropWindow **********************/

/**
 * @brief PropWindow::PropWindow
 * @param parent
 * @param warnOnFail
 */
PropWindow::PropWindow(QWidget *parent, QWidget *warnOnFail) :
    QWidget(parent),
    ui(new Ui::PropWindow),
    warnOnFail(warnOnFail)
{    
    property = std::unique_ptr<DBProp>(new DBProp()) ;

    ui->setupUi(this);
    connect( ui->show, SIGNAL(clicked()), this, SLOT(showProp()));
    connect( ui->perfect, &QPushButton::clicked, this,
             std::bind(&PropWindow::evaluateProp, this, kEval::perfect));
    connect( ui->good, &QPushButton::clicked, this,
             std::bind(&PropWindow::evaluateProp, this, kEval::good));
    connect( ui->hard, &QPushButton::clicked, this,
             std::bind(&PropWindow::evaluateProp, this, kEval::hard));
    connect( ui->again, &QPushButton::clicked, this,
             std::bind(&PropWindow::evaluateProp, this, kEval::again));

    nextProp();
}

/**
 * @brief Prop::showProp Replace show button by definition in GUI.
 */
void PropWindow::showProp() {
    ui->show->setVisible(false);
    ui->def->setVisible(true);
}

/**
 * @brief PropWindow::loadFailed send a PropLoadFail to warnOnFail
 */
void PropWindow::loadFailed() {
    this->hide();
    if (warnOnFail){
        QCoreApplication::postEvent(warnOnFail, new PropLoadFail(), Qt::NormalEventPriority);
    }
}

/**
 * @brief PropWindow::nextProp load and display the next prop
 * @details
 * On fail @see loadFailed
 * @see Prop::load
 */
void PropWindow::nextProp() {
    //@todo if show wasn't trigerred yet, show prop and let a small delay to see

    ui->name->setText("loading...");
    ui->show->setVisible(true);
    ui->def->setVisible(false);

    if (!property->load()) {
        loadFailed(); // if no more prop available
    }

    ui->name->setText(property->name);
    ui->def->setText(property->def);
}

/**
 * @brief PropWindow::evaluateProp slot for evaluation button
 * @details
 * Switch among eval to assign property's new nextReview and nDay. Then @see Prop::save and @see nextProp
 * @param eval
 * @todo more sofisticated and smoothed reassignement of property.nDay
 */
void PropWindow::evaluateProp(kEval const& eval) {
    // Define which policy is used to increase number of days.
    switch (eval) {
    case kEval::perfect:
        property->nDay = round((property->nDay+0.5)*1.5);
        break;
    case kEval::good:
        property->nDay = 3;
        break;
    case kEval::hard:
        property->nDay = 1;
        break;
    case kEval::again:
        property->nDay = 0;
        break;
    }

    // set nextReview to be in nDay
    QDateTime now = QDateTime::currentDateTime();
    QDateTime nextReview = now.addDays(static_cast<qint64>(property->nDay));
    property->nextReview = nextReview.addSecs(-1000); // remove 1000s to force again to reappear

    property->save();
    nextProp();
}

PropWindow::~PropWindow()
{
    delete ui;
}
