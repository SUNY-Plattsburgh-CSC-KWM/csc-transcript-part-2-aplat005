// Andrew Platt
// SFML Version 2.0 (the one that the computers in Beaumont 302 use)

#include <SFML/Graphics.hpp>
#include "MyBanner.hpp"
#include <sstream>
#include <iostream>
#include <functional>
#include <vector>
#include <fstream>

using namespace std;

bool loadSystemFont(sf::Font& font) {
    vector<string> fontPaths = {
        "arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf"
    };
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) return true;
    }
    return false;
}

// Global status string
string globalStatus = "Welcome. Please set Student Name.";

int safeInt(const string& s, bool& success) {
    if (s.empty()) { success = false; return 0; }
    try {
        size_t pos;
        int val = stoi(s, &pos);
        if (pos != s.length()) { success = false; return 0; }
        success = true;
        return val;
    } catch (...) { success = false; return 0; }
}

double safeDouble(const string& s, bool& success) {
    if (s.empty()) { success = false; return 0.0; }
    try {
        double val = stod(s);
        success = true;
        return val;
    } catch (...) { success = false; return 0.0; }
}

// GUI components

struct Button {
    sf::RectangleShape rect;
    sf::Text label;
    function<void()> onClick;
    bool contains(const sf::Vector2f& p) const { return rect.getGlobalBounds().contains(p); }
};

struct InputField {
    sf::RectangleShape box;
    sf::Text text;
    string value;
    bool active = false;
    string placeholder;

    void setFont(const sf::Font& f) {
        text.setFont(f);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::Black);
    }

    void updateText() {
        if (value.empty() && !active) {
            text.setString(placeholder);
            text.setFillColor(sf::Color(120, 120, 120));
        } else {
            text.setString(value + (active ? "|" : ""));
            text.setFillColor(sf::Color::Black);
        }
    }
};

sf::Text makeText(const sf::Font& font, const string& str, int size) {
    sf::Text t;
    t.setFont(font);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(sf::Color::Black);
    return t;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1100, 800), "MyBanner GUI - Final");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!loadSystemFont(font)) {
        cout << "CRITICAL: No font found." << endl;
    }

    // Default name
    Student student("Unnamed Student");

    float leftX = 20.f;
    float startY = 20.f;
    float inputW = 350.f;
    float inputH = 30.f;
    float gap = 15.f;



    // Student name
    InputField inStuName;
    inStuName.box.setPosition(leftX, startY);
    inStuName.box.setSize({250.f, inputH});
    inStuName.box.setFillColor(sf::Color(230,230,250)); // Slightly blue to stand out
    inStuName.placeholder = "Enter Student Name";
    inStuName.setFont(font);

    // Semester
    float semY = startY + inputH + gap + 10.f;
    InputField inYear;
    inYear.box.setPosition(leftX, semY);
    inYear.box.setSize({160.f, inputH});
    inYear.box.setFillColor(sf::Color(230,230,230));
    inYear.placeholder = "Year (e.g. 2025)";
    inYear.setFont(font);

    InputField inExt;
    inExt.box.setPosition(leftX + 170.f, semY);
    inExt.box.setSize({180.f, inputH});
    inExt.box.setFillColor(sf::Color(230,230,230));
    inExt.placeholder = "Ext (10 Spr, 20 Sum)";
    inExt.setFont(font);

    // Course
    float cY = semY + inputH + gap + 20.f;
    
    InputField inCName;
    inCName.box.setPosition(leftX, cY);
    inCName.box.setSize({inputW, inputH});
    inCName.box.setFillColor(sf::Color(230,230,230));
    inCName.placeholder = "Course Name (e.g. Intro to Prog.)";
    inCName.setFont(font);

    InputField inCNum;
    inCNum.box.setPosition(leftX, cY + inputH + gap);
    inCNum.box.setSize({160.f, inputH});
    inCNum.box.setFillColor(sf::Color(230,230,230));
    inCNum.placeholder = "Number (CSC101)";
    inCNum.setFont(font);

    InputField inCredits;
    inCredits.box.setPosition(leftX + 170.f, cY + inputH + gap);
    inCredits.box.setSize({80.f, inputH});
    inCredits.box.setFillColor(sf::Color(230,230,230));
    inCredits.placeholder = "Credits";
    inCredits.setFont(font);

    InputField inGrade;
    inGrade.box.setPosition(leftX + 260.f, cY + inputH + gap);
    inGrade.box.setSize({90.f, inputH});
    inGrade.box.setFillColor(sf::Color(230,230,230));
    inGrade.placeholder = "Grade";
    inGrade.setFont(font);

    vector<InputField*> allFields = { &inStuName, &inYear, &inExt, &inCName, &inCNum, &inCredits, &inGrade };
    for(auto f : allFields) f->updateText();

    // Buttons
    vector<Button> buttons;
    float btnX = leftX + inputW + 30.f;
    float btnY = startY;

    auto addButton = [&](string text, function<void()> cb) {
        Button b;
        b.rect.setSize({200.f, 35.f});
        b.rect.setPosition(btnX, btnY);
        b.rect.setFillColor(sf::Color(200,200,200));
        b.rect.setOutlineColor(sf::Color::Black);
        b.rect.setOutlineThickness(1.f);
        
        b.label = makeText(font, text, 16);
        sf::FloatRect bounds = b.label.getLocalBounds();
        b.label.setPosition(btnX + (200.f - bounds.width)/2.f, btnY + 7.f);
        
        b.onClick = cb;
        buttons.push_back(b);
        btnY += 45.f;
    };

    addButton("Set Name", [&](){
        if (inStuName.value.empty()) {
            globalStatus = "Error: Name cannot be empty.";
        } else {
            student.setName(inStuName.value);
            globalStatus = "Student name updated.";
        }
    });

    addButton("Create Semester", [&](){
        bool okY, okE;
        int y = safeInt(inYear.value, okY);
        int e = safeInt(inExt.value, okE);
        
        if(!okY || !okE) { globalStatus = "Error: Invalid Year or Extension."; return; }

        student.createSemester(y, e);
        globalStatus = "Created Semester " + to_string(y) + "-" + to_string(e);
    });

    addButton("Add Course", [&](){
        bool okY, okE, okC, okG;
        int y = safeInt(inYear.value, okY);
        int e = safeInt(inExt.value, okE);
        int cr = safeInt(inCredits.value, okC);
        double g = safeDouble(inGrade.value, okG);

        if(!okY || !okE) { globalStatus = "Error: Invalid Year/Ext."; return; }
        if(!okC || !okG) { globalStatus = "Error: Invalid Credits/Grade."; return; }
        if(inCName.value.empty() || inCNum.value.empty()) { globalStatus = "Error: Missing Name/Num."; return; }

        Course c(inCName.value, inCNum.value, cr, g);
        student.addCourseToSemester(y, e, c);
        globalStatus = "Added course " + inCNum.value;
    });

    addButton("Delete Course", [&](){
        bool okY, okE;
        int y = safeInt(inYear.value, okY);
        int e = safeInt(inExt.value, okE);
        
        if(!okY || !okE) { globalStatus = "Error: Invalid Year/Ext."; return; }
        if(inCNum.value.empty()) { globalStatus = "Error: Enter Course Number."; return; }

        student.deleteCourseFromSemester(y, e, inCNum.value);
        globalStatus = "Deleted course " + inCNum.value;
    });

    addButton("Save to CSV", [&](){
        string filename = "transcript.csv";
        student.saveCSV(filename);
        globalStatus = "Saved courses to " + filename;
    });

    addButton("Load from CSV", [&](){
        string filename = "transcript.csv";
        
        ifstream f(filename);
        if(!f.good()) {
            globalStatus = "Error: " + filename + " not found.";
            return;
        }
        
        student.loadCSV(filename);
        globalStatus = "Loaded courses from " + filename;
    });

    // Display box
    sf::RectangleShape displayBox;
    displayBox.setPosition(20.f, 350.f);
    displayBox.setSize({1060.f, 430.f});
    displayBox.setFillColor(sf::Color(245,245,245));
    displayBox.setOutlineColor(sf::Color::Black);
    displayBox.setOutlineThickness(1.f);

    sf::Text statusText = makeText(font, "", 18);
    statusText.setFillColor(sf::Color(200, 0, 0));
    statusText.setPosition(20.f, 310.f);

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();

            if (ev.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mPos((float)ev.mouseButton.x, (float)ev.mouseButton.y);

                for (auto& b : buttons) if (b.contains(mPos)) b.onClick();

                for (auto f : allFields) {
                    if (f->box.getGlobalBounds().contains(mPos)) f->active = true;
                    else f->active = false;
                    f->updateText();
                }
            }

            if (ev.type == sf::Event::TextEntered) {
                uint32_t c = ev.text.unicode;
                for (auto f : allFields) {
                    if (f->active) {
                        if (c == 8) { if (!f->value.empty()) f->value.pop_back(); }
                        else if (c == 13) { f->active = false; }
                        else if (c >= 32 && c < 128) { f->value.push_back(static_cast<char>(c)); }
                        f->updateText();
                    }
                }
            }
        }

        // Render
        window.clear(sf::Color(220, 220, 220));

        statusText.setString("Status: " + globalStatus);
        
        for (auto f : allFields) {
            window.draw(f->box);
            f->text.setPosition(f->box.getPosition().x + 5.f, f->box.getPosition().y + 5.f);
            window.draw(f->text);
        }
        for (auto& b : buttons) {
            window.draw(b.rect);
            window.draw(b.label);
        }
        window.draw(statusText);
        window.draw(displayBox);

        // Dump student data
        {
            stringstream ss;
            streambuf* old = cout.rdbuf(ss.rdbuf());
            student.display();
            cout << "\n--------------------------------\n";
            student.summary();
            cout.rdbuf(old);

            string fullText = ss.str();
            sf::Text listText = makeText(font, fullText, 14);
            listText.setPosition(displayBox.getPosition().x + 10.f, displayBox.getPosition().y + 10.f);
            window.draw(listText);
        }

        window.display();
    }

    return 0;
}