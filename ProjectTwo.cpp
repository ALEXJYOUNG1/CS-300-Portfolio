//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Alex Young
// Description : CS 300 Project Two 
//============================================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

//============================================================================
// Data Structure
//============================================================================

struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

//============================================================================
// Helper Functions
//============================================================================

/**
 * Trim whitespace from both ends of a string.
 */
string trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }

    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }

    return s.substr(start, end - start);
}

/**
 * Convert a string to uppercase.
 */
string toUpperStr(string s) {
    for (size_t i = 0; i < s.size(); ++i) {
        s[i] = static_cast<char>(toupper(static_cast<unsigned char>(s[i])));
    }
    return s;
}

/**
 * Split a CSV line by comma.
 */
vector<string> splitCSVLine(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

/**
 * Find a course by course number.
 * Returns index if found, -1 if not found.
 */
int findCourseIndex(const vector<Course>& courses, const string& courseNumber) {
    string target = toUpperStr(trim(courseNumber));

    for (size_t i = 0; i < courses.size(); ++i) {
        if (toUpperStr(courses[i].courseNumber) == target) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

/**
 * Compare courses alphanumerically by course number.
 */
bool compareCourses(const Course& a, const Course& b) {
    return a.courseNumber < b.courseNumber;
}

/**
 * Check whether all prerequisites listed in all courses exist.
 * Prints warnings but still allows program to continue.
 */
void validatePrerequisites(const vector<Course>& courses) {
    for (size_t i = 0; i < courses.size(); ++i) {
        for (size_t j = 0; j < courses[i].prerequisites.size(); ++j) {
            const string& prereq = courses[i].prerequisites[j];
            if (findCourseIndex(courses, prereq) == -1) {
                cout << "Warning: Prerequisite " << prereq
                     << " for course " << courses[i].courseNumber
                     << " was not found in the data file." << endl;
            }
        }
    }
}

//============================================================================
// Core Program Functions
//============================================================================

/**
 * Load courses from a CSV file into a vector<Course>.
 * Expected format:
 * CourseNumber,CourseTitle,Prereq1,Prereq2,...
 */
bool loadCoursesFromFile(const string& fileName, vector<Course>& courses) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: Could not open file \"" << fileName << "\"." << endl;
        return false;
    }

    vector<Course> loadedCourses;
    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        ++lineNumber;

        line = trim(line);

        // Skip blank lines
        if (line.empty()) {
            continue;
        }

        vector<string> tokens = splitCSVLine(line);

        // Must have at least course number and title
        if (tokens.size() < 2) {
            cout << "Warning: Skipping invalid line " << lineNumber
                 << " (not enough fields)." << endl;
            continue;
        }

        Course course;
        course.courseNumber = toUpperStr(tokens[0]);
        course.courseTitle = tokens[1];

        // Remaining tokens are prerequisites
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                course.prerequisites.push_back(toUpperStr(tokens[i]));
            }
        }

        loadedCourses.push_back(course);
    }

    file.close();

    if (loadedCourses.empty()) {
        cout << "Error: No course data was loaded from the file." << endl;
        return false;
    }

    courses = loadedCourses;
    validatePrerequisites(courses);

    cout << "Data loaded successfully." << endl;
    cout << courses.size() << " courses loaded." << endl;

    return true;
}

/**
 * Print all courses in alphanumeric order by course number.
 */
void printCourseList(const vector<Course>& courses) {
    if (courses.empty()) {
        cout << "Please load the data file first." << endl;
        return;
    }

    vector<Course> sortedCourses = courses;
    sort(sortedCourses.begin(), sortedCourses.end(), compareCourses);

    cout << "Here is a sample schedule:" << endl;
    for (size_t i = 0; i < sortedCourses.size(); ++i) {
        cout << sortedCourses[i].courseNumber << ", " << sortedCourses[i].courseTitle << endl;
    }
}

/**
 * Print one course and its prerequisites.
 * Matches sample style.
 */
void printCourseInformation(const vector<Course>& courses, const string& inputCourseNumber) {
    if (courses.empty()) {
        cout << "Please load the data file first." << endl;
        return;
    }

    string courseNumber = toUpperStr(trim(inputCourseNumber));
    int index = findCourseIndex(courses, courseNumber);

    if (index == -1) {
        cout << "Course " << courseNumber << " not found." << endl;
        return;
    }

    const Course& course = courses[index];

    cout << course.courseNumber << ", " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    } else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            cout << course.prerequisites[i];
            if (i < course.prerequisites.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }

    /*
    if (!course.prerequisites.empty()) {
        cout << "Prerequisite course details:" << endl;
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            int pIndex = findCourseIndex(courses, course.prerequisites[i]);
            if (pIndex != -1) {
                cout << "  " << courses[pIndex].courseNumber << ", "
                     << courses[pIndex].courseTitle << endl;
            } else {
                cout << "  " << course.prerequisites[i] << ", [Title not found]" << endl;
            }
        }
    }
    */
}

/**
 * Display the program menu.
 */
void displayMenu() {
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
}

/**
 * Reads an integer menu choice from user input.
 */
bool readMenuChoice(int& choice) {
    string input;
    getline(cin, input);
    input = trim(input);

    if (input.empty()) {
        return false;
    }

    // Makes sure all characters are digits
    for (size_t i = 0; i < input.size(); ++i) {
        if (!isdigit(static_cast<unsigned char>(input[i]))) {
            return false;
        }
    }

    try {
        choice = stoi(input);
    } catch (...) {
        return false;
    }

    return true;
}

//============================================================================
// Main
//============================================================================

int main() {
    vector<Course> courses;
    bool dataLoaded = false;

    cout << "Welcome to the course planner." << endl;

    int choice = 0;

    while (choice != 9) {
        displayMenu();
        cout << "What would you like to do? ";

        int userChoice = 0;
        if (!readMenuChoice(userChoice)) {
            cout << "Invalid input. Please enter a number from the menu." << endl;
            continue;
        }

        choice = userChoice;

        switch (choice) {
            case 1: {
                cout << "What is the name of the file? ";
                string fileName;
                getline(cin, fileName);
                fileName = trim(fileName);

                if (fileName.empty()) {
                    cout << "File name cannot be empty." << endl;
                    break;
                }

                dataLoaded = loadCoursesFromFile(fileName, courses);
                break;
            }

            case 2: {
                if (!dataLoaded) {
                    cout << "Please load the data file first." << endl;
                    break;
                }
                printCourseList(courses);
                break;
            }

            case 3: {
                if (!dataLoaded) {
                    cout << "Please load the data file first." << endl;
                    break;
                }

                cout << "What course do you want to know about? ";
                string courseNumber;
                getline(cin, courseNumber);

                printCourseInformation(courses, courseNumber);
                break;
            }

            case 9:
                cout << "Thank you for using the course planner!" << endl;
                break;

            default:
                cout << choice << " is not a valid option." << endl;
                break;
        }
    }

    return 0;
}