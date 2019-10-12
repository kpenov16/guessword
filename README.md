# guessword
basic example is used from the excelent article
"Basic use of pthreads An introduction to POSIX threads" 
by Peter Seebach January 21, 2004, IBM developerWorks




s133967
Word guess game:

max players: 5
min players: 1
game waits for players to connect in 1 min

Guess a predefined word, for example: "School" by given first and last letter, for example "S----l". 
Every player has a 60 sec time to guess a letter, if a player guessed a letter the letter is added to the word, for example
Player 1 guessed the letter 'h' so the letter is added to the word "S-h--l", next turn all the players can see the word with all guessed letters. 
If two players guess the same letter the fastest player wins a point. A player can guess the hole world and this player wins the game, 
if more than one players guesses correcly the hole world then the fastest wins the game. The game ends when a player guess the hole word or 
when all the letters of the word were guessed. 

procedure:
compile in terminal with:  gcc guessword.c -o guessword -lpthread

server runs in the terminal with: ./guessword <word_to_guess> 
for example: ./guessword Denmark

the player connects to the game in the terminal with: telnet localhost 6173

