#include <pebble.h>

static Window* s_window;
static Layer* s_layer;

int phase(time_t t, int frames) {
  // https://github.com/signetica/MoonPhase
  int julian_centidays = (t + 210866760000) / 864;
  const int MOON_SYNODIC_PERIOD = 2953      // Period of moon cycle in centidays.
  const int MOON_SYNODIC_OFFSET = 245155026 // Reference cycle offset in centidays.
  j = julian_centidays(t);
  p = (j - MOON_SYNODIC_OFFSET) % MOON_SYNODIC_PERIOD;
  return p * frames / MOON_SYNODIC_PERIOD;
}

static GPoint cartesian_from_polar_trigangle(GPoint center, int radius, int trigangle) {
  GPoint ret = {
    .x = (int16_t)(sin_lookup(trigangle) * radius / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(trigangle) * radius / TRIG_MAX_RATIO) + center.y,
  };
  return ret;
}

static GPoint cartesian_from_polar(GPoint center, int radius, int angle_deg) {
  return cartesian_from_polar_trigangle(center, radius, DEG_TO_TRIGANGLE(angle_deg));
}

static void update_layer(Layer* layer, GContext* ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  time_t t = time(NULL);
  struct tm* now = localtime(&t);
  int frames = 59;
  int frame_idx = phase(t, frames);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "%d -> %d", t, frame_idx);
}

static void window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  s_layer = layer_create(bounds);
  layer_set_update_proc(s_layer, update_layer);
  layer_add_child(window_layer, s_layer);
}

static void window_unload(Window* window) {
  if (s_layer) layer_destroy(s_layer);
}

static void tick_handler(struct tm* now, TimeUnits units_changed) {
  if (s_layer) layer_mark_dirty(s_layer);
}

static void init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  if (s_window) window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
