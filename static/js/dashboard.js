/**
 * VigilSense Dashboard - Real-time Data Updates (Black Theme)
 */

// Chart and data storage
let sensorChart = null;
const maxDataPoints = 20; // Keep last 20 data points
const chartData = {
    labels: [],
    temperature: [],
    gas: [],
    sound: [],
    vibration: []
};

// Initialize Chart.js
function initChart() {
    const canvas = document.getElementById('sensorChart');
    if (!canvas) return;
    
    const ctx = canvas.getContext('2d');
    
    sensorChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: chartData.labels,
            datasets: [
                {
                    label: 'Temperature (°C)',
                    data: chartData.temperature,
                    borderColor: '#ef4444',
                    backgroundColor: 'rgba(239, 68, 68, 0.1)',
                    tension: 0.4,
                    borderWidth: 2,
                    pointRadius: 0,
                    pointHoverRadius: 4
                },
                {
                    label: 'Gas (ppm)',
                    data: chartData.gas,
                    borderColor: '#f59e0b',
                    backgroundColor: 'rgba(245, 158, 11, 0.1)',
                    tension: 0.4,
                    borderWidth: 2,
                    pointRadius: 0,
                    pointHoverRadius: 4,
                    yAxisID: 'y1'
                },
                {
                    label: 'Sound (dB)',
                    data: chartData.sound,
                    borderColor: '#8b5cf6',
                    backgroundColor: 'rgba(139, 92, 246, 0.1)',
                    tension: 0.4,
                    borderWidth: 2,
                    pointRadius: 0,
                    pointHoverRadius: 4
                },
                {
                    label: 'Vibration',
                    data: chartData.vibration,
                    borderColor: '#06b6d4',
                    backgroundColor: 'rgba(6, 182, 212, 0.1)',
                    tension: 0.4,
                    borderWidth: 2,
                    pointRadius: 0,
                    pointHoverRadius: 4
                }
            ]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            interaction: {
                mode: 'index',
                intersect: false,
            },
            plugins: {
                legend: {
                    display: true,
                    position: 'bottom',
                    labels: {
                        color: '#9ca3af',
                        font: {
                            size: 10,
                            family: 'Inter'
                        },
                        usePointStyle: true,
                        padding: 8
                    }
                },
                tooltip: {
                    backgroundColor: 'rgba(0, 0, 0, 0.8)',
                    titleColor: '#ffffff',
                    bodyColor: '#ffffff',
                    borderColor: '#404040',
                    borderWidth: 1,
                    padding: 8,
                    displayColors: true
                }
            },
            scales: {
                x: {
                    grid: {
                        color: 'rgba(64, 64, 64, 0.3)',
                        display: false
                    },
                    ticks: {
                        color: '#9ca3af',
                        font: {
                            size: 9,
                            family: 'Inter'
                        },
                        maxRotation: 0,
                        maxTicksLimit: 6
                    },
                    border: {
                        color: '#404040'
                    }
                },
                y: {
                    type: 'linear',
                    display: true,
                    position: 'left',
                    grid: {
                        color: 'rgba(64, 64, 64, 0.3)'
                    },
                    ticks: {
                        color: '#9ca3af',
                        font: {
                            size: 9,
                            family: 'Inter'
                        }
                    },
                    border: {
                        color: '#404040'
                    }
                },
                y1: {
                    type: 'linear',
                    display: true,
                    position: 'right',
                    grid: {
                        drawOnChartArea: false
                    },
                    ticks: {
                        color: '#9ca3af',
                        font: {
                            size: 9,
                            family: 'Inter'
                        }
                    },
                    border: {
                        color: '#404040'
                    }
                }
            },
            elements: {
                point: {
                    hoverRadius: 5
                }
            }
        }
    });
}

// Update chart with new data
function updateChart(data) {
    const now = new Date();
    const timeLabel = `${now.getHours().toString().padStart(2, '0')}:${now.getMinutes().toString().padStart(2, '0')}:${now.getSeconds().toString().padStart(2, '0')}`;
    
    // Add new data point
    chartData.labels.push(timeLabel);
    chartData.temperature.push(data.temperature);
    chartData.gas.push(data.gas);
    chartData.sound.push(data.sound);
    chartData.vibration.push(data.vibration);
    
    // Keep only last N data points
    if (chartData.labels.length > maxDataPoints) {
        chartData.labels.shift();
        chartData.temperature.shift();
        chartData.gas.shift();
        chartData.sound.shift();
        chartData.vibration.shift();
    }
    
    // Update chart
    if (sensorChart) {
        sensorChart.update('none'); // 'none' for smooth animation
    }
}

// Fullscreen functionality (make it global)
window.toggleFullscreen = function() {
    if (!document.fullscreenElement && !document.webkitFullscreenElement && !document.msFullscreenElement) {
        // Enter fullscreen
        if (document.documentElement.requestFullscreen) {
            document.documentElement.requestFullscreen();
        } else if (document.documentElement.webkitRequestFullscreen) { // Safari
            document.documentElement.webkitRequestFullscreen();
        } else if (document.documentElement.msRequestFullscreen) { // IE/Edge
            document.documentElement.msRequestFullscreen();
        }
    } else {
        // Exit fullscreen
        if (document.exitFullscreen) {
            document.exitFullscreen();
        } else if (document.webkitExitFullscreen) { // Safari
            document.webkitExitFullscreen();
        } else if (document.msExitFullscreen) { // IE/Edge
            document.msExitFullscreen();
        }
    }
};

// Update fullscreen button icon
function updateFullscreenIcon() {
    const icon = document.getElementById('fullscreenIcon');
    const text = document.getElementById('fullscreenText');
    
    if (document.fullscreenElement || document.webkitFullscreenElement || document.msFullscreenElement) {
        // Exit fullscreen icon
        icon.innerHTML = '<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 18L18 6M6 6l12 12"></path>';
        text.textContent = 'EXIT';
    } else {
        // Enter fullscreen icon
        icon.innerHTML = '<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 8V4m0 0h4M4 4l5 5m11-1V4m0 0h-4m4 0l-5 5M4 16v4m0 0h4m-4 0l5-5m11 5l-5-5m5 5v-4m0 4h-4"></path>';
        text.textContent = 'FULLSCREEN';
    }
}

// Handle fullscreen changes
function handleFullscreenChange() {
    updateFullscreenIcon();
    // Resize chart when entering/exiting fullscreen
    if (sensorChart) {
        setTimeout(() => {
            sensorChart.resize();
        }, 100);
    }
}

// Listen for fullscreen changes
document.addEventListener('fullscreenchange', handleFullscreenChange);
document.addEventListener('webkitfullscreenchange', handleFullscreenChange);
document.addEventListener('msfullscreenchange', handleFullscreenChange);

// Update sensor data every 2 seconds
function updateSensorData() {
    fetch('/api/sensors')
        .then(response => response.json())
        .then(data => {
            // Update temperature
            document.getElementById('temperature').textContent = data.temperature.toFixed(1);
            updateCardStatus('tempCard', data.temperature > 50);

            // Update humidity
            document.getElementById('humidity').textContent = data.humidity.toFixed(1);
            updateCardStatus('humidityCard', false); // Humidity is always normal

            // Update gas
            document.getElementById('gas').textContent = data.gas.toFixed(1);
            updateCardStatus('gasCard', data.gas > 600);

            // Update flame
            document.getElementById('flame').textContent = data.flame === 1 ? 'DETECTED' : 'NONE';
            document.getElementById('flameStatus').textContent = data.flame === 1 ? 'Alert!' : 'Normal';
            updateCardStatus('flameCard', data.flame === 1);

            // Update sound
            document.getElementById('sound').textContent = data.sound.toFixed(1);
            updateCardStatus('soundCard', data.sound > 300);

            // Update vibration
            document.getElementById('vibration').textContent = data.vibration.toFixed(1);
            updateCardStatus('vibrationCard', data.vibration > 100);
            
            // Update chart
            updateChart(data);
        })
        .catch(error => {
            console.error('Error fetching sensor data:', error);
        });
}

// Update system status
function updateSystemStatus() {
    fetch('/api/status')
        .then(response => response.json())
        .then(data => {
            const banner = document.getElementById('statusBanner');
            const statusText = document.getElementById('statusText');
            
            statusText.textContent = data.message;
            
            if (data.status === 'hazard') {
                banner.className = 'px-4 py-2 border border-red-500 rounded';
                statusText.className = 'text-sm font-medium text-red-400';
            } else {
                banner.className = 'px-4 py-2 border border-gray-600 rounded';
                statusText.className = 'text-sm font-medium text-white';
            }
        })
        .catch(error => {
            console.error('Error fetching system status:', error);
        });
}

// Update card status (color coding)
function updateCardStatus(cardId, isAlert) {
    const card = document.getElementById(cardId);
    if (isAlert) {
        card.classList.add('alert');
    } else {
        card.classList.remove('alert');
    }
}

// Initialize updates on page load
document.addEventListener('DOMContentLoaded', function() {
    // Initialize chart
    initChart();
    
    // Initial data load
    updateSensorData();
    updateSystemStatus();

    // Set up intervals
    setInterval(updateSensorData, 2000);      // Update sensors every 2 seconds
    setInterval(updateSystemStatus, 2000);    // Update status every 2 seconds

    // Setup Telegram toggle
    initTelegramToggle();

    console.log('VigilSense Dashboard initialized');
});

// Initialize Telegram alerts toggle
function initTelegramToggle() {
    const btn = document.getElementById('telegramToggle');
    const stateEl = document.getElementById('telegramState');
    if (!btn || !stateEl) return;

    // Load initial state
    fetch('/api/telegram/status')
        .then(r => r.json())
        .then(s => {
            renderTelegramState(s.enabled);
        })
        .catch(() => renderTelegramState(false));

    // Click to toggle
    btn.addEventListener('click', () => {
        const isOn = stateEl.textContent === 'ON';
        const next = !isOn;
        fetch('/api/telegram/toggle', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ enabled: next })
        })
            .then(r => r.json())
            .then(res => {
                if (res && res.status === 'success') {
                    renderTelegramState(res.enabled);
                }
            })
            .catch(() => {/* noop */});
    });
}

function renderTelegramState(enabled) {
    const stateEl = document.getElementById('telegramState');
    const btn = document.getElementById('telegramToggle');
    if (!stateEl || !btn) return;
    stateEl.textContent = enabled ? 'ON' : 'OFF';
    btn.className = enabled
        ? 'px-4 py-2 border border-green-600 rounded text-sm font-medium text-white hover:bg-gray-900 transition-colors'
        : 'px-4 py-2 border border-gray-600 rounded text-sm font-medium text-white hover:bg-gray-900 transition-colors';
}
