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
 * $Id: cam_control.c 593 2011-09-28 12:25:07Z jose.goncalves $
 *---------------------------------------------------------------------------+*/

#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "v4l2_control.h"


typedef enum {
	INIT, ONE, TWO, THREE, THREE_A, END
} state_t;

static int
toggle_light(unsigned int gpio)
{
	const char     *fmt = "/sys/class/gpio/gpio%u/value";
	char            name[48];
	int             fd;
	int             ret;
	char            state[16];

	snprintf(name, sizeof(name), fmt, gpio);

	fd = open(name, O_RDWR);
	if (fd < 0) {
		perror(name);
		return errno;
	}

	ret = read(fd, state, sizeof(state) - 1);

	if (ret >= 0) {
		if (memcmp(state, "0", 1) == 0) {
			ret = write(fd, "1", 1);
		} else if (memcmp(state, "1", 1) == 0) {
			ret = write(fd, "0", 1);
		} else {
			state[ret] = 0;
			fprintf(stderr, "Invalid GPIO value '%s'\n", state);
		}
	}

	close(fd);

	if (ret < 0) {
		perror(name);
		return errno;
	}

	return 0;
}

/* Unbuffered version of getchar() */
static int
getkey(void)
{
	int             buf;
	int             ret;
	struct termios  old;
	struct termios  new;

	if (tcgetattr(0, &old) != 0)
		return EOF;

	new = old;
	new.c_lflag &= ~(ICANON | ECHO);
	new.c_cc[VMIN] = 1;
	new.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &new) != 0)
		return EOF;

	buf = 0;
	ret = read(0, &buf, 1);

	if (tcsetattr(0, TCSADRAIN, &old) != 0)
		return EOF;

	return (ret < 0 ? EOF : buf);
}

static          uint8_t
inc_img(uint8_t val, unsigned int inc)
{
	if ((val + inc) > 255)
		return 255;
	return (val + inc);
}

static          uint8_t
dec_img(uint8_t val, unsigned int dec)
{
	if (val < dec)
		return 0;
	return (val - dec);
}

int
main(int argc, char *argv[])
{
	int             fd, c, ret;
	state_t         state = INIT;
	step_t          step = LARGE;
	uint8_t         step_img = 8;
	uint8_t         step_freq = 10;
	uint8_t         val;
	led_mode_t      led_mode = LED_AUTO;
	uint8_t         led_freq = 100;
	const char     *device = "/dev/video3";
	unsigned int    gpio = 166;

	if (argc >= 2)
		device = argv[1];

	if (argc >= 3)
		gpio = atoi(argv[2]);

	fd = v4l2_open(device);
	if (fd < 0) {
		fprintf(stderr, "V4L2 Open '%s': %s\n", device, strerror(errno));
		return fd;
	}

	printf("\nWebcam Control Test Application (%s, GPIO%u)\n", device, gpio);
	printf
		("--------------------------------------------------------------------------\n");
	printf
		("Left/Right: Pan | Up/Down: Tilt | PgUp/PgDn, Home/End: Pan + Tilt\n");
	printf
		("'b'/'B': Bright | 'c'/'C': Contrast | 's'/'S': Saturation | 'h'/'H': Sharp\n");
	printf
		("'L': LED mode (Off/On/Blink/Auto) | 'f'/'F': LED frequency (@ Blink Mode)\n");
	printf
		("'P': Pan Reset | 'T': Tilt Reset | '5'(Keypad): Pan + Tilt Reset\n");
	printf("'o'/'O': Focus | 'I': Light (Off/On) | 'R': Full Reset\n");
	printf
		("--------------------------------------------------------------------------\n");
	printf("Press 'Q' to quit.\n\n");

	do {
		c = getkey();
#ifdef DEBUG
		if (isprint(c))
			printf("'%c'\n", c);
		else
			printf("'%02X'\n", c);
#endif
		if (c == EOF) {
			perror("getkey");
			break;
		}

		ret = 0;

		switch (state) {
		case INIT:
			switch (c) {
			case 0x1B:
				state = TWO;
				break;
			case 'b':
				ret = v4l2_get_brightness(fd, &val);
				if (ret == 0)
					ret = v4l2_set_brightness(fd, dec_img(val, step_img));
				break;
			case 'B':
				ret = v4l2_get_brightness(fd, &val);
				if (ret == 0)
					v4l2_set_brightness(fd, inc_img(val, step_img));
				break;
			case 'c':
				ret = v4l2_get_contrast(fd, &val);
				if (ret == 0)
					ret = v4l2_set_contrast(fd, dec_img(val, step_img));
				break;
			case 'C':
				ret = v4l2_get_contrast(fd, &val);
				if (ret == 0)
					v4l2_set_contrast(fd, inc_img(val, step_img));
				break;
			case 's':
				ret = v4l2_get_saturation(fd, &val);
				if (ret == 0)
					ret = v4l2_set_saturation(fd, dec_img(val, step_img));
				break;
			case 'S':
				ret = v4l2_get_saturation(fd, &val);
				if (ret == 0)
					v4l2_set_saturation(fd, inc_img(val, step_img));
				break;
			case 'h':
				ret = v4l2_get_sharpness(fd, &val);
				if (ret == 0)
					ret = v4l2_set_sharpness(fd, dec_img(val, step_img));
				break;
			case 'H':
				ret = v4l2_get_sharpness(fd, &val);
				if (ret == 0)
					v4l2_set_sharpness(fd, inc_img(val, step_img));
				break;
			case 'p':
			case 'P':
				ret = v4l2_pan_reset(fd);
				break;
			case 't':
			case 'T':
				ret = v4l2_tilt_reset(fd);
				break;
			case 'r':
			case 'R':
				ret = v4l2_pan_tilt_reset(fd);
				ret |= v4l2_set_focus(fd, 32);
				ret |= v4l2_image_reset(fd);
				ret |= v4l2_led(fd, LED_AUTO, 0);
				break;
			case 'f':
				if (led_freq < step_freq)
					led_freq = 1;
				else
					led_freq -= step_freq;
				ret = v4l2_led(fd, led_mode, led_freq);
				break;
			case 'F':
				if (led_freq > (255 - step_freq))
					led_freq = 255;
				else
					led_freq += step_freq;
				ret = v4l2_led(fd, led_mode, led_freq);
				break;
			case 'o':
				ret = v4l2_get_focus(fd, &val);
				if (ret == 0)
					ret = v4l2_set_focus(fd, dec_img(val, step_img));
				break;
			case 'O':
				ret = v4l2_get_focus(fd, &val);
				if (ret == 0)
					v4l2_set_focus(fd, inc_img(val, step_img));
				break;
			case 'l':
			case 'L':
				if (led_mode < LED_AUTO)
					led_mode++;
				else
					led_mode = LED_OFF;
				ret = v4l2_led(fd, led_mode, 0);
				break;
			case 'i':
			case 'I':
				toggle_light(gpio);
				break;
			case '+':
				if (step < XLARGE)
					step++;
				break;
			case '-':
				if (step > SMALL)
					step--;
				break;
			case 'q':
			case 'Q':
				state = END;
				break;
			}
			break;

		case TWO:
			if (c == '[')
				state = THREE;
			else if (c == 'O')
				state = THREE_A;
			else
				state = INIT;
			break;

		case THREE:
			switch (c) {
			case 'A':	// Arrow Up
				ret = v4l2_tilt(fd, UP, step);
				break;
			case 'B':	// Arrow Down
				ret = v4l2_tilt(fd, DOWN, step);
				break;
			case 'C':	// Arrow Right
				ret = v4l2_pan(fd, RIGHT, step);
				break;
			case 'D':	// Arrow Left
				ret = v4l2_pan(fd, LEFT, step);
				break;
			case 'E':	// Keypad '5'
				ret = v4l2_pan_tilt_reset(fd);
				break;
			case '1':	// Keypad Home
				ret = v4l2_pan_tilt(fd, LEFT, step, UP, step);
				break;
			case '4':	// Keypad  End
				ret = v4l2_pan_tilt(fd, LEFT, step, DOWN, step);
				break;
			case '5':	// Page Up
				ret = v4l2_pan_tilt(fd, RIGHT, step, UP, step);
				break;
			case '6':	// Page Down
				ret = v4l2_pan_tilt(fd, RIGHT, step, DOWN, step);
				break;
			}
			state = INIT;
			break;

		case THREE_A:
			switch (c) {
			case 'H':	// Home
				ret = v4l2_pan_tilt(fd, LEFT, step, UP, step);
				break;
			case 'F':	// End
				ret = v4l2_pan_tilt(fd, LEFT, step, DOWN, step);
				break;
			}
			state = INIT;
			break;

		default:
			state = INIT;
			break;
		}

		if (ret != 0)
			fprintf(stderr, "V4L2 error: %s\n", strerror(errno));

	} while (state != END);

	v4l2_close(fd);

	return 0;
}
