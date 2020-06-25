#include <stdio.h>

#include "libusb-1.0/libusb.h"

#define BULK_EP_OUT     0x82
#define BULK_EP_IN      0x02

static void print_devs(libusb_device **devs)
{
    libusb_device *dev;
    int i = 0, j = 0;
    uint8_t path[8];

    while ((dev = devs[i++]) != NULL) {
        struct libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) {
            fprintf(stderr, "failed to get device descriptor");
            return;
        }

        printf("%04x:%04x (bus %d, device %d)",
               desc.idVendor, desc.idProduct,
               libusb_get_bus_number(dev), libusb_get_device_address(dev));

        r = libusb_get_port_numbers(dev, path, sizeof(path));
        if (r > 0) {
            printf(" path: %d", path[0]);
            for (j = 1; j < r; j++)
                printf(".%d", path[j]);
        }
        printf("\n");
    }
}

int main(void)
{
    libusb_device **devs;
    int r;
    ssize_t cnt;

    r = libusb_init(NULL);
    if (r < 0)
        return r;
    libusb_device_handle* handle;
    int kernelDriverDetached     = 0;  /* Set to 1 if kernel driver detached */
    int numBytes                 = 0;  /* Actual bytes transferred. */
    uint8_t buffer[64];
    cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0)
        return (int) cnt;

    print_devs(devs);
    libusb_free_device_list(devs, 1);

    handle = libusb_open_device_with_vid_pid(0, 0x093a, 0x2510);
    if (!handle)
    {
        fprintf(stderr, "Unable to open device.\n");
        return 1;
    }
    /* Check whether a kernel driver is attached to interface #0. If so, we'll
   * need to detach it.
   */
    if (libusb_kernel_driver_active(handle, 0))
    {
        r = libusb_detach_kernel_driver(handle, 0);
        if (r == 0)
        {
            kernelDriverDetached = 1;
        }
        else
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

    unsigned char data[4];
    int actual_length;
    r = libusb_bulk_transfer(handle, BULK_EP_IN, data, sizeof(data), &actual_length, 5000);
    if (r == 0 && actual_length == sizeof(data)) {
        // results of the transaction can now be found in the data buffer
        // parse them here and report button press
        int button = data[0];
        switch (button) {
            case 1:
                fprintf(stdout, "Left button");
                break;
            case 2:
                fprintf(stdout, "Right button");
                break;
            case 3:
                fprintf(stdout, "Left+Right button");
                break;
            case 4:
                fprintf(stdout, "Middle button");
                break;
            case 5:
                fprintf(stdout, "Middle+Left button");
                break;
            case 6:
                fprintf(stdout, "Middle+Right button");
                break;
        }
        fprintf(stdout,"Coordinates %i,%i", data[1], data[2]);
        data[3] > 0 ? fprintf(stdout, "Scrolling up.") : fprintf(stdout, "Scrolling down.");
    } else {
        fprintf(stdout, "Actual length %i", actual_length);
        fprintf(stderr, "Error reading interface.\n");
    }

    r = libusb_release_interface(handle, 0);
    if (0 != r)
    {
        fprintf(stderr, "Error releasing interface.\n");
    }

    libusb_exit(NULL);
    return 0;
}