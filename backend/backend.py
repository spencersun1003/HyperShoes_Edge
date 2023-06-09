from flask import Flask, session, redirect, url_for, jsonify
from flask import request
from flask_session import Session
from os import environ
import requests
import json
from flask_cors import CORS
from flask import Flask, render_template

from request_handler.esp_requests import *
# from request_handler.client_requests import *
from request_handler.login_requests import *
# from tools.global_variables import *
import numpy as np
import localDatabase


app = Flask(__name__)
app.secret_key = environ.get("SECRET_KEY")
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)
CORS(app)

ESP32 = "/esp32"
USER = "/user"

email = "brian@uw.edu" # temporary
left_IP = "0.0.0.0"
right_IP = "0.0.0.0"
is_upload = True
record_id = None
record_id_prev = None
left_calibration=[0,0.05,0.05,0.05,0]
right_calibration=[0,0.04,0.04,0.04,0]

mylocalDatabase=localDatabase.shoesDatabase()


@app.route("/signup", methods=['POST', 'GET'])
def signup():
    # if "email" in session: # later change to redis check userid
    # if get_email() is not None:
    global email
    if email is not None:
        return redirect(url_for("logged_in"))
    method = request.method
    fullname = request.form.get("fullname")
    email = request.form.get("email")
    password1 = request.form.get("password1")
    password2 = request.form.get("password2")
    return signup_handler(method, fullname, email, password1, password2)

@app.route('/logged_in')
def logged_in():
    # if "email" in session:
    # if get_email() is not None:
    global email
    if email is not None:
        # email = session["email"]
        return "logged in " + email
    else:
        return redirect(url_for("login"))

@app.route("/login", methods=["POST", "GET"])
def login():
    # if "email" in session:
    # if get_email() is not None:
    global email
    if email is not None:
        return redirect(url_for("logged_in"))
    method = request.method
    email = request.form.get("email")
    password = request.form.get("password")
    (message, status) = login_handler(method, email, password)
    if status:
        # session["email"] = message
        email = message
        # print(session["email"])
        return redirect(url_for('logged_in'))
    else:
        # print(session["email"])
        return message

@app.route("/logout", methods=["POST", "GET"])
def logout():
    # if "email" in session:
    #     session.pop("email", None)
    # if get_email() is not None:
    global email
    if email is not None:
        email = None
        return "signed out"
    else:
        return "main page"

# user initiate or stop recording
# accessed by user
@app.route(USER + "/record", methods=["POST"])
def check_upload():
    global left_IP, right_IP, is_upload
    print(left_IP)
    print(right_IP)
    status = request.args.get('status', 'false')
    frequency = request.args.get('frequency', '100')
    # return check_upload_handler(status, frequency) # global variable not working....
    left_inactive = left_IP == "0.0.0.0"
    right_inactive = right_IP == "0.0.0.0"
    message = ''
    print(left_inactive)
    print(right_inactive)
    if left_inactive:
        message = "left is inactive"
    else:
        url_left = "http://" + left_IP
        if status == 'true':
            is_upload = True
            url_left = url_left + "/start?frequency=" + frequency    
        else:
            is_upload = False
            url_left = url_left + "/stop"
        message = requests.request("GET", url_left, headers={}, data={}).text + "\n"
    
    if right_inactive:
        message = "right is inactive"
    else:
        url_right = "http://" + right_IP
        if status == 'true':
            is_upload = True
            url_right = url_right + "/start?frequency=" + frequency    
        else:
            is_upload = False
            url_right = url_right + "/stop"

        message = message + requests.request("GET", url_right, headers={}, data={}).text
    return message

# get record list
@app.route(USER + "/data/record/all", methods=["GET"])
def get_record_list():
    pass

# get data by record id
@app.route(USER + "/data/record", methods=["GET"])
def get_record_data():
    global record_id
    if record_id == None:
        return jsonify(""), 201, {'Content-Type': 'application/json'}
    data = get_record_entry(record_id)
    data['_id'] = str(data['_id'])
    return jsonify(data), 200, {'Content-Type': 'application/json'}

@app.route(USER + "/data/record/result", methods=["GET"])
def get_record_data_result():
    global record_id_prev
    result = get_record_result(record_id_prev)
    return jsonify(result), 200, {'Content-Type': 'application/json'}

# register device when booting
# accessed by esp32
@app.route(ESP32 + "/register", methods=["POST"])
def register_device():
    global left_IP, right_IP
    IP = request.remote_addr
    device_id = request.form['deviceID']
    if int(device_id) % 2 == 0 :
        left_IP = IP
    else:
        right_IP = IP
    # if "email" in session: # later change to redis check userid
    # if get_email() is not None:
    global email
    if email is not None:
        return register_device_handler(IP, email, device_id)
    else:
        return "Not Loggin yet"

# record the start and stop data
# accessed by esp32
@app.route(ESP32 + "/record", methods=["POST"])
def record_status():
    global is_upload, record_id, record_id_prev
    data = request.form
    # return record_status_handler(data)
    if is_upload:
        device_id = data['deviceID']
        is_recording = bool(data['record'])
        if is_recording:
            if record_id is not None:
                return "already started"
            is_upload = True
            record_id = 11#start_new_record(email, device_id)
            return record_id
        else:
            is_upload = False
            record_id_prev = record_id
            record_id = None
            print(" Stop Recording: ", device_id, False)
            return "Stop Recording"
    else:
        return "Not started, is_upload is False"

# after calling esp32/record to start, then using this to upload data
# accessed by esp32
@app.route(ESP32 + "/upload", methods=["POST"])
def upload_data():
    #print("Got uploaded")
    global is_upload, record_id
    data = request.form
    # return upload_data_handler(data)
    if is_upload is False:
        return "Not uploaded"
    else:

        if data["deviceID"]=="10000000":
            thisdata=data2list(data)
            thisdata=calibration(thisdata,left_calibration)
            print("Add left, id:", data["deviceID"],"data:",thisdata)
            mylocalDatabase.add("left",thisdata)

        else:
            thisdata = data2list(data)
            thisdata = calibration(thisdata,left_calibration)
            print("Add right, id:", data["deviceID"],thisdata)
            mylocalDatabase.add("right",thisdata)
        #insert_new_entry(data, record_id, time.time())
        return "Uploaded"

@app.route('/index', methods=['GET', 'POST'])
def index():

    return render_template('index.html', analysis="test")

@app.route('/data', methods=['GET', 'POST'])
def data():


    return render_template('data.html', analysis="test")


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001)