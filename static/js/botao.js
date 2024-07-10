const toggleBtn = document.getElementById('toggleBtn');

toggleBtn.addEventListener('click', function() {
    if (toggleBtn.textContent === 'Ligar') {
        toggleBtn.textContent = 'Desligar';
        toggleBtn.classList.remove('btn-primary');
        toggleBtn.classList.add('btn-danger'); // Altere para a classe de botão vermelho que preferir
    } else {
        toggleBtn.textContent = 'Ligar';
        toggleBtn.classList.remove('btn-danger');
        toggleBtn.classList.add('btn-primary');
    }
});
