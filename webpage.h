/*
Copyright Production 3000

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


const char* altHtml = R"===(
<!DOCTYPE html>
<html lang='de'>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<title>CO2-Ampel - Anwendungsbeispiel des MVP3000 Frameworks</title>
<script>
var websocket;var $=function(id){return document.getElementById(id);};window.addEventListener('load',()=>{$('calibrate').addEventListener('click',()=>{websocket.send('CALIBRATE');});connect();});function connect(){$('d1').innerHTML='-';$('d1').style.color='grey';$('ds').innerHTML='&#129488;';$('d2').innerHTML='-';$('d3').innerHTML='-';websocket=new WebSocket('ws://'+location.host+'/wssensor');websocket.onclose=function(){connect();};websocket.onmessage=function(e){s=e.data.split(/[;,]+/);$('d1').innerHTML=s[1];if(s[1]>1000){$('d1').style.color='red';$('ds').innerHTML='&#128577;';}else if(s[1]>700){$('d1').style.color='orange';$('ds').innerHTML='&#128528;';}else{$('d1').style.color='green';$('ds').innerHTML='&#128578;';};$('d2').innerHTML=(s[2]/10).toFixed(1);$('d3').innerHTML=(s[3]/10).toFixed(1);}}
</script>
<style>
body {
font-family: Arial, sans-serif;
background-color: rgb(242, 252, 248);
color: rgb(53, 53, 53);
margin: 0;
display: flex;
flex-direction: column;
align-items: center;
justify-content: center;
}
a {
text-decoration: none;
font-weight: bold;
color: #228976;
}
.container {
display: grid;
grid-template-areas:
'head head head'
'data data bastel'
'data data bastel'
'data data mvp'
'cali cali mvp';
gap: 20px;
max-width: 1200px;
padding: 20px;
box-sizing: border-box;
}
.section {
background-color: rgb(239, 239, 239);
padding: 20px;
border-radius: 10px;
border: 1px solid #228976;
}
.section h2 {
margin-top: 0;
}
.head-display {
grid-area: head;
}
.head-display h2 {
margin-top: 0;
margin-bottom: 0;
}
.data-display {
grid-area: data;
}
.bastel-display {
grid-area: bastel;
}
.mvp-display {
grid-area: mvp;
}
.cali-display {
grid-area: cali;
}
.co2 {
font-size: 3em;
font-weight: bold;
text-align: center;
padding-bottom: 50px;
padding-top: 30px;
}
.smiley {
font-size: 3em;
}
.footer {
font-size: 0.8em;
margin-top: 20px;
}
@media (max-width: 768px) {
.container {
grid-template-areas:
    'head'
    'data'
    'cali'
    'mvp'
    'bastel';
}
}
</style>
</head>
<body>
<div class='container'>
<div class='section head-display'>
<h2>CO2-Ampel - Anwendungsbeispiel des MVP3000 Frameworks</h2>
</div>
<div class='section data-display'>
<div class='co2'><span class='smiley' id='ds'>&#129488;</span> <br> <br> <span id='d1'>-</span> ppm</div>
<div>Temperatur im Gehäuse: <span id='d2'>-</span> &deg;C</div>
<div>Relative Luftfeuchtigkeit: <span id='d3'>-</span>&percnt;</div>
</div>
<div class='section bastel-display'>
<img src="data:image/png;base64,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" />
<p>Die CO2-Ampel wird hergestellt und vertrieben von der Bastelgarage (purecrea GmbH).</p>
<p>Weitere Informationen sind auf <a href='https://www.co2ampel.ch' target='_blank'>co2ampel.ch</a> und <a href='https://www.bastelgarage.ch/bauteile/sensoren/co2-gas-luftqualitat/co2-ampel-mit-dss-und-balkenanzeige' target='_blank'>bastelgarage.ch</a> zu finden.</p>
<p>Die <a href='https://github.com/bastelgarage/co2ampel' target='_blank'>offizielle Firmware</a> ist Open Source und auf GitHub verfügbar.</p>
</div>
<div class='section cali-display'>
<h2>Kalibrierung</h2>
<p>Vor der Kalibrierung muss die CO2-Ampel mindestens 5 Minuten im Freien oder direkt am offenen Fenster betrieben werden. Erst danach kann die Kalibrierung zuverlässig durchgeführt werden!</p>
<button id='calibrate'>Kalibrierung starten</button>
</div>
<div class='section mvp-display'>
<img src="data:image/png;base64,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" />
<p>Das <a href='https://www.production3000.com/mvp3000/' target='_blank'>MVP3000 Framework</a> ermöglicht die schnelle Entwicklung eines MVP (minimum viable products) zu Demonstrationszwecken: Daten aufzeichnen, verarbeiten und visualisieren.</p>
<p>Mehr Informationen zu diesem Anwendungsbeispiel ist auf <a href='https://co2ampel.production3000.com/' target='_blank'>co2ampel.production3000.com</a> zu finden.</p>
<p>Der <a href='https://github.com/Production3000/co2ampel' target='_blank'>Quellcode</a> ist ist Open Source und auf GitHub verfügbar.</p>
</div>
</div>
<div class='footer'>
Copyright Production 3000
</div>
</div>
</body></html>)===";
