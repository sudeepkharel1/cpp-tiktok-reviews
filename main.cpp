#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

const int MAX_REVIEWS = 200;
const string DATA_FILE = "tiktok_reviews.txt";

// Arrays to store reviews
string trendNames[MAX_REVIEWS];
int ratings[MAX_REVIEWS];
string reviewers[MAX_REVIEWS];
int reviewCount = 0;

// Convert string to lowercase (for case-insensitive search)
string toLower(const string &s) {
    string result = s;
    for (char &c : result) {
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

// Load reviews from file into arrays
void loadReviewsFromFile() {
    ifstream inFile(DATA_FILE);

    if (!inFile) {
        // File does not exist yet, this is OK
        cout << "No existing review file found. Starting fresh.\n";
        return;
    }

    string line;
    reviewCount = 0;

    while (getline(inFile, line) && reviewCount < MAX_REVIEWS) {
        if (line.empty()) {
            continue;
        }

        // Format: <trend name with spaces> <rating> <reviewer>
        // Strategy:
        //  - last word is reviewer
        //  - second to last is rating (integer)
        //  - everything before that is trend name
        size_t lastSpace = line.rfind(' ');
        if (lastSpace == string::npos) continue;

        string reviewer = line.substr(lastSpace + 1);
        string beforeReviewer = line.substr(0, lastSpace);

        size_t secondLastSpace = beforeReviewer.rfind(' ');
        if (secondLastSpace == string::npos) continue;

        string ratingStr = beforeReviewer.substr(secondLastSpace + 1);
        string trend = beforeReviewer.substr(0, secondLastSpace);

        try {
            int rating = stoi(ratingStr);

            trendNames[reviewCount] = trend;
            ratings[reviewCount] = rating;
            reviewers[reviewCount] = reviewer;
            reviewCount++;
        } catch (...) {
            // If rating is not a valid integer, skip this line
            continue;
        }
    }

    inFile.close();
    cout << reviewCount << " review(s) loaded from file.\n";
}

// Save a single new review to file (append mode)
void appendReviewToFile(const string &trend, int rating, const string &reviewer) {
    ofstream outFile(DATA_FILE, ios::app);
    if (!outFile) {
        cout << "Error: Could not open file for writing.\n";
        return;
    }

    outFile << trend << " " << rating << " " << reviewer << "\n";
    outFile.close();
}

// Display all reviews in a formatted table
void viewAllReviews() {
    if (reviewCount == 0) {
        cout << "No reviews available.\n";
        return;
    }

    cout << left << setw(25) << "TikTok Trend"
         << setw(10) << "Rating"
         << setw(15) << "Reviewer" << "\n";
    cout << "--------------------------------------------------\n";

    for (int i = 0; i < reviewCount; i++) {
        cout << left << setw(25) << trendNames[i]
             << setw(10) << ratings[i]
             << setw(15) << reviewers[i] << "\n";
    }
}

// Search for a trend/influencer by name (case-insensitive)
void searchTrend() {
    if (reviewCount == 0) {
        cout << "No reviews available to search.\n";
        return;
    }

    cout << "Enter trend/influencer name to search: ";
    string searchName;
    getline(cin, searchName);

    string searchLower = toLower(searchName);
    bool found = false;

    for (int i = 0; i < reviewCount; i++) {
        if (toLower(trendNames[i]) == searchLower) {
            if (!found) {
                cout << "Match(es) found:\n";
            }
            found = true;
            cout << "Trend: " << trendNames[i]
                 << ", Rating: " << ratings[i]
                 << ", Reviewed by: " << reviewers[i] << "\n";
        }
    }

    if (!found) {
        cout << "No review found for: " << searchName << "\n";
    }
}

// Add a new review (and append to file + arrays)
void addNewReview() {
    if (reviewCount >= MAX_REVIEWS) {
        cout << "Cannot add more reviews. Maximum reached.\n";
        return;
    }

    string trend;
    int rating;
    string reviewer;

    cout << "Enter trend/influencer name: ";
    getline(cin, trend);

    cout << "Enter rating (1-10): ";
    while (!(cin >> rating) || rating < 1 || rating > 10) {
        cout << "Invalid rating. Please enter a number between 1 and 10: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Clear newline left by cin >>
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter reviewer name (one word is best): ";
    getline(cin, reviewer);

    // Save in arrays
    trendNames[reviewCount] = trend;
    ratings[reviewCount] = rating;
    reviewers[reviewCount] = reviewer;
    reviewCount++;

    // Append to file for persistence
    appendReviewToFile(trend, rating, reviewer);

    cout << "New review added and saved.\n";
}

// Show main menu
void showMenu() {
    cout << "\nWelcome to the TikTok Review Database\n";
    cout << "1. View All Reviews\n";
    cout << "2. Search for a Trend/Influencer\n";
    cout << "3. Add a New Review\n";
    cout << "4. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    loadReviewsFromFile();

    int choice;

    while (true) {
        showMenu();

        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // Clear newline after menu choice
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                viewAllReviews();
                break;
            case 2:
                searchTrend();
                break;
            case 3:
                addNewReview();
                break;
            case 4:
                cout << "Exiting program. Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice. Please choose 1-4.\n";
                break;
        }
    }
    return 0;
}