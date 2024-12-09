#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

typedef struct
{
	int graphData[341];
} QueueElement;

class ModelListener;

class Model
{
public:
	Model();

	void bind(ModelListener *listener)
	{
		modelListener = listener;
	}

	void tick();

	int getSmeter()
	{
		return sMeterViewData;
	}

	float getVFO()
	{
		return vfoViewData;
	}

	QueueElement getGraph()
	{
		return graphViewData;
	}

	void setLSBUSBState(bool state);
	void setRXTXState(bool state);
	void setBandState(uint8_t state);
	void setGraphDisplayState(uint8_t state);
	void setWFBWState(uint8_t state);
	void setStepSize(uint16_t state);

protected:
	ModelListener *modelListener;
	int sMeterViewData;
	float vfoViewData;
	QueueElement graphViewData;
};

#endif // MODEL_HPP
