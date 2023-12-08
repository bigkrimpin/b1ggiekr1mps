#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frei0r.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double sign(double x)
{
	return (x > 0) - (x < 0);
}

typedef struct shadow_instance {
	unsigned int width;
	unsigned int height;

	uint32_t *buff;
	size_t buff_size;

	int offset_x;
	int offset_y;
	float transparency;
	int radius;
} shadow_instance_t;

int f0r_init()
{
	return 1;
}

void f0r_deinit()
{}

void f0r_get_plugin_info(f0r_plugin_info_t *shadow_info)
{
	shadow_info->name = "bk_shadow";
	shadow_info->author = "BIG KRIMPIN";
	shadow_info->plugin_type = F0R_PLUGIN_TYPE_FILTER;
	shadow_info->color_model = F0R_COLOR_MODEL_RGBA8888;
	shadow_info->frei0r_version = FREI0R_MAJOR_VERSION;
	shadow_info->major_version = 0;
	shadow_info->minor_version = 1;
	shadow_info->num_params = 4;
	shadow_info->explanation = "Adds drop shadow to image";
}

f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
	shadow_instance_t *inst = (shadow_instance_t*)calloc(1, sizeof(*inst));

	inst->width = width;
	inst->height = height;

	inst->buff = malloc(sizeof(uint32_t) * width * height);

	inst->offset_x = 10;
	inst->offset_y = 10;
	inst->transparency = 80;
	inst->radius = 0;

	return (f0r_instance_t)inst;
}

void f0r_destruct(f0r_instance_t instance)
{
	shadow_instance_t *inst = (shadow_instance_t*)instance;
	free(inst->buff);
	free(instance);
}

void f0r_get_param_info(f0r_param_info_t *info, int param_index)
{
	switch(param_index)
	{
		case 0:
			info->name = "offset_x";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 1:
			info->name = "offset_y";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 2:
			info->name = "transparency";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 3:
			info->name = "radius";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
	}
}

void f0r_set_param_value(f0r_instance_t instance, f0r_param_t param,
		int param_index)
{
	shadow_instance_t *inst = (shadow_instance_t*)instance;
	switch(param_index)
	{
		case 0:
			inst->offset_x = *((double*)param);
			break;
		case 1:
			inst->offset_y = *((double*)param);
			break;
		case 2:
			inst->transparency = *((double*)param);
			break;
		case 3:
			inst->radius = *((double*)param);
			break;
	}
}

void f0r_get_param_value(f0r_instance_t instance, f0r_param_t param,
		int param_index)
{
	shadow_instance_t *inst = (shadow_instance_t*)instance;
	switch(param_index)
	{
		case 0:
			*((double*)param) = inst->offset_x;
			break;
		case 1:
			*((double*)param) = inst->offset_y;
			break;
		case 2:
			*((double*)param) = inst->transparency;
			break;
		case 3:
			*((double*)param) = inst->radius;
			break;
	}
}

void f0r_update(f0r_instance_t instance, double time, const uint32_t *inframe,
		uint32_t *outframe)
{
	shadow_instance_t *inst = (shadow_instance_t*)instance;
	int w = inst->width;
	int h = inst->height;

	int shadow_x;
	int shadow_y;

	uint8_t transparency = round(inst->transparency / 100 * 255);

	int blurred_value_r;
	int blurred_value_g;
	int blurred_value_b;
	int blurred_value_a;
	int divide_by;

	memset(inst->buff, 0x00, sizeof(uint32_t) * w * h);
	memset(outframe, 0x00, sizeof(uint32_t) * w * h);

	for (int y = 0; y < h; y++) {
		shadow_y = y + inst->offset_y;
		if (shadow_y < 0 || shadow_y >= h)
			continue;
		for (int x = 0; x < w; x++) {
			shadow_x = x + inst->offset_x;
			if (shadow_x < 0 || shadow_x >= w)
				continue;
			if (((inframe[y * w + x] & 0xff000000) >> 24) > 0x00)
				inst->buff[shadow_y * w + shadow_x] = transparency << 24;
		}
	}


	if (inst->radius > 0) {
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				blurred_value_r = 0;
				blurred_value_g = 0;
				blurred_value_b = 0;
				blurred_value_a = 0;
				divide_by = 0;
				for (int kernel_y = 0 - inst->radius; kernel_y <= inst->radius; kernel_y++) {
					if (y + kernel_y < 0 || y + kernel_y >= h)
						continue;
					for (int kernel_x = 0 - inst->radius; kernel_x <= inst->radius; kernel_x++) {
						if (x + kernel_x < 0 || x + kernel_x >= w)
							continue;
						blurred_value_r += inst->buff[(y + kernel_y) * w + (x + kernel_x)]       & 0xff;
						blurred_value_g += inst->buff[(y + kernel_y) * w + (x + kernel_x)] >> 8  & 0xff;
						blurred_value_b += inst->buff[(y + kernel_y) * w + (x + kernel_x)] >> 16 & 0xff;
						blurred_value_a += inst->buff[(y + kernel_y) * w + (x + kernel_x)] >> 24 & 0xff;
						divide_by++;
					}
				}
				if (divide_by > 0) {
					outframe[y * w + x] = (blurred_value_r / divide_by)
						| ((blurred_value_g / divide_by) << 8)
						| ((blurred_value_b / divide_by) << 16)
						| ((blurred_value_a / divide_by) << 24);
				}
			}
		}
	} else {
		memcpy(outframe, inst->buff, sizeof(uint32_t) * w * h);
	}

	for (int i = 0; i < w * h; i++) {
		if (((inframe[i] & 0xff000000) >> 24) > 0x00)
			outframe[i] = inframe[i];
	}
}
