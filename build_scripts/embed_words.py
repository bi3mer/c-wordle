#!/usr/bin/env python3

import sys

def main():
    with open(sys.argv[1], 'r') as f:
        lines = f.readlines()

    with open(sys.argv[2], 'w') as f:
        f.write('#ifndef WORDS_H\n#define WORDS_H\n\n')
        f.write(f'static const int NUM_WORDS = {len(lines)};\n\n')
        f.write('static const char* WORDS[] = {\n')
        for line in lines:
            f.write(f'    "{line.strip()}",\n')
        f.write('};\n\n')
        f.write('#endif\n')

    return 0

if __name__ == "__main__":
    exit(main())