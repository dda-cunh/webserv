#!/usr/bin/env python3
import os, sys, logging

logging.basicConfig(filename='error_script.log', level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

def main():
    logging.debug("Error Script Starting")
    raise RuntimeError("This is a test error")
    logging.debug("Error Script Ending")

if __name__ == "__main__":
    main()