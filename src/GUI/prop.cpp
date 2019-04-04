#include "GUI/prop.h"

Prop::~Prop(){
}

void Prop::save() const {
    throw "save should be implemented by child class";
}
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
