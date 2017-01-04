/*
*update.js
*
*Created by wezard706
*Original source code is https://github.com/3116igbt/16hacku_zabuton
*/


function update_iraira(raw_data) {
    console.log(raw_data);
    graph_data = proc_sensingtime(raw_data);
    console.log(graph_data);
    
    // グラフ全体の設定
    $chart_option =  {
        type: "serial",
        theme: "light",
        dataProvider: graph_data,
        marginTop: 10,
        backgroundAlpha: 1,
        categoryField: "time",
        marginRight: 50,
        categoryAxis: {
            gridAlpha: 0.2,
            axisColor: "#DADADA",
            startOnAxis: true,
            gridCount: 12,
            autoGridCount: false
        },
        graphs: [{
            fillAlphas: 0,
            bullet: "none",
            title: "test",
            valueField: "percent",
            lineThickness: 5,
            lineAlpha: 1
        }],
        valueAxes: [{
            axisAlpha: 0.2,
            dashLength: 1,
            maximum: 100,
            minimum: 0,
            title: "イライラ度（%）",
            position: "left"
        }]
    };
    
    AmCharts.makeChart("worktime_chart", $chart_option);
}

function proc_sensingtime(result) {
    var graph_data = Array();

    // data["result"]を加工
    if (result == null || !(Object.keys(result).length > 0)) { // data["result"]の中身が空か判定
        graph_data[0] = { "Sensingtime": "No Data" };
    } else {
        var i = 0;
        var j = 0;

        // Sensingtimeを加工
        while (j < Object.keys(result).length) {
            if (result[i] != null) {
                graph_data[j] = result[i];
                if (graph_data[j]["time"] != null) {
                    if (graph_data[j]["time"].slice(-12, -7) != "00:00") { 
                        graph_data[j]["time"] = graph_data[j]["time"].slice(-12, -4);
                    } else { // 日付が変わる時は時間データに日付を含める
                        graph_data[j]["time"] = graph_data[j]["time"].slice(-18, -9);
                    }
                }

                i++;
                j++;
            } else {
                i++;
            }
        }
    }
    return graph_data;
}

function change_iraira(diff_min) {
    var query = make_iraira_query(diff_min);
    var socket = io.connect();
    console.log('iraira---------------------------------');
    console.log(query);
    // socket.emit("sql", query1)
    socket.emit("sql", query);

    socket.on('chart', function(data){
        var chartData = JSON.parse(data);
        console.log(chartData);
        update_iraira(chartData);
        // update_zabuton(chartData);
    });

    // update_iraira();
}
/*
function update_zabuton(chartData) {
    if (chartData[0]['remark'] == "down") && (chartData[1]['remark'] == "up") {
        return "downup";
    } else if (chartData[0]['remark'] == "up") && (chartData[1]['remark'] == "down") {
        return "updown";
    } else {
        return 0;
    }
}
*/
function change_zabuton(diff_min) {
    var query = make_zabuton_query(diff_min);
    var socket = io.connect();
    console.log('zabuton---------------------------------');
    console.log(query);
    // socket.emit("sql", query1)
    socket.emit("sql", query);

    var updown_flag = 0;
    socket.on('chart', function(data){
        var chartData = JSON.parse(data);
        console.log(chartData);
        updown_flag = update_zabuton(chartData);
    });

    if (updown_flag == "updown") {
        return "start";
    } else if (updown_flag == "downup") {
        return "stop";
    } else {
        return 0;
    }
}
