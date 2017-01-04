/*
*utility.js
*
*Created by wezard706
*/


function make_iraira_query(diff_min=10) {
	// 10分前から現在までの振動データを取得するクエリを作成
	var now = new Date();
	var prev = new Date(+new Date() + (diff_min * 60 * 1000));
	var options = {year: "numeric", month: "2-digit", day: "2-digit", hour: "2-digit", minute: "2-digit", second: "2-digit"};
	prev.toLocaleDateString("ja-JP", options);
	now = format_time(now);
	prev = format_time(prev);

	// now = '2016-12-17 21:40:00';
	prev = '2016-12-18 14:10:00';
	// var query = "SELECT * FROM iraira WHERE time <= '" + prev + "' AND time >= '" + now + "';";
	var query = "SELECT * FROM iraira WHERE time >= '" + prev + "';";
	console.log(query);
	return query

}

function make_zabuton_query(diff_min=10) {
	// 10分前から現在までの振動データを取得するクエリを作成
	var now = new Date();
	var prev = new Date(+new Date() + (diff_min * 60 * 1000));
	var options = {year: "numeric", month: "2-digit", day: "2-digit", hour: "2-digit", minute: "2-digit", second: "2-digit"};
	prev.toLocaleDateString("ja-JP", options);
	now = format_time(now);
	prev = format_time(prev);

	now = '2016-12-17 21:40:00';
	prev = '2016-12-17 21:50:00';
	var query = "SELECT * FROM zabuton ORDER BY id DESC LIMIT 2;";
	return query
}		

function format_time(time) {
	var year = time.getFullYear();
	var month = zero_pad(time.getMonth() + 1);
	var day = zero_pad(time.getDate());
	var hour = zero_pad(time.getHours()); 
	var min = zero_pad(time.getMinutes());
	var sec = zero_pad(time.getSeconds());
	var new_time = year + '-' + month + '-' + day + ' ' + hour + ':' + min + ':' + sec;
	return new_time
}

function zero_pad(target) { 
	return ('0' + target).slice(-2);
}

/*
var start = new Date();

// 初期化
var hour = 0;
var min = 0;
var sec = 0;
var now = 0;
var datet = 0;

function disp(){

	now = new Date();

	datet = parseInt((now.getTime() - start.getTime()) / 1000);

	hour = parseInt(datet / 3600);
	min = parseInt((datet / 60) % 60);
	sec = datet % 60;

	// 数値が1桁の場合、頭に0を付けて2桁で表示する指定
	if(hour < 10) { hour = "0" + hour; }
	if(min < 10) { min = "0" + min; }
	if(sec < 10) { sec = "0" + sec; }

	// フォーマットを指定（不要な行を削除する）
	var timer1 = hour + ':' + min + ':' + sec; // パターン1
	var timer2 = hour + '時間' + min + '分' + sec + '秒'; // パターン2

	// テキストフィールドにデータを渡す処理（不要な行を削除する）
	// document.form1.field1.value = timer1; // パターン1
	// document.form1.field2.value = timer2; // パターン2
	$('#sittime_value').text(timer1);

	setTimeout("disp()", 1000);

}
*/