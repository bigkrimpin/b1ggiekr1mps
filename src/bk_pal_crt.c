#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frei0r.h"
#include "pal_core.h"

typedef struct pal_crt_instance
{
	unsigned int width;
	unsigned int height;
	
	struct PAL_CRT crt;
	struct PAL_SETTINGS pal;

	/* USER ADJUSTABLE BOOLEANS */
	int blend;
	int color;
	int progressive;
	int raw;

	/* USER ADJUSTABLE VALUES */
	int black_point;
	int white_point;
	int brightness;
	int contrast;
	int hue;
	int saturation;
	int noise;
	int chroma_correction;
	int chroma_lag;
	int color_phase_error;
	int scanlines;
	int xoffset;
	int yoffset;

	unsigned int field;
	unsigned char *screen_buffer;
} pal_crt_instance_t;

int f0r_init()
{
	return 1;
}

void f0r_deinit()
{}

void f0r_get_plugin_info(f0r_plugin_info_t *pal_crt_info)
{
	pal_crt_info->name = "bk_pal_crt";
	/* LMP88959/EMMIR made the PAL-CRT library */
	/* BIG KRIMPIN ported the library to frei0r */
	pal_crt_info->author = "LMP88959/EMMIR, BIG KRIMPIN";
	pal_crt_info->plugin_type = F0R_PLUGIN_TYPE_FILTER;
	pal_crt_info->color_model = F0R_COLOR_MODEL_RGBA8888;
	pal_crt_info->frei0r_version = FREI0R_MAJOR_VERSION;
	pal_crt_info->major_version = 0;
	pal_crt_info->minor_version = 1;
	pal_crt_info->num_params = 17;
	pal_crt_info->explanation = "Applies PAL CRT effect over a source image";
}

f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
	pal_crt_instance_t *inst = (pal_crt_instance_t*)calloc(1, sizeof(*inst));
	unsigned char *screen_buffer = NULL;

	inst->width = width;
	inst->height = height;
	inst->field = 0;
		
	inst->blend = 0;
	inst->color = 1;
	inst->progressive = 1;
	inst->raw = 0;
	inst->black_point = 0;
	inst->white_point = 100;
	inst->brightness = 0;
	inst->contrast = 100;
	inst->hue = 0;
	inst->saturation = 10;
	inst->noise = 12;
	inst->chroma_correction = 0;
	inst->chroma_lag = 0;
	inst->color_phase_error = 0;
	inst->scanlines = 0;
	inst->xoffset = 0;
	inst->yoffset = 0;

	screen_buffer = calloc(inst->width * inst->height, sizeof(int));
	
	pal_init(&inst->crt, inst->width, inst->height, PAL_PIX_FORMAT_RGBA,
			screen_buffer);
	inst->screen_buffer = screen_buffer;

	inst->crt.blend = 1;
	inst->crt.scanlines = 1;

	inst->pal.w = inst->width;
	inst->pal.h = inst->height;

	return (f0r_instance_t)inst;
}

void f0r_destruct(f0r_instance_t instance)
{
	pal_crt_instance_t *inst = (pal_crt_instance_t*)instance;
	free(inst->screen_buffer);
	free(instance);
}

void f0r_get_param_info(f0r_param_info_t *info, int param_index)
{
	switch(param_index)
	{
		case 0:
		       info->name = "blend";
		       info->type = F0R_PARAM_BOOL;
		       info->explanation = "";
		       break;
		case 1:
		       info->name = "color";
		       info->type = F0R_PARAM_BOOL;
		       info->explanation = "";
		       break;
		case 2:
		       info->name = "progressive";
		       info->type = F0R_PARAM_BOOL;
		       info->explanation = "";
		       break;
		case 3:
		       info->name = "raw";
		       info->type = F0R_PARAM_BOOL;
		       info->explanation = "";
		       break;
		case 4:
		       info->name = "black_point";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
		case 5:
		       info->name = "white_point";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
		case 6:
		       info->name = "brightness";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
		case 7:
		       info->name = "contrast";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
		case 8:
		       info->name = "hue";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
		case 9:
		       info->name = "saturation";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
		case 10:
		       info->name = "noise";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
		case 11:
		       info->name = "chroma_correction";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
		case 12:
			info->name = "chroma_lag";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 13:
		       info->name = "color_phase_error";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
		case 14:
		       info->name = "scanlines";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
		case 15:
		       info->name = "xoffset";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
		case 16:
		       info->name = "yoffset";
		       info->type = F0R_PARAM_DOUBLE;
		       info->explanation = "";
		       break;
	}
}

void f0r_set_param_value(f0r_instance_t instance, f0r_param_t param,
		int param_index)
{
	pal_crt_instance_t *inst = (pal_crt_instance_t*)instance;

	switch ( param_index ) {
		case 0:
			inst->blend = *((double*)param);
			break;
		case 1:
			inst->color = *((double*)param);
			break;
		case 2:
			inst->progressive = *((double*)param);
			break;
		case 3:
			inst->raw = *((double*)param);
			break;
		case 4:
			inst->black_point = *((double*)param);
			break;
		case 5:
			inst->white_point = *((double*)param);
			break;
		case 6:
			inst->brightness = *((double*)param);
			break;
		case 7:
			inst->contrast = *((double*)param);
			break;
		case 8:
			inst->hue = *((double*)param);
			break;
		case 9:
			inst->saturation = *((double*)param);
			break;
		case 10:
			inst->noise = *((double*)param);
			break;
		case 11:
			inst->chroma_correction = *((double*)param);
			break;
		case 12:
			inst->chroma_lag = *((double*)param);
			break;
		case 13:
			inst->color_phase_error = *((double*)param);
			break;
		case 14:
			inst->scanlines = *((double*)param);
			break;
		case 15:
			inst->xoffset = *((double*)param);
			break;
		case 16:
			inst->yoffset = *((double*)param);
			break;
	}
}

void f0r_get_param_value(f0r_instance_t instance, f0r_param_t param,
		int param_index)
{
	pal_crt_instance_t *inst = (pal_crt_instance_t*)instance;

	switch ( param_index ) {
		case 0:
			*((double*)param) = inst->blend;
			break;
		case 1:
			*((double*)param) = inst->color;
			break;
		case 2:
			*((double*)param) = inst->progressive;
			break;
		case 3:
			*((double*)param) = inst->raw;
			break;
		case 4:
			*((double*)param) = inst->black_point;
			break;
		case 5:
			*((double*)param) = inst->white_point;
			break;
		case 6:
			*((double*)param) = inst->brightness;
			break;
		case 7:
			*((double*)param) = inst->contrast;
			break;
		case 8:
			*((double*)param) = inst->hue;
			break;
		case 9:
			*((double*)param) = inst->saturation;
			break;
		case 10:
			*((double*)param) = inst->noise;
			break;
		case 11:
			*((double*)param) = inst->chroma_correction;
			break;
		case 12:
			*((double*)param) = inst->chroma_lag;
			break;
		case 13:
			*((double*)param) = inst->color_phase_error;
			break;
		case 14:
			*((double*)param) = inst->scanlines;
			break;
		case 15:
			*((double*)param) = inst->xoffset;
			break;
		case 16:
			*((double*)param) = inst->yoffset;
			break;
	}
}

void f0r_update(f0r_instance_t instance, double time, const uint32_t *inframe,
		uint32_t *outframe)
{
	pal_crt_instance_t *inst = (pal_crt_instance_t*)instance;

	inst->pal.data = (unsigned char*)inframe;
	inst->pal.format = PAL_PIX_FORMAT_RGBA;
	inst->pal.field = inst->field;

	inst->crt.blend = inst->blend;
	inst->pal.as_color = inst->color;
	inst->pal.raw = inst->raw;
	inst->crt.black_point = inst->black_point;
	inst->crt.white_point = inst->white_point;
	inst->crt.brightness = inst->brightness;
	inst->crt.contrast = inst->contrast;
	inst->pal.hue = inst->hue;
	inst->crt.saturation = inst->saturation;
	inst->crt.chroma_correction = inst->chroma_correction;
	inst->crt.chroma_lag = inst->chroma_lag;
	inst->pal.color_phase_error = inst->color_phase_error;
	inst->crt.scanlines = inst->scanlines;
	inst->pal.xoffset = inst->xoffset;
	inst->pal.yoffset = inst->yoffset;

	pal_modulate(&inst->crt, &inst->pal);
	pal_demodulate(&inst->crt, inst->noise);
	if (!inst->progressive)
		inst->field++;

	for (unsigned int x = 0; x < (inst->width * inst->height); x++) {
		outframe[x] = 0xff000000 | (inst->screen_buffer[x * 4 + 2] << 16)
			| (inst->screen_buffer[x * 4 + 1] << 8)
			| (inst->screen_buffer[x * 4]);
	}
}
