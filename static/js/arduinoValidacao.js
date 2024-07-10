// Vetor com IDs fictícios
const arduinoIDs = ["12345", "67890", "24680", "13579", "98765"];

// Função para validar o ID do Arduino
function validarArduinoID() {
    // Obtém o valor digitado no campo de ID do Arduino
    const arduinoId = document.getElementById("arduinoId").value;

    // Verifica se o ID está presente no vetor de IDs
    if (arduinoIDs.includes(arduinoId)) {
        // Redireciona para outra página
        window.location.href = "./pages/compressor.html"; // Substitua com o caminho correto para a página desejada
    } else {
        // Permanece na mesma página
        alert("ID do Arduino inválido. Tente novamente.");
        // Limpa o campo de ID do Arduino
        document.getElementById("arduinoId").value = "";
    }

    // Evita o envio do formulário (comportamento padrão)
    return false;
}
