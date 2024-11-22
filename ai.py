from flask import Flask,request
import pickle

with open('model.pkl','rb') as f:
    ai=pickle.load(f)

app=Flask(__name__)

@app.route('/')
def homePage():
    return 'AI Server Started'

@app.route('/predict',methods=['GET'])
def predict():
    v=request.args.get('v')
    hr=request.args.get('hr')
    data=[[v,hr]]
    result=ai.predict(data)[0]
    return result

if __name__=="__main__":
    app.run(host='0.0.0.0',port=2000,debug=True)