from itertools import cycle, izip

import numpy as np


def check_reduction(filename, reduction):
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
    def spec_adj(x):
        return set(np.where(m[:, x] > 0)[0])

    def char_adj(x):
        return set(np.where(m[x, :] > 0)[1])

    conn_spec = spec_adj([char])
    conn_char = set()

    funcs = cycle([char_adj, spec_adj])
    conns = cycle([conn_spec, conn_char])
    adjs = cycle([conn_char, conn_spec])

    for fun, conn, adj in izip(funcs, conns, adjs):
        i = len(adj)

        for vertex in conn:
            adj_set = fun(vertex)
            adj |= adj_set

        if len(adj) == i:
            break

    return np.fromiter((i for i in conn_spec), dtype=np.uint32)
