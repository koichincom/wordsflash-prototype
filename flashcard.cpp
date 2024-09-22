#include "flashcard.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <curl/curl.h> // libcurl for HTTP requests
#include "json.hpp"    // nlohmann/json for JSON parsing

// Callback function to handle data received from the API
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    userp->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// Flashcard constructor
Flashcard::Flashcard(const std::string &word, const std::string &definition, const std::string &example, int score)
    : word(word), definition(definition), example(example), score(score) {}

// Getters
std::string Flashcard::getWord() const { return word; }
std::string Flashcard::getDefinition() const { return definition; }
std::string Flashcard::getExample() const { return example; }
int Flashcard::getScore() const { return score; }

// Setters
void Flashcard::setScore(int newScore) { score = newScore; }

// Show flashcard details
void Flashcard::showCard() const
{
    std::cout << "Word: " << word << "\nDefinition: " << definition << "\nExample: " << example << "\n";
}

// Speak the word and definition (macOS/Linux)
void Flashcard::speak() const
{
    std::string command = "say \"" + word + " " + definition + "\"";
    system(command.c_str());
}

// FlashcardManager constructor
FlashcardManager::FlashcardManager()
{
    loadFlashcardsFromFile();
}

// Fetch word definition from Free Dictionary API
std::string FlashcardManager::fetchDefinitionFromAPI(const std::string &word)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl)
    {
        std::string url = "https://api.dictionaryapi.dev/api/v2/entries/en/" + word;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "Failed to fetch definition: " << curl_easy_strerror(res) << std::endl;
            return "No definition found.";
        }

        // Parse the JSON response
        try
        {
            auto json = nlohmann::json::parse(readBuffer);
            return json[0]["meanings"][0]["definitions"][0]["definition"];
        }
        catch (...)
        {
            return "Error parsing definition.";
        }
    }
    return "No definition found.";
}

// Add a new flashcard with API definition
void FlashcardManager::addFlashcard(const std::string &word)
{
    std::string definition = fetchDefinitionFromAPI(word);
    std::string example = "No example available."; // Example could also be fetched if needed

    Flashcard newCard(word, definition, example);
    flashcards.push_back(newCard);
    saveFlashcardsToFile();
    std::cout << "Flashcard added with definition: " << definition << "\n";
}

// View all flashcards
void FlashcardManager::viewFlashcards() const
{
    if (flashcards.empty())
    {
        std::cout << "No flashcards available.\n";
        return;
    }
    for (const auto &card : flashcards)
    {
        card.showCard();
    }
}

// Quiz user with random flashcards
void FlashcardManager::quiz()
{
    if (flashcards.empty())
    {
        std::cout << "No flashcards available for quiz.\n";
        return;
    }

    srand(static_cast<unsigned int>(time(0)));
    int randomIndex = rand() % flashcards.size();
    Flashcard &selectedCard = flashcards[randomIndex];

    selectedCard.showCard();
    selectedCard.speak();

    int score;
    std::cout << "Rate your understanding of this word (1-5): ";
    std::cin >> score;
    selectedCard.setScore(score);

    saveFlashcardsToFile();
}

// Load flashcards from CSV file
void FlashcardManager::loadFlashcardsFromFile()
{
    std::ifstream inFile("flashcards.csv");
    if (!inFile)
    {
        std::cout << "No flashcards found, starting fresh.\n";
        return;
    }

    std::string line;
    while (std::getline(inFile, line))
    {
        std::stringstream ss(line);
        std::string word, definition, example;
        int score;

        std::getline(ss, word, ',');
        std::getline(ss, definition, ',');
        std::getline(ss, example, ',');
        ss >> score;

        flashcards.push_back(Flashcard(word, definition, example, score));
    }
    inFile.close();
}

// Save flashcards to CSV file
void FlashcardManager::saveFlashcardsToFile()
{
    std::ofstream outFile("flashcards.csv");
    for (const auto &card : flashcards)
    {
        outFile << "\"" << card.getWord() << "\",\""
                << card.getDefinition() << "\",\""
                << card.getExample() << "\","
                << card.getScore() << "\n";
    }
    outFile.close();
}

// Main menu for user interaction
void FlashcardManager::showMenu()
{
    int choice;
    do
    {
        std::cout << "\n1. Add Flashcard (with auto-definition)\n2. View Flashcards\n3. Quiz\n4. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore(); // Clear input buffer

        switch (choice)
        {
        case 1:
        {
            std::string word;
            std::cout << "Enter a word: ";
            std::getline(std::cin, word);
            addFlashcard(word); // Automatically fetch definition
            break;
        }
        case 2:
            viewFlashcards();
            break;
        case 3:
            quiz();
            break;
        case 4:
            std::cout << "Exiting...\n";
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);
}
