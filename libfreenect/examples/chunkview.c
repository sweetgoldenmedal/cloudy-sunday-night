/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2013 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libfreenect.h"
#ifdef _MSC_VER
#define HAVE_STRUCT_TIMESPEC
#endif
#include <pthread.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>

pthread_t freenect_thread;
volatile int die = 0;

int g_argc;
char **g_argv;

int window;

pthread_mutex_t gl_backbuf_mutex = PTHREAD_MUTEX_INITIALIZER;

// back: owned by libfreenect (implicit for depth)
// mid: owned by callbacks, "latest frame ready"
// front: owned by GL, "currently being drawn"
uint8_t *depth_mid, *depth_front, *depth_mid_avg;

GLuint gl_depth_tex;

freenect_context *f_ctx;
freenect_device *f_dev;
int freenect_angle = 0;
int freenect_led;


pthread_cond_t gl_frame_cond = PTHREAD_COND_INITIALIZER;
int got_depth = 0;

int showAvgs = 0;

/*
Downsample and unpack pixels to 320*240 on the fly. Mind that even though
we setup libfreenect to return packed data, When we process them with the 
chunk callback they will end up in unpacked form. Converting to mm is also
an option. The best way to do it is by passing a lookup table for raw->mm
as the argument to freenect_set_user. You can then access it here as the ud.
*/

void chunk_cb(void *buffer, void *pkt_data, int pkt_num, int pkt_size,void *ud)
{
	// Put something in here that detects the which 8x8 region we're in
	// Adds to running average
	if(pkt_num == 73 || pkt_num == 146) return;
	uint8_t *raw = (uint8_t *) pkt_data;
	uint16_t *frame=(uint16_t *)buffer;
	if(pkt_num > 219){
		raw += (pkt_num-220) * 12;
		frame += 320 * (pkt_num-2);
	}else if(pkt_num > 146){
		raw += (pkt_num-147) * 12 + 4;
		frame += 320 * (pkt_num-2);
	}else if(pkt_num > 73){
		raw += (pkt_num-74) * 12 + 8;
		frame += 320 * (pkt_num-1);
	}else{
		raw += pkt_num * 12;
		frame += 320 * pkt_num;
	}
	
	int n = 0;
	while(n != 40){
		frame[0] =  (raw[0]<<3)  | (raw[1]>>5);
        frame[1] = ((raw[2]<<9)  | (raw[3]<<1) | (raw[4]>>7) ) & 2047;
        frame[2] = ((raw[5]<<7)  | (raw[6]>>1) )           & 2047;
        frame[3] = ((raw[8]<<5)  | (raw[9]>>3) )           & 2047;
        frame[4] =  (raw[11]<<3)  | (raw[12]>>5);
        frame[5] = ((raw[13]<<9)  | (raw[14]<<1) | (raw[15]>>7) ) & 2047;
        frame[6] = ((raw[16]<<7)  | (raw[17]>>1) )           & 2047;
        frame[7] = ((raw[19]<<5)  | (raw[20]>>3) )           & 2047;
        frame+=8;
        raw+=22;
        n++;
	}
	
}

void DrawGLScene()
{
	pthread_mutex_lock(&gl_backbuf_mutex);

	while (!got_depth) {
		pthread_cond_wait(&gl_frame_cond, &gl_backbuf_mutex);
	}


	uint8_t *tmp;

	if (got_depth) {
		tmp = depth_front;
		depth_front = depth_mid;
		depth_mid = tmp;
		got_depth = 0;
	}

	pthread_mutex_unlock(&gl_backbuf_mutex);

	glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 320, 240, 0, GL_RGB, GL_UNSIGNED_BYTE, depth_front);

	glBegin(GL_TRIANGLE_FAN);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 0); glVertex3f(0,0,0);
	glTexCoord2f(1, 0); glVertex3f(320,0,0);
	glTexCoord2f(1, 1); glVertex3f(320,240,0);
	glTexCoord2f(0, 1); glVertex3f(0,240,0);
	glEnd();

	glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y)
{
	if (key == 27) {
		die = 1;
		pthread_join(freenect_thread, NULL);
		glutDestroyWindow(window);
		free(depth_mid);
		free(depth_front);
		// Not pthread_exit because OSX leaves a thread lying around and doesn't exit
		exit(0);
	}
	if (key == 'w') {
		freenect_angle++;
		if (freenect_angle > 30) {
			freenect_angle = 30;
		}
	}
	if (key == 's') {
		freenect_angle = 0;
	}
	if (key == 'x') {
		freenect_angle--;
		if (freenect_angle < -30) {
			freenect_angle = -30;
		}
	}
	if (key == '1') {
		freenect_set_led(f_dev,LED_GREEN);
	}
	if (key == '2') {
		freenect_set_led(f_dev,LED_RED);
	}
	if (key == '3') {
		freenect_set_led(f_dev,LED_YELLOW);
	}
	if (key == '4') {
		freenect_set_led(f_dev,LED_BLINK_GREEN);
	}
	if (key == '5') {
		// 5 is the same as 4
		freenect_set_led(f_dev,LED_BLINK_GREEN);
	}
	if (key == '6') {
		freenect_set_led(f_dev,LED_BLINK_RED_YELLOW);
	}
	if (key == '0') {
		freenect_set_led(f_dev,LED_OFF);
	}
	if (key == ' ' ){ //spacebar
		showAvgs ^= 1;
	}

	freenect_set_tilt_degs(f_dev,freenect_angle);
}

void ReSizeGLScene(int Width, int Height)
{
	glViewport(0,0,Width,Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, 320, 240, 0, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void InitGL(int Width, int Height)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
    glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_FLAT);

	glGenTextures(1, &gl_depth_tex);
	glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ReSizeGLScene(Width, Height);
}

void *gl_threadfunc(void *arg)
{
	printf("GL thread\n");

	glutInit(&g_argc, g_argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(320, 240);
	glutInitWindowPosition(0, 0);

	window = glutCreateWindow("LibFreenect");

	glutDisplayFunc(&DrawGLScene);
	glutIdleFunc(&DrawGLScene);
	glutReshapeFunc(&ReSizeGLScene);
	glutKeyboardFunc(&keyPressed);

	InitGL(320, 240);

	glutMainLoop();

	return NULL;
}

uint16_t t_gamma[2048];

#define SET_RUNNING_AVG(c, n, x, y, i) depth_mid_avg[y*8+x*3+i] = (c + r_tots[n] * depth_mid_avg[y*8+x*3+i]) / (r_tots[n] + 1);

//#define CMA(n, xn1, CMAn) (xn1 + n*CMAn) / (n+1)
#define NUM_CELLS_X 2
#define NUM_CELLS_Y 2
static uint32_t depth_avgs[NUM_CELLS_X * NUM_CELLS_Y * 3];
static uint32_t depth_tots[NUM_CELLS_X * NUM_CELLS_Y];

void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
	int i;
	uint16_t *depth = (uint16_t*)v_depth;
	pthread_mutex_lock(&gl_backbuf_mutex);
	for (i=0; i<320*240; i++) {
        int pval = t_gamma[depth[i]];
        int lb = pval & 0xff;
        switch (pval>>8) {
            case 0:
                depth_mid[3*i+0] = 255;
                depth_mid[3*i+1] = 255-lb;
                depth_mid[3*i+2] = 255-lb;
                break;
            case 1:
                depth_mid[3*i+0] = 255;
                depth_mid[3*i+1] = lb;
                depth_mid[3*i+2] = 0;
                break;
            case 2:
                depth_mid[3*i+0] = 255-lb;
                depth_mid[3*i+1] = 255;
                depth_mid[3*i+2] = 0;
                break;
            case 3:
                depth_mid[3*i+0] = 0;
                depth_mid[3*i+1] = 255;
                depth_mid[3*i+2] = lb;
                break;
            case 4:
                depth_mid[3*i+0] = 0;
                depth_mid[3*i+1] = 255-lb;
                depth_mid[3*i+2] = 255;
                break;
            case 5:
                depth_mid[3*i+0] = 0;
                depth_mid[3*i+1] = 0;
                depth_mid[3*i+2] = 255-lb;
                break;
            default:
                depth_mid[3*i+0] = 0;
                depth_mid[3*i+1] = 0;
                depth_mid[3*i+2] = 0;
                break;
        }
	}
    
    if ( showAvgs )
    {
        int cellW = 320 / NUM_CELLS_X;
        int cellH = 240 / NUM_CELLS_Y;
        memset( depth_avgs, 0, NUM_CELLS_X * NUM_CELLS_Y * 3 * sizeof( uint32_t ));
        memset( depth_tots, 0, NUM_CELLS_X * NUM_CELLS_Y * sizeof( uint32_t ));
        //get the totals:
        for( int i = 0; i < 320*240; ++i )
        {
            int pval = t_gamma[depth[i]];
            int lb = pval & 0xff;
            int finalC = 0;
            if ( pval>>8 == 3 ) finalC = lb;
            else if ( pval>>8 < 3 ) finalC = 255;
            int x = (i % 320) / cellW;
            int y = (i / 320) / cellH;
            
            depth_avgs[3*(y*NUM_CELLS_X+x) + 0] += finalC; //depth_mid[3*i+0];
            depth_avgs[3*(y*NUM_CELLS_X+x) + 1] += finalC; //depth_mid[3*i+1];
            depth_avgs[3*(y*NUM_CELLS_X+x) + 2] += finalC; //depth_mid[3*i+2];
            ++depth_tots[y*NUM_CELLS_X+x];
        }
        
        //average them:
        for( int i = 0; i < NUM_CELLS_X * NUM_CELLS_Y; ++i )
        {
            depth_avgs[3*i + 0] = depth_avgs[3*i + 0] / depth_tots[i];
            depth_avgs[3*i + 1] = depth_avgs[3*i + 1] / depth_tots[i];
            depth_avgs[3*i + 2] = depth_avgs[3*i + 2] / depth_tots[i];
        }
        
        //then set the depth mid to that:
        for ( int i = 0; i < 320*240; ++i )
        {
            int x = (i % 320) / cellW;
            int y = (i / 320) / cellH;
            depth_mid[3*i+0] = depth_avgs[3*(y*NUM_CELLS_X+x) + 0];
            depth_mid[3*i+1] = depth_avgs[3*(y*NUM_CELLS_X+x) + 1];
            depth_mid[3*i+2] = depth_avgs[3*(y*NUM_CELLS_X+x) + 2];
        }
    }

	got_depth++;
	pthread_cond_signal(&gl_frame_cond);
	pthread_mutex_unlock(&gl_backbuf_mutex);
}

void *freenect_threadfunc(void *arg)
{
	int accelCount = 0;

	freenect_set_tilt_degs(f_dev,freenect_angle);
	freenect_set_led(f_dev,LED_RED);
	freenect_set_depth_callback(f_dev, depth_cb);
	freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT_PACKED));
	freenect_set_depth_chunk_callback(f_dev,chunk_cb);
	freenect_start_depth(f_dev);
	
	printf("'w'-tilt up, 's'-level, 'x'-tilt down, '0'-'6'-select LED mode\n");

	while (!die && freenect_process_events(f_ctx) >= 0) {
		//Throttle the text output
		if (accelCount++ >= 2000)
		{
			accelCount = 0;
			freenect_raw_tilt_state* state;
			freenect_update_tilt_state(f_dev);
			state = freenect_get_tilt_state(f_dev);
			double dx,dy,dz;
			freenect_get_mks_accel(state, &dx, &dy, &dz);
			printf("\r raw acceleration: %4d %4d %4d  mks acceleration: %4f %4f %4f", state->accelerometer_x, state->accelerometer_y, state->accelerometer_z, dx, dy, dz);
			fflush(stdout);
		}

	}

	printf("\nshutting down streams...\n");

	freenect_stop_depth(f_dev);

	freenect_close_device(f_dev);
	freenect_shutdown(f_ctx);

	printf("-- done!\n");
	return NULL;
}

int main(int argc, char **argv)
{
	int res;

	depth_mid = (uint8_t*)malloc(320*240*3);
	depth_mid_avg = (uint8_t*)malloc(8 * 8 * 3);
	depth_front = (uint8_t*)malloc(320*240*3);

	printf("Kinect camera test\n");

	int i;
	for (i=0; i<2048; i++) {
		float v = i/2048.0;
		v = powf(v, 3)* 6;
		t_gamma[i] = v*6*256;
	}

	g_argc = argc;
	g_argv = argv;

	if (freenect_init(&f_ctx, NULL) < 0) {
		printf("freenect_init() failed\n");
		return 1;
	}

	freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);
	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));

	int nr_devices = freenect_num_devices (f_ctx);
	printf ("Number of devices found: %d\n", nr_devices);

	int user_device_number = 0;
	if (argc > 1)
		user_device_number = atoi(argv[1]);

	if (nr_devices < 1) {
		freenect_shutdown(f_ctx);
		return 1;
	}

	if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
		printf("Could not open device\n");
		freenect_shutdown(f_ctx);
		return 1;
	}

	res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
	if (res) {
		printf("pthread_create failed\n");
		freenect_shutdown(f_ctx);
		return 1;
	}

	// OS X requires GLUT to run on the main thread
	gl_threadfunc(NULL);

	return 0;
}


/*
    else
    {
        int r_tots[8*8];
        memset( r_tots, 0, 8*8*sizeof(int) );
        for (i=0; i<320*240; i++) {
            int x = (i % 320) / 40;
            int y = (i / 320) / 30;
            int n = y*8+x;
            int pval = t_gamma[depth[i]];
            int lb = pval & 0xff;
            switch (pval>>8) {
                case 0:
                    SET_RUNNING_AVG(255, n, x, y, 0);
                    SET_RUNNING_AVG(255-lb, n, x, y, 1);
                    SET_RUNNING_AVG(255-lb, n, x, y, 2);
                    break;
                case 1:
                    SET_RUNNING_AVG(255, n, x, y, 0);
                    SET_RUNNING_AVG(lb, n, x, y, 1);
                    SET_RUNNING_AVG(0, n, x, y, 2);
                    break;
                case 2:
                    SET_RUNNING_AVG(255-lb, n, x, y, 0);
                    SET_RUNNING_AVG(255, n, x, y, 1);
                    SET_RUNNING_AVG(0, n, x, y, 2);
                    break;
                case 3:
                    SET_RUNNING_AVG(0, n, x, y, 0);
                    SET_RUNNING_AVG(255, n, x, y, 1);
                    SET_RUNNING_AVG(lb, n, x, y, 2);
                    break;
                case 4:
                    SET_RUNNING_AVG(0, n, x, y, 0);
                    SET_RUNNING_AVG(255-lb, n, x, y, 1);
                    SET_RUNNING_AVG(255, n, x, y, 2);
                    break;
                case 5:
                    SET_RUNNING_AVG(0, n, x, y, 0);
                    SET_RUNNING_AVG(0, n, x, y, 1);
                    SET_RUNNING_AVG(255-lb, n, x, y, 2);
                    break;
                default:
                    SET_RUNNING_AVG(0, n, x, y, 0);
                    SET_RUNNING_AVG(0, n, x, y, 1);
                    SET_RUNNING_AVG(0, n, x, y, 2);
                    break;
            }
            r_tots[n] += 1;
        }
        for (i=0; i<320*240; i++) {
            int x = (i % 320) / 40;
            int y = (i / 320) / 30;
            
            depth_mid[3*i+0] = depth_mid_avg[y*8+x+0];
            depth_mid[3*i+1] = depth_mid_avg[y*8+x+1];
            depth_mid[3*i+2] = depth_mid_avg[y*8+x+2];
        }
        
    }
*/
