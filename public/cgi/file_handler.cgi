#!/usr/bin/env python3
import os
import sys
import cgi
from http import HTTPStatus

UPLOAD_DIR = "public/uploads/"

def handle_file_upload():
    print("Content-Type: text/html\n")
    print("Starting file upload...")  # Debugging line
    
    form = cgi.FieldStorage()
    print("Form parsed.")  # Debugging line
    
    if "file" in form:
        file_item = form["file"]
        print(f"File Item: {file_item}")  # Print the file item for debugging
        
        if file_item.filename:
            file_path = os.path.join(UPLOAD_DIR, os.path.basename(file_item.filename))
            with open(file_path, 'wb') as f:
                f.write(file_item.file.read())
            print("Status: 200 OK")
            print(f"File '{file_item.filename}' uploaded successfully!")
        else:
            print("Status: 400 Bad Request")
            print("No file was uploaded.")
    else:
        print("Status: 400 Bad Request")
        print("Form doesn't contain 'file' field.")

def handle_file_delete():
    file_name = os.getenv("PATH_INFO", "").lstrip("/")
    file_path = os.path.join(UPLOAD_DIR, file_name)

    if os.path.exists(file_path):
        os.remove(file_path)
        print("Status: 200 OK")
        print(f"File '{file_name}' deleted successfully!")
    else:
        print("Status: 404 Not Found")
        print(f"File '{file_name}' not found.")

def main():
    request_method = os.getenv("REQUEST_METHOD", "GET")

    # Print environment variables for debugging
    print("Environment Variables:")
    for key, value in os.environ.items():
        print(f"{key}: {value}")

    if request_method == "POST":
        handle_file_upload()
    elif request_method == "DELETE":
        handle_file_delete()
    else:
        print("Status: 405 Method Not Allowed")
        print("Content-Type: text/html\n")
        print("Method not allowed.")

if __name__ == "__main__":
    main()
