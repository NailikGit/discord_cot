CFLAGS := -g -DCCORD_SIGINTCATCH -pthread -ldiscord -lcurl -fsanitize=address

ping_pong:
	clang ping_pong.c -o build/ping_pong $(CFLAGS)

clean:
	rm -r build && mkdir build
