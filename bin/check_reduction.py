from itertools import cycle, izip

import numpy as np


def check_reduction(filename, reduction):
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
    pairs = ((int(sc[1:-1]) - 1, sc[-1]) for sc in reduction.split())

    # print("Matrix:\n{}\n".format(m))

    # realize each signed character in m
    for (char, state) in pairs:
        # find the species adjacent to char
        adj_spec, _ = np.where(m[:, char] > 0)

        # if state is +, add red edges
        if state == '+':
            # find the species connected to char
            con_spec = connected_species(m, char)

            # remove edges incident on char
            m[adj_spec, char] = 0

            # add red edges between char and the species connected but not
            # adjacent to char
            indexes = np.setdiff1d(con_spec, adj_spec, assume_unique=True)
            m[indexes, char] = 2

        elif state == '-':
            # remove edges incident on char
            m[adj_spec, char] = 0

        # print("Reduction: c{}{}\nMatrix:\n{}\n".format(char + 1, state, m))

    return not m.any()


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
