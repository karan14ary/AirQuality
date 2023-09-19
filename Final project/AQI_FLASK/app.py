from flask import Flask, render_template, request, flash, redirect
import sqlite3
import pickle
import requests
import warnings
warnings.filterwarnings('ignore')
import numpy as np
boost=pickle.load(open("xg_boost.pkl",'rb'))

app = Flask(__name__)

def predict(values):
    
    if len(values) == 12:
        Input_array= np.asarray(values)
        Input_reshaped = Input_array.reshape(1,-1)

        prediction = boost.predict(Input_reshaped)
        print(prediction[0])
        res=int(prediction[0])
        return res
    
    

@app.route('/')
def home():
    return render_template('home.html')

@app.route('/index')
def index():
    return render_template('index.html')

@app.route('/userlog', methods=['GET', 'POST'])
def userlog():
    if request.method == 'POST':

        connection = sqlite3.connect('user_data.db')
        cursor = connection.cursor()

        name = request.form['name']
        password = request.form['password']

        query = "SELECT name, password FROM user WHERE name = '"+name+"' AND password= '"+password+"'"
        cursor.execute(query)

        result = cursor.fetchall()

        if len(result) == 0:
            return render_template('index.html', msg='Sorry, Incorrect Credentials Provided,  Try Again')
        else:
            data=requests.get("https://api.thingspeak.com/channels/1866516/feeds.json?api_key=PUTENDOCAGB7S018&results=2")
            coo=data.json()['feeds'][-1]['field4']
            co=data.json()['feeds'][-1]['field3']
            soo=data.json()['feeds'][-1]['field5']
            hum=data.json()['feeds'][-1]['field1']
            temp=data.json()['feeds'][-1]['field2']
            pmtwo=data.json()['feeds'][-1]['field6']
           
            return render_template('fetal.html',coo=coo,co=co,soo=soo,hum=hum,temp=temp,pmtwo=pmtwo)
            
    return render_template('index.html')


@app.route('/userreg', methods=['GET', 'POST'])
def userreg():
    if request.method == 'POST':

        connection = sqlite3.connect('user_data.db')
        cursor = connection.cursor()

        name = request.form['name']
        password = request.form['password']
        mobile = request.form['phone']
        email = request.form['email']
        
        print(name, mobile, email, password)

        command = """CREATE TABLE IF NOT EXISTS user(name TEXT, password TEXT, mobile TEXT, email TEXT)"""
        cursor.execute(command)

        cursor.execute("INSERT INTO user VALUES ('"+name+"', '"+password+"', '"+mobile+"', '"+email+"')")
        connection.commit()

        return render_template('index.html', msg='Successfully Registered')
    
    return render_template('index.html')

@app.route('/logout')
def logout():
    return render_template('index.html')


@app.route("/fetalPage", methods=['GET', 'POST'])
def fetalPage():
    data=requests.get("https://api.thingspeak.com/channels/1866516/feeds.json?api_key=PUTENDOCAGB7S018&results=2")
    coo=data.json()['feeds'][-1]['field4']
    co=data.json()['feeds'][-1]['field3']
    soo=data.json()['feeds'][-1]['field5']
    hum=data.json()['feeds'][-1]['field1']
    temp=data.json()['feeds'][-1]['field2']
    pmtwo=data.json()['feeds'][-1]['field6']
    
    return render_template('fetal.html',coo=coo,co=co,soo=soo,hum=hum,temp=temp,pmtwo=pmtwo)
@app.route("/fetalPage", methods=['GET', 'POST'])
def fetalPages():
    data=requests.get("https://api.thingspeak.com/channels/1866516/feeds.json?api_key=PUTENDOCAGB7S018&results=2")
    lat=data.json()['feeds'][-1]['field7']
    lng=data.json()['feeds'][-1]['field8']
    return render_template('predict.html',lat=lat,lng=lng)
@app.route('/suspect')
def suspect():
   return render_template('suspect.html')
@app.route('/pathological')
def pathological():
   return render_template('pathological.html')


@app.route("/predict", methods = ['POST', 'GET'])
def predictPage():
    if request.method == 'POST':
        to_predict_dict = request.form.to_dict()
        # name = list(to_predict_dict.values())
        to_predict_list = list(map(float, list(to_predict_dict.values())[:-2]))   #[-2]
        print(to_predict_list)
        print(len(to_predict_list))
        out = predict(to_predict_list)
        
        print("Air Quality is {}".format(out))
        if out > 0 and out < 50:
            print('Good')
            buc="GOOD"
        elif (out > 51 and out < 100):
            print('Satisfactory')
            buc="Satisfactory"
        elif (out > 101 and out < 200):
            print('Moderate')
            buc="Moderate"
        elif (out > 201 and out < 300):
            print('Poor')
            buc="Poor"
        elif (out > 301 and out < 400):
            print('Very Poor')
            buc="Very Poor"
        elif (out > 401 and out < 500):
            print('Severe')
            buc="Severe"

        print(to_predict_dict)
        import json
        from urllib.request import urlopen

        url="http://ipinfo.io/json"
        response=urlopen(url)
        data=json.load(response)

        print(data['loc'])
        print(data['city'])
        print(data['region'])

        locc=" \n City is Bangalore  \n region is {}".format(  data['region'])
        

        print("hellooo")
        data=requests.get("https://api.thingspeak.com/channels/1866516/feeds.json?api_key=PUTENDOCAGB7S018&results=2")
        lat=data.json()['feeds'][-1]['field7']
        lng=data.json()['feeds'][-1]['field8']
        return render_template('predict.html', prediction=out,bucket=buc,locc=locc,lat=lat,lng=lng)    ## , pred = out, name=name,
    
    return render_template('predict.html',lat=lat,lng=lng)

if __name__ == '__main__':
	app.run(debug = True)
