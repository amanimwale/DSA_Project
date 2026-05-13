#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <ctime>
#include <vector>

using namespace std;

// Structure to represent a Transaction
struct Transaction {
    int userId;
    double amount;
    time_t timestamp;

    void display() {
        cout << "User [" << userId << "] | Amount: $" << amount 
             << " | Time: " << timestamp << endl;
    }
};

class FraudMonitor {
private:
    queue<Transaction> buffer;          // FIFO Buffer for incoming requests
    stack<Transaction> recentHistory;   // LIFO Tracker for last N transactions
    const int TIME_THRESHOLD = 2;       // Seconds to detect High-Frequency
    const double VOLUME_THRESHOLD = 5.0; // Multiplier for Sudden Volume Shift

public:
    // Step 1: Enqueue incoming request (Sequential Processing)
    void receiveRequest(int id, double amt) {
        Transaction t = {id, amt, time(0)};
        buffer.push(t);
        cout << ">> Transaction queued for User " << id << "..." << endl;
    }

    // Step 2: Process and Analyze
    void processNext() {
        if (buffer.empty()) return;

        Transaction current = buffer.front();
        buffer.pop();

        cout << "\nAnalyzing Transaction for User " << current.userId << ":" << endl;

        // Step 3: Peek at Stack for Anomaly Detection
        if (!recentHistory.empty()) {
            Transaction last = recentHistory.top();

            // Check for High-Frequency Transfers
            double timeGap = difftime(current.timestamp, last.timestamp);
            if (timeGap <= TIME_THRESHOLD) {
                cout << "[ALERT] High-Frequency Transfer! Gap: " << timeGap << "s" << endl;
            }

            // Check for Sudden Large-Volume Shifts
            if (current.amount >= last.amount * VOLUME_THRESHOLD) {
                cout << "[ALERT] Sudden Large-Volume Shift! (Current: $" << current.amount 
                     << " vs Last: $" << last.amount << ")" << endl;
            }
        }

        // Step 4: Update History (LIFO)
        recentHistory.push(current);
        cout << "Transaction processed successfully." << endl;
    }
};

int main() {
    FraudMonitor monitor;
    int choice;
    
    cout << "--- Fraud Detection System Simulation ---" << endl;

    while (true) {
        cout << "\n1. New Transaction\n2. Process Pending\n3. Exit\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            int id;
            double amt;
            cout << "Enter User ID and Amount: ";
            cin >> id >> amt;
            monitor.receiveRequest(id, amt);
        } else if (choice == 2) {
            monitor.processNext();
        } else {
            break;
        }
    }

    return 0;
}
