#ifndef REACTION_GAME_H
#define REACTION_GAME_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QWidget>
#include <QApplication>
#include <QFont>
#include <memory>

#ifdef GPIO_SUPPORT_ENABLED
extern "C" {
#include <gpiod.h>
}
#endif

class GPIOHandler {
public:
    GPIOHandler();
    ~GPIOHandler();
    
    bool isEnabled() const { return gpio_enabled; }
    void setLED(bool state);
    bool readButton();
    void cleanup();

private:
    bool gpio_enabled;
    
#ifdef GPIO_SUPPORT_ENABLED
    struct gpiod_chip* chip;
    struct gpiod_line* led_line;
    struct gpiod_line* button_line;
#endif
};

class ReactionGameWindow : public QMainWindow
{
    Q_OBJECT

public:
    ReactionGameWindow(QWidget *parent = nullptr);
    ~ReactionGameWindow();

private slots:
    void startGame();
    void showReactSignal();
    void reactClicked();
    void resetGame();
    void checkHardwareButton();

private:
    void setupUI();
    
    // UI components
    QWidget* central_widget;
    QVBoxLayout* main_layout;
    QLabel* title_label;
    QLabel* gpio_status_label;
    QLabel* instructions_label;
    QLabel* status_label;
    QPushButton* reaction_button;
    QHBoxLayout* button_layout;
    QPushButton* start_button;
    QPushButton* reset_button;
    QPushButton* quit_button;
    QLabel* result_label;
    
    // Game logic
    std::unique_ptr<GPIOHandler> gpio_handler;
    QTimer* game_timer;
    QTimer* button_check_timer;
    bool game_active;
    qint64 start_time;
};

#endif // REACTION_GAME_H
