#ifndef SCREEN1PRESENTER_HPP
#define SCREEN1PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen1View;

class Screen1Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen1Presenter(Screen1View& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~Screen1Presenter() {}

    //from model
    virtual void sMeterChanged();
    virtual void vfoChanged();
    virtual void graphChanged();
    //from view
    virtual void lsbusbToggled(bool newState);
    virtual void rxtxToggled(bool newState);
    virtual void bandSelected(uint8_t newState);
    virtual void stepSizeSelected(uint16_t newState);
    virtual void graphPresenter(uint8_t graphDone);
    virtual void wfbwSelected(uint8_t selectedWFBW);

private:
    Screen1Presenter();

    Screen1View& view;
};

#endif // SCREEN1PRESENTER_HPP
