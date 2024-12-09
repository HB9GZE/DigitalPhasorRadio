#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

Screen1Presenter::Screen1Presenter(Screen1View &v) :
		view(v)
{

}

void Screen1Presenter::activate()
{

}

void Screen1Presenter::deactivate()
{

}

void Screen1Presenter::sMeterChanged()
{
	view.setSmeter(model->getSmeter());
}

void Screen1Presenter::vfoChanged()
{
	view.setVFO(model->getVFO());
}

void Screen1Presenter::graphChanged()
{
	view.setGraph(model->getGraph());
}

void Screen1Presenter::lsbusbToggled(bool newState)
{
	//save in Model
	model->setLSBUSBState(newState);
	//update UI to reflect state
}

void Screen1Presenter::rxtxToggled(bool newState)
{
	model->setRXTXState(newState);
}

void Screen1Presenter::bandSelected(uint8_t selectedBand)
{
	model->setBandState(selectedBand);
}

void Screen1Presenter::stepSizeSelected(uint16_t selectedStepSize)
{
	model->setStepSize(selectedStepSize);
}

void Screen1Presenter::graphPresenter(uint8_t graphDone)
{
	model->setGraphDisplayState(graphDone);
}

void Screen1Presenter::wfbwSelected(uint8_t selectedWFBW)
{
	model->setWFBWState(selectedWFBW);
}
