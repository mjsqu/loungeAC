const ctx = document.getElementById('chart').getContext('2d');
const hoursSelect = document.getElementById('hours');
const chart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Temperature',
            data: [],
            fill: false,
            borderColor: 'rgb(75, 192, 192)',
            tension: 0.1
        }]
    },
    options: {
        scales: {
            x: {
                title: {
                    display: true,
                    text: 'Time (UTC)'
                }
            },
            y: {
                title: {
                    display: true,
                    text: 'Temperature'
                }
            }
        }
    }
});

async function fetchData() {
    const hours = hoursSelect.value;
    const response = await fetch(`/data?hours=${hours}`);
    const data = await response.json();
    chart.data.labels = data.times.map(time => new Date(time).toLocaleString());
    chart.data.datasets[0].data = data.temperatures;
    chart.update();
}

fetchData();
setInterval(fetchData, 60000);

document.onclick = function(e){
    const tgt = e.target;
    fetch("/heatpump", {method: 'POST', body: tgt.innerHTML})
    //.then(response => response.json())
}