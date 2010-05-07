#define RESET     0
#define BRIGHT    1
#define DIM       2
#define UNDERLINE 3
#define BLINK     4
#define REVERSE   7
#define HIDDEN    8

#define BLACK     0
#define RED       1
#define GREEN     2
#define YELLOW    3
#define BLUE      4
#define MAGENTA   5
#define CYAN      6
#define WHITE     7

static void color_on(int attr, int fg);
static void color_reset(void);

static void color_on(int attr, int fg)
{
    char command[13];

    sprintf(command, "%c[%d;%dm", 0x1B, attr, fg + 30);
    printf("%s", command);
}

static void color_reset()
{
    char command[13];

    sprintf(command, "%c[%dm", 0x1B, 0);
    printf("%s", command);

}

