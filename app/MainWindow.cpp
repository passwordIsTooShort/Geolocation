#include <algorithm>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>

#include "MainWindow.hpp"

MainWindow::MainWindow(std::unique_ptr<ILocationManager> locationManager)
: mLocationManager(std::move(locationManager))
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    auto menuBar = createMenuBar();
    setMenuBar(menuBar);

    auto mainWidget = new QWidget();
    auto outLayout = new QGridLayout(mainWidget);

    int level = 0;
    level = addLocationAddToGridLayout(outLayout, level);
    level = addConfigurationToGridLayout(outLayout, level);
    level = addStatusToGridLayout(outLayout, level);

    setCentralWidget(mainWidget);

    mLocationManager->setOnNewLocationCallback([this](auto ipLocationData)
    {
        handleNewLocation(ipLocationData);
    });
}

QMenuBar* MainWindow::createMenuBar()
{
    auto fileMenu = new QMenu("File");
    fileMenu->addAction("Load from file");
    fileMenu->addAction("Exit");

    auto diagnosticsMenu = new QMenu("Diagnostics");
    diagnosticsMenu->addAction("Verify connection to DB");
    diagnosticsMenu->addAction("Get failed requests list");
    diagnosticsMenu->addAction("Get list of requests from startup");

    auto menuBar = new QMenuBar();
    menuBar->addMenu(fileMenu);
    menuBar->addSeparator();
    menuBar->addMenu(diagnosticsMenu);

    return menuBar;
}

int MainWindow::addLocationAddToGridLayout(QGridLayout* gridLayout, int level)
{
    auto findLabel = new QLabel("IP address or url: ");
    mGetLocationLineEdit = new QLineEdit("");
    mGetLocationLineEdit->setMinimumWidth(300);
    mGetLocationPushButton = new QPushButton("Find location");

    gridLayout->addWidget(findLabel, level, 0);
    gridLayout->addWidget(mGetLocationLineEdit, level, 1);
    gridLayout->addWidget(mGetLocationPushButton, level, 2);

    connect(mGetLocationPushButton, &QPushButton::released, this, &MainWindow::handleAddLocationButton);

    return ++level;
}

int MainWindow::addConfigurationToGridLayout(QGridLayout* gridLayout, int level)
{
    mGetLocationWithForceCheckBox = new QCheckBox("Force update");
    mGetLocationWithForceCheckBox->setChecked(false);
    mGetLocationOnlyDbCheckBox = new QCheckBox("Search only in DB");
    mGetLocationOnlyDbCheckBox->setChecked(false);

    gridLayout->addWidget(mGetLocationWithForceCheckBox, level, 0, 1, GRID_WIDTH);
    ++level;
    gridLayout->addWidget(mGetLocationOnlyDbCheckBox, level, 0, 1, GRID_WIDTH);

    return ++level;
}

int MainWindow::addStatusToGridLayout(QGridLayout* gridLayout, int level)
{
    auto lineSeparator = new QFrame();
    lineSeparator->setFrameShape(QFrame::HLine);
    lineSeparator->setFrameShadow(QFrame::Sunken);

    gridLayout->addWidget(lineSeparator, level, 0, 1, GRID_WIDTH);
    ++level;

    for (auto& locationPlaceholder : mLocationResults)
    {
        locationPlaceholder = new QLabel("");
        locationPlaceholder->setTextInteractionFlags(Qt::TextSelectableByMouse);
        gridLayout->addWidget(locationPlaceholder, level++, 0, 1, GRID_WIDTH);
    }

    return level;
}

void MainWindow::handleAddLocationButton()
{
    QStringList errorStrings;
    const auto urlOrIpToAdd = mGetLocationLineEdit->text();
    if (urlOrIpToAdd.isEmpty())
    {
        errorStrings.emplace_back("Provide first url or ip address that you want to find location");
    }

    LocationAddConfig locationAddConfig;
    locationAddConfig.forceUpdate = mGetLocationWithForceCheckBox->isChecked();
    locationAddConfig.searchOnlyDb = mGetLocationOnlyDbCheckBox->isChecked();
    if (locationAddConfig.forceUpdate && locationAddConfig.searchOnlyDb)
    {
        errorStrings.emplace_back(QString("Search DB option and force update are mutually") +
                                          " exclusive. Check only one, or uncheck both");
    }

    if (errorStrings.empty())
    {
        if (IpAddress::isIpAddress(urlOrIpToAdd.toStdString()))
        {
            handleGetLocation(IpAddress(urlOrIpToAdd.toStdString()), locationAddConfig);
        }
        else if (Url::isUrl(urlOrIpToAdd.toStdString()))
        {
            handleGetLocation(Url(urlOrIpToAdd.toStdString()), locationAddConfig);
        }
        else
        {
            errorStrings.emplace_back("Provided string seems to be neither url or ip");
        }
    }

    if (!errorStrings.empty())
    {
        QMessageBox* msgbox = new QMessageBox();
        msgbox->setWindowTitle("Warning");
        msgbox->setText("Error list:\n" + errorStrings.join('\n'));
        msgbox->open();
    }
}

void MainWindow::printNewLocationError(QString firstLineError, QString secondLineError)
{
    shiftResults();
    auto locationIter = mLocationResults.begin();

    // Filling frist line:
    (*locationIter)->setText(firstLineError);

    // Filling second line:
    ++locationIter;
    (*locationIter)->setText(secondLineError);
}

void MainWindow::handleNewLocation(IpLocationData ipLocationData)
{
    shiftResults();
    auto locationIter = mLocationResults.begin();

    // Filling frist line:
    QString textToFirstLine = "Ip: " + QString::fromStdString(ipLocationData.ipAddress.toString());
    if (!ipLocationData.url.isEmpty())
    {
        textToFirstLine += ", url: " + QString::fromStdString(ipLocationData.url.toString());
    }
    (*locationIter)->setText(textToFirstLine);

    // Filling second line:
    ++locationIter;
    (*locationIter)->setText("Location (latitiude, longtidiude): (" +
                                  QString::number(ipLocationData.geolocationdata.latitude) +
                                  ", " +
                                  QString::number(ipLocationData.geolocationdata.longitude) +
                                  ")");
}

void MainWindow::shiftResults()
{
    auto iter = mLocationResults.rbegin();
    while (iter < mLocationResults.rend() - LINES_PER_SINGLE_RESULT)
    {
        auto textToCopy = (*(iter + LINES_PER_SINGLE_RESULT))->text();
        (*iter)->setText(std::move(textToCopy));
        ++iter;
    }
}
