#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

//defines

#define CTRL_KEY(k) ((k) & 0x1f)

//data

struct termios orig_termios;

//Functions that work with the terminal

void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) //checking for errors
    die("tcsetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr"); //checking for errors
  atexit(disableRawMode);
  
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_iflag &= ~(ICRNL | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); 
  raw.c_cc[VMIN] = 0; //how many bytes need to be read before returning
  raw.c_cc[VTIME] = 100; //input wait time, 1 = .1 seconds
  
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char editorReadKey() {
  int nread;
  char c;
  
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  
  return c;
}

void editorRefreshScreen() {
  write(STDOUT_FILENO, "\x1b[2J", 4); // \x1b is an escape sequence that allows us to use text formatting tasks
  write(STDOUT_FILENO, "\x1b[H", 3);
}

void editorProcessKeypress() {
  char c = editorReadKey();
  
  switch (c) {
    case CTRL_KEY('q'):
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;
  }
}

int main() {
  enableRawMode();
  
  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }
  return 0;
}