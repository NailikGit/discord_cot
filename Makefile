nutte: src/nutte.c
	mkdir -p build
	clang $< -o build/$@ -g -Wall -Wextra -DCCORD_SIGINTCATCH -pthread -ldiscord -lcurl

.PHONY: clean
clean:
	rm -rf build
