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

/* Make sure these stay in the same order as in the XML file */
typedef enum {
	WAVE_SAWTOOTH,
	WAVE_SINE,
	WAVE_SQUARE,
	WAVE_TRIANGLE,
} waveform_t;

typedef struct wavr_instance {
	unsigned int width;
	unsigned int height;

	double ampl;
	double freq;
	double speed;
	waveform_t waveform;
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
	wavr_info->num_params = 4;
	wavr_info->explanation = "Transform the image to a specified waveform";
}

f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
	wavr_instance_t *inst = (wavr_instance_t*)calloc(1, sizeof(*inst));

	inst->width = width;
	inst->height = height;
	inst->ampl = 20;
	inst->freq = 5;
	inst->speed = 1;
	inst->waveform = WAVE_SINE;

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
			info->name = "wave_function";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 1:
			info->name = "amplitude";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 2:
			info->name = "frequency";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 3:
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
			inst->waveform = *((double*)param);
			break;
		case 1:
			inst->ampl = *((double*)param);
			break;
		case 2:
			inst->freq = *((double*)param);
			break;
		case 3:
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
			*((double*)param) = inst->waveform;
			break;
		case 1:
			*((double*)param) = inst->ampl;
			break;
		case 2:
			*((double*)param) = inst->freq;
			break;
		case 3:
			*((double*)param) = inst->speed;
			break;
	}
}

typedef double (*wave_function)(int w, int h, double ampl, double freq, double shift, int x);

double offset_sawtooth_wave(int w, int h, double ampl, double freq, double shift, int x)
{
	return (h * (ampl / 100)) * (((x + shift) / (w / freq)) - floor((1/2) + ((x + shift) / (w / freq))));
}

double offset_sine_wave(int w, int h, double ampl, double freq, double shift, int x)
{
	return (h * (ampl / 100)) * sin((2.0 * M_PI * ((double) x + shift)) / (w / freq));
}

double offset_square_wave(int w, int h, double ampl, double freq, double shift, int x)
{
	return (h * (ampl / 100)) * sign(sin((2.0 * M_PI * ((double) x + shift)) / (w / freq)));
}

double offset_triangle_wave(int w, int h, double ampl, double freq, double shift, int x)
{
	return 2 * (h * (ampl / 100)) * fabs(2 * (((x + shift) / (w / freq)) - floor((x + shift)/ (w / freq) + 0.5))) - (h * (ampl / 100));
}

void f0r_update(f0r_instance_t instance, double time, const uint32_t *inframe,
		uint32_t *outframe)
{
	wavr_instance_t *inst = (wavr_instance_t*)instance;
	int w = inst->width;
	int h = inst->height;
	double offset = 0;
	int offset_int;
	wave_function current_wave_function = NULL;
	
	memset(outframe, 0x00, sizeof(uint32_t) * w * h);

	switch(inst->waveform) {
		case WAVE_SAWTOOTH:
			current_wave_function = offset_sawtooth_wave;
			break;
		case WAVE_SINE:
			current_wave_function = offset_sine_wave;
			break;
		case WAVE_SQUARE:
			current_wave_function = offset_square_wave;
			break;
		case WAVE_TRIANGLE:
			current_wave_function = offset_triangle_wave;
			break;
		default:
			return;
	}

	for (int x = 0; x < w; x++) {
		offset = current_wave_function(w, h, inst->ampl, inst->freq, time * inst->speed, x);
		offset_int = offset;
		for (int y = 0; y < h; y++) {
			if (y + offset_int < 0 || y + offset_int >= h)
				continue;
			outframe[y * w + x] = inframe[(y + offset_int) * w + x];
		}
	}
}
