#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>

struct Transaction {
    int userId;
    double amount;
    long long timestamp; // Unix timestamp in seconds
};

class TransactionMonitor {
private:
    std::queue<Transaction> buffer;          // FIFO Transaction Buffer
    std::stack<Transaction> recentHistory;   // LIFO History Tracker
    
    const double LARGE_VOLUME_THRESHOLD = 10000.0;
    const int TIME_GAP_THRESHOLD = 5; // Seconds

public:
    // Step 1: Enqueue incoming transaction
    void receiveTransaction(int id, double amt) {
        Transaction t = {id, amt, (long long)time(0)};
        buffer.push(t);
        std::cout << "[Buffer] Transaction for User " << id << " enqueued.\n";
    }

    // Step 2: Process and Check for Anomalies
    void processNext() {
        if (buffer.empty()) return;

        Transaction current = buffer.front();
        buffer.pop();

        std::cout << "\nProcessing User " << current.userId << " | Amount: " << current.amount << "...\n";

        // Check for "Sudden Large-Volume Shifts"
        if (current.amount > LARGE_VOLUME_THRESHOLD) {
            std::cout << "!! ALERT: Sudden Large-Volume Shift detected!\n";
        }

        // Step 3: Use Stack to identify "High-Frequency Transfers"
        if (!recentHistory.empty()) {
            Transaction last = recentHistory.top();
            
            // Peek at LIFO to check the time gap
            long long timeGap = current.timestamp - last.timestamp;
            if (timeGap < TIME_GAP_THRESHOLD && current.userId == last.userId) {
                std::cout << "!! ALERT: High-Frequency Transfer (Time gap: " << timeGap << "s)\n";
            }
        }

        // Push current to history stack (LIFO)
        recentHistory.push(current);
    }
};

int main() {
    TransactionMonitor monitor;

    // Simulating incoming requests
    monitor.receiveTransaction(101, 500.0);
    monitor.receiveTransaction(101, 15000.0); // Potential Large Volume
    monitor.receiveTransaction(101, 200.0);   // Potential High Frequency

    // Process the buffer
    monitor.processNext();
    monitor.processNext();
    monitor.processNext();

    return 0;
}
