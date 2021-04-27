/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2021 The Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file
 * List all module initialisation functions.
 */

#ifndef MODULES_H
#define MODULES_H

#ifdef __cplusplus
extern "C" {
#endif

void cfcitybell_init(Settings *settings);
void cfcitybell_close();

void citylife_init(Settings *settings);
void citylife_close();

void random_house_generator_init(Settings *settings);
void random_house_generator_close();

#ifdef __cplusplus
}
#endif

#endif /* MODULES_H */

