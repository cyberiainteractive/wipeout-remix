#include "../types.h"
#include "../utils.h"
#include "ship.h"
#include "weapon.h"
#include "hud.h"
#include "camera.h"
#include "image.h"
#include "game.h"
#include "ui.h"
#include <stdio.h>

static texture_list_t weapon_icon_textures;
static texture_list_t xl_weapon_icon_textures;
static uint16_t target_reticle;
static uint16_t xl_target_reticle;

typedef struct {
	vec2i_t offset;
	uint16_t height;
	rgba_t color;
} speedo_bar_t;

const struct {
	uint16_t width;
	uint16_t skew;
	speedo_bar_t bars[13];
} speedo = {
	.width = 121,
	.skew = 2,
	.bars = {
		{{.x =   6, .y = 12}, .height = 10, .color = rgba( 66,  16,  49, 255)},
		{{.x =  13, .y = 12}, .height = 10, .color = rgba(115,  33,  90, 255)},
		{{.x =  20, .y = 12}, .height = 10, .color = rgba(132,  58, 164, 255)},
		{{.x =  27, .y = 12}, .height = 10, .color = rgba( 99,  90, 197, 255)},
		{{.x =  34, .y = 12}, .height = 10, .color = rgba( 74, 148, 181, 255)},
		{{.x =  41, .y = 12}, .height = 10, .color = rgba( 66, 173, 115, 255)},
		{{.x =  50, .y = 10}, .height = 12, .color = rgba( 99, 206,  58, 255)},
		{{.x =  59, .y =  8}, .height = 12, .color = rgba(189, 206,  41, 255)},
		{{.x =  69, .y =  5}, .height = 13, .color = rgba(247, 140,  33, 255)},
		{{.x =  81, .y =  2}, .height = 15, .color = rgba(255, 197,  49, 255)},
		{{.x =  95, .y =  1}, .height = 16, .color = rgba(255, 222, 115, 255)},
		{{.x = 110, .y =  1}, .height = 16, .color = rgba(255, 239, 181, 255)},
		{{.x = 126, .y =  1}, .height = 16, .color = rgba(255, 255, 255, 255)}
	}
};

static uint16_t speedo_facia_texture;

static uint16_t xl_speedo_panel_texture;
static uint16_t xl_energy_bar_texture;
static uint16_t xl_thrust_bar_texture;

void hud_load(void) {
	speedo_facia_texture = image_get_texture("wipeout/textures/speedo.tim");
	target_reticle = image_get_texture_semi_trans("wipeout/textures/target2.tim");
	weapon_icon_textures = image_get_compressed_textures("wipeout/common/wicons.cmp");

	xl_speedo_panel_texture = image_get_texture_semi_trans("wipeout2/textures/SPBILE2.TIM");
	xl_energy_bar_texture = image_get_texture_semi_trans("wipeout2/textures/ENERGY.TIM");
	xl_thrust_bar_texture = image_get_texture_semi_trans("wipeout2/textures/THRUST.TIM");
	xl_target_reticle = image_get_texture_semi_trans("wipeout2/textures/TARGET2.TIM");
	xl_weapon_icon_textures = image_get_compressed_textures_semi_trans("wipeout2/textures/WEPICON.CMP");
}

static void hud_draw_speedo_bar(vec2i_t *pos, const speedo_bar_t *a, const speedo_bar_t *b, float f, rgba_t color_override) {
	rgba_t left_color, right_color;

	if (color_override.a > 0) {
		left_color = color_override;
		right_color = color_override;
	}
	else {
		left_color = a->color;
		right_color = rgba(
			lerp(a->color.r, b->color.r, f),
			lerp(a->color.g, b->color.g, f),
			lerp(a->color.b, b->color.b, f),
			lerp(a->color.a, b->color.a, f)
		);
	}

	float right_h = lerp(a->height, b->height, f);

	vec2i_t top_left     = vec2i(a->offset.x + 1, a->offset.y);
	vec2i_t bottom_left  = vec2i(a->offset.x + 1 - a->height / speedo.skew, a->offset.y + a->height);
	vec2i_t top_right    = vec2i(lerp(a->offset.x + 1, b->offset.x, f), lerp(a->offset.y, b->offset.y, f));
	vec2i_t bottom_right = vec2i(top_right.x - right_h / speedo.skew, top_right.y + right_h);

	top_left     = ui_scaled(top_left);
	bottom_left  = ui_scaled(bottom_left);
	top_right    = ui_scaled(top_right);
	bottom_right = ui_scaled(bottom_right);

	render_push_tris((tris_t) {
		.vertices = {
			{.pos = {pos->x + bottom_left.x, pos->y + bottom_left.y, 0}, .uv = {0, 0}, .color = left_color},
			{.pos = {pos->x + top_right.x, pos->y + top_right.y, 0}, .uv = {0, 0}, .color = right_color},
			{.pos = {pos->x + top_left.x, pos->y + top_left.y, 0}, .uv = {0, 0}, .color = left_color},
		}
	}, RENDER_NO_TEXTURE);

	render_push_tris((tris_t) {
		.vertices = {
			{.pos = {pos->x + bottom_right.x, pos->y + bottom_right.y, 0}, .uv = {0, 0}, .color = right_color},
			{.pos = {pos->x + top_right.x, pos->y + top_right.y, 0}, .uv = {0, 0}, .color = right_color},
			{.pos = {pos->x + bottom_left.x, pos->y + bottom_left.y, 0}, .uv = {0, 0}, .color = left_color},
		}
	}, RENDER_NO_TEXTURE);
}

static void hud_draw_speedo_bars(vec2i_t *pos, float f, rgba_t color_override) {
	if (f <= 0) return;
	if (f - floor(f) > 0.9) f = ceilf(f);

	f = fminf(f, 13);

	int bars = f;

	for (int i = 1; i < bars; i++) {
		hud_draw_speedo_bar(pos, &speedo.bars[i - 1], &speedo.bars[i], 1, color_override);
	}

	if (bars > 12) return;

	float last_bar_fraction = f - bars + 0.1;
	if (last_bar_fraction <= 0) return;

	last_bar_fraction = fminf(last_bar_fraction, 1);

	int last_bar = bars == 0 ? 1 : bars;

	hud_draw_speedo_bar(pos, &speedo.bars[last_bar - 1], &speedo.bars[last_bar], last_bar_fraction, color_override);
}

static void hud_draw_speedo(int speed, int thrust) {
	vec2i_t facia_pos = ui_scaled_pos(UI_POS_BOTTOM | UI_POS_RIGHT, vec2i(-141, -45));
	vec2i_t bar_pos = ui_scaled_pos(UI_POS_BOTTOM | UI_POS_RIGHT, vec2i(-141, -40));

	hud_draw_speedo_bars(&bar_pos, thrust / 65.0, rgba(255, 0, 0, 128));
	hud_draw_speedo_bars(&bar_pos, speed / 2166.0, rgba(0, 0, 0, 0));

	render_push_2d(facia_pos, ui_scaled(render_texture_size(speedo_facia_texture)), rgba(128, 128, 128, 255), speedo_facia_texture);
}


static void hud_draw_xl_bar(vec2i_t offset, uint16_t texture, float fraction, rgba_t tint) {
    int width = (int)(96.0f * fminf(fmaxf(fraction, 0.0f), 1.0f));
    if (width <= 0) return;

    vec2i_t pos = ui_scaled_pos(
        UI_POS_BOTTOM | UI_POS_RIGHT,
        offset
    );

    vec2i_t size = ui_scaled(vec2i(width, 22));
    vec2i_t uv_size = vec2i(width, 22);

    render_push_2d_tile(pos, vec2i(0, 0), uv_size, size, tint, texture);
}

static void hud_draw_xl(ship_t *ship) {
    int display_lap = max(1, ship->lap + 1);

    float speed_fraction = (ship->speed / 2166.0f) / 13.0f;
    float thrust_fraction = (ship->thrust_mag / 65.0f) / 13.0f;

    hud_draw_xl_bar(
        vec2i(-102, -48),
        xl_thrust_bar_texture,
        speed_fraction,
        rgba(128, 128, 128, 255)
    );

    hud_draw_xl_bar(
        vec2i(-102, -48),
        xl_thrust_bar_texture,
        thrust_fraction,
        rgba(128, 64, 64, 160)
    );

    hud_draw_xl_bar(
        vec2i(-112, -42),
        xl_energy_bar_texture,
        1.0f,
        rgba(128, 128, 128, 255)
    );

    vec2i_t panel_pos = ui_scaled_pos(
        UI_POS_BOTTOM | UI_POS_RIGHT,
        vec2i(-115, -79)
    );

    render_push_2d(
        panel_pos,
        ui_scaled(vec2i(112, 64)),
        rgba(128, 128, 128, 255),
        xl_speedo_panel_texture
    );

    char lap_text[16];
    snprintf(lap_text, sizeof(lap_text), "%d of %d", display_lap, NUM_LAPS);

    ui_draw_text(
        "LAP",
        ui_scaled_pos(UI_POS_TOP | UI_POS_RIGHT, vec2i(-62, 50)),
        UI_SIZE_8,
        UI_COLOR_DEFAULT
    );

    ui_draw_text(
        lap_text,
        ui_scaled_pos(UI_POS_TOP | UI_POS_RIGHT, vec2i(-57, 60)),
        UI_SIZE_8,
        UI_COLOR_DEFAULT
    );

    if (g.race_type != RACE_TYPE_TIME_TRIAL) {
        char pos_text[16];
        snprintf(pos_text, sizeof(pos_text), "%d of %d", ship->position_rank, NUM_PILOTS);

        ui_draw_text(
            "POS",
            ui_scaled_pos(UI_POS_TOP | UI_POS_RIGHT, vec2i(-62, 20)),
            UI_SIZE_8,
            UI_COLOR_DEFAULT
        );

        ui_draw_text(
            pos_text,
            ui_scaled_pos(UI_POS_TOP | UI_POS_RIGHT, vec2i(-57, 30)),
            UI_SIZE_8,
            UI_COLOR_DEFAULT
        );
    }

    if (ship->lap >= 0) {
        ui_draw_time(
            ship->lap_time,
            ui_scaled_pos(UI_POS_BOTTOM | UI_POS_LEFT, vec2i(16, -35)),
            UI_SIZE_8,
            UI_COLOR_DEFAULT
        );

        for (int i = 0; i < ship->lap && i < NUM_LAPS - 1; i++) {
            ui_draw_time(
                g.lap_times[ship->pilot][i],
                ui_scaled_pos(
                    UI_POS_BOTTOM | UI_POS_LEFT,
                    vec2i(16, -25 + (10 * i))
                ),
                UI_SIZE_8,
                UI_COLOR_ACCENT
            );
        }
    }

    if (flags_not(ship->flags, SHIP_DIRECTION_FORWARD)) {
        ui_draw_text_centered(
            "WRONG WAY",
            ui_scaled_pos(UI_POS_MIDDLE | UI_POS_CENTER, vec2i(0, 0)),
            UI_SIZE_16,
            UI_COLOR_ACCENT
        );
    }

    if (ship->weapon_type != WEAPON_TYPE_NONE &&
        ship->weapon_type - 1 < xl_weapon_icon_textures.len) {

        uint16_t icon = texture_from_list(
            xl_weapon_icon_textures,
            ship->weapon_type - 1
        );

        vec2i_t pos = ui_scaled_pos(
            UI_POS_TOP | UI_POS_CENTER,
            vec2i(-7, 25)
        );

        render_push_2d(
            pos,
            ui_scaled(vec2i(32, 35)),
            rgba(128, 128, 128, 255),
            icon
        );
    }

    if (ship->weapon_target) {
        vec3_t projected = render_transform(ship->weapon_target->position);
        vec2i_t screen_size = render_size();
        vec2i_t size = ui_scaled(render_texture_size(xl_target_reticle));

        if (projected.x >= -1 && projected.x <= 1 &&
            projected.y >= -1 && projected.y <= 1 &&
            projected.z < 1) {

            vec2i_t pos = vec2i(
                ((projected.x + 1.0f) / 2.0f) * screen_size.x - size.x / 2,
                ((-projected.y + 1.0f) / 2.0f) * screen_size.y - size.y / 2
            );

            render_push_2d(
                pos,
                size,
                rgba(128, 128, 128, 128),
                xl_target_reticle
            );
        }
    }
}

static void hud_draw_target_icon(vec3_t position) {
	uint16_t texture = save.hud_style == 1 ? xl_target_reticle : target_reticle;

	vec2i_t screen_size = render_size();
	vec2i_t size = ui_scaled(render_texture_size(texture));
	vec3_t projected = render_transform(position);

	if (projected.x < -1 || projected.x > 1 || projected.y < -1 || projected.y > 1 || projected.z >= 1) return;

	vec2i_t pos = vec2i(
		((projected.x + 1.0f) / 2.0f) * screen_size.x - size.x / 2,
		((-projected.y + 1.0f) / 2.0f) * screen_size.y - size.y / 2
	);

	render_push_2d(pos, size, rgba(128, 128, 128, 128), texture);
}

void hud_draw(ship_t *ship) {
	if (save.hud_style == 1) {
		hud_draw_xl(ship);
		return;
	}

	// Current lap time
	if (ship->lap >= 0) {
		ui_draw_time(ship->lap_time, ui_scaled_pos(UI_POS_BOTTOM | UI_POS_LEFT, vec2i(16, -30)), UI_SIZE_16, UI_COLOR_DEFAULT);

		for (int i = 0; i < ship->lap && i < NUM_LAPS-1; i++) {
			ui_draw_time(g.lap_times[ship->pilot][i], ui_scaled_pos(UI_POS_BOTTOM | UI_POS_LEFT, vec2i(16, -45 - (10 * i))), UI_SIZE_8, UI_COLOR_ACCENT);
		}
	}

	// Current Lap
	int display_lap = max(0, ship->lap + 1);

	ui_draw_text("LAP", ui_scaled(vec2i(15, 8)), UI_SIZE_8, UI_COLOR_ACCENT);
	ui_draw_number(display_lap, ui_scaled(vec2i(10, 19)), UI_SIZE_16, UI_COLOR_DEFAULT);

	int width = ui_char_width('0' + display_lap, UI_SIZE_16);

	ui_draw_text("OF", ui_scaled(vec2i((10 + width), 27)), UI_SIZE_8, UI_COLOR_ACCENT);
	ui_draw_number(NUM_LAPS, ui_scaled(vec2i((32 + width), 19)), UI_SIZE_16, UI_COLOR_DEFAULT);

	// Race Position
	if (g.race_type != RACE_TYPE_TIME_TRIAL) {
		ui_draw_text("POSITION", ui_scaled_pos(UI_POS_TOP | UI_POS_RIGHT, vec2i(-90, 8)), UI_SIZE_8, UI_COLOR_ACCENT);
		ui_draw_number(ship->position_rank, ui_scaled_pos(UI_POS_TOP | UI_POS_RIGHT, vec2i(-60, 19)), UI_SIZE_16, UI_COLOR_DEFAULT);
	}

	// Framerate/draw stats
	switch (save.draw_stats) {
	case DRAW_STATS_FPS:
		ui_draw_text("FPS", ui_scaled(vec2i(16, 78)), UI_SIZE_8, UI_COLOR_ACCENT);
		ui_draw_number((int)(g.frame_rate), ui_scaled(vec2i(16, 90)), UI_SIZE_8, UI_COLOR_DEFAULT);
		break;

	case DRAW_STATS_DEBUG: {
		ui_draw_text("TRIS", ui_scaled(vec2i(16, 78)), UI_SIZE_8, UI_COLOR_ACCENT);
		ui_draw_text("CALLS", ui_scaled(vec2i(80, 78)), UI_SIZE_8, UI_COLOR_ACCENT);
		ui_draw_text("MS", ui_scaled(vec2i(144, 78)), UI_SIZE_8, UI_COLOR_ACCENT);

		const render_stats_t *stats = render_frame_get_stats();

		ui_draw_number((int)(stats->num_tris), ui_scaled(vec2i(16, 90)), UI_SIZE_8, UI_COLOR_DEFAULT);
		ui_draw_number((int)(stats->num_draw_calls), ui_scaled(vec2i(80, 90)), UI_SIZE_8, UI_COLOR_DEFAULT);
		ui_draw_number((int)(g.frame_time * 1000), ui_scaled(vec2i(144, 90)), UI_SIZE_8, UI_COLOR_DEFAULT);
		break;
	}

	default:
		break;
	}

	// Lap Record
	ui_draw_text("LAP RECORD", ui_scaled(vec2i(15, 43)), UI_SIZE_8, UI_COLOR_ACCENT);
	ui_draw_time(save.highscores[g.race_class][g.circuit][g.highscore_tab].lap_record, ui_scaled(vec2i(15, 55)), UI_SIZE_8, UI_COLOR_DEFAULT);

	// Wrong way
	if (flags_not(ship->flags, SHIP_DIRECTION_FORWARD)) {
		ui_draw_text_centered("WRONG WAY", ui_scaled_pos(UI_POS_MIDDLE | UI_POS_CENTER, vec2i(-20, 0)), UI_SIZE_16, UI_COLOR_ACCENT);
	}

	// Speedo
	int speedo_speed = (g.camera.update_func == camera_update_attract_internal) ? ship->speed * 7 : ship->speed;
	hud_draw_speedo(speedo_speed, ship->thrust_mag);

	// Weapon icon
	if (ship->weapon_type != WEAPON_TYPE_NONE) {
		vec2i_t pos = ui_scaled_pos(UI_POS_TOP | UI_POS_CENTER, vec2i(-16, 20));
		vec2i_t size = ui_scaled(vec2i(32, 32));

		uint16_t icon = texture_from_list(weapon_icon_textures, ship->weapon_type - 1);
		render_push_2d(pos, size, rgba(128,128,128,255), icon);
	}

	// Lives
	if (g.race_type == RACE_TYPE_CHAMPIONSHIP) {
		for (int i = 0; i < g.lives; i++) {
			ui_draw_icon(UI_ICON_STAR, ui_scaled_pos(UI_POS_BOTTOM | UI_POS_RIGHT, vec2i(-26 - 13 * i, -50)), UI_COLOR_DEFAULT);
		}
	}

	// Weapon target reticle
	if (ship->weapon_target) hud_draw_target_icon(ship->weapon_target->position);
}
