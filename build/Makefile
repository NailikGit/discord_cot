CFLAGS := -g -DCCORD_SIGINTCATCH -pthread -ldiscord -lcurl #-fsanitize=address

all: ping_pong

ping_pong:
	clang ../src/ping_pong.c -o ping_pong $(CFLAGS)

clean:
	rm ping_pong
