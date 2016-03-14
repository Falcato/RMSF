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
\
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


int
controlos(int c)
{
	int             fd, ret=0;
	state_t         state = INIT;
	step_t          step = LARGE;
	const char     *device = "/dev/video1";
	unsigned int    gpio = 166;


	fd = v4l2_open(device);
	if (fd < 0) {
		fprintf(stderr, "V4L2 Open '%s': %s\n", device, strerror(errno));
		return fd;
	}

	printf("\nWebcam Control Test Application (%s, GPIO%u)\n", device, gpio);
	printf
		("--------------------------------------------------------------------------\n");
/*	printf
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
	printf("Press 'Q' to quit.\n\n");*/


	
			switch (c) {
			case 'p':
			case 'P':
				ret = v4l2_pan_reset(fd);
				break;
			case 't':
			case 'T':
				ret = v4l2_tilt_reset(fd);
				break;
			case 'q':
			case 'Q':
				state = END;
				break;
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
				state = END;
				break;
			}
	v4l2_close(fd);

	return 0;
}
