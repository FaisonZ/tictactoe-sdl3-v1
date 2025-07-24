gcc src/tictactoe.c -o build/tictactoe/bin/tictactoe `pkg-config --cflags --libs sdl3` -Wl,-rpath='$ORIGIN/../lib'
