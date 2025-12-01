async function fetchLatest() {
    try{
        const r = await fetch('/latest');
        const data = await r.json();
        const container = document.getElementById('content');
        container.innerHTML = '';
        if (!data.length) {
            container.innerHTML = '<div class = "card">No Data Available</div>';
            return;
        }
        data.slice(0, 10).forEach(item => {
            const card = document.createElement('div');
            card.className = 'card';
            const time = new Date(item._ts*1000).toLocaleDateString();
            card.innerHTML = `<div style="display:flex;justify-content:space-between">
                <div><strong>${item.device_id || 'device'}</strong></div>
                <div class="muted">${time}</div>
                </div>
                <pre>temp_c: ${item.temp_c}\nhum_pct: ${item.hum_pct}\nuptime_s: ${item.uptime_s}</pre>`;
            container.appendChild(card);
        });
    } catch (e) {
        document.getElementById('content').innerHTML = '<div class="card">Error fetching data</div>';
        console.error(e);
    }
}

setInterval(fetchLatest, 1000);
fetchLatest();