#!/usr/bin/env python

import argparse

from datetime import datetime
from flask import Flask, render_template
from flask import send_from_directory

# default http port
DEFAULT_HTTP_PORT = 5050

# default game server port (websocket port)
DEFAULT_WS_PORT = 5051

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

@app.route('/game/<gid>')
def game(gid):
    return render_template("game.html", gid=gid, ws_port=ws_port)

@app.route('/')
def lobby():
    return render_template("lobby.html", ws_port=ws_port)

def parseArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--httpport", type=int,
                        help="Listen port (default=%s)" %
                             DEFAULT_HTTP_PORT,
                        default=DEFAULT_HTTP_PORT)
    parser.add_argument("-w", "--wsport", type=int,
                        help="Listen port (default=%s)" %
                             DEFAULT_WS_PORT,
                        default=DEFAULT_WS_PORT)

    args = parser.parse_args()
    return args

def main():
    args = parseArgs()

    global ws_port
    ws_port = args.wsport

    print "HTTP port=%d" % args.httpport
    print "Websocket (server) port=%d" % ws_port
    app.run(host="0.0.0.0", port=args.httpport, debug=True)

if __name__ == "__main__":
    main()

