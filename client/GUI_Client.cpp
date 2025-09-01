#include "GUI_Client.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QScrollBar>
#include <QPalette>

GUI_Client::GUI_Client(QWidget *parent) : QWidget(parent) {
    setWindowTitle("TinyKV GUI Client");
    setMinimumSize(900, 900);

    // --- Layouts ---
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *commandLayout = new QHBoxLayout();
    QHBoxLayout *topPanel = new QHBoxLayout();

    // --- Theme Toggle ---
    themeToggleButton = new QPushButton("Switch to Dark Mode", this);
    topPanel->addWidget(new QLabel("Theme:"));
    topPanel->addWidget(themeToggleButton);
    topPanel->addStretch();

    // --- History GUI ---
    historyList = new QListWidget(this);
    historyList->setMaximumHeight(80);
    topPanel->addWidget(new QLabel("History:"));
    topPanel->addWidget(historyList);

    mainLayout->addLayout(topPanel);

    // --- Command Dropdown ---
    commandBox = new QComboBox(this);
    commandBox->addItems({"PUT", "GET", "UPDATE", "DELETE", "GET_KEY", "STATS", "SHUTDOWN"});

    // --- Key & Value Fields ---
    keyEdit = new QLineEdit(this);
    keyEdit->setPlaceholderText("Enter Key");
    valueEdit = new QLineEdit(this);
    valueEdit->setPlaceholderText("Enter Value");

    // --- Buttons ---
    sendButton = new QPushButton("Send", this);
    statsButton = new QPushButton("STATS", this);
    shutdownButton = new QPushButton("SHUTDOWN", this);

    commandLayout->addWidget(commandBox);
    commandLayout->addWidget(keyEdit);
    commandLayout->addWidget(valueEdit);
    commandLayout->addWidget(sendButton);

    mainLayout->addLayout(commandLayout);
    mainLayout->addWidget(statsButton);
    mainLayout->addWidget(shutdownButton);

    // --- Output Box ---
    outputBox = new QTextEdit(this);
    outputBox->setReadOnly(true);
    mainLayout->addWidget(new QLabel("Output:"));
    mainLayout->addWidget(outputBox);

    // --- Stats Table ---
    statsTable = new QTableWidget(this);
    statsTable->setColumnCount(2);
    statsTable->setHorizontalHeaderLabels({"Metric", "Value"});
    statsTable->horizontalHeader()->setStretchLastSection(true);
    statsTable->setVisible(false);
    mainLayout->addWidget(statsTable);

    // --- Status Bar ---
    statusBar = new QStatusBar(this);
    statusBar->showMessage("🟢 Connected");
    mainLayout->addWidget(statusBar);

    // --- Socket ---
    socket = new QTcpSocket(this);
    socket->connectToHost("127.0.0.1", 4545);

    // --- Connections ---
    connect(sendButton, &QPushButton::clicked, this, &GUI_Client::onSendClicked);
    connect(statsButton, &QPushButton::clicked, this, &GUI_Client::onStatsClicked);
    connect(shutdownButton, &QPushButton::clicked, this, &GUI_Client::onShutdownClicked);
    connect(commandBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GUI_Client::onCommandChanged);
    connect(socket, &QTcpSocket::readyRead, this, &GUI_Client::onReadyRead);
    connect(themeToggleButton, &QPushButton::clicked, this, &GUI_Client::toggleTheme);
    connect(historyList, &QListWidget::itemClicked, this, &GUI_Client::onHistoryItemClicked);

    updateFieldStates();
    applyTheme();
}

// --- Field State Management ---
void GUI_Client::onCommandChanged(int index) {
    Q_UNUSED(index);
    updateFieldStates();
}

void GUI_Client::updateFieldStates() {
    QString cmd = commandBox->currentText();

    if (cmd == "PUT" || cmd == "UPDATE") {
        keyEdit->setEnabled(true);
        valueEdit->setEnabled(true);
        keyEdit->setStyleSheet("");
        valueEdit->setStyleSheet("");
    } else if (cmd == "GET" || cmd == "DELETE") {
        keyEdit->setEnabled(true);
        valueEdit->setEnabled(false);
        valueEdit->setStyleSheet("background-color: lightgray;");
        keyEdit->setStyleSheet("");
    } else if (cmd == "GET_KEY") {
        keyEdit->setEnabled(false);
        valueEdit->setEnabled(true);
        keyEdit->setStyleSheet("background-color: lightgray;");
        valueEdit->setStyleSheet("");
    } else { // STATS or SHUTDOWN
        keyEdit->setEnabled(false);
        valueEdit->setEnabled(false);
        keyEdit->setStyleSheet("background-color: lightgray;");
        valueEdit->setStyleSheet("background-color: lightgray;");
    }
}

// --- Output Handling ---
void GUI_Client::appendOutput(const QString &text, const QString &color) {
    outputBox->setTextColor(QColor(color));
    outputBox->append(text);
    outputBox->verticalScrollBar()->setValue(outputBox->verticalScrollBar()->maximum());
}

void GUI_Client::displayStats(const QString &statsText) {
    statsTable->setVisible(true);
    statsTable->clearContents();

    QStringList lines = statsText.split('\n', Qt::SkipEmptyParts);
    statsTable->setRowCount(lines.size());

    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        QStringList parts = line.split(":");
        if (parts.size() == 2) {
            statsTable->setItem(i, 0, new QTableWidgetItem(parts[0].trimmed()));
            statsTable->setItem(i, 1, new QTableWidgetItem(parts[1].trimmed()));
        } else {
            statsTable->setItem(i, 0, new QTableWidgetItem(line.trimmed()));
            statsTable->setItem(i, 1, new QTableWidgetItem(""));
        }
    }
}

// --- Command History ---
void GUI_Client::addToHistory(const QString &cmd) {
    if (!cmd.isEmpty()) {
        commandHistory.append(cmd);
        historyList->addItem(cmd);
        if (commandHistory.size() > 20) {
            commandHistory.removeFirst();
            delete historyList->takeItem(0);
        }
        historyIndex = commandHistory.size();
    }
}

void GUI_Client::onHistoryItemClicked(QListWidgetItem *item) {
    QString cmd = item->text();
    keyEdit->setText(cmd);
}

// --- Sending Commands ---
void GUI_Client::onSendClicked() {
    QString textColor = darkMode ? "#FFE4C4" : "#0121daff";


    QString cmd = commandBox->currentText();
    QString key = keyEdit->text();
    QString value = valueEdit->text();

    if (!socket->isOpen()) {
        appendOutput("Error: Not connected to server.", textColor);
        return;
    }

    QString request;
    if (cmd == "PUT" || cmd == "UPDATE") {
        request = cmd + " " + key + " " + value + "\n";
    } else if (cmd == "GET" || cmd == "DELETE") {
        request = cmd + " " + key + "\n";
    } else if (cmd == "GET_KEY") {
        request = cmd + " " + value + "\n";
    } else if (cmd == "STATS") {
        request = "STATS\n";
    } else if (cmd == "SHUTDOWN") {
        socket->write("SHUTDOWN\n");
        socket->flush();
        appendOutput("Client disconnected. Goodbye ;)", textColor);
        statusBar->showMessage("🔴 Disconnected");
        QTimer::singleShot(3000, this, &QWidget::close);
        return;
    }

    addToHistory(request.trimmed());
    socket->write(request.toUtf8());
    socket->flush();
}

// --- STATS & SHUTDOWN ---
void GUI_Client::onStatsClicked() {
    if (socket->isOpen()) socket->write("STATS\n");
}

void GUI_Client::onShutdownClicked() {
    QString textColor = darkMode ? "#FFE4C4" : "#0121daff";

    if (socket->isOpen()) {
        socket->write("SHUTDOWN\n");
        socket->flush();
        appendOutput("Client disconnected. Goodbye ;)",textColor);
        statusBar->showMessage("🔴 Disconnected");
        QTimer::singleShot(3000, this, &QWidget::close);
    }
}

// --- Incoming Data ---
void GUI_Client::onReadyRead() {
    QByteArray response = socket->readAll();
    QString respStr = QString(response);

    // Determine text color dynamically
    QString textColor = darkMode ? "#FFE4C4" : "#000000";

    if (respStr.contains("==== Server Statistics ====")) {
        displayStats(respStr);  // Stats table already styled separately
    }
    else {
        appendOutput(respStr, textColor);
    }
}


// --- Theme Toggle ---
void GUI_Client::toggleTheme() {
    darkMode = !darkMode;
    applyTheme();
    themeToggleButton->setText(darkMode ? "Switch to Light Mode" : "Switch to Dark Mode");
}

void GUI_Client::applyTheme() {
    if (darkMode) {
        this->setStyleSheet("background-color: #1E1E1E; color: #FFE4C4;");
        outputBox->setStyleSheet("background-color: #2C2C2C; color: #FFE4C4;");
        keyEdit->setStyleSheet(keyEdit->isEnabled() ? "" : "background-color: #555555;");
        valueEdit->setStyleSheet(valueEdit->isEnabled() ? "" : "background-color: #555555;");
        statsTable->setStyleSheet("background-color: #2C2C2C; color: #FFE4C4; gridline-color: #444444;");
    } else {
        this->setStyleSheet("background-color: #FFE4C4; color: #000000;");
        outputBox->setStyleSheet("background-color: #FFF8F0; color: #000000;");
        keyEdit->setStyleSheet(keyEdit->isEnabled() ? "" : "background-color: #D3D3D3;");
        valueEdit->setStyleSheet(valueEdit->isEnabled() ? "" : "background-color: #D3D3D3;");
        statsTable->setStyleSheet("background-color: #FFF8F0; color: #000000; gridline-color: #F5DEB3;");
    }
}

