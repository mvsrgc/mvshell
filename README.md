# Mvshell

## Tokenizer
```sh
❯ gcc shell.c -g -o main && echo '<1>"Test|(mvshell)" |<()| "HELLO">>>>>' | ./main
Received from stdin: <1>"Test|(mvshell)" |<()| "HELLO">>>>>
Type: <          Position: 0     Literal: N/A                  Lexeme: <                   
Type: NUMBER     Position: 1     Literal: N/A                  Lexeme: 1                    Value: 1.000000
Type: >          Position: 2     Literal: N/A                  Lexeme: >                   
Type: STRING     Position: 3     Literal: Test|(mvshell)       Lexeme: "Test|(mvshell)"    
Type: STRING     Position: 3     Literal: Test|(mvshell)       Lexeme: "Test|(mvshell)"    
Type: PIPE       Position: 20    Literal: N/A                  Lexeme: |                   
Type: <          Position: 21    Literal: N/A                  Lexeme: <                   
Type: (          Position: 22    Literal: N/A                  Lexeme: (                   
Type: )          Position: 23    Literal: N/A                  Lexeme: )                   
Type: PIPE       Position: 24    Literal: N/A                  Lexeme: |                   
Type: PIPE       Position: 24    Literal: N/A                  Lexeme: |                   
Type: STRING     Position: 26    Literal: HELLO                Lexeme: "HELLO"             
Type: >          Position: 33    Literal: N/A                  Lexeme: >                   
Type: >          Position: 34    Literal: N/A                  Lexeme: >                   
Type: >          Position: 35    Literal: N/A                  Lexeme: >                   
Type: >          Position: 36    Literal: N/A                  Lexeme: >                   
Type: >          Position: 37    Literal: N/A                  Lexeme: >                   
Type: >          Position: 37    Literal: N/A                  Lexeme: >                   
Nb. recognized tokens: 15 (<1>"Test|(mvshell)"|<()|"HELLO">>>>>)
```

```sh
❯ ./main
Testing MVSHELL "It's nice!" (2>1)
Received from stdin: Testing MVSHELL "It's nice!" (2>1)
Type: WORD       Position: 0     Literal: Testing              Lexeme: Testing             
Type: WORD       Position: 8     Literal: MVSHELL              Lexeme: MVSHELL             
Type: STRING     Position: 16    Literal: It's nice!           Lexeme: "It's nice!"        
Type: (          Position: 29    Literal: N/A                  Lexeme: (                   
Type: NUMBER     Position: 30    Literal: N/A                  Lexeme: 2                    Value: 2.000000
Type: >          Position: 31    Literal: N/A                  Lexeme: >                   
Type: NUMBER     Position: 32    Literal: N/A                  Lexeme: 1                    Value: 1.000000
Type: )          Position: 33    Literal: N/A                  Lexeme: )                   
Nb. recognized tokens: 8
```

