
#include "../uart/uart1.h"
#include "framebf.h"
#include "map.h"
#include "mbox.h"
#include "logo.h"
#include "magewalking.h"
#include "boss/dragons.h"
#include "./sub_boss/knight.h"      // character.h here
#include "map_array.h"
#include "stringg.h"
void *memcpy(void *dest, const void *src, int count)
{
    char *d = dest;
    const char *s = src;
    while (count--)
    {
        *d++ = *s++;
    }
    return dest;
}

void *memset(void *dest, int value, int count)
{
    unsigned char *ptr = (unsigned char *)dest;
    unsigned char byteValue = (unsigned char)value;

    for (int i = 0; i < count; i++)
    {
        ptr[i] = byteValue;
    }

    return dest;
}
void npc1_plant_bomb(human npc1, human player)
{
    if (absolute(npc1.x - player.x) < 50 && absolute(npc1.y - player.y) < 50)
    {
        uart_puts("detected by enemy");
    }
}

void character_take_damage(human *characters[], int *got_hit_player,int *take_damaged_once,int size){
        for(int i = 0; i <size; i++){
            if(*take_damaged_once){
                if(*got_hit_player == i){
                    characters[i]->health -= 1;
                    characters[i]->got_hit = 1;
                    *got_hit_player = -1;
                    *take_damaged_once = 0;
                }
            }
        }

        
        
}

int once = 0;
int take_damaged_once = 1;
int got_hit_player = -1;
void play_game()
{  

    moves npc1_moves[] = {
        {'d', 85},
        {'a', 100},
        {'s', 100},
        {'w', 100}};

    moves npc2_moves[] = {
        {'w', 100},
        {'d', 80},
        {'s', 100},
        {'a', 100}};

    moves* all_npc_moves[] = {npc1_moves, npc2_moves};

    // wall block width and height is 38 and 46
    // span npc1 in the below formar
    human player1 = character1_init(38, 46 * 3, 9,0,8,mage_width,mage_height);
    human npc1 = character1_init(38 * 10, 46 * 10, 9,1,8,mage_width,mage_height);
    human npc2 = character1_init(38 * 5, 46 * 9, 9,1,8,mage_width,mage_height);

    // 1 seconds = 1000000
    set_wait_timer(1, 10000); // set 10ms

    while (1)
    {   
        
        // Draw map
        if (once == 0)
        {
            once = 1;
            draw_map_from_array(map2);
        }

        human *characters[] = {&player1,&npc1,&npc2};   // Write only
        human characters2[] = {*characters[0],*characters[1],*characters[2]};  // Read only
        
        char c = getUart();

        *characters[0] = controlCharater(map2,characters2, *characters[0], c, tracking_player_on_map(*characters[0], map2, c),&got_hit_player, mage_walking_allArray);
        
        character_take_damage(&characters,&got_hit_player,&take_damaged_once,3);

        int timer = set_wait_timer(0, 0);
        if (timer)
        {
            if ((timer % 10) == 0)  // every 100ms
            { 
                //*characters[0] = controlCharater(characters2, *characters[0],'t', 0, &got_hit_player,mage_walking_allArray);
                
                for(int i = 0; i< 3; i++){
                    if(characters[i]->got_hit){
                        *characters[i] = controlCharater(map2,characters2, *characters[i],'t', 0, &got_hit_player,mage_walking_allArray);
                        characters[i]->got_hit = 0;
                        if(characters[i]->health == 0){
                            characters[i]->is_alive = 0;
                        }
                        //uart_dec(absolute(characters[i]->health));
                        //uart_dec(i);
                        //uart_sendc('\n');
                    }else{
                        if(i>0){    // exclude player
                            *characters[i] = move(map2,characters2, *characters[i], all_npc_moves[i-1], sizeof(npc1_moves) / sizeof(npc1_moves[0]), 0,&got_hit_player,mage_walking_allArray); 
                        }
                    }

                }
                
                //npc2 = move(characters2, npc2, npc2_moves, sizeof(npc2_moves) / sizeof(npc2_moves[0]), 0,&got_hit_player,mage_walking_allArray);
                
                //int x = npc_hit_detection(characters2, npc1.x,npc1.y);


                for (int i = 0; i < player1.bomb_num; i++)
                {
                    player1.bomb[i].delay++;
                    if (player1.bomb[i].delay > 20)
                    { // delay 2 seconds
                        player1.bomb[i].frame++;
                        if (player1.bomb[i].frame > 11)
                        {
                            player1.bomb[i].frame = 0;
                            player1.bomb[i].state = 0;
                            player1.bomb[i].delay = 0;
                            take_damaged_once = 1;
                        }
                    }
                }
            }

            set_wait_timer(1, 10000); // reset 10ms timer
        }
    }
}

void sub_boss(){
    
    
    human player1 = character1_init(38, 46 * 3, 9,0,8,mage_width,mage_height);
    human knight1 = character1_init(38 * 3, 46 * 5, 9,1,8,59,72);
    human knight2 = character1_init(38 * 3, 46 * 10, 9,1,8,59,72);
    human knight3 = character1_init(38 * 23, 46 * 5, 9,1,8,59,72);
    human knight4 = character1_init(38 * 23, 46 * 10, 9,1,8,59,72);

    set_wait_timer(1, 10000); // set 10ms

    int knight_timer[] = {10,25,40,45};
    int counter[] = {0,0,0,0};
    int move_signal[] = {0,0,0,0};
    int player_prior_x[] = {knight1.x,knight2.x,knight3.x, knight4.x};
    int player_prior_y[] = {knight1.y, knight2.y,knight3.y, knight4.y};
    int prior_player_health = player1.health;
    int game_status = 0;
    draw_stats(5);
    while (1)
    {   
        if(game_status == 1){
            break;
        }
        
        if(prior_player_health != player1.health){
            drawRectARGB32(0,0,500,50,0x00000000,1);
            draw_stats(player1.health);
            prior_player_health = player1.health;
        }
        // Draw map
        if (once == 0)
        {
            once = 1;
            draw_map_from_array(map4);
        }

        human *characters[] = {&player1,&knight1,&knight2,&knight3,&knight4};   // Write only
        human characters2[] = {*characters[0],*characters[1],*characters[2], *characters[3], *characters[4]};  // Read only

        char c = getUart();

        *characters[0] = controlCharater(map4,characters2, *characters[0], c, tracking_player_on_map(*characters[0], map4, c),&got_hit_player, mage_walking_allArray);

        character_take_damage(&characters,&got_hit_player,&take_damaged_once,4);
        
        
        
        int timer = set_wait_timer(0, 0);
        if (timer)
        {
            if ((timer % 10) == 0)
            {    
                //uart_dec(move_signal[0]);
                //uart_sendc('\n');
                for(int i = 0; i< 4; i++){
                    if(characters[i]->got_hit){
                        move_signal[i-1] = 4;   // move to huting phase
                        characters[i]->got_hit = 0;
                        if(characters[i]->health <= 0){
                            move_signal[i-1] = 3;   // move to dying phase
                        }
                    }

                }
                
                for(int i = 0; i < 4; i++){
                    if(move_signal[i] != 3){        // Take hit phase
                        counter[i]++;
                        if(counter[i] == knight_timer[i] && move_signal[i] == 2){ // attack phase
                            if(npc_hit_detection(characters2, player1.x,player1.y)){
                                *characters[0] = controlCharater(map4,characters2, *characters[0], 't', tracking_player_on_map(*characters[0], map4, 't'),&got_hit_player, mage_walking_allArray);
                                player1.health -=1;
                                if(player1.health < 0){
                                    player1.is_alive =0;
                                    game_status = 1;
                                }
                            }
                            if(player1.x != player_prior_x[i] || player1.y != player_prior_y[i]){
                                move_signal[i] = 0;
                            }
                            counter[i] = 0;
                        }else if(counter[i] == knight_timer[i]){        // moving phase
                            player_prior_x[i] = player1.x;
                            player_prior_y[i] = player1.y;
                            move_signal[i] = 1;
                            counter[i] = 0;
                        }
                    }
                    
                }
            
                knight1 = control_knight(player1, knight1,&move_signal[0],player_prior_x[0],player_prior_y[0],5);
                knight2 = control_knight(player1, knight2,&move_signal[1],player_prior_x[1],player_prior_y[1],7);
                knight3 = control_knight(player1, knight3,&move_signal[2],player_prior_x[2],player_prior_y[2],10);
                knight4 = control_knight(player1, knight4,&move_signal[3],player_prior_x[3],player_prior_y[3],8);

                for (int i = 0; i < player1.bomb_num; i++)
                {
                    player1.bomb[i].delay++;
                    if (player1.bomb[i].delay > 20)
                    { // delay 2 seconds
                        player1.bomb[i].frame++;
                        if (player1.bomb[i].frame > 11)
                        {
                            player1.bomb[i].frame = 0;
                            player1.bomb[i].state = 0;
                            player1.bomb[i].delay = 0;
                            take_damaged_once = 1;
                        }
                    }
                }

            }

            set_wait_timer(1, 10000); // reset 10ms timer
        }
    }
}

void game_menu(){
    int i = 0;
    set_wait_timer(1, 10000); // set 10ms
   
    int game_status = 0;
    while (1)
    {   
        char c = getUart();
        if(c == 'q'){
            break;
        }
        if (c == 'p'){
            clearScreen(0);
            sub_boss();
            uart_puts("its joever");
            break;
        }
        int timer = set_wait_timer(0, 0);
        if (timer)
        {
            if ((timer % 10) == 0)
            { 
                c = uart_get_char();
                draw_map_from_array(map4);
                drawGameAsset(i,400,150,dragon_boss_width,dragon_boss_height,dargon_boss_allArray);
                drawString(centerPosition(strlen("BOMBERKING")), 400, "BOMBERKING", 0xd);
                drawString(centerPosition(strlen("PRESS p TO PLAY, q TO QUIT")), 500, "PRESS p TO PLAY, q TO QUIT", 0xd);
                //drawGameAsset(i,400,400,logo_width,logo_height,epd_logo_allArray);
                i++;
                if(i > 7){
                    i = 0;
                }

            }

            set_wait_timer(1, 10000); // reset 10ms timer
            c = uart_get_char();
        }
    }
    
}