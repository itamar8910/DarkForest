#!/usr/bin/python3

import subprocess
import argparse

parser = argparse.ArgumentParser(description='symbol file generator')
parser.add_argument('img')
parser.add_argument('--sym')
parser.add_argument('--line')
args = parser.parse_args()

def generate_symbols(img, sym_file):
    subprocess.check_output('nm {} | c++filt > {}'.format(img, sym_file), shell=True)
    lines = []
    # sort by address
    with open(sym_file, 'r') as f:
        lines = f.readlines()
        lines = ['{} {}'.format(l.split()[0], ' '.join(l.split()[2:])) for l in sorted(lines, key=lambda l : int(l.split()[0], 16)) if l.split()[1] in ['T', 'W']]
    with open(sym_file, 'w') as f:
        f.write('\n'.join(lines))

def generate_lines(img, line_file):
    subprocess.check_output('readelf -wL {} > {}'.format(img, line_file), shell=True)
    lines = []
    # cleanup format
    with open(line_file, 'r') as f:
        lines = f.readlines()
        lines = ['{} {} {}'.format(l.split()[2][2:], l.split()[0], l.split()[1]) for l in lines if l.split() and '0x' in l.split()[-1]]
    with open(line_file, 'w') as f:
        f.write('\n'.join(lines))


generate_symbols(args.img, args.sym)
generate_lines(args.img, args.line)