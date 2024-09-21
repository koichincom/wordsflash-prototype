from flask import Flask, jsonify, request
import requests
from gtts import gTTS
import os

app = Flask(__name__)

FREE_DICTIONARY_API = "https://api.dictionaryapi.dev/api/v2/entries/en/"
PEXELS_API_KEY = 'your-pexels-api-key'
PEXELS_API = "https://api.pexels.com/v1/search"

def get_word_data(word):
    response = requests.get(f"{FREE_DICTIONARY_API}{word}")
    if response.status_code == 200:
        return response.json()[0]
    return None

def get_image(word):
    headers = {"Authorization": PEXELS_API_KEY}
    response = requests.get(f"{PEXELS_API}?query={word}", headers=headers)
    if response.status_code == 200 and len(response.json()['photos']) > 0:
        return response.json()['photos'][0]['src']['medium']
    return None

def generate_audio(word):
    tts = gTTS(text=word, lang='en')
    audio_file = f"{word}.mp3"
    tts.save(audio_file)
    return audio_file

@app.route('/api/word/<word>', methods=['GET'])
def word_lookup(word):
    word_data = get_word_data(word)
    if not word_data:
        return jsonify({"error": "Word not found"}), 404
    
    part_of_speech = word_data['meanings'][0]['partOfSpeech']
    meaning = word_data['meanings'][0]['definitions'][0]['definition']
    synonyms = ", ".join(word_data['meanings'][0]['definitions'][0].get('synonyms', []))
    image = get_image(word)
    audio = generate_audio(word)
    
    response = {
        "word": word,
        "partOfSpeech": part_of_speech,
        "meaning": meaning,
        "synonyms": synonyms,
        "image": image,
        "audioUrl": f"/audio/{audio}"
    }
    return jsonify(response)

@app.route('/audio/<filename>', methods=['GET'])
def serve_audio(filename):
    return app.send_static_file(filename)

if __name__ == '__main__':
    app.run(debug=True)
