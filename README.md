# Mvshell

### Lexes various characters
```sh
❯ ./main
<1>"Test|(mvshell)" |<()| "HELLO">>>>>
Type: <          Position: 0     Literal: N/A                  Lexeme: <                   
Type: NUMBER     Position: 1     Literal: 1                    Lexeme: 1                    Value: 1.000000
Type: >          Position: 2     Literal: N/A                  Lexeme: >                   
Type: STRING     Position: 3     Literal: Test|(mvshell)       Lexeme: "Test|(mvshell)"    
Type: PIPE       Position: 20    Literal: N/A                  Lexeme: |                   
Type: <          Position: 21    Literal: N/A                  Lexeme: <                   
Type: (          Position: 22    Literal: N/A                  Lexeme: (                   
Type: )          Position: 23    Literal: N/A                  Lexeme: )                   
Type: PIPE       Position: 24    Literal: N/A                  Lexeme: |                   
Type: STRING     Position: 26    Literal: HELLO                Lexeme: "HELLO"             
Type: >          Position: 33    Literal: N/A                  Lexeme: >                   
Type: >          Position: 34    Literal: N/A                  Lexeme: >                   
Type: >          Position: 35    Literal: N/A                  Lexeme: >                   
Type: >          Position: 36    Literal: N/A                  Lexeme: >                   
Type: >          Position: 37    Literal: N/A                  Lexeme: >                   
[15 token(s)]
```

### Wildcard support
```sh
❯ ./main
ls -l t*
Type: WORD       Position: 0     Literal: ls                   Lexeme: ls                  
Type: WORD       Position: 3     Literal: -l                   Lexeme: -l                  
Type: WORD       Position: 6     Literal: tokenize.c           Lexeme: t*                  
Type: WORD       Position: 17    Literal: tokenize.h           Lexeme: t*                  
[4 token(s)]
-rw-r--r-- 1 user user 681 Aug 14 22:22 tokenize.c
-rw-r--r-- 1 user user 657 Aug 14 23:33 tokenize.h
```

### Parses commands using recursive descent
```sh
-l
Lexeme: WORD       Position: 0     Literal: -l        
[1 token(s)]
Error: Argument list must be preceded by a command.
```

```sh
-l
Lexeme: WORD       Position: 0     Literal: -l        
[1 token(s)]
Error: Argument list must be preceded by a command.
```

```sh
ls -l
Lexeme: WORD       Position: 0     Literal: ls        
Lexeme: WORD       Position: 3     Literal: -l        
[2 token(s)]
```

### Recognizes types

```sh
echo 12.2
Lexeme: WORD       Position: 0     Literal: echo
Lexeme: NUMBER     Position: 5     Literal: 12.2       Value: 12.200000
[2 token(s)]
12.2
```

```sh
echo "12.0"
Lexeme: WORD       Position: 0     Literal: echo
Lexeme: STRING     Position: 5     Literal: 12.0
[2 token(s)]
12.0
```
