/*
    Copyright (C)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MEresultHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.
    If not, see <http://www.gnu.org/licenses/>.

    Klabautermann Software
    Uwe Jantzen
    Weingartener Straße 33
    76297 Stutensee
    Germany

    file        contour.c

    date        14.04.2019

    author      Uwe Jantzen (jantzen@klabautermann-software.de)

    brief       Handle the contour device

    details

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/hiddev.h>
#include <stdio.h>
#include <fcntl.h>
#include "errors.h"
#include "globals.h"
#include "debug.h"
#include "utils.h"
#include "contour.h"


#define HIDDEV_BUFFER_LEN                       64
#define MAX_HID_DEVICES                         16
#define CONTOUR_USB_VENDOR_CODE             0x1a79
#define CONTOUR_PATH                   "/dev/usb/"
#define DEV_NAME                          "hiddev"


static const short int device_codes[] =
    {
    CONTOUR_USB_CODE,
    CONTOUR_USB_NEXT_CODE,
    CONTOUR_NEXT_ONE
    };


static int usage_code = 0;


/*  function        int open_contour( int * contour_type )

    brief           Searches for a Bayer Contour USB device and if found returns
                    a file handle to it.

    param[out]      int * contour_type, type of contour device found

    return          int, if negative, error code
                         if positive, handle to the contour device
*/
int open_contour( int * contour_type )
    {
    int hiddev_num;
    char device[256];
    int handle;
    struct hiddev_report_info info;
    struct hiddev_devinfo device_info;
    struct hiddev_usage_ref uref;
    int result = NOERR;

    *contour_type = 0;                                                          // no device found ...

    for( hiddev_num = 0, handle = -1; hiddev_num < MAX_HID_DEVICES; ++hiddev_num )
        {
        sprintf(device, "%s%s%d", CONTOUR_PATH, DEV_NAME, hiddev_num);
        debug("Try to open device %s\n", device);
        rotating_bar();
        handle = open(device, O_RDWR);
        debug("handle : %d\n", handle);

        if( handle < 0 )
            continue;

        info.report_type = HID_REPORT_TYPE_OUTPUT;
        info.report_id = HID_REPORT_ID_FIRST;
        result = ioctl(handle, HIDIOCGREPORTINFO, &info);
        if( result < 0 )
            {
            result = -errno;
            debug("Getting report information failed : %d\n", errno);
            goto LoopEnd;
            }

        debug("Type :             %0u\n", info.report_type);
        debug("Id :               %0u\n", info.report_id);
        debug("Number of fields : %0u\n", info.num_fields);

        uref.report_type = HID_REPORT_TYPE_OUTPUT;
        uref.report_id = 0x0;
        uref.field_index = 0;
        uref.usage_index = 0;

        result = ioctl(handle, HIDIOCGUCODE, &uref);
        if( result < 0 )
            {
            result = -errno;
            debug("Getting usage code failed : %d\n", result);
            goto LoopEnd;
            }

        usage_code = uref.usage_code;

        result = ioctl(handle, HIDIOCGDEVINFO, &device_info);
        if( result < 0 )
            {
            result = -errno;
            debug("Getting device information failed : %d\n", errno);
            goto LoopEnd;
            }

        debug("Bustype :          %0u\n", device_info.bustype);
        debug("Bus Number :       %0u\n", device_info.busnum);
        debug("Device Number :    %0u\n", device_info.devnum);
        debug("Interface Number : %0u\n", device_info.ifnum);
        debug("Vendor Id :        0x%04x\n", device_info.vendor);
        debug("Product Id :       0x%04x\n", device_info.product);
        debug("Version :          %0u\n", device_info.version);
        debug("Num of Apps :      %0u\n", device_info.num_applications);

        if( device_info.vendor == CONTOUR_USB_VENDOR_CODE )
            {
            int i;

            for( i = 0; i < (sizeof(device_codes) / sizeof(short int)); ++i )
                {
                if( device_info.product == device_codes[i] )
                    {
                    *contour_type = device_info.product;
                    return handle;
                    }
                }
            }
        debug("Vendor and product doesn't match\n");
LoopEnd:
        close(handle);
        return result;
        }

    return handle;
    }


/*  function        void close_contour( int handle )

    brief           Closes the handle to the contour device handle.

    param[in]       int handle, handle to the contour device
*/
void close_contour( int handle )
    {
    if( handle >= 0 )
        {
        debug("Closing handle %d\n", handle);
        close(handle);
        }
    }


/*  function        int wait_for_contour( int * contour_type )

    brief           Waits for a Contour USB device to become attached.
                    If a device is just attached when entering this function it
                    returns with an error because it is not possible to read
                    from a device more than once.
                    Timeout is set to 30 seconds.
                    Not interruptable yet!

    param[out]      int * contour_type, type of contour device found

    return          int, if negative, error code
                         if positive, file handle
*/
int wait_for_contour( int * contour_type )
    {
    int handle;
    int max_checks = 60;

    handle = open_contour(contour_type);
    if( handle > 0 )
        {
        printf("\nCommunication can't be established if Contour device is just attached!\n");
        printf("Please remove the Contour device and wait some seconds.\n");
        printf("Then FIRST start the program and SECOND attach the Contour device.\n\n");
        close_contour(handle);
        exit(1);
        }

    do
        {
        rotating_bar();
        usleep(500 * 1000);
        handle = open_contour(contour_type);
        --max_checks;
        }
    while( ( handle < 0 ) && ( max_checks ) );

    if( max_checks <= 0 )
        printf("\nNo Contour device found in between 30 seconds\n");

    return handle;
    }


/*  function        int read_contour( int handle, char * buffer, int size )

    brief           Reads from contour device

    param[in]       int handle, handle to the contour device
    param[out]      char * buffer, buffer to fill in the bytes read
    param[in]       int size, size of buffer

    return          int, if negative, error
                         if positive, number of bytes read
*/
int read_contour( int handle, char * buffer, int size )
    {
    struct hiddev_event inbuffer[HIDDEV_BUFFER_LEN];
    int result;
    int n;
    int i;

    if( size < HIDDEV_BUFFER_LEN )
        return -ERR_BUFFER_LEN;

    result = read(handle, inbuffer, sizeof(inbuffer));
    if( result < 0 )
        return -errno;

    n = result / sizeof(struct hiddev_event);
    for( i = 0; i < n; ++i )
        buffer[i] = inbuffer[i].value;

    return n;
    }


/*  function        int write_contour( int handle, char const * buffer, int size )

    brief           Write bytes to the contour device

    param[in]       int handle, handle to the contour device
    param[in]       char const * buffer, bytes to write
    param[in]       int size, number of bytes to write

    return          int, 0 or negative error code
*/
int write_contour( int handle, char const * buffer, int size )
    {
    struct hiddev_usage_ref ref;
    struct hiddev_report_info info;
    int result = 0;
    int idx;

    --size;

    ref.report_id = *buffer++;
    ref.report_type = HID_REPORT_TYPE_OUTPUT;
    ref.field_index = 0;

    ref.usage_code = usage_code;

    for( idx = 0; idx < size; ++idx )
        {
        ref.usage_index = idx;
        ref.value = *buffer++;

        result = ioctl(handle, HIDIOCSUSAGE, &ref);
        if( result )
            goto err;
       }

    info.report_type = HID_REPORT_TYPE_OUTPUT;
    info.report_id =  0;
    info.num_fields = 1;

    result = ioctl(handle, HIDIOCSREPORT, &info);
    if( result )
        goto err2;

    return 0;
err2:
    verbose("HIDIOCSREPORT\n");
err:
    verbose("Error in IOCTL: %s\n", strerror(errno));
    result = -errno;

    return result;
    }
