#!/usr/bin/env python3
import os
import sys
import logging
import json
import urllib.parse

UPLOAD_DIR = "public/uploads/"
logging.basicConfig(filename='cgi_debug.log', level=logging.DEBUG)

def respond_json(data):
    print("Content-Type: application/json\n")
    print(json.dumps(data))

def handle_file_upload():
    try:
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        if content_length == 0:
            raise ValueError("No file uploaded")
        
        input_data = sys.stdin.buffer.read(content_length)
        content_type = os.environ.get('CONTENT_TYPE', '')
        if 'boundary=' not in content_type:
            raise ValueError("No boundary found in Content-Type")
        
        boundary = content_type.split('boundary=')[-1].encode()
        parts = input_data.split(b'--' + boundary)
        
        for part in parts:
            if part.strip() in (b'', b'--'):
                continue
            headers, file_data = part.split(b'\r\n\r\n', 1)
            filename = next((h.split(b'filename=')[-1].strip(b'"') for h in headers.split(b'\r\n') if b'filename=' in h), None)
            if filename:
                file_path = os.path.join(UPLOAD_DIR, os.path.basename(filename.decode('utf-8')))
                with open(file_path, 'wb') as f:
                    f.write(file_data.rstrip(b'\r\n'))
                respond_json({"success": True, "message": f"File '{filename.decode()}' uploaded successfully."})
                return
        raise ValueError("No valid file found in the upload data")
    except Exception as e:
        logging.exception("Error during file upload")
        respond_json({"success": False, "error": str(e)})
        sys.exit(1)

def list_files():
    try:
        files = [{"name": f, "size": os.path.getsize(os.path.join(UPLOAD_DIR, f)), "modified": os.path.getmtime(os.path.join(UPLOAD_DIR, f))} for f in os.listdir(UPLOAD_DIR)]
        respond_json(files)
    except Exception as e:
        logging.exception("Error listing files")
        respond_json({"error": str(e)})
        sys.exit(1)

def delete_file():
    try:
        filename = urllib.parse.parse_qs(os.environ.get('QUERY_STRING', '')).get('filename', [''])[0]
        if not filename:
            raise ValueError("No filename provided for deletion")
        
        file_path = os.path.join(UPLOAD_DIR, filename)
        if os.path.exists(file_path):
            os.remove(file_path)
            respond_json({"success": True, "message": f"File '{filename}' deleted successfully."})
        else:
            respond_json({"success": False, "message": f"File '{filename}' not found."})
    except Exception as e:
        logging.exception("Error during file deletion")
        respond_json({"success": False, "error": str(e)})
        sys.exit(1)

def main():
    logging.debug("CGI Script Starting")
    request_method = os.environ.get("REQUEST_METHOD", "GET")
    if request_method == "POST":
        handle_file_upload()
    elif request_method == "GET":
        list_files()
    elif request_method == "DELETE":
        delete_file()
    else:
        print("Status: 405 Method Not Allowed\nContent-Type: text/html\n\nMethod not allowed.")
        sys.exit(2)
    logging.debug("CGI Script Ending")

if __name__ == "__main__":
    main()