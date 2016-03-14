/*+-----------------------------------------------------------------------------
 * INESC Inovacao (INOV)
 * Rua Alves Redol no. 9
 * 1000-029 Lisboa
 * Portugal
 *------------------------------------------------------------------------------
 * Copyright (C) 2010 INESC Inovacao
 * All rights reserved.
 *
 * THE AUTHORING COMPANY DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL THE AUTHORING COMPANY BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUEN-
 * TIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS AC-
 * TION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *------------------------------------------------------------------------------
 * $Id: v4l2_control.h 592 2011-09-28 12:03:45Z jose.goncalves $
 *---------------------------------------------------------------------------+*/

#ifndef V4L2_CONTROL_H
#define V4L2_CONTROL_H

#include <stdint.h>

typedef enum {
	LEFT, RIGHT
} pan_dir_t;

typedef enum {
	DOWN, UP
} tilt_dir_t;

typedef enum {
	SMALL, MEDIUM, LARGE, XLARGE
} step_t;

typedef enum {
	LED_OFF = 0, LED_ON, LED_BLINK, LED_AUTO
} led_mode_t;

int             v4l2_open(const char *device);

int             v4l2_close(int fd);

int             v4l2_pan(int fd, pan_dir_t dir, step_t step);

int             v4l2_pan_reset(int fd);

int             v4l2_tilt(int fd, tilt_dir_t dir, step_t step);

int             v4l2_tilt_reset(int fd);

int             v4l2_pan_tilt(int fd, pan_dir_t pan_dir, step_t pan_step,
							  tilt_dir_t tilt_dir, step_t tilt_step);

int             v4l2_pan_tilt_reset(int fd);

int             v4l2_led(int fd, led_mode_t mode, uint8_t freq);

int             v4l2_get_brightness(int fd, uint8_t * pval);

int             v4l2_set_brightness(int fd, uint8_t val);

int             v4l2_get_contrast(int fd, uint8_t * pval);

int             v4l2_set_contrast(int fd, uint8_t val);

int             v4l2_get_saturation(int fd, uint8_t * pval);

int             v4l2_set_saturation(int fd, uint8_t val);

int             v4l2_get_sharpness(int fd, uint8_t * pval);

int             v4l2_set_sharpness(int fd, uint8_t val);

int             v4l2_get_focus(int fd, uint8_t * pval);

int             v4l2_set_focus(int fd, uint8_t val);

int             v4l2_image_reset(int fd);

#endif
