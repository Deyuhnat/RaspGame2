

typedef struct
{
    volatile unsigned int x;
    volatile unsigned int y;
    volatile unsigned int frame;
    volatile unsigned int state;
    volatile unsigned int delay;
} Explosion;

typedef struct
{   
    volatile unsigned int x;
    volatile unsigned int y;
    volatile unsigned int frame;
    volatile unsigned int state;
    volatile unsigned int delay;
    Explosion explosion[5];
} Bombs;

typedef struct
{
    char direction;
    int distance;
} moves;

typedef struct
{   
    volatile int is_alive;
    unsigned int frame_width;
    unsigned int frame_height;
    int is_npc;
    unsigned int frame_max;
    volatile int health;
    volatile unsigned int damage;
    volatile unsigned int range;

    volatile unsigned int prior_x;
    volatile unsigned int prior_y;
    volatile unsigned int x;
    volatile unsigned int y;
    volatile unsigned int moveup_frame_offset;
    volatile unsigned int movedown_frame_offset;
    volatile unsigned int moveleft_frame_offset;
    volatile unsigned int moveright_frame_offset;
    volatile int offset;
    int move_index;
    unsigned int bomb_num;
    int got_hit;
    Bombs bomb[20];
} human;

int collision_detection(int map[][28],human humans[], unsigned int object_x, unsigned int object_y);
void drawGameAsset(int frame, unsigned int offset_x, unsigned int offset_y, unsigned int width, unsigned int height, const unsigned long *frame_array[]);
human character1_init(int x, int y, int moveup_offset, int is_npc,unsigned int frame_max, unsigned int frame_width, unsigned int frame_height);
unsigned int absolute(int num);
human controlCharater(int map[][28],human players[], human player1, char c, int is_collision,int *hit_player, const unsigned long *frame_array[]);
human move(int map[][28],human players[], human npc, moves moves[], unsigned int move_size, int is_collision,int *hit_player, const unsigned long *frame_array[]);
int tracking_player_on_map(human player, int map[][28], char c);
int npc_hit_detection(human humans[], unsigned int object_x, unsigned int object_y);