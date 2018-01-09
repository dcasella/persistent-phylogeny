#!/usr/bin/env python2.7

from itertools import cycle, izip

import numpy as np
import argparse


def check_reduction(filename, reduction, verbose=False):
    # apply reduction to the matrix in filename

    # open filename in read mode
    with open(filename, 'r') as file:
        # store lines in a list
        lines = [line.rstrip() for line in file if line.rstrip()]

        # stop execution if lines is empty
        if not lines:
            return False

        # store number of species and characters from line 0
        shape = tuple(map(int, lines[0].split()))

        # stop execution if shape is empty
        if not shape:
            return False

        # store matrix
        m = np.matrix('; '.join(lines[1:]), dtype=np.uint8)

        # stop execution if m is empty
        if m is None:
            return False

        # assert the matrix is correctly sized
        assert m.shape == shape

    # build the list of signed character pairs from reduction
    pairs = ((int(sc[1:-1]), sc[-1]) for sc in reduction.split())

    if verbose:
        print("Matrix:\n{}\n".format(m))

    realize(m, pairs, verbose)

    return not m.any()


def realize(m, pairs, verbose=False):
    # realize each signed character in m
    for (char, state) in pairs:
        if verbose:
            print("Reduction: c{}{}".format(char, state))

        # find the species adjacent to char
        adj_spec, _ = np.where(m[:, char] > 0)
        # find the species connected to char
        con_spec = connected_species(m, char)
        # find the species connected but not adjacent to char
        dif_spec = np.setdiff1d(con_spec, adj_spec, assume_unique=True)

        # if state is +, add red edges
        if state == '+':
            # if the reduction is not feasible
            if active(m, char):
                return False

            # remove edges incident on char
            m[adj_spec, char] = 0
            # add red edges between char and the species in dif_spec
            m[dif_spec, char] = 2

        elif state == '-':
            # if the reduction is not feasible
            if not active(m, char) or dif_spec.size:
                return False

            # remove edges incident on char
            m[adj_spec, char] = 0

        if verbose:
            print("Matrix:\n{}\n".format(m))


def connected_species(m, char):
    # perform transitive closure for char in m

    def spec_adj(x):
        # return the species adjacent to x
        return set(np.where(m[:, x] > 0)[0])

    def char_adj(x):
        # return the characters adjacent to x
        return set(np.where(m[x, :] > 0)[1])

    # find the species adjacent to char
    con_spec = spec_adj([char])

    # initialize the set of characters connected to char
    con_char = set()

    # create cyclic iterators for functions, connected vertices and adjacent
    # vertices
    funcs = cycle([char_adj, spec_adj])
    conns = cycle([con_spec, con_char])
    adjs = cycle([con_char, con_spec])

    # iterate through the aggregation of previously created iterators
    for fun, conn, adj in izip(funcs, conns, adjs):
        # store the size of the vertices adjacent to char
        adj_size = len(adj)

        # iterate through the vertices connected to char
        for vertex in conn:
            # find the vertices adjacent to vertex
            adj_set = fun(vertex)
            # perform union between adj and adj_set
            adj |= adj_set

        # stop iterating if adj didn't grow in size
        if len(adj) == adj_size:
            break

    # create and return a one-dimensional array of con_spec
    return np.fromiter((i for i in con_spec), dtype=np.uint32)


def active(m, char):
    # check if char is active in m

    # if char has red edges
    if 2 in m[:, char]:
        return True

    return False


def main():
    # parse command line arguments and call check_reduction

    # initialize parser
    parser = argparse.ArgumentParser(
        add_help=False,
        description='Apply REDUCTION to the matrix in FILE')

    # change optional arguments title
    parser._optionals.title = 'Options'
    # pop optional arguments group
    parser_optional = parser._action_groups.pop()
    # add required arguments group
    parser_required = parser.add_argument_group('Required')

    # add required arguments
    # input file
    parser_required.add_argument('file', type=str, metavar='FILE',
                                 help='Input matrix file.')
    # reduction for file matrix
    parser_required.add_argument('reduction', type=str, metavar='REDUCTION',
                                 help='C-Reduction to apply.')

    # add optional arguments
    # (change) help message
    parser_optional.add_argument('-h', '--help', action='help',
                                 default=argparse.SUPPRESS,
                                 help='Display this message.')
    # verbosity
    parser_optional.add_argument('-v', '--verbose', action='store_true',
                                 help='Display the matrix as the reduction is '
                                      'applied.')

    # append the previously popped optional arguments group
    parser._action_groups.append(parser_optional)

    # parse command line arguments
    args = parser.parse_args()

    # run the program
    output = check_reduction(args.file, args.reduction, args.verbose)

    if args.verbose:
        print("Successful: {}".format(output))


if __name__ == '__main__':
    main()
