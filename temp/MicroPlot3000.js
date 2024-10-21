/* 
Copyright Production 3000

Licensed under the Apache License, Version 2.0 (the 'License');
you may not use $ file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an 'AS IS' BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

MicroPlot3000 = new function() {
    let $ = this;

    $.init = function(urlFolder, selectIndex = 1, domElementId = 'plot') {
        $.urlFolder = urlFolder;
        $.selectIndex = selectIndex;
        $.domElementId = domElementId;
        // window.addEventListener('load', $.onLoad.bind($));
        $.onLoad();
    };

    $.urlFolder;
    $.domElementName;
    $.domElement;

    // Data
    $.data = [];
    $.selectIndex;

    $.xlabel = '';
    $.ylabel = '';
    $.setLabels = function(xlabel, ylabel) {
        $.xlabel = xlabel;
        $.ylabel = ylabel;
    };

    $.onLoad = function() {
        // Append the HTML and CSS to the DOM
        $.domElement = document.getElementById($.domElementId);
        $.domElement.insertAdjacentHTML('beforeend', $.htmlContent);
        // Load CSV data
        (async () => await $.loadData())();
    };

    $.loadData = async function() {
        let csvString = '';
        // csvString = '0,0,0;1,111,-11111;2,444,-44444;3,999,-99999;';
        // csvString = '0.0,0,0;0.1,0.001,-0.0001;0.2,0.004,-0.0004;0.3,0.009,-0.0009;';

        try {
            const url = `http://${location.host}/${$.urlFolder}`;
            const response = await fetch(url);
            if (!response.ok) {
                throw new Error(`Response status: ${response.status}`);
            }

            csvString = await response.text();
        } catch (error) {
            console.error(error.message);
        }

        $.appendData(csvString);
    };

    $.appendData = function(csvString) {
        // Remove all whitespaces, newline characters, etc.
        csvString = csvString.replace(/\s/g, '');
        // Remove the last semicolon
        csvString = csvString.slice(0, -1);
        
        // Split the data string into an array of objects
        csvString.split(';').forEach(row => {
            let values = row.split(',').map(Number);
            let x = values[0];
            let y = values[$.selectIndex];
            // return {x, y};
            $.data.push({x, y});
        }) ;

        $.plotData();
    };

    $.plotData = function() {
        // The idea to normalize the data for display and use html for the axis is brilliant:
        //  https://dev.to/richharris/a-new-technique-for-making-responsive-javascript-free-charts-gmp

        // Find the max/min x/y values
        let xMax = Math.max(...$.data.map(d => d.x));
        let xMin = Math.min(...$.data.map(d => d.x));
        let yMax = Math.max(...$.data.map(d => d.y));
        let yMin = Math.min(...$.data.map(d => d.y));

        // Scale the data to fit within the svg
        let dataScaled = $.data.map(d => {
            return {
                x: (d.x - xMin) / (xMax - xMin) * 100,
                y: 100 - (d.y - yMin) / (yMax - yMin) * 100
            };
        });

        // Update the polyline points
        $.domElement.querySelector('polyline').setAttribute('points', dataScaled.map(d => `${d.x},${d.y}`).join(' '));

        // Get the order of magnitude for data labeling 
        let magnitudeX = Math.log10(Math.abs(xMax - xMin)) | 0;
        let magnitudeY = Math.log10(Math.abs(yMax - yMin)) | 0;
        // Precision is 2 - magnitude, but needs to be 0 or larger
        let precisionX = Math.max(0, 1 - magnitudeX);
        let precisionY = Math.max(0, 1 - magnitudeY);
        if (magnitudeX < 0) precisionX++;
        if (magnitudeY < 0) precisionY++;

        // Very crude way to make sure the axis labels are readable
        let xlabel = $.xlabel;
        let ylabel = $.ylabel;
        if (magnitudeX > 3) {
            xMin = xMin / 1000;
            xMax = xMax / 1000;
            xlabel += ' (E+3)';
        }
        if (magnitudeX < 0) {
            xMin = xMin * 1000;
            xMax = xMax * 1000;
            precisionX -= 3;
            xlabel += ' (E-3)';
        }
        if (magnitudeY > 3) {
            yMin = yMin / 1000;
            yMax = yMax / 1000;
            ylabel += ' (E+3)';
        }
        if (magnitudeY < 0) {
            yMin = yMin * 1000;
            yMax = yMax * 1000;
            precisionY -= 3;
            ylabel += ' (E-3)';
        }
        $.domElement.querySelector('.xaxis div').textContent = xlabel;
        $.domElement.querySelector('.yaxis div div').textContent = ylabel;

        // Assign values to the x axis using the min/max values
        $.domElement.querySelectorAll('.xaxis span').forEach((span, i) => {
            let number = xMin + (xMax - xMin) / 5 * i;
            span.textContent = number.toFixed(precisionX);
        });
        // Assign values to the y axis using the min/max values
        $.domElement.querySelectorAll('.yaxis span').forEach((span, i) => {
            let number = yMin + (yMax - yMin) / 5 * i;
            span.textContent = number.toFixed(precisionY);
        });
    };

    $.htmlContent = `
<style>

    /* Outer envelope */
    .chart-outer {
        position: relative;
        width: 100%;
        height: 100%;
        padding-left: 3.5em;
        padding-bottom: 2em;
        box-sizing: border-box;
    }

    /* Inner envelope */
    .chart-inner {
        position: relative;
        width: 100%;
        height: 100%;
        font-size: small;
    }
    
    /* Axis lines */
    .xaxis {
        position: absolute;
        top: 100%;
        width: 100%;
        border-top: 1px solid black;
        text-align: center;
        line-height: 1;
    }

    .yaxis {
        position: absolute;
        left: 0;
        height: 100%;
        border-left: 1px solid black;
        line-height: 1;
    }

    /* Ticks */
    span:nth-child(1) { --index: 0; }
    span:nth-child(2) { --index: 1; }
    span:nth-child(3) { --index: 2; }
    span:nth-child(4) { --index: 3; }
    span:nth-child(5) { --index: 4; }
    span:nth-child(6) { --index: 5; }

    .xaxis > span {
        position: absolute;
        line-height: 1;
        left: calc(var(--index)*20%);
        top: 0.5em;
        transform: translate(-50%,0);
    }

    .yaxis > span {
        position: absolute;
        line-height: 1;
        left: -0.5em;
        top: calc(100% - var(--index)*20%);
        transform: translate(-100%,-50%);
    }

    /* Labels */
    .xaxis div {
        margin-top: 2em;
    }

    .yaxis > div {
        position: absolute;
        height: 100%;
        transform: translate(-4.5em, 50%) rotate(-90deg);
        transform-origin: top;
    }

    .yaxis > div > div {
        position: absolute;
        white-space: nowrap;
        transform: translate(-50%);
    }

    /* Graph SVG */
    .graph {
        position: absolute;
        width: 100%;
        height: 100%;
        overflow: visible;
    }
    
    .graph * {
        vector-effect: non-scaling-stroke;
    }
    
    .graph polyline {
        fill: none;
        stroke: red;
        stroke-width: 2;
    }

    .graph line {
        stroke: grey;
        stroke-width: 0.5;
        stroke-dasharray: 2,2;
    }

</style>
<div class='chart-outer'>
    <div class='chart-inner'>
        <div class='xaxis'>
            <span></span>
            <span></span>
            <span></span>
            <span></span>
            <span></span>
            <span></span>
            <div></div>
        </div>
        <div class='yaxis'>
            <span></span>
            <span></span>
            <span></span>
            <span></span>
            <span></span>
            <span></span>
            <div><div></div></div>
        </div>
        <svg class='graph' viewBox='0 0 100 100' preserveAspectRatio='none'>
            <g>
                <!-- Vertical lines -->
                <line x1='20' y1='0' x2='20' y2='100'></line>
                <line x1='40' y1='0' x2='40' y2='100'></line>
                <line x1='60' y1='0' x2='60' y2='100'></line>
                <line x1='80' y1='0' x2='80' y2='100'></line>
                <line x1='100' y1='0' x2='100' y2='100'></line>
                <!-- Horizontal lines, from top with 0 -->
                <line x1='0' y1='0' x2='100' y2='0'></line>
                <line x1='0' y1='20' x2='100' y2='20'></line>
                <line x1='0' y1='40' x2='100' y2='40'></line>
                <line x1='0' y1='60' x2='100' y2='60'></line>
                <line x1='0' y1='80' x2='100' y2='80'></line>
            </g>
            <!-- Data -->
            <polyline points=''></polyline>
        </svg>
    </div>
</div>
    `;

};