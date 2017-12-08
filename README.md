# Persistent Phylogeny

An implementation for the persistent phylogeny polynomial-time algorithm described the article [Solving the Persistent Phylogeny Problem in polynomial time](https://arxiv.org/abs/1611.01017).

## Prerequisites

- gcc 5.0 and above
- [Boost libraries](http://www.boost.org/more/getting_started/index.html)

## Compiling

### Get Boost

> N.B. Skip this block if you already have Boost on your system

#### From repository

Popular Linux and Unix distributions such as Fedora, Debian, and NetBSD include pre-built Boost packages.

Debian example:

```
# apt install libboost-all-dev
```

#### From source

Download Boost.

- Unix: follow the steps at [boost.org](http://www.boost.org/doc/libs/1_65_1/more/getting_started/unix-variants.html)
- Windows: follow the steps at [boost.org](http://www.boost.org/doc/libs/1_65_1/more/getting_started/windows.html)

##### Boost.ProgramOptions

> N.B. Skip this step if you already built Boost on your system

This implementation uses `Boost.ProgramOptions` to parse command line arguments and options, which means it needs separately-compiled library binaries to work.

- Unix: follow the steps at [boost.org](http://www.boost.org/doc/libs/1_65_1/more/getting_started/unix-variants.html#prepare-to-use-a-boost-library-binary)
- Windows: follow the steps at [boost.org](http://www.boost.org/doc/libs/1_65_1/more/getting_started/windows.html#prepare-to-use-a-boost-library-binary)

> NB. Skip this step if you already updated your platform's environment variable

Moreover, on Unix systems, you may need to prepare plaform-specific settings (since `Boost.ProgramOptions` is a shared library).

- Follow the steps at [boost.org](http://www.boost.org/doc/libs/1_65_1/more/getting_started/unix-variants.html#test-your-program)

### Clone the repository

```
$ git clone https://github.com/dcasella/persistent-phylogeny.git
$ cd persistent-phylogeny
```

### Compile and link

```
$ make
```

#### Run tests (optional)

Make sure everything works correctly.

```
$ ./run_tests.sh
```

## Running

```
$ ./bin/ppp file...
```

The program will call the `reduce` function on each graph read from the files.

Examples:

Listing the single files

```
$ ./bin/ppp file1 dir2/file2
```

Or with globs

```
$ ./bin/ppp dir1/*
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
