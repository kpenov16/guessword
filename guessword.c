#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>
#include <getopt.h>
#include <netdb.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define GAME_PORT 6173
#define MAX_WORD_LEN 100
#define DELIMITER_CHAR '-'
char shared[MAX_WORD_LEN] = {'\0'};
char word[MAX_WORD_LEN] = {'\0'};

int warn(char *fmt, ...) {
	int r;
	va_list ap;
	va_start(ap, fmt);
	r = vfprintf(stderr, fmt, ap);
	va_end(ap);
	return r;
}

int fail(char *fmt, ...) {
	int r;
	va_list ap;
	va_start(ap, fmt);
	r = vfprintf(stderr, fmt, ap);
	exit(1);
	/* notreached */
	va_end(ap);
	return r;
}



void init_shared(char, unsigned long);
void init_shared(char c, unsigned long n){    
    for(int i=0; i<n; i++)
        shared[i] = c;
}

bool shared_contains_char(char, char*);
bool shared_contains_char(char guess_char, char* shared_org){
    unsigned long len = strlen(shared_org);
    for (int i = 0; i < len; i++){
        if(tolower(shared_org[i]) == tolower(guess_char))
            return 1;    
    }
    return 0;
}

bool contains_char(char);
bool contains_char(char guess_char){
    unsigned long len = strlen(shared);
    for (int i = 0; i < len; i++){
        if(tolower(shared[i]) == tolower(guess_char))
            return 1;    
    }
    return 0;
}

struct sockets {
	int local;
	FILE *in, *out;
    char* ptr_shared;
    char* ptr_word;
};

int socket_setup(void) {
	struct protoent *tcp_proto;
	struct sockaddr_in local;
	int r, s, one;

	tcp_proto = getprotobyname("tcp");
	if (!tcp_proto) {
		fail("Can't find TCP/IP protocol: %s\n", strerror(errno));
	}
	s = socket(PF_INET, SOCK_STREAM, tcp_proto->p_proto);
	if (s == -1) {
		fail("socket: %s\n", strerror(errno));
	}
	one = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	memset(&local, 0, sizeof(struct sockaddr_in));
	local.sin_family = AF_INET;
	local.sin_port = htons(GAME_PORT);
	r = bind(s, (struct sockaddr *) &local, sizeof(struct sockaddr_in));
	if (r == -1) {
		fail("bind: %s\n", strerror(errno));
	}
	r = listen(s, 5);
	if (r == -1) {
		fail("listen: %s\n", strerror(errno));
	}
	return s;
}

struct sockets* get_new_player_socket(int);
struct sockets* get_new_player_socket(int sock) {
	int conn;
	if ((conn = accept(sock, NULL, NULL)) < 0) {
		warn("accept: %s\n", strerror(errno));
		return 0;
	} else {
		struct sockets *s;
		s = malloc(sizeof(struct sockets));
		if (s == NULL) {
			warn("malloc failed.\n");
			return 0;
		}
		s->local = 0;
		s->in = fdopen(conn, "r");
		s->out = fdopen(conn, "w");
		setlinebuf(s->in);
		setlinebuf(s->out);
        s->ptr_shared = shared;
        s->ptr_word = word;
		return s;
	}
}
void* play(void*);
void* play(void* v){
    struct sockets *s = v;
	if (!s || !s->out || !s->in)
		return NULL;

    for (size_t i = 0; i < 5; i++){
        char guess_char;
        char inbuf[MAX_WORD_LEN]; 
     	fprintf(s->out, "enter character to guess\n");
        fflush(s->out);
        fgets(inbuf, sizeof(inbuf), s->in);
        //fgets(inbuf, MAX_WORD_LEN, stdin); 
        guess_char = inbuf[0];
        if(!shared_contains_char(guess_char, s->ptr_shared)){ //already guessed
            char* p = s->ptr_word;
            unsigned long word_len = strlen(s->ptr_word);
            for(int i=0; i<word_len; i++, p++){
                if(tolower(*p) == tolower(guess_char) && s->ptr_shared[i] == '-' && tolower(guess_char) != '-'){ //there is going to be anasty bug if user types - 
                s->ptr_shared[i] = *p;
                    //char is guessed
                }
            }
            //puts(s->ptr_shared);
            fprintf(s->out, "%s ", s->ptr_shared);
		    fflush(s->out);				
        }else {
            //puts("already guessed");
            fprintf(s->out, "%s ", "already guessed");
            fflush(s->out);
        }
    }
    fprintf(s->out, "buh-bye!\n");
    if (s->local == 0) {
        shutdown(fileno(s->out), SHUT_RDWR);
    }
    fclose(s->out);
    fclose(s->in);
    if (s->local == 0) {
        free(s);
    }
    return 0;
}

int main(int argc, char* args[]){
    char* program = args[0];
    puts(program);

    unsigned long word_len = strlen(args[1]);
    printf("%lu\n", word_len );
    //char* word = args[1];
    memcpy(word, args[1], strlen(args[1])+1); //now it is global
    puts(word);
    
    init_shared(DELIMITER_CHAR, word_len );
    puts(shared);

    int sock = socket_setup();
	int players_count = 0;
    while (players_count <= 2){
   		struct sockets *s = get_new_player_socket(sock);
		if (s) {
            pthread_t p;
            pthread_create(&p, NULL, play, (void *) s);
            //exit(0);
            players_count++;
		}
    }
    
    //wait 5 min and end the game
    //https://www.geeksforgeeks.org/time-delay-c/
    // Converting time into milli_seconds
    int number_of_seconds = 60*5; 
    int milli_seconds = 1000 * number_of_seconds;   
    // Stroing start time 
    clock_t start_time = clock();   
    // looping till required time is not acheived 
    while (clock() < start_time + milli_seconds) 
        ; 

    //print end
    puts("end");
    puts(shared);

    exit(0);            

    //unsigned long guess_len = strlen(args[2]);
    //printf("%lu\n", guess_len );
    //char* guess = args[2];
    //puts(guess);    
    //char guess_char = guess[0];
/*  
    char guess_char;
    //while (contains_char(DELIMITER_CHAR)){ //more to guess
    
        //wait 60 sec and do another turn

        //possible player code    
        char buf[MAX_WORD_LEN]; 
        fgets(buf, MAX_WORD_LEN, stdin); 
        guess_char = buf[0];
        if(!contains_char(guess_char)){ //already guessed
            char* p = word;
            for(int i=0; i<word_len; i++, p++){
                if(tolower(*p) == tolower(guess_char) && shared[i] == '-' && tolower(guess_char) != '-'){ //there is going to be anasty bug if user types - 
                    shared[i] = *p;
                    //char is guessed
                }
            }
            puts(shared);
        }else {puts("already guessed");}

    //}
    
    //puts("end");
    //puts(shared);
    
*/


}