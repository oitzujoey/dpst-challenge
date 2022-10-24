# DOST Challenge

## Building

```bash
git clone https://github.com/oitzujoey/dpst-challenge
./build.bash
./a.out
```

## Commands

The grammar is

| Command | Description                                                                                              |
|:--------|:---------------------------------------------------------------------------------------------------------|
| ?h      | Print command reference.                                                                                 |
| ?s      | Search for a word using a case insensitive query. The first word after the command is used as the query. |
| ?S      | Search for a word using a case sensitive query. The first word after the command is used as the query.   |
| ?a      | Add new words. Any words after command are inserted into the dictionary.                                 |
| ?d      | Delete the dictionary.                                                                                   |
| ?f      | Print the number of free cons cells.                                                                     |
| ?q      | Quit.                                                                                                    |

If no command is given, `?a` is implicitly used.

## Grammar

```ebnf
command letter = "h" | "s" | "S" | "a" | "d" | "f" | "q" ;
command = "?", command letter ;
letter = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M"
       | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z"
	   | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m"
	   | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
	   | "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
	   | "'" | "-" | "_" ;
white space = ? any ASCII character that cannot be produced by the nonterminal "letter" ?
word = letter, { letter } ;
words = { { white space }, word, { white space } } ;
entered line = [ command ], words ;
```

## Usage

Run `a.out`. You will be greeted by the help menu and the command prompt.

```
$ ./a.out
?h Print this help.
?s Search for word. Case insensitive.
?S Search for word. Case sensitive.
?a Add new words. Words are also added when a command is not explicitly given.
?d Delete list.
?f Print free memory.
?q Quit.
> 
```

Type a sentence and then press Return. This will enter the words in the sentence into the dictionary.

```
> The quick brown fox jumped over the lazy dog.
(brow dog fox jump lazy over quic The the)
> 
```

Notice there are two "the"s.

Now, type the search command (case insensitive) and a word to search for after it.

```
> ?s jumped
jump
> ?s cat
> 
```

In the first case, the abbreviation of "jumped" was in the dictionary, so the abbreviation was returned. In the second 
case, "cat" was not in the dictionary, so nothing was returned.

Search for "the".

```
> ?s the
The
> 
```

It's in the dictionary, but it returned the capitalized version. To search for the lowercase version, do a case 
sensitive search.

```
> ?S the
the
> 
```

Print out the sentence again.

```
> ?p
(brow dog fox jump lazy over quic The the)
> 
```

It looks like "jump" is in the dictionary, though "jumped" was entered. Since those words have different lengths, the 
entry for "jumped" will not be returned if we search for "jump".

```
> ?sjump
> 
```

Add it to the dictionary.

```
> jump
(brow dog fox jump jump lazy over quic The the)
> 
```

There are now two entries for "jump", but they have different lengths.

```
> ?sjump
jump
> ?sjumped
jump
> ?sjumpe
> 
```

Delete the dictionary.

```
> ?d
> 
```

Some special characters are counted as letters.

```
> Ebers-Moll model
(Eber mode)
> can't
(can' Eber mode)
> cons_t
(can' cons Eber mode)
> 
```

And finally, quit the program either using ^C or `?q`.

```
> ?q
$
```
