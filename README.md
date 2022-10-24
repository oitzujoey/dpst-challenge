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

## What parts of this project did you find easy?

Parser: I've written parsers before using this same method, so it was a breeze to write.  
Evaluator: Simple verb-noun evaluators like I used are easy to make.  
Documentation: It's a small program, so it wasn't hard to write.  

## What parts of this project did you find challenging?

I didn't encounter anything truly challenging, though there were a few times where bugs and spec misinterpretation slowed me down.

I forgot that when the garbage collector is run, any conses that cannot be reached from the root will be marked as free. I attempted to add temporary roots before running GC, but I settled on the simpler solution of only running the GC once after each time the user's command has been fully evaluated. This approach requires a lot of memory, but I think the cost is worth it since I can easily add more.

I used a somewhat functional programming style when working with lists. Instead of moving links around in the list, I would just create a new one arranged how I needed it. Unfortunately, the specification required that I perform the sort by "reassigning pointer links", which meant that I had to mutate the list in the sorting function. Even though the functional model didn't work there, the mutable sort required much less memory than the original algorithm I used.

## Log

| Start Date | Start    | Finish   | Description |
|------------|---------:|---------:|:------------|
| 2022-10-21 |  6:45 PM |  7:00 PM | Contemplated the specification whilst dining upon a savory Double Western Bacon Cheeseburger®. |
| 2022-10-21 | 10:45 PM | 10:58 PM | Setup DPST, Git, build script, and create `main`. (Note: I used my own development environment and pasted into Replit since that's easier than developing in Replit's editor. Full source code is at "https://github.com/oitzujoey/dpst-challenge".) |
| 2022-10-21 | 11:01 PM | 11:11 PM | Started project. Started work on cons. |
| 2022-10-22 | 12:08 AM | 12:22 AM | Continued work on cons. Started on parser. |
| 2022-10-22 | 12:24 AM |  1:35 AM | Continued work on parser. Started sorter. |
| 2022-10-22 |  1:35 AM |  1:53 AM | Wrote a little documentation. |
| 2022-10-22 | 12:46 PM | 12:58 PM | Finished sort. |
| 2022-10-22 | 12:58 PM |  1:49 PM | Added garbage collection. |
| 2022-10-22 |  1:49 PM |  2:15 PM | Added search. |
| 2022-10-22 |  2:15 PM |  3:07 PM | Added stdin. |
| 2022-10-22 |  3:37 PM |  3:50 PM | Started on REPL. |
| 2022-10-22 |  4:00 PM |  6:30 PM | Finished REPL. Did a little testing. Program is finished for the most part. |
| 2022-10-22 |  7:05 PM |  7:08 PM | Touched up the code and comments. |
| 2022-10-22 |  3:49 PM |  5:15 PM | Created README. |
| 2022-10-22 |  5:15 PM |  6:22 PM | Completed this table and the sections above. |

