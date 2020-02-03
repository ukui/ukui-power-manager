/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2007-2008 Richard Hughes <richard@hughsie.com>
 * Copyright (C) 2013-2017 Xiang Li <lixiang@kylinos.cn>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "egg-debug.h"
#include "egg-discrete.h"

//kobe
struct percent_mapping_struct{
    int origin;/*原百分比，该成员仅为了便于对应，实际没用到*/
    int mapped;/*映射后的百分比*/
};

/**
 * egg_discrete_from_percent:
 * @percentage: The percentage to convert
 * @levels: The number of discrete levels
 *
 * We have to be carefull when converting from %->discrete as precision is very
 * important if we want the highest value.
 *
 * Return value: The discrete value for this percentage.
 **/
guint
egg_discrete_from_percent (guint percentage, guint levels)
{
	/* check we are in range */
	if (percentage > 100)
		return levels;
	if (levels == 0) {
		egg_warning ("levels is 0!");
		return 0;
	}
	return (guint) ((((gfloat) percentage * (gfloat) (levels - 1)) / 100.0f) + 0.5f);
}

//kobe
/*
 * 要求亮度最低5%并且亮度条此时恰好为空，亮度条是根据百分比来显示的，
 * 并且百分比数据类型为整型，所以只能通过百分比映射来将5%-100%映射到0%-100%。
 * 亮度值244时，原来的百分比为5%，亮度条显示5%占用；映射后的百分比为0%，亮度条全
空。
 */
guint map_percent(guint origin)
{
    /*
     * 百分比映射
     * 原值：0  5 10 15 [20] 25 30 35 [40] 45 50 55 [60] 65 70 75 [80] 85 90 95 [100]
     * 映射后： 0 5  10  16  21 26 31  37  42 47 52  58  63 68 73  79  84 89 94  100
     */
    struct percent_mapping_struct mapping_table[20] = {
    {5,0},{10,5},{15,10},{20,16},{25,21},{30,26},{35,31},{40,37},{45,42},{50,47},{55,52},{60,58},{65,63},{70,68},{75,73},{80,79},{85,84},{90,89},{95,94},{100,100}
    };

    /*计算mapping_table的下标*/
    gint index = origin/5 - 1;
    guint mapped = mapping_table[index].mapped;
    return mapped;
}

/**
 * egg_discrete_to_percent:
 * @hw: The discrete level
 * @levels: The number of discrete levels
 *
 * We have to be carefull when converting from discrete->%.
 *
 * Return value: The percentage for this discrete value.
 **/
guint
egg_discrete_to_percent (guint discrete, guint levels)
{
	/* check we are in range */
	if (discrete > levels)
		return 100;
	if (levels == 0) {
		egg_warning ("levels is 0!");
		return 0;
	}
        //kobe
        guint origin_percent = (guint) (((gfloat) discrete * (100.0f / (gfloat) (levels - 1))) + 0.5f);
        guint mapped_percent = map_percent(origin_percent);
        return mapped_percent;
        //return (guint) (((gfloat) discrete * (100.0f / (gfloat) (levels - 1))) + 0.5f);
}

/**
 * egg_discrete_to_fraction:
 * @hw: The discrete level
 * @levels: The number of discrete levels
 *
 * We have to be careful when converting from discrete->fractions.
 *
 * Return value: The floating point fraction (0..1) for this discrete value.
 **/
gfloat
egg_discrete_to_fraction (guint discrete, guint levels)
{
	/* check we are in range */
	if (discrete > levels)
		return 1.0;
	if (levels == 0) {
		egg_warning ("levels is 0!");
		return 0.0;
	}
	return (guint) ((gfloat) discrete / ((gfloat) (levels - 1)));
}

/***************************************************************************
 ***                          MAKE CHECK TESTS                           ***
 ***************************************************************************/
#ifdef EGG_TEST
#include "egg-test.h"

void
egg_discrete_test (gpointer data)
{
	guint value;
	gfloat fvalue;
	EggTest *test = (EggTest *) data;

	if (!egg_test_start (test, "EggDiscrete"))
		return;

	/************************************************************/
	egg_test_title (test, "convert discrete 0/10 levels");
	value = egg_discrete_to_percent (0, 10);
	if (value == 0) {
		egg_test_success (test, "got %i", value);
	} else {
		egg_test_failed (test, "conversion incorrect (%i)", value);
	}

	/************************************************************/
	egg_test_title (test, "convert discrete 9/10 levels");
	value = egg_discrete_to_percent (9, 10);
	if (value == 100) {
		egg_test_success (test, "got %i", value);
	} else {
		egg_test_failed (test, "conversion incorrect (%i)", value);
	}

	/************************************************************/
	egg_test_title (test, "convert discrete 20/10 levels");
	value = egg_discrete_to_percent (20, 10);
	if (value == 100) {
		egg_test_success (test, "got %i", value);
	} else {
		egg_test_failed (test, "conversion incorrect (%i)", value);
	}

	/************************************************************/
	egg_test_title (test, "convert discrete 0/10 levels");
	fvalue = egg_discrete_to_fraction (0, 10);
	if (fvalue > -0.01 && fvalue < 0.01) {
		egg_test_success (test, "got %f", fvalue);
	} else {
		egg_test_failed (test, "conversion incorrect (%f)", fvalue);
	}

	/************************************************************/
	egg_test_title (test, "convert discrete 9/10 levels");
	fvalue = egg_discrete_to_fraction (9, 10);
	if (fvalue > -1.01 && fvalue < 1.01) {
		egg_test_success (test, "got %f", fvalue);
	} else {
		egg_test_failed (test, "conversion incorrect (%f)", fvalue);
	}

	egg_test_end (test);
}

#endif

