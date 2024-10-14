#!/usr/bin/env python3
import os, sys, logging, json

logging.basicConfig(filename='file_handler.log', level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

UPLOAD_DIR = os.environ.get('UPLOAD_DIR')
if not UPLOAD_DIR:
    raise EnvironmentError("UPLOAD_DIR environment variable is not set")

def json_response(success, message=None, error=None):
    response_data = json.dumps({"success": success, "message": message, "error": error})
    print("Content-Type: application/json")
    print(f"Content-Length: {len(response_data)}\n")
    print(response_data)

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
            try:
                headers, file_data = part.split(b'\r\n\r\n', 1)
            except ValueError:
                continue
            
            for header in headers.split(b'\r\n'):
                if b'filename=' in header:
                    filename = os.path.basename(header.split(b'filename=')[-1].strip(b'"').decode('utf-8'))
                    with open(os.path.join(UPLOAD_DIR, filename), 'wb') as f:
                        f.write(file_data.rstrip(b'\r\n--'))
                    json_response(True, "File '{}' uploaded successfully.".format(filename))
                    return
        
        raise ValueError("No valid file found in the upload data")
    except Exception as e:
        logging.exception("Error during file upload")
        json_response(False, error=str(e))
        sys.exit(1)

def list_files():
    try:
        files = []
        for f in os.listdir(UPLOAD_DIR):
            file_info = {
                "name": f,
                "size": os.path.getsize(os.path.join(UPLOAD_DIR, f)),
                "modified": os.path.getmtime(os.path.join(UPLOAD_DIR, f))
            }
            files.append(file_info)
        json_response(True, message=files)
    except Exception as e:
        logging.exception("Error listing files")
        json_response(False, error=str(e))
        sys.exit(1)

def delete_file():
    try:
        filename = os.path.basename(os.environ.get('PATH_INFO', ''))
        if not filename:
            raise ValueError("No file specified for deletion in the path")
        
        file_path = os.path.join(UPLOAD_DIR, filename)
        if os.path.exists(file_path):
            os.remove(file_path)
            json_response(True, f"File '{filename}' deleted successfully.")
        else:
            json_response(False, f"File '{filename}' not found.")
    except Exception as e:
        logging.exception("Error during file deletion")
        json_response(False, error=str(e))
        sys.exit(1)

def print_env_variables():
    for key, value in os.environ.items():
        logging.debug(f"{key}={value}")

def main():
    logging.debug("CGI Script Starting")
    print_env_variables()
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
    logging.debug("=======================================================")

if __name__ == "__main__":
    main()