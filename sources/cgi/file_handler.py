#!/usr/bin/env python3
import os
import sys
import logging
import json
import urllib.parse

UPLOAD_DIR = "public/uploads/"
logging.basicConfig(filename='cgi_debug.log', level=logging.DEBUG)

def handle_file_upload():
    logging.debug("Starting file upload...")
    try:
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        logging.debug(f"Content Length: {content_length}")
        if content_length == 0:
            raise ValueError("No file uploaded")
        
        input_data = sys.stdin.buffer.read(content_length)
        
        content_type = os.environ.get('CONTENT_TYPE', '')
        logging.debug(f"Content-Type: {content_type}")
        
        if 'boundary=' not in content_type:
            raise ValueError("No boundary found in Content-Type")
        
        boundary = content_type.split('boundary=')[-1].encode()
        
        parts = input_data.split(b'--' + boundary)
        
        file_uploaded = False
        for i, part in enumerate(parts):
            if part.strip() in (b'', b'--'):
                continue
            
            try:
                headers, file_data = part.split(b'\r\n\r\n', 1)
            except ValueError:
                continue
            
            header_lines = headers.split(b'\r\n')
            filename = None
            for header in header_lines:
                if b'filename=' in header:
                    filename = header.split(b'filename=')[-1].strip(b'"')
                    logging.debug(f"Found filename: {filename}")
                    break
            
            if filename:
                filename = os.path.basename(filename.decode('utf-8'))
                file_path = os.path.join(UPLOAD_DIR, filename)
                with open(file_path, 'wb') as f:
                    f.write(file_data.rstrip(b'\r\n'))
                print("Content-Type: application/json")
                print()
                print(json.dumps({"success": True, "message": f"File '{filename}' uploaded successfully."}))
                logging.debug(f"File uploaded: {file_path}")
                file_uploaded = True
                return
            else:
                logging.debug(f"No filename found in part {i}")
        
        if not file_uploaded:
            raise ValueError("No valid file found in the upload data")

    except Exception as e:
        logging.exception("Error during file upload")
        print("Content-Type: application/json")
        print()
        print(json.dumps({"success": False, "error": str(e)}))
        sys.exit(1)

def list_files():
    logging.debug("Listing files...")
    try:
        files = os.listdir(UPLOAD_DIR)
        file_list = []
        for file in files:
            file_path = os.path.join(UPLOAD_DIR, file)
            file_info = {
                "name": file,
                "size": os.path.getsize(file_path),
                "modified": os.path.getmtime(file_path)
            }
            file_list.append(file_info)
        
        print("Content-Type: application/json")
        print()
        print(json.dumps(file_list))
        logging.debug(f"Files listed: {len(file_list)}")
    except Exception as e:
        logging.exception("Error listing files")
        print("Content-Type: application/json")
        print()
        print(json.dumps({"error": str(e)}))
        sys.exit(1)

def delete_file():
    logging.debug("Deleting file...")
    try:
        query_string = os.environ.get('QUERY_STRING', '')
        print(f"Query String: {query_string}")
        params = urllib.parse.parse_qs(query_string)
        filename = params.get('filename', [''])[0]

        if not filename:
            raise ValueError("No filename provided for deletion")

        file_path = os.path.join(UPLOAD_DIR, filename)
        
        if os.path.exists(file_path):
            os.remove(file_path)
            response = {"success": True, "message": f"File '{filename}' deleted successfully."}
        else:
            response = {"success": False, "message": f"File '{filename}' not found."}

        print("Content-Type: application/json")
        print()
        print(json.dumps(response))
        logging.debug(f"File deletion attempt: {filename}")
    except Exception as e:
        logging.exception("Error during file deletion")
        print("Content-Type: application/json")
        print()
        print(json.dumps({"success": False, "error": str(e)}))
        sys.exit(1)

def main():
    logging.debug("CGI Script Starting")
    request_method = os.environ.get("REQUEST_METHOD", "GET")
    logging.debug(f"Request Method: {request_method}")

    if request_method == "POST":
        handle_file_upload()
    elif request_method == "GET":
        list_files()
    elif request_method == "DELETE":
        delete_file()
    else:
        print("Status: 405 Method Not Allowed")
        print("Content-Type: text/html")
        print()
        print("Method not allowed.")
        logging.exception("Method not allowed.")
        sys.exit(2)

    logging.debug("CGI Script Ending")

if __name__ == "__main__":
    main()
