#ifndef GUI_CLIENT_HPP
#define GUI_CLIENT_HPP

#include <QWidget>
#include <QTcpSocket>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QStatusBar>
#include <QTableWidget>
#include <QTimer>
#include <QListWidget>

class GUI_Client : public QWidget {
    Q_OBJECT

public:
    GUI_Client(QWidget *parent = nullptr);

private slots:
    void onSendClicked();
    void onStatsClicked();
    void onShutdownClicked();
    void onReadyRead();
    void onCommandChanged(int index);
    void onHistoryItemClicked(QListWidgetItem *item);
    void toggleTheme();

private:
    void updateFieldStates();
    void appendOutput(const QString &text, const QString &color = "black");
    void displayStats(const QString &statsText);
    void addToHistory(const QString &cmd);
    void applyTheme();

    QTcpSocket *socket;
    QComboBox *commandBox;
    QLineEdit *keyEdit;
    QLineEdit *valueEdit;
    QPushButton *sendButton;
    QPushButton *statsButton;
    QPushButton *shutdownButton;
    QTextEdit *outputBox;
    QStatusBar *statusBar;
    QTableWidget *statsTable;
    QPushButton *themeToggleButton;
    QListWidget *historyList;

    QStringList commandHistory;
    int historyIndex = -1;
    bool darkMode = false;
};
#endif
