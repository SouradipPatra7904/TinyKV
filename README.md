Project Overview :
TinyKV is a single-server, single-client key-value store implemented in C++ with both CLI and GUI clients. 
It supports basic key-value operations and provides a polished GUI client with Light/Dark mode, command history, stats, and more.

Features ::-
A. Server : TCP-based server on port 4545.

B. Commands :
    1. PUT <key> <value>
    2. GET <key>
    3. GET_KEY <value>
    4. UPDATE <key> <value>
    5. DELETE <key>
    6. STATS – returns key count and metrics.
    7. SHUTDOWN – closes server gracefully.
    8. Single client connection, multiple commands per session.

C. CLI Client :
    1. Interactive command-line interface.
    2. Connects to server on 127.0.0.1:4545.
    3. Supports multiple commands until SHUTDOWN.

D. GUI Client : Built using Qt6 (Widgets + Network modules).
Features:
    1. Command dropdown (PUT, GET, UPDATE, DELETE, GET_KEY, STATS, SHUTDOWN).
    2. Key/Value fields dynamically enabled/disabled.
    3. Output box displays server responses.
    4. Stats displayed in table format.
    5. Dedicated STATS & SHUTDOWN buttons.
    6. Light / Dark mode toggle.
    7. Command history (click to populate key field).
    8. Status bar indicating connection.
    9. Auto-close GUI 3 seconds after SHUTDOWN.
