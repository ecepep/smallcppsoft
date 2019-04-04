#ifndef PROP_H
#define PROP_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <iostream>
#include <fstream>

/**
 * @brief The Prop class define content of property
 * @details
 * Prop stand for property which is composed of an idprop, the content to be displayed (name, def),
 * the info about the next time to fetch it (nDay, nextReview)
 */
struct Prop  {
    virtual  ~Prop();
    virtual void save() const;
    virtual bool load();

    friend std::ostream& operator<<(std::ostream& os, Prop const & p);

    unsigned int idprop;
    QString name;
    QString def;
    double nDay; /**< min number of day before next review. Shows prop difficulty. */
    QDateTime nextReview; /**< date before next review. */
};



#endif // PROP_H
