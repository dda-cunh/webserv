#!/usr/bin/env python3
import os
import sys
import cgi
import cgitb

cgitb.enable()

UPLOAD_DIR = "public/uploads/"

def debug_print(*args, **kwargs):
    print(*args, **kwargs, file=sys.stderr)

import logging

logging.basicConfig(filename='cgi_debug.log', level=logging.DEBUG)

def handle_file_upload():
    print("Content-Type: text/html")
    print()
    logging.debug("Starting file upload...")
    
    try:
        form = cgi.FieldStorage()
        logging.debug("Form parsed successfully")
        
        if "file" not in form:
            raise ValueError("No file uploaded")
        
        fileitem = form["file"]
        if not fileitem.filename:
            raise ValueError("Empty filename")
        
        fn = os.path.basename(fileitem.filename)
        file_path = os.path.join(UPLOAD_DIR, fn)
        
        with open(file_path, 'wb') as f:
            chunk_size = 8192
            while True:
                chunk = fileitem.file.read(chunk_size)
                if not chunk:
                    break
                f.write(chunk)
        
        print(f"File '{fn}' uploaded successfully to {file_path}")
        logging.debug(f"File uploaded: {file_path}")
    
    except Exception as e:
        logging.exception("Error during file upload")
        print("Status: 500 Internal Server Error")
        print("Content-Type: text/html")
        print()
        print(f"An error occurred: {str(e)}")


def main():
    debug_print("CGI Script Starting")
    request_method = os.environ.get("REQUEST_METHOD", "GET")
    debug_print(f"Request Method: {request_method}")

    if request_method == "POST":
        handle_file_upload()
    else:
        print("Status: 405 Method Not Allowed")
        print("Content-Type: text/html\n")
        print("Method not allowed.")

    debug_print("CGI Script Ending")

if __name__ == "__main__":
    main()