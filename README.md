GROUP 1A
MEMBERS
1.	Onyango Marikus Munda 	SCT221-0120/2024
2.	Anilov Githua Maina		SCT221-0084/2024
3.	Amani Mwale Wasonga		SCT221-0104/2024
4.	Ian Maina				SCT221-0106/2024

Project Title: Real-Time Mobile Money Behavioral Anomaly Detection

1. Problem Statement
Current mobile money platforms face a surge in fraud where unauthorized users gain access to accounts and perform fast transactions to drain funds. Standard security measures often fail to catch these fast-initiated attacks in real-time. There is a critical need for a system that can monitor the flow(money in, money out) of transactions and analyze user transaction history instantly to flag suspicious behavioral patterns.

2. Identified Real-Life Task
The task is to develop a monitoring program that intercepts incoming transaction requests, organizes them for sequential processing, and compares them against the user’s immediate past activity to identify anomalies like "High-Frequency Transfers" or "Sudden Large-Volume Shifts".

3. Role of Data Structures
To solve this, the application will implement the following:
Queue (Linear/Circular): Acts as the Transaction Buffer. Incoming requests from various users are enqueued to ensure they are processed in the exact order they were received (FIFO), preventing data race conditions.
Stack: Acts as the Recent History Tracker. For each active session, the system pushes the last N transactions onto a stack. Because a stack is LIFO, the system can instantly "peek" at the most recent transaction to calculate the time gap between it and the current request. Very short time gaps between transactions leads to identification of a likely fraud.

4. Objectives
Efficient Sequencing: Use a Queue to manage high-volume transaction traffic without data loss.
Rapid Backtracking: Use a Stack to perform near-instantaneous comparisons of current vs. previous user behavior.
Automated Triage: Automatically flag or block transactions that violate predefined behavioral thresholds (e.g., more than 3 transactions within 30 seconds).
