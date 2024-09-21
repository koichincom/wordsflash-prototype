document.getElementById('submitBtn').addEventListener('click', function () {
    const word = document.getElementById('wordInput').value;

    fetch(`/api/word/${word}`)
        .then(response => response.json())
        .then(data => {
            document.getElementById('word').textContent = `Word: ${data.word}`;
            document.getElementById('partOfSpeech').textContent = `Part of Speech: ${data.partOfSpeech}`;
            document.getElementById('meaning').textContent = `Meaning: ${data.meaning}`;
            document.getElementById('synonyms').textContent = `Synonyms: ${data.synonyms}`;
            document.getElementById('image').src = data.image;

            document.getElementById('audioBtn').addEventListener('click', function () {
                const audio = new Audio(data.audioUrl);
                audio.play();
            });
        })
        .catch(error => {
            console.error('Error:', error);
        });
});
