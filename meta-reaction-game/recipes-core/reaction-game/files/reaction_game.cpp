#include "reaction_game.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QFont>
#include <QRandomGenerator>
#include <QElapsedTimer>
#include <QDateTime>
#include <iostream>

// GPIO Handler Implementation
GPIOHandler::GPIOHandler() : gpio_enabled(false)
#ifdef GPIO_SUPPORT_ENABLED
    , chip(nullptr), led_line(nullptr), button_line(nullptr)
#endif
{
#ifdef GPIO_SUPPORT_ENABLED
    try {
        // Open GPIO chip (usually gpiochip0 on Raspberry Pi)
        chip = gpiod_chip_open_by_name("gpiochip0");
        if (!chip) {
            std::cerr << "Failed to open GPIO chip" << std::endl;
            return;
        }
        
        // Configure LED on GPIO 18 (pin 12) - output
        led_line = gpiod_chip_get_line(chip, 18);
        if (!led_line) {
            std::cerr << "Failed to get LED line" << std::endl;
            gpiod_chip_close(chip);
            chip = nullptr;
            return;
        }
        
        if (gpiod_line_request_output(led_line, "reaction_game_led", 0) < 0) {
            std::cerr << "Failed to request LED line as output" << std::endl;
            gpiod_chip_close(chip);
            chip = nullptr;
            return;
        }
        
        // Configure button on GPIO 24 (pin 18) - input with pull-up
        button_line = gpiod_chip_get_line(chip, 24);
        if (!button_line) {
            std::cerr << "Failed to get button line" << std::endl;
            gpiod_line_release(led_line);
            gpiod_chip_close(chip);
            chip = nullptr;
            return;
        }
        
        if (gpiod_line_request_input_flags(button_line, "reaction_game_button", 
                                          GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP) < 0) {
            std::cerr << "Failed to request button line as input" << std::endl;
            gpiod_line_release(led_line);
            gpiod_chip_close(chip);
            chip = nullptr;
            return;
        }
        
        gpio_enabled = true;
        std::cout << "GPIO initialized successfully" << std::endl;
        
    } catch (...) {
        std::cerr << "Exception during GPIO initialization" << std::endl;
        cleanup();
    }
#else
    std::cout << "GPIO support not compiled in" << std::endl;
#endif
}

GPIOHandler::~GPIOHandler() {
    cleanup();
}

void GPIOHandler::setLED(bool state) {
#ifdef GPIO_SUPPORT_ENABLED
    if (gpio_enabled && led_line) {
        gpiod_line_set_value(led_line, state ? 1 : 0);
    }
#endif
}

bool GPIOHandler::readButton() {
#ifdef GPIO_SUPPORT_ENABLED
    if (gpio_enabled && button_line) {
        // Button is active low (0 when pressed due to pull-up)
        return gpiod_line_get_value(button_line) == 0;
    }
#endif
    return false;
}

void GPIOHandler::cleanup() {
#ifdef GPIO_SUPPORT_ENABLED
    if (led_line) {
        gpiod_line_release(led_line);
        led_line = nullptr;
    }
    if (button_line) {
        gpiod_line_release(button_line);
        button_line = nullptr;
    }
    if (chip) {
        gpiod_chip_close(chip);
        chip = nullptr;
    }
#endif
    gpio_enabled = false;
}

// Main Window Implementation
ReactionGameWindow::ReactionGameWindow(QWidget *parent)
    : QMainWindow(parent)
    , gpio_handler(std::make_unique<GPIOHandler>())
    , game_active(false)
    , start_time(0)
{
    setupUI();
    
    // Timer for random delay
    game_timer = new QTimer(this);
    game_timer->setSingleShot(true);
    connect(game_timer, &QTimer::timeout, this, &ReactionGameWindow::showReactSignal);
    
    // Timer for checking hardware button
    button_check_timer = new QTimer(this);
    connect(button_check_timer, &QTimer::timeout, this, &ReactionGameWindow::checkHardwareButton);
    button_check_timer->start(50); // Check every 50ms
}

ReactionGameWindow::~ReactionGameWindow() {
    if (gpio_handler) {
        gpio_handler->cleanup();
    }
}

void ReactionGameWindow::setupUI() {
    setWindowTitle("Reaction Game - E2E GPIO Control");
    setGeometry(100, 100, 800, 600);
    
    // Set dark theme
    setStyleSheet(R"(
        QMainWindow {
            background-color: #2b2b2b;
        }
        QLabel {
            color: white;
            font-size: 18px;
            padding: 10px;
        }
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            padding: 15px;
            font-size: 16px;
            border-radius: 5px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        QPushButton:disabled {
            background-color: #666666;
        }
    )");
    
    central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    
    main_layout = new QVBoxLayout();
    central_widget->setLayout(main_layout);
    
    // Title
    title_label = new QLabel("Reaction Time Game - E2E GPIO Control");
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setFont(QFont("Arial", 24, QFont::Bold));
    main_layout->addWidget(title_label);
    
    // GPIO Status
    QString gpio_status = gpio_handler->isEnabled() ? "GPIO Enabled" : "GPIO Disabled (Software Mode)";
    gpio_status_label = new QLabel(QString("Hardware Status: %1").arg(gpio_status));
    gpio_status_label->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(gpio_status_label);
    
    // Instructions
    QString instructions_text = R"(Instructions:
1. Click "Start" and wait for the GREEN light (LED will turn on)
2. React by clicking "React!" button OR pressing the hardware button
3. Try to get the fastest reaction time!

Hardware: LED on GPIO 18 (Pin 12), Button on GPIO 24 (Pin 18))";
    
    instructions_label = new QLabel(instructions_text);
    instructions_label->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(instructions_label);
    
    // Status display
    status_label = new QLabel("Ready to start!");
    status_label->setAlignment(Qt::AlignCenter);
    status_label->setFont(QFont("Arial", 20));
    main_layout->addWidget(status_label);
    
    // Reaction area
    reaction_button = new QPushButton("React!");
    reaction_button->setMinimumHeight(100);
    reaction_button->setFont(QFont("Arial", 18, QFont::Bold));
    reaction_button->setEnabled(false);
    connect(reaction_button, &QPushButton::clicked, this, &ReactionGameWindow::reactClicked);
    main_layout->addWidget(reaction_button);
    
    // Control buttons
    button_layout = new QHBoxLayout();
    
    start_button = new QPushButton("Start Game");
    connect(start_button, &QPushButton::clicked, this, &ReactionGameWindow::startGame);
    button_layout->addWidget(start_button);
    
    reset_button = new QPushButton("Reset");
    connect(reset_button, &QPushButton::clicked, this, &ReactionGameWindow::resetGame);
    button_layout->addWidget(reset_button);
    
    quit_button = new QPushButton("Quit");
    connect(quit_button, &QPushButton::clicked, this, &QWidget::close);
    button_layout->addWidget(quit_button);
    
    main_layout->addLayout(button_layout);
    
    // Results
    result_label = new QLabel("");
    result_label->setAlignment(Qt::AlignCenter);
    result_label->setFont(QFont("Arial", 16));
    main_layout->addWidget(result_label);
}

void ReactionGameWindow::startGame() {
    start_button->setEnabled(false);
    reaction_button->setEnabled(false);
    reaction_button->setStyleSheet("background-color: #ff6b6b;");
    reaction_button->setText("Wait...");
    status_label->setText("Get ready... Wait for GREEN!");
    result_label->setText("");
    
    // Turn off LED
    gpio_handler->setLED(false);
    
    // Random delay between 2-6 seconds
    int delay = QRandomGenerator::global()->bounded(2000, 6000);
    game_timer->start(delay);
}

void ReactionGameWindow::showReactSignal() {
    reaction_button->setStyleSheet("background-color: #4CAF50;");
    reaction_button->setText("REACT NOW!");
    reaction_button->setEnabled(true);
    status_label->setText("GO! Click button or press hardware button!");
    
    // Turn on LED
    gpio_handler->setLED(true);
    
    start_time = QDateTime::currentMSecsSinceEpoch();
    game_active = true;
}

void ReactionGameWindow::reactClicked() {
    if (game_active) {
        qint64 reaction_time = QDateTime::currentMSecsSinceEpoch() - start_time;
        game_active = false;
        
        // Turn off LED
        gpio_handler->setLED(false);
        
        reaction_button->setEnabled(false);
        start_button->setEnabled(true);
        
        result_label->setText(QString("Your reaction time: %1 ms").arg(reaction_time));
        
        if (reaction_time < 200) {
            status_label->setText("Excellent! Lightning fast!");
        } else if (reaction_time < 300) {
            status_label->setText("Great reaction time!");
        } else if (reaction_time < 400) {
            status_label->setText("Good reaction time!");
        } else {
            status_label->setText("Keep practicing!");
        }
    }
}

void ReactionGameWindow::resetGame() {
    game_timer->stop();
    game_active = false;
    gpio_handler->setLED(false);
    start_button->setEnabled(true);
    reaction_button->setEnabled(false);
    reaction_button->setStyleSheet("background-color: #4CAF50;");
    reaction_button->setText("React!");
    status_label->setText("Ready to start!");
    result_label->setText("");
}

void ReactionGameWindow::checkHardwareButton() {
    if (game_active && gpio_handler->readButton()) {
        reactClicked();
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties for Wayland
    app.setAttribute(Qt::AA_EnableHighDpiScaling, true);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    
    ReactionGameWindow window;
    window.show();
    
    return app.exec();
}

#include "reaction_game.moc"
