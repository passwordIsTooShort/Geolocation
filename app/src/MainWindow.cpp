#include <algorithm>
#include <fstream>
#include <filesystem>

#include <iostream>

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

MainWindow::MainWindow()
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    createMenuBar();

    mMsgbox = new QMessageBox();

    mMainWidget = new QWidget();
    mMainGridLayout = new QGridLayout(mMainWidget);

    int level = 0;
    level = addLocationAddToGridLayout(level);
    level = addConfigurationToGridLayout(level);
    level = addStatusToGridLayout(level);

    setCentralWidget(mMainWidget);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenuBar()
{
    mFileMenu = menuBar()->addMenu(tr("&File"));
    mFileMenu->addAction(FILE_CONFIGURATION_NAME);
    mFileMenu->addAction(FILE_LOAD_FROM_FILE_NAME);
    mFileMenu->addAction(FILE_EXIT_NAME);

    menuBar()->addSeparator();

    mDiagnosticsMenu = menuBar()->addMenu(tr("&Diagnostics"));
    mDiagnosticsMenu->addAction(DIAG_VERIFY_DB_NAME);
    mDiagnosticsMenu->addAction(DIAG_GET_FAIL_LIST_NAME);
    mDiagnosticsMenu->addAction(DIAG_REQ_FROM_STARTUP_LIST_NAME);

    connect(mFileMenu, &QMenu::triggered, this, &MainWindow::handleMenu);
    connect(mDiagnosticsMenu, &QMenu::triggered, this, &MainWindow::handleMenu);
}

int MainWindow::addLocationAddToGridLayout(int level)
{
    mGetLocationLabel = new QLabel("IP address or url: ");
    mGetLocationLineEdit = new QLineEdit("");
    mGetLocationLineEdit->setMinimumWidth(300);
    mGetLocationPushButton = new QPushButton("Get location");
    mRemoveLocationPushButton = new QPushButton("Remove location");

    mMainGridLayout->addWidget(mGetLocationLabel, level, 0, 2, 1);
    mMainGridLayout->addWidget(mGetLocationLineEdit, level, 1, 2, 1);
    mMainGridLayout->addWidget(mGetLocationPushButton, level, 2, 1, 1);
    mMainGridLayout->addWidget(mRemoveLocationPushButton, ++level, 2, 1, 1);

    connect(mGetLocationPushButton, &QPushButton::released, this, &MainWindow::handleGetLocationButton);
    connect(mRemoveLocationPushButton, &QPushButton::released, this, &MainWindow::handleRemoveLocationButton);

    return ++level;
}

int MainWindow::addConfigurationToGridLayout(int level)
{
    mGetLocationWithForceCheckBox = new QCheckBox("Force update");
    mGetLocationWithForceCheckBox->setChecked(false);
    mGetLocationOnlyDbCheckBox = new QCheckBox("Search only in DB");
    mGetLocationOnlyDbCheckBox->setChecked(false);

    mMainGridLayout->addWidget(mGetLocationWithForceCheckBox, level, 0, 1, GRID_WIDTH);
    ++level;
    mMainGridLayout->addWidget(mGetLocationOnlyDbCheckBox, level, 0, 1, GRID_WIDTH);

    return ++level;
}

int MainWindow::addStatusToGridLayout(int level)
{
    mLineSeparator = new QFrame();
    mLineSeparator->setFrameShape(QFrame::HLine);
    mLineSeparator->setFrameShadow(QFrame::Sunken);

    mMainGridLayout->addWidget(mLineSeparator, level, 0, 1, GRID_WIDTH);
    ++level;

    for (auto& locationPlaceholder : mLocationResults)
    {
        locationPlaceholder = new QLabel("");
        locationPlaceholder->setTextInteractionFlags(Qt::TextSelectableByMouse);
        mMainGridLayout->addWidget(locationPlaceholder, level++, 0, 1, GRID_WIDTH);
    }

    return level;
}

void MainWindow::handleGetLocationButton()
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

    if (!errorStrings.empty())
    {
        showDialog("Error list:\n" + errorStrings.join('\n'));
    }
    else
    {
        emit requestToGetLocation(urlOrIpToAdd, locationAddConfig);
    }
}

void MainWindow::handleRemoveLocationButton()
{
    QStringList errorStrings;
    const auto urlOrIpToAdd = mGetLocationLineEdit->text();
    if (urlOrIpToAdd.isEmpty())
    {
        errorStrings.emplace_back("Provide first url or ip address that you want to remove from DB");
    }

    if (!errorStrings.empty())
    {
        showDialog("Error list:\n" + errorStrings.join('\n'));
    }
    else
    {
        emit requestToRemoveLocation(urlOrIpToAdd);
    }
}

void MainWindow::handleSaveConfigurationButton()
{
    auto databaseType = DatabaseConfig::DatabaseType::SQL_LITE;
    auto databasePath = mDatabaseLocationLineEdit->text().toStdString();
    auto tableName = mTableNameLineEdit->text().toStdString();
    DatabaseConfig databaseConfig(databaseType, databasePath, tableName);

    auto apiKey = mApiKeyLineEdit->text().toStdString();

    emit saveConfiguration(AppConfigurationData{databaseConfig, apiKey});
}

void MainWindow::handleLoadMultipleButton()
{
    const auto loadMultipleFileName = mLoadMultipleFileLineEdit->text();
    if (loadMultipleFileName.isEmpty())
    {
        showDialog("Empty file name. Provide correct filename");
        return;
    }

    std::filesystem::path inputFilePath(loadMultipleFileName.toStdString());
    if (!std::filesystem::exists(inputFilePath))
    {
        showDialog("File: " + QString::fromStdString(inputFilePath.string()) + " does not exist. Provide correct filename");
        return;
    }

    LocationAddConfig locationAddConfig;
    locationAddConfig.forceUpdate = false;
    locationAddConfig.searchOnlyDb = false;

    std::ifstream inputFile(inputFilePath.string());
    std::string line;
    while (inputFile >> line)
    {
        emit requestToGetLocation(QString::fromStdString(line), locationAddConfig);
    }
}

void MainWindow::onNewLocation(IpLocationData ipLocationData)
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

void MainWindow::handleMenu(QAction* action)
{
    const auto actionName = action->text();
    if (actionName == FILE_CONFIGURATION_NAME)
    {
        openConfigurationMenu();
    }
    else if (actionName == FILE_LOAD_FROM_FILE_NAME)
    {
        openLoadMultipleMenu();
    }
    else
    {
        showDialog("Functionality not implemented yet");
    }
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

void MainWindow::openConfigurationMenu()
{
    mConfigurationWindow = new QWidget();
    mConfigurationWindowLayout = new QGridLayout(mConfigurationWindow);

    mApiLabel = new QLabel("API key: ");
    mApiKeyLineEdit = new QLineEdit("");
    mApiKeyLineEdit->setMinimumWidth(300);

    int level = 0;
    mConfigurationWindowLayout->addWidget(mApiLabel, level, 0);
    mConfigurationWindowLayout->addWidget(mApiKeyLineEdit, level, 1);

    ++level;
    mDatabaseLocationLabel = new QLabel("Database path: ");
    mDatabaseLocationLineEdit = new QLineEdit("");
    mDatabaseLocationLineEdit->setMinimumWidth(300);

    mConfigurationWindowLayout->addWidget(mDatabaseLocationLabel, level, 0);
    mConfigurationWindowLayout->addWidget(mDatabaseLocationLineEdit, level, 1);

    ++level;
    mTableNameLabel = new QLabel("Table name: ");
    mTableNameLineEdit = new QLineEdit("");
    mTableNameLineEdit->setMinimumWidth(300);

    mConfigurationWindowLayout->addWidget(mTableNameLabel, level, 0);
    mConfigurationWindowLayout->addWidget(mTableNameLineEdit, level, 1);

    ++level;
    mSaveConfigurationPushButton = new QPushButton("Save config");
    mConfigurationWindowLayout->addWidget(mSaveConfigurationPushButton, level, 0);

    mConfigurationWindow->show();

    connect(mSaveConfigurationPushButton, &QPushButton::released, this, &MainWindow::handleSaveConfigurationButton);
}

void MainWindow::openLoadMultipleMenu()
{
    mLoadMultipleWindow = new QWidget();
    mLoadMultipleWindowLayout = new QGridLayout(mLoadMultipleWindow);

    mLoadMultipleLabel = new QLabel("Path to input file: ");
    mLoadMultipleFileLineEdit = new QLineEdit("");
    mLoadMultipleFileLineEdit->setMinimumWidth(300);

    int level = 0;
    mLoadMultipleWindowLayout->addWidget(mLoadMultipleLabel, level, 0);
    mLoadMultipleWindowLayout->addWidget(mLoadMultipleFileLineEdit, level, 1);

    ++level;
    mLoadMultiplePushButton = new QPushButton("Load");
    mLoadMultipleWindowLayout->addWidget(mLoadMultiplePushButton, level, 0);

    mLoadMultipleWindow->show();

    connect(mLoadMultiplePushButton, &QPushButton::released, this, &MainWindow::handleLoadMultipleButton);
}

void MainWindow::showDialog(QString text, DialogLevel dialogLevel)
{
    mMsgbox->close();

    QString windowTitle = "Unknown";
    if (dialogLevel == DialogLevel::WARNING)
    {
        windowTitle = "Warning";
    }
    else if (dialogLevel == DialogLevel::INFO)
    {
        windowTitle = "Info";
    }

    mMsgbox->setWindowTitle(windowTitle);
    mMsgbox->setText(text);
    mMsgbox->open();
}
