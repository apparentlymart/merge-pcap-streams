
#include <pcap.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

pcap_dumper_t *dumper;

int main(int argc, char **argv) {
    int i, select_rc;
    fd_set read_fds;
    pcap_t **pcaps;
    char pcap_error[PCAP_ERRBUF_SIZE];

    pcaps = malloc(sizeof(pcap_t*) * argc);

    FD_ZERO(&read_fds);
    for (i = 1; i < argc; i++) {
        int fd = atoi(argv[i]);
        FILE *f;
        f = fdopen(fd, "r");
        FD_SET(fd, &read_fds);

        pcaps[i] = pcap_fopen_offline(f, (char*)&pcap_error);
        if (pcaps[i] == NULL) {
            fprintf(stderr, "%s", pcap_error);
        }
    }

    // FIXME: This assumes there's at least one fd passed. It'll probably
    // crash hard if none are passed, but it would be better to fail with
    // a decent error message.
    dumper = pcap_dump_fopen(pcaps[1], stdout);

    while (1) {
        select_rc = select(sizeof(read_fds) * 8, &read_fds, NULL, NULL, NULL);

        for (i = 1; i < argc; i++) {
            pcap_t *pcap = pcaps[i];
            int fd = fileno(pcap_file(pcap));
            if (FD_ISSET(fd, &read_fds)) {
                pcap_dispatch(pcap, -1, &pcap_dump, (u_char*)dumper);
            }
        }
    }
    return 0;
}
