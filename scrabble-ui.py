#!/usr/bin/env python

from datetime import datetime
from flask import Flask, render_template
from flask import send_from_directory


app = Flask( __name__ )

jsFiles = { "ui.js", "game.js", "globals.js", "lobby.js" }
mediaFiles = { "pop.wav" }

@app.after_request
def after_request(response):
    response.headers['Last-Modified'] = datetime.now()
    response.headers['Cache-Control'] = 'no-store, no-cache, must-revalidate, ' \
                                        'post-check=0, pre-check=0, max-age=0'
    response.headers['Pragma'] = 'no-cache'
    response.headers['Expires'] = '-1'
    return response

@app.route('/js/<path>')
def staticJs(path):
    if path not in jsFiles:
        return "not found"
    return send_from_directory('js', path)

@app.route('/media/<path>')
def mediaFile(path):
    if path not in mediaFiles:
        return "not found"
    return send_from_directory('media', path)

@app.route('/')
def game():
    return render_template("game.html")

@app.route('/lobby')
def lobby():
    return render_template("lobby.html")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5050, debug=True)

