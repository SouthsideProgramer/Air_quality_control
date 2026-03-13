async function fetchStatus() {
  try {
    const res = await fetch('/api/status');
    const data = await res.json();

    document.getElementById('apName').textContent = data.ap_name;
    document.getElementById('ip').textContent = data.ip;
    document.getElementById('clients').textContent = data.clients;
    document.getElementById('uptime').textContent = formatUptime(data.uptime);

    document.getElementById('temperature').textContent = data.temperature.toFixed(1);
    document.getElementById('humidity').textContent = data.humidity.toFixed(1);
    document.getElementById('noise').textContent = data.noise;
    document.getElementById('lux').textContent = data.lux;
    document.getElementById('pm25').textContent = data.pm25;
    document.getElementById('pm10').textContent = data.pm10;
    document.getElementById('pressure').textContent = data.pressure.toFixed(1);
    document.getElementById('heap').textContent = data.heap;

    const badge = document.getElementById('sensorReady');
    const logBox = document.getElementById('logBox');

    if (data.sensor_ready) {
      badge.textContent = 'Sensor Online';
      badge.classList.remove('off');
      badge.classList.add('on');

      logBox.textContent =
        `Air sensor connected successfully\n` +
        `Temperature: ${data.temperature.toFixed(1)} °C\n` +
        `Humidity: ${data.humidity.toFixed(1)} %\n` +
        `Noise: ${data.noise} dB\n` +
        `PM2.5: ${data.pm25} ug/m³\n` +
        `PM10: ${data.pm10} ug/m³\n` +
        `Pressure: ${data.pressure.toFixed(1)} kPa\n` +
        `Light: ${data.lux} Lux`;
    } else {
      badge.textContent = 'Sensor Offline';
      badge.classList.remove('on');
      badge.classList.add('off');

      logBox.textContent =
        `Waiting for sensor data...\n` +
        `Check Modbus wiring / slave ID / baudrate.\n` +
        `AP is still running normally at ${data.ip}`;
    }
  } catch (err) {
    document.getElementById('logBox').textContent =
      'Cannot fetch /api/status\n' + err;
  }
}

function formatUptime(sec) {
  const h = Math.floor(sec / 3600);
  const m = Math.floor((sec % 3600) / 60);
  const s = sec % 60;
  return `${h}h ${m}m ${s}s`;
}

fetchStatus();
setInterval(fetchStatus, 2000);