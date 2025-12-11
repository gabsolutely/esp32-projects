// static/script.js
const tempEl = document.getElementById('temperature');
const humEl = document.getElementById('humidity');
const fanEl = document.getElementById('fan');
const stateEl = document.getElementById('state');

async function fetchLatest() {
    try {
        const response = await fetch('/latest');
        const data = await response.json();

        console.log("Fetched:", data);

        if (data.state === null || data.state === undefined) {
            stateEl.textContent = "State: N/A";
        } else if (data.state) {
            stateEl.textContent = "State: ARMED";
        } else {
            stateEl.textContent = "State: DISARMED";
        }

        if (data.state === false) {
            tempEl.textContent = "Temp: --";
            humEl.textContent = "Humidity: --";
            fanEl.textContent = "Fan Status: --";
            return;
        }

        if (data.temperature != null)
            tempEl.textContent = "Temp: " + data.temperature.toFixed(1) + " °C";
        else
            tempEl.textContent = "Temp: --";

        if (data.humidity != null)
            humEl.textContent = "Humidity: " + data.humidity.toFixed(1) + " %";
        else
            humEl.textContent = "Humidity: --";

        if (data.fanOn != null)
            fanEl.textContent = "Fan Status: " + (data.fanOn ? "ON" : "OFF");
        else
            fanEl.textContent = "Fan Status: --";

    } catch (err) {
        console.error("Error fetching latest data:", err);
    }
}

setInterval(fetchLatest, 1000);
fetchLatest();
