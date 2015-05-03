#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_day_week_layer;

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_time_layer = text_layer_create(GRect(0, 0, bounds.size.w, 42));
    s_date_layer = text_layer_create(GRect(0, 42, bounds.size.w, 32));
    s_day_week_layer = text_layer_create(GRect(0, 74, bounds.size.w, 22));

    //text_layer_set_background_color(s_time_layer, GColorBlack);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_text_color(s_date_layer, GColorBlack);
    text_layer_set_text_color(s_day_week_layer, GColorBlack);

    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
    text_layer_set_font(s_day_week_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    //text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);

    window_set_background_color(s_main_window, GColorWhite);

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_week_layer));
}

static void main_window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
}

static void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    static char time_buffer[] = "00:00";

    if (clock_is_24h_style() == true) {
        strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
        strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
    }

    text_layer_set_text(s_time_layer, time_buffer);
}

static void update_date() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    static char date_buffer[] = "00 000 0000";

    strftime(date_buffer, sizeof("00 000 0000"), "%d %b %Y", tick_time);

    text_layer_set_text(s_date_layer, date_buffer);

    static char day_week_buffer[] = "day: 000 , week: 00";

    strftime(day_week_buffer, sizeof("day: 000 , week: 00"), "day: %j , week: %W", tick_time);

    text_layer_set_text(s_day_week_layer, day_week_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    update_date();
}

static void init() {
    s_main_window = window_create();

    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    window_stack_push(s_main_window, true);
    update_time();
    update_date();
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
