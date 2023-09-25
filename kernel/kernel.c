
#include "../uart/uart1.h"
#include "framebf.h"
#include "mbox.h"
#include "game.h"
#include "image.h"
#include "video.h"

#define MAX_CMD_SIZE 100
#define MAX_HISTORY 10
#define NULL ((void *)0)

// the compiler randomly ask for memcpy so i included it here
// some manual string functions (cannot include "string.h")

char *welcome_message =
    "####### ####### ####### #######  #####  #        #####    ###   \n"
    "#       #       #          #    #     # #    #  #     #  #   #  \n"
    "#       #       #          #          # #    #  #     # #     # \n"
    "#####   #####   #####      #     #####  #    #   ###### #     # \n"
    "#       #       #          #    #       #######       # #     # \n"
    "#       #       #          #    #            #  #     #  #   #  \n"
    "####### ####### #######    #    #######      #   #####    ###   \n"
    "\n"
    "\n"
    " #     #                                                           \n"
    " #  #  # ###### #       ####   ####  #    # ######    #####  ####  \n"
    " #  #  # #      #      #    # #    # ##  ## #           #   #    # \n"
    " #  #  # #####  #      #      #    # # ## # #####       #   #    # \n"
    " #  #  # #      #      #      #    # #    # #           #   #    # \n"
    " #  #  # #      #      #    # #    # #    # #           #   #    # \n"
    "  ## ##  ###### ######  ####   ####  #    # ######      #    ####  \n"
    "\n"
    "\n"
    " d888b  d8888b.  .d88b.  db    db d8888b.    .d88b.  .d8888.\n"
    "88' Y8b 88  `8D .8P  Y8. 88    88 88  `8D   .8P  Y8. 88'  YP\n"
    "88      88oobY' 88    88 88    88 88oodD'   88    88 `8bo.   \n"
    "88  ooo 88`8b   88    88 88    88 88~~~     88    88   `Y8b. \n"
    "88. ~8~ 88 `88. `8b  d8' 88b  d88 88        `8b  d8' db   8D \n"
    " Y888P  88   YD  `Y88P'  ~Y8888P' 88         `Y88P'  `8888Y' \n"
    "\n"
    "Developed by Group 26\n"
    "\n";

char *commands[] = {
    "help",
    "clear",
    "setcolor",
    "play"};

int num_commands = sizeof(commands) / sizeof(commands[0]);
char *colors[] = {
    "BLACK",
    "RED",
    "GREEN",
    "YELLOW",
    "BLUE",
    "PURPLE",
    "CYAN",
    "WHITE"};

char *ansiColors[] = {
    "\033[30m", // BLACK
    "\033[31m", // RED
    "\033[32m", // GREEN
    "\033[33m", // YELLOW
    "\033[34m", // BLUE
    "\033[35m", // PURPLE
    "\033[36m", // CYAN
    "\033[37m", // WHITE
};

char *ansiBackgroundColors[] = {
    "\033[40m", // BLACK
    "\033[41m", // RED
    "\033[42m", // GREEN
    "\033[43m", // YELLOW
    "\033[44m", // BLUE
    "\033[45m", // PURPLE
    "\033[46m", // CYAN
    "\033[47m", // WHITE
};

char cmd_history[MAX_HISTORY][MAX_CMD_SIZE];
int history_index = 0;
int current_index = 0;

void showmenu()
{
    uart_puts(
        "\n\tEnter a command from below:\n"
        "\tdisplaytext\tDisplay text on screen\n"
        "\tshowimage\tDisplay a small image\n"
        "\tshowlargeimage\tDisplay a scrollable large image\n"
        "\tshowvideo\tDisplay a video\n"
        "\tplay\t\tPlay game\n");
}

void uppercaseLetter(char *str)
{
    while (*str)
    {
        if (*str >= 'a' && *str <= 'z')
        {
            *str = *str - ('a' - 'A');
        }
        str++;
    }
}

void setcolor(const char *textColor, const char *backgroundColor)
{
    char upperTextColor[MAX_CMD_SIZE] = {0};
    char upperBackgroundColor[MAX_CMD_SIZE] = {0};

    if (textColor)
    {
        strcpy(upperTextColor, textColor);
        uppercaseLetter(upperTextColor);
    }
    if (backgroundColor)
    {
        strcpy(upperBackgroundColor, backgroundColor);
        uppercaseLetter(upperBackgroundColor);
    }

    if (textColor)
    {
        for (int i = 0; i < 8; i++)
        {
            if (strcmp(upperTextColor, colors[i]) == 0)
            {
                uart_puts(ansiColors[i]);
            }
        }
    }
    if (backgroundColor)
    {
        for (int i = 0; i < 8; i++)
        {
            if (strcmp(upperBackgroundColor, colors[i]) == 0)
            {
                uart_puts(ansiBackgroundColors[i]);
            }
        }
    }
}

void drawLargeImageScroll() {
    uart_puts("Use WASD to scroll. Press Enter to quit scroll mode ");

    // Assuming you have image1, image2, and image3 loaded and ready to be displayed
    unsigned char *images[] = {image2image2, image3image3, image4image4};
    int currentImageIndex = 0; // start with the first image
    
    int y = 0;
    int x = 0;
    
    drawImage(images[currentImageIndex], x, y, 1920, 1080); // draw the first image

    while (1) {
        char c = uart_getc();

        if (c == 'w') {
            y -= 20;
        } else if (c == 's') {
            y += 20;
        } else if (c == 'd') {
            currentImageIndex = (currentImageIndex + 1) % 3; // go to the next image in the array, and loop back to the first image after the last one
            x = 0; // reset x coordinate whenever the image changes
            y = 0; // reset y coordinate whenever the image changes
        } else if (c == 'a') {
            currentImageIndex = (currentImageIndex - 1 + 3) % 3; // go to the previous image in the array, and loop back to the last image after the first one
            x = 0; // reset x coordinate whenever the image changes
            y = 0; // reset y coordinate whenever the image changes
        } else if (c == '\n') {
            uart_puts("\n");
            break;
        }

        clearScreen(0);
        drawImage(images[currentImageIndex], x, y, 1920, 1080); // draw the current image
    }
}
void playVideo()
{
    uart_puts("Playing video \n");
    uart_puts("Press Enter to stop ");
    char c = uart_get_char();
    int i = 0;
    while (c != '\n')
    {
        if (i > 7)
            i = 0;
        // printf("%d\n", i);
        drawImage(video_frames[i], 0, 0, 453, 421);
        wait_ms(60000);
        i++;
        c = uart_get_char();
    }
    uart_puts("\nVideo stopped");
}
void wait_ms(unsigned int n)
{
    register unsigned long f, t, r;

    // Get the current counter frequency
    asm volatile("mrs %0, cntfrq_el0"
                 : "=r"(f));
    // Read the current counter
    asm volatile("mrs %0, cntpct_el0"
                 : "=r"(t));
    // Calculate expire value for counter
    t += ((f / 1000) * n) / 1000;
    do
    {
        asm volatile("mrs %0, cntpct_el0"
                     : "=r"(r));
    } while (r < t);
}

void display_prompt()
{
    uart_puts("GroupOS> ");
}
void execute_command(char *cmd)
{
    if (strcmp(cmd, commands[0]) == 0)
    {
        uart_puts("*Supported commands:\n");
        uart_puts("help, clear, setcolor, showinfo, play, showimage, showlargeimage, showvideo, displaytext\n\n");
    }

    else if (strcmp(cmd, "showimage") == 0)
    {
        clearScreen(0);
        // framebf_init(1024, 720);
        drawImage(image1image1, 0, 0, 480, 270);
    }
    else if (strcmp(cmd, "showlargeimage") == 0)
    {
        clearScreen(0);
        // framebf_init(1024, 720);
        drawLargeImageScroll();
    }
    else if (strcmp(cmd, "showvideo") == 0)
    {
        clearScreen(0);
        // framebf_init(1024, 720);
        playVideo();
    }
    else if (strcmp(cmd, "displaytext") == 0)
    {
        clearScreen(0);
        drawOnScreen();
    }

    else if (strcmp(cmd, commands[1]) == 0)
    {
        // for (int i = 0; i < 50; i++)
        //     uart_puts("\n");
        uart_puts("\033[2J\033[H");
    }
    else if (strncmp(cmd, commands[2], 8) == 0) // setcolor command
    {
        char *token = strtok(cmd, " ");
        char *textColor = NULL;
        char *backgroundColor = NULL;

        while (token != NULL)
        {
            if (strcmp(token, "-t") == 0)
            {
                token = strtok(NULL, " ");
                textColor = token;
            }
            else if (strcmp(token, "-b") == 0)
            {
                token = strtok(NULL, " ");
                backgroundColor = token;
            }
            token = strtok(NULL, " ");
        }

        setcolor(textColor, backgroundColor);
    }

    else if (strcmp(cmd, commands[3]) == 0)
    {
        clearScreen(0);
        game_menu();
    }

    else
    {
        uart_puts("Unrecognized command!\n");
    }
}

void cli()
{
    static char cli_buffer[MAX_CMD_SIZE];
    static int index = 0;
    char c;
    while (1)
    {
        c = uart_getc();
        uart_sendc(c); // echo the character
        // Auto-completion
        if (c == '\t')
        {
            for (int i = 0; i < num_commands; i++)
            {
                if (strncmp(cli_buffer, commands[i], strlen(cli_buffer)) == 0)
                {
                    strcpy(cli_buffer, commands[i]);
                    index = strlen(cli_buffer);

                    uart_puts("\rGroupOS>                                                                                            "); // Clear line
                    uart_puts("\rGroupOS> ");
                    uart_puts(cli_buffer); // print the entire buffer, including the auto-completed part

                    break;
                }
            }
        }
        // History UP
        else if (c == '_')
        {
            if (current_index == history_index)
            {
                // If currently at the most recent command, save current input first
                strcpy(cmd_history[history_index % MAX_HISTORY], cli_buffer);
            }
            current_index = (current_index - 1 + MAX_HISTORY) % MAX_HISTORY;
            strcpy(cli_buffer, cmd_history[current_index]);
            index = strlen(cli_buffer);
            uart_puts("\rGroupOS>                                                                                                   ");
            uart_puts("\rGroupOS> ");
            uart_puts(cli_buffer);
        }
        // History DOWN
        else if (c == '+')
        {
            current_index = (current_index + 1) % MAX_HISTORY;
            if (current_index == history_index)
            {
                // If going back to the most recent command, retrieve the saved input
                strcpy(cli_buffer, cmd_history[current_index]);
            }
            else
            {
                strcpy(cli_buffer, cmd_history[current_index]);
            }
            index = strlen(cli_buffer);
            uart_puts("\rGroupOS>                                                                                                   "); // Clear line
            uart_puts("\rGroupOS> ");
            uart_puts(cli_buffer);
        }
        else if (c == 0x08 || c == 0x7F) // ASCII for backspace key and del key
        {
            if (index > 0) // check there are characters to delete
            {
                index = index - 1;
                cli_buffer[index] = '\0';                                                                                              // "delete" the last character in the buffer
                uart_puts("\b \b");                                                                                                    // echo backspace, space, backspace to terminal
                uart_puts("\rGroupOS>                                                                                              "); // Clear the line
                uart_puts("\rGroupOS> ");
                uart_puts(cli_buffer);
            }
            else
            {
                uart_puts("\rGroupOS> "); // prevent delete through display prompt
            }
        }
        else if (c != '\n')
        {
            cli_buffer[index++] = c;
        }
        else
        {
            cli_buffer[index] = '\0';

            // Store in history
            if (index > 0)
            {
                strcpy(cmd_history[history_index % MAX_HISTORY], cli_buffer);
                history_index++;
                if (history_index >= MAX_HISTORY)
                {
                    history_index = 0;
                }
            }
            current_index = history_index;
            uart_puts("\n");
            if (index > 0)
            {
                execute_command(cli_buffer);
            }
            else
            {
                uart_puts("");
            }
            index = 0;
            display_prompt();
        }
    }
}

void main()
{
    framebf_init(1024, 720);
    // intitialize UART
    uart_init();
    setcolor("red", "black");
    uart_puts(welcome_message);
    showmenu();
    display_prompt();
    drawOnScreen();

    while (1)
    {
        cli();
    }
}
