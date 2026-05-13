#include <iostream>
#include <string>
#include <map>
#include <ctime>      
#include <cstdlib>
#include <iomanip>
using namespace std;

//  SECTION 1 — CONSTANTS & THRESHOLDS
const int    QUEUE_CAPACITY      = 20;   // Max pending transactions in buffer
const int    HISTORY_DEPTH       = 5;    // Transactions kept per user in stack
const double HIGH_FREQ_SECONDS   = 30.0; // Gap < 30 s  ? High-Frequency flag
const double LARGE_VOLUME_RATIO  = 5.0;  // Amount > 5× avg ? Large-Volume flag
//  SECTION 2 — TRANSACTION STRUCT
struct Transaction {
    int         id;          // Unique transaction ID
    string      userID;      // Owner of the transaction
    double      amount;      // Dollar amount
    string      type;        // "DEBIT" or "CREDIT"
    time_t      timestamp;   // Captured via ctime's time()
};


//  SECTION 3 — CIRCULAR QUEUE  (Transaction Buffer)
struct CircularQueue {
    Transaction data[QUEUE_CAPACITY];
    int  front;   // Index of the oldest item (next to dequeue)
    int  rear;    // Index of the newest item
    int  count;   // How many items are currently stored

    CircularQueue() : front(0), rear(-1), count(0) {}

    // Returns true if no more items can be added
    bool isFull()  const { return count == QUEUE_CAPACITY; }

    // Returns true if there is nothing to process
    bool isEmpty() const { return count == 0; }

    // ENQUEUE — add a new transaction to the back of the queue
    bool enqueue(const Transaction& t) {
        if (isFull()) {
            cout << "[QUEUE] Buffer full! Transaction " << t.id << " rejected.\n";
            return false;
        }
        // Advance rear with wrap-around using modulo
        rear = (rear + 1) % QUEUE_CAPACITY;
        data[rear] = t;
        count++;
        cout << "[QUEUE] Transaction " << t.id
             << " from user '" << t.userID
             << "' enqueued. (Buffer: " << count << "/" << QUEUE_CAPACITY << ")\n";
        return true;
    }

    // DEQUEUE — remove and return the oldest transaction
    Transaction dequeue() {
        Transaction t = data[front];
        front = (front + 1) % QUEUE_CAPACITY;  // Advance front with wrap-around
        count--;
        return t;
    }

    // PEEK — look at the oldest transaction without removing it
    Transaction peek() const { return data[front]; }
};


//  SECTION 4 — STACK  (Recent History Tracker)
struct Stack {
    Transaction data[HISTORY_DEPTH];
    int top;   // Index of the topmost (most recent) element; -1 = empty

    Stack() : top(-1) {}

    bool isEmpty() const { return top == -1; }
    bool isFull()  const { return top == HISTORY_DEPTH - 1; }

    // PUSH — add the newest transaction on top
    void push(const Transaction& t) {
        if (isFull()) {
            // Drop the oldest entry (data[0]) by shifting left
            for (int i = 0; i < HISTORY_DEPTH - 1; i++)
                data[i] = data[i + 1];
            // top stays at HISTORY_DEPTH-1; just overwrite the top slot
            data[top] = t;
        } else {
            data[++top] = t;
        }
    }

    // PEEK — return the most recent transaction WITHOUT removing it
    Transaction peek() const { return data[top]; }

    // Compute average amount of all stored transactions
    double averageAmount() const {
        if (isEmpty()) return 0.0;
        double sum = 0.0;
        for (int i = 0; i <= top; i++) sum += data[i].amount;
        return sum / (top + 1);
    }
};


//  SECTION 5 — FRAUD ANALYSIS FUNCTION
void analyzeFraud(const Transaction& t, Stack& history) {
    bool fraudDetected = false;

    cout << "\n  [ANALYSIS] Processing Txn #" << t.id
         << " | User: " << t.userID
         << " | Amount: $" << fixed << setprecision(2) << t.amount
         << " | Type: " << t.type << "\n";

    if (!history.isEmpty()) {
        Transaction last = history.peek();  // Most recent past transaction

        // --- CHECK 1: HIGH-FREQUENCY TRANSFER ---
        // difftime() returns seconds between two time_t values (ctime, no chrono)
        double gap = difftime(t.timestamp, last.timestamp);
        cout << "    Time since last transaction: " << gap << " seconds\n";

        if (gap < HIGH_FREQ_SECONDS && gap >= 0) {
            cout << "    *** ANOMALY DETECTED: HIGH-FREQUENCY TRANSFER ***\n"
                 << "    >>> Two transactions within " << gap
                 << "s (threshold: " << HIGH_FREQ_SECONDS << "s)\n";
            fraudDetected = true;
        }

        // --- CHECK 2: SUDDEN LARGE-VOLUME SHIFT ---
        double avg = history.averageAmount();
        if (avg > 0 && t.amount > LARGE_VOLUME_RATIO * avg) {
            cout << "    *** ANOMALY DETECTED: SUDDEN LARGE-VOLUME SHIFT ***\n"
                 << "    >>> Amount $" << t.amount
                 << " is " << (t.amount / avg)
                 << "x the user's average ($" << avg << ")\n";
            fraudDetected = true;
        }
    } else {
        cout << "    No prior history for this user. Baseline being established.\n";
    }

    if (!fraudDetected)
        cout << "    [OK] Transaction appears normal.\n";

    // Push the evaluated transaction onto the user's history stack
    history.push(t);
}
//  SECTION 6 — HELPER: GET MANUAL TIMESTAMP
time_t getTimestamp() {
    int choice;
    cout << "    Use (1) current time  or  (2) enter custom seconds-since-epoch? ";
    cin  >> choice;
    if (choice == 2) {
        long long secs;
        cout << "    Enter seconds-since-epoch (e.g. 1700000000): ";
        cin  >> secs;
        return (time_t)secs;
    }
    return time(NULL);   // time(NULL) returns current wall-clock time as time_t
}

//  SECTION 7 — MAIN  (Program Entry Point)
int main() {
    // 7a. Data structure instantiation
    CircularQueue buffer;                  // Single shared queue for all users
    map<string, Stack> userHistories;      // One stack per unique userID
    int txnCounter = 1;                    // Auto-incrementing transaction ID

    cout << "==============================================\n";
    cout << "  TRANSACTION FRAUD MONITORING SYSTEM\n";
    cout << "==============================================\n\n";

    int mainChoice;
    do {
        cout << "\n--- MAIN MENU ---\n";
        cout << "1. Submit a new transaction (enqueue)\n";
        cout << "2. Process next transaction in buffer (dequeue + analyze)\n";
        cout << "3. Process ALL pending transactions\n";
        cout << "4. View buffer status\n";
        cout << "5. Exit\n";
        cout << "Choice: ";
        cin  >> mainChoice;

        // OPTION 1 — Submit / Enqueue a transaction
        if (mainChoice == 1) {
            Transaction t;
            t.id = txnCounter++;

            cout << "\n Enter User ID (no spaces, e.g. alice123): ";
            cin  >> t.userID;

            cout << "  Enter amount ($): ";
            cin  >> t.amount;

            cout << "  Enter type (DEBIT / CREDIT): ";
            cin  >> t.type;

            cout << "  Set timestamp:\n";
            t.timestamp = getTimestamp();

            // Step: Hand the transaction to the circular queue buffer
            buffer.enqueue(t);
        }

        // OPTION 2 — Dequeue ONE transaction and run fraud checks
        else if (mainChoice == 2) {
            if (buffer.isEmpty()) {
                cout << "\n  [QUEUE] Buffer is empty. Nothing to process.\n";
            } else {
                // Step: Take the oldest transaction out of the queue (FIFO)
                Transaction t = buffer.dequeue();

                // Step: Retrieve (or auto-create) the stack for this user
                Stack& history = userHistories[t.userID];

                // Step: Run fraud analysis, comparing t against history stack
                analyzeFraud(t, history);
            }
        }

        // OPTION 3 — Drain the entire queue
        else if (mainChoice == 3) {
            if (buffer.isEmpty()) {
                cout << "\n  [QUEUE] Buffer is empty.\n";
            } else {
                cout << "\n  Processing all " << buffer.count << " pending transaction(s)...\n";
                while (!buffer.isEmpty()) {
                    Transaction t = buffer.dequeue();
                    Stack& history = userHistories[t.userID];
                    analyzeFraud(t, history);
                }
                cout << "\n  [QUEUE] Buffer cleared.\n";
            }
        }

        // OPTION 4 — Show queue fill level
        else if (mainChoice == 4) {
            cout << "\n  Buffer: " << buffer.count << " / "
                 << QUEUE_CAPACITY << " slots used.\n";
            if (!buffer.isEmpty())
                cout << "  Next to process: Txn #" << buffer.peek().id
                     << " from user '" << buffer.peek().userID << "'\n";
        }

    } while (mainChoice != 5);

    cout << "\nSystem shut down. Goodbye.\n";
    return 0;
}
