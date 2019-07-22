// this is my frist c progmrm, dnont bluyy me

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL_mixer.h> // i can't program anything audio related worth shit so
#include <SDL2/SDL.h>
#include <errno.h>


// Singular subtitle
struct sub {
    long when;   /* when to show the subtitle in ms */
    char* what; /* the actual subtitle */
};

// the delimiter is cut off. It's A Feature™
char* fgetuntil(FILE* fp, char delimiter) {
    char* chars;
    char  i = (char)0xFF;
    int   size = 0;
    long  skbefore = ftell(fp);

    do {
        i = fgetc(fp);
        size++;
    } while (i != EOF && i != delimiter);
    // go back
    fseek(fp, skbefore, 0);
    // malloc chars and read
    chars = malloc(size);
    if (fread(chars, 1, size-1, fp)<size-1) {printf("\nRead error in fgetuntil()\n"); exit(1);}
    fgetc(fp);
    chars[size-1] = 0;

    return chars;
}

// basically VB Left$()
char* left(char* string, int amount) {
    int i;
    char* out;
    
    out = malloc(amount+1);
    for (i=0; i<=amount; i++) {
        out[i] = string[i];
    }
    out[amount-1]=0x0;

    return out;
}

// basically VB Right$()
char* right(char* string, int amount) {
    int i;
    char* out;

    out = malloc(amount+1);
    for (i=strlen(string)-amount; i<strlen(string); i++) {
        out[i-(strlen(string)-amount)] = string[i]; //saves an extra step by copy 0x0
    }

    return out;
}

// imagine string[index:]
char* rightfrom(char* string, int index) {
    int i;
    char* out;

    out = malloc(strlen(string)-index+1);
    for (i=index; i<strlen(string); i++) {
        out[i-index] = string[i]; // saves an extra step by copying 0x0
    }

    return out;
}

// okay this is literally InStr() from VB but only one character is supported
int instr(char* string, char chara) {
    int i = 0;

    do {
        i++;
    } while (!(string[i]==chara || i>strlen(string)));

    return i;
}

// turn line into sub
struct sub createsub(char* line) {
    int        splitpoint = 0;
    struct sub thesub;

    splitpoint = instr(line,' ');

    if (splitpoint==1 || splitpoint==strlen(line)-1) {printf("\n!! SUB PARSE ERROR !!\nCan't find a space in line '%s'", line); exit(-6);}

    thesub.when = atoi(left(line, splitpoint-1));
    thesub.what = rightfrom(line, splitpoint+1);

    return thesub;
}

// get filesize of file
long fsize(FILE* fp) {
    long skafter;
    long skbefore = ftell(fp);

    fseek(fp, 0, SEEK_END);
    skafter = ftell(fp);
    fseek(fp, 0, skbefore);

    return skafter;
}

// count number of delimiters in file
long fcount(FILE* fp, char delimiter) {
    long skbefore = ftell(fp);
    long out = 0;
    char i;

    fseek(fp, 0, 0);
    while (1) {
        i = fgetc(fp);
        if (i==delimiter) out++;
        if (i==EOF) break;
    }
    fseek(fp, 0, skbefore);

    return out;
}

int repeatuntil = -1;
int repeatfreq = -1;
char* repeatwhat = "";

// process subway sandwich transaction have a nice day eat more subways please we're going out of buisness
int subproc(char* sub) {
    int i;
    int   mode = 0;
    int   sublen = strlen(sub);
    char* temp;
    char* temp2;

    for (i=0; i<sublen; i++) {
        switch (sub[i])
        {
            case '\\':
                i++;
                switch (sub[i]) {
                    case '\\':
                        putchar('\\');
                        break;
                    case 'n':
                        putchar('\n');
                        break;
                    case 'r':
                        putchar('\r');
                        break;
                    case 'E':
                        exit(0);
                        break;
                    case '~':
                        i++;
                        temp = rightfrom(sub,i);           // cut off before numbers
                        temp2 = left(temp,instr(temp,'}')); // cut off after numbers
                        free(temp);
                        repeatfreq = atoi(left(temp2,instr(temp2,',')));
                        repeatuntil = atoi(rightfrom(temp2,instr(temp2,',')+1));
                        repeatwhat = left(sub,i-2);
                        fflush(stdout);
                        return 1; // switch to repeat mode
                        break;
                    default:
                        printf("uh");
                        break;
                }
                break;
            default:
                putchar(sub[i]);
                break;
        }
    fflush(stdout);
    }
    
    return mode;
}

int sleepms(long a) {
    int ns;

    ns = usleep(a*1000);
    printf("ERR %i\n", errno); 
    
    return ns;
}

// y'know
int main(int argc, char* argv[]) {
    Mix_Music*  mediafile;
    FILE*       subfile;
    struct sub* subs;
    int         i;
    int         mode;
    long        sublen;
    long        startms;

    // we need argv[1] and argv[2], corresponding to mediafile and subfile
    if (argc != 3) {
        printf("\n*achem*\n\n");
        printf("%s [mediafile] [subfile]\n\n", argv[0]);
        printf("mediafile: An audio file to play while subfile prints.\n");
        printf("subfile: A file containing subtitles in my dumb format.\n\n");
        return 1;
    }

    printf("Initializing SDL mixer...\r");
    fflush(stdout);

    // Start SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf(
            "\nSDL couldn't initialize!\n%s\n",
            SDL_GetError()
        );
        return -1;
    }
    // Start SDL_mixer
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0) {
        printf(
            "\nSDL mixer couldn't initialize!\n%s\n",
            Mix_GetError()
        );
        return -2;
    }

    printf("Loading mediafile...     \r");
    fflush(stdout);

    // Load mediafile
    mediafile = Mix_LoadMUS(argv[1]);
    if (mediafile == NULL) {
        printf(
            "\nSDL mixer couldn't open mediafile!\n%s\n",
            Mix_GetError()
        );
        return -3;
    }

    printf("Opening subfile...       \r");
    fflush(stdout);

    // Open subfile
    subfile = fopen(argv[2], "r");
    if (subfile == NULL) {
        printf("\nCouldn't open subfile!\n");
        return -4;
    }

    printf("Processing subfile...    \r");
    fflush(stdout);

    // get each sub and put it in the table of subs
    sublen = fcount(subfile,'\n');
    subs = malloc(sizeof(struct sub) * (sublen+1));
    for (i=0; i<sublen; i++) {
        subs[i] = createsub(fgetuntil(subfile, '\n'));
        if (i>0) {
            if (subs[i].when < subs[i-1].when) {
                printf("\n!! SUBFILE PARSE ERROR !!\nSub %li starts before the last, which will lock up the program!\n", subs[i].when);
                return -5;
            }
        }
    }

    fclose(subfile);

    /* wow we're actually gonna show the subs now this must be the easiest part
    ...



    NOT */

    printf("                         \r");
    fflush(stdout);

    Mix_PlayMusic(mediafile, 1);
    startms = SDL_GetTicks();
    mode = 0; i = 0;
    while (i<sublen) {
        switch(mode) {
            case 0: // default
                sleepms(( startms + subs[i].when ) - SDL_GetTicks());
                mode = subproc(subs[i].what);
                i++;
                break;
            case 1: // repeating
                if ((SDL_GetTicks()-startms) % repeatfreq == 0) {
                    printf("%s",repeatwhat);
                    fflush(stdout);
                }
                if ((SDL_GetTicks()-startms) >= repeatuntil) {
                    mode = 0;
                }
                break;
        }
    }

    return 0;
}