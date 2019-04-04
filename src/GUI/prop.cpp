#include "GUI/prop.h"

Prop::~Prop(){
}

/**
 * @brief Prop::save property
 * @details
 * Save this members.
 * @note must be overload by child
 */
void Prop::save() const {
    throw "save should be implemented by child class";
}

/**
  * @brief Prop::load the next property to this
  * @details
  * Load the next property and add the values to this members
  * @note must be overload by child
  * @warning do not save Prop. @see Prop::save
  */
bool Prop::load(){
    throw "load should be implemented by child class";
}

std::ostream& operator<<(std::ostream& os, Prop const & p){
    os <<"idprop: "<<p.idprop<<std::endl;
//    os <<"name: "<<p.name.toUtf8().constData()<<std::endl;
//    os <<"nDay: "<<p.nDay<<std::endl;
//    os <<"nextReview: "<<p.nextReview.toString().toUtf8().constData()<<std::endl;
    return os;
}
