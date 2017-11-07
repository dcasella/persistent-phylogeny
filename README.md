# Persistent Phylogeny

An implementation for the persistent phylogeny polynomial-time algorithm described the paper [Solving the Persistent Phylogeny Problem in polynomial time](https://arxiv.org/abs/1611.01017).

## Prerequisites

- gcc 6.1 and above (or 5.0 and above, specifying -std=c++14)
- [Boost libraries](http://www.boost.org/more/getting_started/index.html)

## Compiling

Clone the repository

```
git clone https://github.com/dcasella/persistent-phylogeny.git
cd persistent-phylogeny
```

Compile and link

```
make
```

## Running

```
./bin/ppp file...
```

The program will call the `reduce` function on each graph read from the files.

Examples:

Listing the single files

```
./bin/ppp tests/no_5x2.txt tests/no_6x3.txt
```

Or with globs

```
./bin/ppp tests/no_*[^d].txt
```

## Input file structure

The first line must contain the size of the matrix.  
Empty lines (after the first) are ignored.  
Subsequent lines represent the matrix itself.

```
N M

Matrix made up of space-separated boolean values
```

Example:

```
6 3

0 0 1
0 1 0
0 1 1
1 0 0
1 0 1
1 1 0
```
