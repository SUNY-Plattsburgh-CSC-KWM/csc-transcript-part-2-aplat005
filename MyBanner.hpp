#ifndef MYBANNER_HPP
#define MYBANNER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

class Course {
    private:
        string courseName;
        string courseNumber;
        int credits;
        double grade;

    public:
        Course();
        Course(string name, string number, int credits, double grade);

        string getCourseName() const;
        string getCourseNumber() const;
        int getCredits() const;
        double getGrade() const;

        void setGrade(double grade);

        string toCSV() const;
        void display() const;
    };

class Semester {
    private:
        vector<Course> courses;
        int year;
        int extension;

    public:
        Semester();
        Semester(int year, int extension);

        void addCourse(const Course& course);
        void deleteCourse(const string& courseNumber);

        double calculateGPA() const;

        void display() const;
        string toCSV() const;

        int getYear() const;
        int getExtension() const;
        vector<Course>& getCourses();
        const vector<Course>& getCourses() const;
    };

class Student {
    private:
        string name;
        vector<Semester> semesters;

    public:
        Student();
        Student(string name);

        void createSemester(int year, int extension);
        void deleteSemester(int year, int extension);

        void addCourseToSemester(int year, int extension, const Course& course);
        void deleteCourseFromSemester(int year, int extension, const string& courseNumber);

        void display() const;
        void summary() const;

        void sortGrades(int year, int extension);
        void sortCourseNumber(int year, int extension);

        void saveCSV(const string& filename) const;
        void loadCSV(const string& filename);

        double calculateCumulativeGPA() const;

        void setName(string name);
    };

#endif // MYBANNER_HPP
