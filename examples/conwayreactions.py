#!/usr/bin/env python

#This code is very ugly, because it just needs to produce some chemilang code, until we get a preprocessor at least
def reactions():
    reactions = ""
    for i in range(1, 9):
        for j in range(i+1, 9):
            side = "n" + str(i) + " + n" + str(j)
            reactions += "\t\t" + side + " ->(2) " + side + " + c;\n"
    for i in range(1, 9):
        for j in range(i+1, 9):
            for k in range(j+1, 9):
                side = "n" + str(i) + " + n" + str(j) + " + n" + str(k)
                reactions += "\t\t" + side + " + c ->(3) " + side + ";\n"
    return reactions

print("module area {")
print("\tinput: [n1, n2, n3, n4, n5, n6, n7, n8];")
print("\toutput: c;")
print("\treactions: {")
print(reactions())
print("\t}")
print("}")

print("module main {")
s = 9
grid = "\toutput: ["
for i in range(s):
    for j in range(s):
        grid += "c" + str(i) + str(j) + ", "
grid = grid[:-2] + "];"
print(grid)

print("\tcompositions: {")
for i in range(1, s-1):
    for j in range(1, s-1):
        out = "c" + str(i) + str(j) + " = area(c" + str(i-1) + str(j-1) + ", c" + str(i) + str(j-1) + ", c" + str(i+1) + str(j-1) + ", c" + str(i-1) + str(j) + ", c" + str(i+1) + str(j) + ", c" + str(i-1) + str(j+1) + ", c" + str(i) + str(j+1) + ", c" + str(i+1) + str(j+1) + ");"
        print("\t\t" + out)
print("\t}")
print("}")
