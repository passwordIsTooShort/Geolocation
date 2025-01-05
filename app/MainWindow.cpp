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

MainWindow::MainWindow()
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
}

QMenuBar* MainWindow::createMenuBar()
{
    auto fileMenu = new QMenu("File");
    fileMenu->addAction(FILE_CONFIGURATION_NAME);
    fileMenu->addAction(FILE_LOAD_FROM_FILE_NAME);
    fileMenu->addAction(FILE_EXIT_NAME);

    auto diagnosticsMenu = new QMenu("Diagnostics");
    diagnosticsMenu->addAction(DIAG_VERIFY_DB_NAME);
    diagnosticsMenu->addAction(DIAG_GET_FAIL_LIST_NAME);
    diagnosticsMenu->addAction(DIAG_REQ_FROM_STARTUP_LIST_NAME);

    auto menuBar = new QMenuBar();
    menuBar->addMenu(fileMenu);
    menuBar->addSeparator();
    menuBar->addMenu(diagnosticsMenu);

    connect(fileMenu, &QMenu::triggered, this, &MainWindow::handleMenu);
    connect(diagnosticsMenu, &QMenu::triggered, this, &MainWindow::handleMenu);

    return menuBar;
}

int MainWindow::addLocationAddToGridLayout(QGridLayout* gridLayout, int level)
{
    auto findLabel = new QLabel("IP address or url: ");
    mGetLocationLineEdit = new QLineEdit("");
    mGetLocationLineEdit->setMinimumWidth(300);
    mGetLocationPushButton = new QPushButton("Get location");

    gridLayout->addWidget(findLabel, level, 0);
    gridLayout->addWidget(mGetLocationLineEdit, level, 1);
    gridLayout->addWidget(mGetLocationPushButton, level, 2);

    connect(mGetLocationPushButton, &QPushButton::released, this, &MainWindow::handleGetLocationButton);

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
        showWarning("Error list:\n" + errorStrings.join('\n'));
    }
    else
    {
        emit requestToGetLocation(urlOrIpToAdd, locationAddConfig);
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
    else
    {
        showWarning("Functionality not implemented yet");
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
    auto configurationWindow = new QWidget();
    auto configurationWindowLayout = new QGridLayout(configurationWindow);

    auto apiLabel = new QLabel("API key: ");
    mApiKeyLineEdit = new QLineEdit("");
    mApiKeyLineEdit->setMinimumWidth(300);

    int level = 0;
    configurationWindowLayout->addWidget(apiLabel, level, 0);
    configurationWindowLayout->addWidget(mApiKeyLineEdit, level, 1);

    ++level;
    auto databaseLocationLabel = new QLabel("Database path: ");
    mDatabaseLocationLineEdit = new QLineEdit("");
    mDatabaseLocationLineEdit->setMinimumWidth(300);

    configurationWindowLayout->addWidget(databaseLocationLabel, level, 0);
    configurationWindowLayout->addWidget(mDatabaseLocationLineEdit, level, 1);

    ++level;
    auto tableNameLabel = new QLabel("Table name: ");
    mTableNameLineEdit = new QLineEdit("");
    mTableNameLineEdit->setMinimumWidth(300);

    configurationWindowLayout->addWidget(tableNameLabel, level, 0);
    configurationWindowLayout->addWidget(mTableNameLineEdit, level, 1);

    ++level;
    mSaveConfigurationPushButton = new QPushButton("Save config");
    configurationWindowLayout->addWidget(mSaveConfigurationPushButton, level, 0);

    configurationWindow->show();

    connect(mSaveConfigurationPushButton, &QPushButton::released, this, &MainWindow::handleSaveConfigurationButton);
}

void MainWindow::showWarning(QString warning)
{
    QMessageBox* msgbox = new QMessageBox();
    msgbox->setWindowTitle("Warning");
    msgbox->setText(warning);
    msgbox->open();
}
