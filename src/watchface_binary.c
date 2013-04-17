#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define CIRCLE_RADIUS 6

#define MY_UUID { 0x14, 0x55, 0x7E, 0xD8, 0x8C, 0xF7, 0x4A, 0xF8, 0xA0, 0x10, 0xB9, 0xD8, 0xC5, 0x27, 0x21, 0xB3 }
PBL_APP_INFO(MY_UUID,
             "Binary", "Mikulas",
             1, 1, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

BmpContainer background_image;
Layer led_layer;

GPoint getCenter(int row, int led) {
  return GPoint(12 + led * 24, 70 + 27 * row);
}

void color_led(GContext* ctx, int row, int led, bool on) {
  graphics_context_set_fill_color(ctx, on ? GColorWhite : GColorBlack);
  graphics_fill_circle(ctx, getCenter(row, led), CIRCLE_RADIUS);
}

void led_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  PblTm t;
  get_time(&t);

  unsigned short hour = t.tm_hour % 12;
  unsigned short min = t.tm_min;

  color_led(ctx, 0, 0, t.tm_hour > 12);

  color_led(ctx, 0, 2, hour & 8);
  color_led(ctx, 0, 3, hour & 4);
  color_led(ctx, 0, 4, hour & 2);
  color_led(ctx, 0, 5, hour & 1);

  color_led(ctx, 1, 0, min & 32);
  color_led(ctx, 1, 1, min & 16);
  color_led(ctx, 1, 2, min & 8);
  color_led(ctx, 1, 3, min & 4);
  color_led(ctx, 1, 4, min & 2);
  color_led(ctx, 1, 5, min & 1);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  // Need to be static because they're used by the system later.
  static char time_text_top[] = "8 4 2 1";
  static char time_text_bottom[] = "32 16 8 4 2 1";

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image);
  layer_add_child(&window.layer, &background_image.layer.layer);

  layer_init(&led_layer, window.layer.frame);
  led_layer.update_proc = &led_layer_update_callback;

  layer_add_child(&window.layer, &led_layer);
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&background_image);
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;

  layer_mark_dirty(&led_layer);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = SECOND_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
