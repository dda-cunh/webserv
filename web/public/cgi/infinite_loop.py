#!/usr/bin/env python3
import os, sys, logging

logging.basicConfig(filename='infinite_loop_script.log', level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

def main():
    logging.debug("Infinite Loop Script Starting")
    while True:
        pass
    logging.debug("Infinite Loop Script Ending")

if __name__ == "__main__":
    main()