The program runs via the command line via ./lab5 {mode} {inputArg0} {inputArg1}

Mode can be either Bloom or Bitvector.

In Bloom the input arguments are the file paths of text files, it stores them in a bloom filter and then finds what they have
in common with each other.

In Bitvector the arguments are strings and the program prints what characters they both have, the union of their characters
and the intersection of their characters.
