async function updateDashboard() {
    try {
        const response = await fetch('/api/weather');
        const data = await response.json();

        document.getElementById('temp').innerText = data.temperature.toFixed(1);
        document.getElementById('hum').innerText = data.humidity.toFixed(0);
        document.getElementById('pres').innerText = data.pressure.toFixed(1);
        document.getElementById('uv').innerText = data.uv.toFixed(1);
        document.getElementById('wind').innerText = data.wind_speed.toFixed(0);
        document.getElementById('dir').innerText = data.wind_direction;
        
        const rainEl = document.getElementById('rain');
        rainEl.innerText = data.rain ? 'SÍ' : 'NO';
        rainEl.className = data.rain ? 'rain-on' : 'rain-off';
        
        document.getElementById('timestamp').innerText = `Última actualización: ${data.timestamp}`;
    } catch (error) {
        console.error('Error fetching data:', error);
    }
}

// Actualizar cada 3 segundos
setInterval(updateDashboard, 3000);
updateDashboard();
