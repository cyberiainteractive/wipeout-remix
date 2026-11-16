#include "taunt.h"
#include "../sfx.h"
#include "../../input.h"

#define TAUNT_COOLDOWN_FRAMES  180

static int hit_confirmed = 0;
static int cooldown = 0;

void taunt_init(void)
{
    hit_confirmed = 0;
    cooldown = 0;
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

    if (!hit_confirmed || cooldown > 0) {
        return;
    }

    if (input_pressed(A_TAUNT)) {
        sfx_taunt_play_random();

        hit_confirmed = 0;
        cooldown = TAUNT_COOLDOWN_FRAMES;
    }
}

void taunt_reset(void)
{
    hit_confirmed = 0;
    cooldown = 0;
}

void taunt_cleanup(void)
{
    taunt_reset();
}
