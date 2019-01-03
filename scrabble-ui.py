#!/usr/bin/env python

from flask import Flask, render_template
from flask import send_from_directory


app = Flask( __name__ )

jsFiles = { "ui.js", "game.js", "globals.js" }
mediaFiles = { "pop.wav" }

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
def mainpage():
    return render_template("game.html")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5050, debug=True)

