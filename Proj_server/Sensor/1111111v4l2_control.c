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
 * $Id: v4l2_control.c 592 2011-09-28 12:03:45Z jose.goncalves $
 *---------------------------------------------------------------------------+*/

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include "v4l2_control.h"

#ifdef DEBUG
#include <stdio.h>
#include <string.h>
#define debug(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

#define IOCTL_RETRY 3

#define V4L2_CID_BASE_EXTCTR             0x0A046D01
//#define V4L2_CID_BASE_EXTCTR             0x9a0902
#define V4L2_CID_BASE_LOGITECH           V4L2_CID_BASE_EXTCTR
/*#define V4L2_CID_PAN_RESET           0x9a0906
#define V4L2_CID_TILT_RESET           0x9a0907
#define V4L2_CID_TILT_RELATIVE           0x9a0905
#define V4L2_CID_PAN_RELATIVE           0x9a0904*/
#define V4L2_CID_PANTILT_RESET_LOGITECH  (V4L2_CID_BASE_LOGITECH+2)
#define V4L2_CID_FOCUS_LOGITECH          (V4L2_CID_BASE_LOGITECH+3)
#define V4L2_CID_LED1_MODE_LOGITECH      (V4L2_CID_BASE_LOGITECH+4)
#define V4L2_CID_LED1_FREQUENCY_LOGITECH (V4L2_CID_BASE_LOGITECH+5)


/* ioctl with a number of retries in the case of failure
 * args:
 *   fd - device descriptor
 *   request - ioctl reference
 *   arg - pointer to ioctl data
 * returns:
 *   ioctl result
 */
static int
xioctl(int fd, int request, void *arg)
{
	int             ret;
	int             tries;

	for (tries = IOCTL_RETRY; tries != 0; tries--) {
		ret = ioctl(fd, request, arg);
		if (ret == 0)
			return 0;
		if ((errno != EINTR) && (errno != EAGAIN) && (errno != ETIMEDOUT)) {
			debug("ioctl (%#08x): %s\n", request, strerror(errno));
			return ret;
		}
		debug("ioctl (%#08x) retry)\n", request);
	}
	debug("ioctl (%#08x) retried %d times - giving up: %s)\n", request,
		  IOCTL_RETRY, strerror(errno));

	return ret;
}

static          int32_t
step2val(step_t step)
{
	switch (step) {
	case SMALL:
		return 64;
	case MEDIUM:
		return 128;
	case LARGE:
		return 192;
	case XLARGE:
		return 256;
	default:
		debug("Invalid Pan or Tilt step (%d)", step);
		return 0;
	}
}

static          int32_t
tstep2val(tilt_dir_t dir, step_t step)
{
	int32_t         val;

	if (dir == DOWN) {
		val = step2val(step);
	} else if (dir == UP) {
		val = -step2val(step);
	} else {
		debug("Invalid Tilt direction (%d)", dir);
		val = 0;
	}

	return val;
}

static          int32_t
pstep2val(pan_dir_t dir, step_t step)
{
	int32_t         val;

	if (dir == LEFT) {
		val = step2val(step);
	} else if (dir == RIGHT) {
		val = -step2val(step);
	} else {
		debug("Invalid Pan direction (%d)", dir);
		val = 0;
	}

	return val;
}

int
v4l2_open(const char *device)
{
	return (open(device, O_RDWR | O_NONBLOCK));
}

int
v4l2_close(int fd)
{
	return (close(fd));
}

int
v4l2_pan(int fd, pan_dir_t dir, step_t step)
{
	struct v4l2_ext_controls ctrls = { 0 };
	struct v4l2_ext_control ctrl = { 0 };

	//ctrl.id = V4L2_CID_PAN_RELATIVE;
	ctrl.id = 0x9a0904;
	ctrl.value = pstep2val(dir, step);
	ctrls.count = 1;
	ctrls.controls = &ctrl;

	debug("Pan: %d\n", ctrl.value);
	return (xioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls));
}

int
v4l2_pan_reset(int fd)
{
	struct v4l2_ext_controls ctrls = { 0 };
	struct v4l2_ext_control ctrl = { 0 };

	//ctrl.id = V4L2_CID_PAN_RESET;
	ctrl.id = 0x9a0906;
	ctrl.value = 1;
	ctrls.count = 1;
	ctrls.controls = &ctrl;

	debug("Pan Reset\n");
	return (xioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls));
}

int
v4l2_tilt(int fd, tilt_dir_t dir, step_t step)
{
	struct v4l2_ext_controls ctrls = { 0 };
	struct v4l2_ext_control ctrl = { 0 };

	//ctrl.id = V4L2_CID_TILT_RELATIVE;
	ctrl.id = 0x9a0905;
	ctrl.value = tstep2val(dir, step);
	ctrls.count = 1;
	ctrls.controls = &ctrl;

	debug("AAAAAAAAATilt: %d\n", ctrl.value);
	return (xioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls));
}

int
v4l2_tilt_reset(int fd)
{
	struct v4l2_ext_controls ctrls = { 0 };
	struct v4l2_ext_control ctrl = { 0 };

	//ctrl.id = V4L2_CID_TILT_RESET;
	ctrl.id = 0x9a0907;
	ctrl.value = 1;
	ctrls.count = 1;
	ctrls.controls = &ctrl;

	debug("Tilt Reset\n");
	return (xioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls));
}

int
v4l2_pan_tilt(int fd, pan_dir_t pan_dir, step_t pan_step,
			  tilt_dir_t tilt_dir, step_t tilt_step)
{
	struct v4l2_ext_controls ctrls = { 0 };
	struct v4l2_ext_control ctrl[2] = { {0}, {0} };

	//ctrl[0].id = V4L2_CID_PAN_RELATIVE;
	ctrl[0].id = 0x9a0904;
	ctrl[0].value = pstep2val(pan_dir, pan_step);
	//ctrl[1].id = V4L2_CID_TILT_RELATIVE;
	ctrl[1].id = 0x9a0905;
	ctrl[1].value = tstep2val(tilt_dir, tilt_step);
	ctrls.count = 2;
	ctrls.controls = ctrl;

	debug("Pan & Tilt: %d & %d\n", ctrl[0].value, ctrl[1].value);
	return (xioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls));
}

int
v4l2_pan_tilt_reset(int fd)
{
	struct v4l2_ext_controls ctrls = { 0 };
	struct v4l2_ext_control ctrl = { 0 };

	ctrl.id = V4L2_CID_PANTILT_RESET_LOGITECH;
	ctrl.value = 3;
	ctrls.count = 1;
	ctrls.controls = &ctrl;

	debug("Pan & Tilt Reset\n");
	return (xioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls));
}

int
v4l2_led(int fd, led_mode_t mode, uint8_t freq)
{
	struct v4l2_ext_controls ctrls = { 0 };
	struct v4l2_ext_control ctrl[2] = { {0}, {0} };

	ctrl[0].id = V4L2_CID_LED1_MODE_LOGITECH;
	ctrl[0].value = mode;
	ctrl[1].id = V4L2_CID_LED1_FREQUENCY_LOGITECH;
	ctrl[1].value = freq;
	ctrls.count = 2;
	ctrls.controls = ctrl;

	debug("LED: Mode=%d; Freq=%u\n", ctrl[0].value, ctrl[1].value);
	return (xioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls));
}

int
v4l2_get_brightness(int fd, uint8_t * pval)
{
	struct v4l2_control ctrl;
	int             ret;

	ctrl.id = V4L2_CID_BRIGHTNESS;
	ctrl.value = 0;

	debug("Get Brightness\n");
	ret = xioctl(fd, VIDIOC_G_CTRL, &ctrl);
	if (ret == 0)
		*pval = ctrl.value;
	return ret;
}

int
v4l2_set_brightness(int fd, uint8_t val)
{
	struct v4l2_control ctrl;

	ctrl.id = V4L2_CID_BRIGHTNESS;
	ctrl.value = val;

	debug("Brightness: %u\n", val);
	return (xioctl(fd, VIDIOC_S_CTRL, &ctrl));
}

int
v4l2_get_contrast(int fd, uint8_t * pval)
{
	struct v4l2_control ctrl;
	int             ret;

	ctrl.id = V4L2_CID_CONTRAST;
	ctrl.value = 0;

	debug("Get Contrast");
	ret = xioctl(fd, VIDIOC_G_CTRL, &ctrl);
	if (ret == 0)
		*pval = ctrl.value;
	return ret;
}

int
v4l2_set_contrast(int fd, uint8_t val)
{
	struct v4l2_control ctrl;

	ctrl.id = V4L2_CID_CONTRAST;
	ctrl.value = val;

	debug("Contrast: %u\n", val);
	return (xioctl(fd, VIDIOC_S_CTRL, &ctrl));
}

int
v4l2_get_saturation(int fd, uint8_t * pval)
{
	struct v4l2_control ctrl;
	int             ret;

	ctrl.id = V4L2_CID_SATURATION;
	ctrl.value = 0;

	debug("Get Saturation\n");
	ret = xioctl(fd, VIDIOC_G_CTRL, &ctrl);
	if (ret == 0)
		*pval = ctrl.value;
	return ret;
}

int
v4l2_set_saturation(int fd, uint8_t val)
{
	struct v4l2_control ctrl;

	ctrl.id = V4L2_CID_SATURATION;
	ctrl.value = val;

	debug("Saturation: %u\n", val);
	return (xioctl(fd, VIDIOC_S_CTRL, &ctrl));
}

int
v4l2_get_sharpness(int fd, uint8_t * pval)
{
	struct v4l2_control ctrl;
	int             ret;

	ctrl.id = V4L2_CID_SHARPNESS;
	ctrl.value = 0;

	debug("Get Sharpness\n");
	ret = xioctl(fd, VIDIOC_G_CTRL, &ctrl);
	if (ret == 0)
		*pval = ctrl.value;
	return ret;
}

int
v4l2_set_sharpness(int fd, uint8_t val)
{
	struct v4l2_control ctrl;

	ctrl.id = V4L2_CID_SHARPNESS;
	ctrl.value = val;

	debug("Sharpness: %u\n", val);
	return (xioctl(fd, VIDIOC_S_CTRL, &ctrl));
}

int
v4l2_get_focus(int fd, uint8_t * pval)
{
	struct v4l2_ext_controls ctrls = { 0 };
	struct v4l2_ext_control ctrl = { 0 };
	int             ret;

	ctrl.id = V4L2_CID_FOCUS_LOGITECH;
	ctrls.count = 1;
	ctrls.controls = &ctrl;

	debug("Get Focus\n");
	ret = xioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
	if (ret == 0)
		*pval = ctrl.value;
	return ret;
}

int
v4l2_set_focus(int fd, uint8_t val)
{
	struct v4l2_ext_controls ctrls = { 0 };
	struct v4l2_ext_control ctrl = { 0 };

	ctrl.id = V4L2_CID_FOCUS_LOGITECH;
	ctrl.value = val;
	ctrls.count = 1;
	ctrls.controls = &ctrl;

	debug("Focus: %u\n", ctrl.value);
	return (xioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls));
}

int
v4l2_image_reset(int fd)
{
	struct v4l2_control ctrl;
	int             ret;

	ctrl.id = V4L2_CID_BRIGHTNESS;
	ctrl.value = 128;
	ret = xioctl(fd, VIDIOC_S_CTRL, &ctrl);

	ctrl.id = V4L2_CID_CONTRAST;
	ctrl.value = 32;
	ret |= xioctl(fd, VIDIOC_S_CTRL, &ctrl);

	ctrl.id = V4L2_CID_SATURATION;
	ctrl.value = 32;
	ret |= xioctl(fd, VIDIOC_S_CTRL, &ctrl);

	ctrl.id = V4L2_CID_SHARPNESS;
	ctrl.value = 224;
	ret |= xioctl(fd, VIDIOC_S_CTRL, &ctrl);

	return ret;
}
