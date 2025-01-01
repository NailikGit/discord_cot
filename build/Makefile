CFLAGS := -g -DCCORD_SIGINTCATCH -pthread -ldiscord -lcurl #-fsanitize=address

all: ping_pong

ping_pong:
	clang src/ping_pong.c -o build/ping_pong $(CFLAGS)

clean:
	rm -r build && mkdir build
