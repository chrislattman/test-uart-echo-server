#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int main(void) {
    int fd;
    struct termios options;
    const char *buf = "Hello\n";
    size_t nbyte, recv_idx = 0;
    ssize_t res;
    char received[256];

    fd = open("/dev/cu.usbserial-14320", O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("/dev/cu.usbserial-14320");
        return -1;
    }

    if (tcflush(fd, TCIOFLUSH) < 0) {
        perror("tcflush");
        close(fd);
        return -1;
    }
    tcgetattr(fd, &options);
    // options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    // options.c_oflag &= ~(ONLCR | OCRNL);
    // options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    // read timeout
    // options.c_cc[VTIME] = 1;
    // options.c_cc[VMIN] = 0;
    cfsetispeed(&options, B9600); // using macro to ensure standard speed
    cfsetospeed(&options, B9600);
    if (tcsetattr(fd, TCSANOW, &options) < 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    nbyte = strlen(buf) + 1;
    res = write(fd, buf, nbyte);
    if (res != (ssize_t) nbyte) {
        perror("write");
        close(fd);
        return -1;
    }

    do {
        if (recv_idx == 1 && received[recv_idx - 1] == '\0') { // weird case
            recv_idx = 0;
        }
        res = read(fd, received + recv_idx, 1);
        if (res != 1) {
            perror("read");
            close(fd);
            return -1;
        }
        ++recv_idx;
    } while (recv_idx < sizeof(received) && received[recv_idx - 1] != '\n');
    received[recv_idx - 1] = '\0';
    puts(received);

    return 0;
}
