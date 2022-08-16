#!/usr/bin/env python3

import sys
import re
import fileinput


if __name__ == "__main__":
    with open(sys.argv[3], 'r') as file:
        for line in file:
            line = re.sub(sys.argv[1], sys.argv[2], line)
            print(line, end='')
