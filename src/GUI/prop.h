#ifndef PROP_H
#define PROP_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <iostream>
#include <fstream>

class Prop  {
public:
    virtual  ~Prop();
    virtual void save() const;
    virtual bool load(); /**< @warning do not save Prop. @see Prop::save */

    friend std::ostream& operator<<(std::ostream& os, Prop const & p);

    unsigned int idprop;
    QString name;
    QString def;
    double nDay; /**< min number of day before next review. Shows prop difficulty. */
    QDateTime nextReview; /**< date before next review. */
};



#endif // PROP_H
