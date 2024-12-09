#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    //virtual void showNewVFO(bool refreshText){};
    virtual void sMeterChanged(){};
    virtual void vfoChanged(){};
    virtual void graphChanged(){};


protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
