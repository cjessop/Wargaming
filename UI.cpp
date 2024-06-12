#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMapControl/QMapControl.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QMainWindow mainWindow;

    QWidget* centralWidget = new QWidget(&mainWindow);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QPushButton* button = new QPushButton("Click Me");
    layout->addWidget(button);

    // Create a map widget
    QMapControl* map = new QMapControl(&mainWindow);
    layout->addWidget(map);

    mainWindow.setCentralWidget(centralWidget);
    mainWindow.show();

    return app.exec();
}