#define _GNU_SOURCE

#include <libexplain/tcgetattr.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>

#define PORT_NAME "/dev/ttyS0"
#define PORT_SPEED B38400

const char *port_name = PORT_NAME;

static void serial_handler(int sig, siginfo_t *siginfo, void *context)
{
	ssize_t read_len;
	unsigned char recv;

	printf("in handler\n");

	read_len = read(siginfo->si_fd, &recv, 1);
	if (read_len == 1)
		printf("Byte received: %c\n", recv);
}

int main(int argc, char *argv[])
{
	printf("hello world: %d\n", getpid());

	int serial_fd, file_status;
	struct termios termSettings;
	struct sigaction act = { 0 };

	/** open serial port as read and write, does not cause the terminal to become  */
	/**     the controlling terminal for the process and reading will fail if  */
	/**     there is no data available */
	serial_fd = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);

	if (serial_fd < 0) {
		perror("Opening serial port failed");
		return -1;
	}

	printf("Terminal opened\n");

	if (tcgetattr(serial_fd, &termSettings) < 0) {
		perror("Getting terminal attributes failed");
		printf("Error reason: %s\n",
		       explain_tcgetattr(serial_fd, &termSettings));
		goto error;
	}

	/** set the terminal into raw mode such that input is available character by character */
	cfmakeraw(&termSettings);
	/** Set the baudrate of the serial connection */
	cfsetspeed(&termSettings, PORT_SPEED);

	/** Set modifications immediately (TCSANOW) */
	if (tcsetattr(serial_fd, TCSANOW, &termSettings) < 0) {
		perror("Setting terminal settings failed");
		goto error;
	}

	/** Set signal and callback for serial ready */
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = serial_handler;

	/** Set sigaction mask to ignore all signals except SIGIO during execution of handler */
	sigfillset(&act.sa_mask);
	sigdelset(&act.sa_mask, SIGIO);

	/** Set callback */
	if (sigaction(SIGIO, &act, NULL) < 0) {
		perror("Setting sigaction failed");
		goto error;
	}

	printf("Handler set\n");

	/** Set async flags on fd, not sure if needed since serial fd was opened with  */
	/**     async flags */
	file_status = fcntl(serial_fd, F_GETFL);
	file_status |= O_ASYNC | O_NONBLOCK;
	fcntl(serial_fd, F_SETFL, file_status);
	/** Set signal to produce when i/o is ready */
	fcntl(serial_fd, F_SETSIG, SIGIO);
	/** Make signals sent to current proc */
	fcntl(serial_fd, F_SETOWN, getpid());

	while (1) {
		printf("Waiting...\n");
		sleep(10);
	}

	close(serial_fd);
	return 0;

error:
	close(serial_fd);
	return -1;
}
