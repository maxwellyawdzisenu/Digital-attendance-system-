#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

// Student Class
class Student {
public:
    string name;
    string index;
    
    Student(string n = "", string idx = "") {
        name = n;
        index = idx;
    }
    
    void display() {
        cout << index << " - " << name;
    }
    
    string toFileString() {
        return index + "," + name;
    }
};

// Attendance Session Class
class AttendanceSession {
public:
    string courseCode;
    string date;
    string time;
    int duration;
    vector<string> present;
    vector<string> late;
    vector<string> absent;
    
    AttendanceSession(string code = "", string d = "", string t = "", int h = 1) {
        courseCode = code;
        date = d;
        time = t;
        duration = h;
    }
    
    void markPresent(string idx) { present.push_back(idx); }
    void markLate(string idx) { late.push_back(idx); }
    void markAbsent(string idx) { absent.push_back(idx); }
    
    bool isMarked(string idx) {
        for (string i : present) if (i == idx) return true;
        for (string i : late) if (i == idx) return true;
        for (string i : absent) if (i == idx) return true;
        return false;
    }
    
    void display() {
        cout << "\n=== SESSION ===\n";
        cout << courseCode << " | " << date << " | " << time << " (" << duration << "h)\n";
        cout << "Present: " << present.size() << " | Late: " << late.size() << " | Absent: " << absent.size() << "\n";
    }
    
    void report() {
        cout << "\n=== ATTENDANCE REPORT ===\n";
        cout << courseCode << " - " << date << "\n";
        cout << "PRESENT:";
        for (string i : present) cout << " " << i;
        cout << "\nLATE:";
        for (string i : late) cout << " " << i;
        cout << "\nABSENT:";
        for (string i : absent) cout << " " << i;
        cout << "\n";
    }
    
    void saveToFile() {
        ofstream f("session_" + courseCode + "_" + date + ".txt");
        f << "COURSE: " << courseCode << "\nDATE: " << date << "\nTIME: " << time << "\n";
        f << "PRESENT:\n"; for (string i : present) f << i << "\n";
        f << "LATE:\n"; for (string i : late) f << i << "\n";
        f << "ABSENT:\n"; for (string i : absent) f << i << "\n";
        f.close();
    }
};

// Global Data
vector<Student> students;
vector<AttendanceSession> sessions;

// File Operations
void loadStudents() {
    ifstream f("students.txt");
    string line;
    students.clear();
    while (getline(f, line)) {
        size_t c = line.find(",");
        if (c != string::npos)
            students.push_back(Student(line.substr(c+1), line.substr(0,c)));
    }
    f.close();
}

void saveStudents() {
    ofstream f("students.txt");
    for (Student s : students) f << s.index << "," << s.name << "\n";
    f.close();
}

// Improved Loading with Error Handling
void loadSessions() {
    ifstream f("sessions.txt");
    if (!f.is_open()) {
        // Try to load from backup if main file missing
        ifstream backup("sessions_backup.txt");
        if (backup.is_open()) {
            cout << "Main file not found. Loading from backup...\n";
            f = move(backup);
        } else {
            cout << "No existing sessions. Starting fresh.\n";
            return;
        }
    }
    
    string line;
    sessions.clear();
    while (getline(f, line)) {
        if (line.empty()) continue;
        
        size_t p1 = line.find(",");
        size_t p2 = line.find(",", p1+1);
        size_t p3 = line.find(",", p2+1);
        
        if (p1 == string::npos || p2 == string::npos || p3 == string::npos) {
            cout << "Skipping invalid line: " << line << "\n";
            continue;
        }
        
        string code = line.substr(0, p1);
        string date = line.substr(p1+1, p2-p1-1);
        string time = line.substr(p2+1, p3-p2-1);
        
        try {
            int dur = stoi(line.substr(p3+1));
            sessions.push_back(AttendanceSession(code, date, time, dur));
        } catch (...) {
            cout << "Invalid duration in line: " << line << "\n";
        }
    }
    f.close();
    cout << "Loaded " << sessions.size() << " sessions.\n";
}

// Improved File Operations with Backup
void saveSessions() {
    // First, create a backup of existing file
    ifstream check("sessions.txt");
    if (check.good()) {
        check.close();
        rename("sessions.txt", "sessions_backup.txt");
    }
    
    // Save new sessions
    ofstream f("sessions.txt");
    if (!f.is_open()) {
        cout << "Error: Cannot save sessions!\n";
        return;
    }
    
    for (AttendanceSession s : sessions) {
        f << s.courseCode << "," << s.date << "," << s.time << "," << s.duration << "\n";
        s.saveToFile(); // Save individual session file
    }
    f.close();
    cout << "Sessions saved successfully. Backup created.\n";
}
// ============================================
// HELPER FUNCTIONS - IMPROVED READABILITY
// ============================================

// Display a formatted header
void showHeader(string title) {
    cout << "\n========================================\n";
    cout << "   " << title << "\n";
    cout << "========================================\n";
}

// Show success message with checkmark
void showSuccess(string msg) {
    cout << "Successful " << msg << "\n";
}

// Show error message with X mark
void showError(string msg) {
    cout << "An Error " << msg << "\n";
}

// Show warning message
void showWarning(string msg) {
    cout << "Warning " << msg << "\n";
}

// Pause and wait for user to press Enter
void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

// Ask user for confirmation
bool confirm(string msg) {
    char ch;
    cout << msg << " (y/n): ";
    cin >> ch;
    return (ch == 'y' || ch == 'Y');
}
// Validation for 8 digits + 1 letter (e.g., 01240154D)
bool validIndex(string i) { 
    if (i.length() != 9) return false; // 8 digits + 1 letter = 9 total
    
    // Check first 8 characters are digits
    for (int pos = 0; pos < 8; pos++) {
        if (!isdigit(i[pos])) return false;
    }
    
    // Check last character is a letter
    if (!isalpha(i[8])) return false;
    
    return true;
}

bool studentExists(string i) { 
    for (Student s : students) if (s.index == i) return true; 
    return false; 
}

// Student Functions with better validation
void registerStudent() {
    string n, i;
    cout << "Invalid index! Must be 8 digits followed by a letter (e.g., 01240154D).\n";
    
    // Validate index format
    if (!validIndex(i)) { 
        cout << "Invalid index! Must be 3 digits (001-999).\n"; 
        return; 
    }
    
    // Check if already exists
    if (studentExists(i)) { 
        cout << "Student with index " << i << " already exists!\n"; 
        return; 
    }
    
    cout << "Name: "; 
    cin.ignore(); 
    getline(cin, n);
    
    // Validate name not empty
    if (n.length() < 2) {
        cout << "Name too short!\n";
        return;
    }
    
    students.push_back(Student(n, i));
    saveStudents();
    cout << "Student registered successfully!\n";
}
void viewStudents() {
    showHeader("STUDENT LIST");
    
    if (students.empty()) {
        showWarning("No students registered.");
        pause();
        return;
    }
    
    for (int i=0; i<students.size(); i++) {
        cout << i+1 << ". ";
        students[i].display();
        cout << "\n";
    }
    
    showSuccess("Total: " + to_string(students.size()) + " student(s)");
    pause();
}

void searchStudent() {
    string i; cout << "Index: "; cin >> i;
    for (Student s : students) if (s.index == i) { 
        cout << "Found: "; s.display(); cout << "\n"; return; 
    }
    cout << "Not found\n";
}

// Session Functions
void createSession() {
    string c, d, t; int h;
    cout << "Course: "; cin >> c;
    cout << "Date (YYYY-MM-DD): "; cin >> d;
    cout << "Time (HH:MM): "; cin >> t;
    cout << "Hours (1-4): "; cin >> h;
    if (h<1 || h>4) { cout << "Invalid hours\n"; return; }
    sessions.push_back(AttendanceSession(c, d, t, h));
    saveSessions();
    cout << "Session created!\n";
}

void viewSessions() {
    if (sessions.empty()) cout << "\nNo sessions\n";
    else for (int i=0; i<sessions.size(); i++) {
        cout << "\n" << i+1 << "."; sessions[i].display();
    }
}

// Attendance Marking
void markAttendance() {
    if (sessions.empty()) { cout << "\nNo sessions\n"; return; }
    if (students.empty()) { cout << "\nNo students\n"; return; }
    
    cout << "\nSelect session:\n";
    for (int i=0; i<sessions.size(); i++) cout << i+1 << ". " << sessions[i].courseCode << "\n";
    int s; cin >> s; s--;
    if (s<0 || s>=sessions.size()) { cout << "Invalid\n"; return; }
    
    sessions[s].display();
    cout << "\nStudents:\n";
    for (Student stu : students) {
        cout << stu.index << " - " << stu.name;
        if (sessions[s].isMarked(stu.index)) cout << " [DONE]";
        cout << "\n";
    }
    
    string idx; int st;
    cout << "\nEnter index (0 to finish): ";
    while (cin >> idx, idx != "0") {
        if (!studentExists(idx)) { cout << "Not found. Try: "; continue; }
        if (sessions[s].isMarked(idx)) { cout << "Already marked. Next: "; continue; }
        cout << "1=Present 2=Late 3=Absent: "; cin >> st;
        if (st==1) { sessions[s].markPresent(idx); cout << "Present\n"; }
        else if (st==2) { sessions[s].markLate(idx); cout << "Late\n"; }
        else if (st==3) { sessions[s].markAbsent(idx); cout << "Absent\n"; }
        else cout << "Invalid\n";
        cout << "Next index (0 to finish): ";
    }
    saveSessions();
    sessions[s].report();
}

// Summary Function (this is the good one - keep it!)
void summary() {
    if (sessions.empty()) { cout << "\nNo sessions\n"; return; }
    int p=0, l=0, a=0;
    for (auto ses : sessions) { 
        p += ses.present.size(); 
        l += ses.late.size(); 
        a += ses.absent.size(); 
    }
    int t = p+l+a;
    cout << "\n=== SUMMARY ===\n";
    cout << "Sessions: " << sessions.size() << "\n";
    cout << "Total marks: " << t << "\n";
    if (t>0) {
        cout << "Present: " << p << " (" << (p*100/t) << "%)\n";
        cout << "Late: " << l << " (" << (l*100/t) << "%)\n";
        cout << "Absent: " << a << " (" << (a*100/t) << "%)\n";
    }
    
    char save;
    cout << "\nSave summary to file? (y/n): ";
    cin >> save;
    if (save == 'y' || save == 'Y') {
        ofstream file("summary_report.txt");
        file << "ATTENDANCE SUMMARY REPORT\n";
        file << "=========================\n";
        
        // Get current date
        time_t now = time(0);
        char* dt = ctime(&now);
        file << "Date: " << dt;
        
        file << "Sessions: " << sessions.size() << "\n";
        file << "Total Marks: " << t << "\n\n";
        file << "Present: " << p;
        if (t>0) file << " (" << (p*100/t) << "%)";
        file << "\nLate: " << l;
        if (t>0) file << " (" << (l*100/t) << "%)";
        file << "\nAbsent: " << a;
        if (t>0) file << " (" << (a*100/t) << "%)";
        file << "\n";
        file.close();
        cout << "Summary saved to summary_report.txt\n";
    }
}

// MAIN FUNCTION
int main() {
    loadStudents(); 
    loadSessions();
    
    int ch;
    do {
        cout << "\n=== WEEK 3 ATTENDANCE ===\n";
        cout << "1. Register Student\n";
        cout << "2. View Students\n";
        cout << "3. Search Student\n";
        cout << "4. Create Session\n";
        cout << "5. View Sessions\n";
        cout << "6. Mark Attendance\n";
        cout << "7. Session Report\n";
        cout << "8. Overall Summary\n";
        cout << "9. Exit\n";
        cout << "Choice: ";
        cin >> ch;
        
        if (ch==1) registerStudent();
        else if (ch==2) viewStudents();
        else if (ch==3) searchStudent();
        else if (ch==4) createSession();
        else if (ch==5) viewSessions();
        else if (ch==6) markAttendance();
        else if (ch==7) {
            if (sessions.empty()) cout << "No sessions\n";
            else { 
                int s; 
                cout << "Session (1-" << sessions.size() << "): "; 
                cin >> s; 
                sessions[s-1].report(); 
            }
        }
        else if (ch==8) summary();
        else if (ch==9) { 
            saveStudents(); 
            saveSessions(); 
            cout << "Goodbye!\n"; 
        }
        else cout << "Invalid choice\n";
        
    } while (ch != 9);
    
    return 0;
}
