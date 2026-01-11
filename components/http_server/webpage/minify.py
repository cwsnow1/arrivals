#!/usr/bin/env python

import gzip
from pathlib import Path

def main():
    script_dir = Path(__file__).resolve().parent
    with open(script_dir.joinpath('index.html')) as f:
        contents = f.read()
    contents = contents.replace('    ', '')
    contents = contents.replace('\n', '')
    open(script_dir.joinpath('index.html.gz'), 'wb').write(gzip.compress(contents.encode()))


if __name__ == '__main__':
    main()