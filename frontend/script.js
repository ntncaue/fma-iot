const BROKER_URL = 'wss://broker.hivemq.com:8884/mqtt';
const TELEMETRY_TOPIC = 'fma/patio/motos';
const dashboard = document.getElementById('dashboard');
const errorMessage = document.getElementById('error-message');
const mapContainer = document.getElementById('map-container');
const telemetryData = new Map();
let mqttClient;

const GRID_COLS = 10;
const GRID_ROWS = 5;
const parkingSpots = [];

const parkingLotBounds = {
    minLat: -23.5510,
    maxLat: -23.5495,
    minLng: -46.6345,
    maxLng: -46.6325,
};

function createParkingLot() {
    for (let i = 0; i < GRID_ROWS * GRID_COLS; i++) {
        const spot = document.createElement('div');
        spot.className = 'parking-spot';
        spot.dataset.spotIndex = i;
        mapContainer.appendChild(spot);
        parkingSpots.push(spot);
    }
}

function connectMqtt() {
    mqttClient = mqtt.connect(BROKER_URL);

    mqttClient.on('connect', () => {
        console.log('Conectado ao Broker MQTT!');
        errorMessage.style.display = 'none';
        mqttClient.subscribe(TELEMETRY_TOPIC, (err) => {
            if (err) {
                console.error('Falha ao se inscrever no tópico:', err);
            }
        });
    });

    mqttClient.on('message', (topic, message) => {
        try {
            const moto = JSON.parse(message.toString());
            moto.timestamp = new Date().toISOString(); // Adiciona timestamp
            telemetryData.set(moto.motorcycle_id, moto);
            renderAll();
        } catch (e) {
            console.error('Erro ao processar mensagem MQTT:', e);
        }
    });

    mqttClient.on('error', (err) => {
        console.error('Erro no MQTT:', err);
        errorMessage.textContent = 'Não foi possível conectar ao broker MQTT.';
        errorMessage.style.display = 'block';
        mqttClient.end();
    });

    mqttClient.on('close', () => {
        console.log('Conexão MQTT fechada. Tentando reconectar...');
        setTimeout(connectMqtt, 5000);
    });
}

function renderAll() {
    const data = Array.from(telemetryData.values());
    updateDashboard(data);
    updateIcons(data);
}

function updateDashboard(data) {
    dashboard.innerHTML = '<h2>Motos</h2>';
    if (data.length === 0) {
        dashboard.innerHTML += '<p>Aguardando dados de telemetria...</p>';
        return;
    }

    data.sort((a, b) => new Date(b.timestamp) - new Date(a.timestamp));

    data.forEach(moto => {
        const card = document.createElement('div');
        card.className = 'moto-card';
        card.id = `card-${moto.motorcycle_id}`;

        const statusClass = `status-${moto.status.toLowerCase().replace(' ', '_')}`;

        card.innerHTML = `
            <h3>${moto.motorcycle_id}</h3>
            <p><span class="label">Status:</span> <span class="${statusClass}">${moto.status}</span></p>
            <p><span class="label">Última Atualização:</span> ${new Date(moto.timestamp).toLocaleString('pt-BR')}</p>
            <div class="command-section">
                <select onchange="sendCommand('${moto.motorcycle_id}', this.value)">
                    <option value="">Mudar Status</option>
                    <option value="estacionada">Estacionada</option>
                    <option value="em_movimento">Em Movimento</option>
                    <option value="manutencao">Manutenção</option>
                </select>
                <button onclick="findMoto('${moto.motorcycle_id}')">Encontrar</button>
            </div>
        `;
        dashboard.appendChild(card);
    });
}

function sendCommand(motorcycle_id, status) {
    if (!status || !mqttClient) return;
    const topic = `fma/patio/motos/${motorcycle_id}/cmd`;
    const payload = JSON.stringify({ command: 'set_status', status: status });
    mqttClient.publish(topic, payload, (err) => {
        if (err) {
            console.error('Falha ao enviar comando:', err);
        } else {
            console.log(`Comando para setar status para '${status}' enviado para ${motorcycle_id}`);
        }
    });
}

function findMoto(motorcycle_id) {
    // Efeito visual no frontend
    const moto = telemetryData.get(motorcycle_id);
    if (moto) {
        const spotIndex = mapGpsToGrid(moto.lat, moto.lng);
        const spot = parkingSpots[spotIndex];
        if (spot && spot.firstChild) {
            const icon = spot.firstChild;
            icon.classList.add('highlight');
            setTimeout(() => {
                icon.classList.remove('highlight');
            }, 2000);
        }
    }

    // Envia comando para o dispositivo
    if (!mqttClient) return;
    const topic = `fma/patio/motos/${motorcycle_id}/cmd`;
    const payload = JSON.stringify({ command: 'find_moto' });
    mqttClient.publish(topic, payload, (err) => {
        if (err) {
            console.error('Falha ao enviar comando "find_moto":', err);
        } else {
            console.log(`Comando "find_moto" enviado para ${motorcycle_id}`);
        }
    });
}

function updateIcons(data) {
    parkingSpots.forEach(spot => spot.innerHTML = '');

    data.forEach(moto => {
        const { motorcycle_id, lat, lng, status } = moto;
        const spotIndex = mapGpsToGrid(lat, lng);

        if (spotIndex >= 0 && spotIndex < parkingSpots.length) {
            const icon = document.createElement('div');
            icon.className = 'moto-icon';
            icon.innerHTML = getIconContent(status);
            icon.title = `${motorcycle_id} - ${status}`;
            parkingSpots[spotIndex].appendChild(icon);
        }
    });
}

function mapGpsToGrid(lat, lng) {
    const { minLat, maxLat, minLng, maxLng } = parkingLotBounds;
    const col = Math.floor(((lng - minLng) / (maxLng - minLng)) * GRID_COLS);
    const row = Math.floor(((maxLat - lat) / (maxLat - minLat)) * GRID_ROWS);
    return row * GRID_COLS + col;
}

function getIconContent(status) {
    const colors = {
        estacionada: 'green',
        em_movimento: 'orange',
        manutencao: 'red'
    };
    const color = colors[status] || 'blue';
    return `<span style="color: ${color}; font-size: 2em;">🏍️</span>`;
}

document.addEventListener('DOMContentLoaded', () => {
    createParkingLot();
    connectMqtt();
});
