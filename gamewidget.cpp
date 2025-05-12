include "gamewidget.h"
#include <QPainter>
#include <QKeyEvent>

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent), bgX(0), frameCount(0) {
    setFixedSize(800, 400);
    bgImage.load(":/images/background.png");

    player = new Player(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWidget::gameLoop);
    timer->start(16);
}

GameWidget::~GameWidget() {
    delete player;
    qDeleteAll(obstacles);
    qDeleteAll(coins);
}

void GameWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.drawPixmap(bgX, 0, bgImage);
    painter.drawPixmap(bgX + bgImage.width(), 0, bgImage);

    player->draw(&painter);

    for (Obstacle *obs : obstacles)
        obs->draw(&painter);
    for (Coin *coin : coins)
        coin->draw(&painter);

    score.draw(&painter);
}

void GameWidget::keyPressEvent(QKeyEvent *event) {
    player->handleKey(event);
}

void GameWidget::gameLoop() {
    bgX -= 5;
    if (bgX <= -bgImage.width()) bgX = 0;

    player->updateState();

    frameCount++;
    if (frameCount % 5 == 0)
        score.addDistance();

    if (frameCount % 100 == 0)
        obstacles.append(new Obstacle(width(), 330));
    if (frameCount % 150 == 0)
        coins.append(new Coin(width(), 250));

    for (int i = obstacles.size() - 1; i >= 0; --i) {
        obstacles[i]->update();
        if (obstacles[i]->rect().intersects(player->rect())) {
            timer->stop();
        }
        if (obstacles[i]->isOffScreen()) {
            delete obstacles[i];
            obstacles.removeAt(i);
        }
    }

    for (int i = coins.size() - 1; i >= 0; --i) {
        coins[i]->update();
        if (coins[i]->rect().intersects(player->rect())) {
            score.addCoin();
            delete coins[i];
            coins.removeAt(i);
        } else if (coins[i]->isOffScreen()) {
            delete coins[i];
            coins.removeAt(i);
        }
    }

    update();
}
