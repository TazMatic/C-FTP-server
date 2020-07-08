#!/usr/bin/env python3
import sys
import socket
import argparse


def parseCMD():
    parser = argparse.ArgumentParser()
    parser.add_argument('hostname', help='Destination hostname or IP')
    parser.add_argument('port', help='Destination port')
    parser.add_argument('files', help='Input files to be sent', nargs='+')
    parser.add_argument('-t', '--tcp', action='store_const',
                        dest='tcp', help='Use TCP protocol: Default',
                        required=False, default=True, const=True)
    parser.add_argument('-u', '--udp', action='store_const',
                        dest='udp', help='Use UDP protocol',
                        required=False, default=False, const=True)

    return parser.parse_args()


def main():
    # Process command line arguaments
    args = parseCMD()
    print(args)

if __name__ == '__main__':
    try:
        main()
    except (KeyboardInterrupt, Exception):
        print("\nProgram exiting...")
