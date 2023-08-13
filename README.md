# Mvshell

## Tokenizer
```sh
❯ gcc shell.c -o main && echo '<1>"Test|(mvshell)" |<()| ' | ./main
Received from stdin: <1>"Test|(mvshell)" |<()| 
Lexeme (at 0): <
Parsed (at 1): 1
Lexeme (at 2): >
Lexeme (at 3): "Test|(mvshell)"
Literal (at 3): Test|(mvshell)
Lexeme (at 20): |
Lexeme (at 21): <
Lexeme (at 22): (
Lexeme (at 23): )
Lexeme (at 24): |
Nb. recognized tokens: 8 (<>"Test|(mvshell)"|<()|)
```
