#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#define pdPASS	TRUE

//#ifdef SIMULATOR
#ifndef SIMULATOR
#include "cmsis_os2.h"
#include "main.h"
#include <string.h> // memcpy

extern "C"
{
extern osMessageQueueId_t sMeterQueueHandle;
extern osMessageQueueId_t vfoQueueHandle;
extern osMessageQueueId_t dynGraphQueueHandle;
extern uint8_t stateLSBUSB;
extern uint8_t stateRXTX;
extern uint8_t stateBand;
extern uint8_t graphHasBeenDisplayed;
extern uint8_t stateWFBW;
extern uint16_t stateStepSize;
}
#endif

#ifndef FALSE     /* in case these macros already exist */
#define FALSE 0   /* values of boolean */
#endif
#ifndef TRUE
#define TRUE  1
#endif

Model::Model() :
		modelListener(0), sMeterViewData(9), vfoViewData(7074.0), graphViewData()
{

}

void Model::tick()

{
#ifndef SIMULATOR

	uint16_t newSmeterValue;
	float newVFOValue;
	QueueElement dataReceived;
	//startTimerA = TRUE;

	if (osMessageQueueGet(sMeterQueueHandle, &newSmeterValue, 0U, 0) == osOK)
	{
		if (newSmeterValue != sMeterViewData)
		{
			sMeterViewData = newSmeterValue;
			modelListener->sMeterChanged();
		}
	}

	if (osMessageQueueGet(vfoQueueHandle, &newVFOValue, 0U, 0) == osOK)
	{
		if (newVFOValue != vfoViewData)
		{
			vfoViewData = newVFOValue;
			modelListener->vfoChanged();
		}
	}

	if (osMessageQueueGet(dynGraphQueueHandle, &dataReceived, 0U, 0) == osOK)
	{
		// Copy data from queue to destination array
		memcpy(&graphViewData, &dataReceived, sizeof(dataReceived));
		modelListener->graphChanged();
	}

#endif
}

void Model::setLSBUSBState(bool state)
{
#ifndef SIMULATOR
	stateLSBUSB = state;
#endif
}

void Model::setRXTXState(bool state)
{
#ifndef SIMULATOR
	stateRXTX = state;
#endif
}

void Model::setBandState(uint8_t state)
{
#ifndef SIMULATOR
	stateBand = state;
#endif
}

void Model::setStepSize(uint16_t state)
{
#ifndef SIMULATOR
	stateStepSize = state;
#endif
}

void Model::setGraphDisplayState(uint8_t state)
{
#ifndef SIMULATOR
	graphHasBeenDisplayed = state;
#endif
}

void Model::setWFBWState(uint8_t state)
{
#ifndef SIMULATOR
	stateWFBW = state;
#endif
}
