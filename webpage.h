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


const char altHtml[] PROGMEM = R"===(
<!DOCTYPE html>
<html lang='de'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>CO2-Ampel - Anwendungsbeispiel des MVP3000 Frameworks</title>
    <script>
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
            width: 100%;
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
            width: 100%;
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
            <img src='data:image/png;base64,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' />
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
            <img src='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAUAAAABNBAMAAAAlV6DzAAAOl3pUWHRSYXcgcHJvZmlsZSB0eXBlIGV4aWYAAHjapZlZkuRIbobfeQodwR2+ADiOr2ZzAx1fHxiRpe6a0kNrMiwjmAzSFwD/AuZz/vtf9/kvfkqr8tSm1r33xE/16jI4sPT5Ge97TvV9f3/qz1H++/mn7u9NwqnCZ/n8af3zmX/Of2/4+cyDo/aXgWx9v5h//8Lrd3z7baDvRCVWJBx8F/L4d6Ainy/yd4Dx2VbqbvrXLczz+fzZyBsGfp94M0/zO9jnu9/+rkr0dmOeInJKLon3Ur4LKPFbnzLii/e9c2EujeP4GRz9rISA/ClOv36cFd3zScUfLvpbVtL/ka3yE6Pfs1Xle0n5Lcj91+cfzz+5/Tkrb+j/Wj/2PZK/nzdL67Oi36Ifv/duu++e2cWonVD376Z+tvgecR1JqTG1PSytJ+W3MYS+L+dlVPWiFDbzTV4rexaycnPNO49883k/V14sscp5RDkQWVLek1ZUXFaJ/NV45StavOxiJHm9aa9Ffq0lv9N6Ws87mzHzzlwqmcEyt/zj1/NPb7g3YptzxJLU509+RSLYLCMyF+9cFvC+36C2N8A/r99/Iq+FDLaIckDECez8DDFb/l8mKG+iCxc2Pj8YzLq/AxAipm4sJhcyQNZARe45qYjmTCCNBA2WDoBkkoHcmmwWKbWAIhWTmJpbNL+XShNOP5yHzMhEK70oufEySFatjfrRatTQaFBga603bda8jV567a33rj1IcWjR+mjTrqqmrsOKVWvWTc3Mbbh4gTSbd1c3dx+DOQcjD+4eXDDGlFlmne2Zfeq06XMsymfV1VZfumz5Glt22fDH7lu3bd/j5EMpnXra6UePHT/jUmq3PLfedvvVa9fv+JW1b1r/7fUPspa/WZM3U3Gh/soaZ1V/hshBJy1yRsJQkUzGNVJAQUvkLFmuVSJzkbPkAiqasMgWOds5MkYG68nSbv7J3SOfjEbm/qO8PVr/ljf5/2buidT9w8z9e97+lLUdMrTejH1QGEFNBfQtZ3ddU/YlLHPNzFGrYyerfT8bDpylrzwRoaF2dPNtJeDT0mGX50SsLu/MJq16JsBIHbnc2k9MUXc/j64h2ySki9n7TlN6PWdpZZlL++XC2xjLRtu7EdFemGUsQ5b3XiUfzWcf5MgrK5uCm1gkwZPCsNp29aGzs6Ttucc0rgIDptpLvpG3IsvzLJSRVBiyWFACOV+bL0pSH83r0lvKDANRPBW+vESWwFcbkEKavrxfn1IYcA/Lc5L+ZNpHjxLZK+s54xTr5dyz79G5y+zLLwWCjl/vBNFq9pSXFTJGQrXcY/fJaSkx03zrEALYPCsTJ5vGMiiBe4t0SN3TaT6o01rmKiNKPd9YLvE8vp/hLjL6blqxA5vKOeqo/pl6YwFs0NapOi7Vt2C2ItMy2cnrKppyD1tttz75RpTWhNKJm8Fwq0xpJBxBksnCl7KHOlnzlTXvmdR7nZScXWqYv0chog/Gy4hAWs2FeGWL41AI9u9nVdY1zpgei2hAIXdyi4S1tNY63GUuVJg83XJRzUI6bMDnTVZvUtotEAxjLWRwlwOwBNx1DNgKcIG7QWH66hANWMrPtcxUqQQGZVLwQPJc17zOYSi8B+OMdFKrO1NQdjX7zrOPTIVJPjWRk3vRtTp6uzJGuxsQZSqdXI25yDOcVsjmuDWd26lOGKq3mrs7iGGoUidaAxzGE/FPa8BOmTKCFsbeV/eNpOEkT/K7hiW4x0GZtQylcIjsFwSr+0mbvZ/xtOkR/0H8NyxwtOGYRCdOgkTa2BH53gsRnXetXQ9eYmA7KtDDeFC7m9rIj+UzgY4Ec252IrBcMAvEEzx2C2VNbRbZKx0DVAc2tgM33uKqpC57OW0/q/obWnzx5OsDJMlqpmyWw7meyG2U3mQxxIRtmuxwVNu1EwzKZXRs01PffWBu9qBowiOB/mEQ9Mod9E0nL8UW2eaPGUiPWRfIqtnGRBHQjNyefX0UYdNUtR3sWMzN5ePcKiys5du3OTt39tEyNdpJPSXK6Yg1RXeVOlqjOJtpyt+uQbtjUOEbBoVYxQhBpaBIxgEuhzKTumKJWEw4pJ61Bnu25ybdYd7vgP0q+d6diaUGGJAvcMMbq7sW+DyC09Jgq2FntDaNe0o3aMQOdJQWeQchuVMHEoEpSj0HtQ0EjLgfiA8mx2HOWrUwACQATxxIe93Z6hPzb2N+VKEw/4bk+IK4krhtbUKPbhDvCv5P5zjfgLILD1NSSQ/sLjFQuJkNRziA63WGMRUQ3l4lxmbBjiyTrLO8PWcBOsUH0gTbehAQeZbbHlQ8oe63xehUJo2PwYGI2RgFIemAZOnLKhXwmMWtGA6SMC/lSYpgefVHFT6rFgGvmDohVksHjOCXXYL7ur00wrumfNDXxXvjHo9UUJWMhCQ+A1IZ4KguJ2SIH9goQZbDmU2wEwpZA4O+cAQH0u6gE7gUkqPwM9XicPFzBYFlYxoDlIhGJ9k9ZBbw8gc0Y1BJx+FByhlCBB/iM0oFiqKYs7e8HkjikKBX0ukmLAoKOjer9eVRL4YdAA4sJrYmFBmE1bAwBywRedgE0/C0oejiAEVI1gba9Bi2BFEn4dw4F314mRBW3A2y2QHUQ0Y3HO6YF8garn5wcgcVBlQDIQyugRPewmIJW6B7QS8bvzhxWhaZYASHR0uLz8Lk6LmZSx4IejUqsRiKWcJjnXTZIbK1YyDQ2i7Sn5jfgupXsnO3noZvL+gspMTt9lBnxGWJoxcwoLPfdysw4/kL6345F2avGKzRkBMEAPfS0nhT/ByKHPshfSyYc7IsfHND8kD7XQcRgq4Fk+SQDFVM9bG8RVcJFR4uWyNFLGnXE94RbfUG8TeOPKws7oZetG+Ci0wDe7To1AEJ7YU2F7gRrkNycaboJm4E2/7zVCOdueeZWJezQPfQi0LCNWtVKB5rmiiJJBerUQ8hY2PaoxNFT89TMNh7ht8kZMAZNkPxPOSzekHcqb7XQRKC8E0O9bY98CuYpLPjaQqAy7gRzhA3CYBS3uAzdXqyjzKCVFwCVgr7iHmItjqBwDkm7j28m13Qg0Ls9ZDwV08wBbiayQyhGbuS2zMYVmASAci+L7UPsXFzLXeTlN1l4F8F9wJEmHUSiBzR5Sz2GPsCdXEEv0U+UlALMtugDvwuNx/6M8qOO8G54pJI9LMmeisN4VnsKR/cHncAa45vi76jhWQrmEMXo6LuARV4Y7JWIWlyCaQuTY0Htneu8Vjn9TYprB/ayBFSRRMy1aXpCSmlAZoRzEMVN4ritd2I03pY4pw40IRrpRuI/cEhaybbXP8KoYbnwBEQLbgFy11Qox7+fXeTiRtkfmIUjdmV3hhObK9NNi1iU3Bl9C6aB9YRC0wlisIxnQ4nQN2jZiqe1wa6+CCuFD/gcFQ8tpmvsSntc+4EGdJow/tgDoGCb/G4INvCDR54B4YD3Xjo+sC8p16hHqGPMY1QqeFQGXXjslM+0bS1bwj9wCGoEoZKC51NHtFtHArwIQtthH+ak3fqBQcQ/j16I8zCtqixFawM1VE/StNIaPLC4EvxRu8hCIORtanRycX6XkeaCxOPSg2OsGgp/CN8QS7AYjy2iSwbc0F19CMUdIV+7KEvm3tPwtIZTDK1BcRZDsJiQYwrDA8UcT50QZNRWoprDl1FZ5UQGa7iIXxwR6JHuWhdr9QPtYtPRGztXQZDhEkEKScaSPrmdddGh+8cIYWQ5d2CGfWxQ1iw8Q2NR3LiuZQS4xzWDgQw2gpwEZcC6dMUp5kPwhS4yrNeqrdDbJkmAbgwiGD6Vq5Qp2Q8CBK0woRGTcGgfXY6HnZZF3xDIicUDqpJMkr4xCMBxb7FYgqeTIkuPPpuFP5Ki5b0Anqqktbi86SpQJKBdZISDS0IWv6sjGCQIKg943+JDyaFyg2PQ4WhP5fOH68OTpHRNShV9BbjDmXE4EG8CM7z9oX0wtFJ9OBENBRzhWwEvcFblMm5AX3qFWCiRVFoKywO1jFFO0pzezDsnEDhEbnZJIMtKIQ+lZziFCjfoGj6/H5DfDPWDa/E1LgeKo12EHtHM3Geg6Ih9oTBx4XpSHCvEkVDq9JonQkV0AdNOBOoXhDGw3xlhnmn7A8I60ujO8JXNkzjTBc6wTLM8NEt0EcSQU6gJAeFlJC9k5HQ1qFNRJ7G/YVhtSeeCCA1CxcNUSQNs7sQkrFBJx0jjtDhTVyiWMepQZZ3hXFK8RghbsxdtCGQBxwkEElzHSYaDexwa7ULgYKpKOwUXRM9Oa1FK+ALHR10x8Et0bhnEHUeZghelEDijspjYsy4x3MPvOVrtt+nESOarSE5NFxCL4OCKbODF9S3OSbPSm1hWo3e/n3SHT0VmO8+1nqLilbUojnC3eFOTuu4NEJAHzVxnNouju0VHGS/djqEOih7C+dLpLkCx7/rG694SjY+PJOiC2uQLNCnngAaNvkh4Btc0V3ZiQ2z0hGUGPIUTz+jhb/hpSpawMG4b5MSyvjXpOGznVAAY8qaF+RKfnCf+DtcSopOLyJOzLaQEzaWw5hUnyvcPKJRMk2AFKiWHaCJM+wGig0LbRSSnk4QsvhPDdNgh3FzOENM9Z6Sor2YjWhgElPodhpPP/u151BGo2jfh3A03XQ0NBdwliFefsk/i5odxHo88iOeaCjgs3hIhhr0J5qxaMtqdEb4/3UdDCUyQ71vWH+WKGeNEtQh2ITqtPtumcwtg2nvEajgwaxSWS1Wv1kOmgZZTWqppBTdIIGkIPiqxINirMQGavSCFd846ZRj/5xLDwtr8Q8ojRYJM9MgK4w//IxEI9xkBaqnL8B30doRaubEHonQaKOQqDuXFn0EYxmqj3emciwWRiMqudfgR4GWoCpcqbMTAkDPSB7RcLzmCTXFZ0wgMR7qauPegRrVQ5TK0U7XlEcN66U0aNH+Bv3naDNrdJS0nt5BAbaB/ktZFtYPcVQNocE5oHgzHkMMXCOGzJ//AdWkIp5odQ06AAABhGlDQ1BJQ0MgcHJvZmlsZQAAeJx9kT1Iw0AcxV9TRSktCnYQdchQnSyIFXHUKhShQqgVWnUwufQLmjQkLS6OgmvBwY/FqoOLs64OroIg+AHi7OCk6CIl/i8ptIjx4Lgf7+497t4BQqPMNKtrAtD0qplKxMVMdlXseUUA/QhhGDGZWcacJCXhOb7u4ePrXZRneZ/7c4TUnMUAn0g8ywyzSrxBPL1ZNTjvE4dZUVaJz4nHTbog8SPXFZffOBccFnhm2Eyn5onDxGKhg5UOZkVTI54ijqiaTvlCxmWV8xZnrVxjrXvyFwZz+soy12mOIIFFLEGCCAU1lFBGFVFadVIspGg/7uEfcvwSuRRylcDIsYAKNMiOH/wPfndr5WOTblIwDnS/2PbHKNCzCzTrtv19bNvNE8D/DFzpbX+lAcx8kl5va5EjoG8buLhua8oecLkDDD4Zsik7kp+mkM8D72f0TVlg4BYIrLm9tfZx+gCkqavkDXBwCIwVKHvd4929nb39e6bV3w+YdnK2Z5dB9wAADRppVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADw/eHBhY2tldCBiZWdpbj0i77u/IiBpZD0iVzVNME1wQ2VoaUh6cmVTek5UY3prYzlkIj8+Cjx4OnhtcG1ldGEgeG1sbnM6eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IlhNUCBDb3JlIDQuNC4wLUV4aXYyIj4KIDxyZGY6UkRGIHhtbG5zOnJkZj0iaHR0cDovL3d3dy53My5vcmcvMTk5OS8wMi8yMi1yZGYtc3ludGF4LW5zIyI+CiAgPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIKICAgIHhtbG5zOnhtcE1NPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvbW0vIgogICAgeG1sbnM6c3RFdnQ9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9zVHlwZS9SZXNvdXJjZUV2ZW50IyIKICAgIHhtbG5zOmRjPSJodHRwOi8vcHVybC5vcmcvZGMvZWxlbWVudHMvMS4xLyIKICAgIHhtbG5zOkdJTVA9Imh0dHA6Ly93d3cuZ2ltcC5vcmcveG1wLyIKICAgIHhtbG5zOnRpZmY9Imh0dHA6Ly9ucy5hZG9iZS5jb20vdGlmZi8xLjAvIgogICAgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvIgogICB4bXBNTTpEb2N1bWVudElEPSJnaW1wOmRvY2lkOmdpbXA6NGI5OGQxOTYtNWU2NC00OTEwLWIxMWQtNWQ1ZmYxOTI5NDZhIgogICB4bXBNTTpJbnN0YW5jZUlEPSJ4bXAuaWlkOmIzOThlM2YxLWJhOWItNDJjZi05YTNiLWNlMTc0Y2FjOWNjOSIKICAgeG1wTU06T3JpZ2luYWxEb2N1bWVudElEPSJ4bXAuZGlkOjU4ZGRmNjI4LTQ2Y2UtNDAyNi05MWVjLTY4NGM3NmFlMDEzYSIKICAgZGM6Rm9ybWF0PSJpbWFnZS9wbmciCiAgIEdJTVA6QVBJPSIyLjAiCiAgIEdJTVA6UGxhdGZvcm09IkxpbnV4IgogICBHSU1QOlRpbWVTdGFtcD0iMTcyOTA4NTYzNDYyODEyMiIKICAgR0lNUDpWZXJzaW9uPSIyLjEwLjMwIgogICB0aWZmOk9yaWVudGF0aW9uPSIxIgogICB4bXA6Q3JlYXRvclRvb2w9IkdJTVAgMi4xMCI+CiAgIDx4bXBNTTpIaXN0b3J5PgogICAgPHJkZjpTZXE+CiAgICAgPHJkZjpsaQogICAgICBzdEV2dDphY3Rpb249InNhdmVkIgogICAgICBzdEV2dDpjaGFuZ2VkPSIvIgogICAgICBzdEV2dDppbnN0YW5jZUlEPSJ4bXAuaWlkOjE3ZTA4MDczLWFjOTQtNGM0ZS1iYTdmLTVjYTRiZmUxMTM1NSIKICAgICAgc3RFdnQ6c29mdHdhcmVBZ2VudD0iR2ltcCAyLjEwIChMaW51eCkiCiAgICAgIHN0RXZ0OndoZW49IjIwMjQtMTAtMTZUMTU6MzM6NTQrMDI6MDAiLz4KICAgIDwvcmRmOlNlcT4KICAgPC94bXBNTTpIaXN0b3J5PgogIDwvcmRmOkRlc2NyaXB0aW9uPgogPC9yZGY6UkRGPgo8L3g6eG1wbWV0YT4KICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgIAo8P3hwYWNrZXQgZW5kPSJ3Ij8+8necDwAAAA9QTFRFAAAAI4l3WJWWcq6gpczGFAeV9wAAAAF0Uk5TAEDm2GYAAAABYktHRACIBR1IAAAACXBIWXMAABcSAAAXEgFnn9JSAAAAB3RJTUUH6AoQDSE2Z5dvWAAABmNJREFUaN7tmmmCoyoQgDFwAFEPoN3vABBzAJ3k/md6UZaqYhPNLH+aHzNdisUnFLVgGKtqTyn7BzvdKh8rdpOu9QuVB+jTmCud6cF48Ih9yndtYdy9PZDWvbX7hd52W+3VBQ1mVOhNKzzWU0CvwA/kgDj0eRQA/WNSlQBv/vICgH0K0MwZkkejQYTMCPA9dg6QY0V5wEi7Wa4k4P4KMppCHfLw8JkkoK4DjLQ3oDYCbCmg6SVkyEwAhwwgl1WAsfbGowDg4/XSbjAjP4M3GtyIixt7dq+eAVyd0Qr9Bvye323faNsfIwA6ixNeewOsHtBPtR1qsRDK3eg912j/2BT8mvy7xoAAL/QI23WEXeUZYFlacBkR4I0C7jf9SKM3lwEBGqV9EpCDs2BCAaCigIL6lB4AxxBQBIBkJOQAMKCf9hiw8VegJQBvsfYG9gMBZBnABraziAA53vsEUGNHmgdE2jkF7GsAR6t1QO5YYUCG9z4BnFBEqQN0I2+XBqOuCOi0aTSSjgB1DhA58RLgirTbRxygCgB5AIh3Uh6w+RAw1D5awHVftpKbWR3EhLRadbWAyzEg2utuNndAvt9Gjvo5SfQIjjz6EqA4AagSgGJ//CDUfbPrM3gGcEwAbkO1MSDND9lfAkzN4PZvf5RuKfZ3bDA5g9sYdQmrvg5YuYsd4IQBtw6KAqKUf/7SZJO0jCz3H/KDE3Izu+bWA74zoMeL+nMfquojyRKGukrAIRFJ9v96GkloN+e3o1i8RLFY2bePAIfLsdimn1MB0K8Rhw5xNtPg5Mynt2N9NoO0B4DO9rKAznwFWJNGWiFf78lWWnGW5miWcj6oonxwgGo0C9hgv0NzXo61Q/Uw4AEFcqXrmAUE7QJn1APKrHOAfvJXW/8KTVb963tGlVVj762kZDSvcZcFQK99ctob/K4FQGqEsp8h2PCoEnOdzAEEPo2Y7+DpUoBEe8fCpSsBeh+qg2hNARU4MdwndQKQAsQPmic94HoA2KTK4OhkYaDHNyjC8TpAnjpZGOBOHpBTc0LTxRPnN4lDJ10FGGn3gOIAEBISoeni8dQJmLvY+RRBTMFbpAE94YNRQFtab5U+BgR5+0vhgyo3tJj39qLZiuEZgnM/HPGeoJAM+yRHedz34iFbqb1ey2Ef8XrFj73qtL/YT/tpP+2n/ePWxDUEDrNDfAk7by2j1uMvDNcbQ5Ezg65ldJTmeTqWBRzgEw0uHIUm4pqRugUdj9oh+hLgkvk8xaLcCoV+HeFt7ReR71EVDtKKAEV83sjwdxxy6Rbmfk16baI1J9OtUlKUZSKrygRbXjJBlQXsIhPs6Stv4mvBnx2SEioJMyu8xocEOjThKQZsI+yezZNZWNdB77NCJBFKYFVFE1SfmyB6ZjVWbavDCaTBW/zNSVdMkIcmyBMcC/3KRe7Yqn4l/+m0BGY/FkywL5vgmpwnngFUbupsOWAlu1INurdJMCYruOn2tAkO6a1tlJn1c+bG/edFkODenzLBNomN5gwghAdzoE7aXsIPMRR+MtCdN8E0toRlVHaG7Vml9Um9lby3gZMldd0E016Q56PrGkCE0gi40wUTXCsyhaIJ2tNE/921iFuVKdDZnQITFGmKtBfsHv5IwANqQDK4Dr7zM5j1gkxGmUJUtvO0m067QH8OQAGXpOSX+JQJ8g9McIEVQIBrDLj6+TzIFH6zCaZmcKkAPMgFx7QX7IpecE0BzvO8+A/jGcApAZj3glNkgqLSBKe8GzRfkE4sscgGYlHygsVkVZSqjKa0iyeySWSFCQ6nTbCQKfhkIQ3YEzfjAPVHyeoJE5Qo+i6FSBI66nymEOeCkQmKM8mqTwHKoS4A5B8lq6dN0AJCxoKRWivBPZOsqgtesJisNhWAIpkB0uRrT7fWj5JVWVMvJWyQZtQDSVhdMmsz6qmYCx6U7OdNEFJ+DRUK1E6NTRKcQyrVS6wUiNuwhq9KVs3tGyFD1VIf106lTOFWctNL1gS7zKlRO0v046L32gpUjCi7tRma5F06LNnHsgnqehNstzTh2/8Mdfvxg54ZbNinTEnbEF11IK41wSmTShM7oX5hSUlbCDkwwe5Kyc5kCdC91H9EGog+kAoHlxfrJZkNxCqYdIU/s5m7MpKu1kvsSr0kv+4zXv1pJs/fNb55vxtXo393yT6WveDZ9j9Zvv7SIeMp1wAAAABJRU5ErkJggg==' />
            <p>Das <a href='https://www.production3000.com/mvp3000/' target='_blank'>MVP3000 Framework</a> ermöglicht die schnelle Entwicklung eines MVP (minimum viable products) zu Demonstrationszwecken: Daten aufzeichnen, verarbeiten und visualisieren.</p>
            <p>Mehr Informationen zu diesem Anwendungsbeispiel ist auf <a href='https://co2ampel.production3000.com/' target='_blank'>co2ampel.production3000.com</a> zu finden.</p>
            <p>Der <a href='https://github.com/Production3000/co2ampel' target='_blank'>Quellcode</a> ist ist Open Source und auf GitHub verfügbar.</p>
        </div>
    </div>
    <div class='footer'>
        Copyright Production 3000
    </div>
</body>
</html>
)===";
