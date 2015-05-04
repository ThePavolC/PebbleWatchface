#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_day_week_layer;

static GBitmap *s_bluetooth_bitmap_active;
static GBitmap *s_bluetooth_bitmap_inactive;
static BitmapLayer *s_bluetooth_layer;
static TextLayer *s_bluetooth_time_layer;

static void bluetooth_connection_handler(bool connected);

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_time_layer = text_layer_create(GRect(0, 0, bounds.size.w, 42));
    s_date_layer = text_layer_create(GRect(0, 42, bounds.size.w, 28));
    s_day_week_layer = text_layer_create(GRect(0, 70, bounds.size.w, 26));

    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_font(s_day_week_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));

    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    text_layer_set_text_alignment(s_day_week_layer, GTextAlignmentCenter);

    s_bluetooth_bitmap_active = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_ICON_ACTIVE);
    s_bluetooth_bitmap_inactive = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_ICON_INACTIVE);
    s_bluetooth_layer = bitmap_layer_create(GRect(0, 100, 22, 22));
    s_bluetooth_time_layer = text_layer_create(GRect(24, 102, bounds.size.w, 16));
    text_layer_set_font(s_bluetooth_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text(s_bluetooth_time_layer, "00:00");
    bluetooth_connection_handler(bluetooth_connection_service_peek());

    window_set_background_color(s_main_window, GColorWhite);

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_week_layer));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bluetooth_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bluetooth_time_layer));
}

static void main_window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
    text_layer_destroy(s_day_week_layer);
    gbitmap_destroy(s_bluetooth_bitmap_active);
    gbitmap_destroy(s_bluetooth_bitmap_inactive);
    bitmap_layer_destroy(s_bluetooth_layer);
    text_layer_destroy(s_bluetooth_time_layer);
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

static void update_date_day_week() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    static char date_buffer[] = "ABC, 00 000 0000";
    strftime(date_buffer, sizeof("ABC, 00 000 0000"), "%a, %d %b %Y", tick_time);
    text_layer_set_text(s_date_layer, date_buffer);

    static char day_week_buffer[] = "day: 000 week: 00";
    strftime(day_week_buffer, sizeof("day: 000 week: 00"), "day: %j week: %W", tick_time);
    text_layer_set_text(s_day_week_layer, day_week_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    update_date_day_week();
}

static void bluetooth_connection_handler(bool connected) {
    vibes_double_pulse();

    if (connected) {
        bitmap_layer_set_bitmap(s_bluetooth_layer, s_bluetooth_bitmap_active);
    } else {
        bitmap_layer_set_bitmap(s_bluetooth_layer, s_bluetooth_bitmap_inactive);
    }

    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    static char time_buffer[] = "(00:00)";

    if (clock_is_24h_style() == true) {
        strftime(time_buffer, sizeof("(00:00)"), "(%H:%M)", tick_time);
    } else {
        strftime(time_buffer, sizeof("(00:00)"), "(%I:%M)", tick_time);
    }

    text_layer_set_text(s_bluetooth_time_layer, time_buffer);
}

static void init() {
    s_main_window = window_create();

    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    window_stack_push(s_main_window, true);

    update_time();
    update_date_day_week();

    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    bluetooth_connection_service_subscribe(bluetooth_connection_handler);
}

static void deinit() {
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
