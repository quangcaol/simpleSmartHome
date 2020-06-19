window.onload = function update() {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	if (this.readyState == 4 && this.status == 200) {
	var sensor = this.responseText.split(",");
	document.getElementById("temperature").innerHTML = sensor[0];
	document.getElementById("humidity").innerHTML = sensor[1];
	if (sensor[2] == "1") {
		document.getElementById("btn1").style.backgroundColor = "#008CBA";
	}
	else {
		document.getElementById("btn1").style.backgroundColor = "#002969";
    }
	if (sensor[3] == "1") {
		document.getElementById("btn2").style.backgroundColor = "#008CBA";
	}
	else {
		document.getElementById("btn2").style.backgroundColor = "#002969";
    }
	if (sensor[4] == "1") {
		document.getElementById("btn3").style.backgroundColor = "#008CBA";
	}
	else {
		document.getElementById("btn3").style.backgroundColor = "#002969";
    }
	if (sensor[5] == "1") {
		document.getElementById("btn4").style.backgroundColor = "#008CBA";
	}
	else {
		document.getElementById("btn4").style.backgroundColor = "#002969";
    }
	if (sensor[6] == "1") {
		document.getElementById("btn1").disabled = false;
		document.getElementById("btn2").disabled = false;
		document.getElementById("btn3").disabled = false;
		document.getElementById("btn4").disabled = false;
		document.getElementById("manual").checked = true;
		document.getElementById("fanVel").disabled = false;
	}
	else {
		document.getElementById("btn1").disabled = true;
		document.getElementById("btn2").disabled = true;
		document.getElementById("btn3").disabled = true;
		document.getElementById("btn4").disabled = true;
		document.getElementById("manual").checked = false;
		document.getElementById("fanVel").disabled = true;
		document.getElementById("btn1").style.backgroundcolor = "#000000";
		document.getElementById("btn2").style.backgroundcolor = "#000000";
		document.getElementById("btn3").style.backgroundcolor = "#000000";
		document.getElementById("btn4").style.backgroundcolor = "#000000";
	}
	document.getElementById("Lux").innerHTML = sensor[7];
	}
	};
	xhttp.open("GET","/root", true);
	xhttp.send();
}


function DHT(){
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
var sensor = this.responseText.split(",");
document.getElementById("temperature").innerHTML = sensor[0];
document.getElementById("humidity").innerHTML = sensor[1];
document.getElementById("Lux").innerHTML = sensor[2];
}
};
xhttp.open("GET","/parameter", true);
xhttp.send();
}
setInterval(DHT, 10000);

function Light(){
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
	if (this.responseText == "1") {
		document.getElementById("btn1").style.backgroundColor = "#008CBA";
	}
	else {
		document.getElementById("btn1").style.backgroundColor = "#002969";
    }
}
};
xhttp.open("GET","/light", true);
xhttp.send();
}
function Fan(){
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
	if (this.responseText == "1") {
		document.getElementById("btn2").style.backgroundColor = "#008CBA";
	}
	else {
		document.getElementById("btn2").style.backgroundColor = "#002969";
    }
}
};
xhttp.open("GET","/fan", true);
xhttp.send();
}
function Alert(){
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
	if (this.responseText == "1") {
		document.getElementById("btn3").style.backgroundColor = "#008CBA";
	}
	else {
		document.getElementById("btn3").style.backgroundColor = "#002969";
    }
}
};
xhttp.open("GET","/alert", true);
xhttp.send();
}
function Roofs(){
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
	if (this.responseText == "1") {
		document.getElementById("btn4").style.backgroundColor = "#008CBA";
	}
	else {
		document.getElementById("btn4").style.backgroundColor = "#002969";
    }
}
};
xhttp.open("GET","/roof", true);
xhttp.send();
}
function Manual(){
	var checkBox = document.getElementById("manual");
	var mode = "1";
	var xhttp = new XMLHttpRequest();
	if (checkBox.checked == true) {
		mode = "1";
		document.getElementById("btn1").disabled = false;
		document.getElementById("fanVel").disabled = false
		document.getElementById("btn2").disabled = false;
		document.getElementById("btn3").disabled = false;
		document.getElementById("btn4").disabled = false;
		document.getElementById("btn1").style.backgroundcolor = "#008CBA";
		document.getElementById("btn2").style.backgroundcolor = "#008CBA";
		document.getElementById("btn3").style.backgroundcolor = "#008CBA";
		document.getElementById("btn4").style.backgroundcolor = "#008CBA";
		update();
		}
	else {
		mode = "0";
		document.getElementById("btn1").disabled = true;
		document.getElementById("btn2").disabled = true;
		document.getElementById("fanVel").disabled = true;
		document.getElementById("btn3").disabled = true;
		document.getElementById("btn4").disabled = true;
		document.getElementById("btn1").style.backgroundcolor = "#000000";
		document.getElementById("btn2").style.backgroundcolor = "#000000";
		document.getElementById("btn3").style.backgroundcolor = "#000000";
		document.getElementById("btn4").style.backgroundcolor = "#000000";
		}
xhttp.open("POST","/mode?cbox="+mode, true);
xhttp.setRequestHeader("Content-Type", "text/plain");
xhttp.send();
}


function fanspeed(val) {
	var cuoi = val*0.01*1024;
	document.getElementById("velocity").innerHTML = val.concat("%");
	var xhttp = new XMLHttpRequest();
	xhttp.open("POST","/speed?vel="+cuoi, true);
	xhttp.setRequestHeader("Content-Type", "text/plain");
	xhttp.send();
}
