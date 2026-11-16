#include "taunt.h"
#include "../sfx.h"
#include "../../input.h"
#include "../../utils.h"
#include "../game.h"

#define TAUNT_COOLDOWN_FRAMES  180
#define TAUNT_DISPLAY_FRAMES    90

static int hit_confirmed = 0;
static int cooldown = 0;

static int active_pilot = -1;
static int display_timer = 0;

void taunt_init(void)
{
    hit_confirmed = 0;
    cooldown = 0;
    active_pilot = -1;
    display_timer = 0;
}

void taunt_on_weapon_hit(void)
{
    hit_confirmed = 1;
}

void taunt_update(void)
{
    if (cooldown > 0) {
        cooldown--;
    }

    if (display_timer > 0) {
        display_timer--;

        if (display_timer == 0) {
            active_pilot = -1;
        }
    }

    if (!hit_confirmed || cooldown > 0) {
        return;
    }

    if (input_pressed(A_TAUNT)) {
        uint32_t count = sfx_taunt_count();

        if (count > 0) {
            sfx_taunt_play(rand_int(0, count));

            active_pilot = g.pilot;
            display_timer = TAUNT_DISPLAY_FRAMES;
        }

        hit_confirmed = 0;
        cooldown = TAUNT_COOLDOWN_FRAMES;
    }
}

int taunt_get_active_pilot(void)
{
    return active_pilot;
}

void taunt_reset(void)
{
    hit_confirmed = 0;
    cooldown = 0;
    active_pilot = -1;
    display_timer = 0;
}

void taunt_cleanup(void)
{
    taunt_reset();
}
