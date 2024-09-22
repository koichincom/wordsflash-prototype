#ifndef FLASHCARD_H
#define FLASHCARD_H

#include <string>
#include <vector>

class Flashcard
{
private:
    std::string word;
    std::string definition;
    std::string example;
    int score;

public:
    Flashcard(const std::string &word, const std::string &definition, const std::string &example, int score = 1);

    // Getters
    std::string getWord() const;
    std::string getDefinition() const;
    std::string getExample() const;
    int getScore() const;

    // Setters
    void setScore(int newScore);

    // Display flashcard
    void showCard() const;

    // Speak the word and definition (macOS/Linux only)
    void speak() const;
};

class FlashcardManager
{
private:
    std::vector<Flashcard> flashcards;

    // Load flashcards from CSV
    void loadFlashcardsFromFile();

    // Save flashcards to CSV
    void saveFlashcardsToFile();

    // Fetch word definition from Free Dictionary API
    std::string fetchDefinitionFromAPI(const std::string &word);

public:
    FlashcardManager();

    // Add flashcard with automatic definition
    void addFlashcard(const std::string &word);

    // View all flashcards
    void viewFlashcards() const;

    // Start a quiz
    void quiz();

    // Show main menu
    void showMenu();
};

#endif
