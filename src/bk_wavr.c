#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frei0r.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


typedef struct wavr_instance {
	unsigned int width;
	unsigned int height;

	double ampl;
	double freq;
	double shift;
	double speed;
} wavr_instance_t;

int f0r_init()
{
	return 1;
}

void f0r_deinit()
{}

void f0r_get_plugin_info(f0r_plugin_info_t *wavr_info)
{
	wavr_info->name = "bk_wavr";
	wavr_info->author = "BIG KRIMPIN";
	wavr_info->plugin_type = F0R_PLUGIN_TYPE_FILTER;
	wavr_info->color_model = F0R_COLOR_MODEL_RGBA8888;
	wavr_info->frei0r_version = FREI0R_MAJOR_VERSION;
	wavr_info->major_version = 0;
	wavr_info->minor_version = 1;
	wavr_info->num_params = 3;
	wavr_info->explanation = "Makes the image do a wave";
}

f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
	wavr_instance_t *inst = (wavr_instance_t*)calloc(1, sizeof(*inst));

	inst->width = width;
	inst->height = height;
	inst->ampl = 20;
	inst->freq = 5;
	inst->shift = 0;
	inst->speed = 1;

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
			info->name = "amplitude";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 1:
			info->name = "frequency";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 2:
			info->name = "speed";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
	}
}

void f0r_set_param_value(f0r_instance_t instance, f0r_param_t param,
		int param_index)
{
	wavr_instance_t *inst = (wavr_instance_t*)instance;
	switch(param_index)
	{
		case 0:
			inst->ampl = *((double*)param);
			break;
		case 1:
			inst->freq = *((double*)param);
			break;
		case 2:
			inst->speed = *((double*)param);
			break;
	}
}

void f0r_get_param_value(f0r_instance_t instance, f0r_param_t param,
		int param_index)
{
	wavr_instance_t *inst = (wavr_instance_t*)instance;
	switch(param_index)
	{
		case 0:
			*((double*)param) = inst->ampl;
			break;
		case 1:
			*((double*)param) = inst->freq;
			break;
		case 2:
			*((double*)param) = inst->speed;
			break;
	}
}

void f0r_update(f0r_instance_t instance, double time, const uint32_t *inframe,
		uint32_t *outframe)
{
	wavr_instance_t *inst = (wavr_instance_t*)instance;
	int w = inst->width;
	int h = inst->height;
	double offset = 0;
	int offset_int;
	
	memset(outframe, 0x00, sizeof(uint32_t) * w * h);

	for (int x = 0; x < w; x++) {
		offset = (h * (inst->ampl / 100)) * sin((2.0 * M_PI * ((double) x + inst->shift)) / (w / inst->freq));
		offset_int = offset;
		for (int y = 0; y < h; y++) {
			if (y + offset_int < 0 || y + offset_int >= h)
				continue;
			outframe[y * w + x] = inframe[(y + offset_int) * w + x];
		}
	}

	inst->shift += inst->speed;
}
