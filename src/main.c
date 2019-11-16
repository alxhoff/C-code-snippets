#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static void action_handler(int sig, siginfo_t *siginfo, void *context)
{
	printf("Sending %d from PID: %ld, UID: %ld\n", siginfo->si_signo,
	       (long)siginfo->si_pid, (long)siginfo->si_uid);
}

int main(int argc, char *argv[])
{
	printf("Press CTRL+Z to send SIGTSTP\n");

	struct sigaction act = { { 0 } };

	//has two extra parameters instead of just signal number
	act.sa_sigaction = &action_handler;

	act.sa_flags = SA_SIGINFO;

	if (sigaction(SIGTSTP, &act, NULL) < 0) {
		perror("Sigaction");
		return 1;
	}

	while (1)
		sleep(10);

	return 0;
}
