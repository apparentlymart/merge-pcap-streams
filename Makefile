
all: bin/merge-pcap-streams

bin/merge-pcap-streams: src/main.c
	gcc src/main.c -o bin/merge-pcap-streams -lpcap
