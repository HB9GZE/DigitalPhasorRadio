// 4.24.0 0xd9ab60d4
// Generated by imageconverter. Please, do not edit!

#include <images/BitmapDatabase.hpp>
#include <touchgfx/Bitmap.hpp>

extern const unsigned char image_alternate_theme_images_widgets_button_regular_height_50_medium_round_action[]; // BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_MEDIUM_ROUND_ACTION_ID = 0, Size: 240x50 pixels
extern const unsigned char image_alternate_theme_images_widgets_button_regular_height_50_medium_round_active[]; // BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_MEDIUM_ROUND_ACTIVE_ID = 1, Size: 240x50 pixels
extern const unsigned char image_alternate_theme_images_widgets_button_regular_height_50_small_rounded_action[]; // BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_SMALL_ROUNDED_ACTION_ID = 2, Size: 175x50 pixels
extern const unsigned char image_alternate_theme_images_widgets_button_regular_height_50_small_rounded_active[]; // BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_SMALL_ROUNDED_ACTIVE_ID = 3, Size: 175x50 pixels

const touchgfx::Bitmap::BitmapData bitmap_database[] = {
    { image_alternate_theme_images_widgets_button_regular_height_50_medium_round_action, 0, 240, 50, 18, 1, 204, ((uint8_t)touchgfx::Bitmap::COMPRESSED_ARGB8888) >> 3, 48, ((uint8_t)touchgfx::Bitmap::COMPRESSED_ARGB8888) & 0x7 },
    { image_alternate_theme_images_widgets_button_regular_height_50_medium_round_active, 0, 240, 50, 18, 1, 204, ((uint8_t)touchgfx::Bitmap::COMPRESSED_ARGB8888) >> 3, 48, ((uint8_t)touchgfx::Bitmap::COMPRESSED_ARGB8888) & 0x7 },
    { image_alternate_theme_images_widgets_button_regular_height_50_small_rounded_action, 0, 175, 50, 5, 0, 165, ((uint8_t)touchgfx::Bitmap::COMPRESSED_ARGB8888) >> 3, 50, ((uint8_t)touchgfx::Bitmap::COMPRESSED_ARGB8888) & 0x7 },
    { image_alternate_theme_images_widgets_button_regular_height_50_small_rounded_active, 0, 175, 50, 5, 0, 165, ((uint8_t)touchgfx::Bitmap::COMPRESSED_ARGB8888) >> 3, 50, ((uint8_t)touchgfx::Bitmap::COMPRESSED_ARGB8888) & 0x7 }
};

namespace BitmapDatabase
{
const touchgfx::Bitmap::BitmapData* getInstance()
{
    return bitmap_database;
}

uint16_t getInstanceSize()
{
    return (uint16_t)(sizeof(bitmap_database) / sizeof(touchgfx::Bitmap::BitmapData));
}
} // namespace BitmapDatabase