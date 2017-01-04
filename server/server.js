/*
* server.js
* Created by 3116igbt
* Original source code is https://github.com/3116igbt/16hacku_zabuton
*
* server.js is connect all devices and proccessing states.
*/


require('date-utils');
var twitter = require('twitter');
var tweet_client = new twitter({
    consumer_key: 'AAAAAAAAAAAAAAAAAAAAAAAAa',
    consumer_secret: 'AAAAAAAAAAAAAAAAAAAAAAA',
    access_token_key: 'AAAAAAAAAAAAAAAAA',
    access_token_secret: 'AAAAAAAAAAAAAA',
});
var tweet_params = { screen_name: 'nodejs' };

var http = require('http');
var fs = require('fs');
var socketIo = require('socket.io');
var pg = require('pg');

var wss = require('ws').Server;
var port = 3000;
var mport = 12020;
var DBhost = "postgres://postgres:password@localhost:5432/hacku";
var client_sit = new pg.Client(DBhost);
var client_frust = new pg.Client(DBhost);

var sit_second = 100 * 1000;

var httpserver = http.createServer();
httpserver.listen(port);

var ws_Browser = socketIo.listen(httpserver);
var ws_Mbed = new wss({ port: 12020 });
var wsm_groval;
var count = 0;

var mbed_id = [];

var sit_tweet = "今日も一日がんばるぞい！";
var longsit_tweet = " ﾀ ｽ ｹ ﾃ";
var frust_tweet = "お仕事楽ﾋｨｨｨｨｨ~~~~~~~";
var stand_tweet = "帰るンゴwwwwww";


//sql関係

sql_insertsit = function (status) {
    client_sit = new pg.Client(DBhost);
    client_sit.connect(function (err) {
        if (err) throw err;
        console.log("sql");
        var timestamp = new Date().toFormat("YYYY-MM-DD HH24:MI:SS");
        client_sit.query("INSERT INTO zabuton(time,mode,remark) values('" + timestamp + "', 'sit',' " + status + "');", function (err, value) {
            if (err) throw err;
            console.log("sql commit");
        });

    });
};

sql_insertfrust = function (value) {
    client_frust = new pg.Client(DBhost);
    client_frust.connect(function (err) {
        if (err) throw err;
        console.log("sql");
        var timestamp = new Date().toFormat("YYYY-MM-DD HH24:MI:SS");
        var percent = value;
        client_frust.query("INSERT INTO iraira(time,percent) values('" + timestamp + "', " + value + ");", function (err, value) {
            if (err) throw err;
            console.log("sql commit");
        });
    });
};


sql_custom = function (query,socket) {
    var result;
    var client_select = new pg.Client(DBhost);
    client_select.connect(function (err) {
        if (err) throw err;
        console.log("sql");
        client_select.query(query, function (err, value) {
            if (err) throw err;
            console.log("get data");
            var strdata = JSON.stringify(value["rows"]).replace(/T/g, ' ').replace(/Z/g, '');
            console.log(strdata);
            socket.emit("chart", strdata);
            console.log("send to blowser");
            client_select.end();
        });
    });

};

//twitter
var tweet = function (sentence) {
    var timestamp = new Date().toFormat("YYYY-MM-DD HH24:MI:SS ");
    tweet_client.post('statuses/update', { status: timestamp+sentence },
        function (error, tweet, response) {
            if (!error) {
                console.log(tweet)
            }
        });
};


//webクライアント向け
httpserver.on("request", function (req, res) {
    if (req.url.match(/socket.io/) == null) {
        fs.readFile(__dirname + req.url, function (err, data) {
            console.log(__dirname + req.url);
            if (err) {
                console.log(err);
                res.writeHead(500);
                return res.end('server error');
            }
            console.log("sucsess");
            switch (req.url) {
                case "/status.html":
                    res.writeHead(200, { "Content-Type": "text/html; charset=UTF-8" });
                    res.end(data);
                    break;
                case "/index.html":
                    res.writeHead(200, { "Content-Type": "text/html; charset=UTF-8" });
                    res.end(data);
                    break;
                default:
                    res.writeHead(200);
                    res.end(data);
            }
        });
    }
});


toMbed_start = function () {
    if (wsm_groval != null) {
        wsm_groval.send("start");
    }
};

toMbed_stop = function () {
    if (wsm_groval != null) {
        wsm_groval.send("stop");
    }
};

ws_Browser.sockets.on("connection", function (socket) {
    socket.emit("message", "replyed", function (data) {
        console.log("recved!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! :" + data);
    });

    socket.on("greet", function (data) {
        console.log("greet recved!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! :" + data);
        count++;
        socket.emit("acks", "counts: " + count.toString());
    });

    socket.on("start", function (data) {
        console.log("!!!!!!!start!!!!!!!!!!! :" + data);
        toMbed_start();
    });

    socket.on("stop", function (data) {
        console.log("!!!!!!!stop!!!!!!!!!!! :" + data);
        toMbed_stop();
    });

    socket.on("sql", function (data) {
        console.log("!!!!!!!sql!!!!!!!!!!! :" + data);
        sql_custom(data, socket);
    });
});




ws_Mbed.on('connection', function (ws) {
    wsm_groval = ws;
    countor = 0;
    console.log("connect from mbed");

    ws.on('message', function (message) {
        console.log('received:'+ message);
        var type;

        JSON.parse(message, function (key, value) {
           // console.log('key,value : %s , %s', key,value);

            if (key == "name") {
                console.log("name");
                type = value;
            }

            if (key == "value") {
                console.log("value type : "+ type);
                if (type == "test") {
                    console.log('messtype:test , ' + value);
                    var data = {};
                    data["name"] = "test";
                    data["value"] = "hello";
                    var strdata = JSON.stringify(data);

                    ws.send(strdata);
                    
                    console.log('data send ' + strdata);
                } else if (type == "status_sit") {
                    console.log('messtype:status_sit , ' + value);
                    sql_insertsit(value);
                    if (value == "down") {
                        tweet(sit_tweet);
                        setTimeout(function () {
                            tweet(longsit_tweet);
                        }, sit_second);
                    } else {
                        tweet(stand_tweet);
                    }
                } else if (type == "status_frust") {
                    console.log('messtype:status_frust , ' + value);
                    var frust = Number(value);
                    ws_Browser.sockets.emit("frustvalue", value);
                    sql_insertfrust(frust);
                    if (frust>50)
                        tweet(frust_tweet);
                }
            }
        });

    });

});