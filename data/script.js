async function fetchStatus() {
  try {
    const res = await fetch('/api/status');
    const data = await res.json();

    console.log(data); // Debug: kiểm tra JSON trả về

    // ==== Air Sensor ====
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

    // ==== Water Sensor ====
    document.getElementById('waterTemp').textContent =
      data.water_temp && data.water_temp !== 0 ? data.water_temp.toFixed(1) + ' °C' : 'Loading...';
    document.getElementById('ecVoltage').textContent =
      data.ec_voltage && data.ec_voltage !== 0 ? data.ec_voltage.toFixed(3) + ' V' : 'Loading...';
    document.getElementById('ecRaw').textContent =
      data.ec_raw && data.ec_raw !== 0 ? data.ec_raw.toFixed(3) + ' mS/cm' : 'Loading...';
    document.getElementById('ecComp').textContent =
      data.ec_comp && data.ec_comp !== 0 ? data.ec_comp.toFixed(3) + ' mS/cm' : 'Loading...';

    // ==== Sensor Status Badge ====
    const badge = document.getElementById('sensorReady');
    const logBox = document.getElementById('logBox');

    if (data.sensor_ready) {
      badge.textContent = 'Air sensor Online';
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
      badge.textContent = 'Air sensor Offline';
      badge.classList.remove('on');
      badge.classList.add('off');

      logBox.textContent =
        `Waiting for air sensor data...\n` +
        `Check Modbus wiring / slave ID / baudrate.\n` +
        `AP is still running normally at ${data.ip}`;
    }
  } catch (err) {
    document.getElementById('logBox').textContent =
      'Cannot fetch /api/status\n' + err;
  }
}

async function fetchKValue() {
  try {
    const res = await fetch('/api/kvalue');
    const data = await res.json();
    document.getElementById('kValueInput').value = data.k_value.toFixed(3);
  } catch(e) { console.error(e); }
}

document.getElementById('kValueBtn').addEventListener('click', async () => {
  const val = parseFloat(document.getElementById('kValueInput').value);
  if (isNaN(val)) return;

  try {
    await fetch('/api/kvalue', {
      method: 'POST',
      headers: {'Content-Type':'application/json'},  // quan trọng
      body: JSON.stringify({k_value: val})
    });
    fetchKValue(); // refresh input
    console.log("[KValue] Set to", val);
  } catch(e) {
    console.error(e);
  }
});

// Gọi lúc load
fetchKValue();

// ==== Format uptime ====
function formatUptime(sec) {
  const h = Math.floor(sec / 3600);
  const m = Math.floor((sec % 3600) / 60);
  const s = sec % 60;
  return `${h}h ${m}m ${s}s`;
}

// ==== Fetch interval ====
fetchStatus();
setInterval(fetchStatus, 2000);