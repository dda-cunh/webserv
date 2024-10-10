#!/usr/bin/env python3
import os
import sys
import logging

UPLOAD_DIR = "public/uploads/"

logging.basicConfig(filename='cgi_debug.log', level=logging.DEBUG)

def handle_file_upload():
    print("Content-Type: text/html")
    print()

    logging.debug("Starting file upload...")
    
    try:
        # Get the content length from the environment
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        if content_length == 0:
            raise ValueError("No file uploaded")

        # Read the incoming data
        input_data = sys.stdin.read(content_length)
        content_type = os.environ.get('CONTENT_TYPE')
        boundary = b'--' + content_type.split('boundary=')[-1].encode()

        parts = input_data.encode('utf-8').split(boundary)  # Encode input_data to bytes

        for part in parts:
            if part == b'' or part == b'--\r\n':
                continue
            
            # Split header and file data
            try:
                headers, file_data = part.split(b'\r\n\r\n', 1)
            except ValueError:
                continue  # If there are no headers, skip

            header_lines = headers.split(b'\r\n')
            filename = None

            for header in header_lines:
                if b'filename=' in header:
                    # Extract filename
                    filename = header.split(b'filename=')[-1].strip(b'"')

            if filename is not None:
                filename = os.path.basename(filename.decode('utf-8'))
                file_path = os.path.join(UPLOAD_DIR, filename)

                with open(file_path, 'wb') as f:
                    f.write(file_data.rstrip(b'\r\n--\r\n'))  # Remove trailing CRLF and boundary
        
                print(f"File '{filename}' uploaded successfully.")
                logging.debug(f"File uploaded: {file_path}")
                return
        
        raise ValueError("Empty filename")

    except Exception as e:
        logging.exception("Error during file upload")
        print("Status: 500 Internal Server Error")
        print("Content-Type: text/html")
        print()
        print("An error occurred.")
        sys.exit(1)

def main():
    logging.debug("CGI Script Starting")
    request_method = os.environ.get("REQUEST_METHOD", "GET")
    logging.debug(f"Request Method: {request_method}")

    if request_method == "POST":
        handle_file_upload()
    else:
        print("Status: 405 Method Not Allowed")
        print("Content-Type: text/html\n")
        print("Method not allowed.")
        sys.exit(2)
        
    logging.debug("CGI Script Ending")

if __name__ == "__main__":
    main()