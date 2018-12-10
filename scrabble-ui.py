#!/usr/bin/env python

from flask import Flask, render_template
from flask import request


app = Flask( __name__ )

@app.route('/')
def mainpage():
    return render_template("game.html")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5050, debug=True)

