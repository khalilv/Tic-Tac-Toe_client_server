#include <stdio.h>
#include <string.h>	
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <unistd.h>	
#include <stdlib.h>
#include <time.h>

struct USER{
	char username[100];
	char password[100];
	struct USER *next;
};

enum GAME_STATE{ 
	CREATOR_WON=-2,
	IN_PROGRESS_CREATOR_NEXT=-1,
	DRAW=0,
	IN_PROGRESS_CHALLENGER_NEXT=1,
	CHALLENGER_WON=2
};

struct GAME{
	char gamename[100];
	struct USER *creator;
	struct USER *challenger;
	enum GAME_STATE state;
	char ttt[3][3];
	struct GAME *next;
};

struct USER *user_list_head = NULL;
struct GAME *game_list_head = NULL;


int existUserPass(char *username, char *password){
	if(user_list_head == NULL){
		return 1; 
	}else{
		struct USER *ptr;
		ptr = (struct USER *)malloc(sizeof(struct USER)); 
		ptr = user_list_head; 

		while(ptr != NULL){

			if(strcmp(ptr->username, username)==0 && strcmp(ptr->password, password)==0){
					return 0;  
				}

			if(strcmp(ptr->username, username)==0 && strcmp(ptr->password, password)!=0){
					return 1;   
				}

			ptr = ptr->next;
		}
	}
	return 1; 			
}
int existGame(char *gamename){
	if(game_list_head == NULL){
		return 1; 
	}else{
		struct GAME *ptr;
		ptr = (struct GAME *)malloc(sizeof(struct GAME)); 
		ptr = game_list_head; 

		while(ptr != NULL){

			if(strcmp(ptr->gamename, gamename)==0){
					return 0;  
			}

			ptr = ptr->next;
		}
	}
	return 1; 			
}


int addMove(char ttt[3][3], char *move , char symbol){

	if(strcmp(move, "a1")==0 && ttt[0][0] != 'x' && ttt[0][0] != 'o'){
		ttt[0][0]= symbol; 
		return 0; 
	}else if (strcmp(move, "a2")==0 && ttt[0][1] != 'x' && ttt[0][1] != 'o'){
		ttt[0][1]= symbol;
		return 0; 
	}else if (strcmp(move, "a3")==0 && ttt[0][2] != 'x' && ttt[0][2] != 'o'){
		ttt[0][2]= symbol;
		return 0; 
	}else if (strcmp(move, "b1")==0 && ttt[1][0] != 'x' && ttt[1][0] != 'o'){
		ttt[1][0]= symbol; 
		return 0; 
	}else if (strcmp(move, "b2")==0 && ttt[1][1] != 'x' && ttt[1][1] != 'o'){
		ttt[1][1]= symbol;
		return 0;  
	}else if (strcmp(move, "b3")==0 && ttt[1][2] != 'x' && ttt[1][2] != 'o'){
		ttt[1][2]= symbol; 
		return 0; 
	}else if (strcmp(move, "c1")==0 && ttt[2][0] != 'x' && ttt[2][0] != 'o'){
		ttt[2][0] = symbol; 
		return 0; 
	}else if (strcmp(move, "c2")==0 && ttt[2][1] != 'x' && ttt[2][1] != 'o'){
		ttt[2][1] = symbol; 
		return 0; 
	}else if (strcmp(move, "c3")==0 && ttt[2][2] != 'x' && ttt[2][2] != 'o'){
		ttt[2][2] = symbol; 
		return 0; 
	}
	return 1; 

}

int checkBoardState(char ttt[3][3]){
	
	//loop up to down for x's
	int i,j;
	int count=0;
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			if(ttt[j][i]=='x'){
				count++;
			}
			if(count ==3){
				return 2; 
			}
		}
		count = 0; 
	}

	//loop left to right for x's
	count =0;
	for(i=0; i<3; i++){
		for (j=0; j<3; j++){
			if(ttt[i][j]=='x'){
				count++; 
			}
			if(count==3){
				return 2; 
			}
		}
		count =0; 
	}

	//check diagonals for x's 
	if(ttt[0][0]=='x' && ttt[1][1]=='x' && ttt[2][2]=='x'){
		return 2; 
	}

	if(ttt[0][2]=='x' && ttt[1][1]=='x' && ttt[2][0]=='x'){
		return 2; 
	}
	
	//loop up to down for o's
	count=0;
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			if(ttt[j][i]=='o'){
				count++;
			}
			if(count ==3){
				return -2; 
			}
		}
		count = 0; 
	}

	//loop left to right for o's
	count =0;
	for(i=0; i<3; i++){
		for (j=0; j<3; j++){
			if(ttt[i][j]=='x'){
				count++; 
			}
			if(count==3){
				return -2; 
			}
		}
		count =0; 
	}

	//check diagonals for o's 
	if(ttt[0][0]=='o' && ttt[1][1]=='o' && ttt[2][2]=='o'){
		return -2; 
	}
	
	if(ttt[0][2]=='o' && ttt[1][1]=='o' && ttt[2][0]=='o'){
		return -2; 
	}

	//now check if board is full, if it is return 0, if it isnt return 5 
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			if(ttt[i][j] != 'x' && ttt[i][j] != 'o'){
				return 5; 
			}
		}
	}
	return 0; 
}




int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , read_size;
	struct sockaddr_in server , client;
	char client_message[2000];

	unsigned short int port = 8888;

	if( argc > 1 )
		port = (unsigned short int)atoi(argv[1]);
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( port );
	
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("bind failed. Error");
		return 1;
	}

	listen(socket_desc , 3);

	printf( "Game server ready on port %d.\n", port );

	while( 1 ){
		c = sizeof(struct sockaddr_in);

		//accept connection from an incoming client
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0)
		{
			perror("accept failed");
			return 1;
		}

		char temp[200];
		memset(client_message, '\0', 200);
		int bytes_read = 0;
		while( bytes_read < 200 ){
			read_size = recv(client_sock , temp , 200, 0);
			if(read_size <= 0){
				puts("Client disconnected");
				fflush(stdout);
				close(client_sock);
				close(socket_desc);
				return 0;
			}
			memcpy( client_message+bytes_read, temp, read_size );
			bytes_read += read_size;
		}

	  	char response[2000];
	  	response[0] = '\0';
	  	char* command = strtok( client_message, "," );
	  	char *username = strtok( NULL, "," );
	  	char *password = strtok( NULL, ",");

		if( command == NULL || username == NULL || password == NULL ){
			sprintf( response, "MUST ENTER A VALID COMMAND WITH ARGUMENTS FROM THE LIST:\n" );
			sprintf( response+strlen(response), "LOGIN,USER,PASS\n" );
			sprintf( response+strlen(response), "CREATE,USER,PASS,GAMENAME\n" );
			sprintf( response+strlen(response), "JOIN,USER,PASS,GAMENAME,SQUARE\n" );
			sprintf( response+strlen(response), "MOVE,USER,PASS,GAMENAME,SQUARE\n" );
			sprintf( response+strlen(response), "LIST,USER,PASS\n" );
			sprintf( response+strlen(response), "SHOW,USER,PASS,GAMENAME\n" );
			write(client_sock , response , 2000);  
		  	close(client_sock);
			continue;
		}

		if( strcmp( command, "LOGIN" ) == 0 ){
			//struct USER *ptr;
			//ptr = (struct USER *)malloc(sizeof(struct USER)); 
			if(user_list_head == NULL){
				struct USER *firstuser;
				firstuser = (struct USER *)malloc(sizeof(struct USER)); 
				strcpy((*firstuser).username, username);
				strcpy((*firstuser).password, password); 
				(*firstuser).next = NULL;  
				user_list_head = firstuser; 
				//*ptr = firstuser;
				sprintf(response, "NEW USER CREATED SUCCESFULLY\n"); 
			}else{
				struct USER *ptr;
				ptr = (struct USER *)malloc(sizeof(struct USER)); 
				ptr = user_list_head; 

				while(1){
					
					if(strcmp(ptr->username, username)==0 && strcmp(ptr->password, password)==0){
						sprintf(response, "USER ALREADY CREATED\n");
						break;  
					}

					if(strcmp(ptr->username, username)==0 && strcmp(ptr->password, password)!=0){
						sprintf(response, "INCORRECT PASSWORD\n");
						break;  
					}

					if(ptr->next != NULL){
						ptr = ptr->next; 
						continue; 
					}else{
						struct USER *newuser;
						newuser = (struct USER *)malloc(sizeof(struct USER)); 
						strcpy((*newuser).username, username);
						strcpy((*newuser).password, password); 
						(*newuser).next = NULL;
						ptr->next = newuser; 
						sprintf(response, "NEW USER CREATED SUCCESFULLY\n");
						break; 
					}
				}	
	  		}
		}
	  	else if( strcmp( command, "CREATE" ) == 0 ){ 
	  		
			int check = existUserPass(username, password);
			if(check == 1){
				sprintf( response, "USER NOT CREATED, PLEASE LOGIN FIRST\n");
			}else{
				char *game_name = strtok( NULL, ",");

			if( game_name == NULL ){
				sprintf( response, "CREATE COMMAND MUST BE CALLED AS FOLLOWS:\n" );
				sprintf( response+strlen(response), "CREATE,USER,PASS,GAMENAME\n" );
				write(client_sock , response , 2000);  
		  		close(client_sock);
		  		continue;
			}

			struct GAME *game = (struct GAME*)malloc( sizeof(struct GAME) );
			strcpy( game->gamename, game_name );
			for( int row=0; row<3; row++ )
				for( int col=0; col<3; col++ )
					game->ttt[row][col] = ' ';
			
			//FIND USER	
			struct USER *userptr;
			userptr = (struct USER *)malloc(sizeof(struct USER)); 
			userptr = user_list_head;
			while(userptr != NULL){
				if(strcmp(userptr->username, username)==0 && strcmp(userptr->password, password)==0){
					break; 
				}
				userptr= userptr->next;
			}

			if (game_list_head == NULL){
					game->creator = userptr; 
					game->next = NULL;
					game_list_head = game;
					sprintf( response, "GAME CREATED SUCCESSFULLY\n");
				}else{
					struct GAME *ptr;
					ptr = (struct GAME *)malloc(sizeof(struct GAME)); 
					ptr = game_list_head; 

					if(strcmp(game_list_head->gamename, game_name)==0){
						sprintf( response, "GAME ALREADY CREATED\n");
					}else{

						while(ptr->next != NULL){
	
							if(strcmp(ptr->gamename, game_name)==0){
								sprintf( response, "GAME ALREADY CREATED\n"); 
								break; 
							}
							ptr=ptr->next; 

						}

						if(strcmp(ptr->gamename, game_name)==0){
							sprintf( response, "GAME ALREADY CREATED\n");
						}else{
							game->creator = userptr; 
							game->next = NULL;
							ptr->next = game; 
							sprintf( response, "GAME CREATED SUCCESSFULLY\n");
						}
					}
				}
			
			sprintf( response, "EXAMPLE GAME CREATED. WAITING FOR OPPONENT.\r\n");
			sprintf( response, "%sa  %c | %c | %c \r\n",response,  game->ttt[0][0],  game->ttt[0][1],  game->ttt[0][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sb  %c | %c | %c \r\n", response, game->ttt[1][0],  game->ttt[1][1],  game->ttt[1][2]);
			sprintf( response, "%s  ---|---|---\r\n", response );
			sprintf( response, "%sc  %c | %c | %c \r\n", response, game->ttt[2][0],  game->ttt[2][1],  game->ttt[2][2]);
			sprintf( response, "%s\r\n", response );
			sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );				
			}	
		}
		else if( strcmp( command, "JOIN" ) == 0 ){ 
			char *gamename = strtok(NULL,","); 
			char *move = strtok(NULL,","); 

			if(gamename == NULL || move==NULL){
				sprintf( response, "INVALID INPUT. INCLUDE GAMENAME and MOVE\n");
			}else{
				int checkGame = existGame(gamename); 
				int checkUser = existUserPass(username, password); 
				if(checkGame == 1 || checkUser == 1){
					sprintf( response, "ERROR GAME/USER NOT CREATED\n");
				}else{
					struct USER *userptr;
					userptr = (struct USER *)malloc(sizeof(struct USER)); 
					userptr = user_list_head;
					while(userptr != NULL){
						if(strcmp(userptr->username, username)==0 && strcmp(userptr->password, password)==0){
							break; 
						}
						userptr= userptr->next;
					}

					struct GAME *gameptr; 
					gameptr= (struct GAME *)malloc(sizeof(struct GAME));
					gameptr= game_list_head; 
					while(gameptr != NULL){
						if(strcmp(gameptr->gamename, gamename)==0){
							break;
						}
						gameptr=gameptr->next; 
					}

					if(gameptr->challenger != NULL){
						sprintf(response, "ERROR. GAME ALREADY FULL\n"); 
					}else if(strcmp(username, gameptr->creator->username)==0 && strcmp(gameptr->creator->password, password)==0){
						sprintf(response, "ERROR. CREATOR ATTEMPTING TO PLAY AGAINST CREATOR"); 
					}else{
						gameptr->challenger = userptr; 
						if(addMove(gameptr->ttt, move, 'x') != 0){
							sprintf(response, "USER ADDED SUCCESFULLY BUT ERROR ADDING MOVE\n"); 	
						}else{
							sprintf(response, "USER ADDED SUCCESFULLY AND MOVE PLAYED\n"); 
							gameptr->state=-1; 
							sprintf( response, "%sa  %c | %c | %c \r\n",response,  gameptr->ttt[0][0],  gameptr->ttt[0][1],  gameptr->ttt[0][2]);
							sprintf( response, "%s  ---|---|---\r\n", response );
							sprintf( response, "%sb  %c | %c | %c \r\n", response, gameptr->ttt[1][0],  gameptr->ttt[1][1],  gameptr->ttt[1][2]);
							sprintf( response, "%s  ---|---|---\r\n", response );
							sprintf( response, "%sc  %c | %c | %c \r\n", response, gameptr->ttt[2][0],  gameptr->ttt[2][1],  gameptr->ttt[2][2]);
							sprintf( response, "%s\r\n", response );
							sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );
						}
					}
				}
			}
		}
		else if( strcmp( command, "MOVE" ) == 0 ){ 
			char *gamename = strtok(NULL,","); 
			char *move = strtok(NULL,","); 

			if(gamename == NULL || move==NULL){
				sprintf( response, "INVALID INPUT\n");
			}else{
				int checkGame = existGame(gamename); 
				int checkUser = existUserPass(username, password); 
				if(checkGame == 1 || checkUser == 1){
					sprintf( response, "ERROR GAME/USER NOT CREATED\n");
				}else{
					//FIND USER
					struct USER *userptr;
					userptr = (struct USER *)malloc(sizeof(struct USER)); 
					userptr = user_list_head;
					while(userptr != NULL){
						if(strcmp(userptr->username, username)==0 && strcmp(userptr->password, password)==0){
							break; 
						}
						userptr= userptr->next;
					}
					//FIND GAME
					struct GAME *gameptr; 
					gameptr= (struct GAME *)malloc(sizeof(struct GAME));
					gameptr= game_list_head; 
					while(gameptr != NULL){
						if(strcmp(gameptr->gamename, gamename)==0){
							break;
						}
						gameptr=gameptr->next; 
					}

					if(gameptr->state != 2 || gameptr->state != -2 || gameptr->state != 0){
						if(strcmp(gameptr->creator->username, username) != 0 && strcmp(gameptr->challenger->username, username) != 0 ){
							sprintf( response, "ERROR USER/CHALLENGER NOT IN REQUESTED GAME\n");
						}else{
							if(strcmp(gameptr->creator->username, userptr->username) == 0 && strcmp(gameptr->creator->password, userptr->password) == 0 && gameptr->state != -1){
								sprintf( response, "ERROR NOT THE CREATORS TURN\n");
							}else{
								if( strcmp(gameptr->challenger->username, userptr->username) == 0 && strcmp(gameptr->challenger->password, userptr->password)==0 && gameptr->state != 1){
									sprintf( response, "ERROR NOT THE CHALLENGERS TURN\n");
								}else{
									if(strcmp(gameptr->challenger->username, userptr->username) == 0 && strcmp(gameptr->challenger->password, userptr->password)==0 && gameptr->state == 1){
										//proper input for move
										if(addMove(gameptr->ttt, move, 'x') == 0){
											int boardstate = checkBoardState(gameptr->ttt);
											gameptr->state = boardstate;  
											if(boardstate == 5){
												gameptr->state=-1; 
												sprintf(response, "MOVE MADE SUCCESSFULLY. OPPONENTS TURN\n");
											}
											if(boardstate == 0){
												sprintf(response, "MOVE MADE SUCCESSFULLY GAME ENDED IN A DRAW\n");
											}
											if(boardstate == 2){
												sprintf(response, "MOVE MADE SUCCESSFULLY CHALLENGER WON\n");
											}
											if(boardstate == -2){
												sprintf(response, "MOVE MADE SUCCESSFULLY CREATOR WON\n");
											}
											sprintf( response, "%sa  %c | %c | %c \r\n",response,  gameptr->ttt[0][0],  gameptr->ttt[0][1],  gameptr->ttt[0][2]);
											sprintf( response, "%s  ---|---|---\r\n", response );
											sprintf( response, "%sb  %c | %c | %c \r\n", response, gameptr->ttt[1][0],  gameptr->ttt[1][1],  gameptr->ttt[1][2]);
											sprintf( response, "%s  ---|---|---\r\n", response );
											sprintf( response, "%sc  %c | %c | %c \r\n", response, gameptr->ttt[2][0],  gameptr->ttt[2][1],  gameptr->ttt[2][2]);
											sprintf( response, "%s\r\n", response );
											sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );
										}else{
											sprintf( response, "ERROR ADDING MOVE/INVALID MOVE OR SPOT ALREADY TAKEN\n");
										}
									}else{
										if(strcmp(gameptr->creator->username, userptr->username) == 0 && strcmp(gameptr->creator->password, userptr->password) == 0 && gameptr->state == -1){
											if(addMove(gameptr->ttt, move, 'o') == 0){
												int boardstate = checkBoardState(gameptr->ttt); 
												gameptr->state = boardstate;  
												if(boardstate == 5){
													gameptr->state=1; 
													sprintf(response, "MOVE MADE SUCCESSFULLY. OPPONENTS TURN\n");
												}
												if(boardstate == 0){
													sprintf(response, "MOVE MADE SUCCESSFULLY GAME ENDED IN A DRAW\n");
												}
												if(boardstate == 2){
													sprintf(response, "MOVE MADE SUCCESSFULLY CHALLENGER WON\n");
												}
												if(boardstate == -2){
													sprintf(response, "MOVE MADE SUCCESSFULLY CREATOR WON\n");
												}
												sprintf( response, "%sa  %c | %c | %c \r\n",response,  gameptr->ttt[0][0],  gameptr->ttt[0][1],  gameptr->ttt[0][2]);
												sprintf( response, "%s  ---|---|---\r\n", response );
												sprintf( response, "%sb  %c | %c | %c \r\n", response, gameptr->ttt[1][0],  gameptr->ttt[1][1],  gameptr->ttt[1][2]);
												sprintf( response, "%s  ---|---|---\r\n", response );
												sprintf( response, "%sc  %c | %c | %c \r\n", response, gameptr->ttt[2][0],  gameptr->ttt[2][1],  gameptr->ttt[2][2]);
												sprintf( response, "%s\r\n", response );
												sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );
											}else{
												sprintf( response, "ERROR ADDING MOVE/INVALID MOVE OR SPOT ALREADY TAKEN\n");
											}
										}
									}
								}
							}
						}
					}else{
						sprintf(response, "GAME ALREADY FINISHED"); 
					}
				}
			}
		}
		else if( strcmp( command, "LIST" ) == 0 ){ 
			if(existUserPass(username,password)!= 0){
				sprintf(response, "INCORRECT USER/PASSWORD\n");
			}else{
				struct GAME *gameptr; 
				gameptr = (struct GAME *)malloc(sizeof(struct GAME)); 
				gameptr = game_list_head; 
				if(gameptr == NULL){
					sprintf(response, "NO GAMES IN PROGRESS\n"); 
				}
				while(gameptr != NULL){
					char *stateresponse; 
					if(gameptr->state == 0){
						stateresponse = "ENDED AS A DRAW"; 
					}
					if(gameptr->state == 2){
						stateresponse = "ENDED. CHALLENGER WON"; 
					}
					if(gameptr->state == -2){
						stateresponse = "ENDED. CREATOR WON"; 
					}
					if(gameptr->state == -1){
						stateresponse = " IN PROGRESS. CREATER NEXT TO PLAY AS o"; 
					}
					if(gameptr->state == 1){
						stateresponse = "IN PROGRESS. CHALLENGER NEXT TO PLAY AS x"; 
					}
					sprintf(response, "GAME: %s. CREATED BY: %s. CHALLENGED BY: %s. STATE: %s\n", gameptr->gamename, gameptr->creator->username, gameptr->challenger->username, stateresponse); 
					gameptr = gameptr->next; 
				}
			}
		}
		else if( strcmp( command, "SHOW" ) == 0 ){
			char *gamename = strtok(NULL,","); 
			if(gamename == NULL || existGame(gamename) != 0 || existUserPass(username, password) != 0){
				sprintf(response, "ERROR: GAME NAME OR USER DOESNT EXIST: PLEASE LOGIN AND/OR CREATE GAME FIRST\n"); 
			}else{
				//find requested game 
				struct GAME *gameptr; 
				gameptr = (struct GAME *)malloc(sizeof(struct GAME)); 
				gameptr = game_list_head; 
				while(gameptr != NULL){
					if(strcmp(gameptr->gamename, gamename)==0){
						break; 
					}
					gameptr= gameptr->next; 
				}
				char *stateresponse; 
				if(gameptr->state == 0){
					stateresponse = "ENDED AS A DRAW"; 
				}
				if(gameptr->state == 2){
					stateresponse = "ENDED. CHALLENGER WON"; 
				}
				if(gameptr->state == -2){
					stateresponse = "ENDED. CREATOR WON"; 
				}
				if(gameptr->state == -1){
					stateresponse = " IN PROGRESS. CREATER NEXT TO PLAY AS o"; 
				}
				if(gameptr->state == 1){
					stateresponse = "IN PROGRESS. CHALLENGER NEXT TO PLAY AS x"; 
				}
				sprintf(response, "GAME: %s, BETWEEN CREATOR: %s AND CHALLENGER: %s. STATE: %s\n", gameptr->gamename, gameptr->creator->username, gameptr->challenger->username, stateresponse); 
				sprintf( response, "%sa  %c | %c | %c \r\n",response,  gameptr->ttt[0][0],  gameptr->ttt[0][1],  gameptr->ttt[0][2]);
				sprintf( response, "%s  ---|---|---\r\n", response );
				sprintf( response, "%sb  %c | %c | %c \r\n", response, gameptr->ttt[1][0],  gameptr->ttt[1][1],  gameptr->ttt[1][2]);
				sprintf( response, "%s  ---|---|---\r\n", response );
				sprintf( response, "%sc  %c | %c | %c \r\n", response, gameptr->ttt[2][0],  gameptr->ttt[2][1],  gameptr->ttt[2][2]);
				sprintf( response, "%s\r\n", response );
				sprintf( response, "%s   %c   %c   %c\r\n", response, '1', '2', '3' );

			}
		}
		else{
	  		sprintf( response, "COMMAND %s NOT IMPLEMENTED", command );
		}

		write(client_sock , response , 2000);  
		close(client_sock);
	}

	close(socket_desc);	
	
	return 0;
}









