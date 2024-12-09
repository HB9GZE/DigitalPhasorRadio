#include <gui/screen1_screen/Screen1View.hpp>
#include <images/BitmapDatabase.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/widgets/PixelDataWidget.hpp>

#include <string.h> // needed for memcpy

#define PIXEL_W 341
#define PIXEL_H 80

unsigned char pixel_data[PIXEL_H * PIXEL_W * 3];
touchgfx::PixelDataWidget pixelDataWidget;
uint8_t stateView_band = 40;
uint8_t stateView_WFBW; //0 = wide (100kHz), 1 = narrow (16kHz)
bool stateView_lsbusb = 0;
bool stateView_rxtx = 0;
uint16_t stateView_stepSize = 1000;
uint8_t graphDisplayedFinished;
uint8_t datapoint;
uint8_t counter;

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
	Screen1ViewBase::setupScreen();

	pixelDataWidget.setPosition(70, 165, PIXEL_W, PIXEL_H);
	pixelDataWidget.setPixelData(pixel_data);
	pixelDataWidget.setBitmapFormat(Bitmap::RGB888);
	pixelDataWidget.setAlpha(255);
	pixelDataWidget.setTouchable(false);
	add(pixelDataWidget);
	pixelDataWidget.invalidate();
}

void Screen1View::tearDownScreen()
{
	Screen1ViewBase::tearDownScreen();
}

void Screen1View::toggleRXTX()
{
	if (stateView_rxtx == 0)
	{
		stateView_rxtx = 1; //Tx now
		buttonPTT.setBitmaps(touchgfx::Bitmap(BITMAP_PTT_ID), touchgfx::Bitmap(BITMAP_PTT_ID));
	}
	else
	{
		stateView_rxtx = 0; //Rx now
		buttonPTT.setBitmaps(touchgfx::Bitmap(BITMAP_PTT_OFF_ID), touchgfx::Bitmap(BITMAP_PTT_OFF_ID));
	}
	buttonPTT.invalidate();
	presenter->rxtxToggled(stateView_rxtx);
}

void Screen1View::toggleLSBUSB()
{
	if (stateView_lsbusb == 0)
	{
		stateView_lsbusb = 1; //USB now
		buttonLSBUSB.setBitmaps(touchgfx::Bitmap(BITMAP_USB_ID), touchgfx::Bitmap(BITMAP_USB_ID));
	}
	else
	{
		stateView_lsbusb = 0; //LSB now
		buttonLSBUSB.setBitmaps(touchgfx::Bitmap(BITMAP_LSB_ID), touchgfx::Bitmap(BITMAP_LSB_ID));
	}
	buttonLSBUSB.invalidate();
	presenter->lsbusbToggled(stateView_lsbusb);
}

void Screen1View::selectBand()
{
	if (stateView_band == 20)
	{
		stateView_band = 40; //40m now
		buttonToggleBand.setBitmaps(touchgfx::Bitmap(BITMAP_M40M_ID), touchgfx::Bitmap(BITMAP_M40M_ID));
		stateView_lsbusb = 0; //LSB now
		buttonLSBUSB.setBitmaps(touchgfx::Bitmap(BITMAP_LSB_ID), touchgfx::Bitmap(BITMAP_LSB_ID));
	}
	else if (stateView_band == 40)
	{
		stateView_band = 80; //80m now
		buttonToggleBand.setBitmaps(touchgfx::Bitmap(BITMAP_M80M_ID), touchgfx::Bitmap(BITMAP_M80M_ID));
		stateView_lsbusb = 0; //LSB now
		buttonLSBUSB.setBitmaps(touchgfx::Bitmap(BITMAP_LSB_ID), touchgfx::Bitmap(BITMAP_LSB_ID));
	}
	else if (stateView_band == 80)
	{
		stateView_band = 20; //20m now
		buttonToggleBand.setBitmaps(touchgfx::Bitmap(BITMAP_M20M_ID), touchgfx::Bitmap(BITMAP_M20M_ID));
		stateView_lsbusb = 1; //USB now
		buttonLSBUSB.setBitmaps(touchgfx::Bitmap(BITMAP_USB_ID), touchgfx::Bitmap(BITMAP_USB_ID));
	}
	presenter->lsbusbToggled(stateView_lsbusb);
	presenter->bandSelected(stateView_band);
	buttonToggleBand.invalidate();
	buttonLSBUSB.invalidate();
}

void Screen1View::setSmeter(int sMeterValue)
{
	if (sMeterValue == 0)
	{
		//boxProgress1.setColor(touchgfx::Color::getColorFromRGB(24, 245, 65));
		boxProgress1.setValue(10);
	}
	else if (sMeterValue == 1)
	{
		boxProgress1.setValue(22);
	}
	else if (sMeterValue == 2)
	{
		boxProgress1.setValue(33);
	}
	else if (sMeterValue == 3)
	{
		boxProgress1.setValue(44);
	}
	else if (sMeterValue == 4)
	{
		boxProgress1.setValue(55);
	}
	else if (sMeterValue == 5)
	{
		boxProgress1.setValue(66);
	}
	else if (sMeterValue == 6)
	{
		boxProgress1.setValue(77);
	}
	else if (sMeterValue == 7)
	{
		boxProgress1.setValue(88);
	}
	else if (sMeterValue == 8)
	{
		boxProgress1.setValue(99);
	}
	else if (sMeterValue == 9)
	{
		boxProgress1.setValue(110);
	}
	else if (sMeterValue == 10)
	{
		boxProgress1.setValue(122);
	}
}

void Screen1View::setVFO(float vfoValue)
{
	if (stateView_band == 20)
	{
		Unicode::snprintfFloat(textAreaVFOBuffer, TEXTAREAVFO_SIZE, "%8.2f", vfoValue);
	}
	else
	{
		Unicode::snprintfFloat(textAreaVFOBuffer, TEXTAREAVFO_SIZE, "%7.2f", vfoValue);
	}
	textAreaVFO.invalidate();
}

void Screen1View::setGraph(QueueElement graphValue)
{
	for (int i = 0; i < (PIXEL_H * PIXEL_W * 3) - 1023; i++)
	{
		pixel_data[(PIXEL_H * PIXEL_W * 3) - 1 - i] = pixel_data[(PIXEL_H * PIXEL_W * 3) - 1 - 1023 - i];
	}

	for (uint16_t i = 0; i < 341; i++)
	{
		dynamicGraph1.addDataPoint(graphValue.graphData[i]);

		if (graphValue.graphData[i] < 3)
		{
			pixel_data[(i * 3)] = 0;
			pixel_data[(i * 3) + 1] = 0;
			pixel_data[(i * 3) + 2] = 0;
		}
		else if (graphValue.graphData[i] < 5)
		{
			pixel_data[(i * 3)] = 173;
			pixel_data[(i * 3) + 1] = 173;
			pixel_data[(i * 3) + 2] = 173;
		}
		else if (graphValue.graphData[i] < 10)
		{
			pixel_data[(i * 3)] = 245;
			pixel_data[(i * 3) + 1] = 245;
			pixel_data[(i * 3) + 2] = 245;
		}
		else if (graphValue.graphData[i] < 15)
		{
			pixel_data[(i * 3)] = 251;
			pixel_data[(i * 3) + 1] = 255;
			pixel_data[(i * 3) + 2] = 0;
		}
		else if (graphValue.graphData[i] < 20)
		{
			pixel_data[(i * 3)] = 0;
			pixel_data[(i * 3) + 1] = 255;
			pixel_data[(i * 3) + 2] = 55;
		}

		else
		{
			pixel_data[i * 3] = 0;
			pixel_data[(i * 3) + 1] = 255;
			pixel_data[(i * 3) + 2] = 55;
		}
	}

	pixel_data[170 * 3] = 255;  //yellow vertical line
	pixel_data[(170 * 3) + 1] = 255;
	pixel_data[(170 * 3) + 2] = 0;

	dynamicGraph1.invalidate();
	pixelDataWidget.invalidate();
	graphDisplayedFinished = 1;
	presenter->graphPresenter(graphDisplayedFinished);
}

void Screen1View::setWFBWup()
{
	if (stateView_WFBW < 3)
	{
		stateView_WFBW++;
	}
	presenter->wfbwSelected(stateView_WFBW);
	textAreaWFBW.invalidate();
	if (stateView_WFBW == 1)
	{
		Unicode::snprintf(textAreaWFBWBuffer, TEXTAREAWFBW_SIZE,"66kHz");
		textAreaWFBW.invalidate();
	}
	else if(stateView_WFBW == 2)
	{
		Unicode::snprintf(textAreaWFBWBuffer, TEXTAREAWFBW_SIZE,"33kHz");
		textAreaWFBW.invalidate();
	}
	else if(stateView_WFBW == 3)
	{
		Unicode::snprintf(textAreaWFBWBuffer, TEXTAREAWFBW_SIZE, "17kHz");
		textAreaWFBW.invalidate();
	}
}

void Screen1View::setWFBWdown()
{
	if (stateView_WFBW > 0)
	{
		stateView_WFBW--;
	}
	presenter->wfbwSelected(stateView_WFBW);
	if (stateView_WFBW == 0)
	{
		Unicode::snprintf(textAreaWFBWBuffer, TEXTAREAWFBW_SIZE,"100kHz");
		textAreaWFBW.invalidate();
	}
	else if(stateView_WFBW == 1)
	{
		Unicode::snprintf(textAreaWFBWBuffer, TEXTAREAWFBW_SIZE,"66kHz");
		textAreaWFBW.invalidate();
	}
	else if(stateView_WFBW == 2)
	{
		Unicode::snprintf(textAreaWFBWBuffer, TEXTAREAWFBW_SIZE,"33kHz");
		textAreaWFBW.invalidate();
	}
}

void Screen1View::changeStepSize()
{
	if (stateView_stepSize == 1000)
	{
		stateView_stepSize = 100;
		Unicode::snprintf(textAreaStepsizeBuffer, TEXTAREASTEPSIZE_SIZE, "stepsize: 100Hz");
		textAreaStepsize.invalidate();
	}
	else if (stateView_stepSize == 100)
	{
		stateView_stepSize = 10;
		Unicode::snprintf(textAreaStepsizeBuffer, TEXTAREASTEPSIZE_SIZE, "stepsize: 10Hz");
		textAreaStepsize.invalidate();
	}
	else if (stateView_stepSize == 10)
	{
		stateView_stepSize = 1000;
		Unicode::snprintf(textAreaStepsizeBuffer, TEXTAREASTEPSIZE_SIZE, "stepsize: 1000Hz");
		textAreaStepsize.invalidate();
	}
	presenter->stepSizeSelected(stateView_stepSize);
}
