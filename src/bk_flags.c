#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frei0r.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Make sure the order here matches the one in the Kdenlive XML file */
typedef enum {
	FLAG_RAINBOW,
	FLAG_BISEXUAL,
	FLAG_NONBINARY,
	FLAG_PANSEXUAL,
	FLAG_TRANS,
} flag_t;

typedef struct wavr_instance {
	unsigned int width;
	unsigned int height;

	flag_t flag;
} flags_instance_t;

int f0r_init()
{
	return 1;
}

void f0r_deinit()
{}

void f0r_get_plugin_info(f0r_plugin_info_t *flags_info)
{
	flags_info->name = "bk_flags";
	flags_info->author = "BIG KRIMPIN";
	flags_info->plugin_type = F0R_PLUGIN_TYPE_SOURCE;
	flags_info->color_model = F0R_COLOR_MODEL_RGBA8888;
	flags_info->frei0r_version = FREI0R_MAJOR_VERSION;
	flags_info->major_version = 0;
	flags_info->minor_version = 1;
	flags_info->num_params = 1;
	flags_info->explanation = "Generates flags";
}

f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
	flags_instance_t *inst = (flags_instance_t*)calloc(1, sizeof(*inst));

	inst->width = width;
	inst->height = height;

	return (f0r_instance_t)inst;
}

void f0r_destruct(f0r_instance_t instance)
{
	free(instance);
}

void f0r_get_param_info(f0r_param_info_t *info, int param_index)
{
	switch(param_index)
	{
		case 0:
			info->name = "flag";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
	}
}

void f0r_set_param_value(f0r_instance_t instance, f0r_param_t param,
		int param_index)
{
	flags_instance_t *inst = (flags_instance_t*)instance;
	switch(param_index)
	{
		case 0:
			inst->flag = *((double*)param);
			break;
	}
}

void f0r_get_param_value(f0r_instance_t instance, f0r_param_t param,
		int param_index)
{
	flags_instance_t *inst = (flags_instance_t*)instance;
	switch(param_index)
	{
		case 0:
			*((double*)param) = inst->flag;
			break;
	}
}

void draw_box(uint32_t color, int x1, int y1, int x2, int y2, int frame_w, int frame_h, uint32_t *outframe)
{
	for (int y = y1; y <= y2; y++) {
		for (int x = x1; x <= x2; x++) {
			outframe[y * frame_w + x] = color;
		}
	}
}

void draw_flag(uint32_t *colors, unsigned int bar_count, int w, int h, uint32_t *outframe)
{
	for (unsigned int i = 0; i < bar_count; i++) {
		draw_box(colors[i], 0, h / bar_count * i, w, h / bar_count * (i + 1), w, h, outframe);
	}
}

typedef void (*flag_function)(int w, int h, uint32_t *outframe);

void generate_flag_rainbow(int w, int h, uint32_t *outframe)
{
	uint32_t rainbow_colors[] = {0xFF0000E5,0xFF008DFF,0xFF00EEFF,0xFF218102,0xFFFF4C00,0xFF880077};
	draw_flag(&rainbow_colors[0], 6, w, h, outframe);
}

void generate_flag_bisexual(int w, int h, uint32_t *outframe)
{
	uint32_t bisexual_colors[] = {0xff7002d6, 0xff7002d6, 0xff964f9b, 0xffa83800, 0xffa83800};
	draw_flag(&bisexual_colors[0], 5, w, h, outframe);
}

void generate_flag_nonbinary(int w, int h, uint32_t *outframe)
{
	uint32_t nonbinary_colors[] = {0xff33f4ff, 0xffffffff, 0xffd0599b, 0xff2d2d2d};
	draw_flag(&nonbinary_colors[0], 4, w, h, outframe);
}

void generate_flag_pansexual(int w, int h, uint32_t *outframe)
{
	uint32_t pansexual_colors[] = {0xff8c21ff, 0xff00d8ff, 0xffffb121};
	draw_flag(&pansexual_colors[0], 3, w, h, outframe);
}

void generate_flag_trans(int w, int h, uint32_t *outframe)
{
	uint32_t trans_colors[] = {0xffface5b, 0xffb8a9f5, 0xffffffff, 0xffb8a9f5, 0xffface5b};
	draw_flag(&trans_colors[0], 5, w, h, outframe);
}

void f0r_update(f0r_instance_t instance, double time, const uint32_t *inframe,
		uint32_t *outframe)
{
	flags_instance_t *inst = (flags_instance_t*)instance;
	int w = inst->width;
	int h = inst->height;
	flag_function current_flag_function;
	
	memset(outframe, 0x00, sizeof(uint32_t) * w * h);

	switch(inst->flag) {
		case FLAG_RAINBOW:
			current_flag_function = generate_flag_rainbow;
			break;
		case FLAG_BISEXUAL:
			current_flag_function = generate_flag_bisexual;
			break;
		case FLAG_NONBINARY:
			current_flag_function = generate_flag_nonbinary;
			break;
		case FLAG_PANSEXUAL:
			current_flag_function = generate_flag_pansexual;
			break;
		case FLAG_TRANS:
			current_flag_function = generate_flag_trans;
			break;
	}
	current_flag_function(w, h, outframe);
}
