#include "MyBanner.hpp"
#include <sstream>
#include <iomanip>
#include <map>
using namespace std;

Course::Course() : courseName(""), courseNumber(""), credits(0), grade(0.0) {}
Course::Course(string name, string number, int credits, double grade)
    : courseName(name), courseNumber(number), credits(credits), grade(grade) {}

string Course::getCourseName() const {
    return courseName;
}
string Course::getCourseNumber() const {
    return courseNumber;
}
int Course::getCredits() const {
    return credits;
}
double Course::getGrade() const {
    return grade;
}

void Course::setGrade(double newGrade) {
    grade = newGrade;
}

string Course::toCSV() const {
    stringstream ss;
    ss << courseName << "," << courseNumber << "," << credits << "," << grade;
    return ss.str();
}

void Course::display() const {
    cout << courseNumber << " " << courseName << " Credits: " << credits << " Grade: " << grade << endl; 
}

Semester::Semester() : year(0), extension(0) {}
Semester::Semester(int y, int ext) : year(y), extension(ext) {}

void Semester::addCourse(const Course& course) {
    courses.push_back(course);
}

void Semester::deleteCourse(const string& courseNumber) {
    courses.erase(remove_if(courses.begin(), courses.end(),
        [&](const Course& c) {
            return c.getCourseNumber() == courseNumber;
        }),
        courses.end());
}

double Semester::calculateGPA() const {
    if (courses.empty()) {
        return 0.0;
    }
    double totalPoints = 0.0;
    int totalCredits = 0;
    for (const auto& c : courses) {
        totalPoints += c.getGrade() * c.getCredits();
        totalCredits += c.getCredits();
    }

    return totalCredits > 0 ? totalPoints / totalCredits : 0.0;
}

void Semester::display() const {
    cout << "Semester: " << year << extension << endl;
    cout << "Number | Course Name | Credits | Grade" << endl;
    cout << "----------------------------------------" << endl;

    for (const auto& c : courses) {
        c.display();
    }

    cout << "GPA: " << calculateGPA() << "\n\n";
}

string Semester::toCSV() const {
    stringstream ss;
    for (const auto& c : courses) {
        ss << year << "," << extension << "," << c.toCSV() << endl;
    }
    return ss.str();
}

int Semester::getYear() const {
    return year;
}
int Semester::getExtension() const {
    return extension;
}
vector<Course>& Semester::getCourses() {
    return courses;
}
const vector<Course>& Semester::getCourses() const {
    return courses;
}

Student::Student() : name("") {}
Student::Student(string n) : name(n) {}

void Student::createSemester(int year, int extension) {
    semesters.push_back(Semester(year, extension));
}

void Student::deleteSemester(int year, int extension) {
    semesters.erase(remove_if(semesters.begin(), semesters.end(),
        [&](const Semester& s) {
            return s.getYear() == year && s.getExtension() == extension;
        }),
        semesters.end());
}

void Student::addCourseToSemester(int year, int extension, const Course& course) {
    for (auto& s : semesters) {
        if (s.getYear() == year && s.getExtension() == extension) {
            s.addCourse(course);
            return;
        }
    }

    Semester newSem(year, extension);
    newSem.addCourse(course);
    semesters.push_back(newSem);
}

void Student::deleteCourseFromSemester(int year, int extension, const string& courseNumber) {
    for (auto& s : semesters) {
        if (s.getYear() == year && s.getExtension() == extension) {
            s.deleteCourse(courseNumber);
            return;
        }
    }
}

void Student::display() const {
    cout << "Student: " << name << endl;
    for (const auto& s : semesters) {
        s.display();
    }
}

void Student::summary() const {
    cout << "Summary:" << endl;
    cout << "Student: " << name << endl;
    double totalCredits = 0.0;
    double totalPoints = 0.0;

    for (const auto& s : semesters) {
        double semGPA = s.calculateGPA();
        int semCredits = 0;
        for (const auto& c : s.getCourses()) {
            semCredits += c.getCredits();
        }

        cout << "Semester: " << s.getYear() << s.getExtension() << endl;
        cout << "Credits: " << semCredits << endl;
        cout << "GPA: " << fixed << semGPA << endl;

        totalCredits += semCredits;
        totalPoints += semGPA * semCredits;
    }

    if (totalCredits > 0)
        cout << "Cumulative GPA: " << fixed << (totalPoints / totalCredits) << endl;
    else
        cout << "No credits yet." << endl;
}

void Student::sortGrades(int year, int extension) {
    for (auto& s : semesters) {
        if (s.getYear() == year && s.getExtension() == extension) {
            sort(s.getCourses().begin(), s.getCourses().end(),
                [](const Course& a, const Course& b) {
                    return a.getGrade() > b.getGrade();
                });
        }
    }
}

void Student::sortCourseNumber(int year, int extension) {
    for (auto& s : semesters) {
        if (s.getYear() == year && s.getExtension() == extension) {
            sort(s.getCourses().begin(), s.getCourses().end(),
                [](const Course& a, const Course& b) {
                    return a.getCourseNumber() < b.getCourseNumber();
                });
        }
    }
}

void Student::saveCSV(const string& filename) const {
    ofstream file(filename);
    if (!file) {
        cerr << "Error: could not open " << filename << " for writing." << endl;
        return;
    }

    for (const auto& s : semesters) {
        file << s.toCSV();
    }
    file.close();
}

void Student::loadCSV(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: could not open " << filename << " for reading." << endl;
        return;
    }

    semesters.clear();
    string line;
    map<pair<int,int>, Semester> semesterMap;

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        vector<string> fields;

        while (getline(ss, token, ',')) {
             fields.push_back(token);
        }

        if (fields.size() < 6) {
            continue;
        }

        int year = stoi(fields[0]);
        int extension = stoi(fields[1]);
        string name = fields[2];
        string number = fields[3];
        int credits = stoi(fields[4]);
        double grade = stod(fields[5]);

        Course c(name, number, credits, grade);
        auto key = make_pair(year, extension);

        if (semesterMap.find(key) == semesterMap.end()) {
            semesterMap[key] = Semester(year, extension);
        }
        semesterMap[key].addCourse(c);
    }

    for (auto& kv : semesterMap) {
        semesters.push_back(kv.second);
    }

    file.close();
}

double Student::calculateCumulativeGPA() const {
    double totalPoints = 0.0;
    int totalCredits = 0;

    for (const auto& s : semesters) {
        for (const auto& c : s.getCourses()) {
            totalPoints += c.getGrade() * c.getCredits();
            totalCredits += c.getCredits();
        }
    }
    return totalCredits > 0 ? totalPoints / totalCredits : 0.0;
}

void Student::setName(string n) {
    name = n;
}