#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Ghost
{
    char name[10];
    int moving_direction , remaining_time_of_defensive_mode , initial_place[2] , current_place[2] , next_place[2];
    bool offensive_mode;
};

struct Pacman
{
    int moving_direction , initial_place[2] , current_place[2] , next_place[2] , number_of_lives ;
};

void Place_Of_Pacman( struct Pacman *pacman , char game_board[100][100] , int , int );
int Score_Increase( struct Ghost ghost[] , struct Pacman *pacman , char game_board[100][100] );
bool Is_Pacman_Win( char game_board[100][100] , int , int );

int main()
{
    struct Ghost ghost[4];
    struct Pacman pacman;
    int minute , second , score , number_of_rows , number_of_columns , counter1 = 0 , counter2 = 0 , counter3 = 0;
    char game_board[100][100] , ch;
    scanf("%d %d\n",&number_of_rows,&number_of_columns);
    while( counter1 < number_of_rows )
    {
        counter2 = 0;
        while( counter2 < number_of_columns )
        {
            scanf("%c",&ch);
            if( ch != '\n' )
            {
                game_board[counter1][counter2] = ch;
                counter2++;
            }
        }
        counter1++;
        if( counter1 == number_of_rows && ch == '\n' )
            break;
    }
    scanf("%02d:%02d %d\n",&minute,&second,&score);
    scanf("pacman: %d %d (%d,%d) (%d,%d)\n",&pacman.moving_direction,&pacman.number_of_lives,&pacman.initial_place[0],&pacman.initial_place[1],&pacman.current_place[0],&pacman.current_place[1]);
    for( counter1 = 0 ; counter1 < 4 ; counter1++ )
    {
        scanf("%s %d %d ",&ghost[counter1].name,&ghost[counter1].moving_direction,&ghost[counter1].offensive_mode);
        if( !ghost[counter1].offensive_mode )
            scanf("%d ",&ghost[counter1].remaining_time_of_defensive_mode);
        scanf("(%d,%d) (%d,%d)",&ghost[counter1].initial_place[0],&ghost[counter1].initial_place[1],&ghost[counter1].current_place[0],&ghost[counter1].current_place[1]);
    }

    Place_Of_Pacman( &pacman , game_board , number_of_rows , number_of_columns );
    score += Score_Increase( ghost , &pacman , game_board );
    printf("(%d,%d)\n%d\n",pacman.next_place[0],pacman.next_place[1],score);
    if( Is_Pacman_Win( game_board , number_of_rows , number_of_columns ) )
        printf("Yes");
    else
        printf("No");
    minute += ( ++second ) / 60;
    second %= 60;
    return 0;
}

void Place_Of_Pacman( struct Pacman *pacman , char game_board[100][100] , int number_of_rows , int number_of_columns )
{
    switch( (*pacman).moving_direction )
    {
        case 1:(*pacman).next_place[0] = ( number_of_rows + (*pacman).current_place[0] - 1 ) % number_of_rows;
               (*pacman).next_place[1] = (*pacman).current_place[1];
               break;
        case 2:(*pacman).next_place[0] = (*pacman).current_place[0];
               (*pacman).next_place[1] = ( (*pacman).current_place[1] + 1 ) % number_of_columns;
               break;
        case 3:(*pacman).next_place[0] = ( (*pacman).current_place[0] + 1 ) % number_of_rows;
               (*pacman).next_place[1] = (*pacman).current_place[1];
               break;
        case 4:(*pacman).next_place[0] = (*pacman).current_place[0];
               (*pacman).next_place[1] = ( number_of_columns + (*pacman).current_place[1] - 1 ) % number_of_columns;
               break;
    }
    if( game_board[(*pacman).next_place[0]][(*pacman).next_place[1]] == '#' )
    {
        (*pacman).next_place[0] = (*pacman).current_place[0];
        (*pacman).next_place[1] = (*pacman).current_place[1];
    }
}

int Score_Increase( struct Ghost ghost[4] , struct Pacman *pacman , char game_board[100][100] )
{
    int score_increase = 0 , counter , temp = (*pacman).number_of_lives ;
    switch( game_board[(*pacman).next_place[0]][(*pacman).next_place[1]] )
    {
        case '*':score_increase++;
                 game_board[(*pacman).next_place[0]][(*pacman).next_place[1]] = '_';
                 break;
        case '^':score_increase += 20;
                 game_board[(*pacman).next_place[0]][(*pacman).next_place[1]] = '_';
                 break;
        case 'O':for( counter = 0 ; counter < 4 ; counter++ )
                 {
                     ghost[counter].offensive_mode = false;
                     ghost[counter].remaining_time_of_defensive_mode = 10;
                 }
                 game_board[(*pacman).next_place[0]][(*pacman).next_place[1]] = '_';
                 break;
    }
    for( counter = 0 ; counter < 4 ; counter++ )
    {
        if( ghost[counter].current_place[0] == (*pacman).next_place[0] && ghost[counter].current_place[1] == (*pacman).next_place[1] )
        switch( ghost[counter].offensive_mode )
        {
            case true:(*pacman).number_of_lives--;break;
            case false:score_increase += 50;
                       ghost[counter].next_place[0] = ghost[counter].initial_place[0];
                       ghost[counter].next_place[1] = ghost[counter].initial_place[1];
                       ghost[counter].offensive_mode = true;
                       break;
        }
    }
    if( temp != (*pacman).number_of_lives )
    {
        (*pacman).next_place[0] = (*pacman).initial_place[0];
        (*pacman).next_place[1] = (*pacman).initial_place[1];
    }
    return score_increase;
}
bool Is_Pacman_Win( char game_board[100][100] , int number_of_rows , int number_of_columns )
{
    int counter1 , counter2;
    for( counter1 = 0 ; counter1 < number_of_rows ; counter1++ )
        for( counter2 = 0 ; counter2 < number_of_columns ; counter2++ )
            if( game_board[counter1][counter2] == '*' || game_board[counter1][counter2] == 'O' )
                return false;
    return true;
}
