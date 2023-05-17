"""Flask application with embedded chatbot and analytics"""
from flask import Flask, render_template
from flask_bootstrap import Bootstrap
from flask_moment import Moment
from flask_wtf import FlaskForm
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired

import time

app = Flask(__name__)
bootstrap = Bootstrap(app)
moment = Moment(app)
app.config['SECRET_KEY'] = 'bdqVBWEsRebA4d@GiXm7'

chat=bot.chatbot()

class NameForm(FlaskForm):
    name = StringField('What is your name?', validators=[DataRequired()])
    submit = SubmitField('Submit')


class Chatbot(FlaskForm):
    response = StringField("",
                           validators=[DataRequired()])
    submit = SubmitField('Submit')


@app.errorhandler(404)
def page_not_found(e):
    return render_template('404.html', e=e), 404


@app.route('/', methods=['GET', 'POST'])
def index():
    form = NameForm()
    name = None
    instr = "Please proceed to the Chatbot"
    if form.validate_on_submit():
        name = form.name.data
    return render_template('index.html', form=form, name=name, instr=instr)


@app.route('/chatbot', methods=['GET', 'POST'])
def chatbot():
    form = Chatbot()
    response = None
    text=None
    if form.validate_on_submit():
        response = form.response.data
        text = chat.set_answer(response)
    prompt = chat.get_inquiry()

    return render_template('chatbot.html', form=form,prompt=prompt, response=text)

@app.route('/stat',methods=['GET','POST'])
def stat():

    analysis=chat.stat_bar(1)
    return render_template('stat.html',analysis=analysis)


if __name__ == '__main__':
    app.run(host='127.0.0.1', port=8080, debug=True)
