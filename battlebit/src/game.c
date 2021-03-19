//
// Created by carson on 5/20/20.
//

#include <stdlib.h>
#include <stdio.h>
#include "game.h"

// STEP 9 - Synchronization: the GAME structure will be accessed by both players interacting
// asynchronously with the server.  Therefore the data must be protected to avoid race conditions.
// Add the appropriate synchronization needed to ensure a clean battle.

static game * GAME = NULL;

void game_init() {
    if (GAME) {
        free(GAME);
    }
    GAME = malloc(sizeof(game));
    GAME->status = CREATED;
    game_init_player_info(&GAME->players[0]);
    game_init_player_info(&GAME->players[1]);
}

void game_init_player_info(player_info *player_info) {
    player_info->ships = 0;
    player_info->hits = 0;
    player_info->shots = 0;
}

int game_fire(game *game, int player, int x, int y) {
    // Step 5 - This is the crux of the game.  You are going to take a shot from the given player and
    // update all the bit values that store our game state.
    //
    //  - You will need up update the players 'shots' value
    //  - you You will need to see if the shot hits a ship in the opponents ships value.  If so, record a hit in the
    //    current players hits field
    //  - If the shot was a hit, you need to flip the ships value to 0 at that position for the opponents ships field
    //
    //  If the opponents ships value is 0, they have no remaining ships, and you should set the game state to
    //  PLAYER_1_WINS or PLAYER_2_WINS depending on who won.
}

unsigned long long int xy_to_bitval(int x, int y) {
    // Step 1 - implement this function.  We are taking an x, y position
    // and using bitwise operators, converting that to an unsigned long long
    // with a 1 in the position corresponding to that x, y
    //
    // x:0, y:0 == 0b00000...0001 (the one is in the first position)
    // x:1, y: 0 == 0b00000...10 (the one is in the second position)
    // ....
    // x:0, y: 1 == 0b100000000 (the one is in the eighth position)
    //
    // you will need to use bitwise operators and some math to produce the right
    // value.

    unsigned long long value;

    if (x > 7 || x < 0 || y > 7 || y < 0){

        value = 0;

    } else{

        unsigned int bit_shifts = (1 * x) + (8 * y);

        value = 1ull << bit_shifts;
    }
    return value;
}

struct game * game_get_current() {
    return GAME;
}

int game_load_board(struct game *game, int player, char * spec) {
    // Step 2 - implement this function.  Here you are taking a C
    // string that represents a layout of ships, then testing
    // to see if it is a valid layout (no off-the-board positions
    // and no overlapping ships)
    //

    // if it is valid, you should write the corresponding unsigned
    // long long value into the Game->players[player].ships data
    // slot and return 1
    //
    // if it is invalid, you should return -1

    //      RULES FOR A BAD BOARD:

    //      1.) If the spec isn't a certain length, it's a bad board
    //      2.) If there is a negative value (-1) in the string, it's a bad board.
    //      3.) If there is both [c and C, b and B, d and D, s and S, p and p]
    //      4.) If they ship goes outside the board's boundries

    struct game *gameon = game_get_current();
    struct player_info *player_info = &gameon->players[player];

    if(spec == NULL){
        return -1;
    }


    int length = 0;

    int c = 0;
    int b = 0;
    int d = 0;
    int s = 0;
    int p = 0;

    char dir;

    for(int k = 0; k < 15; k+= 3){
        if(spec[k] == 'c'){
            length = 5;
            dir = 'v';
            c++;
            if (c > 1){
                return -1;
            }

        } else if(spec[k] == 'b'){
            length = 4;
            dir = 'v';
            b++;
            if (b > 1){
                return -1;
            }

        } else if(spec[k] == 's') {
            length = 3;
            dir = 'v';
            s++;
            if (s > 1) {
                return -1;
            }

        } else if(spec[k] == 'd') {
            length = 3;
            dir = 'v';
            d++;
            if (d > 1) {
                return -1;
            }

        } else if(spec[k] == 'p') {
            length = 2;
            dir = 'v';
            p++;
            if (p > 1) {
                return -1;
            }

        } else if(spec[k] == 'C') {
            length = 5;
            dir = 'h';
            c++;
            if (c > 1) {
                return -1;
            }

        }

        else if(spec[k] == 'B') {
            length = 4;
            dir = 'h';
            b++;
            if (b > 1) {
                return -1;
            }

        } else if(spec[k] == 'P') {
            length = 2;
            dir = 'h';
            p++;
            if (p > 1) {
                return -1;
            }

        } else if(spec[k] == 'D') {
            length = 3;
            dir = 'h';
            d++;
            if (d > 1) {
                return -1;
            }

        } else if(spec[k] == 'S') {
            length = 3;
            dir = 'h';
            s++;
            if (s > 1) {
                return -1;
            }

        } else{
            return (-1);
        }

        int start_x = spec[k+1] - '0';
        int start_y = spec[k+2] - '0';


        if(start_x < 0 || start_y < 0) {return -1;}
        if(start_x > 7 || start_y > 7) {return -1;}


        if(dir == 'h'){
            if(add_ship_horizontal(player_info, start_x, start_y, length) == -1){

                int valid = add_ship_horizontal(player_info, start_x, start_y, length);
                if(valid == -1){
                    return -1;

                }
            }
        }

        if(dir == 'v'){
            if(add_ship_vertical(player_info, start_x, start_y, length) == -1){

                int valid = add_ship_vertical(player_info, start_x, start_y, length);
                if(valid == -1){
                    return -1;
                }
            }
        }
    }

    if(player == 1){
        game->status = PLAYER_0_TURN;
    }

    return 1;

}

int add_ship_horizontal(player_info *player, int x, int y, int length) {
    // implement this as part of Step 2
    // returns 1 if the ship can be added, -1 if not
    // hint: this can be defined recursively

    unsigned long long temp  = xy_to_bitval(x,y);

    if (length == 0){

        return 1;
    } else {
         if (temp & player->ships){
             return -1;
         } else{

             player->ships = player->ships | temp;
             return add_ship_horizontal(player, x + 1, y, length - 1);
         }
     }
}

int add_ship_vertical(player_info *player, int x, int y, int length) {
    // implement this as part of Step 2
    // returns 1 if the ship can be added, -1 if not
    // hint: this can be defined recursively

    unsigned long long temp  = xy_to_bitval(x,y);
    unsigned long long temp1 = player->ships;

    if (length == 0){

        return 1;

    } else {
        if (temp & temp1){
            return -1;
        } else{
            player->ships = player->ships | temp;
            return add_ship_vertical(player, x, y + 1, length - 1);
        }
    }
}