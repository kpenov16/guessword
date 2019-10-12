#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_WORD_LEN 100
#define DELIMITER_CHAR '-'
char shared[MAX_WORD_LEN] = {'\0'};
void init_shared(char, unsigned long);
void init_shared(char c, unsigned long n){    
    for(int i=0; i<n; i++)
        shared[i] = c;
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

int main(int argc, char* args[]){
    char* program = args[0];
    puts(program);

    unsigned long word_len = strlen(args[1]);
    printf("%lu\n", word_len );
    char* word = args[1];
    puts(word);
    
    init_shared(DELIMITER_CHAR, word_len );
    puts(shared);

    unsigned long guess_len = strlen(args[2]);
    printf("%lu\n", guess_len );
    char* guess = args[2];
    puts(guess);    
    char guess_char = guess[0];

    while (contains_char(DELIMITER_CHAR)){ //more to guess
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
    
    }
    puts("end");
    puts(shared);
    
























}