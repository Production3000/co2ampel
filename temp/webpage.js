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


// Minified:
// var websocket;var $=function(id){return document.getElementById(id);};window.addEventListener('load',()=>{$('calibrate').addEventListener('click',()=>{websocket.send('CALIBRATE');});connect();});function connect(){$('d1').innerHTML='-';$('d1').style.color='grey';$('ds').innerHTML='&#129488;';$('d2').innerHTML='-';$('d3').innerHTML='-';websocket=new WebSocket('ws://'+location.host+'/wssensor');websocket.onclose=function(){connect();};websocket.onmessage=function(e){s=e.data.split(/[;,]+/);$('d1').innerHTML=s[1];if(s[1]>1000){$('d1').style.color='red';$('ds').innerHTML='&#128577;';}else if(s[1]>700){$('d1').style.color='orange';$('ds').innerHTML='&#128528;';}else{$('d1').style.color='green';$('ds').innerHTML='&#128578;';};$('d2').innerHTML=(s[2]/10).toFixed(1);$('d3').innerHTML=(s[3]/10).toFixed(1);}}


var websocket;
var $ = function(id) { return document.getElementById(id); };

window.addEventListener('load', () => {
    $('calibrate').addEventListener('click', () => { websocket.send('CALIBRATE'); } );
    connect();
});

function connect() {
    // $('coninfo').innerHTML = 'Connecting ...';
    $('d1').innerHTML = '-';
    $('d1').style.color = 'grey';
    $('ds').innerHTML = '&#129488;';
    $('d2').innerHTML = '-';
    $('d3').innerHTML = '-';

    websocket = new WebSocket('ws://' + location.host + '/wssensor');

    websocket.onopen = function() {
        // $('coninfo').innerHTML = 'Connected';
    }
    websocket.onclose = function() {
        // $('coninfo').innerHTML = 'Not connected';
        connect();
    }
    websocket.onmessage = function(e) {
        s = e.data.split(/[;,]+/)
        $('d1').innerHTML = s[1];
        if (s[1] > 1000) {
            $('d1').style.color = 'red';
            $('ds').innerHTML = '&#128577;';
        } else if (s[1] > 700) {
            $('d1').style.color = 'orange';
            $('ds').innerHTML = '&#128528;';
        } else {
            $('d1').style.color = 'green';
            $('ds').innerHTML = '&#128578;';
        }
        $('d2').innerHTML = (s[2] / 10).toFixed(1);
        $('d3').innerHTML = (s[3] / 10).toFixed(1);
    }
    websocket.onerror = function() {
        // $('coninfo').innerHTML = 'Error';
    };
}
