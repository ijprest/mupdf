// Copyright (C) 2004-2025 Artifex Software, Inc.
//
// This file is part of MuPDF.
//
// MuPDF is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// MuPDF is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with MuPDF. If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
//
// Alternative licensing terms are available from the licensor.
// For commercial licensing, see <https://www.artifex.com/> or contact
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

#include "mupdf/fitz.h"

typedef struct fz_gate_device
{
	fz_device super;
	fz_device *target;
	int enabled;
	int target_xobject;
	int mask_depth;
} fz_gate_device;

static int
gate_should_paint(fz_gate_device *gdev)
{
	return gdev->enabled || gdev->mask_depth > 0;
}

static void
gate_close_device(fz_context *ctx, fz_device *dev)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_close_device(ctx, gdev->target);
}

static void
gate_drop_device(fz_context *ctx, fz_device *dev)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_drop_device(ctx, gdev->target);
}

static void
gate_fill_path(fz_context *ctx, fz_device *dev, const fz_path *path, int even_odd, fz_matrix ctm,
	fz_colorspace *cs, const float *color, float alpha, fz_color_params params)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	if (gate_should_paint(gdev))
		fz_fill_path(ctx, gdev->target, path, even_odd, ctm, cs, color, alpha, params);
}

static void
gate_stroke_path(fz_context *ctx, fz_device *dev, const fz_path *path, const fz_stroke_state *stroke, fz_matrix ctm,
	fz_colorspace *cs, const float *color, float alpha, fz_color_params params)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	if (gate_should_paint(gdev))
		fz_stroke_path(ctx, gdev->target, path, stroke, ctm, cs, color, alpha, params);
}

static void
gate_clip_path(fz_context *ctx, fz_device *dev, const fz_path *path, int even_odd, fz_matrix ctm, fz_rect scissor)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_clip_path(ctx, gdev->target, path, even_odd, ctm, scissor);
}

static void
gate_clip_stroke_path(fz_context *ctx, fz_device *dev, const fz_path *path, const fz_stroke_state *stroke, fz_matrix ctm, fz_rect scissor)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_clip_stroke_path(ctx, gdev->target, path, stroke, ctm, scissor);
}

static void
gate_fill_text(fz_context *ctx, fz_device *dev, const fz_text *text, fz_matrix ctm,
	fz_colorspace *cs, const float *color, float alpha, fz_color_params params)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	if (gate_should_paint(gdev))
		fz_fill_text(ctx, gdev->target, text, ctm, cs, color, alpha, params);
}

static void
gate_stroke_text(fz_context *ctx, fz_device *dev, const fz_text *text, const fz_stroke_state *stroke, fz_matrix ctm,
	fz_colorspace *cs, const float *color, float alpha, fz_color_params params)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	if (gate_should_paint(gdev))
		fz_stroke_text(ctx, gdev->target, text, stroke, ctm, cs, color, alpha, params);
}

static void
gate_clip_text(fz_context *ctx, fz_device *dev, const fz_text *text, fz_matrix ctm, fz_rect scissor)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_clip_text(ctx, gdev->target, text, ctm, scissor);
}

static void
gate_clip_stroke_text(fz_context *ctx, fz_device *dev, const fz_text *text, const fz_stroke_state *stroke, fz_matrix ctm, fz_rect scissor)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_clip_stroke_text(ctx, gdev->target, text, stroke, ctm, scissor);
}

static void
gate_ignore_text(fz_context *ctx, fz_device *dev, const fz_text *text, fz_matrix ctm)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	if (gate_should_paint(gdev))
		fz_ignore_text(ctx, gdev->target, text, ctm);
}

static void
gate_fill_shade(fz_context *ctx, fz_device *dev, fz_shade *shd, fz_matrix ctm, float alpha, fz_color_params params)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	if (gate_should_paint(gdev))
		fz_fill_shade(ctx, gdev->target, shd, ctm, alpha, params);
}

static void
gate_fill_image(fz_context *ctx, fz_device *dev, fz_image *img, fz_matrix ctm, float alpha, fz_color_params params)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	if (gate_should_paint(gdev))
		fz_fill_image(ctx, gdev->target, img, ctm, alpha, params);
}

static void
gate_fill_image_mask(fz_context *ctx, fz_device *dev, fz_image *img, fz_matrix ctm,
	fz_colorspace *cs, const float *color, float alpha, fz_color_params params)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	if (gate_should_paint(gdev))
		fz_fill_image_mask(ctx, gdev->target, img, ctm, cs, color, alpha, params);
}

static void
gate_clip_image_mask(fz_context *ctx, fz_device *dev, fz_image *img, fz_matrix ctm, fz_rect scissor)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_clip_image_mask(ctx, gdev->target, img, ctm, scissor);
}

static void
gate_pop_clip(fz_context *ctx, fz_device *dev)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_pop_clip(ctx, gdev->target);
}

static void
gate_begin_mask(fz_context *ctx, fz_device *dev, fz_rect area, int luminosity, fz_colorspace *cs,
	const float *bc, fz_color_params params)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_begin_mask(ctx, gdev->target, area, luminosity, cs, bc, params);
	gdev->mask_depth++;
}

static void
gate_end_mask(fz_context *ctx, fz_device *dev, fz_function *fn)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_end_mask_tr(ctx, gdev->target, fn);
	if (gdev->mask_depth > 0)
		gdev->mask_depth--;
}

static void
gate_begin_group(fz_context *ctx, fz_device *dev, fz_rect area, fz_colorspace *cs, int isolated, int knockout, int blendmode, float alpha)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_begin_group(ctx, gdev->target, area, cs, isolated, knockout, blendmode, alpha);
}

static void
gate_end_group(fz_context *ctx, fz_device *dev)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_end_group(ctx, gdev->target);
}

static int
gate_begin_tile(fz_context *ctx, fz_device *dev, fz_rect area, fz_rect view, float xstep, float ystep, fz_matrix ctm, int id, int doc_id)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	return fz_begin_tile_tid(ctx, gdev->target, area, view, xstep, ystep, ctm, id, doc_id);
}

static void
gate_end_tile(fz_context *ctx, fz_device *dev)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_end_tile(ctx, gdev->target);
}

static void
gate_render_flags(fz_context *ctx, fz_device *dev, int set, int clear)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_render_flags(ctx, gdev->target, set, clear);
}

static void
gate_set_default_colorspaces(fz_context *ctx, fz_device *dev, fz_default_colorspaces *cs)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_set_default_colorspaces(ctx, gdev->target, cs);
}

static void
gate_begin_layer(fz_context *ctx, fz_device *dev, const char *layer_name)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_begin_layer(ctx, gdev->target, layer_name);
}

static void
gate_end_layer(fz_context *ctx, fz_device *dev)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_end_layer(ctx, gdev->target);
}

static void
gate_begin_structure(fz_context *ctx, fz_device *dev, fz_structure standard, const char *raw, int idx)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_begin_structure(ctx, gdev->target, standard, raw, idx);
}

static void
gate_end_structure(fz_context *ctx, fz_device *dev)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_end_structure(ctx, gdev->target);
}

static void
gate_begin_metatext(fz_context *ctx, fz_device *dev, fz_metatext meta, const char *text)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_begin_metatext(ctx, gdev->target, meta, text);
}

static void
gate_end_metatext(fz_context *ctx, fz_device *dev)
{
	fz_gate_device *gdev = (fz_gate_device *)dev;
	fz_end_metatext(ctx, gdev->target);
}

fz_device *
fz_new_gate_device(fz_context *ctx, fz_device *target, int target_xobject)
{
	fz_gate_device *gdev;

	if (target == NULL)
		fz_throw(ctx, FZ_ERROR_ARGUMENT, "gate devices require a target");

	gdev = fz_new_derived_device(ctx, fz_gate_device);
	gdev->target = fz_keep_device(ctx, target);
	gdev->enabled = 0;
	gdev->target_xobject = target_xobject;
	gdev->mask_depth = 0;

	gdev->super.hints = target->hints;
	gdev->super.flags = target->flags;

	gdev->super.close_device = gate_close_device;
	gdev->super.drop_device = gate_drop_device;

	gdev->super.fill_path = gate_fill_path;
	gdev->super.stroke_path = gate_stroke_path;
	gdev->super.clip_path = gate_clip_path;
	gdev->super.clip_stroke_path = gate_clip_stroke_path;

	gdev->super.fill_text = gate_fill_text;
	gdev->super.stroke_text = gate_stroke_text;
	gdev->super.clip_text = gate_clip_text;
	gdev->super.clip_stroke_text = gate_clip_stroke_text;
	gdev->super.ignore_text = gate_ignore_text;

	gdev->super.fill_shade = gate_fill_shade;
	gdev->super.fill_image = gate_fill_image;
	gdev->super.fill_image_mask = gate_fill_image_mask;
	gdev->super.clip_image_mask = gate_clip_image_mask;

	gdev->super.pop_clip = gate_pop_clip;

	gdev->super.begin_mask = gate_begin_mask;
	gdev->super.end_mask = gate_end_mask;
	gdev->super.begin_group = gate_begin_group;
	gdev->super.end_group = gate_end_group;

	gdev->super.begin_tile = gate_begin_tile;
	gdev->super.end_tile = gate_end_tile;

	gdev->super.render_flags = gate_render_flags;
	gdev->super.set_default_colorspaces = gate_set_default_colorspaces;

	gdev->super.begin_layer = gate_begin_layer;
	gdev->super.end_layer = gate_end_layer;

	gdev->super.begin_structure = gate_begin_structure;
	gdev->super.end_structure = gate_end_structure;

	gdev->super.begin_metatext = gate_begin_metatext;
	gdev->super.end_metatext = gate_end_metatext;

	return &gdev->super;
}

int
fz_gate_device_match_xobject(fz_context *ctx, fz_device *dev, int objnum)
{
	fz_gate_device *gdev;

	(void)ctx;

	if (dev == NULL || dev->drop_device != gate_drop_device)
		return 0;

	gdev = (fz_gate_device *)dev;
	if (gdev->enabled)
		return 0;
	if (objnum <= 0 || objnum != gdev->target_xobject)
		return 0;

	gdev->enabled = 1;
	gdev->target_xobject = 0;
	return 1;
}
