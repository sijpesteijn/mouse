#include <stdio.h>
#include "libusb-1.0/libusb.h"


int main(int argc, char *argv[])
{
    fprintf(stdout, "\n\tTrying to open usb mouse on endpoint 0x81 with vendorId 0x093a and productId 0x2510 \n");

    libusb_device **devs;
    int r;
    ssize_t cnt;

    r = libusb_init(NULL);
    if (r < 0)
        return r;
    libusb_device_handle* handle;
    cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0)
        return (int) cnt;

    libusb_free_device_list(devs, 1);

    handle = libusb_open_device_with_vid_pid(0, 0x093a, 0x2510);
    if (!handle)
    {
        fprintf(stderr, "Unable to open device.\n");
        return 1;
    }
    /* Check whether a kernel driver is attached to interface #0. If so, we'll  need to detach it. */
    if (libusb_kernel_driver_active(handle, 0))
    {
        r = libusb_detach_kernel_driver(handle, 0);
        if (r != 0)
        {
            fprintf(stderr, "Error detaching kernel driver.\n");
            return 1;
        }
    }
    /* Claim interface #0. */
    r = libusb_claim_interface(handle, 0);
    if (r != 0)
    {
        fprintf(stderr, "Error claiming interface.\n");
        return 1;
    }
    fprintf(stdout, "\n\tReading 500 mouse values.  \n");
    unsigned char data[4];
    int actual_length;
    int i = 0;
    while(i++ < 500) {
        r = libusb_interrupt_transfer(handle, 0x81, data, sizeof(data), &actual_length, 0);
        if (r == 0 && actual_length == sizeof(data)) {
            // results of the transaction can now be found in the data buffer
            // parse them here and report button press
            int button = data[0];
            switch (button) {
                case 1:
                    fprintf(stdout, "Left button\n");
                    break;
                case 2:
                    fprintf(stdout, "Right button\n");
                    break;
                case 3:
                    fprintf(stdout, "Left+Right button\n");
                    break;
                case 4:
                    fprintf(stdout, "Middle button\n");
                    break;
                case 5:
                    fprintf(stdout, "Middle+Left button\n");
                    break;
                case 6:
                    fprintf(stdout, "Middle+Right button\n");
                    break;
            }
            fprintf(stdout, "Coordinates %i,%i\n", data[1], data[2]);
            data[3] > 0 ? fprintf(stdout, "Scrolling up.\n") : fprintf(stdout, "Scrolling down.\n");
        } else {
            fprintf(stdout, "Actual length %i", actual_length);
            fprintf(stderr, "Error reading interface.\n");
        }

    }
    r = libusb_release_interface(handle, 0);
    if (0 != r)
    {
        fprintf(stderr, "Error releasing interface.\n");
    }

    libusb_exit(NULL);
    return 0;
}