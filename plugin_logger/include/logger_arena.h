#ifndef LOGGER_ARENA_H
#define LOGGER_ARENA_H

#define FLAG_ARENA_IS_INVALID   0x00000000
#define FLAG_ARENA_HAS_PLAYER1  0x00000001
#define FLAG_ARENA_HAS_PLAYER2  0x00000002
#define FLAG_ARENA_HAS_COMMENT1 0x00000004
#define FLAG_ARENA_HAS_COMMENT2 0x00000008
#define FLAG_ARENA_HAS_RESULT   0x00000010
#define FLAG_ARENA_HAS_ALL      0x0000001F

#define QUERY_ARENA_FLAG(arena_name,flag) (arena_name.flags & flag)
#define COMMENT_LENGTH 2048

typedef struct
{
    char players[2][MAX_BUF];
    int result;
           /*0: player0 won. 1: player1 won. -1: not valid */
    char comment[2][COMMENT_LENGTH];
           /*0: winner comment. 1: looser comment*/
    char timestamp[MAX_BUF];
    int counter;
    char screenshoot[MAX_BUF];
    int flags;
           /*screenshoots for player 0 and 1 at fatal moment*/
} arena_data;

#define EMPTY_ARENA_DATAS { {"Player1","Player2"},\
                          -1,                     \
                          {"",""},                \
                          "",-1,                  \
                          "",                \
                          FLAG_ARENA_IS_INVALID}
arena_data empty_arena = EMPTY_ARENA_DATAS;
arena_data last_arena  = EMPTY_ARENA_DATAS, current_arena = EMPTY_ARENA_DATAS;

#endif /* LOGGER_ARENA_H */

